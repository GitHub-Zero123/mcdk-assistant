// Type definitions for Minecraft Bedrock Edition script APIs
// Project: https://docs.microsoft.com/minecraft/creator/
// Definitions by: Jake Shirley <https://github.com/JakeShirley>
//                 Mike Ammerlaan <https://github.com/mammerla>

/* *****************************************************************************
   Copyright (c) Microsoft Corporation.
   ***************************************************************************** */
/**
 * @packageDocumentation
 *
 * Manifest Details
 * ```json
 * {
 *   "module_name": "@minecraft/common",
 *   "version": "1.3.0"
 * }
 * ```
 *
 */
/**
 * An enum describing the type of InvalidArgumentError
 */
export enum InvalidArgumentErrorType {
    /**
     * @remarks
     * The argument has a duplicate in this domain.
     *
     */
    Duplicate = 'Duplicate',
    /**
     * @remarks
     * The argument is empty.
     *
     */
    Empty = 'Empty',
    /**
     * @remarks
     * The argument is not the correct type.
     *
     */
    InvalidType = 'InvalidType',
    /**
     * @remarks
     * The argument is unknown in this domain.
     *
     */
    Unknown = 'Unknown',
    /**
     * @remarks
     * No specified error type.
     *
     */
    Unspecified = 'Unspecified',
    /**
     * @remarks
     * The argument is unsupported for this type.
     *
     */
    UnsupportedValue = 'UnsupportedValue',
}

/**
 * 表示一个最大/最小值的结构，用于描述数值的取值范围。
 * 
 * Represents a min/max structure for expressing a potential
 * range of numbers.
 */
export interface NumberRange {
    /**
     * @remarks
     * 能够取到的最大值。
     * 
     * Maximum value within a range.
     *
     */
    max: number;
    /**
     * @remarks
     * 能够取到的最小值。
     * 
     * Minimum value within a range.
     *
     */
    min: number;
}

/**
 * 此类错误表示调用的方法参数或待设置的属性超出允许的范围。
 *
 * This type of error is thrown when a parameter to a method or
 * property is out of expected bounds.
 */
// @ts-ignore Class inheritance allowed for native defined classes
export class ArgumentOutOfBoundsError extends Error {
    private constructor();
    /**
     * @remarks
     * @earlyExecution
     *
     */
    readonly index: number;
    /**
     * @remarks
     * 参数允许的最大值。
     * 
     * Max expected value for the condition.
     *
     * @earlyExecution
     */
    readonly maxValue?: number;
    /**
     * @remarks
     * 参数允许的最小值。
     *
     * Min expected value for the condition.
     *
     * @earlyExecution
     *
     */
    readonly minValue?: number;
    /**
     * @remarks
     * 传入参数的值。
     *
     * Passed-in value for the argument.
     *
     * @earlyExecution
     *
     */
    readonly value?: number;
}

// @ts-ignore Class inheritance allowed for native defined classes
export class ContainerSizeOutOfBoundsError extends Error {
    private constructor();
    /**
     * @remarks
     * @earlyExecution
     *
     */
    readonly maxValue: number;
}

/**
 * 表示在处理函数时引擎中发生的底层错误。
 * 
 * Specifies an underlying error in the engine in processing a
 * function.
 */
// @ts-ignore Class inheritance allowed for native defined classes
export class EngineError extends Error {
    private constructor();
}

/**
 * 表示指定传递给方法的参数不正确或不被允许。
 * 
 * Specifies that a passed-in argument to a method is not
 * correct or allowed.
 */
// @ts-ignore Class inheritance allowed for native defined classes
export class InvalidArgumentError extends Error {
    /**
     * @remarks
     * Index of the argument that is in error.
     *
     * @earlyExecution
     *
     */
    readonly index: number;
    /**
     * @remarks
     * Type of the argument error.
     *
     * @earlyExecution
     *
     */
    readonly type: InvalidArgumentErrorType;
    /**
     * @remarks
     * @param funcName
     * @param argTypeName
     * @param errorType
     * @param argIndex
     */
    constructor(funcName: string, argTypeName: string, errorType: InvalidArgumentErrorType, argIndex?: number);
}

// @ts-ignore Class inheritance allowed for native defined classes
export class PropertyOutOfBoundsError extends Error {
    private constructor();
    /**
     * @remarks
     * @earlyExecution
     *
     */
    readonly maxValue?: number;
    /**
     * @remarks
     * @earlyExecution
     *
     */
    readonly minValue?: number;
    /**
     * @remarks
     * @earlyExecution
     *
     */
    readonly value: number;
}

// @ts-ignore Class inheritance allowed for native defined classes
export class RuntimeConditionError extends Error {
    private constructor();
}

// @ts-ignore Class inheritance allowed for native defined classes
export class UnsupportedFunctionalityError extends Error {
    private constructor();
}
