// Type definitions for Minecraft Bedrock Edition script APIs
// Project: https://docs.microsoft.com/minecraft/creator/
// Definitions by: Jake Shirley <https://github.com/JakeShirley>
//                 Mike Ammerlaan <https://github.com/mammerla>

/* *****************************************************************************
   Copyright (c) Microsoft Corporation.
   ***************************************************************************** */
/**
 * @beta
 * @packageDocumentation
 * The `@minecraft/server-graphics` module contains APIs to
 * change graphics and rendering settings.
 *
 * Manifest Details
 * ```json
 * {
 *   "module_name": "@minecraft/server-graphics",
 *   "version": "1.0.0-beta"
 * }
 * ```
 *
 */
import { EngineError, InvalidArgumentError } from '@minecraft/common';
import { BiomeType, Player, RGB, Vector3 } from '@minecraft/server';
/**
 * Used to affect atmospheric scattering per biome for Vibrant
 * Visuals
 */
export class BiomeAtmospherics {
    private constructor();
    /**
     * @remarks
     * Resets the horizon blend max to the value set by resource
     * packs or via the 'setHorizonBlendMax' API in behavior packs
     *
     * @worldMutation
     *
     * @throws This function can throw errors.
     *
     * {@link EngineError}
     */
    resetHorizonBlendMax(): void;
    /**
     * @remarks
     * Resets the horizon blend mie start to the value set by
     * resource packs or via the 'setHorizonBlendMieStart' API in
     * behavior packs
     *
     * @worldMutation
     *
     * @throws This function can throw errors.
     *
     * {@link EngineError}
     */
    resetHorizonBlendMieStart(): void;
    /**
     * @remarks
     * Resets the horizon blend min to the value set by resource
     * packs or via the 'setHorizonBlendMin' API in behavior packs
     *
     * @worldMutation
     *
     * @throws This function can throw errors.
     *
     * {@link EngineError}
     */
    resetHorizonBlendMin(): void;
    /**
     * @remarks
     * Resets the horizon blend start to the value set by resource
     * packs or via the 'setHorizonBlendStart' API in behavior
     * packs
     *
     * @worldMutation
     *
     * @throws This function can throw errors.
     *
     * {@link EngineError}
     */
    resetHorizonBlendStart(): void;
    /**
     * @remarks
     * Resets the moon mie strength to the value set by resource
     * packs or via the 'setMoonMieStrength' API in behavior packs
     *
     * @worldMutation
     *
     * @throws This function can throw errors.
     *
     * {@link EngineError}
     */
    resetMoonMieStrength(): void;
    /**
     * @remarks
     * Resets the rayleigh strength to the value set by resource
     * packs or via the 'setRayleighStrength' API in behavior packs
     *
     * @worldMutation
     *
     * @throws This function can throw errors.
     *
     * {@link EngineError}
     */
    resetRayleighStrength(): void;
    /**
     * @remarks
     * Resets the sky horizon color to the color set by resource
     * packs or via the 'setSkyHorizonColor' API in behavior packs
     *
     * @worldMutation
     *
     * @throws This function can throw errors.
     *
     * {@link EngineError}
     */
    resetSkyHorizonColor(): void;
    /**
     * @remarks
     * Resets the sky zenith color to the color set by resource
     * packs or via the 'setSkyZenithColor' API in behavior packs
     *
     * @worldMutation
     *
     * @throws This function can throw errors.
     *
     * {@link EngineError}
     */
    resetSkyZenithColor(): void;
    /**
     * @remarks
     * Resets the sun glare shape to the value set by resource
     * packs or via the 'setSunGlareShape' API in behavior packs
     *
     * @worldMutation
     *
     * @throws This function can throw errors.
     *
     * {@link EngineError}
     */
    resetSunGlareShape(): void;
    /**
     * @remarks
     * Resets the sun mie strength to the value set by resource
     * packs or via the 'setSunMieStrength' API in behavior packs
     *
     * @worldMutation
     *
     * @throws This function can throw errors.
     *
     * {@link EngineError}
     */
    resetSunMieStrength(): void;
    /**
     * @remarks
     * Sets the horizon blend max for atmospheric scattering in
     * Vibrant Visuals
     *
     * @worldMutation
     *
     * @param blendMax
     * Either a number (range [0,1]) or a set of keyframes. The
     * keyframes are composed of key value pairs. The key is a
     * number (range [0,1]) to signify a time of day (0.0 and 1.0
     * are noon, 0.25 is sunset, 0.5 is midnight, and 0.75 is
     * sunrise). The value is also a number (range [0,1])
     * @throws This function can throw errors.
     *
     * {@link InvalidArgumentError}
     */
    setHorizonBlendMax(blendMax: number | Record<number, number>): void;
    /**
     * @remarks
     * Sets the horizon blend mie start for atmospheric scattering
     * in Vibrant Visuals
     *
     * @worldMutation
     *
     * @param blendMieStart
     * Either a number (range [0,1.2]) or a set of keyframes. The
     * keyframes are composed of key value pairs. The key is a
     * number (range [0,1]) to signify a time of day (0.0 and 1.0
     * are noon, 0.25 is sunset, 0.5 is midnight, and 0.75 is
     * sunrise). The value is also a number (range [0,1.2])
     * @throws This function can throw errors.
     *
     * {@link InvalidArgumentError}
     */
    setHorizonBlendMieStart(blendMieStart: number | Record<number, number>): void;
    /**
     * @remarks
     * Sets the horizon blend min for atmospheric scattering in
     * Vibrant Visuals
     *
     * @worldMutation
     *
     * @param blendMin
     * Either a number (range [0,1]) or a set of keyframes. The
     * keyframes are composed of key value pairs. The key is a
     * number (range [0,1]) to signify a time of day (0.0 and 1.0
     * are noon, 0.25 is sunset, 0.5 is midnight, and 0.75 is
     * sunrise). The value is also a number (range [0,1])
     * @throws This function can throw errors.
     *
     * {@link InvalidArgumentError}
     */
    setHorizonBlendMin(blendMin: number | Record<number, number>): void;
    /**
     * @remarks
     * Sets the horizon blend start for atmospheric scattering in
     * Vibrant Visuals
     *
     * @worldMutation
     *
     * @param blendStart
     * Either a number (range [0,1]) or a set of keyframes. The
     * keyframes are composed of key value pairs. The key is a
     * number (range [0,1]) to signify a time of day (0.0 and 1.0
     * are noon, 0.25 is sunset, 0.5 is midnight, and 0.75 is
     * sunrise). The value is also a number (range [0,1])
     * @throws This function can throw errors.
     *
     * {@link InvalidArgumentError}
     */
    setHorizonBlendStart(blendStart: number | Record<number, number>): void;
    /**
     * @remarks
     * Sets the moon mie strength for atmospheric scattering in
     * Vibrant Visuals
     *
     * @worldMutation
     *
     * @param moonMieStrength
     * Either a number (range [0,60]) or a set of keyframes. The
     * keyframes are composed of key value pairs. The key is a
     * number (range [0,1]) to signify a time of day (0.0 and 1.0
     * are noon, 0.25 is sunset, 0.5 is midnight, and 0.75 is
     * sunrise). The value is also a number (range [0,60])
     * @throws This function can throw errors.
     *
     * {@link InvalidArgumentError}
     */
    setMoonMieStrength(moonMieStrength: number | Record<number, number>): void;
    /**
     * @remarks
     * Sets the rayleigh strength for atmospheric scattering in
     * Vibrant Visuals
     *
     * @worldMutation
     *
     * @param rayleighStrength
     * Either a number (range [0,11]) or a set of keyframes. The
     * keyframes are composed of key value pairs. The key is a
     * number (range [0,1]) to signify a time of day (0.0 and 1.0
     * are noon, 0.25 is sunset, 0.5 is midnight, and 0.75 is
     * sunrise). The value is also a number (range [0,11])
     * @throws This function can throw errors.
     *
     * {@link InvalidArgumentError}
     */
    setRayleighStrength(rayleighStrength: number | Record<number, number>): void;
    /**
     * @remarks
     * Sets the sky horizon color for atmospheric scattering in
     * Vibrant Visuals
     *
     * @worldMutation
     *
     * @param color
     * Either a RGB triplet or a set of keyframes. The keyframes
     * are composed of key value pairs. The key is a number (range
     * [0,1]) to signify a time of day (0.0 and 1.0 are noon, 0.25
     * is sunset, 0.5 is midnight, and 0.75 is sunrise). The value
     * is a RGB triplet
     * @throws This function can throw errors.
     *
     * {@link InvalidArgumentError}
     */
    setSkyHorizonColor(color: Record<number, RGB> | RGB): void;
    /**
     * @remarks
     * Sets the sky zenith color for atmospheric scattering in
     * Vibrant Visuals
     *
     * @worldMutation
     *
     * @param color
     * Either a RGB triplet or a set of keyframes. The keyframes
     * are composed of key value pairs. The key is a float in the
     * range 0-1 to signify a time of day and the value is a RGB
     * triplet
     * @throws This function can throw errors.
     *
     * {@link InvalidArgumentError}
     */
    setSkyZenithColor(color: Record<number, RGB> | RGB): void;
    /**
     * @remarks
     * Sets the sun glare shape for atmospheric scattering in
     * Vibrant Visuals
     *
     * @worldMutation
     *
     * @param sunGlareShape
     * Either a number (range [0,50]) or a set of keyframes. The
     * keyframes are composed of key value pairs. The key is a
     * number (range [0,1]) to signify a time of day (0.0 and 1.0
     * are noon, 0.25 is sunset, 0.5 is midnight, and 0.75 is
     * sunrise). The value is also a number (range [0,50])
     * @throws This function can throw errors.
     *
     * {@link InvalidArgumentError}
     */
    setSunGlareShape(sunGlareShape: number | Record<number, number>): void;
    /**
     * @remarks
     * Sets the sun mie strength for atmospheric scattering in
     * Vibrant Visuals
     *
     * @worldMutation
     *
     * @param sunMieStrength
     * Either a number (range [0,60]) or a set of keyframes. The
     * keyframes are composed of key value pairs. The key is a
     * number (range [0,1]) to signify a time of day (0.0 and 1.0
     * are noon, 0.25 is sunset, 0.5 is midnight, and 0.75 is
     * sunrise). The value is also a number (range [0,60])
     * @throws This function can throw errors.
     *
     * {@link InvalidArgumentError}
     */
    setSunMieStrength(sunMieStrength: number | Record<number, number>): void;
}

