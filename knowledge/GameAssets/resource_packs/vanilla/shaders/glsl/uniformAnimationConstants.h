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

#if defined(USE_INSTANCE) && defined(NTES_ANIMATED_MODEL_BATCH)
#ifndef NTES_BATCH_BONES
#ifdef NTES_BATCH_LOW
#define NTES_BATCH_BONES 96
#define NTES_BATCH_INSTANCES 24
#else
#define NTES_BATCH_BONES 224
#define NTES_BATCH_INSTANCES 50
#endif
#endif
#ifndef NTES_BATCH_BONE_BASE_VECTORS
#define NTES_BATCH_BONE_BASE_VECTORS ((NTES_BATCH_INSTANCES + 3) / 4)
#endif
UNIFORM mat3x4 BONES_70[NTES_BATCH_BONES];
UNIFORM mat4x3 INSTANCE_WORLDMAT_50[NTES_BATCH_INSTANCES];
UNIFORM vec4 INSTANCE_BONE_BASE_50[NTES_BATCH_BONE_BASE_VECTORS];
#else
UNIFORM mat3x4 BONES_70[70];
#ifdef USE_INSTANCE
UNIFORM mat4x3 INSTANCE_WORLDMAT_50[50];
#endif
#endif

#if defined(USE_INSTANCE) && defined(NTES_ANIMATED_MODEL_BATCH_TEXTURE)
LAYOUT_BINDING(6) uniform sampler2D TEXTURE_6;
UNIFORM vec4 INSTANCE_TEXTURE_INFO_50;
#endif

mat4 mat3x4ToMat4(mat3x4 boneMat3x4){
		mat4 boneMat4x4;

		boneMat4x4[0] = boneMat3x4[0];
		boneMat4x4[1] = boneMat3x4[1];
		boneMat4x4[2] = boneMat3x4[2];
		boneMat4x4[3] = vec4(0, 0, 0, 1);

		return boneMat4x4;
}

#if defined(USE_INSTANCE) && defined(NTES_ANIMATED_MODEL_BATCH_TEXTURE)
vec4 NtesFetchAnimationBatchTexel(float texelIndex) {
	float y = floor(texelIndex * INSTANCE_TEXTURE_INFO_50.z);
	float x = texelIndex - y * INSTANCE_TEXTURE_INFO_50.x;
	return texelFetch(TEXTURE_6, ivec2(int(x), int(y)), 0);
}
#endif

mat4 GetBoneMatForNetease(int boneId){
#if defined(USE_INSTANCE) && defined(NTES_ANIMATED_MODEL_BATCH_TEXTURE)
	vec4 instanceBase = NtesFetchAnimationBatchTexel(INSTANCE_TEXTURE_INFO_50.y + float(gl_InstanceID * 4));
	float boneTexelBase = INSTANCE_TEXTURE_INFO_50.y + instanceBase.w + float(boneId * 3);
	mat3x4 boneMat = mat3x4(
		NtesFetchAnimationBatchTexel(boneTexelBase),
		NtesFetchAnimationBatchTexel(boneTexelBase + 1.0),
		NtesFetchAnimationBatchTexel(boneTexelBase + 2.0));
	return transpose(mat3x4ToMat4(boneMat));
#elif defined(USE_INSTANCE) && defined(NTES_ANIMATED_MODEL_BATCH)
	int boneBaseVecIndex = gl_InstanceID / 4;
	int boneBaseComponent = gl_InstanceID - boneBaseVecIndex * 4;
	vec4 boneBasePacked = INSTANCE_BONE_BASE_50[boneBaseVecIndex];
	float boneBase = boneBaseComponent == 0 ? boneBasePacked.x : (boneBaseComponent == 1 ? boneBasePacked.y : (boneBaseComponent == 2 ? boneBasePacked.z : boneBasePacked.w));
	boneId += int(boneBase + 0.5);
#endif
    return transpose(mat3x4ToMat4(BONES_70[boneId]));
}

#ifdef USE_INSTANCE
mat4 mat4x3ToMat4(mat4x3 worldMat4x3){
	mat4 worldMat4x4;
	worldMat4x4[0] = vec4(worldMat4x3[0], 0);
	worldMat4x4[1] = vec4(worldMat4x3[1], 0);
	worldMat4x4[2] = vec4(worldMat4x3[2], 0);
	worldMat4x4[3] = vec4(worldMat4x3[3], 1);
	return worldMat4x4;
}

mat4 GetInstanceWorldMatForNetease(){
#if defined(NTES_ANIMATED_MODEL_BATCH_TEXTURE)
	float instanceTexelBase = INSTANCE_TEXTURE_INFO_50.y + float(gl_InstanceID * 4);
	vec4 c0 = NtesFetchAnimationBatchTexel(instanceTexelBase);
	vec4 c1 = NtesFetchAnimationBatchTexel(instanceTexelBase + 1.0);
	vec4 c2 = NtesFetchAnimationBatchTexel(instanceTexelBase + 2.0);
	vec4 c3 = NtesFetchAnimationBatchTexel(instanceTexelBase + 3.0);
	c0.w = 0.0;
	c3.w = 1.0;
	return mat4(c0, c1, c2, c3);
#else
	return mat4x3ToMat4(INSTANCE_WORLDMAT_50[gl_InstanceID]);
#endif
}
#endif

#endif
