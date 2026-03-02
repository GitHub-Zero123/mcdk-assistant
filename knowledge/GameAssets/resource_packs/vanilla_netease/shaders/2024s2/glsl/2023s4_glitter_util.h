precision highp float;

vec2 MFTriplanarUV(float tiling){
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

vec4 vip_glitter(){
	// 根据uv获取纹理色 如果是用了NO_TEXTURE宏则会返回 1.0
	vec4 color = getSampledColor(TEXTURE_0, uv);
	vec2 baseMask = getSampledColor(TEXTURE_1, uv).rg;

	// 原材质有一个基于相机位置的噪声 但是看起来对最终效果影响不大 就没有加入
	vec2 triplanarUV = MFTriplanarUV(1.0 / GLITTER_SIZE);
	float glitterBlend = getSampledColor(TEXTURE_3, triplanarUV).r;
	if (TWICE_MORE_SPARKLE >= 0.5){
		glitterBlend += getSampledColor(TEXTURE_3, fract(triplanarUV + 0.5)).r;
	}
	vec3 glitterSource = glitterColor * glitterBlend * GLITTER_INTENSITY;

	/******* calculate glitter alpha *******/
	vec2 maskUV0 = fract(uv * MASK_UV_SCALE0 + TIME * MASK_UV_FLOW0);
	vec2 maskUV1 = fract(uv * MASK_UV_SCALE1 + TIME * MASK_UV_FLOW1);
	float maskAlpha0 = texture2D(TEXTURE_1, maskUV0).b;
	float maskAlpha1 = texture2D(TEXTURE_1, maskUV1).b;
	float maskAlpha = maskAlpha0 * maskAlpha1;
	maskAlpha = mix(0.0, maskAlpha, baseMask.r);
	maskAlpha += pow(baseMask.r, 8.0);
	float glitterAlpha = mix(0.0, glitterSource.r, maskAlpha);

	/******* calculate inside noise color *******/
	vec2 insideNoiseUV = cross(normalize(normalVS), normalize(positionVS)).rg * 0.5 + 0.5;
	insideNoiseUV = fract(insideNoiseUV * INSIDE_NOISE_UV_SCALE + TIME * INSIDE_NOISE_UV_FLOW);

	vec3 insideColor = getSampledColor(TEXTURE_2, insideNoiseUV).rgb;
	insideColor = mix(insideColor, glitterColor, glitterAlpha); // mix with glitter

	/******* calculate light color *******/
	color.rgb = color.rgb * mix(light.rgb, vec3(1.0), EMISSIVE_BASECOLOR_ENABLED);
	insideColor.rgb = insideColor.rgb * mix(light.rgb, vec3(1.0), EMISSIVE_SHADER_ENABLED);

	/******* final mixture *******/
	color.rgb = mix(color.rgb, insideColor, baseMask.g);
	color.a = IS_USE_ALPHA * glitterAlpha + (1.0 - IS_USE_ALPHA) * 1.0;
	color.a = (1.0 - baseMask.g) * 1.0 + baseMask.g * color.a;
	return color;
}