/**
 * Used to affect color grading for Vibrant Visuals
 */
export class BiomeColorGrading {
    private constructor();
    /**
     * @remarks
     * Resets the contrast of highlights to the value set by
     * resource packs
     *
     * @worldMutation
     *
     * @throws This function can throw errors.
     *
     * {@link EngineError}
     */
    resetHighlightsContrast(): void;
    /**
     * @remarks
     * Resets the gain of highlights to the value set by resource
     * packs
     *
     * @worldMutation
     *
     * @throws This function can throw errors.
     *
     * {@link EngineError}
     */
    resetHighlightsGain(): void;
    /**
     * @remarks
     * Resets the gamma of highlights to the value set by resource
     * packs
     *
     * @worldMutation
     *
     * @throws This function can throw errors.
     *
     * {@link EngineError}
     */
    resetHighlightsGamma(): void;
    /**
     * @remarks
     * Resets the highlights min to the value set by resource packs
     *
     * @worldMutation
     *
     * @throws This function can throw errors.
     *
     * {@link EngineError}
     */
    resetHighlightsMin(): void;
    /**
     * @remarks
     * Resets the offset of highlights to the value set by resource
     * packs
     *
     * @worldMutation
     *
     * @throws This function can throw errors.
     *
     * {@link EngineError}
     */
    resetHighlightsOffset(): void;
    /**
     * @remarks
     * Resets the saturation of highlights to the value set by
     * resource packs
     *
     * @worldMutation
     *
     * @throws This function can throw errors.
     *
     * {@link EngineError}
     */
    resetHighlightsSaturation(): void;
    /**
     * @remarks
     * Resets the contrast of midtones to the value set by resource
     * packs
     *
     * @worldMutation
     *
     * @throws This function can throw errors.
     *
     * {@link EngineError}
     */
    resetMidtonesContrast(): void;
    /**
     * @remarks
     * Resets the gain of midtones to the value set by resource
     * packs
     *
     * @worldMutation
     *
     * @throws This function can throw errors.
     *
     * {@link EngineError}
     */
    resetMidtonesGain(): void;
    /**
     * @remarks
     * Resets the gamma of midtones to the value set by resource
     * packs
     *
     * @worldMutation
     *
     * @throws This function can throw errors.
     *
     * {@link EngineError}
     */
    resetMidtonesGamma(): void;
    /**
     * @remarks
     * Resets the offset of midtones to the value set by resource
     * packs
     *
     * @worldMutation
     *
     * @throws This function can throw errors.
     *
     * {@link EngineError}
     */
    resetMidtonesOffset(): void;
    /**
     * @remarks
     * Resets the saturation of midtones to the value set by
     * resource packs
     *
     * @worldMutation
     *
     * @throws This function can throw errors.
     *
     * {@link EngineError}
     */
    resetMidtonesSaturation(): void;
    /**
     * @remarks
     * Resets the contrast of shadows to the value set by resource
     * packs
     *
     * @worldMutation
     *
     * @throws This function can throw errors.
     *
     * {@link EngineError}
     */
    resetShadowsContrast(): void;
    /**
     * @remarks
     * Resets the gain of shadows to the value set by resource
     * packs
     *
     * @worldMutation
     *
     * @throws This function can throw errors.
     *
     * {@link EngineError}
     */
    resetShadowsGain(): void;
    /**
     * @remarks
     * Resets the gamma of shadows to the value set by resource
     * packs
     *
     * @worldMutation
     *
     * @throws This function can throw errors.
     *
     * {@link EngineError}
     */
    resetShadowsGamma(): void;
    /**
     * @remarks
     * Resets the shadows max to the value set by resource packs
     *
     * @worldMutation
     *
     * @throws This function can throw errors.
     *
     * {@link EngineError}
     */
    resetShadowsMax(): void;
    /**
     * @remarks
     * Resets the offset of shadows to the value set by resource
     * packs
     *
     * @worldMutation
     *
     * @throws This function can throw errors.
     *
     * {@link EngineError}
     */
    resetShadowsOffset(): void;
    /**
     * @remarks
     * Resets the saturation of shadows to the value set by
     * resource packs
     *
     * @worldMutation
     *
     * @throws This function can throw errors.
     *
     * {@link EngineError}
     */
    resetShadowsSaturation(): void;
    /**
     * @remarks
     * Resets the temperature to the value set by resource packs
     *
     * @worldMutation
     *
     * @throws This function can throw errors.
     *
     * {@link EngineError}
     */
    resetTemperature(): void;
    /**
     * @remarks
     * Sets the contrast of highlights for color grading in Vibrant
     * Visuals
     *
     * @worldMutation
     *
     * @param highlightsContrast
     * A Vector3 (range [0.0f, 4.0f]). Used to set the contrast of
     * highlights. Describes the tonal range, the difference in
     * luminance between the bright and dark pixels in an image. An
     * image with high contrast will have pixels with a wide range
     * of luminance values, whereas an image with low contrast will
     * have pixels of a relatively small luminance range. A value
     * of 1.0 results in no change in contrast to the original
     * image. A value of 0.0 results in a completely washed-out,
     * gray image. Values > 1.0 increase the brightness of
     * highlights while darkening the shadows in the final image.
     * @throws This function can throw errors.
     *
     * {@link InvalidArgumentError}
     */
    setHighlightsContrast(highlightsContrast: Vector3): void;
    /**
     * @remarks
     * Sets the gain of highlights for color grading in Vibrant
     * Visuals
     *
     * @worldMutation
     *
     * @param highlightsGain
     * A Vector3 (range [0.0f, 10.0f]). Used to set the gain of
     * highlights. A multiplication factor applied to each color
     * channel to adjust the overall luminance intensity of the
     * highlight range. A value of 1.0 results in no change to the
     * original image. Values < 1.0 darken the image while values >
     * 1.0 brighten it. A value of 0.0 cancels out the color
     * channel completely. Gain is multiplicative and therefore has
     * a stronger effect on brighter pixels than darker pixels.
     * @throws This function can throw errors.
     *
     * {@link InvalidArgumentError}
     */
    setHighlightsGain(highlightsGain: Vector3): void;
    /**
     * @remarks
     * Sets the gamma of highlights for color grading in Vibrant
     * Visuals
     *
     * @worldMutation
     *
     * @param highlightsGamma
     * A Vector3 (range [0.0f, 4.0f]). Used to set the gamma of
     * highlights. An exponential factor applied to the final color
     * after both color grading and tone mapping to adjust the
     * overall luminance intensity of the image. The standard value
     * for gamma is 2.2. Lower values darken the final image,
     * whereas higher values brighten it. Too high a gamma will
     * cause the final image to appear washed-out.
     * @throws This function can throw errors.
     *
     * {@link InvalidArgumentError}
     */
    setHighlightsGamma(highlightsGamma: Vector3): void;
    /**
     * @remarks
     * Sets the highlights min for color grading in Vibrant Visuals
     *
     * @worldMutation
     *
     * @param highlightsMin
     * A number (range [1.0f, 20.0f]). Used to set the highlights
     * min. A factor multiplied by the average luminance of the
     * scene to determine which pixels are considered highlights.
     * Pixels with luminance greater than HighlightsMin *
     * AverageLuminance will have the highlights set of color
     * grading values applied. A value of 1.0 indicates highlights
     * occupy the entire range of values including and above the
     * average luminance. Higher values will cause the minimum
     * required luminance value for a pixel to be considered a
     * highlight to rise. This value should not be equal to
     * ShadowsMax.
     * @throws This function can throw errors.
     *
     * {@link InvalidArgumentError}
     */
    setHighlightsMin(highlightsMin: number): void;
    /**
     * @remarks
     * Sets the offset of highlights for color grading in Vibrant
     * Visuals
     *
     * @worldMutation
     *
     * @param highlightsOffset
     * A Vector3 (range [-1.0f, 1.0f]). Used to set the offset of
     * highlights. An additive factor that is multiplied by the
     * average luminance of the scene and then added to a given
     * color channel to adjust the overall luminance intensity of
     * the image. A value of 0.0 results in no change. Values > 0.0
     * brighten the image, values < 0.0 darken it. Offset is
     * additive and therefore has a stronger effect on darker
     * pixels than brighter pixels.
     * @throws This function can throw errors.
     *
     * {@link InvalidArgumentError}
     */
    setHighlightsOffset(highlightsOffset: Vector3): void;
    /**
     * @remarks
     * Sets the saturation of highlights for color grading in
     * Vibrant Visuals
     *
     * @worldMutation
     *
     * @param highlightsSaturation
     * A Vector3 (range [0.0f, 10.0f]). Used to set the saturation
     * of highlights. Determines the hue intensity of colors. A
     * value of 1.0 results in no change in saturation to the
     * original image. A value of 0.0 results in a grayscale image.
     * Values > 1.0 increase the intensity of colors.
     * @throws This function can throw errors.
     *
     * {@link InvalidArgumentError}
     */
    setHighlightsSaturation(highlightsSaturation: Vector3): void;
    /**
     * @remarks
     * Sets the contrast of midtones for color grading in Vibrant
     * Visuals
     *
     * @worldMutation
     *
     * @param midtonesContrast
     * A Vector3 (range [0.0f, 4.0f]). Used to set the contrast of
     * midtones. Describes the tonal range, the difference in
     * luminance between the bright and dark pixels in an image. An
     * image with high contrast will have pixels with a wide range
     * of luminance values, whereas an image with low contrast will
     * have pixels of a relatively small luminance range. A value
     * of 1.0 results in no change in contrast to the original
     * image. A value of 0.0 results in a completely washed-out,
     * gray image. Values > 1.0 increase the brightness of
     * highlights while darkening the shadows in the final image.
     * @throws This function can throw errors.
     *
     * {@link InvalidArgumentError}
     */
    setMidtonesContrast(midtonesContrast: Vector3): void;
    /**
     * @remarks
     * Sets the gain of midtones for color grading in Vibrant
     * Visuals
     *
     * @worldMutation
     *
     * @param midtonesGain
     * A Vector3 (range [0.0f, 10.0f]). Used to set the gain of
     * midtones. A multiplication factor applied to each color
     * channel to adjust the overall luminance intensity of the
     * midtone range. A value of 1.0 results in no change to the
     * original image. Values < 1.0 darken the image while values >
     * 1.0 brighten it. A value of 0.0 cancels out the color
     * channel completely. Gain is multiplicative and therefore has
     * a stronger effect on brighter pixels than darker pixels.
     * @throws This function can throw errors.
     *
     * {@link InvalidArgumentError}
     */
    setMidtonesGain(midtonesGain: Vector3): void;
    /**
     * @remarks
     * Sets the gamma of midtones for color grading in Vibrant
     * Visuals
     *
     * @worldMutation
     *
     * @param midtonesGamma
     * A Vector3 (range [0.0f, 4.0f]). Used to set the gamma of
     * midtones. An exponential factor applied to the final color
     * after both color grading and tone mapping to adjust the
     * overall luminance intensity of the image. The standard value
     * for gamma is 2.2. Lower values darken the final image,
     * whereas higher values brighten it. Too high a gamma will
     * cause the final image to appear washed-out.
     * @throws This function can throw errors.
     *
     * {@link InvalidArgumentError}
     */
    setMidtonesGamma(midtonesGamma: Vector3): void;
    /**
     * @remarks
     * Sets the offset of midtones for color grading in Vibrant
     * Visuals
     *
     * @worldMutation
     *
     * @param midtonesOffset
     * A Vector3 (range [-1.0f, 1.0f]). Used to set the offset of
     * midtones. An additive factor that is multiplied by the
     * average luminance of the scene and then added to a given
     * color channel to adjust the overall luminance intensity of
     * the image. A value of 0.0 results in no change. Values > 0.0
     * brighten the image, values < 0.0 darken it. Offset is
     * additive and therefore has a stronger effect on darker
     * pixels than brighter pixels.
     * @throws This function can throw errors.
     *
     * {@link InvalidArgumentError}
     */
    setMidtonesOffset(midtonesOffset: Vector3): void;
    /**
     * @remarks
     * Sets the saturation of midtones for color grading in Vibrant
     * Visuals
     *
     * @worldMutation
     *
     * @param midtonesSaturation
     * A Vector3 (range [0.0f, 10.0f]). Used to set the saturation
     * of midtones. Determines the hue intensity of colors. A value
     * of 1.0 results in no change in saturation to the original
     * image. A value of 0.0 results in a grayscale image. Values >
     * 1.0 increase the intensity of colors.
     * @throws This function can throw errors.
     *
     * {@link InvalidArgumentError}
     */
    setMidtonesSaturation(midtonesSaturation: Vector3): void;
    /**
     * @remarks
     * Sets the contrast of shadows for color grading in Vibrant
     * Visuals
     *
     * @worldMutation
     *
     * @param shadowsContrast
     * A Vector3 (range [0.0f, 4.0f]). Used to set the contrast of
     * shadows. Describes the tonal range, the difference in
     * luminance between the bright and dark pixels in an image. An
     * image with high contrast will have pixels with a wide range
     * of luminance values, whereas an image with low contrast will
     * have pixels of a relatively small luminance range. A value
     * of 1.0 results in no change in contrast to the original
     * image. A value of 0.0 results in a completely washed-out,
     * gray image. Values > 1.0 increase the brightness of
     * highlights while darkening the shadows in the final image.
     * @throws This function can throw errors.
     *
     * {@link InvalidArgumentError}
     */
    setShadowsContrast(shadowsContrast: Vector3): void;
    /**
     * @remarks
     * Sets the gain of shadows for color grading in Vibrant
     * Visuals
     *
     * @worldMutation
     *
     * @param shadowsGain
     * A Vector3 (range [0.0f, 10.0f]). Used to set the gain of
     * shadows. A multiplication factor applied to each color
     * channel to adjust the overall luminance intensity of the
     * shadow range. A value of 1.0 results in no change to the
     * original image. Values < 1.0 darken the image while values >
     * 1.0 brighten it. A value of 0.0 cancels out the color
     * channel completely. Gain is multiplicative and therefore has
     * a stronger effect on brighter pixels than darker pixels.
     * @throws This function can throw errors.
     *
     * {@link InvalidArgumentError}
     */
    setShadowsGain(shadowsGain: Vector3): void;
    /**
     * @remarks
     * Sets the gamma of shadows for color grading in Vibrant
     * Visuals
     *
     * @worldMutation
     *
     * @param shadowsGamma
     * A Vector3 (range [0.0f, 4.0f]). Used to set the gamma of
     * shadows. An exponential factor applied to the final color
     * after both color grading and tone mapping to adjust the
     * overall luminance intensity of the image. The standard value
     * for gamma is 2.2. Lower values darken the final image,
     * whereas higher values brighten it. Too high a gamma will
     * cause the final image to appear washed-out.
     * @throws This function can throw errors.
     *
     * {@link InvalidArgumentError}
     */
    setShadowsGamma(shadowsGamma: Vector3): void;
    /**
     * @remarks
     * Sets the shadows max for color grading in Vibrant Visuals
     *
     * @worldMutation
     *
     * @param shadowsMax
     * A number (range [0.0f, 1.0f]). Used to set the shadows max.
     * A factor multiplied by the average luminance of the scene to
     * determine which pixels are considered shadows. Pixels with
     * luminance less than ShadowsMax * AverageLuminance will have
     * the shadows set of color grading values applied. A value of
     * 1.0 indicates shadows occupy the entire range of values
     * including and up to the average luminance. Lower values will
     * cause the maximum required luminance value for a pixel to be
     * considered a shadow to drop. This value should not be equal
     * to HighlightsMin.
     * @throws This function can throw errors.
     *
     * {@link InvalidArgumentError}
     */
    setShadowsMax(shadowsMax: number): void;
    /**
     * @remarks
     * Sets the offset of shadows for color grading in Vibrant
     * Visuals
     *
     * @worldMutation
     *
     * @param shadowsOffset
     * A Vector3 (range [-1.0f, 1.0f]). Used to set the offset of
     * shadows. An additive factor that is multiplied by the
     * average luminance of the scene and then added to a given
     * color channel to adjust the overall luminance intensity of
     * the image. A value of 0.0 results in no change. Values > 0.0
     * brighten the image, values < 0.0 darken it. Offset is
     * additive and therefore has a stronger effect on darker
     * pixels than brighter pixels.
     * @throws This function can throw errors.
     *
     * {@link InvalidArgumentError}
     */
    setShadowsOffset(shadowsOffset: Vector3): void;
    /**
     * @remarks
     * Sets the saturation of shadows for color grading in Vibrant
     * Visuals
     *
     * @worldMutation
     *
     * @param shadowsSaturation
     * A Vector3 (range [0.0f, 10.0f]). Used to set the saturation
     * of shadows. Determines the hue intensity of colors. A value
     * of 1.0 results in no change in saturation to the original
     * image. A value of 0.0 results in a grayscale image. Values >
     * 1.0 increase the intensity of colors.
     * @throws This function can throw errors.
     *
     * {@link InvalidArgumentError}
     */
    setShadowsSaturation(shadowsSaturation: Vector3): void;
    /**
     * @remarks
     * Sets the temperature for color grading in Vibrant Visuals
     *
     * @worldMutation
     *
     * @param temperature
     * A number (range [1000.0f, 15000.0f]). Used to set the
     * temperature. The overall image temperature measured in
     * Kelvin. The default value is 6500.0, the standard "daylight"
     * illumination.
     * @throws This function can throw errors.
     *
     * {@link InvalidArgumentError}
     */
    setTemperature(temperature: number): void;
}

