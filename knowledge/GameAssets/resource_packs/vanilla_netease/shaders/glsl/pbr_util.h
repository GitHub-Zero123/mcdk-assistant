#ifndef PBR_UTIL_H
#define PBR_UTIL_H

// ============================================================
// Cook-Torrance PBR with Metalness Workflow
// ============================================================

// ---------- Constants ----------
#define PI 3.14159265359
#define INV_PI 0.31830988618

#define DIELECTRIC_F0 vec3(0.04)

// Compute F0 from IOR: F0 = ((ior - 1) / (ior + 1))^2
// Returns default DIELECTRIC_F0 when ior <= 0 (disabled).
vec3 iorToF0(float ior) {
    float r = (ior - 1.0) / (ior + 1.0);
    return vec3(r * r);
}

// Mobile uses a higher minimum to avoid fp16 underflow.
// MIN_PERCEPTUAL_ROUGHNESS^4 > 0 in fp16 => 2^(-14/4) rounded up
#if defined(TARGET_MOBILE)
#define MIN_PERCEPTUAL_ROUGHNESS 0.089
#define MIN_ROUGHNESS            0.007921
#else
#define MIN_PERCEPTUAL_ROUGHNESS 0.045
#define MIN_ROUGHNESS            0.002025
#endif

#define MIN_NOV 1e-4

// Forward declarations are needed on PC (#version 410) due to cross-include
// function visibility rules. However, Apple's GLSL ES compiler (A15 GPU, iOS)
// may treat prototype + definition with 'out' params as a redefinition error.
// Guard with __VERSION__ >= 400 so they are only emitted on desktop GL.
#if __VERSION__ >= 400
vec3 evaluateDirectPBR(
    vec3 N,
    vec3 V,
    vec3 L,
    vec3 lightColor,
    vec3 albedo,
    float metalness,
    float perceptualRoughness
);

void evaluateDirectPBRComponents(
    vec3 N,
    vec3 V,
    vec3 L,
    vec3 lightColor,
    vec3 albedo,
    float metalness,
    float perceptualRoughness,
    out vec3 diffuseDirect,
    out vec3 specularDirect
);
#endif // __VERSION__ >= 400

// ---------- Roughness Remapping ----------
// Artist-facing perceptual roughness maps to squared microfacet roughness.
float perceptualRoughnessToRoughness(float perceptualRoughness) {
    return perceptualRoughness * perceptualRoughness;
}

float roughnessToPerceptualRoughness(float roughness) {
    return sqrt(roughness);
}

// ---------- Texture Packing Convention ----------
// TEXTURE_0: Albedo (RGB) + AO or Alpha (A)
// TEXTURE_1: Normal (RG, octahedral or two-channel) + Roughness (B) + Metalness (A)
// TEXTURE_2: IBL Cubemap (samplerCube, bound externally)
// TEXTURE_3: Reserved (emissive / mask, optional)
// Note: BRDF LUT is replaced by analytical approximation (split-sum fitting)

// ============================================================
// Normal Distribution Function: GGX / Trowbridge-Reitz
// Uses Lagrange's identity on mobile to avoid fp16 precision issues
// ||N x H||^2 = 1 - NoH^2 (computed stably via cross product)
// mediump: all inputs/outputs are [0,~65k] after clamp, fp16-safe
// ============================================================
mediump float D_GGX(mediump float NoH, mediump float roughness, mediump vec3 N, mediump vec3 H) {
    // Use a cross product to compute 1-NoH^2.
    // This avoids floating-point cancellation when NoH is close to 1
    mediump vec3 NxH = cross(N, H);
    mediump float oneMinusNoHSquared = dot(NxH, NxH);

    mediump float a = NoH * roughness;
    // Apple GPU mediump can overflow on intermediate multiplies near the fp16 edge.
    mediump float k = min(roughness / (oneMinusNoHSquared + a * a), 250.0);
    mediump float d = k * (k * INV_PI);
    return d;
}

