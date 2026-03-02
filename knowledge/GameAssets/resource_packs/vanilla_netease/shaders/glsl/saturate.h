#include "uniformExtraVectorConstants.h"

const vec3 luminanceWeight = vec3(0.2126, 0.7152, 0.0722);

vec3 saturate(in vec3 color) {
	if (COLOR_MAPPING_PARAM.w > 0.0) {                      // toggle
		vec3 minCon = vec3(0.5);
		color = mix(minCon, color, COLOR_MAPPING_PARAM.z);  // contrast

		vec3 minSat = vec3(dot(color, luminanceWeight));
		color = mix(minSat, color, COLOR_MAPPING_PARAM.y);  // saturation

		color *= COLOR_MAPPING_PARAM.x;                     // brightness
	}
	return color;
}
