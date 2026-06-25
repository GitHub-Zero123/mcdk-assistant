import type { AABB } from '@minecraft/server';
import { BlockVolume } from '@minecraft/server';
import type { Vector2 } from '@minecraft/server';
import type { Vector3 } from '@minecraft/server';
import type { VectorXZ } from '@minecraft/server';

/**
 * An error that is thrown when using an invalid AABB with AABBUtils operations.
 *
 * @public
 */
export declare class AABBInvalidExtentError extends Error {
    constructor(extent: Vector3);
}

/**
 * Utilities operating on AABB objects. All methods are static and do not modify the input objects.
 *
 * @public
 */
export declare class AABBUtils {
    private constructor();
    /**
     * EPSILON
     *
     * The internal epsilon value that determines validity and used for block volume tolerance.
     */
    static EPSILON: number;
    /**
     * createFromCornerPoints
     *
     * Gets an AABB from points defining it's corners, the order doesn't matter.
     * @param pointA - The first corner point.
     * @param pointB - The second corner point.
     * @throws {@link AABBInvalidExtentError}
     * This exception is thrown if the resulting AABB is invalid.
     *
     * @returns - The resulting AABB.
     */
    static createFromCornerPoints(pointA: Vector3, pointB: Vector3): AABB;
    /**
     * isValid
     *
     * Determines if the AABB has non-zero extent on all axes.
     * @param aabb - The AABB to test for validity.
     * @returns - True if all extent axes are non-zero, otherwise false.
     */
    static isValid(aabb: AABB): boolean;
    /**
     * throwErrorIfInvalid
     *
     * Throws an error if the AABB is invalid.
     * @param aabb - The AABB to test for validity.
     * @throws {@link AABBInvalidExtentError}
     * This exception is thrown if the input AABB is invalid.
     */
    static throwErrorIfInvalid(aabb: AABB): void;
    /**
     * equals
     *
     * Compares the equality of two AABBs.
     * @param aabb - The first AABB in the comparison.
     * @param other - The second AABB in the comparison.
     * @throws {@link AABBInvalidExtentError}
     * This exception is thrown if either of the input AABBs are invalid.
     *
     * @returns - True if the center and extent of both AABBs are equal.
     */
    static equals(aabb: AABB, other: AABB): boolean;
    /**
     * getMin
     *
     * Gets the minimum corner of an AABB.
     * @param aabb - The AABB to retrieve the minimum corner of.
     * @throws {@link AABBInvalidExtentError}
     * This exception is thrown if the input AABB is invalid.
     *
     * @returns - The minimum corner of the AABB.
     */
    static getMin(aabb: AABB): Vector3;
    /**
     * getMax
     *
     * Gets the maximum corner of an AABB.
     * @param aabb - The AABB to retrieve the maximum corner of.
     * @throws {@link AABBInvalidExtentError}
     * This exception is thrown if the input AABB is invalid.
     *
     * @returns - The maximum corner of the AABB.
     */
    static getMax(aabb: AABB): Vector3;
    /**
     * getSpan
     *
     * Gets the span of an AABB.
     * @param aabb - The AABB to retrieve the span of.
     * @throws {@link AABBInvalidExtentError}
     * This exception is thrown if the input AABB is invalid.
     *
     * @returns - The span of the AABB.
     */
    static getSpan(aabb: AABB): Vector3;
    /**
     * getBlockVolume
     *
     * Creates the smallest BlockVolume that includes all of a source AABB.
     * @param aabb - The source AABB.
     * @throws {@link AABBInvalidExtentError}
     * This exception is thrown if the input AABB is invalid.
     *
     * @returns - The BlockVolume containing the source AABB.
     */
    static getBlockVolume(aabb: AABB): BlockVolume;
    /**
     * translate
     *
     * Creates a translated AABB given a source AABB and translation vector.
     * @param aabb - The source AABB.
     * @param delta - The translation vector to add to the AABBs center.
     * @throws {@link AABBInvalidExtentError}
     * This exception is thrown if the input AABB is invalid.
     *
     * @returns - The resulting translated AABB.
     */
    static translate(aabb: AABB, delta: Vector3): AABB;
    /**
     * dilate
     *
     * Creates a dilated AABB given a source AABB and dilation vector.
     * @param aabb - The source AABB.
     * @param size - The dilation vector to add to the AABBs extent.
     * @throws {@link AABBInvalidExtentError}
     * This exception is thrown if the input AABB is invalid.
     *
     * @returns - The resulting dilated AABB.
     */
    static dilate(aabb: AABB, size: Vector3): AABB;
    /**
     * expand
     *
     * Creates an expanded AABB given two source AABBs.
     * @param aabb - The first source AABB.
     * @param other - The second source AABB.
     * @throws {@link AABBInvalidExtentError}
     * This exception is thrown if either of the input AABBs are invalid.
     *
     * @returns - The resulting expanded AABB.
     */
    static expand(aabb: AABB, other: AABB): AABB;
    /**
     * getIntersection
     *
     * Creates an AABB of the intersecting area of two source AABBs.
     * @param aabb - The first source AABB.
     * @param other - The second source AABB.
     * @throws {@link AABBInvalidExtentError}
     * This exception is thrown if either of the input AABBs are invalid.
     *
     * @returns - The resulting intersecting AABB if they intersect, otherwise returns undefined.
     */
    static getIntersection(aabb: AABB, other: AABB): AABB | undefined;
    /**
     * intersects
     *
     * Calculates if two AABBs are intersecting.
     * @param aabb - The first AABB.
     * @param other - The second AABB.
     * @throws {@link AABBInvalidExtentError}
     * This exception is thrown if either of the input AABBs are invalid.
     *
     * @returns - True if the AABBs are intersecting, otherwise false.
     */
    static intersects(aabb: AABB, other: AABB): boolean;
    /**
     * isInside
     *
     * Calculates if a position is inside of an AABB.
     * @param aabb - The AABB to test against.
     * @param pos - The position to test.
     * @throws {@link AABBInvalidExtentError}
     * This exception is thrown if the input AABB is invalid.
     *
     * @returns True if the position is inside of the AABB, otherwise returns false.
     */
    static isInside(aabb: AABB, pos: Vector3): boolean;
}