// ============================================================
// Geometry Function: Smith-GGX (height-correlated)
// ============================================================
mediump float V_SmithGGXCorrelated(mediump float NoV, mediump float NoL, mediump float roughness) {
    mediump float a2 = roughness * roughness;
    mediump float lambdaV = NoL * sqrt((NoV - a2 * NoV) * NoV + a2);
    mediump float lambdaL = NoV * sqrt((NoL - a2 * NoL) * NoL + a2);
    // This lower bound prevents fp16 overflow in the reciprocal.
    return 0.5 / max(lambdaV + lambdaL, 0.0000077);
}

// Fast height-correlated visibility approximation.
mediump float V_SmithGGXCorrelatedFast(mediump float NoV, mediump float NoL, mediump float roughness) {
    // Keep the reciprocal away from the fp16 edge on Apple GPU.
    return 0.5 / max(mix(2.0 * NoL * NoV, NoL + NoV, roughness), 1e-4);
}

// Anisotropic GGX distribution for silk.
mediump float D_GGX_Anisotropic(mediump float roughnessT, mediump float roughnessB, mediump float ToH, mediump float BoH, mediump float NoH) {
    mediump float a2 = max(roughnessT * roughnessB, MIN_ROUGHNESS * MIN_ROUGHNESS);
    mediump vec3 v = vec3(roughnessB * ToH, roughnessT * BoH, a2 * NoH);
    mediump float v2 = max(dot(v, v), 1e-6);
    // invPI * a2^3 / dot(v, v)^2.
    return (a2 * a2 * a2) * INV_PI / (v2 * v2);
}

// Height-correlated Smith visibility for anisotropic GGX.
mediump float V_SmithGGXCorrelatedAnisotropic(
    mediump float roughnessT,
    mediump float roughnessB,
    mediump float ToV,
    mediump float BoV,
    mediump float NoV,
    mediump float ToL,
    mediump float BoL,
    mediump float NoL
) {
    mediump float lambdaV = NoL * length(vec3(roughnessT * ToV, roughnessB * BoV, NoV));
    mediump float lambdaL = NoV * length(vec3(roughnessT * ToL, roughnessB * BoL, NoL));
    return 0.5 / max(lambdaV + lambdaL, 0.0000077);
}

void convertAnisotropyToRoughness(mediump float roughness, mediump float anisotropy, out mediump float roughnessT, out mediump float roughnessB) {
    mediump float a = clamp(anisotropy, -0.99, 0.99);
    roughnessT = max(roughness * (1.0 + a), MIN_ROUGHNESS);
    roughnessB = max(roughness * (1.0 - a), MIN_ROUGHNESS);
}

vec3 ComputeViewFacingNormal(vec3 V, vec3 T) {
    vec3 projected = V - T * dot(V, T);
    float projectedLen2 = dot(projected, projected);
    return projectedLen2 > 1e-5 ? projected * inversesqrt(projectedLen2) : V;
}

vec3 GetAnisotropicModifiedNormal(vec3 grainDir, vec3 N, vec3 V, float anisotropy) {
    vec3 grainNormal = ComputeViewFacingNormal(V, grainDir);
    return normalize(mix(N, grainNormal, anisotropy));
}

// Single-fetch approximation for anisotropic GGX IBL.
void GetGGXAnisotropicModifiedNormalAndRoughness(
    vec3 bitangentWS,
    vec3 tangentWS,
    vec3 N,
    vec3 V,
    float anisotropy,
    float perceptualRoughness,
    out vec3 iblN,
    out float iblPerceptualRoughness
) {
    vec3 grainDirWS = (anisotropy >= 0.0) ? bitangentWS : tangentWS;
    float stretch = abs(anisotropy) * clamp(1.5 * sqrt(perceptualRoughness), 0.0, 1.0);
    iblN = GetAnisotropicModifiedNormal(grainDirWS, N, V, stretch);
    iblPerceptualRoughness = max(perceptualRoughness * clamp(1.2 - abs(anisotropy), 0.0, 1.0), MIN_PERCEPTUAL_ROUGHNESS);
}

// ============================================================
// Fresnel: Schlick Approximation
// ============================================================

// Form with an explicit f90 parameter.
mediump vec3 F_Schlick(mediump vec3 f0, mediump float f90, mediump float VoH) {
    // pow5 inlined for mobile performance
    mediump float x = 1.0 - VoH;
    mediump float x2 = x * x;
    mediump float x5 = x2 * x2 * x;
    return f0 + (f90 - f0) * x5;
}

