precision highp float;

float Noise(){
    float TimeX = TIME * VertSpeed;
    float splitAmout = (1.0 + sin(TimeX * 6.0)) * 0.5;
    splitAmout *= 1.0 + sin(TimeX * 16.0) * 0.5;
    splitAmout *= 1.0 + sin(TimeX * 19.0) * 0.5;
    splitAmout *= 1.0 + sin(TimeX * 27.0) * 0.5;
    splitAmout = pow(splitAmout, Amplitude);
    splitAmout *= (0.05 * VertMoveAmount);
    return splitAmout;
}

float randomNoise(float x, float y){
	return fract(sin(dot(vec2(x, y), vec2(12.598987, 78.9233))) * 43758.35453);
}

float strength(){
	float stren = 0.5 + 0.5 * cos(TIME * Frequency);
	return fract(stren);
}

float trunc1(float x, float num_levels){
	return floor(x * num_levels) / num_levels;
}

vec2 trunc2(vec2 x, vec2 num_levels){
	return floor(x * num_levels) / num_levels;
}

float lerp1(float a, float b, float x){
	return x * (b - a) + a;
}

vec4 lerp4(vec4 a, vec4 b, float x){
	return x * (b - a) + a;
}

float blocLineRandom(vec2 uv) {
	float timeMul = TIME * strength();
	float truncTime = trunc1(timeMul, 4.0);
	vec2 temp = trunc2(uv.yy, vec2(8.0, 8.0)) + 100.0 * truncTime;
	float uv_trunc = randomNoise(temp.x, temp.y);
	float uv_randomTrunc = 6.0 * trunc1(timeMul, 24.0 * uv_trunc);
	float teamFloat = uv.y + uv_randomTrunc;
	
	temp = trunc2(vec2(teamFloat, teamFloat), vec2(8.0 *Block_LinesWidth , 8.0 *Block_LinesWidth) );
	float blockLine_random = 0.5 * randomNoise(temp.x, temp.y);
	
	blockLine_random = sign(blockLine_random) * clamp((abs(blockLine_random) - Block_Amount) / 0.4, 0.0, 1.0);
	blockLine_random = lerp1(0.0, blockLine_random, Block_Offset);

	return blockLine_random;
}

float blocLineRandomSmall(vec2 uv) {
	float timeMul = TIME * strength();
	float truncTime = trunc1(timeMul, 4.0);       
	vec2 temp = trunc2(uv.yy, vec2(8.0, 8.0)) + 100.0 * truncTime;
	float uv_trunc = randomNoise(temp.x, temp.y);
	float uv_randomTrunc = 6.0 * trunc1(timeMul, 24.0 * uv_trunc);
	float teamFloat = uv.y + uv_randomTrunc;

	temp = trunc2(
		vec2(teamFloat, teamFloat),
		vec2(
			8.0 * (Block_LinesWidth + Block_small_add),
			8.0 * (Block_LinesWidth + Block_small_add)
		)
	);
	float blockLine_random = 0.5 * randomNoise(temp.x, temp.y);	
	blockLine_random = sign(blockLine_random) * clamp((abs(blockLine_random) - Block_Amount) / (0.4), 0.0, 1.0);
	blockLine_random = lerp1(0.0, blockLine_random, (Block_Offset + 1.0));

	return blockLine_random;
}

vec4 glitch_car(){
    
	float split = Noise();

	float blockLine_random = blocLineRandom(uv);
	float blockLine_random_small = blocLineRandomSmall(uv);

	float uv_block_X = clamp(uv.x + (0.1 * (blockLine_random * 0.5 - 0.025)), 0.0, 1.0);
	float uv_block_X2 = clamp(uv.x - (0.1 * (blockLine_random_small * 0.5 - 0.025)), 0.0, 1.0);

	vec2 glitchUV = vec2(uv_block_X, uv.y);
	vec2 glitchUV2 = vec2(uv_block_X2, uv.y);

	vec4 result = vec4(0.0);

	vec4 baseColor = texture2D(TEXTURE_0, uv);

	vec3 maskTex = texture2D(TEXTURE_2, uv).rgb;

	vec4 blockGlitch = texture2D(TEXTURE_0, glitchUV);
	float alphaBlock = blockGlitch.a;
	alphaBlock = step(0.1, alphaBlock); //边框大块扰动的mask
	vec4 glitchCol = blockGlitch;

	//small block
	float blockGlitchSmall = texture2D(TEXTURE_2, glitchUV2).r;
	blockGlitchSmall = step(0.1, blockGlitchSmall) * maskTex.g; // use G channel to mask scanning area

	vec4 noiseTex = texture2D(TEXTURE_1, uv2);     //故障纹理 noiseTex.a 扫描区，无扰动
	float tempMask = min(blockGlitchSmall, noiseTex.a); //扫描区，小块
	float allGlitchAlpha = clamp(tempMask + alphaBlock, 0.0, 1.0); //块状故障抖动大+小
	allGlitchAlpha = step(0.1, allGlitchAlpha);
	
	blockGlitchSmall = lerp1(0.0, blockGlitchSmall, noiseTex.a);
	vec4 glitchColSmall = blockGlitchSmall * vec4(jitterColor, 1.0);

	glitchCol = lerp4(baseColor, glitchCol, maskTex.r);
	glitchCol = lerp4(glitchCol, glitchColSmall, blockGlitchSmall);

	vec4 emissionCol = lerp4(vec4(0.0), glitchCol, allGlitchAlpha) * Emission;
	result = glitchCol + emissionCol;
	
	split = step(NoiseStep * 0.001, split);
	float noiseAlpha = max(split, allGlitchAlpha);
	noiseAlpha = lerp1(0.0, allGlitchAlpha, split);
	result.rgb = lerp4(result, noiseTex, noiseAlpha).rgb;

    return result;
}
