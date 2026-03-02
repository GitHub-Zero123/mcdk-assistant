#ifndef ENTITY_FRAGMENT_UTIL_H
#define ENTITY_FRAGMENT_UTIL_H
#include "fragmentVersionCentroidUV.h"
#include "uniformEntityConstants.h"
#include "uniformShaderConstants.h"
#include "uniformPerFrameConstants.h"
#include "local_light_util.h"
#include "util.h"
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

#ifdef TINTED_ALPHA_TEST
	varying float alphaTestMultiplier;
#endif

#ifdef GLINT
	varying vec2 layer1UV;
	varying vec2 layer2UV;
	varying vec4 glintColor;
	varying vec4 tileLightColor;
#endif

#ifdef COLOR_BASED
	varying vec4 vertColor;
#endif

#ifdef USE_OVERLAY
	// When drawing horses on specific android devices, overlay color ends up being garbage data.
	// Changing overlay color to high precision appears to fix the issue on devices tested
	varying highp vec4 overlayColor;
#endif

#ifdef NETEASE_METALLIC
varying vec3 worldNormalM;
varying vec3 worldViewDirM;
varying vec3 worldLightDirM;
#endif
/**************** Preset Variables End ****************/


bool shouldDiscard(vec4 albedo) {
#ifdef NO_DISCARD_EPSILON
	return NEEDS_DISCARD(albedo);
#endif
	highp float epsilon = 0.0001;
	bool result = false;
	vec3 diffuse = albedo.rgb;
	highp float alpha = albedo.a;

#ifdef USE_EMISSIVE
	#ifdef USE_ONLY_EMISSIVE
		// C.a == 0.0f || C.a == 1.0f
		result = alpha < epsilon || alpha > 1.0 - epsilon;
	#else
		// C.a + C.r + C.g + C.b == 0.0
		result = dot(vec4(diffuse, alpha), vec4(1.0, 1.0, 1.0, 1.0)) < epsilon;
	#endif
#else
	#ifdef USE_STRICT_ALPHA_TEST
		// C.a == 0.0
		result = alpha < epsilon;
	#else
		#ifndef USE_COLOR_MASK
			// C.a < 0.5
			result = alpha < 0.5;
		#else
			// C.a == 0.0
			result = alpha == 0.0;
		#endif
	#endif
#endif

	return result;
}

vec4 glintBlend(vec4 dest, vec4 source) {
	// glBlendFuncSeparate(GL_SRC_COLOR, GL_ONE, GL_ONE, GL_ZERO)
	return vec4(source.rgb * source.rgb, abs(source.a)) + vec4(dest.rgb, 0.0);
}

vec4 getSampledColor(sampler2D texToSample, vec2 uv) {
	vec4 color = vec4(1.0);

#ifndef NO_TEXTURE
	#if !defined(TEXEL_AA) || !defined(TEXEL_AA_FEATURE)
		color = texture2D(texToSample, uv );
	#else
		color = texture2D_AA(texToSample, uv);
	#endif
#endif // NO_TEXTURE

	return color;
}