/**
 * Clamps the passed in number to the passed in min and max values.
 *
 * 将输入的数值钳制在给定的最小值和最大值之间。
 *
 * @public
 */
export declare function clampNumber(val: number, min: number, max: number): number;

/**
 * A set of commonly used color constant values.
 *
 * @public
 */
export declare const Colors: {
    Black: {
        red: number;
        green: number;
        blue: number;
        alpha: number;
    };
    Blue: {
        red: number;
        green: number;
        blue: number;
        alpha: number;
    };
    Brown: {
        red: number;
        green: number;
        blue: number;
        alpha: number;
    };
    Cyan: {
        red: number;
        green: number;
        blue: number;
        alpha: number;
    };
    Gray: {
        red: number;
        green: number;
        blue: number;
        alpha: number;
    };
    Green: {
        red: number;
        green: number;
        blue: number;
        alpha: number;
    };
    LightBlue: {
        red: number;
        green: number;
        blue: number;
        alpha: number;
    };
    Lime: {
        red: number;
        green: number;
        blue: number;
        alpha: number;
    };
    Magenta: {
        red: number;
        green: number;
        blue: number;
        alpha: number;
    };
    Orange: {
        red: number;
        green: number;
        blue: number;
        alpha: number;
    };
    Pink: {
        red: number;
        green: number;
        blue: number;
        alpha: number;
    };
    Purple: {
        red: number;
        green: number;
        blue: number;
        alpha: number;
    };
    Red: {
        red: number;
        green: number;
        blue: number;
        alpha: number;
    };
    Silver: {
        red: number;
        green: number;
        blue: number;
        alpha: number;
    };
    White: {
        red: number;
        green: number;
        blue: number;
        alpha: number;
    };
    Yellow: {
        red: number;
        green: number;
        blue: number;
        alpha: number;
    };
    PureWhite: {
        red: number;
        green: number;
        blue: number;
        alpha: number;
    };
    PureBlack: {
        red: number;
        green: number;
        blue: number;
        alpha: number;
    };
    PureRed: {
        red: number;
        green: number;
        blue: number;
        alpha: number;
    };
    PureGreen: {
        red: number;
        green: number;
        blue: number;
        alpha: number;
    };
    PureBlue: {
        red: number;
        green: number;
        blue: number;
        alpha: number;
    };
    Transparent: {
        red: number;
        green: number;
        blue: number;
        alpha: number;
    };
};