// Convenience: f90 = 1.0 (common case)
mediump vec3 F_Schlick(mediump float VoH, mediump vec3 f0) {
    return F_Schlick(f0, 1.0, VoH);
}

// Compute f90 from f0.
// f90 = saturate(dot(f0, vec3(50.0 * 0.33)))
mediump float computeF90(mediump vec3 f0) {
    return clamp(dot(f0, vec3(50.0 * 0.33)), 0.0, 1.0);
}

// Roughness-aware Schlick approximation for IBL.
vec3 F_SchlickRoughness(float cosTheta, vec3 f0, float roughness) {
    float x = 1.0 - cosTheta;
    float x2 = x * x;
    float x5 = x2 * x2 * x;
    return f0 + (max(vec3(1.0 - roughness), f0) - f0) * x5;
}

// ============================================================
// Diffuse BRDF: Lambertian
// ============================================================
float Fd_Lambert() {
    return INV_PI;
}

// Rough diffuse response for silk.
mediump float Fd_DisneyDiffuse(mediump float NoV, mediump float NoL, mediump float LoH, mediump float perceptualRoughness) {
    mediump float energyBias = mix(0.0, 0.5, perceptualRoughness);
    mediump float energyFactor = mix(1.0, 1.0 / 1.51, perceptualRoughness);
    mediump float fd90 = energyBias + 2.0 * LoH * LoH * perceptualRoughness;

    mediump float lightScatterX = 1.0 - NoL;
    mediump float lightScatterX2 = lightScatterX * lightScatterX;
    mediump float lightScatter = 1.0 + (fd90 - 1.0) * lightScatterX2 * lightScatterX2 * lightScatterX;

    mediump float viewScatterX = 1.0 - NoV;
    mediump float viewScatterX2 = viewScatterX * viewScatterX;
    mediump float viewScatter = 1.0 + (fd90 - 1.0) * viewScatterX2 * viewScatterX2 * viewScatterX;

    return lightScatter * viewScatter * energyFactor * INV_PI;
}

/*
// Disabled fitted diffuse FGD for visual comparison.
// Fit target: 64x64, 4096-sample, B10-quantized diffuse channel.
// Six-coefficient mobile fit: RMSE 6.84e-4, max error 3.61e-3.
mediump float preintegratedDisneyDiffuseFGD(mediump float NoV, mediump float perceptualRoughness) {
    mediump float n = clamp(NoV, 0.0, 1.0);
    mediump float p = clamp(perceptualRoughness, 0.0, 1.0);
    mediump float grazing = 1.0 - n;
    mediump float grazing2 = grazing * grazing;
    grazing = grazing2 * grazing2 * grazing;

    mediump float base = 0.94252535 + 0.05423709 * p;
    mediump float edge = -0.47121978 + p * (0.91124779 + 0.59019486 * n + 0.06782767 * p);
    return clamp(base + grazing * edge, 0.5, 1.5);
}
*/

// Previous analytic diffuse FGD, restored because it preserves a wider visible silk range.
mediump float preintegratedDisneyDiffuseFGD(mediump float NoV, mediump float perceptualRoughness) {
    mediump float energyBias = mix(0.0, 0.5, perceptualRoughness);
    mediump float energyFactor = mix(1.0, 1.0 / 1.51, perceptualRoughness);
    mediump float fd90 = energyBias + 2.0 * NoV * NoV * perceptualRoughness;

    mediump float viewScatterX = 1.0 - NoV;
    mediump float viewScatterX2 = viewScatterX * viewScatterX;
    mediump float viewScatter = 1.0 + (fd90 - 1.0) * viewScatterX2 * viewScatterX2 * viewScatterX;

    // Cosine-weighted average of pow5(1 - NoL) over the hemisphere is 1/21.
    mediump float lightScatter = 1.0 + (fd90 - 1.0) * 0.0476190476;
    return clamp(viewScatter * lightScatter * energyFactor, 0.0, 1.5);
}

// ============================================================
// IBL Split-Sum Approximation
// Curve-fitted BRDF LUT replacement for the mobile path.
// Replaces the 2D LUT texture lookup with analytical approximation
// ============================================================