/**
 * Used to affect lighting for Vibrant Visuals
 */
export class BiomeLighting {
    private constructor();
    /**
     * @remarks
     * Resets the ambient color to the value set by resource packs
     *
     * @worldMutation
     *
     * @throws This function can throw errors.
     *
     * {@link EngineError}
     */
    resetAmbientColor(): void;
    /**
     * @remarks
     * Resets the ambient illuminance to the value set by resource
     * packs
     *
     * @worldMutation
     *
     * @throws This function can throw errors.
     *
     * {@link EngineError}
     */
    resetAmbientIlluminance(): void;
    /**
     * @remarks
     * Resets the emissive desaturation to the value set by
     * resource packs
     *
     * @worldMutation
     *
     * @throws This function can throw errors.
     *
     * {@link EngineError}
     */
    resetEmissiveDesaturation(): void;
    /**
     * @remarks
     * Resets the flash color to the value set by resource packs
     *
     * @worldMutation
     *
     * @throws This function can throw errors.
     *
     * {@link EngineError}
     */
    resetFlashColor(): void;
    /**
     * @remarks
     * Resets the flash illuminance to the value set by resource
     * packs
     *
     * @worldMutation
     *
     * @throws This function can throw errors.
     *
     * {@link EngineError}
     */
    resetFlashIlluminance(): void;
    /**
     * @remarks
     * Resets the moon color to the value set by resource packs
     *
     * @worldMutation
     *
     * @throws This function can throw errors.
     *
     * {@link EngineError}
     */
    resetMoonColor(): void;
    /**
     * @remarks
     * Resets the moon illuminance to the value set by resource
     * packs
     *
     * @worldMutation
     *
     * @throws This function can throw errors.
     *
     * {@link EngineError}
     */
    resetMoonIlluminance(): void;
    /**
     * @remarks
     * Resets the orbital offset to the value set by resource packs
     *
     * @worldMutation
     *
     * @throws This function can throw errors.
     *
     * {@link EngineError}
     */
    resetOrbitalOffsetDegrees(): void;
    /**
     * @remarks
     * Resets the sky intensity to the value set by resource packs
     *
     * @worldMutation
     *
     * @throws This function can throw errors.
     *
     * {@link EngineError}
     */
    resetSkyIntensity(): void;
    /**
     * @remarks
     * Resets the sun color to the value set by resource packs
     *
     * @worldMutation
     *
     * @throws This function can throw errors.
     *
     * {@link EngineError}
     */
    resetSunColor(): void;
    /**
     * @remarks
     * Resets the sun illuminance to the value set by resource
     * packs
     *
     * @worldMutation
     *
     * @throws This function can throw errors.
     *
     * {@link EngineError}
     */
    resetSunIlluminance(): void;
    /**
     * @remarks
     * Sets the ambient color for lighting in Vibrant Visuals
     *
     * @worldMutation
     *
     * @throws This function can throw errors.
     *
     * {@link InvalidArgumentError}
     */
    setAmbientColor(color: Record<number, RGB> | RGB): void;
    /**
     * @remarks
     * Sets the ambient illuminance for lighting in Vibrant Visuals
     *
     * @worldMutation
     *
     * @throws This function can throw errors.
     *
     * {@link InvalidArgumentError}
     */
    setAmbientIlluminance(illuminance: number | Record<number, number>): void;
    /**
     * @remarks
     * Sets the emissive desaturation for lighting in Vibrant
     * Visuals
     *
     * @worldMutation
     *
     * @throws This function can throw errors.
     *
     * {@link InvalidArgumentError}
     */
    setEmissiveDesaturation(value: number): void;
    /**
     * @remarks
     * Sets the flash color for lighting in Vibrant Visuals
     *
     * @worldMutation
     *
     * @throws This function can throw errors.
     *
     * {@link InvalidArgumentError}
     */
    setFlashColor(color: Record<number, RGB> | RGB): void;
    /**
     * @remarks
     * Sets the flash illuminance for lighting in Vibrant Visuals
     *
     * @worldMutation
     *
     * @throws This function can throw errors.
     *
     * {@link InvalidArgumentError}
     */
    setFlashIlluminance(illuminance: number | Record<number, number>): void;
    /**
     * @remarks
     * Sets the moon color for lighting in Vibrant Visuals
     *
     * @worldMutation
     *
     * @throws This function can throw errors.
     *
     * {@link InvalidArgumentError}
     */
    setMoonColor(color: Record<number, RGB> | RGB): void;
    /**
     * @remarks
     * Sets the moon illuminance for lighting in Vibrant Visuals
     *
     * @worldMutation
     *
     * @throws This function can throw errors.
     *
     * {@link InvalidArgumentError}
     */
    setMoonIlluminance(illuminance: number | Record<number, number>): void;
    /**
     * @remarks
     * Sets the orbital offset for lighting in Vibrant Visuals
     *
     * @worldMutation
     *
     * @throws This function can throw errors.
     *
     * {@link InvalidArgumentError}
     */
    setOrbitalOffsetDegrees(degrees: number | Record<number, number>): void;
    /**
     * @remarks
     * Sets the sky intensity for lighting in Vibrant Visuals
     *
     * @worldMutation
     *
     * @throws This function can throw errors.
     *
     * {@link InvalidArgumentError}
     */
    setSkyIntensity(intensity: number | Record<number, number>): void;
    /**
     * @remarks
     * Sets the sun color for lighting in Vibrant Visuals
     *
     * @worldMutation
     *
     * @throws This function can throw errors.
     *
     * {@link InvalidArgumentError}
     */
    setSunColor(color: Record<number, RGB> | RGB): void;
    /**
     * @remarks
     * Sets the sun illuminance for lighting in Vibrant Visuals
     *
     * @worldMutation
     *
     * @throws This function can throw errors.
     *
     * {@link InvalidArgumentError}
     */
    setSunIlluminance(illuminance: number | Record<number, number>): void;
}