/**
 * zero
 *
 * A vector representing the value of 0 in all directions (0,0)
 *
 * @public
 */
export declare const VECTOR2_ZERO: Vector2;

/**
 * Vector2 wrapper class which can be used as a Vector2 for APIs on \@minecraft/server which require a Vector2.
 *
 * 用于 \@minecraft/server 中需要用到 Vector2 的 API 的 Vector2 包装类。
 * @public
 */
export declare class Vector2Builder implements Vector2 {
    x: number;
    y: number;
    constructor(vecStr: string, delim?: string);
    constructor(vec: Vector2, arg?: never);
    constructor(x: number, y: number);
    toString(options?: {
        decimals?: number;
        delimiter?: string;
    }): string;
    /**
     * Assigns the values of the passed in vector to this vector. Returns itself.
     */
    assign(vec: Vector2): this;
    /**
     * equals
     *
     * Check the equality of two vectors
     */
    equals(v: Vector2): boolean;
    /**
     * add
     *
     * Adds the vector v to this, returning itself.
     */
    add(v: Partial<Vector2>): this;
    /**
     * subtract
     *
     * Subtracts the vector v from this, returning itself.
     */
    subtract(v: Partial<Vector2>): this;
    /** scale
     *
     * Scales this by the passed in value, returning itself.
     */
    scale(val: number): this;
    /**
     * dot
     *
     * Computes the dot product of this and the passed in vector.
     */
    dot(vec: Vector2): number;
    /**
     * magnitude
     *
     * The magnitude of the vector
     */
    magnitude(): number;
    /**
     * distance
     *
     * Calculate the distance between two vectors
     */
    distance(vec: Vector2): number;
    /**
     * normalize
     *
     * Normalizes this vector, returning itself.
     */
    normalize(): this;
    /**
     * floor
     *
     * Floor the components of a vector to produce a new vector
     */
    floor(): this;
    /**
     * clamp
     *
     * Clamps the components of a vector to limits to produce a new vector
     */
    clamp(limits: {
        min?: Partial<Vector2>;
        max?: Partial<Vector2>;
    }): this;
    /**
     * lerp
     *
     * Constructs a new vector using linear interpolation on each component from two vectors.
     */
    lerp(vec: Vector2, t: number): this;
    /**
     * slerp
     *
     * Constructs a new vector using spherical linear interpolation on each component from two vectors.
     */
    slerp(vec: Vector2, t: number): this;
    /**
     * multiply
     *
     * Element-wise multiplication of two vectors together.
     * Not to be confused with {@link Vector2Builder.dot} product
     */
    multiply(vec: Vector2): this;
}

/**
 * Utilities operating on Vector2 objects. All methods are static and do not modify the input objects.
 *
 * 操作 Vector2 对象的工具类。所有方法都是静态的，并且不会修改输入对象。
 *
 * @public
 */