// Mobile DFG approximation.
// Returns vec2(scale, bias) for split-sum: F0 * scale + bias
vec2 prefilteredDFG_Karis(float NoV, float roughness) {
    // Analytical split-sum coefficients.
    const vec4 c0 = vec4(-1.0, -0.0275, -0.572, 0.022);
    const vec4 c1 = vec4(1.0, 0.0425, 1.04, -0.04);
    vec4 r = roughness * c0 + c1;
    float a004 = min(r.x * r.x, exp2(-9.28 * NoV)) * r.x + r.y;
    return vec2(-1.04, 1.04) * a004 + r.zw;
}

// Alternative cloth/mobile approximation.
vec2 prefilteredDFG_LazarovFilament(float NoV, float roughness) {
    // Alternative fitted coefficients.
    vec4 p0 = vec4(0.5745, 1.548, -0.02397, 1.301);
    vec4 p1 = vec4(0.5753, -0.2511, -0.02066, 0.4755);
    vec4 t = roughness * p0 + p1;
    float bias = clamp(t.x * min(t.y, exp2(-7.672 * NoV)) + t.z, 0.0, 1.0);
    float delta = clamp(t.w, 0.0, 1.0);
    float scale = delta - bias;
    return vec2(scale, bias);
}

// ============================================================
// Charlie DFG approximation for cloth.
// ------------------------------------------------------------
// Charlie NDF with cloth visibility and an IBL fit.
//
// Notes:
// - Input roughness uses perceptual roughness domain [0, 1], same as current code.
// - This keeps compatibility with our existing split-sum style IBL pipeline.
// ============================================================
float prefilteredDFG_CharlieCore(float NoV, float perceptualRoughness) {
    const vec3 c0 = vec3(0.95, 1250.0, 0.0095);
    const vec4 c1 = vec4(0.04, 0.2, 0.3, 0.2);

    NoV = clamp(NoV, MIN_NOV, 1.0);
    perceptualRoughness = clamp(perceptualRoughness, MIN_PERCEPTUAL_ROUGHNESS, 1.0);

    float a = 1.0 - NoV;
    float b = 1.0 - perceptualRoughness;

    float n = pow(c1.x + a, 64.0);
    float e = b - c0.x;
    float g = exp2(-(e * e) * c0.y);
    float f = b + c1.y;

    float a2 = a * a;
    float a4 = a2 * a2;
    float a8 = a4 * a4;
    float c = n * g + c1.z * (a + c1.w) * perceptualRoughness + f * f * a8;
    return min(c, 18.0);
}

// Cloth IBL approximation packed as (scale, bias)-like pair.
// The second channel uses a fitted proportional term.
vec2 prefilteredDFG_ClothCharlie(float NoV, float perceptualRoughness) {
    float r = prefilteredDFG_CharlieCore(NoV, perceptualRoughness);
    return vec2(r, r * 0.0095);
}

// ============================================================
// IBL: Evaluate indirect lighting
// prefilteredEnvColor: pre-filtered environment map sample (from cubemap at roughness mip)
// irradiance: diffuse irradiance from cubemap
// ============================================================
vec3 evaluateIBL(
    vec3 irradiance,
    vec3 prefilteredEnvColor,
    vec3 f0,
    float NoV,
    float roughness,
    vec3 albedo,
    float metalness
) {
    vec2 dfg = prefilteredDFG_Karis(NoV, roughness);
    vec3 specularColor = f0 * dfg.x + dfg.y;
    vec3 kD = (1.0 - specularColor) * (1.0 - metalness);

    vec3 diffuse = kD * albedo * irradiance * INV_PI;
    vec3 specular = prefilteredEnvColor * specularColor;

    return diffuse + specular;
}

// ============================================================
// Octahedral Mapping
// Maps the full unit sphere to a [0,1]^2 square via octahedron projection.
// Reference: "Survey of Efficient Representations for Independent Unit Vectors"
//            Cigolle et al. 2014
// ============================================================

// Helper: octahedral wrap for the lower hemisphere
vec2 octWrap(vec2 v) {
    return (1.0 - abs(v.yx)) * vec2(v.x >= 0.0 ? 1.0 : -1.0,
                                     v.y >= 0.0 ? 1.0 : -1.0);
}

