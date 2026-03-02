#ifndef __LOCAL_LIGHT_UTIL_H__
#define __LOCAL_LIGHT_UTIL_H__
#include "uniformPerFrameConstants.h"

vec3 CalcPointLighting(in vec3 P, in vec3 N, in vec4 lightPosRange, in vec4 lightColorAtten)
{
	vec3 L = lightPosRange.rgb - P;
	float D2 = dot(L, L) + 0.001;
	L *= inversesqrt(D2);
	float NoL = clamp( dot(N, L), 0.0, 1.0);
	float Atten = clamp(D2 / (lightPosRange.w * lightPosRange.w), 0.0, 1.0);
	Atten = 1.0 - Atten * Atten;
	Atten *= Atten;
	Atten *= 1.0 / (D2 * lightColorAtten.w + 1.0);
	vec3 color = lightColorAtten.rgb * (NoL * Atten);
	return color;
}

vec3 CalcSpotLighting(in vec3 P, in vec3 N, in vec4 lightPosRange, in vec4 lightColorAtten, 
	in vec3 lightDir, in vec3 fallOffCosHalfThetaPHi)
{
	vec3 L = lightPosRange.rgb - P;
	float D2 = dot(L, L) + 0.001;
	L *= inversesqrt(D2);
	float NoL = clamp( dot(N, L), 0.0, 1.0);
	float Atten = clamp(D2 / (lightPosRange.w * lightPosRange.w), 0.0, 1.0);
	Atten = 1.0 - Atten * Atten;
	Atten *= Atten;
	Atten *= 1.0 / (D2 * lightColorAtten.w + 1.0);
	float DoL = dot( lightDir, -L );
	float Spot = clamp(DoL * fallOffCosHalfThetaPHi.y + fallOffCosHalfThetaPHi.z, 0.0, 1.0);
	Spot *= Spot;
	vec3 color = lightColorAtten.rgb * (NoL * Atten * Spot);
	return color;
	// DiffLit += color * shadowMapCoefficient;
	// SpecLit += color * GetPbrBRDF(L, N, V, rough2, NoV, NoL, SpecularColor) ;
}

vec3 _LocalLightRadiance(vec4 position, vec4 normal) {
//#if defined(RNGL_UNLIT) || !defined(FANCY)
//	return vec3(0.0);
//#else
	// vec3 pLightColor1 = pointLightColor1.xyz;
	// vec3 pLightPos = pointLightPosRadius1.xyz;
	// float radius = pointLightPosRadius1.w;
	// float dLight = max(0.0, dot(N, normalize(pLightPos - position)));
	// float attenuation = attenuate_cusp(length(position.xyz - pLightPos), radius, 1.0);
	// return (pLightColor1 * dLight + vec3(1.0)) * attenuation;
	//float lightNum = LIGHT_NUM.x;
	vec3 ret = vec3(0);
	//if (lightNum >= 1.0)
#if	defined(NETEASE_LOCAL_LIGHT_NUM_1) || defined(NETEASE_LOCAL_LIGHT_NUM_2) || defined(NETEASE_LOCAL_LIGHT_NUM_3)
	{
		float type = LIGHT_DIR_TYPE_0.w;
		if (type < 1.0)
		{
			ret.xyz += CalcPointLighting(position.xyz, normal.xyz, LIGHT_POS_RANGE_0.xyzw, LIGHT_COLOR_ATTEN_0.xyzw);
		}
		else
		{
			ret.xyz += CalcSpotLighting(position.xyz, normal.xyz, LIGHT_POS_RANGE_0.xyzw, LIGHT_COLOR_ATTEN_0.xyzw,
				LIGHT_DIR_TYPE_0.xyz, LIGHT_FALLOFF_COS_0.xyz);
		}
	}
#endif
	//else if (lightNum >= 2.0)
#if	defined(NETEASE_LOCAL_LIGHT_NUM_2) || defined(NETEASE_LOCAL_LIGHT_NUM_3)
	{
		float type = LIGHT_DIR_TYPE_1.w;
		if (type < 1.0)
		{
			ret.xyz += CalcPointLighting(position.xyz, normal.xyz, LIGHT_POS_RANGE_1.xyzw, LIGHT_COLOR_ATTEN_1.xyzw);
		}
		else
		{
			ret.xyz += CalcSpotLighting(position.xyz, normal.xyz, LIGHT_POS_RANGE_1.xyzw, LIGHT_COLOR_ATTEN_1.xyzw,
				LIGHT_DIR_TYPE_1.xyz, LIGHT_FALLOFF_COS_1.xyz);
		}
	}
#endif
	//else if (lightNum >= 3.0)
#if	defined(NETEASE_LOCAL_LIGHT_NUM_3)
	{
		float type = LIGHT_DIR_TYPE_2.w;
		if (type < 1.0)
		{
			ret.xyz += CalcPointLighting(position.xyz, normal.xyz, LIGHT_POS_RANGE_2.xyzw, LIGHT_COLOR_ATTEN_2.xyzw);
		}
		else
		{
			ret.xyz += CalcSpotLighting(position.xyz, normal.xyz, LIGHT_POS_RANGE_2.xyzw, LIGHT_COLOR_ATTEN_2.xyzw,
				LIGHT_DIR_TYPE_2.xyz, LIGHT_FALLOFF_COS_2.xyz);
		}
	}
#endif
	return ret;

//#endif
}

vec4 getLightColor(vec4 worldSpacePosition, vec4 worldSpaceNormal) {
	vec3 finalColor = vec3(0, 0, 0);
	vec3 localLightRad = _LocalLightRadiance(worldSpacePosition, worldSpaceNormal);
	finalColor += localLightRad.xyz;
	return vec4(finalColor.xyz, 0);
}
#endif