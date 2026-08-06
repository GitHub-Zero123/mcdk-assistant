#ifndef NETEASE_FRAGMENT_COMMON_H
#define NETEASE_FRAGMENT_COMMON_H

#include "fragmentVersionCentroidUV.h"
#include "uniformEntityConstants.h"
#include "uniformShaderConstants.h"
#include "util.h"
#include "local_light_util.h"
#line 9

#ifdef USE_EMISSIVE
	#ifdef USE_ONLY_EMISSIVE
		#define NEEDS_DISCARD(C) (C.a == 0.0 || C.a == 1.0 )
	#else
		#define NEEDS_DISCARD(C) (C.a + C.r + C.g + C.b == 0.0)
	#endif
#else
	#ifndef USE_COLOR_MASK
		#define NEEDS_DISCARD(C) (C.a < 0.5)
	#else
		#define NEEDS_DISCARD(C) (C.a == 0.0)
	#endif
#endif

/**************** Preset Variables Begin ****************/
#if defined(USE_ALPHA) || defined(USE_BRIGHT)
uniform vec4 HIDE_COLOR_FRAGMENT;
#endif

#ifdef PET_DISTANCE_ALPHA
	varying float cameraDistance;
#endif

#ifdef VIP_MOUNT_ALPHA
	varying float cameraDistToMount;
#endif

#ifdef GLINT
	varying vec2 layer1UV;
	varying vec2 layer2UV;
	varying vec4 glintColor;
	varying vec4 tileLightColor;
#endif

#ifdef USE_OVERLAY
	// When drawing horses on specific android devices, overlay color ends up being garbage data.
	// Changing overlay color to high precision appears to fix the issue on devices tested
	varying highp vec4 overlayColor;
#endif
/**************** Preset Variables End ****************/


vec4 glintBlend(vec4 dest, vec4 source) {
	// glBlendFuncSeparate(GL_SRC_COLOR, GL_ONE, GL_ONE, GL_ZERO)
	return vec4(source.rgb * source.rgb, abs(source.a)) + vec4(dest.rgb, 0.0);
}

vec4 getSampledColor(sampler2D texToSample, vec2 uv) {
	vec4 color;
#if !defined(TEXEL_AA) || !defined(TEXEL_AA_FEATURE)
	color = texture2D( texToSample, uv );
#else
	color = texture2D_AA(texToSample, uv);
#endif
	return color;
}

void applyOverlayColor(inout vec4 color) {
#ifdef USE_OVERLAY
	//use either the diffuse or the OVERLAY_COLOR
	color.rgb = mix(color, overlayColor, overlayColor.a).rgb;
#endif
}

void applyGlint(sampler2D glintTexture, inout vec4 color) {
#ifdef GLINT
	// Applies color mask to glint texture instead and blends with original color
	vec4 layer1 = texture2D(glintTexture, fract(layer1UV)).rgbr * glintColor;
	vec4 layer2 = texture2D(glintTexture, fract(layer2UV)).rgbr * glintColor;
	vec4 glint = (layer1 + layer2) * tileLightColor;

	color = glintBlend(color, glint);
#endif
}

vec3 getReflectColor(sampler2D reflectTex, vec4 clipPos, vec3 worldNormal, vec3 viewDir, float reflectIntensity) {
#ifdef RNGL_REFLECT
	vec2 reflectUV = clipPos.xy / clipPos.w;
	reflectUV.xy = reflectUV.xy * 0.5 + vec2(0.5, 0.5);
	reflectUV.y = 1.0 - reflectUV.y;
	vec3 reflectColor = texture2D(reflectTex, reflectUV.xy).xyz;//screen space uv.
	float NoV = dot(normalize(viewDir), normalize(worldNormal.xyz));
	float BRDFApprox2NV = exp2( -9.28 * NoV ) ;
	reflectColor = reflectColor.rgb * BRDFApprox2NV * reflectIntensity;
	return reflectColor;
#else
	return vec3(0.0, 0.0, 0.0);
#endif
}

float getBloomMask(sampler2D bloomTexture, vec2 uv) {
	float bloomMask = 1.0;
// sample bloom mask value
#if !defined(TEXEL_AA) || !defined(TEXEL_AA_FEATURE)
	bloomMask = texture2D(bloomTexture, uv).r;
#else
	bloomMask = texture2D_AA(bloomTexture, uv).r;
#endif // !defined(TEXEL_AA) || !defined(TEXEL_AA_FEATURE)
	return bloomMask;
}

float getEmissive(sampler2D emissiveTex, vec2 uv) {
#ifdef USE_BLOOM
	return getBloomMask(emissiveTex, uv);
#else
	return 0.0;
#endif
}

void applyFog(inout vec4 color, vec4 fogColor) {
	color.rgb = mix( color.rgb, fogColor.rgb, fogColor.a );
}

void neteaseModelCommonFrag(inout vec4 color) {
#ifdef USE_ALPHA
	color.a *= HIDE_COLOR_FRAGMENT.a;
#endif

#ifdef PET_SKILL_TIPS_PREVIEW
	color.rgb = mix(color.rgb, vec3(0, 0, 1), 0.6);
	color.a = 0.4;
#endif

#ifdef USE_BRIGHT
	color.rgb *= HIDE_COLOR_FRAGMENT.a;
#endif

// PET_DISTANCE_ALPHA and VIP_MOUNT_ALPHA shouldn't work simultaneously
#ifdef PET_DISTANCE_ALPHA
	color.a = 0.5 + step(2.0, cameraDistance) * 0.5;
#endif

#ifdef VIP_MOUNT_ALPHA
	color.a = 0.25 + step(2.5, cameraDistToMount) * 0.75;
#endif

#if defined(ALPHA_TEST)
	if(NEEDS_DISCARD(color))
		discard;
#endif
}

#endif // NETEASE_FRAGMENT_COMMON_H