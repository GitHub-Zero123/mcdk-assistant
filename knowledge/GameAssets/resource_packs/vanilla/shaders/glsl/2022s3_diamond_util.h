precision highp float;

float lerp1(float a, float b, float x){
	return a + x * (b - a);
}

float rand(vec2 uv){
	return fract(sin(dot(uv, vec2(12.9898, 78.233))) * 43758.5453);
}

vec2 loop(vec2 value, vec2 size){
return fract(value / size) * size;
}

vec2 random_unit_vector(vec2 pos){
	float r = rand(pos * vec2(1.3219547, 0.5133416));
	r *= 3.1415926535 * 2.0;
	return vec2(
		cos(r),
		sin(r)
	);
}

float perlin(vec2 uuv, float BLOCK_SIZE, float BLOCK_NUM){
	vec2 index = floor(uuv / BLOCK_SIZE);
	vec2 MAGIC = vec2(BLOCK_NUM, BLOCK_NUM);
	index = fract(index / BLOCK_NUM) * BLOCK_NUM;
	uuv = fract(uuv / BLOCK_SIZE);

	vec2 v0 = loop(index, MAGIC);
	float val0 = dot(random_unit_vector(v0), uuv);
	v0 = loop(index + vec2(0.0, 1.0), MAGIC);
	float val1 = dot(random_unit_vector(v0), uuv - vec2(0.0, 1.0));
	float ret = lerp1(val0, val1, uuv.y);

	v0 = loop(index + vec2(1.0, 0.0), MAGIC);
	val0 = dot(random_unit_vector(v0), uuv - vec2(1.0, 0.0));
	v0 = loop(index + vec2(1.0, 1.0), MAGIC);
	val1 = dot(random_unit_vector(v0), uuv - vec2(1.0, 1.0));

	return lerp1(ret, lerp1(val0, val1, uuv.y), uuv.x);
}

vec4 diamond(){
	vec3 viewDir = normalize(worldViewDir);
	vec3 normalDir = normalize(worldNormal);

	mediump float fresnelVal = 1.0 - dot(normalDir, viewDir);
	fresnelVal = clamp(FRES_BIAS + FRES_SCALE * fresnelVal * fresnelVal, 0.0, 1.0);

	// refraction
	vec3 dir = refract(viewDir, normalDir, 1.0 / ETA) * 0.5 + 0.5 + TIME * 0.04 * dispersionSpeed_F;
	mediump vec3 rColor = texture2D(TEXTURE_1, fract(dir.xy)).rgb * DISPARSION_F;
	
	// reflection
	dir = reflect(viewDir, normalDir) * 0.5 + 0.5;
	rColor += texture2D(TEXTURE_1, dir.xy).rgb * fresnelVal;

	//sparkles
	mediump vec2 star_uv = floor(floor(uv * vec2(512.0, 512.0) * UV_SCALE) / STAR_SIZE);
	mediump vec2 move_dir = normalize(vec2(rand(star_uv * 0.4987368153), rand(-star_uv * 0.31168489)) - 0.5);
	star_uv = star_uv + TIME * move_dir;
	mediump float star_val = perlin(star_uv, UV_PARA.x, UV_PARA.y) * 0.5 + 0.5;
	star_val = smoothstep(0.8 - 0.2 * StarsDensity_F, 0.8, star_val);
	mediump vec3 sparCol = rColor * star_val * shineInt_F;

	// ************* Mixture *************
	mediump vec3 result = rColor + sparCol;
	if (COLOR_MODE == 0.0){
		result = result * colorF;
	}else{
		result = result + colorF;
	}

	// base color and alpha mask
	mediump vec4 baseColor = texture2D(TEXTURE_0, uv);
	mediump float alphaMask = step(0.5, baseColor.a);
	result = alphaMask * baseColor.rgb + (1.0 - alphaMask) * result;

	// 1.0(use base map) or MIN_ALPHA(use shader color)
	alphaMask += (1.0 - alphaMask) * fresnelVal; // stronger reflection brings higher alpha
	alphaMask = max(alphaMask, MIN_ALPHA);

	return vec4(result, alphaMask);
}
