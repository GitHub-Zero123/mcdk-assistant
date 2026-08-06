
// Get the gray result for color rgb.
vec3 graying(vec3 rgb, float R)
{
	float gray = dot(rgb, vec3(0.2126, 0.7152, 0.0722));
	return rgb * R + vec3(gray, gray, gray) * (1.0 - R);
}
