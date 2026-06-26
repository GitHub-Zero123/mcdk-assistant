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
 * `@minecraft/server-gametest` 为脚本API在脚手架与内容测试方面提供支持。
 * 
 * The @minecraft/server-gametest module provides scriptable
 * APIs for scaffolding and testing content experiences in
 * Minecraft.
 *
 * Manifest Details
 * ```json
 * {
 *   "module_name": "@minecraft/server-gametest",
 *   "version": "1.0.0-beta"
 * }
 * ```
 *
 */
import { EngineError, InvalidArgumentError } from '@minecraft/common';
import { Block, BlockPermutation, BlockType, Dimension, DimensionLocation, DimensionType, Direction, Entity, FluidType, GameMode, InvalidEntityError, ItemStack, ItemType, Player, RGB, Vector2, Vector3 } from '@minecraft/server';
export enum GameTestCompletedErrorReason {
    Cleanup = 'Cleanup',
    Done = 'Done',
}

export enum GameTestErrorType {
    Assert = 'Assert',
    AssertAtPosition = 'AssertAtPosition',
    ExecutionTimeout = 'ExecutionTimeout',
    ExhaustedAttempts = 'ExhaustedAttempts',
    FailConditionsMet = 'FailConditionsMet',
    LevelStateModificationFailed = 'LevelStateModificationFailed',
    MethodNotImplemented = 'MethodNotImplemented',
    SimulatedPlayerOutOfBounds = 'SimulatedPlayerOutOfBounds',
    Unknown = 'Unknown',
    Waiting = 'Waiting',
}

export enum LookDuration {
    Continuous = 'Continuous',
    Instant = 'Instant',
    UntilMove = 'UntilMove',
}

export enum PersonaArmSize {
    Slim = 'Slim',
    Wide = 'Wide',
}

export enum PersonaPieceType {
    Arms = 'Arms',
    Back = 'Back',
    Body = 'Body',
    Bottom = 'Bottom',
    Capes = 'Capes',
    Dress = 'Dress',
    Eyes = 'Eyes',
    FaceAccessory = 'FaceAccessory',
    FacialHair = 'FacialHair',
    Feet = 'Feet',
    Hair = 'Hair',
    Hands = 'Hands',
    Head = 'Head',
    HighPants = 'HighPants',
    Hood = 'Hood',
    LeftArm = 'LeftArm',
    LeftLeg = 'LeftLeg',
    Legs = 'Legs',
    Mouth = 'Mouth',
    Outerwear = 'Outerwear',
    RightArm = 'RightArm',
    RightLeg = 'RightLeg',
    Skeleton = 'Skeleton',
    Skin = 'Skin',
    Top = 'Top',
}

/**
 * Returns information about whether this fence is connected to
 * other fences in several directions.
 */
export class FenceConnectivity {
    private constructor();
    /**
     * @remarks
     * Represents whether this fence block is connected to another
     * fence to the east (x + 1).
     *
     */
    readonly east: boolean;
    /**
     * @remarks
     * Represents whether this fence block is connected to another
     * fence to the north (z - 1).
     *
     */
    readonly north: boolean;
    /**
     * @remarks
     * Represents whether this fence block is connected to another
     * fence to the south (z + 1).
     *
     */
    readonly south: boolean;
    /**
     * @remarks
     * Represents whether this fence block is connected to another
     * fence to the west (x - 1).
     *
     */
    readonly west: boolean;
}

/**
 * Executes a set of steps defined via chained .thenXyz
 * methods, sequentially. This facilitates a 'script' of
 * GameTest setup methods and assertions over time.
 */
export class GameTestSequence {
    private constructor();
    /**
     * @remarks
     * Runs the given callback as a step within a GameTest
     * sequence. Exceptions thrown within the callback will end
     * sequence execution.
     *
     * @worldMutation
     *
     * @param callback
     * Callback function to execute.
     * @returns
     * Returns a GameTestSequence object where additional .thenXyz
     * method steps can be added.
     */
    thenExecute(callback: () => void): GameTestSequence;
    /**
     * @remarks
     * After a delay, runs the given callback as a step within a
     * GameTest sequence. Exceptions thrown within the callback
     * will end sequence execution.
     *
     * @worldMutation
     *
     * @param delayTicks
     * Number of ticks to wait before executing the callback.
     * @param callback
     * Callback function to execute.
     * @returns
     * Returns a GameTestSequence object where additional .thenXyz
     * method steps can be added.
     */
    thenExecuteAfter(delayTicks: number, callback: () => void): GameTestSequence;
    /**
     * @remarks
     * Runs the given callback every tick for the given number of
     * ticks.
     *
     * @worldMutation
     *
     * @param callback
     * Callback function to execute.
     * @returns
     * Returns a GameTestSequence object where additional .thenXyz
     * method steps can be added.
     */
    thenExecuteFor(tickCount: number, callback: () => void): GameTestSequence;
    /**
     * @remarks
     * Causes the test to fail if this step in the GameTest
     * sequence is reached.
     *
     * @worldMutation
     *
     * @param errorMessage
     * Error message summarizing the failure condition.
     */
    thenFail(errorMessage: string): void;
    /**
     * @remarks
     * Idles the GameTest sequence for the specified delayTicks.
     *
     * @worldMutation
     *
     * @param delayTicks
     * Number of ticks to delay for this step in the GameTest
     * sequence.
     * @returns
     * Returns a GameTestSequence object where additional .thenXyz
     * method steps can be added.
     */
    thenIdle(delayTicks: number): GameTestSequence;
    /**
     * @remarks
     * Marks the GameTest a success if this step is reached in the
     * GameTest sequence.
     *
     * @worldMutation
     *
     */
    thenSucceed(): void;
    /**
     * @remarks
     * Executes the given callback every tick until it succeeds.
     * Exceptions thrown within the callback will end sequence
     * execution.
     *
     * @worldMutation
     *
     * @param callback
     * Testing callback function to execute. Typically, this
     * function will have .assertXyz functions within it.
     * @returns
     * Returns a GameTestSequence object where additional .thenXyz
     * method steps can be added.
     */
    thenWait(callback: () => void): GameTestSequence;
    /**
     * @remarks
     * After a delay from the previous step, executes the given
     * callback every tick until it succeeds. Exceptions thrown
     * within the callback will end sequence execution.
     *
     * @worldMutation
     *
     * @param delayTicks
     * Tick (after the previous step in the GameTest sequence) to
     * run the callback at.
     * @param callback
     * Testing callback function to execute. Typically, this
     * function will have .assertXyz functions within it.
     * @returns
     * Returns a GameTestSequence object where additional .thenXyz
     * method steps can be added.
     */
    thenWaitAfter(delayTicks: number, callback: () => void): GameTestSequence;
}

export class NavigationResult {
    private constructor();
    readonly isFullPath: boolean;
    getPath(): Vector3[];
}

/**
 * A utility class to set GameTest parameters for a test.
 * Methods can be chained together to set multiple properties.
 */