/**
 * Used to affect water for Vibrant Visuals
 */
export class BiomeWater {
    private constructor();
    /**
     * @remarks
     * Resets the CDOM  to the value set by resource packs
     *
     * @worldMutation
     *
     * @throws This function can throw errors.
     *
     * {@link EngineError}
     */
    resetCDOM(): void;
    /**
     * @remarks
     * Resets the chlorophyll concentration to the value set by
     * resource packs
     *
     * @worldMutation
     *
     * @throws This function can throw errors.
     *
     * {@link EngineError}
     */
    resetChlorophyll(): void;
    /**
     * @remarks
     * Resets the suspended sediment  to the value set by resource
     * packs
     *
     * @worldMutation
     *
     * @throws This function can throw errors.
     *
     * {@link EngineError}
     */
    resetSuspendedSediment(): void;
    /**
     * @remarks
     * Resets the wave depth to the value set by resource packs
     *
     * @worldMutation
     *
     * @throws This function can throw errors.
     *
     * {@link EngineError}
     */
    resetWavesDepth(): void;
    /**
     * @remarks
     * Resets the wave direction increment to the value set by
     * resource packs
     *
     * @worldMutation
     *
     * @throws This function can throw errors.
     *
     * {@link EngineError}
     */
    resetWavesDirectionIncrement(): void;
    /**
     * @remarks
     * Resets the wave frequency to the value set by resource packs
     *
     * @worldMutation
     *
     * @throws This function can throw errors.
     *
     * {@link EngineError}
     */
    resetWavesFrequency(): void;
    /**
     * @remarks
     * Resets the wave frequency scaling to the value set by
     * resource packs
     *
     * @worldMutation
     *
     * @throws This function can throw errors.
     *
     * {@link EngineError}
     */
    resetWavesFrequencyScaling(): void;
    /**
     * @remarks
     * Resets the wave mix to the value set by resource packs
     *
     * @worldMutation
     *
     * @throws This function can throw errors.
     *
     * {@link EngineError}
     */
    resetWavesMix(): void;
    /**
     * @remarks
     * Resets the wave octaves to the value set by resource packs
     *
     * @worldMutation
     *
     * @throws This function can throw errors.
     *
     * {@link EngineError}
     */
    resetWavesOctaves(): void;
    /**
     * @remarks
     * Resets the wave pull to the value set by resource packs
     *
     * @worldMutation
     *
     * @throws This function can throw errors.
     *
     * {@link EngineError}
     */
    resetWavesPull(): void;
    /**
     * @remarks
     * Resets the wave shape to the value set by resource packs
     *
     * @worldMutation
     *
     * @throws This function can throw errors.
     *
     * {@link EngineError}
     */
    resetWavesShape(): void;
    /**
     * @remarks
     * Resets the wave speed to the value set by resource packs
     *
     * @worldMutation
     *
     * @throws This function can throw errors.
     *
     * {@link EngineError}
     */
    resetWavesSpeed(): void;
    /**
     * @remarks
     * Resets the wave speed scaling to the value set by resource
     * packs
     *
     * @worldMutation
     *
     * @throws This function can throw errors.
     *
     * {@link EngineError}
     */
    resetWavesSpeedScaling(): void;
    /**
     * @remarks
     * Sets the CDOM for water in Vibrant Visuals
     *
     * @worldMutation
     *
     * @param cdom
     * A number (range [0,15]). Used to set the CDOM. High
     * concentrations produce yellow to yellow-brown colors, due to
     * CDOM strongly absorbing blue wavelengths. Open oceans
     * typically have little to no CDOM, and thus retain a blue
     * appearance; fresh water sources, like rivers, tend to have
     * higher concentrations.
     * @throws This function can throw errors.
     *
     * {@link InvalidArgumentError}
     */
    setCDOM(cdom: number): void;
    /**
     * @remarks
     * Sets the chlorophyll concentration for water in Vibrant
     * Visuals
     *
     * @worldMutation
     *
     * @param chlorophyll
     * A number (range [0,10]). Used to set the chlorophyll
     * concentration. High concentrations produce green colors, due
     * to chlorophyll strongly absorbing blue and red wavelengths.
     * @throws This function can throw errors.
     *
     * {@link InvalidArgumentError}
     */
    setChlorophyll(chlorophyll: number): void;
    /**
     * @remarks
     * Sets the suspended sediment for water in Vibrant Visuals
     *
     * @worldMutation
     *
     * @param suspendedSediment
     * A number (range [0,300]). Used to set the suspended
     * sediment.  High concentrations produce red to red-brown
     * colors, due to suspended sediment strongly absorbing blue
     * and green wavelengths. Suspended sediment, like clay and
     * silt, tend to be concentrated in rivers and can indicate
     * recent floods or sources of pollution.
     * @throws This function can throw errors.
     *
     * {@link InvalidArgumentError}
     */
    setSuspendedSediment(suspendedSediment: number): void;
    /**
     * @remarks
     * Sets the wave depth for water in Vibrant Visuals
     *
     * @worldMutation
     *
     * @param wavesDepth
     * A number (range [0,10]). Used to set the wave depth.
     * Determines how much waves displace the water surface. Larger
     * values will result in deeper waves, whereas smaller values
     * will produce shallower waves.
     * @throws This function can throw errors.
     *
     * {@link InvalidArgumentError}
     */
    setWavesDepth(wavesDepth: number): void;
    /**
     * @remarks
     * Sets the wave direction increment for water in Vibrant
     * Visuals
     *
     * @worldMutation
     *
     * @param wavesDirectionIncrement
     * A number (range [0.0f, 360.0f]). Used to set the wave
     * direction increment. An angle, in degrees, that controls how
     * much the heading changes between each octave.
     * @throws This function can throw errors.
     *
     * {@link InvalidArgumentError}
     */
    setWavesDirectionIncrement(wavesDirectionIncrement: number): void;
    /**
     * @remarks
     * Sets the wave frequency for water in Vibrant Visuals
     *
     * @worldMutation
     *
     * @param wavesFrequency
     * A number (range [0,10]). Used to set the wave frequency.
     * Determines how many waves there are per water block. Can
     * also be thought of as the size of the waves. Larger values
     * will create more tightly packed waves, whereas smaller
     * values will spread waves out over a wider area.
     * @throws This function can throw errors.
     *
     * {@link InvalidArgumentError}
     */
    setWavesFrequency(wavesFrequency: number): void;
    /**
     * @remarks
     * Sets the wave frequency scaling for water in Vibrant Visuals
     *
     * @worldMutation
     *
     * @param wavesFrequencyScaling
     * A number (range [0,2]). Used to set the wave frequency
     * scaling. Specifies how much wave frequency changes between
     * octaves. A value of 1 will result in no change between
     * octaves. Values higher than 1 will cause frequencies to
     * increase while values less than 1 will cause frequencies to
     * decrease.
     * @throws This function can throw errors.
     *
     * {@link InvalidArgumentError}
     */
    setWavesFrequencyScaling(wavesFrequencyScaling: number): void;
    /**
     * @remarks
     * Sets the wave mix for water in Vibrant Visuals
     *
     * @worldMutation
     *
     * @param wavesMix
     * A number (range [0.0, 1.0]). Used to set the wave mix.
     * Controls how much each octave is blended into the
     * neighboring octave.
     * @throws This function can throw errors.
     *
     * {@link InvalidArgumentError}
     */
    setWavesMix(wavesMix: number): void;
    /**
     * @remarks
     * Sets the wave octaves for water in Vibrant Visuals
     *
     * @worldMutation
     *
     * @param wavesOctaves
     * A number (range [1.0, 10]). Used to set the wave octaves.
     * Determines how many layers of waves to simulate; high values
     * result in more complex waves
     * @throws This function can throw errors.
     *
     * {@link InvalidArgumentError}
     */
    setWavesOctaves(wavesOctaves: number): void;
    /**
     * @remarks
     * Sets the wave pull for water in Vibrant Visuals
     *
     * @worldMutation
     *
     * @param wavesPull
     * A number (range [-1.0f, 1.0f]). Used to set the wave pull.
     * Controls how much smaller waves are pulled into larger
     * waves. A value of 0 results in no pull. Values larger than 0
     * will pull waves in a standard concave fashion, whereas
     * values less than 0 will pull waves in a convex fashion,
     * resulting in more pillowing waves as opposed to cresting
     * waves.
     * @throws This function can throw errors.
     *
     * {@link InvalidArgumentError}
     */
    setWavesPull(wavesPull: number): void;
    /**
     * @remarks
     * Sets the wave shape for water in Vibrant Visuals
     *
     * @worldMutation
     *
     * @param wavesShape
     * A number (range [1.0, 10]). Used to set the wave shape.
     * Adjusts the core shape of waves. A value of 1 results in a
     * pure sine wave, whereas values larger than 1 will produce
     * sharper waves.
     * @throws This function can throw errors.
     *
     * {@link InvalidArgumentError}
     */
    setWavesShape(wavesShape: number): void;
    /**
     * @remarks
     * Sets the wave speed for water in Vibrant Visuals
     *
     * @worldMutation
     *
     * @param wavesSpeed
     * A number (range [0.01,10]). Used to set the wave speed.
     * etermines the movement speed of the first wave and the
     * starting value of the Speed Scaling parameter.
     * @throws This function can throw errors.
     *
     * {@link InvalidArgumentError}
     */
    setWavesSpeed(wavesSpeed: number): void;
    /**
     * @remarks
     * Sets the wave speed scaling for water in Vibrant Visuals
     *
     * @worldMutation
     *
     * @param wavesSpeedScaling
     * A number (range [0.0,2]). Used to set the wave speed
     * scaling. Controls how much faster each subsequent octave
     * moves. A value of 1 will result in no change between
     * octaves. Values higher than 1 will cause speeds to increase
     * while values less than 1 will cause speeds to decrease.
     * @throws This function can throw errors.
     *
     * {@link InvalidArgumentError}
     */
    setWavesSpeedScaling(wavesSpeedScaling: number): void;
}