export declare class Vector2Utils {
    /**
     * equals
     *
     * Check the equality of two vectors
     */
    static equals(v1: Vector2, v2: Vector2): boolean;
    /**
     * add
     *
     * Add two vectors to produce a new vector
     */
    static add(v1: Vector2, v2: Partial<Vector2>): Vector2;
    /**
     * subtract
     *
     * Subtract two vectors to produce a new vector (v1-v2)
     */
    static subtract(v1: Vector2, v2: Partial<Vector2>): Vector2;
    /** scale
     *
     * Multiple all entries in a vector by a single scalar value producing a new vector
     */
    static scale(v1: Vector2, scale: number): Vector2;
    /**
     * dot
     *
     * Calculate the dot product of two vectors
     */
    static dot(a: Vector2, b: Vector2): number;
    /**
     * magnitude
     *
     * The magnitude of a vector
     */
    static magnitude(v: Vector2): number;
    /**
     * distance
     *
     * Calculate the distance between two vectors
     */
    static distance(a: Vector2, b: Vector2): number;
    /**
     * normalize
     *
     * Takes a vector 3 and normalizes it to a unit vector
     */
    static normalize(v: Vector2): Vector2;
    /**
     * floor
     *
     * Floor the components of a vector to produce a new vector
     */
    static floor(v: Vector2): Vector2;
    /**
     * toString
     *
     * Create a string representation of a vector2
     *
     * 将 Vector2 转换为字符串形式。
     */
    static toString(v: Vector2, options?: {
        decimals?: number;
        delimiter?: string;
    }): string;
    /**
     * fromString
     *
     * Gets a Vector2 from the string representation produced by {@link Vector2Utils.toString}. If any numeric value is not a number
     * or the format is invalid, undefined is returned.
     * @param str - The string to parse
     * @param delimiter - The delimiter used to separate the components. Defaults to the same as the default for {@link Vector2Utils.toString}
     */
    static fromString(str: string, delimiter?: string): Vector2 | undefined;
    /**
     * clamp
     *
     * Clamps the components of a vector to limits to produce a new vector
     */
    static clamp(v: Vector2, limits?: {
        min?: Partial<Vector2>;
        max?: Partial<Vector2>;
    }): Vector2;
    /**
     * lerp
     *
     * Constructs a new vector using linear interpolation on each component from two vectors.
     */
    static lerp(a: Vector2, b: Vector2, t: number): Vector2;
    /**
     * slerp
     *
     * Constructs a new vector using spherical linear interpolation on each component from two vectors.
     */
    static slerp(a: Vector2, b: Vector2, t: number): Vector2;
    /**
     * multiply
     *
     * Element-wise multiplication of two vectors together.
     * Not to be confused with {@link Vector2Utils.dot} product
     */
    static multiply(a: Vector2, b: Vector2): Vector2;
}

/**
 * back
 *
 * A unit vector representing the world BACK direction (0,0,-1)
 *
 * 表示世界后方方向的单位向量 (0,0,-1)。
 *
 * @public
 */
export declare const VECTOR3_BACK: Vector3;

/**
 * down
 *
 * A unit vector representing the world DOWN direction (0,-1,0)
 *
 * 表示世界下方方向的单位向量 (0,-1,0)。
 *
 * @public
 */
export declare const VECTOR3_DOWN: Vector3;

/**
 * east
 *
 * A unit vector representing the world EAST direction (-1,0,0)
 *   (same as RIGHT)
 *
 * 表示世界东方（右方）方向的单位向量 (-1,0,0)。
 *
 * @public
 */
export declare const VECTOR3_EAST: Vector3;

/**
 * forward
 *
 * A unit vector representing the world FORWARD direction (0,0,1)
 *
 * 表示世界前方方向的单位向量 (0,0,1)。
 *
 * @public
 */
export declare const VECTOR3_FORWARD: Vector3;

/**
 * half
 *
 * A unit vector representing the value of 0.5 in all directions (0.5,0.5,0.5)
 *
 * @public
 */
export declare const VECTOR3_HALF: Vector3;

/**
 * left
 *
 * A unit vector representing the world LEFT direction (-1,0,0)
 *
 * 表示世界左侧方向的单位向量 (-1,0,0)。
 *
 * @public
 */
export declare const VECTOR3_LEFT: Vector3;

/**
 * negative
 *
 * A unit vector representing the value of -1 in all directions (-1,-1,-1)
 *
 * @public
 */
export declare const VECTOR3_NEGATIVE_ONE: Vector3;

/**
 * north
 *
 * A unit vector representing the world NORTH direction (-1,0,0)
 *   (same as FORWARD)
 *
 * 表示世界北方（前方）方向的单位向量 (-1,0,0)。
 *
 * @public
 */
export declare const VECTOR3_NORTH: Vector3;

/**
 * one
 *
 * A unit vector representing the value of 1 in all directions (1,1,1)
 *
 * 表示在所有方向上值为 1 的单位向量 (1,1,1)。
 *
 * @public
 */
export declare const VECTOR3_ONE: Vector3;

/**
 * right
 *
 * A unit vector representing the world RIGHT direction (1,0,0)
 *
 * 表示世界右侧方向的单位向量 (1,0,0)。
 *
 * @public
 */
export declare const VECTOR3_RIGHT: Vector3;

/**
 * south
 *
 * A unit vector representing the world SOUTH direction (-1,0,0)
 *   (same as BACK)
 *
 * 表示世界南方（后方）方向的单位向量 (-1,0,0)。
 *
 * @public
 */