export class RegistrationBuilder {
    private constructor();
    /**
     * @remarks
     * Sets the batch for the test to run in.
     *
     * @worldMutation
     *
     * @earlyExecution
     *
     * @param batchName
     * Name of the batch for the test.
     * @returns
     * RegistrationBuilder object where additional configuration
     * methods can be called.
     */
    batch(batchName: string): RegistrationBuilder;
    /**
     * @remarks
     * Sets the maximum number of times a test will try to rerun if
     * it fails.
     *
     * @worldMutation
     *
     * @earlyExecution
     *
     * @returns
     * RegistrationBuilder object where additional configuration
     * methods can be called.
     */
    maxAttempts(attemptCount: number): RegistrationBuilder;
    /**
     * @remarks
     * Sets the maximum number of ticks a test will run for before
     * timing out and failing.
     *
     * @worldMutation
     *
     * @earlyExecution
     *
     * @returns
     * RegistrationBuilder object where additional configuration
     * methods can be called.
     */
    maxTicks(tickCount: number): RegistrationBuilder;
    /**
     * @remarks
     * Size around the GameTest, in blocks, that should be reserved
     * for the test when running multiple tests together.
     *
     * @worldMutation
     *
     * @earlyExecution
     *
     * @param paddingBlocks
     * Size, in blocks, around the GameTest where additional
     * GameTests should not be created.
     * @returns
     * RegistrationBuilder object where additional configuration
     * methods can be called.
     */
    padding(paddingBlocks: number): RegistrationBuilder;
    /**
     * @remarks
     * Whether this test is required to pass as part of its broader
     * set of tests.
     *
     * @worldMutation
     *
     * @earlyExecution
     *
     * @param isRequired
     * If set to true, the test must pass in order for the entire
     * run of tests to pass.
     * @returns
     * RegistrationBuilder object where additional configuration
     * methods can be called.
     */
    required(isRequired: boolean): RegistrationBuilder;
    /**
     * @remarks
     * Sets the number of successful test runs to be considered
     * successful.
     *
     * @worldMutation
     *
     * @earlyExecution
     *
     * @returns
     * RegistrationBuilder object where additional configuration
     * methods can be called.
     */
    requiredSuccessfulAttempts(attemptCount: number): RegistrationBuilder;
    /**
     * @remarks
     * If true, runs the test in all four rotations when run via
     * /gametest runset.
     *
     * @worldMutation
     *
     * @earlyExecution
     *
     */
    rotateTest(rotate: boolean): RegistrationBuilder;
    /**
     * @remarks
     * Sets the number of ticks for a test to wait before executing
     * when the structure is spawned.
     *
     * @worldMutation
     *
     * @earlyExecution
     *
     * @returns
     * RegistrationBuilder object where additional configuration
     * methods can be called.
     */
    setupTicks(tickCount: number): RegistrationBuilder;
    /**
     * @remarks
     * Overrides the default structure placement with a specific
     * location and dimension. If height (y) is set to
     * Dimension.heightRange.max, the structure will snap to the
     * ground. If the dimension is not specified, it will run in
     * the dimension the command was run from.
     *
     * @worldMutation
     *
     * @earlyExecution
     *
     * @returns
     * RegistrationBuilder object where additional configuration
     * methods can be called.
     * @throws This function can throw errors.
     *
     * {@link EngineError}
     *
     * {@link InvalidArgumentError}
     */
    structureLocation(
        structureLocation: Vector3,
        structureDimension?: DimensionType | string,
    ): RegistrationBuilder;
    /**
     * @remarks
     * Sets the name of the structure for a test to use. "xyz:bar"
     * will load `/structures/xyz/bar.mcstructure` from the
     * behavior pack stack.
     *
     * @worldMutation
     *
     * @earlyExecution
     *
     * @returns
     * RegistrationBuilder object where additional configuration
     * methods can be called.
     * @seeExample phantomsShouldFlyFromCats.ts
     */
    structureName(structureName: string): RegistrationBuilder;
    /**
     * @remarks
     * Adds a tag to a test. You can run all tests with a given tag
     * with `/gametest runset <tag>`.
     *
     * @worldMutation
     *
     * @earlyExecution
     *
     * @returns
     * RegistrationBuilder object where additional configuration
     * methods can be called.
     * @seeExample phantomsShouldFlyFromCats.ts
     */
    tag(tag: string): RegistrationBuilder;
}

/**
 * Implements a class that can be used for testing sculk
 * spreading behaviors. This sculk spreader class can drive the
 * growth of sculk around a particular block.
 */
export class SculkSpreader {
    private constructor();
    /**
     * @remarks
     * Gets the maximum charge of a sculk spreader.
     *
     * @throws This property can throw when used.
     */
    readonly maxCharge: number;
    /**
     * @remarks
     * Adds a cursor - which is a notional waypoint that the sculk
     * will spread in the direction of.
     *
     * @worldMutation
     *
     */
    addCursorsWithOffset(offset: Vector3, charge: number): void;
    /**
     * @remarks
     * Retrieves the current position of the specified cursor.
     *
     * @worldMutation
     *
     * @throws This function can throw errors.
     */
    getCursorPosition(index: number): Vector3;
    /**
     * @remarks
     * Returns a number of overall cursors for this sculk spreader.
     *
     * @worldMutation
     *
     * @throws This function can throw errors.
     */
    getNumberOfCursors(): number;
    /**
     * @remarks
     * Gets the total current charge of the sculk spreader.
     *
     * @worldMutation
     *
     * @throws This function can throw errors.
     */
    getTotalCharge(): number;
}

/**
 * A simulated player can be used within GameTests to represent
 * how a player moves throughout the world and to support
 * testing of how entities and the environment will react to a
 * player. This type derives much of its structure and methods
 * from the {@link Player} type. Note that many
 * types of events that may be available for entities more
 * broadly, such as item use events, may not fire in the same
 * capacity for simulated players.
 */
