precision highp float;

vec3 CustomReflectionVector(vec3 vec1, vec3 cameraVector) {
    vec3 vec2 = normalize(vec1);
    vec3 result = (dot(cameraVector, vec2) * vec2 * 2.0) - cameraVector;
    return result;
}

vec2 MFTriplanarUV(float tiling) {
    /*
	* find the max dimension and convert it into vector mask
	*/
    float _step = step(absNormalWS.r, absNormalWS.g);
    vec3 dimensionMask = _step * vec3(0.0, 1.0, 0.0) + (1.0 - _step) * vec3(1.0, 0.0, 0.0);

    float maxDim = max(absNormalWS.r, absNormalWS.g);
    _step = step(maxDim, absNormalWS.b);
    dimensionMask = (1.0 - _step) * dimensionMask + _step * vec3(0.0, 0.0, 1.0);

    vec3 cameraPos = positionWS / 200.0;
    vec2 triUV = cameraPos.rg * dimensionMask.b + cameraPos.gb * dimensionMask.r + cameraPos.rb * dimensionMask.g;
    return fract(triUV * tiling);
}

vec3 BlendLighten(vec3 v1, vec3 v2) {
    return vec3(max(v1.x, v2.x), max(v1.y, v2.y), max(v1.z, v2.z));
}

