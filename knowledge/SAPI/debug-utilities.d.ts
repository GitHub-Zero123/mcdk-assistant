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
 * Contains debug utility functions.
 *
 * Manifest Details
 * ```json
 * {
 *   "module_name": "@minecraft/debug-utilities",
 *   "version": "1.0.0-beta"
 * }
 * ```
 *
 */
import { ArgumentOutOfBoundsError } from '@minecraft/common';
import { Dimension, DimensionLocation, Entity, Player, RGBA, RawMessage, RawMessageError, Vector2, Vector3 } from '@minecraft/server';
/**
 * The length of the arrow's head/tip.
 */
// @ts-ignore Class inheritance allowed for native defined classes
export class DebugArrow extends DebugLine {
    /**
     * @remarks
     * Adds a new debug shape to the world.
     *
     */
    headLength: number;
    /**
     * @remarks
     * The radius of the arrow's head/tip.
     *
     */
    headRadius: number;
    /**
     * @remarks
     * The number of segments for the base circle of the arrow's
     * head/tip (default: 4).
     *
     * Bounds: [3, 128]
     */
    headSegments: number;
    constructor(
        location: DimensionLocation | Vector3,
        endLocation: Vector3,
    );
}

/**
 * A debug shape class that represents a box or cuboid.
 */
// @ts-ignore Class inheritance allowed for native defined classes
export class DebugBox extends DebugShape {
    /**
     * @remarks
     * The bounding box of the shape. The final box will be this
     * bound multiplied by the shape's scale.
     *
     */
    bound: Vector3;
    constructor(location: DimensionLocation | Vector3);
}

/**
 * A debug shape class that represents a circle (2D).
 */
// @ts-ignore Class inheritance allowed for native defined classes
export class DebugCircle extends DebugShape {
    constructor(location: DimensionLocation | Vector3);
}

/**
 * A debug shape class that represents a cone.
 */
// @ts-ignore Class inheritance allowed for native defined classes
export class DebugCone extends DebugShape {
    /**
     * @remarks
     * The height of the cone.
     *
     */
    height: number;
    /**
     * @remarks
     * The number of segments used to approximate the circular base
     * of the cone. Bounds: [3, 128]
     *
     * Bounds: [3, 128]
     */
    numSegments: number;
    /**
     * @remarks
     * The radii of the cone's circular base (x: bottom radius, y:
     * top radius).
     *
     */
    radii: Vector2;
    constructor(location: DimensionLocation | Vector3);
}

/**
 * A debug shape class that represents a cylinder.
 */
// @ts-ignore Class inheritance allowed for native defined classes
export class DebugCylinder extends DebugShape {
    /**
     * @remarks
     * The height of the cylinder.
     *
     */
    height: number;
    /**
     * @remarks
     * The number of segments used to approximate the circular
     * cross-section of the cylinder. Bounds: [3, 128]
     *
     * Bounds: [3, 128]
     */
    numSegments: number;
    /**
     * @remarks
     * The radii of the cylinder's circular cross-section (x:
     * bottom radius, y: top radius).
     *
     */
    radii: Vector2;
    constructor(location: DimensionLocation | Vector3);
}

/**
 * Debug Drawing class used to allow adding and removing
 * wireframe shapes in world space.
 */
export class DebugDrawer {
    private constructor();
    /**
     * @remarks
     * Adds a new debug shape to the world.
     *
     * @param shape
     * The debug shape to be added. Should be of type DebugBox,
     * DebugLine, DebugCircle, DebugSphere, DebugArrow or
     * DebugText.
     */
    addShape(shape: DebugShape, dimension?: Dimension): void;
    /**
     * @remarks
     * Removes all debug shapes from the world.
     *
     */
    removeAll(): void;
    /**
     * @remarks
     * Removes an instance of a debug shape from the world. This is
     * equivalent to calling remove on the shape itself.
     *
     */
    removeShape(shape: DebugShape): void;
}

/**
 * A debug shape class that represents an ellipsoid.
 */
// @ts-ignore Class inheritance allowed for native defined classes
export class DebugEllipsoid extends DebugShape {
    /**
     * @remarks
     * The radii of the ellipsoid along each axis (x, y, z).
     *
     */
    radii: Vector3;
    /**
     * @remarks
     * The number of segments used to approximate the ellipsoid per
     * axis. Bounds: [3, 128]
     *
     * Bounds: [3, 128]
     */
    segmentsPerAxis: number;
    constructor(location: DimensionLocation | Vector3);
}

/**
 * A debug shape class that represents a line segment.
 */
// @ts-ignore Class inheritance allowed for native defined classes
export class DebugLine extends DebugShape {
    /**
     * @remarks
     * The end location of the line segment. The final line will
     * spawn between location and endLocation.
     *
     */
    endLocation: Vector3;
    constructor(
        location: DimensionLocation | Vector3,
        endLocation: Vector3,
    );
}

/**
 * A debug shape class that represents a pyramid.
 */
// @ts-ignore Class inheritance allowed for native defined classes
export class DebugPyramid extends DebugShape {
    /**
     * @remarks
     * The depth of the pyramid's base.
     *
     */
    depth?: number;
    /**
     * @remarks
     * The height of the pyramid.
     *
     */
    height: number;
    /**
     * @remarks
     * The width of the pyramid's base.
     *
     */
    width: number;
    constructor(location: DimensionLocation | Vector3);
}

/**
 * The base class for all debug shapes. Represents an object in
 * the world and its base properties.
 */