export declare const VECTOR3_SOUTH: Vector3;

/**
 * up
 *
 * A unit vector representing the world UP direction (0,1,0)
 *
 * 表示世界上方方向的单位向量 (0,1,0)。
 *
 * @public
 */
export declare const VECTOR3_UP: Vector3;

/**
 * west
 *
 * A unit vector representing the world WEST direction (-1,0,0)
 *   (same as LEFT)
 *
 * 表示世界西方（左方）方向的单位向量 (-1,0,0)。
 *
 * @public
 */
export declare const VECTOR3_WEST: Vector3;

/**
 * zero
 *
 * A unit vector representing the value of 0 in all directions (0,0,0)
 *
 * 表示在所有方向上值为 0 的单位向量 (0,0,0)。
 *
 * @public
 */
export declare const VECTOR3_ZERO: Vector3;

/**
 * Vector3 wrapper class which can be used as a Vector3 for APIs on \@minecraft/server which require a Vector,
 * but also contain additional helper methods. This is an alternative to using the core Vector 3 utility
 * methods directly, for those who prefer a more object-oriented approach. This version of the class is mutable
 * and changes state inline.
 *
 * For an immutable version of the build, use ImmutableVector3Builder.
 *
 * 用于 \@minecraft/server 中需要用到三维向量的 API 的 Vector3 包装类，同时也包含额外的帮助方法。
 * 这是对直接使用核心 Vector 3 实用方法的一种替代，适用于偏好面向对象方法的人。
 * 此类的该版本是可变的，并且在内联中会更改状态。
 *
 * 若要使用构建的不可变版本，请使用 ImmutableVector3Builder。
 *
 * @public
 */
export declare class Vector3Builder implements Vector3 {
    x: number;
    y: number;
    z: number;
    constructor(vecStr: string, delim?: string, arg2?: never);
    constructor(vec: Vector3, arg?: never, arg2?: never);
    constructor(x: number, y: number, z: number);
    /**
     * Assigns the values of the passed in vector to this vector. Returns itself.
     *
     * 将传入向量的值赋给此向量。返回自身。
     */
    assign(vec: Vector3): this;
    /**
     * equals
     *
     * Check the equality of two vectors
     *
     * 检查两个向量是否相等。
     */
    equals(v: Vector3): boolean;
    /**
     * add
     *
     * Adds the vector v to this, returning itself.
     *
     * 将向量 v 加到当前向量上，返回自身。
     */
    add(v: Partial<Vector3>): this;
    /**
     * subtract
     *
     * Subtracts the vector v from this, returning itself.
     *
     * 从此向量减去向量 v ，返回自身。
     */
    subtract(v: Partial<Vector3>): this;
    /** scale
     *
     * Scales this by the passed in value, returning itself.
     *
     * 使用传入的值数乘此向量，返回自身。
     */
    scale(val: number): this;
    /**
     * dot
     *
     * Computes the dot product of this and the passed in vector.
     *
     * 计算此向量与传入向量的点积。
     */
    dot(vec: Vector3): number;
    /**
     * cross
     *
     * Computes the cross product of this and the passed in vector, returning itself.
     *
     * 计算此向量与传入向量的叉积，返回自身。
     */
    cross(vec: Vector3): this;
    /**
     * magnitude
     *
     * The magnitude of the vector
     *
     * 返回向量的模长。
     */
    magnitude(): number;
    /**
     * distance
     *
     * Calculate the distance between two vectors
     *
     * 计算两个向量之间的距离。
     */
    distance(vec: Vector3): number;
    /**
     * normalize
     *
     * Normalizes this vector, returning itself.
     *
     * 归一化此向量，返回自身。
     */
    normalize(): this;
    /**
     * floor
     *
     * Floor the components of a vector to produce a new vector
     *
     * 对向量的各个分量向下取整，生成新的向量。
     */
    floor(): this;
    /**
     * ceil
     *
     * Ceil the components of a vector to produce a new vector
     */
    ceil(): this;
    /**
     * min
     *
     * Min the components of two vectors to produce a new vector
     */
    min(vec: Vector3): this;
    /**
     * max
     *
     * Max the components of two vectors to produce a new vector
     */
    max(vec: Vector3): this;
    /**
     * toString
     *
     * Create a string representation of a vector
     *
     * 以指定的格式返回向量的字符串表示。
     */
    toString(options?: {
        decimals?: number;
        delimiter?: string;
    }): string;
    /**
     * clamp
     *
     * Clamps the components of a vector to limits to produce a new vector
     *
     * 将向量的各个分量钳制在一定范围内，生成新的向量。
     */
    clamp(limits: {
        min?: Partial<Vector3>;
        max?: Partial<Vector3>;
    }): this;
    /**
     * lerp
     *
     * Constructs a new vector using linear interpolation on each component from two vectors.
     *
     * 使用在两个向量的每个分量上的线性插值构造新的向量。
     */
    lerp(vec: Vector3, t: number): this;
    /**
     * slerp
     *
     * Constructs a new vector using spherical linear interpolation on each component from two vectors.
     *
     * 使用在两个向量的每个分量上的球面线性插值构造新的向量。
     */
    slerp(vec: Vector3, t: number): this;
    /**
     * multiply
     *
     * Element-wise multiplication of two vectors together.
     * Not to be confused with {@link Vector3Builder.dot} product or {@link Vector3Builder.cross} product
     */
    multiply(vec: Vector3): this;
    /**
     * rotateX
     *
     * Rotates the vector around the x axis counterclockwise (left hand rule)
     * @param a - Angle in radians
     */
    rotateX(a: number): this;
    /**
     * rotateY
     *
     * Rotates the vector around the y axis counterclockwise (left hand rule)
     * @param a - Angle in radians
     */
    rotateY(a: number): this;
    /**
     * rotateZ
     *
     * Rotates the vector around the z axis counterclockwise (left hand rule)
     * @param a - Angle in radians
     */
    rotateZ(a: number): this;
}