vec4 vip_mirrorholo() {
    mat3 world2tangent = transpose(mat3(tangetWS, cross(tangetWS, worldNormal), worldNormal));

    vec4 baseColor = getSampledColor(TEXTURE_0, uv);
    vec4 mergeTexture = getSampledColor(TEXTURE_1, uv);
    float mergeTextureAlpha = mergeTexture.a;

    // Bump offset
    vec3 cameraVector = (VIEW_POS - positionWS);
    vec3 customReflectionVector = CustomReflectionVector(vec3(0.0, 0.0, 1.0), world2tangent * cameraVector);
    vec2 bumpOffsetUV = vec2(0.0, 0.0);
    if (USE_TRIP_MAP_HOLO > 0.0000) {
        bumpOffsetUV = MFTriplanarUV(HOLO_PATTERN_TILE);
    } else {
        bumpOffsetUV = uv * HOLO_PATTERN_TILE;
    }
    vec2 bumpOffset = (bumpOffsetUV + ((customReflectionVector.rg * (HOLO_PATTERN_DEPTH / abs(customReflectionVector.b))) / 1024));

    // Holo source shape
    float mergeTextureR = getSampledColor(TEXTURE_1, bumpOffset).r;
    vec4 homogeneousViewSpacePosition = vec4(0.0, 0.0, 1.0, 1.0);
    mat4 inverseViewMatrix = inverse(WORLDVIEW);
    vec4 cameraDirectionVec = inverseViewMatrix * homogeneousViewSpacePosition;
    float holoA = ((dot(cameraVector, worldNormal) + (dot(cameraDirectionVec.xyz, worldNormal) * 0.55)) * 1.0 * HOLO_COLOR_CHANGE_MULTIPLIER) + (HOLO_PATTERN_INFLUENCE * mergeTextureR);
    vec3 reflectionVec = (reflect(cameraVector, worldNormal));
    float holoB = dot(vec4(0.12,0.262,1.0,1.0).rgb, reflectionVec);
    float holoSourceMap = mix(holoA, holoB, IRIDESCENT_SURFACE_NORMAL_INFLUENCE);

    // Holo grad
    vec4 holoGradient = getSampledColor(TEXTURE_2, vec2(holoSourceMap, holoSourceMap));
    float holoPatternR = getSampledColor(TEXTURE_1, bumpOffset).r;
    vec3 holoGrad = mix(mix(holoGradient.rgb, vec4(0.26,0.26,0.26,0).rgb, HOLO_BG_COLOR_AMOUNT), holoGradient.rgb, holoPatternR);
    float bgMask = 1.0 - holoPatternR;

    // Texture1
    vec2 textureUV1 = uv / TEXTURE_UV_SCALE1;
    vec2 textureCharacterUV1 = vec2(textureUV1.x + TEXTURE_U_OFFSET1, textureUV1.y + TEXTURE_V_OFFSET1);
    vec4 textureCharacter1 = getSampledColor(TEXTURE_3, textureCharacterUV1);
    float holoMask = textureCharacter1.a * mergeTextureAlpha;
    vec3 texture1 = mix(baseColor.rgb, mergeTextureAlpha * textureCharacter1.rgb, holoMask);

    // Texture2
    vec2 textureUV2 = uv / TEXTURE_UV_SCALE2;
    vec2 textureCharacterUV2 = vec2(textureUV2.x + TEXTURE_U_OFFSET2, textureUV2.y + TEXTURE_V_OFFSET2);
    vec4 textureCharacter2 = getSampledColor(TEXTURE_3, textureCharacterUV2);
    float holoMask2 = textureCharacter2.a * mergeTextureAlpha;
    vec3 texture2 = mix(baseColor.rgb, mergeTextureAlpha * textureCharacter2.rgb, holoMask2);

    // Holo Grad1
    vec2 iridescentUV = vec2(0.0, 0.0);
    if (USE_TRIP_MAP_HOLO_NOISE > 0.0000) {
        iridescentUV = MFTriplanarUV(IRIDESCENT_NOISE_TILE);
    } else {
        iridescentUV = IRIDESCENT_NOISE_TILE * uv;
    }
    float rot1 = (IRIDESCENT_ROTATE / 180.0 * 3.1415926);
    float cosRot1 = cos(rot1);
    float sinRot1 = sin(rot1);
    float minusSinRot1 = -1.0 * sinRot1;
    vec2 rotatorCameraVector11 = (world2tangent * normalize(cameraVector)).xy - vec2(0.5, 0.5);
    vec2 rotatorCameraVector12 = vec2(dot(vec2(cosRot1, minusSinRot1), rotatorCameraVector11), dot(vec2(sinRot1, cosRot1), rotatorCameraVector11)) + vec2(0.5, 0.5);
    
    float maskAlpha = pow(abs(fract((getSampledColor(TEXTURE_1, iridescentUV).b * vec3(rotatorCameraVector12, cameraVector.z)).x) * 2.0 - 1.0), IRIDESCENT_RANGE);

    float holoGradLerpA = ((dot(cameraVector, worldNormal) + (dot(cameraDirectionVec.xyz, worldNormal) * 0.6)) * IRIDESCENT_COLOR_CHANGE_MULTIPLIER) + (IRIDESCENT_NOISE_INFLUENCE * maskAlpha);
    float holoGradUV = mix(holoGradLerpA, holoB, IRIDESCENT_SURFACE_NORMAL_INFLUENCE);
    vec3 holoGrad1 = getSampledColor(TEXTURE_2, vec2(holoGradUV, holoGradUV)).rgb * maskAlpha;

    // Total
    float input = dot(cameraVector, worldNormal);
    float outputAlpha1 = clamp(((input - INPUT_LOW) * ((TARGET_HIGH - TARGET_LOW) / (INPUT_HIGH - INPUT_LOW))) + TARGET_LOW, 0.0, 1.0);

    vec3 textureMix = mix(texture1, texture2, outputAlpha1);
    float holoMaskMix = mix(holoMask, holoMask2, outputAlpha1);

    vec3 blendLightenOutput = BlendLighten(textureMix * DIFFUSE_BLEED_INTO_HOLO, holoGrad);
    float alpha = clamp((holoMaskMix + DIFFUSE_HOLO_LERP) + (bgMask * (1.0 - BG_HOLO_INTENSITY)), 0.0, 1.0);
    vec3 lerp2 = mix(blendLightenOutput, textureMix, alpha);

    float rot2 = (SHINE_ROTATE / 180.0 * 3.1415926);
    float cosRot2 = cos(rot2);
    float sinRot2 = sin(rot2);
    float minusSinRot2 = -1.0 * sinRot2;
    vec2 rotatorCameraVector21 = (world2tangent * normalize(cameraVector)).xy - vec2(0.5, 0.5);
    vec2 rotatorCameraVector22 = vec2(dot(vec2(cosRot2, minusSinRot2), rotatorCameraVector21), dot(vec2(sinRot2, cosRot2), rotatorCameraVector21)) + vec2(0.5, 0.5);

    float output = pow(abs(fract(vec3(rotatorCameraVector22, cameraVector.z).x) * 2.0 - 1.0), SHINE_RANGE) * getSampledColor(TEXTURE_1, uv * SHINE_SHAPE_SIZE).g;

    vec3 temp = lerp2 + output;

    vec3 lerpB = vec3(0.0, 0.0, 0.0);
    if (IS_USE_SECOND > 0.0000) {
        lerpB = temp + (holoGrad1 * IRIDESCENT_SECOND_STRENGTH * temp);
    } else {
        lerpB = temp;
    }

    baseColor.rgb = mix(baseColor.rgb, lerpB * 0.8, (1 - baseColor.a));
    return baseColor;
}