// Direction (normalized) -> Octahedral UV in [0,1]^2
// +Y (sky/up) maps to center (0.5, 0.5), -Y (ground) maps to corners.
// Uses Y<->Z swizzle: cubemap Y-up axis becomes octahedral Z-pole axis.
vec2 dirToOctUV(vec3 n) {
    // Swizzle: cubemap (X, Y-up, Z) → octahedral (X, Z, Y-pole)
    vec3 oct = vec3(n.x, n.z, n.y);
    // Project onto octahedron: divide by L1 norm
    oct /= (abs(oct.x) + abs(oct.y) + abs(oct.z));
    // Fold lower hemisphere
    oct.xy = oct.z >= 0.0 ? oct.xy : octWrap(oct.xy);
    // Remap from [-1,1] to [0,1]
    return oct.xy * 0.5 + 0.5;
}

// Octahedral UV in [0,1]^2 -> Direction (normalized)
// Inverse of dirToOctUV.
// Center (0.5, 0.5) → +Y (sky), corners → -Y (ground).
vec3 octUVToDir(vec2 uv) {
    // Remap from [0,1] to [-1,1]
    vec2 f = uv * 2.0 - 1.0;
    // Reconstruct z from the remaining L1 budget
    vec3 n = vec3(f.x, f.y, 1.0 - abs(f.x) - abs(f.y));
    // Unfold lower hemisphere
    if (n.z < 0.0) {
        n.xy = octWrap(n.xy);
    }
    // Swizzle back: octahedral (X, Y, Z-pole) → cubemap (X, Z-pole→Y-up, Y→Z)
    return normalize(vec3(n.x, n.z, n.y));
}

float computeReflectionLOD(float perceptualRoughness, float maxLOD) {
    return clamp(perceptualRoughness, 0.0, 1.0) * maxLOD;
}

// ============================================================
// Octahedral Mipmap Atlas sampling
// All 7 logical mip levels packed into a single 256x256 texture.
// Manual trilinear: 2x texture() + mix() between adjacent mips.
// ============================================================

// Atlas dimensions
const vec2 OCT_ATLAS_SIZE = vec2(256.0, 256.0);
const int  OCT_MAX_MIP = 6;

// Per-mip layout: vec4(offsetX, offsetY, paddedSize, contentSize)
const vec4 OCT_MIP_PARAMS[7] = vec4[7](
    vec4(  0.0,   0.0, 130.0, 128.0),  // mip0
    vec4(  0.0, 130.0,  66.0,  64.0),  // mip1
    vec4( 66.0, 130.0,  34.0,  32.0),  // mip2
    vec4(100.0, 130.0,  18.0,  16.0),  // mip3
    vec4(118.0, 130.0,  10.0,   8.0),  // mip4
    vec4(  0.0, 196.0,   6.0,   4.0),  // mip5
    vec4(  6.0, 196.0,   4.0,   2.0)   // mip6
);

// Convert direction + mip index to atlas UV with border offset and bilinear clamp.
vec2 octMipAtlasUV(vec3 dir, int mipIdx) {
    vec2 contentUV = dirToOctUV(normalize(dir));  // [0, 1]^2
    vec4 p = OCT_MIP_PARAMS[mipIdx];
    // pixel coord = mipOffset + 1 border + contentUV * contentSize
    vec2 pixel = vec2(p.x, p.y) + vec2(1.0) + contentUV * p.w;
    vec2 uv = pixel / OCT_ATLAS_SIZE;
    // Clamp to prevent bilinear bleeding into adjacent mip regions
    vec2 lo = (vec2(p.x, p.y) + 0.5) / OCT_ATLAS_SIZE;
    vec2 hi = (vec2(p.x, p.y) + p.z - 0.5) / OCT_ATLAS_SIZE;
    return clamp(uv, lo, hi);
}