// @ts-ignore Class inheritance allowed for native defined classes
export class SimulatedPlayer extends Player {
    private constructor();
    /**
     * @remarks
     * Rotation of the head across pitch and yaw angles.
     *
     * @throws This property can throw when used.
     */
    readonly headRotation: Vector2;
    /**
     * @remarks
     * Returns whether the simulated player is sprinting.
     *
     * @worldMutation
     *
     */
    isSprinting: boolean;
    /**
     * @remarks
     * Causes the simulated player to make an attack 'swipe'.
     * Returns true if the attack was performed - for example, the
     * player was not on cooldown and had a valid target. Target
     * selection is performed by raycasting from the player's head.
     *
     * @worldMutation
     *
     * @throws This function can throw errors.
     */
    attack(): boolean;
    /**
     * @remarks
     * Causes the simulated player to attack the provided target.
     * Returns true if the attack was performed - for example, the
     * player was not on cooldown and had a valid target. The
     * attack can be performed at any distance and does not require
     * line of sight to the target entity.
     *
     * @worldMutation
     *
     * @throws This function can throw errors.
     */
    attackEntity(entity: Entity): boolean;
    /**
     * @remarks
     * Destroys the block at blockLocation, respecting the rules of
     * the server player's game mode. The block will be hit until
     * broken, an item is used or stopBreakingBlock is called.
     * Returns true if the block at blockLocation is solid.
     *
     * @worldMutation
     *
     * @param blockLocation
     * Location of the block to interact with.
     * @param direction
     * Direction to place the specified item within.
     * Defaults to: 1
     * @throws This function can throw errors.
     *
     * {@link GameTestError}
     *
     * {@link InvalidEntityError}
     */
    breakBlock(blockLocation: Vector3, direction?: Direction): boolean;
    /**
     * @remarks
     * @worldMutation
     *
     * @throws This function can throw errors.
     */
    chat(message: string): void;
    /**
     * @remarks
     * Simulates and performs a disconnection of the simulated
     * player from the world.
     *
     * @worldMutation
     *
     * @throws This function can throw errors.
     */
    disconnect(): void;
    /**
     * @remarks
     * Drops the simulated player's selected item
     *
     * @worldMutation
     *
     * @throws This function can throw errors.
     */
    dropSelectedItem(): boolean;
    /**
     * @remarks
     * Causes the simulated player to start flying as though they
     * were flying in creative mode. For flying with Elytra, see
     * function glide.
     *
     * @worldMutation
     *
     * @throws This function can throw errors.
     */
    fly(): void;
    /**
     * @remarks
     * Gives the simulated player a particular item stack.
     *
     * @worldMutation
     *
     * @param itemStack
     * Item to give.
     * @param selectSlot
     * Whether to set the selected slot once given.
     * Defaults to: false
     * @throws This function can throw errors.
     */
    giveItem(itemStack: ItemStack, selectSlot?: boolean): boolean;
    /**
     * @remarks
     * Causes the simulated player to start gliding. Elytra must be
     * equipped and the player must be in the air.
     *
     * @worldMutation
     *
     * @returns
     * Returns true if the simulated player begins to glide.
     * Returns false if the player is already gliding, or the
     * player does not have Elytra equipped, is in water or is on
     * the ground.
     * @throws This function can throw errors.
     */
    glide(): boolean;
    /**
     * @remarks
     * Performs a raycast from the player’s head and interacts with
     * the first intersected block or entity. Returns true if the
     * interaction was successful. Maximum range is 6 blocks.
     *
     * @worldMutation
     *
     * @throws This function can throw errors.
     */
    interact(): boolean;
    /**
     * @remarks
     * Causes the simulated player to interact with a block. The
     * block at the specified block location must be solid. Returns
     * true if the interaction was performed.
     *
     * @worldMutation
     *
     * @param blockLocation
     * Location of the block to interact with.
     * @param direction
     * Direction to place the specified item within.
     * Defaults to: 1
     * @throws This function can throw errors.
     *
     * {@link GameTestError}
     *
     * {@link InvalidEntityError}
     */
    interactWithBlock(blockLocation: Vector3, direction?: Direction): boolean;
    /**
     * @remarks
     * Causes the simulated player to interact with a mob. Returns
     * true if the interaction was performed.
     *
     * @worldMutation
     *
     * @param entity
     * Entity to interact with.
     * @throws This function can throw errors.
     *
     * {@link InvalidArgumentError}
     *
     * {@link InvalidEntityError}
     */
    interactWithEntity(entity: Entity): boolean;
    /**
     * @remarks
     * Causes the simulated player to jump.
     *
     * @worldMutation
     *
     * @returns
     * True if a jump was performed.
     * @throws This function can throw errors.
     */
    jump(): boolean;
    /**
     * @remarks
     * Rotates the simulated player's head/body to look at the
     * given block location.
     *
     * @worldMutation
     *
     * @param duration
     * Defaults to: 2
     * @throws This function can throw errors.
     *
     * {@link GameTestError}
     *
     * {@link InvalidEntityError}
     */
    lookAtBlock(blockLocation: Vector3, duration?: LookDuration): void;
    /**
     * @remarks
     * Rotates the simulated player's head/body to look at the
     * given entity.
     *
     * @worldMutation
     *
     * @param duration
     * Defaults to: 2
     * @throws This function can throw errors.
     */
    lookAtEntity(entity: Entity, duration?: LookDuration): void;
    /**
     * @remarks
     * Rotates the simulated player's head/body to look at the
     * given location.
     *
     * @worldMutation
     *
     * @param duration
     * Defaults to: 2
     * @throws This function can throw errors.
     */
    lookAtLocation(location: Vector3, duration?: LookDuration): void;
    /**
     * @remarks
     * Orders the simulated player to walk in the given direction
     * relative to the GameTest.
     *
     * @worldMutation
     *
     * @param speed
     * Defaults to: 1
     * Bounds: [0, 1]
     * @throws This function can throw errors.
     */
    move(westEast: number, northSouth: number, speed?: number): void;
    /**
     * @remarks
     * Orders the simulated player to walk in the given direction
     * relative to the player's current rotation.
     *
     * @worldMutation
     *
     * @param speed
     * Defaults to: 1
     * Bounds: [0, 1]
     * @throws This function can throw errors.
     */
    moveRelative(leftRight: number, backwardForward: number, speed?: number): void;
    /**
     * @remarks
     * Orders the simulated player to move to the given block
     * location in a straight line. If a move or navigation is
     * already playing, this will override the last
     * move/navigation.
     *
     * @worldMutation
     *
     * @throws This function can throw errors.
     */
    moveToBlock(blockLocation: Vector3, options?: MoveToOptions): void;
    /**
     * @remarks
     * Orders the simulated player to move to the given location in
     * a straight line. If a move or navigation is already playing,
     * this will override the last move/navigation.
     *
     * @worldMutation
     *
     * @throws This function can throw errors.
     *
     * {@link Error}
     *
     * {@link GameTestError}
     *
     * {@link InvalidEntityError}
     */
    moveToLocation(location: Vector3, options?: MoveToOptions): void;
    /**
     * @remarks
     * Orders the simulated player to move to a specific block
     * location using navigation. If a move or navigation is
     * already playing, this will override the last move/walk. Note
     * that if the simulated player gets stuck, that simulated
     * player will stop. The player must be touching the ground in
     * order to start navigation.
     *
     * @worldMutation
     *
     * @param speed
     * Defaults to: 1
     * Bounds: [0, 1]
     * @throws This function can throw errors.
     *
     * {@link GameTestError}
     *
     * {@link InvalidEntityError}
     */
    navigateToBlock(blockLocation: Vector3, speed?: number): NavigationResult;
    /**
     * @remarks
     * Will use navigation to follow the selected entity to within
     * a one block radius. If a move or navigation is already
     * playing, this will override the last move/navigation.
     *
     * @worldMutation
     *
     * @param speed
     * Defaults to: 1
     * Bounds: [0, 1]
     * @throws This function can throw errors.
     *
     * {@link InvalidArgumentError}
     *
     * {@link InvalidEntityError}
     */
    navigateToEntity(entity: Entity, speed?: number): NavigationResult;
    /**
     * @remarks
     * Orders the simulated player to move to a specific location
     * using navigation. If a move or navigation is already
     * playing, this will override the last move/walk. Note that if
     * the simulated player gets stuck, that simulated player will
     * stop. The player must be touching the ground in order to
     * start navigation.
     *
     * @worldMutation
     *
     * @param speed
     * Defaults to: 1
     * Bounds: [0, 1]
     * @throws This function can throw errors.
     *
     * {@link GameTestError}
     *
     * {@link InvalidEntityError}
     */
    navigateToLocation(location: Vector3, speed?: number): NavigationResult;
    /**
     * @remarks
     * Use navigation to follow the route provided via the
     * locations parameter. If a move or navigation is already
     * playing, this will override the last move/navigation.
     *
     * @worldMutation
     *
     * @param locations
     * A list of locations to use for routing.
     * @param speed
     * Net speed to use for doing the navigation.
     * Defaults to: 1
     * Bounds: [0, 1]
     * @throws This function can throw errors.
     *
     * {@link GameTestError}
     *
     * {@link InvalidEntityError}
     */
    navigateToLocations(locations: Vector3[], speed?: number): void;
    /**
     * @remarks
     * Respawns the particular simulated player.
     *
     * @worldMutation
     *
     * @throws This function can throw errors.
     */
    respawn(): boolean;
    /**
     * @remarks
     * Causes the simulated player to turn by the provided angle,
     * relative to the player's current rotation.
     *
     * @worldMutation
     *
     * @throws This function can throw errors.
     */
    rotateBody(angleInDegrees: number): void;
    /**
     * @remarks
     * Causes the simulated player to turn to face the provided
     * angle, relative to the GameTest.
     *
     * @worldMutation
     *
     * @throws This function can throw errors.
     */
    setBodyRotation(angleInDegrees: number): void;
    /**
     * @remarks
     * Sets a particular item for the simulated player.
     *
     * @worldMutation
     *
     * @param itemStack
     * Item to set.
     * @param slot
     * Slot to place the given item in.
     * @param selectSlot
     * Whether to set the selected slot once set.
     * Defaults to: false
     * @throws This function can throw errors.
     */
    setItem(itemStack: ItemStack, slot: number, selectSlot?: boolean): boolean;
    /**
     * @remarks
     * Updates information about the player's skin.
     *
     * @worldMutation
     *
     * @param options
     * Options for the skin to set on the player.
     * @throws This function can throw errors.
     *
     * {@link InvalidEntityError}
     */
    setSkin(options: PlayerSkinData): void;
    /**
     * @remarks
     * @worldMutation
     *
     * @param slot
     * Defaults to: 0
     * @throws This function can throw errors.
     */
    startBuild(slot?: number): void;
    /**
     * @remarks
     * Stops destroying the block that is currently being hit.
     *
     * @worldMutation
     *
     * @throws This function can throw errors.
     */
    stopBreakingBlock(): void;
    /**
     * @remarks
     * @worldMutation
     *
     * @throws This function can throw errors.
     */
    stopBuild(): void;
    /**
     * @remarks
     * Causes the simulated player to stop flying.
     *
     * @worldMutation
     *
     * @throws This function can throw errors.
     */
    stopFlying(): void;
    /**
     * @remarks
     * Causes the simulated player to stop gliding.
     *
     * @worldMutation
     *
     * @throws This function can throw errors.
     */
    stopGliding(): void;
    /**
     * @remarks
     * Stops interacting with entities or blocks.
     *
     * @worldMutation
     *
     * @throws This function can throw errors.
     */
    stopInteracting(): void;
    /**
     * @remarks
     * Stops moving/walking/following if the simulated player is
     * moving.
     *
     * @worldMutation
     *
     * @throws This function can throw errors.
     */
    stopMoving(): void;
    /**
     * @remarks
     * Causes the simulated player to stop swimming.
     *
     * @worldMutation
     *
     * @throws This function can throw errors.
     */
    stopSwimming(): void;
    /**
     * @remarks
     * Stops using the currently active item.
     *
     * @worldMutation
     *
     * @returns
     * Returns the item that was in use. Undefined if no item was
     * in use.
     * @throws This function can throw errors.
     */
    stopUsingItem(): ItemStack | undefined;
    /**
     * @remarks
     * Causes the simulated player to start swimming.
     *
     * @worldMutation
     *
     * @throws This function can throw errors.
     */
    swim(): void;
    /**
     * @remarks
     * Causes the simulated player to use an item. Does not consume
     * the item. Returns false if the item is on cooldown.
     *
     * @worldMutation
     *
     * @param itemStack
     * Item to use.
     * @throws This function can throw errors.
     */
    useItem(itemStack: ItemStack): boolean;
    /**
     * @remarks
     * Causes the simulated player to hold and use an item in their
     * inventory.
     *
     * @worldMutation
     *
     * @param slot
     * Index of the inventory slot.
     * @throws This function can throw errors.
     */
    useItemInSlot(slot: number): boolean;
    /**
     * @remarks
     * Causes the simulated player to use an item in their
     * inventory on a block. The block at the specified block
     * location must be solid. Returns true if the item was used.
     *
     * @worldMutation
     *
     * @param slot
     * Index of the slot to use.
     * @param blockLocation
     * Location to use the item upon.
     * @param direction
     * Direction to place the specified item within.
     * Defaults to: 1
     * @param faceLocation
     * Location relative to the bottom north-west corner of the
     * block where the item is placed.
     * Defaults to: null
     * @throws This function can throw errors.
     *
     * {@link GameTestError}
     *
     * {@link InvalidEntityError}
     */
    useItemInSlotOnBlock(
        slot: number,
        blockLocation: Vector3,
        direction?: Direction,
        faceLocation?: Vector3,
    ): boolean;
    /**
     * @remarks
     * Causes the simulated player to use an item on a block. The
     * block at the specified block location must be solid. Returns
     * true if the item was used.
     *
     * @worldMutation
     *
     * @param itemStack
     * Item to use.
     * @param blockLocation
     * Location to use the item upon.
     * @param direction
     * Direction to place the specified item within.
     * Defaults to: 1
     * @param faceLocation
     * Location relative to the bottom north-west corner of the
     * block where the item is placed.
     * Defaults to: null
     * @throws This function can throw errors.
     *
     * {@link GameTestError}
     *
     * {@link InvalidEntityError}
     */
    useItemOnBlock(
        itemStack: ItemStack,
        blockLocation: Vector3,
        direction?: Direction,
        faceLocation?: Vector3,
    ): boolean;
}

