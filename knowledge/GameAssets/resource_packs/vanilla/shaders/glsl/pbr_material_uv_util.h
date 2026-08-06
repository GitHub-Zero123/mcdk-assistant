#ifndef PBR_MATERIAL_UV_UTIL_H
#define PBR_MATERIAL_UV_UTIL_H

#ifndef PBR_BASE_UV_TILING
#define PBR_BASE_UV_TILING vec2(1.0, 1.0)
#endif
#ifndef PBR_BASE_UV_OFFSET
#define PBR_BASE_UV_OFFSET vec2(0.0, 0.0)
#endif
#ifndef PBR_BASE_UV_ROTATOR
#define PBR_BASE_UV_ROTATOR 0.0
#endif
#ifndef PBR_DETAIL_UV_TILING
#define PBR_DETAIL_UV_TILING vec2(1.0, 1.0)
#endif
#ifndef PBR_DETAIL_UV_OFFSET
#define PBR_DETAIL_UV_OFFSET vec2(0.0, 0.0)
#endif
#ifndef PBR_DETAIL_UV_ROTATOR
#define PBR_DETAIL_UV_ROTATOR 0.0
#endif
vec2 pbrRotateVector(vec2 value, float rotator) {
    float angle = rotator * 6.28318530718;
    float s = sin(angle);
    float c = cos(angle);
    return vec2(
        value.x * c - value.y * s,
        value.x * s + value.y * c
    );
}

vec2 pbrApplyUVTransform(vec2 baseUV, vec2 tiling, vec2 offset, float rotator) {
    vec2 uvControl = baseUV * tiling + offset;
    vec2 centered = uvControl - vec2(0.5, 0.5);
    return pbrRotateVector(centered, rotator) + vec2(0.5, 0.5);
}

vec2 pbrUETextureUV(vec2 uvCoord) {
    return vec2(uvCoord.x, 1.0 - uvCoord.y);
}

vec2 pbrGetMaterialUV(vec2 baseUV) {
#ifdef USE_PBR_UV_TRANSFORM
#ifdef USE_MATERIAL_UNIFORM_REFLECTION
    return pbrApplyUVTransform(baseUV, pbr_base_uv_transform.xy, pbr_base_uv_transform.zw, pbr_base_uv_rotator);
#else
    return pbrApplyUVTransform(baseUV, PBR_BASE_UV_TILING, PBR_BASE_UV_OFFSET, PBR_BASE_UV_ROTATOR);
#endif
#elif defined(USE_ANISOTROPIC_SILK)
    return baseUV * SILK_TEXTURE_UV_SCALE;
#else
    return baseUV;
#endif
}

vec2 pbrGetDetailUV(vec2 baseUV) {
#ifdef USE_PBR_DETAIL_UV_TRANSFORM
#ifdef USE_MATERIAL_UNIFORM_REFLECTION
    return pbrApplyUVTransform(baseUV, pbr_detail_uv_transform.xy, pbr_detail_uv_transform.zw, pbr_detail_uv_rotator);
#else
    return pbrApplyUVTransform(baseUV, PBR_DETAIL_UV_TILING, PBR_DETAIL_UV_OFFSET, PBR_DETAIL_UV_ROTATOR);
#endif
#else
#if defined(USE_PBR_MATERIAL_UNIFORM_PARAMS) && defined(USE_MATERIAL_UNIFORM_REFLECTION)
    return pbrApplyUVTransform(baseUV, vec2(max(detail_uv_scale, 0.0)), vec2(0.0, 0.0), detail_rotation_turns);
#else
    return baseUV * DETAIL_UV_SCALE;
#endif
#endif
}

#endif