// Manual trilinear: sample two adjacent mips and interpolate.
vec3 sampleIBLOctAtlas(sampler2D atlas, vec3 dir, float perceptualRoughness) {
    float lod = clamp(perceptualRoughness, 0.0, 1.0) * float(OCT_MAX_MIP);
    int lodLo = int(floor(lod));
    int lodHi = min(lodLo + 1, OCT_MAX_MIP);
    float f = fract(lod);

    vec2 uvLo = octMipAtlasUV(dir, lodLo);
    vec2 uvHi = octMipAtlasUV(dir, lodHi);

    vec3 cLo = texture(atlas, uvLo).rgb;
    vec3 cHi = texture(atlas, uvHi).rgb;

    return mix(cLo, cHi, f);
}

// Legacy single-texture octahedral sampling (deprecated, kept for reference)
vec3 sampleIBLOctahedral(sampler2D octMap, vec3 dir, float perceptualRoughness, float maxLOD) {
    float lod = computeReflectionLOD(perceptualRoughness, maxLOD);
    vec2 uv = dirToOctUV(normalize(dir));
#if __VERSION__ >= 300
    return textureLod(octMap, uv, lod).rgb;
#else
    #if defined(GL_EXT_shader_texture_lod)
        return texture2DLodEXT(octMap, uv, lod).rgb;
    #else
        return texture2D(octMap, uv).rgb;
    #endif
#endif
}

vec3 sampleIBLCubemap(samplerCube envCubemap, vec3 dir, float perceptualRoughness, float maxLOD) {
    float lod = computeReflectionLOD(perceptualRoughness, maxLOD);
#if __VERSION__ >= 300
    return textureLod(envCubemap, normalize(dir), lod).rgb;
#else
    // GLSL ES 100 fragment path must use EXT entrypoints for explicit LOD.
    // Using textureLod directly here can silently behave like LOD0 on some drivers.
    #if defined(GL_EXT_shader_texture_lod)
        return textureCubeLodEXT(envCubemap, normalize(dir), lod).rgb;
    #else
        return textureCube(envCubemap, normalize(dir)).rgb;
    #endif
#endif
}

// ============================================================
// Energy compensation for multiple scattering.
// Prevents darkening at high roughness for metallic surfaces
// ============================================================
vec3 computeEnergyCompensation(vec3 f0, vec2 dfg) {
    // dfg.y is the integral of the BRDF, energyCompensation accounts for
    // the energy lost to multiple scattering events
    return 1.0 + f0 * (1.0 / max(dfg.y, 0.001) - 1.0);
}

// ============================================================
// Specular dominant direction.
// At higher roughness, the reflection lobe becomes wider and shifts
// toward the normal. This adjusts the reflection vector accordingly.
// ============================================================
vec3 getSpecularDominantDirection(vec3 N, vec3 R, float roughness) {
    // roughness here is alpha (perceptualRoughness^2).
    // Using alpha directly (not alpha^2) for a stronger pull toward N,
    // which reduces R jitter and IBL shimmer at moderate roughness.
    return normalize(mix(R, N, roughness));
}

vec3 GetSpecularDominantDir(vec3 N, vec3 R, float perceptualRoughness, float NoV) {
    float p = clamp(perceptualRoughness, MIN_PERCEPTUAL_ROUGHNESS, 1.0);
    float a = max(1.0 - p * p, 0.0);
    float s = sqrt(a);
    float lerpFactor = (s + p * p) * clamp(a * a + mix(0.0, a, NoV * NoV), 0.0, 1.0);
    return normalize(mix(N, R, lerpFactor));
}

// ============================================================
// Derivative-Based Specular Anti-Aliasing
//
// Uses screen-space partial derivatives of the tangent-space normal
// map sample to estimate sub-pixel normal variance. This is more
// robust than Toksvig when mip levels are renormalized or the
// normal map is BC5/XY encoded, because it measures actual per-pixel
// variation rather than relying on vector length shrinkage.
//
// Tuning:
//   SPEC_AA_VARIANCE_SCALE - how aggressively to widen roughness
//     0.2 = subtle, 0.5 = moderate, 1.0+ = aggressive
// ============================================================
// Gated SpecAA: only intervenes strongly when perceptualRoughness is low
// (sharp highlights), and caps the maximum roughness increase to avoid
// washing out material detail on already-rough surfaces.
#define SPEC_AA_SCALE    0.45  // 0.2~0.8: overall strength
#define SPEC_AA_MIN_PR   0.12  // below this pr, full intervention
#define SPEC_AA_MAX_ADD  0.30  // maximum pr increase cap