/**
 * These well-known tags can be used to classify different
 * tests into suites to run.
 */
export class Tags {
    private constructor();
    /**
     * @remarks
     * Indicates that the tagged test should be a part of all
     * suites.
     *
     */
    static readonly suiteAll = 'suite:all';
    /**
     * @remarks
     * Indicates that the tagged test should be a part of an
     * internal (debug) test suite.
     *
     */
    static readonly suiteDebug = 'suite:debug';
    /**
     * @remarks
     * Indicates that the tagged test should be a part of the
     * default test suite.
     *
     */
    static readonly suiteDefault = 'suite:default';
    /**
     * @remarks
     * Indicates that the tagged test should be a part of a suite
     * of disabled tests.
     *
     */
    static readonly suiteDisabled = 'suite:disabled';
    static readonly suiteNextUpdate = 'suite:nextupdate';
}

/**
 * Main class for GameTest functions, with helpers and data for
 * manipulating the respective test. Note that all methods of
 * this class expect BlockLocations and Locations relative to
 * the GameTest structure block.
 */
export class Test {
    private constructor();
    /**
     * @remarks
     * Tests that the condition specified in _condition_ is true.
     * If not, an error with the specified _message_ is thrown.
     *
     * @param condition
     * Expression of the condition to evaluate.
     * @param message
     * Message that is passed if the _condition_ does not evaluate
     * to true.
     * @throws This function can throw errors.
     *
     * {@link GameTestCompletedError}
     *
     * {@link GameTestError}
     */
    assert(condition: boolean, message: string): void;
    /**
     * @remarks
     * Tests that a block of the specified type is present at the
     * specified location. If it is not, an exception is thrown.
     *
     * @param blockType
     * Expected block type.
     * @param blockLocation
     * Location of the block to test at.
     * @param isPresent
     * If true, this function tests whether a block of the
     * specified type is at the location. If false, tests that a
     * block of the specified type is not present.
     * Defaults to: true
     * @throws This function can throw errors.
     *
     * {@link GameTestCompletedError}
     *
     * {@link GameTestError}
     */
    assertBlockPresent(
        blockType: BlockType | string,
        blockLocation: Vector3,
        isPresent?: boolean,
    ): void;
    /**
     * @remarks
     * Tests that a block has a particular state value at the
     * specified location. If it does not have that state value, an
     * exception is thrown.
     *
     * @param blockLocation
     * Location of the block to test at.
     * @param callback
     * Callback function that contains additional tests based on
     * the block at the specified location.
     * @throws This function can throw errors.
     *
     * {@link GameTestCompletedError}
     *
     * {@link GameTestError}
     */
    assertBlockState(blockLocation: Vector3, callback: (arg0: Block) => boolean): void;
    /**
     * @remarks
     * Tests that an entity can reach a particular location.
     * Depending on the value of canReach, throws an exception if
     * the condition is not met.
     *
     * @param mob
     * Entity that you wish to test the location against.
     * @param blockLocation
     * Structure-relative location to test whether the specified
     * mob can reach.
     * @param canReach
     * If true, tests whether the mob can reach the location. If
     * false, tests whether the mob is not able to reach the
     * location.
     * Defaults to: true
     * @throws This function can throw errors.
     *
     * {@link GameTestCompletedError}
     *
     * {@link GameTestError}
     */
    assertCanReachLocation(
        mob: Entity,
        blockLocation: Vector3,
        canReach?: boolean,
    ): void;
    /**
     * @remarks
     * Tests that a container (e.g., a chest) at the specified
     * location contains a specified of item stack. If not, an
     * error is thrown.
     *
     * @param itemStack
     * Represents the type of item to check for. The specified
     * container must contain at least 1 item matching the item
     * type defined in _itemStack_.
     * @param blockLocation
     * Location of the block with a container (for example, a
     * chest) to test the contents of.
     * @throws This function can throw errors.
     *
     * {@link GameTestCompletedError}
     *
     * {@link GameTestError}
     */
    assertContainerContains(itemStack: ItemStack, blockLocation: Vector3): void;
    /**
     * @remarks
     * Tests that a container (e.g., a chest) at the specified
     * location is empty. If not, an error is thrown.
     *
     * @param blockLocation
     * Location of the block with a container (for example, a
     * chest) to test is empty of contents.
     * @throws This function can throw errors.
     *
     * {@link GameTestCompletedError}
     *
     * {@link GameTestError}
     */
    assertContainerEmpty(blockLocation: Vector3): void;
    /**
     * @remarks
     * Tests that an entity has a specific piece of armor equipped.
     * If not, an error is thrown.
     *
     * @param entityTypeIdentifier
     * Identifier of the entity to match (e.g.,
     * 'minecraft:skeleton').
     * @param armorSlot
     * Container slot index to test.
     * @param armorName
     * Name of the armor to look for.
     * @param armorData
     * Data value integer to look for.
     * @param blockLocation
     * Location of the entity with armor to test for.
     * @param hasArmor
     * Whether or not the entity is expected to have the specified
     * armor equipped.
     * Defaults to: true
     * @throws This function can throw errors.
     *
     * {@link GameTestCompletedError}
     *
     * {@link GameTestError}
     */
    assertEntityHasArmor(
        entityTypeIdentifier: string,
        armorSlot: number,
        armorName: string,
        armorData: number,
        blockLocation: Vector3,
        hasArmor?: boolean,
    ): void;
    /**
     * @remarks
     * Tests that an entity has a particular component. If not, an
     * exception is thrown.
     *
     * @param entityTypeIdentifier
     * Identifier of the specified entity (e.g.,
     * 'minecraft:skeleton'). If the namespace is not specified,
     * 'minecraft:' is assumed.
     * @param componentIdentifier
     * Identifier of the component to check for. If the namespace
     * is not specified, 'minecraft:' is assumed.
     * @param blockLocation
     * Location of the block with a container (for example, a
     * chest.)
     * @param hasComponent
     * Determines whether to test that the component exists, or
     * does not.
     * Defaults to: true
     * @throws This function can throw errors.
     *
     * {@link GameTestCompletedError}
     *
     * {@link GameTestError}
     */
    assertEntityHasComponent(
        entityTypeIdentifier: string,
        componentIdentifier: string,
        blockLocation: Vector3,
        hasComponent?: boolean,
    ): void;
    /**
     * @remarks
     * Depending on the value for isPresent, tests that a
     * particular entity is present or not present at the specified
     * location. Depending on the value of isPresent, if the entity
     * is found or not found, an error is thrown.
     *
     * @param entity
     * Specific entity to test for.
     * @param blockLocation
     * Location of the entity to test for.
     * @param isPresent
     * Whether to test that an entity is present or not present at
     * the specified location.
     * Defaults to: true
     * @throws This function can throw errors.
     *
     * {@link GameTestCompletedError}
     *
     * {@link GameTestError}
     */
    assertEntityInstancePresent(
        entity: Entity,
        blockLocation: Vector3,
        isPresent?: boolean,
    ): void;
    /**
     * @remarks
     * Tests that an entity instance is present within the GameTest
     * area. If not, an exception is thrown.
     *
     * @param entity
     * Entity instance to test for.
     * @param isPresent
     * If true, this function tests whether the specified entity is
     * present in the GameTest area. If false, tests that the
     * specified entity is not present.
     * Defaults to: true
     * @throws This function can throw errors.
     *
     * {@link GameTestCompletedError}
     *
     * {@link GameTestError}
     * @seeExample simpleMobTest.ts 9fe10d88
     */
    assertEntityInstancePresentInArea(entity: Entity, isPresent?: boolean): void;
    /**
     * @remarks
     * Depending on the value of isPresent, tests for the presence
     * or non-presence of entity of a specified type at a
     * particular location. If the condition is not met, an
     * exception is thrown.
     *
     * @param entityTypeIdentifier
     * Type of entity to test for (e.g., 'minecraft:skeleton'). If
     * an entity namespace is not specified, 'minecraft:' is
     * assumed.
     * @param blockLocation
     * Location of the entity to test for.
     * @param searchDistance
     * The distance to search for the entity from the
     * blockLocation.
     * Defaults to: 0
     * @param isPresent
     * If true, this function tests whether an entity of the
     * specified type is present. If false, tests that an entity of
     * the specified type is not present.
     * Defaults to: true
     * @throws This function can throw errors.
     *
     * {@link GameTestCompletedError}
     *
     * {@link GameTestError}
     */
    assertEntityPresent(
        entityTypeIdentifier: string,
        blockLocation: Vector3,
        searchDistance?: number,
        isPresent?: boolean,
    ): void;
    /**
     * @remarks
     * Tests that an entity of a specified type is present within
     * the GameTest area. If not, an exception is thrown.
     *
     * @param entityTypeIdentifier
     * Type of entity to test for (e.g., 'minecraft:skeleton'). If
     * an entity namespace is not specified, 'minecraft:' is
     * assumed.
     * @param isPresent
     * If true, this function tests whether an entity of the
     * specified type is present in the GameTest area. If false,
     * tests that an entity of the specified type is not present.
     * Defaults to: true
     * @throws This function can throw errors.
     *
     * {@link GameTestCompletedError}
     *
     * {@link GameTestError}
     * @seeExample simpleMobTest.ts 1a4be3f1
     * @seeExample simpleMobGameTest.ts
     */
    assertEntityPresentInArea(entityTypeIdentifier: string, isPresent?: boolean): void;
    /**
     * @remarks
     * Tests that an entity (e.g., a skeleton) at the specified
     * location has a particular piece of data. If not, an error is
     * thrown.
     *
     * @param blockLocation
     * Location of the entity to look for.
     * @param entityTypeIdentifier
     * Identifier of the entity (e.g., 'minecraft:skeleton') to
     * look for. Note if no namespace is specified, 'minecraft:' is
     * assumed.
     * @param callback
     * Callback function where facets of the selected entity can be
     * tested for. If this callback function returns false or no
     * entity with the specified identifier is found, an exception
     * is thrown.
     * @throws This function can throw errors.
     *
     * {@link GameTestCompletedError}
     *
     * {@link GameTestError}
     */
    assertEntityState(
        blockLocation: Vector3,
        entityTypeIdentifier: string,
        callback: (arg0: Entity) => boolean,
    ): void;
    /**
     * @remarks
     * Depending on the value of isTouching, tests that an entity
     * of a specified type is touching or connected to another
     * entity. If the condition is not met, an exception is thrown.
     *
     * @param entityTypeIdentifier
     * Type of entity to test for (e.g., 'minecraft:skeleton'). If
     * an entity namespace is not specified, 'minecraft:' is
     * assumed.
     * @param location
     * Location of the entity to test for.
     * @param isTouching
     * If true, this function tests whether the entity is touching
     * the specified location. If false, tests that an entity is
     * not testing the specified location.
     * Defaults to: true
     * @throws This function can throw errors.
     *
     * {@link GameTestCompletedError}
     *
     * {@link GameTestError}
     */
    assertEntityTouching(entityTypeIdentifier: string, location: Vector3, isTouching?: boolean): void;
    /**
     * @remarks
     * Depending on the value of isWaterlogged, tests that a block
     * at a location contains water. If the condition is not met,
     * an error is thrown. Pure water blocks are not considered to
     * be waterlogged.
     *
     * @param blockLocation
     * Location of the block to test for.
     * @param isWaterlogged
     * Whether to test that the block at _position_ is expected to
     * be waterlogged.
     * Defaults to: true
     * @throws This function can throw errors.
     *
     * {@link GameTestCompletedError}
     *
     * {@link GameTestError}
     */
    assertIsWaterlogged(blockLocation: Vector3, isWaterlogged?: boolean): void;
    /**
     * @remarks
     * Tests that items of a particular type and count are present
     * within an area. If not, an error is thrown.
     *
     * @param itemType
     * Type of item to look for.
     * @param blockLocation
     * Location to search around for the specified set of items.
     * @param searchDistance
     * Range, in blocks, to aggregate a count of items around. If
     * 0, will only search the particular block at _position_.
     * @param count
     * Number of items, at minimum, to look and test for.
     * @throws This function can throw errors.
     *
     * {@link GameTestCompletedError}
     *
     * {@link GameTestError}
     */
    assertItemEntityCountIs(
        itemType: ItemType | string,
        blockLocation: Vector3,
        searchDistance: number,
        count: number,
    ): void;
    /**
     * @remarks
     * Depending on the value of isPresent, tests whether a
     * particular item entity is present or not at a particular
     * location. If the condition is not met, an exception is
     * thrown.
     *
     * @param itemType
     * Type of item to test for.
     * @param blockLocation
     * Location of the item entity to test for.
     * @param searchDistance
     * Radius in blocks to look for the item entity.
     * Defaults to: 0
     * @param isPresent
     * If true, this function tests whether an item entity of the
     * specified type is present. If false, tests that an item
     * entity of the specified type is not present.
     * Defaults to: true
     * @throws This function can throw errors.
     *
     * {@link GameTestCompletedError}
     *
     * {@link GameTestError}
     */
    assertItemEntityPresent(
        itemType: ItemType | string,
        blockLocation: Vector3,
        searchDistance?: number,
        isPresent?: boolean,
    ): void;
    /**
     * @remarks
     * Tests that Redstone power at a particular location matches a
     * particular value. If not, an exception is thrown.
     *
     * @param blockLocation
     * Location to test.
     * @param power
     * Expected power level.
     * @throws This function can throw errors.
     *
     * {@link GameTestCompletedError}
     *
     * {@link GameTestError}
     */
    assertRedstonePower(blockLocation: Vector3, power: number): void;
    /**
     * @remarks
     * Destroys a block at a particular location.
     *
     * @worldMutation
     *
     * @param blockLocation
     * Location of the block to destroy.
     * @param dropResources
     * Whether to add resources exposed with a particular drop.
     * Defaults to: false
     * @throws This function can throw errors.
     *
     * {@link GameTestCompletedError}
     *
     * {@link GameTestError}
     */
    destroyBlock(blockLocation: Vector3, dropResources?: boolean): void;
    /**
     * @remarks
     * Marks the current test as a failure case.
     *
     * @param errorMessage
     * Error message summarizing the failure condition.
     * @throws This function can throw errors.
     */
    fail(errorMessage: string): void;
    /**
     * @remarks
     * Runs the given callback. If the callback does not throw an
     * exception, the test is marked as a failure.
     *
     * @param callback
     * Callback function that runs. If the function runs
     * successfully, the test is marked as a failure. Typically,
     * this function will have .assertXyz method calls within it.
     * @throws This function can throw errors.
     */
    failIf(callback: () => void): void;
    /**
     * @remarks
     * Gets a block at the specified block location.
     *
     * @worldMutation
     *
     * @param blockLocation
     * Location of the block to retrieve.
     * @throws This function can throw errors.
     *
     * {@link GameTestCompletedError}
     *
     * {@link GameTestError}
     */
    getBlock(blockLocation: Vector3): Block;
    /**
     * @remarks
     * Gets the dimension of this test.
     *
     * @throws This function can throw errors.
     *
     * {@link GameTestCompletedError}
     *
     * {@link GameTestError}
     */
    getDimension(): Dimension;
    /**
     * @remarks
     * If the block at the specified block location is a fence,
     * this returns a helper object with details on how a fence is
     * connected.
     *
     * @worldMutation
     *
     * @param blockLocation
     * Location of the block to retrieve.
     * @throws This function can throw errors.
     *
     * {@link GameTestCompletedError}
     *
     * {@link GameTestError}
     */
    getFenceConnectivity(blockLocation: Vector3): FenceConnectivity;
    /**
     * @remarks
     * Retrieves a sculk spreader object that can be used to
     * control and manage how sculk grows from a block.
     *
     * @worldMutation
     *
     * @param blockLocation
     * Location of the block to retrieve a sculk spreader from.
     * @returns
     * Returns the SculkSpreader or undefined if no SculkSpreader
     * is present on the block.
     * @throws This function can throw errors.
     *
     * {@link GameTestCompletedError}
     *
     * {@link GameTestError}
     */
    getSculkSpreader(blockLocation: Vector3): SculkSpreader | undefined;
    /**
     * @remarks
     * Returns the direction of the current test - see the {@link
     * minecraftserver.Direction} enum for more information on
     * potential values (north, east, south, west - values 2-5).
     *
     * @throws This function can throw errors.
     *
     * {@link GameTestCompletedError}
     */
    getTestDirection(): Direction;
    /**
     * @remarks
     * This asynchronous function will wait for the specified time
     * in ticks before continuing execution.
     *
     * @worldMutation
     *
     * @param tickDelay
     * Amount of time to wait, in ticks.
     * @throws This function can throw errors.
     *
     * {@link GameTestCompletedError}
     */
    idle(tickDelay: number): Promise<void>;
    /**
     * @remarks
     * Returns whether or not the test is currently in the Clean Up
     * step after running.
     *
     * @worldMutation
     *
     */
    isCleaningUp(): boolean;
    /**
     * @remarks
     * Returns whether or not the test has already completed
     *
     * @worldMutation
     *
     */
    isCompleted(): boolean;
    /**
     * @remarks
     * Kills all entities within the GameTest structure.
     *
     * @worldMutation
     *
     * @throws This function can throw errors.
     *
     * {@link GameTestCompletedError}
     *
     * {@link GameTestError}
     */
    killAllEntities(): void;
    /**
     * @remarks
     * @worldMutation
     *
     * @throws This function can throw errors.
     *
     * {@link GameTestCompletedError}
     *
     * {@link GameTestError}
     */
    onPlayerJump(mob: Entity, jumpAmount: number): void;
    /**
     * @remarks
     * Presses a button at a block location.
     *
     * @worldMutation
     *
     * @param blockLocation
     * Location to push the button at.
     * @throws
     * Will throw an error if a button is not present at the
     * specified position.
     *
     * {@link GameTestCompletedError}
     *
     * {@link GameTestError}
     */
    pressButton(blockLocation: Vector3): void;
    /**
     * @remarks
     * Displays the specified message to all players.
     *
     * @worldMutation
     *
     * @param text
     * Message to display.
     * @throws This function can throw errors.
     *
     * {@link GameTestCompletedError}
     *
     * {@link GameTestError}
     */
    print(text: string): void;
    /**
     * @remarks
     * Pulls a lever at a block location.
     *
     * @worldMutation
     *
     * @param blockLocation
     * Location to pull the lever at.
     * @throws
     * Will throw an error if a lever is not present at the
     * specified position.
     *
     * {@link GameTestCompletedError}
     *
     * {@link GameTestError}
     */
    pullLever(blockLocation: Vector3): void;
    /**
     * @remarks
     * Sends a Redstone pulse at a particular location by creating
     * a temporary Redstone block.
     *
     * @worldMutation
     *
     * @param blockLocation
     * Location to pulse Redstone at.
     * @param duration
     * Number of ticks to pulse Redstone.
     * @throws This function can throw errors.
     *
     * {@link GameTestCompletedError}
     *
     * {@link GameTestError}
     */
    pulseRedstone(blockLocation: Vector3, duration: number): void;
    /**
     * @remarks
     * From a BlockLocation, returns a new BlockLocation with
     * coordinates relative to the current GameTest structure
     * block. For example, the relative coordinates for the block
     * above the structure block are (0, 1, 0). Rotation of the
     * GameTest structure is also taken into account.
     *
     * @param worldBlockLocation
     * Absolute location in the world to convert to a relative
     * location.
     * @returns
     * A location relative to the GameTest command block.
     * @throws This function can throw errors.
     *
     * {@link GameTestCompletedError}
     *
     * {@link GameTestError}
     */
    relativeBlockLocation(worldBlockLocation: Vector3): Vector3;
    /**
     * @remarks
     * From a location, returns a new location with coordinates
     * relative to the current GameTest structure block. For
     * example, the relative coordinates for the block above the
     * structure block are (0, 1, 0). Rotation of the GameTest
     * structure is also taken into account.
     *
     * @worldMutation
     *
     * @param worldLocation
     * Absolute location in the world to convert to a relative
     * location.
     * @returns
     * A location relative to the GameTest command block.
     * @throws This function can throw errors.
     *
     * {@link GameTestCompletedError}
     *
     * {@link GameTestError}
     */
    relativeLocation(worldLocation: Vector3): Vector3;
    /**
     * @remarks
     * Removes a simulated player from the world.
     *
     * @worldMutation
     *
     * @param simulatedPlayer
     * Simulated player to remove.
     * @throws This function can throw errors.
     *
     * {@link GameTestCompletedError}
     */
    removeSimulatedPlayer(simulatedPlayer: SimulatedPlayer): void;
    /**
     * @remarks
     * Returns a relative direction given the current rotation of
     * the current test. Passing in Direction.south will return the
     * test direction; Passing in Direction.north will return the
     * opposite of the test direction, and so on.
     *
     * @worldMutation
     *
     * @param direction
     * Direction to translate into a direction relative to the
     * GameTest facing. Passing in Direction.south will return the
     * test direction; Passing in Direction.north will return the
     * opposite of the test direction, and so on.
     * @throws This function can throw errors.
     *
     * {@link GameTestCompletedError}
     *
     * {@link GameTestError}
     */
    rotateDirection(direction: Direction): Direction;
    /**
     * @remarks
     * @worldMutation
     *
     * @throws This function can throw errors.
     *
     * {@link GameTestCompletedError}
     *
     * {@link GameTestError}
     */
    rotateVector(vector: Vector3): Vector3;
    /**
     * @remarks
     * Runs a specific callback after a specified delay of ticks
     *
     * @worldMutation
     *
     * @param delayTicks
     * Number of ticks to delay before running the specified
     * callback.
     * @param callback
     * Callback function to execute.
     * @throws This function can throw errors.
     */
    runAfterDelay(delayTicks: number, callback: () => void): void;
    /**
     * @remarks
     * Runs the given callback after a delay of _tick_ ticks from
     * the start of the GameTest.
     *
     * @worldMutation
     *
     * @param tick
     * Tick (after the start of the GameTest) to run the callback
     * at.
     * @param callback
     * Callback function to execute.
     * @throws This function can throw errors.
     */
    runAtTickTime(tick: number, callback: () => void): void;
    /**
     * @remarks
     * Runs the given callback after the GameTest has completed
     * regardless if the test passed, failed, or timed out.
     *
     * @worldMutation
     *
     * @param callback
     * Callback to execute.
     * @throws This function can throw errors.
     *
     * {@link GameTestCompletedError}
     *
     * {@link GameTestError}
     */
    runOnFinish(callback: () => void): void;
    /**
     * @remarks
     * Sets a block to a particular configuration (a
     * BlockPermutation) at the specified block location.
     *
     * @worldMutation
     *
     * @param blockData
     * Permutation that contains the configuration data for a
     * block.
     * @param blockLocation
     * Location of the block to set.
     * @throws This function can throw errors.
     *
     * {@link GameTestCompletedError}
     *
     * {@link GameTestError}
     */
    setBlockPermutation(blockData: BlockPermutation, blockLocation: Vector3): void;
    /**
     * @remarks
     * Sets a block to a particular type at the specified block
     * location.
     *
     * @worldMutation
     *
     * @param blockType
     * Type of block to set.
     * @param blockLocation
     * Location of the block to set.
     * @throws This function can throw errors.
     *
     * {@link GameTestCompletedError}
     *
     * {@link GameTestError}
     * @seeExample minibiomes.ts
     */
    setBlockType(blockType: BlockType | string, blockLocation: Vector3): void;
    /**
     * @remarks
     * For blocks that are fluid containers - like a cauldron -
     * changes the type of fluid within that container.
     *
     * @worldMutation
     *
     * @param location
     * Location of the fluid container block.
     * @param type
     * Type of fluid to set. See {@link FluidType}
     * for a list of values.
     * @throws This function can throw errors.
     *
     * {@link GameTestCompletedError}
     *
     * {@link GameTestError}
     */
    setFluidContainer(location: Vector3, type: FluidType): void;
    /**
     * @remarks
     * Sets the fuse of an explodable entity.
     *
     * @worldMutation
     *
     * @param entity
     * Entity that is explodable.
     * @param fuseLength
     * Length of time, in ticks, before the entity explodes.
     * @throws This function can throw errors.
     *
     * {@link GameTestCompletedError}
     *
     * {@link GameTestError}
     */
    setTntFuse(entity: Entity, fuseLength: number): void;
    /**
     * @remarks
     * Spawns an entity at a location.
     *
     * @worldMutation
     *
     * @param entityTypeIdentifier
     * Type of entity to create. If no namespace is provided,
     * 'minecraft:' is assumed. Note that an optional initial spawn
     * event can be specified between less than/greater than signs
     * (e.g., namespace:entityType<spawnEvent>).
     * @returns
     * The spawned entity. If the entity cannot be spawned, returns
     * undefined.
     * @throws This function can throw errors.
     *
     * {@link GameTestCompletedError}
     *
     * {@link GameTestError}
     * @seeExample simpleMobTest.ts 1a4be3f1
     * @seeExample simpleMobGameTest.ts
     * @seeExample phantomsShouldFlyFromCats.ts
     * @seeExample minibiomes.ts
     */
    spawn(entityTypeIdentifier: string, blockLocation: Vector3): Entity;
    /**
     * @remarks
     * Spawns an entity at a location.
     *
     * @worldMutation
     *
     * @param entityTypeIdentifier
     * Type of entity to create. If no namespace is provided,
     * 'minecraft:' is assumed. Note that an optional initial spawn
     * event can be specified between less than/greater than signs
     * (e.g., namespace:entityType<spawnEvent>).
     * @returns
     * The spawned entity. If the entity cannot be spawned, returns
     * undefined.
     * @throws This function can throw errors.
     *
     * {@link GameTestCompletedError}
     *
     * {@link GameTestError}
     */
    spawnAtLocation(entityTypeIdentifier: string, location: Vector3): Entity;
    /**
     * @remarks
     * Spawns an item entity at a specified location.
     *
     * @worldMutation
     *
     * @param itemStack
     * ItemStack that describes the item entity to create.
     * @param location
     * Location to create the item entity at.
     * @throws This function can throw errors.
     *
     * {@link GameTestCompletedError}
     *
     * {@link GameTestError}
     */
    spawnItem(itemStack: ItemStack, location: Vector3): Entity;
    /**
     * @remarks
     * Creates a new simulated player within the world.
     *
     * @worldMutation
     *
     * @param blockLocation
     * Location where to spawn the simulated player.
     * @param name
     * Name to give the new simulated player.
     * Defaults to: "Simulated Player"
     * @param gameMode
     * Defaults to: 0
     * @throws This function can throw errors.
     *
     * {@link GameTestCompletedError}
     *
     * {@link GameTestError}
     */
    spawnSimulatedPlayer(
        blockLocation: Vector3,
        name?: string,
        gameMode?: GameMode,
    ): SimulatedPlayer;
    /**
     * @remarks
     * Spawns an entity at a location without any AI behaviors.
     * This method is frequently used in conjunction with methods
     * like .walkTo to create predictable mob actions.
     *
     * @worldMutation
     *
     * @param blockLocation
     * Location where the entity should be spawned.
     * @throws This function can throw errors.
     *
     * {@link GameTestCompletedError}
     *
     * {@link GameTestError}
     */
    spawnWithoutBehaviors(entityTypeIdentifier: string, blockLocation: Vector3): Entity;
    /**
     * @remarks
     * Spawns an entity at a location without any AI behaviors.
     * This method is frequently used in conjunction with methods
     * like .walkTo to create predictable mob actions.
     *
     * @worldMutation
     *
     * @param location
     * Location where the entity should be spawned.
     * @throws This function can throw errors.
     *
     * {@link GameTestCompletedError}
     *
     * {@link GameTestError}
     */
    spawnWithoutBehaviorsAtLocation(
        entityTypeIdentifier: string,
        location: Vector3,
    ): Entity;
    /**
     * @remarks
     * Tests that a particular item entity is present at a
     * particular location. If not, an exception is thrown.
     *
     * @worldMutation
     *
     * @param blockLocation
     * BlockLocation containing a multiface block.
     * @param fromFace
     * Face to spread from. This face must already be set.
     * @param direction
     * Direction to spread. Use the Minecraft.Direction enum to
     * specify a direction.
     * @throws This function can throw errors.
     *
     * {@link GameTestCompletedError}
     *
     * {@link GameTestError}
     */
    spreadFromFaceTowardDirection(
        blockLocation: Vector3,
        fromFace: Direction,
        direction: Direction,
    ): void;
    /**
     * @remarks
     * Creates a new GameTestSequence - A set of steps that play
     * out sequentially within a GameTest.
     *
     * @worldMutation
     *
     * @returns
     * A new GameTestSequence with chaining methods that facilitate
     * creating a set of steps.
     * @throws This function can throw errors.
     *
     * {@link GameTestCompletedError}
     */
    startSequence(): GameTestSequence;
    /**
     * @remarks
     * Marks the current test as a success case.
     *
     * @worldMutation
     *
     * @throws This function can throw errors.
     */
    succeed(): void;
    /**
     * @remarks
     * Runs the given callback. If the callback does not throw an
     * exception, the test is marked as a success.
     *
     * @worldMutation
     *
     * @param callback
     * Callback function that runs. If the function runs
     * successfully, the test is marked as a success. Typically,
     * this function will have .assertXyz method calls within it.
     * @throws This function can throw errors.
     */
    succeedIf(callback: () => void): void;
    /**
     * @remarks
     * Marks the test as a success at the specified tick.
     *
     * @worldMutation
     *
     * @param tick
     * Tick after the start of the GameTest to mark the test as
     * successful.
     * @throws This function can throw errors.
     */
    succeedOnTick(tick: number): void;
    /**
     * @remarks
     * Runs the given callback at _tick_ ticks after the start of
     * the test. If the callback does not throw an exception, the
     * test is marked as a failure.
     *
     * @worldMutation
     *
     * @param tick
     * Tick after the start of the GameTest to run the testing
     * callback at.
     * @param callback
     * Callback function that runs. If the function runs
     * successfully, the test is marked as a success.
     * @throws This function can throw errors.
     */
    succeedOnTickWhen(tick: number, callback: () => void): void;
    /**
     * @remarks
     * Runs the given callback every tick. When the callback
     * successfully executes, the test is marked as a success.
     * Specifically, the test will succeed when the callback does
     * not throw an exception.
     *
     * @worldMutation
     *
     * @param callback
     * Testing callback function that runs. If the function runs
     * successfully, the test is marked as a success.
     * @throws This function can throw errors.
     * @seeExample simpleMobGameTest.ts
     */
    succeedWhen(callback: () => void): void;
    /**
     * @remarks
     * Depending on the condition of isPresent, tests for the
     * presence of a block of a particular type on every tick. When
     * the specified block of a type is found or not found
     * (depending on isPresent), the test is marked as a success.
     *
     * @worldMutation
     *
     * @param blockType
     * Type of block to test for.
     * @param blockLocation
     * Location of the block to test at.
     * @param isPresent
     * If true, this function tests whether a block of the
     * specified type is present. If false, tests that a block of
     * the specified type is not present.
     * Defaults to: true
     * @throws This function can throw errors.
     *
     * {@link GameTestCompletedError}
     *
     * {@link GameTestError}
     */
    succeedWhenBlockPresent(
        blockType: BlockType | string,
        blockLocation: Vector3,
        isPresent?: boolean,
    ): void;
    /**
     * @remarks
     * Tests for the presence of a component on every tick.
     * Depending on the value of hasComponent, when the specified
     * component is found, the test is marked as a success.
     *
     * @worldMutation
     *
     * @param entityTypeIdentifier
     * Type of entity to look for. If no namespace is specified,
     * 'minecraft:' is assumed.
     * @param componentIdentifier
     * Type of component to test for the presence of. If no
     * namespace is specified, 'minecraft:' is assumed.
     * @param blockLocation
     * Block location of the entity to test.
     * @param hasComponent
     * If true, this function tests for the presence of a
     * component. If false, this function tests for the lack of a
     * component.
     * @throws This function can throw errors.
     */
    succeedWhenEntityHasComponent(
        entityTypeIdentifier: string,
        componentIdentifier: string,
        blockLocation: Vector3,
        hasComponent: boolean,
    ): void;
    /**
     * @remarks
     * Depending on the value of isPresent, tests for the presence
     * of an entity on every tick. When an entity of the specified
     * type is found or not found (depending on isPresent), the
     * test is marked as a success.
     *
     * @worldMutation
     *
     * @param entityTypeIdentifier
     * Type of entity to test for (e.g., 'minecraft:skeleton'). If
     * an entity namespace is not specified, 'minecraft:' is
     * assumed.
     * @param blockLocation
     * Location of the entity to test for.
     * @param isPresent
     * If true, this function tests whether an entity of the
     * specified type is present. If false, tests that an entity of
     * the specified type is not present.
     * Defaults to: true
     * @throws This function can throw errors.
     * @seeExample phantomsShouldFlyFromCats.ts
     * @seeExample minibiomes.ts
     */
    succeedWhenEntityPresent(
        entityTypeIdentifier: string,
        blockLocation: Vector3,
        isPresent?: boolean,
    ): void;
    /**
     * @remarks
     * Triggers a block event from a fixed list of available block
     * events.
     *
     * @worldMutation
     *
     * @param event
     * Event to trigger. Valid values include minecraft:drip,
     * minecraft:grow_stalagtite, minecraft:grow_stalagmite,
     * minecraft:grow_up, minecraft:grow_down and
     * minecraft:grow_sideways.
     * @param eventParameters
     * Defaults to: []
     * @throws This function can throw errors.
     *
     * {@link GameTestCompletedError}
     *
     * {@link GameTestError}
     */
    triggerInternalBlockEvent(blockLocation: Vector3, event: string, eventParameters?: number[]): void;
    /**
     * @remarks
     * This asynchronous function will wait until the code in the
     * specified callback successfully completes. until can be used
     * in conjunction with .assert functions to evaluate that a
     * condition is true.
     *
     * @worldMutation
     *
     * @param callback
     * Function with code to evaluate.
     * @throws This function can throw errors.
     *
     * {@link GameTestCompletedError}
     */
    until(callback: () => void): Promise<void>;
    /**
     * @remarks
     * Forces a mob to walk to a particular location. Usually used
     * in conjunction with methods like .spawnWithoutBehaviors to
     * have more predictable mob behaviors. Mobs will stop
     * navigation as soon as they intersect the target location.
     *
     * @worldMutation
     *
     * @param mob
     * Mob entity to give orders to.
     * @param blockLocation
     * Location where the entity should be walk to.
     * @param speedModifier
     * Adjustable modifier to the mob's walking speed.
     * Defaults to: 1
     * @throws This function can throw errors.
     *
     * {@link GameTestCompletedError}
     *
     * {@link GameTestError}
     */
    walkTo(mob: Entity, blockLocation: Vector3, speedModifier?: number): void;
    /**
     * @remarks
     * Forces a mob to walk to a particular location. Usually used
     * in conjunction with methods like .spawnWithoutBehaviors to
     * have more predictable mob behaviors. Mobs will stop
     * navigation as soon as they intersect the target location.
     *
     * @worldMutation
     *
     * @param mob
     * Mob entity to give orders to.
     * @param location
     * Location where the entity should be walk to.
     * @param speedModifier
     * Adjustable modifier to the mob's walking speed.
     * Defaults to: 1
     * @throws This function can throw errors.
     *
     * {@link GameTestCompletedError}
     *
     * {@link GameTestError}
     */
    walkToLocation(mob: Entity, location: Vector3, speedModifier?: number): void;
    /**
     * @remarks
     * From a BlockLocation with coordinates relative to the
     * GameTest structure block, returns a new BlockLocation with
     * coordinates relative to world. Rotation of the GameTest
     * structure is also taken into account.
     *
     * @param relativeBlockLocation
     * Location relative to the GameTest command block.
     * @returns
     * An absolute location relative to the GameTest command block.
     * @throws This function can throw errors.
     *
     * {@link GameTestCompletedError}
     *
     * {@link GameTestError}
     */
    worldBlockLocation(relativeBlockLocation: Vector3): Vector3;
    /**
     * @remarks
     * From a location with coordinates relative to the GameTest
     * structure block, returns a new location with coordinates
     * relative to world. Rotation of the GameTest structure is
     * also taken into account.
     *
     * @param relativeLocation
     * Location relative to the GameTest command block.
     * @returns
     * An absolute location relative to the GameTest command block.
     * @throws This function can throw errors.
     *
     * {@link GameTestCompletedError}
     *
     * {@link GameTestError}
     */
    worldLocation(relativeLocation: Vector3): Vector3;
}