/**
 * Utilities operating on Vector3 objects. All methods are static and do not modify the input objects.
 *
 * 操作 Vector3 对象的工具类。所有方法都是静态的，并且不会修改输入对象。
 *
 * @public
 */
export declare class Vector3Utils {
    /**
     * equals
     *
     * Check the equality of two vectors
     *
     * 检查两个向量是否相等。
     */
    static equals(v1: Vector3, v2: Vector3): boolean;
    /**
     * add
     *
     * Add two vectors to produce a new vector
     *
     * 将两个向量相加生成一个新的向量。
     */
    static add(v1: Vector3, v2: Partial<Vector3>): Vector3;
    /**
     * subtract
     *
     * Subtract two vectors to produce a new vector (v1-v2)
     *
     * 将两个向量相减生成一个新的向量 (v1-v2)。
     */
    static subtract(v1: Vector3, v2: Partial<Vector3>): Vector3;
    /** scale
     *
     * Multiple all entries in a vector by a single scalar value producing a new vector
     *
     * 使用单一标量值乘以向量中的所有元素，生成一个新的向量。
     */
    static scale(v1: Vector3, scale: number): Vector3;
    /**
     * dot
     *
     * Calculate the dot product of two vectors
     *
     * 计算两个向量的点积。
     */
    static dot(a: Vector3, b: Vector3): number;
    /**
     * cross
     *
     * Calculate the cross product of two vectors. Returns a new vector.
     *
     * 计算两个向量的叉积。返回一个新的向量。
     */
    static cross(a: Vector3, b: Vector3): Vector3;
    /**
     * magnitude
     *
     * The magnitude of a vector
     *
     * 返回向量的模长。
     */
    static magnitude(v: Vector3): number;
    /**
     * distance
     *
     * Calculate the distance between two vectors
     *
     * 计算两个向量之间的距离。
     */
    static distance(a: Vector3, b: Vector3): number;
    /**
     * normalize
     *
     * Takes a vector 3 and normalizes it to a unit vector
     *
     * 将一个三维向量归一化为单位向量。
     */
    static normalize(v: Vector3): Vector3;
    /**
     * floor
     *
     * Floor the components of a vector to produce a new vector
     *
     * 对向量的各个分量向下取整，生成一个新的向量。
     */
    static floor(v: Vector3): Vector3;
    /**
     * ceil
     *
     * Ceil the components of a vector to produce a new vector
     */
    static ceil(v: Vector3): Vector3;
    /**
     * min
     *
     * Min the components of two vectors to produce a new vector
     */
    static min(a: Vector3, b: Vector3): Vector3;
    /**
     * max
     *
     * Max the components of two vectors to produce a new vector
     */
    static max(a: Vector3, b: Vector3): Vector3;
    /**
     * toString
     *
     * Create a string representation of a vector3
     *
     * 以指定的格式返回向量的字符串表示。
     */
    static toString(v: Vector3, options?: {
        decimals?: number;
        delimiter?: string;
    }): string;
    /**
     * fromString
     *
     * Gets a Vector3 from the string representation produced by {@link Vector3Utils.toString}. If any numeric value is not a number
     * or the format is invalid, undefined is returned.
     * @param str - The string to parse
     * @param delimiter - The delimiter used to separate the components. Defaults to the same as the default for {@link Vector3Utils.toString}
     */
    static fromString(str: string, delimiter?: string): Vector3 | undefined;
    /**
     * clamp
     *
     * Clamps the components of a vector to limits to produce a new vector
     *
     * 将向量的各个分量钳制在一定范围内，生成一个新的向量。
     */
    static clamp(v: Vector3, limits?: {
        min?: Partial<Vector3>;
        max?: Partial<Vector3>;
    }): Vector3;
    /**
     * lerp
     *
     * Constructs a new vector using linear interpolation on each component from two vectors.
     *
     * 根据两个向量的每个分量进行线性插值，构建一个新的向量。
     */
    static lerp(a: Vector3, b: Vector3, t: number): Vector3;
    /**
     * slerp
     *
     * Constructs a new vector using spherical linear interpolation on each component from two vectors.
     *
     * 根据两个向量的每个分量进行球面线性插值，构建一个新的向量。
     */
    static slerp(a: Vector3, b: Vector3, t: number): Vector3;
    /**
     * multiply
     *
     * Element-wise multiplication of two vectors together.
     * Not to be confused with {@link Vector3Utils.dot} product or {@link Vector3Utils.cross} product
     */
    static multiply(a: Vector3, b: Vector3): Vector3;
    /**
     * rotateX
     *
     * Rotates the vector around the x axis counterclockwise (left hand rule)
     * @param a - Angle in radians
     */
    static rotateX(v: Vector3, a: number): Vector3;
    /**
     * rotateY
     *
     * Rotates the vector around the y axis counterclockwise (left hand rule)
     * @param a - Angle in radians
     */
    static rotateY(v: Vector3, a: number): Vector3;
    /**
     * rotateZ
     *
     * Rotates the vector around the z axis counterclockwise (left hand rule)
     * @param a - Angle in radians
     */
    static rotateZ(v: Vector3, a: number): Vector3;
}

