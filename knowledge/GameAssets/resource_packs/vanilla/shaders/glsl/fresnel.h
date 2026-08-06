const float fresnelPow = 1.68;
const float fresnelBias = 0.0;
const float fresnelScale = 0.98;
#ifdef SWK_SKIN
const vec4 fresnelColor = vec4(0.894117, 0.713725, 0.219607, 0.0);
#else
const vec4 fresnelColor = vec4(1.0, 0.125, 0.125, 0.0);
#endif

float fresnel(vec3 normal, vec3 viewDir){

    float NdotV = dot( normal, viewDir );

	return fresnelBias + fresnelScale * pow( 1.0 - NdotV, fresnelPow );
}

// more customized fresnel
float fresnel_ex(vec3 normal, vec3 viewDir, float bias, float scale, float powValue){
	float result = bias + scale * pow(1.0 - dot(normal, viewDir), powValue);
    return clamp(result, 0.0, 1.0);
}