export interface GameTestErrorContext {
    absolutePosition: Vector3;
    relativePosition: Vector3;
    tickCount: number;
}

export interface MoveToOptions {
    faceTarget?: boolean;
    speed?: number;
}

/**
 * Data pertaining to a specific player Persona piece.
 */
export interface PlayerPersonaPiece {
    id: string;
    isDefaultPiece?: boolean;
    packId: string;
    productId: string;
    type: PersonaPieceType;
}

/**
 * Data pertaining to a player's configured skin.
 */
export interface PlayerSkinData {
    armSize?: PersonaArmSize;
    personaPieces?: PlayerPersonaPiece[];
    skinColor?: RGB;
}

// @ts-ignore Class inheritance allowed for native defined classes
export class GameTestCompletedError extends Error {
    private constructor();
    /**
     * @remarks
     * @earlyExecution
     *
     */
    readonly reason: GameTestCompletedErrorReason;
}

// @ts-ignore Class inheritance allowed for native defined classes
export class GameTestError extends Error {
    private constructor();
    /**
     * @remarks
     * @earlyExecution
     *
     */
    readonly context?: GameTestErrorContext;
    /**
     * @remarks
     * @earlyExecution
     *
     */
    readonly messageParameters: string[];
    /**
     * @remarks
     * @earlyExecution
     *
     */
    readonly type: GameTestErrorType;
}

