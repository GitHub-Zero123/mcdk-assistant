#ifndef NETEASE_VERTEX_COMMON_H
#define NETEASE_VERTEX_COMMON_H

#include "vertexVersionCentroidUV.h"
#include "uniformWorldConstants.h"
#include "uniformEntityConstants.h"
#include "uniformPerFrameConstants.h"
#ifdef USE_SKINNING
#include "uniformAnimationConstants.h"
#endif

#line 12

/**************** Preset Variables Begin ****************/
attribute mediump vec4 POSITION;
attribute vec2 TEXCOORD_0;
attribute vec4 NORMAL;
#if defined(USE_SKINNING)
attribute float BONEID_0;
#endif

#ifdef PET_DISTANCE_ALPHA
	varying float cameraDistance;
#endif

#ifdef VIP_MOUNT_ALPHA
	varying float cameraDistToMount;
#endif

#ifdef USE_OVERLAY
	// When drawing horses on specific android devices, overlay color ends up being garbage data.
	// Changing overlay color to high precision appears to fix the issue on devices tested
	varying highp vec4 overlayColor;
#endif

#ifdef GLINT
	varying vec2 layer1UV;
	varying vec2 layer2UV;
	varying vec4 tileLightColor;
	varying vec4 glintColor;
#endif

#ifdef RNGL_LIGHT
	uniform vec4 HIDE_COLOR;
#endif

#ifdef USE_UV_FRAME_ANIM
	uniform vec4 UV_FRAME_ANIM_PARAM;	//(gridRowInverse, gridColInverse, gridCol, curFrame)
#endif
/**************** Preset Variables End ****************/


/**************** Preset Functions Begin ****************/
vec2 _calculateLayerUV(float offset, float rotation) {
	vec2 uv = TEXCOORD_0;
	uv -= 0.5;
	float rsin = sin(rotation);
	float rcos = cos(rotation);
	uv = mat2(rcos, -rsin, rsin, rcos) * uv;
	uv.x += offset;
	uv += 0.5;

	return uv * GLINT_UV_SCALE;
}

vec2 _calculateFrameAnimUV(vec2 uv, float gridRowInverse, float gridColInverse, float gridCol, float curFrame) {
	float curRow = floor(curFrame * gridColInverse + 0.1);
	float curCol = curFrame - curRow * gridCol;	
	return (uv + vec2(curCol, curRow)) * vec2(gridColInverse, gridRowInverse);
}

float _lightIntensity(vec4 position, vec4 normal) {
#if defined(RNGL_UNLIT) || !defined(FANCY)
	return 1.0;
#endif

	const float AMBIENT = 0.45;
	const float XFAC = -0.1;
	const float ZFAC = 0.1;

	vec3 N = normalize((WORLD * normal).xyz);

	N.y *= TILE_LIGHT_COLOR.w; //TILE_LIGHT_COLOR.w contains the direction of the light

//take care of double sided polygons on materials without culling
#ifdef FLIP_BACKFACES
	vec3 viewDir = normalize((WORLD * position).xyz);
	if( dot(N, viewDir) > 0.0 )
		N *= -1.0;
#endif

#ifdef RNGL_LIGHT
	vec3 lightDir = normalize(HIDE_COLOR.xyz);  // light dir uses world coords
	float dLight = max(0.0, dot(N, lightDir));  // diffuse in directional light
#ifdef LIGHT_IN_PAPERDOLL
	float bias = 0.0;
#else
	float bias = 0.2;
#endif
	return dLight * (1.0 - AMBIENT - bias) + AMBIENT + 0.2;  // origin range [0.65, 1.0]
#else
	float yLight = (1.0+N.y) * 0.5;
	return yLight * (1.0-AMBIENT) + N.x*N.x * XFAC + N.z*N.z * ZFAC + AMBIENT;
#endif // RNGL_LIGHT
}
/**************** Preset Functions End ****************/


void getEntitySpacePositionAndNormal(out POS4 pos, out POS4 normal) {
#ifdef USE_SKINNING
	#ifdef NETEASE_SKINNING
		MAT4 boneMat = GetBoneMatForNetease(int(BONEID_0));
		pos = boneMat * POSITION;
		normal = boneMat * NORMAL;
		#ifdef USE_INSTANCE
			MAT4 instanceMat = GetInstanceWorldMatForNetease();
			pos = instanceMat * pos;
			normal = instanceMat * normal;
		#endif
	#else
		#if defined(LARGE_VERTEX_SHADER_UNIFORMS)
			pos = BONES[int(BONEID_0)] * POSITION;
			normal = BONES[int(BONEID_0)] * NORMAL;
		#else
			pos = BONE * POSITION;
			normal = BONE * NORMAL;
		#endif
	#endif // NETEASE_SKINNING
#else
	pos = POSITION;
	normal = NORMAL * vec4(1.0, 1.0, 1.0, 0.0);
	return;
#endif // USE_SKINNING
}

void applyUVAnim(inout vec2 uv) {
#ifdef USE_UV_FRAME_ANIM
	uv = _calculateFrameAnimUV(uv, UV_FRAME_ANIM_PARAM.x, UV_FRAME_ANIM_PARAM.y, UV_FRAME_ANIM_PARAM.z, UV_FRAME_ANIM_PARAM.w);	
#endif
}

vec4 getLightColor(POS4 entitySpacePosition, POS4 entitySpaceNormal) {
	float intensity = _lightIntensity(entitySpacePosition, entitySpaceNormal);
#ifdef USE_OVERLAY
	intensity += OVERLAY_COLOR.a * 0.35;
#endif
	return vec4(vec3(intensity) * TILE_LIGHT_COLOR.xyz, 1.0);
}

vec4 getFogColor(vec4 projSpacePos) {
	float fogAlpha = ((projSpacePos.z / RENDER_DISTANCE) - FOG_CONTROL.x) / (FOG_CONTROL.y - FOG_CONTROL.x);
	return vec4(FOG_COLOR.rgb, clamp(fogAlpha, 0.0, 1.0));
}

void calculateOverlayColor() {
#ifdef USE_OVERLAY
	overlayColor = OVERLAY_COLOR;
#endif
}

void calculateGlint() {
#ifdef GLINT
	glintColor = GLINT_COLOR;
	layer1UV = _calculateLayerUV(UV_OFFSET.x, UV_ROTATION.x);
	layer2UV = _calculateLayerUV(UV_OFFSET.y, UV_ROTATION.y);
	tileLightColor = TILE_LIGHT_COLOR;
#endif
}

void neteaseModelCommonVert() {
#ifdef PET_DISTANCE_ALPHA
	cameraDistance = length(VIEW_POS.xyz - (WORLD * vec4(0.0, 0.0, 0.0, 1.0)).xyz);
#endif

#ifdef VIP_MOUNT_ALPHA
	const vec3 VIP_MOUNT_OFFSET = vec3(0.0, 1.5, 0.0);
	cameraDistToMount = length(VIEW_POS.xyz - ((WORLD * vec4(0.0, 0.0, 0.0, 1.0)).xyz + VIP_MOUNT_OFFSET));
#endif
}

vec3 getViewDir(POS4 entitySpacePosition) {
#if defined(RNGL_REFLECT)
	vec3 worldPos = (WORLD * entitySpacePosition).xyz;
	return worldPos.xyz - VIEW_POS.xyz;
#else
	return vec3(0.0, 1.0, 0.0);
#endif
}

#endif // NETEASE_VERTEX_COMMON_H
