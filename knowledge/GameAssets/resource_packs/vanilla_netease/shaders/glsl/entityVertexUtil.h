#ifndef ENTITY_VERTEX_UTIL_H
#define ENTITY_VERTEX_UTIL_H

#include "vertexVersionCentroidUV.h"
#include "uniformWorldConstants.h"
#include "uniformEntityConstants.h"
#include "uniformPerFrameConstants.h"
#include "uniformExtraVectorConstants.h"
#ifdef USE_SKINNING
#include "uniformAnimationConstants.h"
#endif
#line 12

/**************** Preset Variables Begin ****************/
attribute mediump vec4 POSITION;
attribute vec2 TEXCOORD_0;
attribute vec4 NORMAL;
#if defined(USE_SKINNING)
	#ifdef MCPE_PLATFORM_NX
		attribute uint BONEID_0;
	#else
		attribute float BONEID_0;
	#endif
#endif

#ifdef RNGL_LIGHT
uniform vec4 HIDE_COLOR;
#endif

#ifdef COLOR_BASED
	attribute vec4 COLOR;
	varying vec4 vertColor;
#endif

#ifdef USE_OVERLAY
	// When drawing horses on specific android devices, overlay color ends up being garbage data.
	// Changing overlay color to high precision appears to fix the issue on devices tested
	varying highp vec4 overlayColor;
#endif

#ifdef TINTED_ALPHA_TEST
	varying float alphaTestMultiplier;
#endif

#ifdef NETEASE_METALLIC
varying vec3 worldNormalM;
varying vec3 worldViewDirM;
varying vec3 worldLightDirM;
#endif

#ifdef USE_UV_FRAME_ANIM
uniform vec4 UV_FRAME_ANIM_PARAM;	//(gridRowInverse, gridColInverse, gridCol, curFrame)

vec2 calculateFrameAnimUV(vec2 uv, float gridRowInverse, float gridColInverse, float gridCol, float curFrame) {
	float curRow = floor(curFrame * gridColInverse + 0.1);
	float curCol = curFrame - curRow * gridCol;	
	return (uv + vec2(curCol, curRow)) * vec2(gridColInverse, gridRowInverse);
}
#endif

#ifdef GLINT
varying vec2 layer1UV;
varying vec2 layer2UV;
varying vec4 tileLightColor;
varying vec4 glintColor;
#endif
/**************** Preset Variables End ****************/

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

const float AMBIENT = 0.45;
const float XFAC = -0.1;
const float ZFAC = 0.1;
float _lightIntensity(vec4 position, vec4 normal) {
#if defined(RNGL_UNLIT) || !defined(FANCY)
	return 1.0;
#else
	vec3 N = normalize( (WORLD * normal).xyz );
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
		return dLight * (1.0-AMBIENT-0.2) + AMBIENT+0.2;  // origin range [0.65, 1.0]
	#else
		float yLight = (1.0+N.y) * 0.5;
		return yLight * (1.0-AMBIENT) + N.x*N.x * XFAC + N.z*N.z * ZFAC + AMBIENT;
	#endif // RNGL_LIGHT
#endif
}


vec4 getLightColor(vec4 entitySpacePosition, vec4 entitySpaceNormal) {
	float L = _lightIntensity(entitySpacePosition, entitySpaceNormal);

#ifdef USE_OVERLAY
	L += OVERLAY_COLOR.a * 0.35;
#endif

	return vec4(TILE_LIGHT_COLOR.xyz * L, 1.0);
}

void getEntitySpacePositionAndNormal(out POS4 entitySpacePosition, out POS4 entitySpaceNormal) {
#ifdef USE_SKINNING
	#ifdef NETEASE_SKINNING
		MAT4 boneMat = GetBoneMatForNetease(int(BONEID_0));
		entitySpacePosition = boneMat * POSITION;
		entitySpaceNormal = boneMat * NORMAL;
	#else
		#if defined(LARGE_VERTEX_SHADER_UNIFORMS)
			entitySpacePosition = BONES[int(BONEID_0)] * POSITION;
			entitySpaceNormal = BONES[int(BONEID_0)] * NORMAL;
		#else
			entitySpacePosition = BONE * POSITION;
			entitySpaceNormal = BONE * NORMAL;
		#endif
	#endif
#else
	entitySpacePosition = POSITION;
	entitySpaceNormal = NORMAL;
#endif
}

void applyUVAnim(inout vec2 uv) {
#ifdef USE_UV_ANIM
	uv = UV_ANIM.xy + (uv * UV_ANIM.zw);
#endif

#ifdef USE_UV_FRAME_ANIM
	uv = calculateFrameAnimUV(uv, UV_FRAME_ANIM_PARAM.x, UV_FRAME_ANIM_PARAM.y, UV_FRAME_ANIM_PARAM.z, UV_FRAME_ANIM_PARAM.w);	
#endif
}

void calculateGlint(){
#ifdef GLINT
	glintColor = GLINT_COLOR;
	layer1UV = _calculateLayerUV(UV_OFFSET.x, UV_ROTATION.x);
	layer2UV = _calculateLayerUV(UV_OFFSET.y, UV_ROTATION.y);
	tileLightColor = TILE_LIGHT_COLOR;
#endif
}

void calculateOverlayColor(){
#ifdef USE_OVERLAY
	overlayColor = OVERLAY_COLOR;
#endif
}

vec4 getFogColor(vec4 pos){
	float fogControl = ((pos.z / RENDER_DISTANCE) - FOG_CONTROL.x) / (FOG_CONTROL.y - FOG_CONTROL.x);
	return vec4(
		FOG_COLOR.rgb,
		clamp(fogControl, 0.0, 1.0)
	);
}

void entityCommonVert() {
#ifdef TINTED_ALPHA_TEST
	alphaTestMultiplier = OVERLAY_COLOR.a;
#endif

#ifdef COLOR_BASED
	vertColor = COLOR;
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

void calculateSpecificProcess(inout POS4 entitySpacePosition, in POS4 entitySpaceNormal) {
#ifdef NETEASE_DRAWLINE
	// 放大轮廓
	entitySpacePosition = entitySpacePosition + normalize(entitySpaceNormal) * OVERLAY_COLOR.a;
#endif
#ifdef NETEASE_METALLIC
	worldNormalM = (WORLD * entitySpaceNormal).xyz;
	worldViewDirM = VIEW_POS - (WORLD * entitySpacePosition).xyz;
	worldLightDirM = SUN_DIR.xyz; 
#endif
} 

#endif // ENTITY_VERTEX_UTIL_H