/**
 * @remarks
 * Returns data about a player's skin.
 *
 * @worldMutation
 *
 * @param player
 * The player who's skin is returned.
 * @throws This function can throw errors.
 *
 * {@link InvalidArgumentError}
 *
 * {@link InvalidEntityError}
 */
export function getPlayerSkin(player: Player): PlayerSkinData;
/**
 * @remarks
 * Registers a new GameTest function. This GameTest will become
 * available in Minecraft via `/gametest run
 * [testClassName]:[testName]`.
 *
 * @worldMutation
 *
 * @earlyExecution
 *
 * @param testClassName
 * Name of the class of tests this test should be a part of.
 * @param testName
 * Name of this specific test.
 * @param testFunction
 * Implementation of the test function.
 * @returns
 * Returns a {@link RegistrationBuilder} object where
 * additional options for this test can be specified via
 * builder methods.
 * @seeExample simpleMobGameTest.ts
 */
export function register(
    testClassName: string,
    testName: string,
    testFunction: (arg0: Test) => void,
): RegistrationBuilder;
/**
 * @remarks
 * Registers a new GameTest function that is designed for
 * asynchronous execution. This GameTest will become available
 * in Minecraft via `/gametest run [testClassName]:[testName]`.
 *
 * @worldMutation
 *
 * @earlyExecution
 *
 * @param testClassName
 * Name of the class of tests this test should be a part of.
 * @param testName
 * Name of this specific test.
 * @param testFunction
 * Implementation of the test function.
 * @returns
 * Returns a {@link RegistrationBuilder} object where
 * additional options for this test can be specified via
 * builder methods.
 * @seeExample simpleMobAsyncTest.ts
 */