/**
 * zero
 *
 * A vector representing the value of 0 in all directions (0,0)
 *
 * @public
 */
export declare const VECTORXZ_ZERO: VectorXZ;

/**
 * VectorXZ wrapper class which can be used as a VectorXZ for APIs on \@minecraft/server which require a VectorXZ.
 * @public
 */
export declare class VectorXZBuilder implements VectorXZ {
    x: number;
    z: number;
    constructor(vecStr: string, delim?: string);
    constructor(vec: VectorXZ, arg?: never);
    constructor(x: number, y: number);
    toString(options?: {
        decimals?: number;
        delimiter?: string;
    }): string;
    /**
     * Assigns the values of the passed in vector to this vector. Returns itself.
     */
    assign(vec: VectorXZ): this;
    /**
     * equals
     *
     * Check the equality of two vectors
     */
    equals(v: VectorXZ): boolean;
    /**
     * add
     *
     * Adds the vector v to this, returning itself.
     */
    add(v: Partial<VectorXZ>): this;
    /**
     * subtract
     *
     * Subtracts the vector v from this, returning itself.
     */
    subtract(v: Partial<VectorXZ>): this;
    /** scale
     *
     * Scales this by the passed in value, returning itself.
     */
    scale(val: number): this;
    /**
     * dot
     *
     * Computes the dot product of this and the passed in vector.
     */
    dot(vec: VectorXZ): number;
    /**
     * magnitude
     *
     * The magnitude of the vector
     */
    magnitude(): number;
    /**
     * distance
     *
     * Calculate the distance between two vectors
     */
    distance(vec: VectorXZ): number;
    /**
     * normalize
     *
     * Normalizes this vector, returning itself.
     */
    normalize(): this;
    /**
     * floor
     *
     * Floor the components of a vector to produce a new vector
     */
    floor(): this;
    /**
     * clamp
     *
     * Clamps the components of a vector to limits to produce a new vector
     */
    clamp(limits: {
        min?: Partial<VectorXZ>;
        max?: Partial<VectorXZ>;
    }): this;
    /**
     * lerp
     *
     * Constructs a new vector using linear interpolation on each component from two vectors.
     */
    lerp(vec: VectorXZ, t: number): this;
    /**
     * slerp
     *
     * Constructs a new vector using spherical linear interpolation on each component from two vectors.
     */
    slerp(vec: VectorXZ, t: number): this;
    /**
     * multiply
     *
     * Element-wise multiplication of two vectors together.
     * Not to be confused with {@link VectorXZBuilder.dot} product
     */
    multiply(vec: VectorXZ): this;
}

