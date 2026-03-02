#include "uniformExtraVectorConstants.h"

#if defined(RNGL_SHADOW_SOFT) && __VERSION__ >= 300
#define samplerType sampler2D
#define samplerFunc sampleShadowPercentage

#else  // RNGL_SHADOW only
#define samplerType sampler2D
#define samplerFunc sampleShadowTexture
#endif


#if defined(RNGL_SHADOW_CASTER) && defined(RNGL_SHADOW_TRANSPARENT)
void clipShadowTransparency(float pixelAlpha) {
	const float thresh[4] = float[4](0.2, 0.6, 0.8, 0.4);
	ivec2 modPos = ivec2(gl_FragCoord.xy) % 2;  // compat with PCF 1x1
	if (pixelAlpha < thresh[modPos.x * 2 + modPos.y])
		discard;
}
#endif

POS3 getShadowSpaceCoord(POS3 worldPos) {
	// world space (translated to view space) to shadow space position
	MAT4 biasedTransform = mat4(EXTRA_VECTOR1, EXTRA_VECTOR2, EXTRA_VECTOR3, EXTRA_VECTOR4);
	POS4 shadowCoord = biasedTransform * vec4(worldPos, 1.0);  // BIAS*PROJ*VIEW (SHADOW)
	return shadowCoord.xyz / shadowCoord.w;
}

float sampleShadowTexture(sampler2D shadowMap, highp vec3 coord, float bias) {
	vec2 size = vec2(textureSize(shadowMap, 0));
	vec3 offset = vec3(1.0 / size, 0.0);
	coord.xy -= 0.5 * offset.xy;  // sample 4 texels with GL_NEAREST to simulate PCF 1x1
	vec2 residue = fract(coord.xy * size);  // highp coord for enough precision (#160582)
	vec4 zBlocker = vec4(
		texture2D(shadowMap, coord.xy + offset.zz).r,
		texture2D(shadowMap, coord.xy + offset.xz).r,
		texture2D(shadowMap, coord.xy + offset.zy).r,
		texture2D(shadowMap, coord.xy + offset.xy).r);
	float zReceiver = coord.z;  // in light if (zBlocker >= zReceiver || zBlocker >= 1.0) (no blocker)
	vec4 samples = min(step(zReceiver, zBlocker) + step(1.0, zBlocker), 1.0);  // bvec || not supported on gles
	
	#ifdef RNGL_SHADOW_TRANSPARENT
		return (samples.s + samples.t + samples.p + samples.q) * 0.25;  // % in light
	#else
		return mix(mix(samples.s, samples.t, residue.x), mix(samples.p, samples.q, residue.x), residue.y);
	#endif
}

#if defined(RNGL_SHADOW_SOFT) && __VERSION__ >= 300
float sampleShadowPercentage(sampler2D shadowMap, vec3 coord, float bias) {
	#ifdef RNGL_SHADOW_SLOW
		int numSamples = 23;
		const vec2 poissonDisk[23] = vec2[](
			vec2(-0.15375204, -0.14084946), vec2( 0.02578044,  0.29375481), vec2( 0.21974229, -0.28661491),
			vec2(-0.41065391,  0.07263889), vec2( 0.39340293,  0.25025074), vec2(-0.13259392, -0.49324276),
			vec2(-0.25427194,  0.48958514), vec2( 0.55398148, -0.20231315), vec2(-0.57821812, -0.23868003),
			vec2( 0.27947577,  0.59722349), vec2( 0.20697397, -0.65986563), vec2(-0.62495514,  0.36217428),
			vec2( 0.73427404,  0.16142809), vec2(-0.44870994, -0.63824372), vec2(-0.10378174,  0.80087656),
			vec2( 0.63776135, -0.53750594), vec2(-0.85899279, -0.03552205), vec2( 0.62706717,  0.62401559),
			vec2(-0.04198309, -0.90792311), vec2(-0.59746436,  0.71596198), vec2( 0.94699961, -0.12741746),
			vec2(-0.80281529, -0.55857797), vec2( 0.21948137,  0.97561669));
	#else
		int numSamples = 11;
		const vec2 poissonDisk[11] = vec2[](
			vec2(-0.22232508, -0.20366799), vec2( 0.03727845,  0.42476876),
			vec2( 0.31774683, -0.41444448), vec2(-0.59380457,  0.10503567),
			vec2( 0.56885970,  0.36186198), vec2(-0.19173049, -0.71322785),
			vec2(-0.36767661,  0.70793894), vec2( 0.80105589, -0.29254433),
			vec2(-0.83610201, -0.34513076), vec2( 0.40412130,  0.86358374),
			vec2( 0.29928386, -0.95416412));
	#endif

	vec2 uvShadow = coord.xy;
	float zReceiver = coord.z;
	float lightSize = 0.001 * bias;

	float randAngle = fract(sin(dot(gl_FragCoord.xy, vec2(12.9898, 78.233))) * 43758.5453);
	vec2 sampleJitter1 = vec2(cos(randAngle), -sin(randAngle));
	vec2 sampleJitter2 = vec2(-sampleJitter1.y, sampleJitter1.x);

	float searchRadius = lightSize;
	int numBlockers = 0;

	int numTest = 0;  // try early exit
	for (int i = 1; i < numSamples; i += 2, numTest++) {
		vec2 offset = vec2(dot(poissonDisk[i], sampleJitter1), dot(poissonDisk[i], sampleJitter2));
		float zBlocker = texture2D(shadowMap, uvShadow + offset * searchRadius).r;
		if (zBlocker < zReceiver && zBlocker < 1.0) {
			numBlockers++;
		}
	}
	if (numBlockers == 0 || numBlockers == numTest) {
		#ifdef RNGL_SHADOW_DEBUG
			return 2.0;
		#else
			return 1.0 - step(0.5, float(numBlockers));
		#endif
	}

	for (int i = 0; i < numSamples; i += 2) {
		vec2 offset = vec2(dot(poissonDisk[i], sampleJitter1), dot(poissonDisk[i], sampleJitter2));
		float zBlocker = texture2D(shadowMap, uvShadow + offset * searchRadius).r;
		if (zBlocker < zReceiver && zBlocker < 1.0) {
			numBlockers++;
		}
	}
	return 1.0 - float(numBlockers) / float(numSamples);
}
#endif

vec4 getShadowedLight(samplerType shadowMap, vec4 inLight, vec3 lightDir, float baseBias, POS3 worldPos, vec3 worldNormal) {
	POS3 shadowCoord = getShadowSpaceCoord(worldPos + fract(baseBias) * worldNormal);
	bool inside = shadowCoord.x > 0.0 && shadowCoord.x < 1.0 && shadowCoord.y > 0.0 && shadowCoord.y < 1.0;
	float mask = inside ? samplerFunc(shadowMap, shadowCoord, floor(baseBias)) : 1.0;

	float light = smoothstep(0.0, 0.2, dot(worldNormal, lightDir));
	inLight.rgb = vec3(mix(0.65, 1.0, min(light, mask)));

	#ifdef RNGL_SHADOW_DEBUG
		inLight.rgb = (mask == 2.0) ? vec3(0.0, 10.0, 0.0) : vec3(mask);
	#endif

	return inLight;
}

#undef samplerType
#undef samplerFunc