export function registerAsync(
    testClassName: string,
    testName: string,
    testFunction: (arg0: Test) => Promise<void>,
): RegistrationBuilder;
/**
 * @remarks
 * Sets a callback that is called after the batch gets called.
 * This will overwrite previously set callbacks for this batch.
 *
 * @worldMutation
 *
 * @earlyExecution
 *
 * @param batchName
 * Name of the batch of tests the callback will run after.
 * @throws This function can throw errors.
 *
 * {@link GameTestError}
 */
export function setAfterBatchCallback(batchName: string, batchCallback: () => void): void;
/**
 * @remarks
 * Sets a callback that is called before the batch gets called.
 * This will overwrite previously set callbacks for this batch.
 *
 * @worldMutation
 *
 * @earlyExecution
 *
 * @param batchName
 * Name of the batch of tests the callback will run before.
 * @throws This function can throw errors.
 *
 * {@link GameTestError}
 */
export function setBeforeBatchCallback(batchName: string, batchCallback: () => void): void;
/**
 * @remarks
 * Spawns a simulated player that isn't associated to a
 * specific {@link Test}.  You can use {@link
 * SimulatedPlayer.remove} to remove the player from the world.
 *
 * @worldMutation
 *
 * @param location
 * The location in which to spawn the player.
 * @param name
 * The name for the player.
 * @param gameMode
 * The game mode for the player.
 * @throws This function can throw errors.
 *
 * {@link EngineError}
 */
export function spawnSimulatedPlayer(
    location: DimensionLocation,
    name: string,
    gameMode: GameMode,
): SimulatedPlayer;