float specAA_NormalMapDeriv(float pr, vec3 normalTS_raw) {
    // Screen-space derivatives of tangent-space normal (unnormalized)
    vec3 dx = dFdx(normalTS_raw);
    vec3 dy = dFdy(normalTS_raw);

    float v = dot(dx, dx) + dot(dy, dy);

    // Threshold + soft compress: ignore tiny noise, saturate big changes
    v = max(v - 0.02, 0.0);
    v = v / (v + 0.25);
    v = clamp(v, 0.0, 1.0);

    // Gate: only widen when pr is small (sharp specular); leave rough surfaces alone
    float gate = 1.0 - smoothstep(SPEC_AA_MIN_PR, 0.6, pr);

    float add = min(SPEC_AA_SCALE * v * gate, SPEC_AA_MAX_ADD);
    return clamp(pr + add, 0.0, 1.0);
}

// ============================================================
// Full Cook-Torrance Specular BRDF Evaluation
// Returns specular contribution for a single directional light
// Uses cross-product GGX evaluation for mobile precision.
// ============================================================
highp vec3 specularBRDF(mediump vec3 N, mediump vec3 H, mediump float NoH, mediump float NoV, mediump float NoL, mediump float VoH, mediump float roughness, mediump vec3 f0) {
    highp float D = D_GGX(NoH, roughness, N, H);
    highp float V = V_SmithGGXCorrelatedFast(NoV, NoL, roughness);
    mediump float f90 = computeF90(f0);
    highp vec3 F = F_Schlick(f0, f90, VoH);
    return (D * V) * F;
}

mediump vec3 specularBRDFAnisotropic(
    mediump vec3 T,
    mediump vec3 B,
    mediump vec3 N,
    mediump vec3 V,
    mediump vec3 L,
    mediump vec3 H,
    mediump float NoH,
    mediump float NoV,
    mediump float NoL,
    mediump float VoH,
    mediump float roughness,
    mediump float anisotropy,
    mediump vec3 f0
) {
    mediump float roughnessT;
    mediump float roughnessB;
    convertAnisotropyToRoughness(roughness, anisotropy, roughnessT, roughnessB);

    mediump float ToH = dot(T, H);
    mediump float BoH = dot(B, H);
    mediump float ToV = dot(T, V);
    mediump float BoV = dot(B, V);
    mediump float ToL = dot(T, L);
    mediump float BoL = dot(B, L);

    mediump float D = D_GGX_Anisotropic(roughnessT, roughnessB, ToH, BoH, NoH);
    mediump float Vis = V_SmithGGXCorrelatedAnisotropic(roughnessT, roughnessB, ToV, BoV, NoV, ToL, BoL, NoL);
    mediump float f90 = computeF90(f0);
    mediump vec3 F = F_Schlick(f0, f90, VoH);
    return (D * Vis) * F;
}

// ============================================================
// Complete PBR Direct Lighting (single light)
// Perceptual roughness input; squared roughness internally.
// ============================================================
vec3 evaluateDirectPBR(
    vec3 N,                    // world-space normal
    vec3 V,                    // view direction (toward camera)
    vec3 L,                    // light direction (toward light)
    vec3 lightColor,           // light radiance
    vec3 albedo,               // base color
    float metalness,           // metalness [0,1]
    float perceptualRoughness  // perceptual roughness [0,1] (artist-facing)
) {
    // Clamp perceptual roughness, then square it.
    perceptualRoughness = max(perceptualRoughness, MIN_PERCEPTUAL_ROUGHNESS);
    float roughness = perceptualRoughnessToRoughness(perceptualRoughness);

    vec3 H = normalize(V + L);

    float NoV = max(dot(N, V), MIN_NOV);
    float NoL = max(dot(N, L), 0.0);
    float NoH = max(dot(N, H), 0.0);
    float VoH = max(dot(V, H), 0.0);

    // Metalness workflow: derive f0 and diffuse color
    // f0 = baseColor * metallic + reflectance * (1 - metallic)
    // Default reflectance = 0.04 (IOR 1.5 dielectric)
    vec3 f0 = mix(DIELECTRIC_F0, albedo, metalness);
    vec3 diffuseColor = albedo * (1.0 - metalness);

    // Specular BRDF (Cook-Torrance)
    vec3 Fr = specularBRDF(N, H, NoH, NoV, NoL, VoH, roughness, f0);

    // NOTE: Energy compensation (computeEnergyCompensation) is intentionally
    // NOT applied to direct lighting. It is designed for IBL multi-scattering
    // and applying it here amplifies brightness variations that worsen shimmer.

    // Diffuse (energy-conserving Lambertian)
    vec3 Fd = diffuseColor * Fd_Lambert();

    return (Fd + Fr) * lightColor * NoL;
}