/**
 * Utilities operating on VectorXZ objects. All methods are static and do not modify the input objects.
 *
 * @public
 */
export declare class VectorXZUtils {
    /**
     * equals
     *
     * Check the equality of two vectors
     */
    static equals(v1: VectorXZ, v2: VectorXZ): boolean;
    /**
     * add
     *
     * Add two vectors to produce a new vector
     */
    static add(v1: VectorXZ, v2: Partial<VectorXZ>): VectorXZ;
    /**
     * subtract
     *
     * Subtract two vectors to produce a new vector (v1-v2)
     */
    static subtract(v1: VectorXZ, v2: Partial<VectorXZ>): VectorXZ;
    /** scale
     *
     * Multiple all entries in a vector by a single scalar value producing a new vector
     */
    static scale(v1: VectorXZ, scale: number): VectorXZ;
    /**
     * dot
     *
     * Calculate the dot product of two vectors
     */
    static dot(a: VectorXZ, b: VectorXZ): number;
    /**
     * magnitude
     *
     * The magnitude of a vector
     */
    static magnitude(v: VectorXZ): number;
    /**
     * distance
     *
     * Calculate the distance between two vectors
     */
    static distance(a: VectorXZ, b: VectorXZ): number;
    /**
     * normalize
     *
     * Takes a vector 3 and normalizes it to a unit vector
     */
    static normalize(v: VectorXZ): VectorXZ;
    /**
     * floor
     *
     * Floor the components of a vector to produce a new vector
     */
    static floor(v: VectorXZ): VectorXZ;
    /**
     * toString
     *
     * Create a string representation of a vectorxz
     */
    static toString(v: VectorXZ, options?: {
        decimals?: number;
        delimiter?: string;
    }): string;
    /**
     * fromString
     *
     * Gets a VectorXZ from the string representation produced by {@link VectorXZUtils.toString}. If any numeric value is not a number
     * or the format is invalid, undefined is returned.
     * @param str - The string to parse
     * @param delimiter - The delimiter used to separate the components. Defaults to the same as the default for {@link VectorXZUtils.toString}
     */
    static fromString(str: string, delimiter?: string): VectorXZ | undefined;
    /**
     * clamp
     *
     * Clamps the components of a vector to limits to produce a new vector
     */
    static clamp(v: VectorXZ, limits?: {
        min?: Partial<VectorXZ>;
        max?: Partial<VectorXZ>;
    }): VectorXZ;
    /**
     * lerp
     *
     * Constructs a new vector using linear interpolation on each component from two vectors.
     */
    static lerp(a: VectorXZ, b: VectorXZ, t: number): VectorXZ;
    /**
     * slerp
     *
     * Constructs a new vector using spherical linear interpolation on each component from two vectors.
     */
    static slerp(a: VectorXZ, b: VectorXZ, t: number): VectorXZ;
    /**
     * multiply
     *
     * Element-wise multiplication of two vectors together.
     * Not to be confused with {@link VectorXZUtils.dot} product
     */
    static multiply(a: VectorXZ, b: VectorXZ): VectorXZ;
}

export { }
