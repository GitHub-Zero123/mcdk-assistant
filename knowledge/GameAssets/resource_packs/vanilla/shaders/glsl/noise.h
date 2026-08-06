
float rand(vec2 uv){
	return fract(sin(dot(uv, vec2(0.5161573, 0.4346789)))* 13571.5977913417);
}

float _lerp(float a, float b, float x){
	x = x * x * (3.0 - 2.0 * x);
	return a + (b - a) * x;
}

vec2 _loop(vec2 value, vec2 size){
	return fract(value / size) * size;
}

vec2 random_unit_vector(vec2 pos){
	float r = rand(pos * vec2(1.3219547, 0.5133416));
	r *= 3.1415926535 * 2.0;
	return normalize(vec2(
		cos(r),
		sin(r)
	));
}

float perlin(vec2 uuv, float block_size, float block_num){
	vec2 index = floor(uuv / block_size);
	vec2 MAGIC = vec2(block_num, block_num);
	index = fract(index / block_num) * block_num;
	uuv = fract(uuv / block_size);

	vec2 v0 = _loop(index, MAGIC);
	float val0 = dot(random_unit_vector(v0), uuv);
	v0 = _loop(index + vec2(0.0, 1.0), MAGIC);
	float val1 = dot(random_unit_vector(v0), uuv - vec2(0.0, 1.0));
	float ret = _lerp(val0, val1, uuv.y);

	v0 = _loop(index + vec2(1.0, 0.0), MAGIC);
	val0 = dot(random_unit_vector(v0), uuv - vec2(1.0, 0.0));
	v0 = _loop(index + vec2(1.0, 1.0), MAGIC);
	val1 = dot(random_unit_vector(v0), uuv - vec2(1.0, 1.0));

	return _lerp(ret, _lerp(val0, val1, uuv.y), uuv.x);
}