void evaluateDirectPBRComponentsWithF0(
    vec3 N,
    vec3 V,
    vec3 L,
    vec3 lightColor,
    vec3 albedo,
    float metalness,
    float perceptualRoughness,
    vec3 customDielectricF0,
    out vec3 diffuseDirect,
    out vec3 specularDirect
) {
    mediump float pr = max(perceptualRoughness, MIN_PERCEPTUAL_ROUGHNESS);
    mediump float roughness = perceptualRoughnessToRoughness(pr);

    mediump vec3 H = normalize(V + L);

    mediump float NoV = max(dot(N, V), MIN_NOV);
    mediump float NoL = max(dot(N, L), 0.0);
    mediump float NoH = max(dot(N, H), 0.0);
    mediump float VoH = max(dot(V, H), 0.0);

    mediump vec3 f0 = mix(customDielectricF0, albedo, metalness);
    mediump vec3 diffuseColor = albedo * (1.0 - metalness);

    highp vec3 Fr = specularBRDF(N, H, NoH, NoV, NoL, VoH, roughness, f0);
    mediump vec3 Fd = diffuseColor * Fd_Lambert();

    diffuseDirect = Fd * lightColor * NoL;
    specularDirect = Fr * lightColor * NoL;
}

void evaluateDirectPBRComponents(
    vec3 N,
    vec3 V,
    vec3 L,
    vec3 lightColor,
    vec3 albedo,
    float metalness,
    float perceptualRoughness,
    out vec3 diffuseDirect,
    out vec3 specularDirect
) {
    evaluateDirectPBRComponentsWithF0(N, V, L, lightColor, albedo, metalness, perceptualRoughness, DIELECTRIC_F0, diffuseDirect, specularDirect);
}

// ============================================================
// Convenience: full PBR shading with direct + IBL
// ============================================================
vec4 evaluatePBR(
    vec3 N,
    vec3 V,
    vec3 L,
    vec3 lightColor,
    vec3 albedo,
    float metalness,
    float roughness,
    float ao,
    vec3 irradiance,
    vec3 prefilteredEnv,
    float baseAlpha
) {
    roughness = max(roughness, MIN_ROUGHNESS);
    float NoV = max(dot(N, V), MIN_NOV);

    vec3 f0 = mix(DIELECTRIC_F0, albedo, metalness);

    // Direct lighting
    vec3 direct = evaluateDirectPBR(N, V, L, lightColor, albedo, metalness, roughness);

    // Indirect lighting (IBL)
    vec3 indirect = evaluateIBL(irradiance, prefilteredEnv, f0, NoV, roughness, albedo, metalness);

    vec3 color = (direct + indirect * ao);

    return vec4(color, baseAlpha);
}

// ============================================================
// Simple tonemapping (Reinhard) for HDR -> LDR on mobile
// ============================================================
vec3 tonemapReinhard(vec3 color) {
    return color / (color + vec3(1.0));
}

// Filmic tone mapping approximation.
vec3 tonemapACES(vec3 x) {
    const float a = 2.51;
    const float b = 0.03;
    const float c = 2.43;
    const float d = 0.59;
    const float e = 0.14;
    return clamp((x * (a * x + b)) / (x * (c * x + d) + e), 0.0, 1.0);
}

// Linear to sRGB
vec3 linearToSRGB(vec3 color) {
    return pow(max(color, vec3(0.0)), vec3(1.0 / 2.2));
}

// sRGB to Linear
vec3 sRGBToLinear(vec3 color) {
    return pow(max(color, vec3(0.0)), vec3(2.2));
}

#endif // PBR_UTIL_H