export class DebugShape {
    private constructor();
    /**
     * @remarks
     * The entity this shape is attached to. When set, this shape
     * will copy the root location of the attached entity and the
     * shape's position will be used as an offset.
     *
     */
    attachedTo?: Entity;
    /**
     * @remarks
     * The color of the shape.
     *
     */
    color: RGBA;
    /**
     * @remarks
     * The dimension the shape is visible within. If the dimension
     * is undefined, it will display in all dimensions.
     *
     */
    readonly dimension: Dimension;
    /**
     * @remarks
     * Returns true if the shape has a limited time span before
     * being removed.
     *
     */
    readonly hasDuration: boolean;
    /**
     * @remarks
     * The location of the shape. For most shapes the location is
     * the centre of the shape, except DebugLine and DebugArrow
     * where this represents the start point of the line.
     *
     */
    readonly location: Vector3;
    /**
     * @remarks
     * If defined, this distance will be used to determine how far
     * away this shape will be rendered for each client. By default
     * the distance will match the client's render distance
     * setting.
     *
     * Minimum Value: 0
     */
    maximumRenderDistance?: number;
    /**
     * @remarks
     * The rotation of the shape (Euler angles - [Pitch, Yaw,
     * Roll]).
     *
     */
    rotation: Vector3;
    /**
     * @remarks
     * The scale of the shape. This does not apply to DebugLine or
     * DebugArrow.
     *
     */
    scale: number;
    /**
     * @remarks
     * The time left (in seconds) until this shape is automatically
     * removed. Returns undefined if the shape does not have a
     * limited life-span.
     *
     */
    timeLeft?: number;
    /**
     * @remarks
     * The total initial time-span (in seconds) until this shape is
     * automatically removed. Returns undefined if the shape does
     * not have a limited life-span.
     *
     */
    readonly totalTimeLeft?: number;
    /**
     * @remarks
     * The list of players that this shape will be visible to. If
     * left empty, the shape will be visible to all players.
     *
     */
    visibleTo: Player[];
    /**
     * @remarks
     * Removes this shape from the world. The shape can be re-added
     * via the DebugDrawer's addShape method.
     *
     */
    remove(): void;
    /**
     * @remarks
     * Set the location and dimension of the shape. If the
     * dimension is undefined, it will display in all dimensions.
     * For most shapes the location is the centre of the shape,
     * except DebugLine and DebugArrow where this represents the
     * start point of the line.
     *
     */
    setLocation(location: DimensionLocation | Vector3): void;
}

/**
 * A debug shape class that represents a sphere.
 */
// @ts-ignore Class inheritance allowed for native defined classes
export class DebugSphere extends DebugShape {
    constructor(location: DimensionLocation | Vector3);
}

/**
 * A debug shape class that represents a text label in the
 * world with a background.
 */
// @ts-ignore Class inheritance allowed for native defined classes
export class DebugText extends DebugShape {
    /**
     * @remarks
     * If set to true, the debug text will render the back-face of
     * the background. Defaults to true but will always be false if
     * 'useRotation' is set to false.
     *
     */
    backfaceVisible: boolean;
    /**
     * @remarks
     * The color of the background plate of the text. If set to
     * undefined, it will use the default color.
     *
     */
    backgroundColorOverride?: RGBA;
    /**
     * @remarks
     * If set to true, the text will be hidden behind blocks or
     * entities. By default this is set to false (will always
     * render).
     *
     */
    depthTest: boolean;
    /**
     * @remarks
     * Get the text of the debug text shape. Returns the RawText of
     * the debug text if `setText` was called with a RawMessage or
     * a RawText object, otherwise returns a string.
     *
     */
    readonly text: RawMessage | string;
    /**
     * @remarks
     * If set to true, the debug text will render the back-face of
     * the text. Defaults to true but will always be false if
     * 'useRotation' is set to false.
     *
     */
    textBackfaceVisible: boolean;
    /**
     * @remarks
     * If set to true, the text will not face the camera and
     * instead will use the rotation from the shape.
     *
     */
    useRotation: boolean;
    constructor(
        location: DimensionLocation | Vector3,
        text: RawMessage | string,
    );
    /**
     * @remarks
     * Sets the text to display.
     *
     * @throws This function can throw errors.
     *
     * {@link ArgumentOutOfBoundsError}
     *
     * {@link RawMessageError}
     */
    setText(text: RawMessage | string): void;
}

export interface HandleCounts {
    handleCounts: Record<string, number>;
    name: string;
    packId: string;
    scriptModuleUUID: string;
}

export interface PluginStats {
    plugins: HandleCounts[];
}

export interface RuntimeStats {
    arrayCount: number;
    atomCount: number;
    atomSize: number;
    fastArrayCount: number;
    fastArrayElementCount: number;
    functionCodeSize: number;
    functionCount: number;
    functionLineCount: number;
    functionSize: number;
    memoryAllocatedCount: number;
    memoryAllocatedLimit: number;
    memoryAllocatedSize: number;
    memoryUsedCount: number;
    memoryUsedSize: number;
    objectCount: number;
    objectSize: number;
    propertyCount: number;
    propertySize: number;
    stringCount: number;
    stringSize: number;
}

/**
 * @remarks
 * Collect type usage stats for each active script
 * plugin/add-on.
 *
 * @returns
 * PluginStats object.
 */
export function collectPluginStats(): PluginStats;
/**
 * @remarks
 * Collect memory stats for script runtimes.
 *
 * @returns
 * RuntimeStats object.
 */
export function collectRuntimeStats(): RuntimeStats;
/**
 * @remarks
 * Disable watchdog slow and spike warnings per plugin.
 *
 * @param disable
 * Flag to disable or re-enable warnings.
 * @throws This function can throw errors.
 */
export function disableWatchdogTimingWarnings(disable: boolean): void;
/**
 * @remarks
 * Debug Drawing class used to allow adding and removing
 * wireframe shapes in world space.
 *
 */
export const debugDrawer: DebugDrawer;