/**
 * @remarks
 * Retrieves the BiomeAtmospherics component to control
 * atmoshperic scattering for Vibrant Visuals.
 *
 * @worldMutation
 *
 */
export function getBiomeAtmospherics(biome: BiomeType): BiomeAtmospherics;
/**
 * @remarks
 * Retrieves the BiomeColorGrading component to control color
 * grading for Vibrant Visuals.
 *
 * @worldMutation
 *
 */
export function getBiomeColorGrading(biome: BiomeType): BiomeColorGrading;
/**
 * @remarks
 * Retrieves the BiomeLighting component to control lighting
 * for Vibrant Visuals.
 *
 * @worldMutation
 *
 */
export function getBiomeLighting(biome: BiomeType): BiomeLighting;
/**
 * @remarks
 * Retrieves the BiomeWater component to control water for
 * Vibrant Visuals
 *
 * @worldMutation
 *
 */
export function getBiomeWater(biome: BiomeType): BiomeWater;
/**
 * @remarks
 * Retrieves the PlayerAtmospherics component to control
 * atmospheric scattering for a particular player in Vibrant
 * Visuals. This offers the same controls as BiomeAtmospherics,
 * but PlayerAtmospherics controls will always take precedence
 * over BiomeAtmospherics.
 *
 * @worldMutation
 *
 */
export function getPlayerAtmospherics(
    biome: BiomeType,
    player: Player,
): BiomeAtmospherics;
/**
 * @remarks
 * Retrieves the PlayerColorGrading component to control color
 * grading for a particular player in Vibrant Visuals. This
 * offers the same controls as BiomeColorGrading, but
 * PlayerColorGrading controls will always take precedence over
 * BiomeColorGrading.
 *
 * @worldMutation
 *
 */
export function getPlayerColorGrading(
    biome: BiomeType,
    player: Player,
): BiomeColorGrading;
/**
 * @remarks
 * Retrieves the PlayerLighting component to control lighting
 * for a particular player in Vibrant Visuals. This offers the
 * same controls as BiomeLighting, but PlayerLighting controls
 * will always take precedence over BiomeLighting.
 *
 * @worldMutation
 *
 */
export function getPlayerLighting(biome: BiomeType, player: Player): BiomeLighting;
/**
 * @remarks
 * Retrieves the PlayerWater component to control water for a
 * particular player in Vibrant Visuals. This offers the same
 * controls as BiomeWater, but PlayerWater controls will always
 * take precedence over BiomeWater.
 *
 * @worldMutation
 *
 */
export function getPlayerWater(biome: BiomeType, player: Player): BiomeWater;
