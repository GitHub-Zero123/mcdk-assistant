#ifndef _UNIFORM_ANIMATION_CONSTANTS_H
#define _UNIFORM_ANIMATION_CONSTANTS_H

#include "uniformMacro.h"

#ifdef MCPE_PLATFORM_NX
layout(binding = 4) uniform AnimationConstants {
#endif
// BEGIN_UNIFORM_BLOCK(AnimationConstants) - unfortunately this macro does not work on old Amazon platforms so using above 3 lines instead
#if defined(LARGE_VERTEX_SHADER_UNIFORMS)
UNIFORM MAT4 BONES[8];
#else
UNIFORM MAT4 BONE;
#endif
END_UNIFORM_BLOCK


UNIFORM mat3x4 BONES_70[70];

mat4 mat3x4ToMat4(mat3x4 boneMat3x4){
		mat4 boneMat4x4;
		
		boneMat4x4[0] = boneMat3x4[0];
		boneMat4x4[1] = boneMat3x4[1];
		boneMat4x4[2] = boneMat3x4[2];
		boneMat4x4[3] = vec4(0, 0, 0, 1);
		
		return boneMat4x4;
}

mat4 GetBoneMatForNetease(int boneId){
    return transpose(mat3x4ToMat4(BONES_70[boneId]));
}

#ifdef USE_INSTANCE
UNIFORM mat4x3 INSTANCE_WORLDMAT_50[50];

mat4 mat4x3ToMat4(mat4x3 worldMat4x3){
		mat4 worldMat4x4;
		
		worldMat4x4[0] = vec4(worldMat4x3[0], 0);
		worldMat4x4[1] = vec4(worldMat4x3[1], 0);
		worldMat4x4[2] = vec4(worldMat4x3[2], 0);
		worldMat4x4[3] = vec4(worldMat4x3[3], 1);
		
		return worldMat4x4;
}

mat4 GetInstanceWorldMatForNetease(){
	return mat4x3ToMat4(INSTANCE_WORLDMAT_50[gl_InstanceID]);
}
#endif

#endif