void applyOverlayColor(inout vec4 color) {
#ifdef USE_OVERLAY
	//use either the diffuse or the OVERLAY_COLOR
	color.rgb = mix(color, overlayColor, overlayColor.a).rgb;
#endif
#ifdef NETEASE_DRAWLINE
	color = vec4(OVERLAY_COLOR.rgb, 1.0);
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

vec3 getReflectColor(sampler2D TEXTURE_1, vec4 clipPos, vec3 worldNormal, vec3 viewDir, float reflectIntensity) {
#ifdef RNGL_REFLECT
	vec2 reflectUV = clipPos.xy / clipPos.w;
	reflectUV.xy = reflectUV.xy * 0.5 + vec2(0.5, 0.5);
	reflectUV.y = 1.0 - reflectUV.y;
	vec3 reflectColor = texture2D(TEXTURE_1, reflectUV.xy).xyz;//screen space uv.
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
#if defined(USE_BLOOM) && !defined(NO_TEXTURE)
	return getBloomMask(emissiveTex, uv);
#else
	return 0.0;
#endif
}

void applyFog(inout vec4 color, vec4 fogColor) {
	color.rgb = mix( color.rgb, fogColor.rgb, fogColor.a );
}

void applySpecificProcess(inout vec4 color) {
#ifdef NETEASE_METALLIC
	const float shininess = 64.0;  
	const float metalness = 1.0;  
	vec3 lightColor = vec3(1.0, 1.0, 1.0);
	highp vec3 N = normalize(worldNormalM);
	highp vec3 V = normalize(worldViewDirM);
	highp vec3 L = normalize(worldLightDirM);
	highp vec3 H = normalize(V + L);
	vec3 baseColor = color.rgb;

	float diff = max(dot(N, L), 0.0);
	vec3 diffuse = diff * lightColor * baseColor;

	// Fresnel
	float F0 = 0.04;
	vec3 F0Metal = mix(vec3(F0), baseColor, metalness);
	float fresnel = pow(1.0 - max(dot(N, V), 0.0), 5.0);
	vec3 fresnelEffect = F0Metal + (1.0 - F0Metal) * fresnel;

	float spec = pow(max(dot(N, H), 0.0), shininess);
	vec3 specular = fresnelEffect * spec * lightColor;

	vec3 ambient = baseColor * 0.6;

	vec3 finalColor = ambient + diffuse + specular;
	color.rgb = finalColor;
#endif
}

#if defined(USE_MULTITEXTURE) || defined(GLINT_BLEND_BLOOM)
void entityCommonFrag(inout vec4 color, sampler2D TEXTURE_0, sampler2D TEXTURE_1, sampler2D TEXTURE_2) {
#else
void entityCommonFrag(inout vec4 color, sampler2D TEXTURE_0, sampler2D TEXTURE_1) {
#endif

#ifndef NO_TEXTURE
	#ifdef MASKED_MULTITEXTURE
		vec4 tex1 = texture2D(TEXTURE_1, uv);
		// If tex1 has a non-black color and no alpha, use color; otherwise use tex1 
		float maskedTexture = float(dot( tex1.rgb, vec3(1.0, 1.0, 1.0) ) * ( 1.0 - tex1.a ) > 0.0);
		color = mix(tex1, color, maskedTexture);
	#endif // MASKED_MULTITEXTURE

	#if defined(ALPHA_TEST) && !defined(USE_MULTITEXTURE) && !defined(MULTIPLICATIVE_TINT)
		if(shouldDiscard(color))
			discard;
	#endif // ALPHA_TEST

	#ifdef TINTED_ALPHA_TEST
		vec4 testColor = color;
		testColor.a *= alphaTestMultiplier;
		if(shouldDiscard(testColor))
			discard;
	#endif // TINTED_ALPHA_TEST
#endif // NO_TEXTURE

#ifdef COLOR_BASED
	color *= vertColor;
#endif

#ifdef MULTI_COLOR_TINT
	// Texture is a mask for tinting with two colors
	vec2 colorMask = color.rg;

	// Apply the base color tint
	color.rgb = colorMask.rrr * CHANGE_COLOR.rgb;

	// Apply the secondary color mask and tint so long as its grayscale value is not 0
	color.rgb = mix(color, colorMask.gggg * MULTIPLICATIVE_TINT_CHANGE_COLOR, ceil(colorMask.g)).rgb;
#else
	#ifdef USE_COLOR_MASK
		color.rgb = mix(color.rgb, color.rgb * CHANGE_COLOR.rgb, color.a);
		color.a *= CHANGE_COLOR.a;
	#endif

	#ifdef ITEM_IN_HAND
		color.rgb = mix(color.rgb, color.rgb * CHANGE_COLOR.rgb, vertColor.a);
		#if defined(MCPE_PLATFORM_NX) && defined(NO_TEXTURE) && defined(GLINT)
			// TODO(adfairfi): This needs to be properly fixed soon. We have a User Story for it in VSO: 102633
			vec3 dummyColor = texture2D(TEXTURE_0, vec2(0.0, 0.0)).rgb;
			color.rgb += dummyColor * 0.000000001;
		#endif
	#endif
#endif // MULTI_COLOR_TINT

#ifdef USE_MULTITEXTURE
	vec4 tex1 = texture2D(TEXTURE_1, uv);
	vec4 tex2 = texture2D(TEXTURE_2, uv);
	color.rgb = mix(color.rgb, tex1.rgb, tex1.a);
	#ifdef ALPHA_TEST
		if (color.a < 0.5 && tex1.a == 0.0) {
			discard;
		}
	#endif

	#ifdef COLOR_SECOND_TEXTURE
		if (tex2.a > 0.0) {
			color.rgb = tex2.rgb + (tex2.rgb * CHANGE_COLOR.rgb - tex2.rgb) * tex2.a;//lerp(tex2.rgb, tex2 * changeColor.rgb, tex2.a)
		}
	#else
		color.rgb = mix(color.rgb, tex2.rgb, tex2.a);
	#endif
#endif // USE_MULTITEXTURE

#if defined(MULTIPLICATIVE_TINT) && defined(ALPHA_TEST)
	vec4 tintTex = texture2D(TEXTURE_1, uv);
	#ifdef MULTIPLICATIVE_TINT_COLOR
		tintTex.rgb = tintTex.rgb * MULTIPLICATIVE_TINT_CHANGE_COLOR.rgb;
	#endif

	color.rgb = mix(color.rgb, tintTex.rgb, tintTex.a);
	if (color.a + tintTex.a <= 0.0) {
		discard;
	}
#endif

#ifdef USE_ALPHA
	color.a *= HIDE_COLOR_FRAGMENT.a;
#endif

#ifdef USE_BRIGHT
	color.rgb *= HIDE_COLOR_FRAGMENT.a;
#endif

#ifdef UI_ENTITY
	color.a *= HUD_OPACITY;
#endif

#ifdef PET_SKILL_PREVIEW
	color.rgb = mix(color.rgb, vec3(0.0, 0.0, 1.0), 0.6);
	color.a = 0.6;
#endif
}

#endif // ENTITY_FRAGMENT_UTIL_H
