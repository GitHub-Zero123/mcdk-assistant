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
 * 包含了大量用于修改 Minecraft 世界（实体、方块、维度等）的类型。
 * 
 * Contains many types related to manipulating a Minecraft
 * world, including entities, blocks, dimensions, and more.
 *
 * Manifest Details
 * ```json
 * {
 *   "module_name": "@minecraft/server",
 *   "version": "2.10.0-beta"
 * }
 * ```
 *
 */
import { ArgumentOutOfBoundsError, EngineError, InvalidArgumentError, NumberRange, PropertyOutOfBoundsError, UnsupportedFunctionalityError } from '@minecraft/common';
// @ts-ignore Optional types-only package, will decay to any if @minecraft/vanilla-data isn't installed
import type { BlockStateMapping, BlockStateSuperset, MinecraftBlockTypes, MinecraftEntityTypes, MinecraftFeatureTypes, MinecraftItemTypes, MinecraftPotionDeliveryTypes, MinecraftPotionEffectTypes } from '@minecraft/vanilla-data';
/**
 * Specifies different targeting modes for use in aim-assist.
 */
export enum AimAssistTargetMode {
    /**
     * @remarks
     * Angle based targeting.
     *
     */
    Angle = 'Angle',
    /**
     * @remarks
     * Distance based targeting.
     *
     */
    Distance = 'Distance',
}

/**
 * The types of block components that are accessible via
 * function Block.getComponent.
 */
export enum BlockComponentTypes {
    /**
     * @beta
     */
    DynamicProperties = 'minecraft:dynamic_properties',
    FluidContainer = 'minecraft:fluid_container',
    /**
     * @beta
     */
    Instrument = 'minecraft:instrument_sound',
    /**
     * @remarks
     * Represents the inventory of a block in the world. Used with
     * blocks like chests.
     *
     */
    Inventory = 'minecraft:inventory',
    /**
     * @remarks
     * Represents the color of a block when displayed on a map.
     *
     */
    MapColor = 'minecraft:map_color',
    /**
     * @remarks
     * Represents a block that can move (such as a piston).
     *
     */
    Movable = 'minecraft:movable',
    /**
     * @remarks
     * When present, this block has piston-like behavior. Contains
     * additional properties for discovering block piston state.
     *
     */
    Piston = 'minecraft:piston',
    /**
     * @remarks
     * Represents a how a block interacts with precipitation (such
     * as rain or snow).
     *
     */
    PrecipitationInteractions = 'minecraft:precipitation_interactions',
    /**
     * @remarks
     * Represents a block that can play a record.
     *
     */
    RecordPlayer = 'minecraft:record_player',
    /**
     * @remarks
     * Represents a block that can output a redstone signal.
     *
     */
    RedstoneProducer = 'minecraft:redstone_producer',
    /**
     * @remarks
     * Represents a block that can display text on it.
     *
     */
    Sign = 'minecraft:sign',
}

/**
 * An enumeration describing the state of a block piston.
 */
export enum BlockPistonState {
    /**
     * @remarks
     * Whether the piston is fully expanded.
     *
     */
    Expanded = 'Expanded',
    /**
     * @remarks
     * Whether the piston is in the process of expanding.
     *
     */
    Expanding = 'Expanding',
    /**
     * @remarks
     * Whether the piston is fully retracted.
     *
     */
    Retracted = 'Retracted',
    /**
     * @remarks
     * Whether the piston is in the process of retracting.
     *
     */
    Retracting = 'Retracting',
}

/**
 * Description of the resulting intersection test on two
 * BlockVolume objects
 */
export enum BlockVolumeIntersection {
    /**
     * @remarks
     * Volume B has no intersection points with Volume A
     *
     */
    Disjoint = 0,
    /**
     * @remarks
     * Volume B resides completely inside Volume A
     *
     */
    Contains = 1,
    /**
     * @remarks
     * Volume B partially intersects Volume A
     *
     */
    Intersects = 2,
}

/**
 * An enum of error reasons relating to using {@link
 * ItemBookComponent}.
 */
export enum BookErrorReason {
    /**
     * @remarks
     * The requested page content exceeds the max page length of
     * 256.
     *
     */
    ExceedsMaxPageLength = 'ExceedsMaxPageLength',
    /**
     * @remarks
     * The page could not be created as it would exceed the max
     * page count of 50.
     *
     */
    ExceedsMaxPages = 'ExceedsMaxPages',
    /**
     * @remarks
     * The title being signed exceeds the maximum title length of
     * 16.
     *
     */
    ExceedsTitleLength = 'ExceedsTitleLength',
}

/**
 * The state of a button on a keyboard, controller, or touch
 * interface.
 */
export enum ButtonState {
    Pressed = 'Pressed',
    Released = 'Released',
}

/**
 * @beta
 * Represents the type of shake to apply to the camera.
 */
export enum CameraShakeType {
    /**
     * @remarks
     * A positional shake that moves the camera along its axes.
     *
     */
    Positional = 'Positional',
    /**
     * @remarks
     * A rotational shake that rotates the camera around its axes.
     *
     */
    Rotational = 'Rotational',
}

/**
 * The required permission level to execute the custom command.
 */
export enum CommandPermissionLevel {
    /**
     * @remarks
     * Anything can run this level.
     *
     */
    Any = 0,
    /**
     * @remarks
     * Any operator can run this command, including command blocks.
     *
     */
    GameDirectors = 1,
    /**
     * @remarks
     * Any operator can run this command, but NOT command blocks.
     *
     */
    Admin = 2,
    /**
     * @remarks
     * Any server host can run this command.
     *
     */
    Host = 3,
    /**
     * @remarks
     * Only dedicated server can run this command.
     *
     */
    Owner = 4,
}

/**
 * @beta
 * The Action enum determines how the CompoundBlockVolume
 * considers the associated CompoundBlockVolumeItem when
 * performing inside/outside calculations.
 */
export enum CompoundBlockVolumeAction {
    /**
     * @remarks
     * The associated BlockVolume is considered a positive space,
     * and any intersection tests are considered hits
     *
     */
    Add = 0,
    /**
     * @remarks
     * The associated BlockVolume is considered a negative or void
     * space, and any intersection tests are considered misses.
     * Using the Subtract action, it is possible to `punch holes`
     * in block volumes so that any intersection tests may pass
     * through such spaces
     *
     */
    Subtract = 1,
}

/**
 * @beta
 * An enum describing the relativity of the
 * CompoundBlockVolumeItem, relative to the parent
 * CompoundVolume.
 */
export enum CompoundBlockVolumePositionRelativity {
    /**
     * @remarks
     * The locations within the associated BlockVolume are relative
     * to the CompoundBlockVolume to which they were added
     *
     */
    Relative = 0,
    /**
     * @remarks
     * The locations within the associated BlockVolume are in
     * absolute world space
     *
     */
    Absolute = 1,
}

/**
 * Reasons that the {@link
 * @minecraft/server.ContainerRulesError} was thrown.
 */
export enum ContainerRulesErrorReason {
    /**
     * @remarks
     * Thrown when trying to add item that was defined in {@link
     * ContainerRules.bannedItems}.
     *
     */
    BannedItem = 'BannedItem',
    /**
     * @remarks
     * Thrown when trying to add item with `Storage Item` component
     * to container with {@link
     * ContainerRules.allowNestedStorageItems} set to false.
     *
     */
    NestedStorageItem = 'NestedStorageItem',
    /**
     * @remarks
     * Thrown when trying to add item not defined in non-empty
     * {@link ContainerRules.allowedItems}.
     *
     */
    NotAllowedItem = 'NotAllowedItem',
    /**
     * @remarks
     * Thrown when trying to add item that pushed the containers
     * weight over the {@link ContainerRules.weightLimit}.
     *
     */
    OverWeightLimit = 'OverWeightLimit',
    /**
     * @remarks
     * Thrown when trying to add item with zero weight defined by
     * the `Storage Weight Modifier` component to container with a
     * defined {@link ContainerRules.weightLimit}
     *
     */
    ZeroWeightItem = 'ZeroWeightItem',
}

/**
 * Control Scheme types which define how the player moves in
 * response to player inputs.
 * See the following page for more details on control schemes:
 * https://learn.microsoft.com/en-us/minecraft/creator/documents/controlschemes
 */
export enum ControlScheme {
    CameraRelative = 'CameraRelative',
    CameraRelativeStrafe = 'CameraRelativeStrafe',
    LockedPlayerRelativeStrafe = 'LockedPlayerRelativeStrafe',
    PlayerRelative = 'PlayerRelative',
    PlayerRelativeStrafe = 'PlayerRelativeStrafe',
}

/**
 * Reason why custom command registration failed.
 */
export enum CustomCommandErrorReason {
    /**
     * @remarks
     * Command name already registered.
     *
     */
    AlreadyRegistered = 'AlreadyRegistered',
    /**
     * @remarks
     * Custom Command references an enum that has not been
     * registered.
     *
     */
    EnumDependencyMissing = 'EnumDependencyMissing',
    /**
     * @remarks
     * Supplied Custom Command namespace does not match previous
     * registrations for this add-on.
     *
     */
    NamespaceMismatch = 'NamespaceMismatch',
    /**
     * @remarks
     * Too many command parameters defined in CustomCommand.
     *
     */
    ParameterLimit = 'ParameterLimit',
    /**
     * @remarks
     * Custom command registry can not be accessed after world
     * initialized event.
     *
     */
    RegistryInvalid = 'RegistryInvalid',
    /**
     * @remarks
     * Command parameters cannot be redefined during reload. Only
     * the script closure itself can be changed.
     *
     */
    RegistryReadOnly = 'RegistryReadOnly',
    /**
     * @beta
     * @remarks
     * Non enum type command parameters cannot use enumName.
     *
     */
    UnexpectedEnumName = 'UnexpectedEnumName',
}

/**
 * The types of paramaters accepted by a custom command.
 */
export enum CustomCommandParamType {
    /**
     * @remarks
     * Block type parameter provides a {@link BlockType}.
     *
     */
    BlockType = 'BlockType',
    /**
     * @remarks
     * Boolean parameter.
     *
     */
    Boolean = 'Boolean',
    /**
     * @remarks
     * Entity selector parameter provides an {@link Entity}.
     *
     */
    EntitySelector = 'EntitySelector',
    /**
     * @remarks
     * Entity type parameter provides an {@link EntityType}.
     *
     */
    EntityType = 'EntityType',
    /**
     * @remarks
     * Command enum parameter.
     *
     */
    Enum = 'Enum',
    /**
     * @remarks
     * Float parameter.
     *
     */
    Float = 'Float',
    /**
     * @remarks
     * Integer parameter.
     *
     */
    Integer = 'Integer',
    /**
     * @remarks
     * Item type parameter provides an {@link ItemType}.
     *
     */
    ItemType = 'ItemType',
    /**
     * @remarks
     * Location parameter provides a {@link
     * @minecraft/server.Vector3}.
     *
     */
    Location = 'Location',
    /**
     * @remarks
     * Player selector parameter provides a {@link Player}.
     *
     */
    PlayerSelector = 'PlayerSelector',
    /**
     * @remarks
     * String parameter.
     *
     */
    String = 'String',
}

/**
 * Who executed the command.
 */
export enum CustomCommandSource {
    /**
     * @remarks
     * Command originated from a command block.
     *
     */
    Block = 'Block',
    /**
     * @remarks
     * Command originated from an entity or player.
     *
     */
    Entity = 'Entity',
    NPCDialogue = 'NPCDialogue',
    /**
     * @remarks
     * Command originated from the server.
     *
     */
    Server = 'Server',
}

export enum CustomCommandStatus {
    Success = 0,
    Failure = 1,
}

export enum CustomComponentNameErrorReason {
    NoNamespace = 1,
    DisallowedNamespace = 2,
}

/**
 * An enumeration for the various difficulty levels of
 * Minecraft.
 */
export enum Difficulty {
    /**
     * @remarks
     * Easy difficulty level.
     *
     */
    Easy = 'Easy',
    /**
     * @remarks
     * Hard difficulty level.
     *
     */
    Hard = 'Hard',
    /**
     * @remarks
     * Normal difficulty level.
     *
     */
    Normal = 'Normal',
    /**
     * @remarks
     * Peaceful difficulty level.
     *
     */
    Peaceful = 'Peaceful',
}

/**
 * 通用枚举之一。表示朝向。
 * 
 * A general purpose relative direction enumeration.
 */
export enum Direction {
    /**
     * @remarks
     * 表示下方（y - 1）。
     * 
     * Returns the {@link Block} beneath (y - 1) of this item.
     *
     */
    Down = 'Down',
    /**
     * @remarks
     * 表示东方（x + 1）。
     * 
     * Returns the {@link Block} to the east (x + 1) of this item.
     *
     */
    East = 'East',
    /**
     * @remarks
     * 表示北方（z - 1）。
     * 
     * Returns the {@link Block} to the east (z + 1) of this item.
     *
     */
    North = 'North',
    /**
     * @remarks
     * 表示南方（z + 1）。
     * 
     * Returns the {@link Block} to the south (z - 1) of this item.
     *
     */
    South = 'South',
    /**
     * @remarks
     * 表示上方（y + 1）。
     * 
     * Returns the {@link Block} above (y + 1) of this item.
     *
     */
    Up = 'Up',
    /**
     * @remarks
     * 表示西方（x - 1）。
     * 
     * Returns the {@link Block} to the west (x - 1) of this item.
     *
     */
    West = 'West',
}

/**
 * 表示记分板分数的显示方式。
 * 
 * Specifies a mechanism for displaying scores on a scoreboard.
 */
export enum DisplaySlotId {
    /**
     * @remarks
     * 记分项上的分数将显示在玩家名称牌下方。
     * 
     * Displays the score below the player's name.
     *
     */
    BelowName = 'BelowName',
    /**
     * @remarks
     * 该记分项与对应的分数持有者将显示在暂停菜单上。
     * 
     * Displays the score as a list on the pause screen.
     *
     */
    List = 'List',
    /**
     * @remarks
     * 记分项将在屏幕右侧显示。
     * 
     * Displays the score on the side of the player's screen.
     *
     */
    Sidebar = 'Sidebar',
}

/**
 * Specifies different colors for use as dye.
 */
export enum DyeColor {
    /**
     * @remarks
     * Black dye color.
     *
     */
    Black = 'Black',
    /**
     * @remarks
     * Blue dye color.
     *
     */
    Blue = 'Blue',
    /**
     * @remarks
     * Brown dye color.
     *
     */
    Brown = 'Brown',
    /**
     * @remarks
     * Cyan dye color.
     *
     */
    Cyan = 'Cyan',
    /**
     * @remarks
     * Gray dye color.
     *
     */
    Gray = 'Gray',
    /**
     * @remarks
     * Green dye color.
     *
     */
    Green = 'Green',
    /**
     * @remarks
     * Light blue dye color.
     *
     */
    LightBlue = 'LightBlue',
    /**
     * @remarks
     * Lime dye color.
     *
     */
    Lime = 'Lime',
    /**
     * @remarks
     * Magenta dye color.
     *
     */
    Magenta = 'Magenta',
    /**
     * @remarks
     * Orange dye color.
     *
     */
    Orange = 'Orange',
    /**
     * @remarks
     * Pink dye color.
     *
     */
    Pink = 'Pink',
    /**
     * @remarks
     * Purple dye color.
     *
     */
    Purple = 'Purple',
    /**
     * @remarks
     * Red dye color.
     *
     */
    Red = 'Red',
    /**
     * @remarks
     * Silver dye color.
     *
     */
    Silver = 'Silver',
    /**
     * @remarks
     * White dye color.
     *
     */
    White = 'White',
    /**
     * @remarks
     * Yellow dye color.
     *
     */
    Yellow = 'Yellow',
}

export enum EasingType {
    InBack = 'InBack',
    InBounce = 'InBounce',
    InCirc = 'InCirc',
    InCubic = 'InCubic',
    InElastic = 'InElastic',
    InExpo = 'InExpo',
    InOutBack = 'InOutBack',
    InOutBounce = 'InOutBounce',
    InOutCirc = 'InOutCirc',
    InOutCubic = 'InOutCubic',
    InOutElastic = 'InOutElastic',
    InOutExpo = 'InOutExpo',
    InOutQuad = 'InOutQuad',
    InOutQuart = 'InOutQuart',
    InOutQuint = 'InOutQuint',
    InOutSine = 'InOutSine',
    InQuad = 'InQuad',
    InQuart = 'InQuart',
    InQuint = 'InQuint',
    InSine = 'InSine',
    Linear = 'Linear',
    OutBack = 'OutBack',
    OutBounce = 'OutBounce',
    OutCirc = 'OutCirc',
    OutCubic = 'OutCubic',
    OutElastic = 'OutElastic',
    OutExpo = 'OutExpo',
    OutQuad = 'OutQuad',
    OutQuart = 'OutQuart',
    OutQuint = 'OutQuint',
    OutSine = 'OutSine',
    Spring = 'Spring',
}

export enum EnchantmentSlot {
    ArmorFeet = 'ArmorFeet',
    ArmorHead = 'ArmorHead',
    ArmorLegs = 'ArmorLegs',
    ArmorTorso = 'ArmorTorso',
    Axe = 'Axe',
    Bow = 'Bow',
    CarrotStick = 'CarrotStick',
    CosmeticHead = 'CosmeticHead',
    Crossbow = 'Crossbow',
    Elytra = 'Elytra',
    FishingRod = 'FishingRod',
    Flintsteel = 'Flintsteel',
    Hoe = 'Hoe',
    MeleeSpear = 'MeleeSpear',
    Pickaxe = 'Pickaxe',
    Shears = 'Shears',
    Shield = 'Shield',
    Shovel = 'Shovel',
    Spear = 'Spear',
    Sword = 'Sword',
}

/**
 * The entity's attach location point. Contains points such as
 * head, body, leg, etc to attach the camera to.
 */
export enum EntityAttachPoint {
    Body = 'Body',
    BreathingPoint = 'BreathingPoint',
    DropAttachPoint = 'DropAttachPoint',
    ExplosionPoint = 'ExplosionPoint',
    Eyes = 'Eyes',
    Feet = 'Feet',
    Head = 'Head',
    Mouth = 'Mouth',
    WeaponAttachPoint = 'WeaponAttachPoint',
}

/**
 * The types of entity components that are accessible via
 * function Entity.getComponent.
 */
export enum EntityComponentTypes {
    /**
     * @remarks
     * When added, this component makes the entity spawn with a
     * rider of the specified entityType.
     *
     */
    AddRider = 'minecraft:addrider',
    /**
     * @remarks
     * Adds a timer for the entity to grow up. It can be
     * accelerated by giving the entity the items it likes as
     * defined by feedItems.
     *
     */
    Ageable = 'minecraft:ageable',
    /**
     * @remarks
     * Defines what blocks this entity can breathe in and gives
     * them the ability to suffocate.
     *
     */
    Breathable = 'minecraft:breathable',
    /**
     * @remarks
     * When added, this component signifies that the entity can
     * climb up ladders.
     *
     */
    CanClimb = 'minecraft:can_climb',
    /**
     * @remarks
     * When added, this component signifies that the entity can
     * fly, and the pathfinder won't be restricted to paths where a
     * solid block is required underneath it.
     *
     */
    CanFly = 'minecraft:can_fly',
    /**
     * @remarks
     * When added, this component signifies that the entity can
     * power jump like the horse does within Minecraft.
     *
     */
    CanPowerJump = 'minecraft:can_power_jump',
    /**
     * @remarks
     * Defines the entity's color. Only works on certain entities
     * that have predefined color values (e.g., sheep, llama,
     * shulker).
     *
     */
    Color = 'minecraft:color',
    /**
     * @remarks
     * Defines the entity's secondary color. Only works on certain
     * entities that have predefined secondary color values (e.g.,
     * tropical fish).
     *
     */
    Color2 = 'minecraft:color2',
    CursorInventory = 'minecraft:cursor_inventory',
    /**
     * @remarks
     * Represents this entity's ender inventory properties.
     *
     */
    EnderInventory = 'minecraft:ender_inventory',
    /**
     * @remarks
     * Provides access to a mob's equipment slots. This component
     * exists for all mob entities.
     *
     */
    Equippable = 'minecraft:equippable',
    /**
     * @remarks
     * When added, this component signifies that this entity
     * doesn't take damage from fire.
     *
     */
    FireImmune = 'minecraft:fire_immune',
    /**
     * @remarks
     * When added, this component signifies that this entity can
     * float in liquid blocks.
     *
     */
    FloatsInLiquid = 'minecraft:floats_in_liquid',
    /**
     * @remarks
     * Represents the flying speed of an entity.
     *
     */
    FlyingSpeed = 'minecraft:flying_speed',
    /**
     * @remarks
     * Defines how much friction affects this entity.
     *
     */
    FrictionModifier = 'minecraft:friction_modifier',
    /**
     * @remarks
     * Defines the interactions with this entity for healing it.
     *
     */
    Healable = 'minecraft:healable',
    /**
     * @remarks
     * Defines the health properties of an entity.
     *
     */
    Health = 'minecraft:health',
    /**
     * @remarks
     * Defines this entity's inventory properties.
     *
     */
    Inventory = 'minecraft:inventory',
    /**
     * @remarks
     * When added, this component signifies that this entity is a
     * baby.
     *
     */
    IsBaby = 'minecraft:is_baby',
    /**
     * @remarks
     * When added, this component signifies that this entity is
     * charged.
     *
     */
    IsCharged = 'minecraft:is_charged',
    /**
     * @remarks
     * When added, this component signifies that this entity is
     * currently carrying a chest.
     *
     */
    IsChested = 'minecraft:is_chested',
    /**
     * @remarks
     * When added, this component signifies that dyes can be used
     * on this entity to change its color.
     *
     */
    IsDyeable = 'minecraft:is_dyeable',
    /**
     * @remarks
     * When added, this component signifies that this entity can
     * hide from hostile mobs while invisible.
     *
     */
    IsHiddenWhenInvisible = 'minecraft:is_hidden_when_invisible',
    /**
     * @remarks
     * When added, this component signifies that this entity this
     * currently on fire.
     *
     */
    IsIgnited = 'minecraft:is_ignited',
    /**
     * @remarks
     * When added, this component signifies that this entity is an
     * illager captain.
     *
     */
    IsIllagerCaptain = 'minecraft:is_illager_captain',
    /**
     * @remarks
     * When added, this component signifies that this entity is
     * currently saddled.
     *
     */
    IsSaddled = 'minecraft:is_saddled',
    /**
     * @remarks
     * When added, this component signifies that this entity is
     * currently shaking.
     *
     */
    IsShaking = 'minecraft:is_shaking',
    /**
     * @remarks
     * When added, this component signifies that this entity is
     * currently sheared.
     *
     */
    IsSheared = 'minecraft:is_sheared',
    /**
     * @remarks
     * When added, this component signifies that this entity can be
     * stacked.
     *
     */
    IsStackable = 'minecraft:is_stackable',
    /**
     * @remarks
     * When added, this component signifies that this entity is
     * currently stunned.
     *
     */
    IsStunned = 'minecraft:is_stunned',
    /**
     * @remarks
     * When added, this component signifies that this entity is
     * currently tamed.
     *
     */
    IsTamed = 'minecraft:is_tamed',
    /**
     * @remarks
     * If added onto the entity, this indicates that the entity
     * represents a free-floating item in the world. Lets you
     * retrieve the actual item stack contents via the itemStack
     * property.
     *
     */
    Item = 'minecraft:item',
    /**
     * @remarks
     * Defines the base movement speed in lava of this entity.
     *
     */
    LavaMovement = 'minecraft:lava_movement',
    /**
     * @remarks
     * Allows this entity to be leashed and defines the conditions
     * and events for this entity when is leashed.
     *
     */
    Leashable = 'minecraft:leashable',
    /**
     * @remarks
     * When added, this component signifies that this entity
     * contains an additional variant value. Can be used to further
     * differentiate variants.
     *
     */
    MarkVariant = 'minecraft:mark_variant',
    /**
     * @remarks
     * Defines the general movement speed of this entity.
     *
     */
    Movement = 'minecraft:movement',
    /**
     * @remarks
     * When added, this movement control allows the mob to swim in
     * water and walk on land.
     *
     */
    MovementAmphibious = 'minecraft:movement.amphibious',
    /**
     * @remarks
     * When added, this component allows the movement of an entity.
     *
     */
    MovementBasic = 'minecraft:movement.basic',
    /**
     * @remarks
     * When added, this move control causes the mob to fly.
     *
     */
    MovementFly = 'minecraft:movement.fly',
    /**
     * @remarks
     * When added, this move control allows a mob to fly, swim,
     * climb, etc.
     *
     */
    MovementGeneric = 'minecraft:movement.generic',
    /**
     * @remarks
     * When added, this movement control allows the mob to glide.
     *
     */
    MovementGlide = 'minecraft:movement.glide',
    /**
     * @remarks
     * When added, this move control causes the mob to hover.
     *
     */
    MovementHover = 'minecraft:movement.hover',
    /**
     * @remarks
     * Move control that causes the mob to jump as it moves with a
     * specified delay between jumps.
     *
     */
    MovementJump = 'minecraft:movement.jump',
    /**
     * @remarks
     * When added, this move control causes the mob to hop as it
     * moves.
     *
     */
    MovementSkip = 'minecraft:movement.skip',
    /**
     * @remarks
     * When added, this move control causes the mob to sway side to
     * side giving the impression it is swimming.
     *
     */
    MovementSway = 'minecraft:movement.sway',
    /**
     * @remarks
     * Allows this entity to generate paths that include vertical
     * walls (for example, like Minecraft spiders do.)
     *
     */
    NavigationClimb = 'minecraft:navigation.climb',
    /**
     * @remarks
     * Allows this entity to generate paths by flying around the
     * air like the regular Ghast.
     *
     */
    NavigationFloat = 'minecraft:navigation.float',
    /**
     * @remarks
     * Allows this entity to generate paths in the air (for
     * example, like Minecraft parrots do.)
     *
     */
    NavigationFly = 'minecraft:navigation.fly',
    /**
     * @remarks
     * Allows this entity to generate paths by walking, swimming,
     * flying and/or climbing around and jumping up and down a
     * block.
     *
     */
    NavigationGeneric = 'minecraft:navigation.generic',
    /**
     * @remarks
     * Allows this entity to generate paths in the air (for
     * example, like the Minecraft Bees do.) Keeps them from
     * falling out of the skies and doing predictive movement.
     *
     */
    NavigationHover = 'minecraft:navigation.hover',
    /**
     * @remarks
     * Allows this entity to generate paths by walking around and
     * jumping up and down a block like regular mobs.
     *
     */
    NavigationWalk = 'minecraft:navigation.walk',
    /**
     * @beta
     * @remarks
     * Adds NPC capabilities to an entity such as custom skin,
     * name, and dialogue interactions.
     *
     */
    Npc = 'minecraft:npc',
    /**
     * @remarks
     * When present on an entity, this entity is on fire.
     *
     */
    OnFire = 'minecraft:onfire',
    /**
     * @remarks
     * Use this component to read the exhaustion of a player. This
     * is only available on players.
     *
     */
    Exhaustion = 'minecraft:player.exhaustion',
    /**
     * @remarks
     * Use this component to read the hunger of a player. This is
     * only available on players.
     *
     */
    Hunger = 'minecraft:player.hunger',
    /**
     * @remarks
     * Use this component to read the saturation of a player. This
     * is only available on players.
     *
     */
    Saturation = 'minecraft:player.saturation',
    /**
     * @remarks
     * The projectile component controls the properties of a
     * projectile entity and allows it to be shot in a given
     * direction. This component is present when the entity has the
     * minecraft:projectile component.
     *
     */
    Projectile = 'minecraft:projectile',
    /**
     * @remarks
     * Sets the distance through which the entity can push through.
     *
     */
    PushThrough = 'minecraft:push_through',
    /**
     * @remarks
     * When added, this component adds the capability that an
     * entity can be ridden by another entity.
     *
     */
    Rideable = 'minecraft:rideable',
    /**
     * @remarks
     * This component is added to any entity when it is riding
     * another entity.
     *
     */
    Riding = 'minecraft:riding',
    /**
     * @remarks
     * Sets the entity's visual size.
     *
     */
    Scale = 'minecraft:scale',
    /**
     * @remarks
     * Skin Id value. Can be used to differentiate skins, such as
     * base skins for villagers.
     *
     */
    SkinId = 'minecraft:skin_id',
    /**
     * @remarks
     * Defines the entity's strength to carry items.
     *
     */
    Strength = 'minecraft:strength',
    /**
     * @remarks
     * Defines the rules for an entity to be tamed by the player.
     *
     */
    Tameable = 'minecraft:tameable',
    /**
     * @remarks
     * Contains options for taming a rideable entity based on the
     * entity that mounts it.
     *
     */
    TameMount = 'minecraft:tamemount',
    /**
     * @remarks
     * Used to determine the type families the entity belongs to.
     *
     */
    TypeFamily = 'minecraft:type_family',
    /**
     * @remarks
     * Defines the general movement speed underwater of this
     * entity.
     *
     */
    UnderwaterMovement = 'minecraft:underwater_movement',
    /**
     * @remarks
     * Used to differentiate the component group of a variant of an
     * entity from others. (e.g. ocelot, villager).
     *
     */
    Variant = 'minecraft:variant',
    /**
     * @remarks
     * When added, this component signifies that this entity wants
     * to become a jockey.
     *
     */
    WantsJockey = 'minecraft:wants_jockey',
}

/**
 * Describes the source of damage from an Entity.
 */
export enum EntityDamageCause {
    /**
     * @remarks
     * Damage caused by a falling anvil.
     *
     */
    anvil = 'anvil',
    /**
     * @remarks
     * Damage caused from a non-Entity explosion. For example, an
     * exploding bed.
     *
     */
    blockExplosion = 'blockExplosion',
    /**
     * @remarks
     * Damage caused by Campfires.
     *
     */
    campfire = 'campfire',
    /**
     * @remarks
     * Unused.
     *
     */
    charging = 'charging',
    /**
     * @remarks
     * Damage caused by physically touching an Entity or Block. For
     * example, touching a Sweet Berry bush or Pufferfish.
     *
     */
    contact = 'contact',
    /**
     * @remarks
     * Damage caused by an Entity being out of air and inside a
     * liquid block.
     *
     */
    drowning = 'drowning',
    /**
     * @remarks
     * Damage caused by an Entity attack.
     *
     */
    entityAttack = 'entityAttack',
    /**
     * @remarks
     * Damage caused by an Entity explosion. For example, a Creeper
     * or Wither.
     *
     */
    entityExplosion = 'entityExplosion',
    /**
     * @remarks
     * Damage caused by falling onto the ground.
     *
     */
    fall = 'fall',
    /**
     * @remarks
     * Damage caused by falling blocks. Note: Anvils and
     * Stalactites have their own damage causes.
     *
     */
    fallingBlock = 'fallingBlock',
    /**
     * @remarks
     * Damage caused by catching on fire.
     *
     */
    fire = 'fire',
    /**
     * @remarks
     * Damage caused by burning over time.
     *
     */
    fireTick = 'fireTick',
    /**
     * @remarks
     * Damage caused by fireworks.
     *
     */
    fireworks = 'fireworks',
    /**
     * @remarks
     * Damage caused by flying into a wall at high speed while
     * gliding with Elytra.
     *
     */
    flyIntoWall = 'flyIntoWall',
    /**
     * @remarks
     * Damage caused by staying inside a Powder Snow block.
     *
     */
    freezing = 'freezing',
    /**
     * @remarks
     * Damage caused by touching a Lava block.
     *
     */
    lava = 'lava',
    /**
     * @remarks
     * Damage caused by being struck by lightning.
     *
     */
    lightning = 'lightning',
    maceSmash = 'maceSmash',
    /**
     * @remarks
     * Damage caused by magical attacks. For example, Evoker Fang
     * or Conduit Block.
     *
     */
    magic = 'magic',
    /**
     * @remarks
     * Damage caused by touching a Magma block.
     *
     */
    magma = 'magma',
    /**
     * @remarks
     * Damage caused by no source. For example, from a command or
     * script.
     *
     */
    none = 'none',
    /**
     * @remarks
     * Damage caused by an indirect source. For example, setting a
     * mob's health to 0 in a behavior pack.
     *
     */
    override = 'override',
    /**
     * @remarks
     * Damage caused by a Piston.
     *
     */
    piston = 'piston',
    /**
     * @remarks
     * Damage caused by a projectile.
     *
     */
    projectile = 'projectile',
    /**
     * @remarks
     * Damage caused by Goat ramming.
     *
     */
    ramAttack = 'ramAttack',
    /**
     * @remarks
     * Damage caused by the /kill command.
     *
     */
    selfDestruct = 'selfDestruct',
    /**
     * @remarks
     * Damage caused by the Warden's Sonic Boom attack.
     *
     */
    sonicBoom = 'sonicBoom',
    /**
     * @remarks
     * Damage caused by a Soul Campfire.
     *
     */
    soulCampfire = 'soulCampfire',
    /**
     * @remarks
     * Damage caused by a falling Stalactite block.
     *
     */
    stalactite = 'stalactite',
    /**
     * @remarks
     * Damage caused by touching a Stalagmite block.
     *
     */
    stalagmite = 'stalagmite',
    /**
     * @remarks
     * Damage caused over time by having an empty hunger bar.
     *
     */
    starve = 'starve',
    /**
     * @remarks
     * Damage caused by an Entity being out of air and inside a
     * non-liquid block.
     *
     */
    suffocation = 'suffocation',
    /**
     * @remarks
     * Damage caused by an Entity being in an inhabitable climate.
     * For example, a Snow Golem in a biome with a temperature
     * greater than 1.
     *
     */
    temperature = 'temperature',
    /**
     * @remarks
     * Damage caused by the Thorns armor enchantment and by the
     * Guardian thorns effect.
     *
     */
    thorns = 'thorns',
    /**
     * @remarks
     * Damage caused over time by falling into the void.
     *
     */
    'void' = 'void',
    /**
     * @remarks
     * Damage caused by the Wither effect. For example, from
     * touching a Wither Rose.
     *
     */
    wither = 'wither',
}

/**
 * Describes the source of healing of an Entity.
 */
export enum EntityHealCause {
    /**
     * @remarks
     * Healing caused by items such as potions.
     *
     */
    Heal = 'Heal',
    /**
     * @remarks
     * Healing caused by regeneration effects.
     *
     */
    Regeneration = 'Regeneration',
    /**
     * @remarks
     * Healing caused when hunger is full.
     *
     */
    SelfHeal = 'SelfHeal',
    /**
     * @remarks
     * Healing caused when Totem of Undying is activated.
     *
     */
    TotemOfUndying = 'TotemOfUndying',
}

/**
 * An enumeration describing initialization cause of an entity.
 */
export enum EntityInitializationCause {
    /**
     * @remarks
     * Case when an entity is created as child of other entity or
     * entities, e.g., cows making a cow or slimes making smaller
     * slimes after dying.
     *
     */
    Born = 'Born',
    /**
     * @remarks
     * Case when an entity is created by an event, e.g., a
     * Wandering trader spawning llamas.
     *
     */
    Event = 'Event',
    /**
     * @remarks
     * Case when an entity is loaded into the world.
     *
     */
    Loaded = 'Loaded',
    /**
     * @remarks
     * Case when an entity is naturally spawned in the world.
     *
     */
    Spawned = 'Spawned',
    /**
     * @remarks
     * Case when an entity is transformed into another entity.
     *
     */
    Transformed = 'Transformed',
}

/**
 * Enumerator describing the source of an Entity swing. Sent as
 * part of {@link PlayerSwingStartAfterEvent}
 */
export enum EntitySwingSource {
    /**
     * @remarks
     * Sent when the Entity swings as part of an attack.
     *
     */
    Attack = 'Attack',
    /**
     * @remarks
     * Sent when the Entity swings as part of a build action.
     *
     */
    Build = 'Build',
    /**
     * @remarks
     * Sent when the Entity swings as part of dropping an item.
     *
     */
    DropItem = 'DropItem',
    /**
     * @remarks
     * Sent when the Entity swings as part of an event response.
     *
     */
    Event = 'Event',
    /**
     * @remarks
     * Sent when the Entity swings as part of an interaction.
     *
     */
    Interact = 'Interact',
    /**
     * @remarks
     * Sent when the Entity swings as part of a mine action.
     *
     */
    Mine = 'Mine',
    /**
     * @remarks
     * Sent when the Entity swing has no determinable source.
     *
     */
    None = 'None',
    /**
     * @remarks
     * Sent when the Entity swings as part of throwing an item.
     *
     */
    ThrowItem = 'ThrowItem',
    /**
     * @remarks
     * Sent when the Entity swings as part of using an item.
     *
     */
    UseItem = 'UseItem',
}

/**
 * The equipment slot of the mob. This includes armor, offhand
 * and mainhand slots.
 */
export enum EquipmentSlot {
    /**
     * @beta
     * @remarks
     * The body slot. This slot is used to hold armor for
     * non-humanoid mobs.
     *
     */
    Body = 'Body',
    /**
     * @remarks
     * The chest slot. This slot is used to hold items such as
     * Chestplate or Elytra.
     *
     */
    Chest = 'Chest',
    /**
     * @remarks
     * The feet slot. This slot is used to hold items such as
     * Boots.
     *
     */
    Feet = 'Feet',
    /**
     * @remarks
     * The head slot. This slot is used to hold items such as
     * Helmets or Carved Pumpkins.
     *
     */
    Head = 'Head',
    /**
     * @remarks
     * The legs slot. This slot is used to hold items such as
     * Leggings.
     *
     */
    Legs = 'Legs',
    /**
     * @remarks
     * The mainhand slot. For players, the mainhand slot refers to
     * the currently active hotbar slot.
     *
     */
    Mainhand = 'Mainhand',
    /**
     * @remarks
     * The offhand slot. This slot is used to hold items such as
     * shields and maps.
     *
     */
    Offhand = 'Offhand',
}

/**
 * Represents the type of fluid for use within a fluid
 * containing block, like a cauldron.
 */
export enum FluidType {
    /**
     * @remarks
     * Represents lava as a type of fluid.
     *
     */
    Lava = 'Lava',
    /**
     * @remarks
     * Represents a potion as a type of fluid.
     *
     */
    Potion = 'Potion',
    /**
     * @remarks
     * Represents powder snow as a type of fluid.
     *
     */
    PowderSnow = 'PowderSnow',
    /**
     * @remarks
     * Represents water as a type of fluida.
     *
     */
    Water = 'Water',
}

/**
 * Represents a game mode for the current world experience.
 */
export enum GameMode {
    /**
     * @remarks
     * World is in a more locked-down experience, where blocks may
     * not be manipulated.
     *
     */
    Adventure = 'Adventure',
    /**
     * @remarks
     * World is in a full creative mode. In creative mode, the
     * player has all the resources available in the item selection
     * tabs and the survival selection tab. They can also destroy
     * blocks instantly including those which would normally be
     * indestructible. Command and structure blocks can also be
     * used in creative mode. Items also do not lose durability or
     * disappear.
     *
     */
    Creative = 'Creative',
    /**
     * @remarks
     * World is in spectator mode. In spectator mode, spectators
     * are always flying and cannot become grounded. Spectators can
     * pass through solid blocks and entities without any
     * collisions, and cannot use items or interact with blocks or
     * mobs. Spectators cannot be seen by mobs or other players,
     * except for other spectators; spectators appear as a
     * transparent floating head.
     *
     */
    Spectator = 'Spectator',
    /**
     * @remarks
     * World is in a survival mode, where players can take damage
     * and entities may not be peaceful. Survival mode is where the
     * player must collect resources, build structures while
     * surviving in their generated world. Activities can, over
     * time, chip away at player health and hunger bar.
     *
     */
    Survival = 'Survival',
}

/**
 * Game rules. These values can also be controlled via the
 * /gamerule command.
 */
export enum GameRule {
    /**
     * @remarks
     * Whether command blocks should notify admins when they
     * perform commands.
     *
     */
    CommandBlockOutput = 'commandBlockOutput',
    /**
     * @remarks
     * Controls whether command blocks can execute commands.
     *
     */
    CommandBlocksEnabled = 'commandBlocksEnabled',
    /**
     * @remarks
     * Controls whether the day and night cycles progress.
     *
     */
    DoDayLightCycle = 'doDayLightCycle',
    /**
     * @remarks
     * Controls whether non-mob entities do drops. ie. Item Frame
     *
     */
    DoEntityDrops = 'doEntityDrops',
    /**
     * @remarks
     * Controls whether fire spreads.
     *
     */
    DoFireTick = 'doFireTick',
    /**
     * @remarks
     * Controls whether players immediately respawn or are shown
     * the death screen.
     *
     */
    DoImmediateRespawn = 'doImmediateRespawn',
    /**
     * @remarks
     * Controls whether players deal with the effects of not
     * sleeping (such as Phantom spawning).
     *
     */
    DoInsomnia = 'doInsomnia',
    /**
     * @remarks
     * Determines whether players should be able to craft only
     * those recipes that they've unlocked first - when
     * dolimitedcrafting is set to true.
     *
     */
    DoLimitedCrafting = 'doLimitedCrafting',
    /**
     * @remarks
     * Controls whether mobs drop loot.
     *
     */
    DoMobLoot = 'doMobLoot',
    /**
     * @remarks
     * Controls whether mobs spawn naturally in the world.
     *
     */
    DoMobSpawning = 'doMobSpawning',
    /**
     * @remarks
     * Controls whether blocks drop items when destroyed.
     *
     */
    DoTileDrops = 'doTileDrops',
    /**
     * @remarks
     * Controls whether the weather can change naturally.
     *
     */
    DoWeatherCycle = 'doWeatherCycle',
    /**
     * @remarks
     * Controls whether entities take damage from drowning.
     *
     */
    DrowningDamage = 'drowningDamage',
    /**
     * @remarks
     * Controls whether entities take damage from falling.
     *
     */
    FallDamage = 'fallDamage',
    /**
     * @remarks
     * Controls whether entities take damage from fire.
     *
     */
    FireDamage = 'fireDamage',
    /**
     * @remarks
     * Controls whether there is damage from freezing.
     *
     */
    FreezeDamage = 'freezeDamage',
    /**
     * @remarks
     * The maximum number of commands that can be executed
     * simultaneously by the /function command.
     *
     */
    FunctionCommandLimit = 'functionCommandLimit',
    /**
     * @remarks
     * Controls whether players keep their inventories when they
     * die.
     *
     */
    KeepInventory = 'keepInventory',
    /**
     * @remarks
     * The maximum number of chained commands that can execute per
     * tick.
     *
     */
    MaxCommandChainLength = 'maxCommandChainLength',
    /**
     * @remarks
     * Controls whether mob griefing can happen in the world.
     * Example: A Creeper explosion destroying blocks.
     *
     */
    MobGriefing = 'mobGriefing',
    /**
     * @remarks
     * Controls whether players can regenerate health.
     *
     */
    NaturalRegeneration = 'naturalRegeneration',
    /**
     * @remarks
     * The percentage of players required to be sleeping in order
     * to advance to the next day.
     *
     */
    PlayersSleepingPercentage = 'playersSleepingPercentage',
    /**
     * @beta
     * @remarks
     * Controls which player waypoints are automatically added to
     * the players locator bar.
     *
     */
    PlayerWaypoints = 'playerWaypoints',
    /**
     * @remarks
     * Controls whether projectiles (entities with a projectile
     * component, like Arrows, thrown Tridents or Fireworks) can
     * destroy certain blocks that support this interaction (such
     * as Chorus Fruit, Dripstone or Decorated Pots). Restrictions
     * on which projectiles can destroy certain blocks apply.
     *
     */
    ProjectilesCanBreakBlocks = 'projectilesCanBreakBlocks',
    /**
     * @remarks
     * Controls whether players can damage each other.
     *
     */
    Pvp = 'pvp',
    /**
     * @remarks
     * Controls how frequently random ticks occur. A value of 0 or
     * less will disable random ticks. The default value is 1.
     *
     */
    RandomTickSpeed = 'randomTickSpeed',
    /**
     * @remarks
     * Controls whether built-in (vanilla) recipes automatically
     * unlock as the player progresses through the game (one
     * alternative to this is to use the /recipe command based on
     * custom gameplay logic.)
     *
     */
    RecipesUnlock = 'recipesUnlock',
    /**
     * @remarks
     * Controls whether respawn blocks (e.g. Bed, Respawn Anchor)
     * explode in other dimensions.
     *
     */
    RespawnBlocksExplode = 'respawnBlocksExplode',
    /**
     * @remarks
     * Controls whether command output is displayed to players.
     * Also controls whether Command Block output is stored by
     * default.
     *
     */
    SendCommandFeedback = 'sendCommandFeedback',
    /**
     * @remarks
     * Controls whether Border Block effects are shown.
     *
     */
    ShowBorderEffect = 'showBorderEffect',
    /**
     * @remarks
     * Controls whether player coordinates are displayed.
     *
     */
    ShowCoordinates = 'showCoordinates',
    /**
     * @remarks
     * Controls whether the days a player has played is displayed.
     *
     */
    ShowDaysPlayed = 'showDaysPlayed',
    /**
     * @remarks
     * Controls whether death messages are displayed in chat.
     *
     */
    ShowDeathMessages = 'showDeathMessages',
    /**
     * @remarks
     * Controls whether standard player notifications for recipes
     * will show. When set to false, 'player unlocked recipes' are
     * no longer sent as player notifications.
     *
     */
    ShowRecipeMessages = 'showRecipeMessages',
    /**
     * @remarks
     * Controls whether item tags are shown. E.g. 'Can Place On',
     * 'Can Destroy', item lock icons, etc.
     *
     */
    ShowTags = 'showTags',
    /**
     * @remarks
     * The block radius from world spawn that a player is allowed
     * to spawn in. Does not affect Adventure mode. The default
     * value is 10 blocks.
     *
     */
    SpawnRadius = 'spawnRadius',
    /**
     * @remarks
     * Affects whether TNT blocks can be lit.
     *
     */
    TntExplodes = 'tntExplodes',
    /**
     * @remarks
     * Controls whether blocks randomly drop loot or all blocks
     * drop loot when destroyed by an explosion. Defaults to false.
     *
     */
    TntExplosionDropDecay = 'tntExplosionDropDecay',
}

/**
 * Describes the graphics mode of a client. Used by {@link
 * Player.graphicsMode}
 */
export enum GraphicsMode {
    /**
     * @remarks
     * A graphics mode that refers to the Deferred Technical
     * Preview graphics mode setting.
     *
     */
    Deferred = 'Deferred',
    /**
     * @remarks
     * A graphics mode that refers to the Fancy graphics mode
     * setting. Most special graphics effects are turned on in this
     * setting.
     *
     */
    Fancy = 'Fancy',
    /**
     * @remarks
     * A graphics mode that refers to the Ray Traced graphics mode
     * setting. This setting enables ray tracing.
     *
     */
    RayTraced = 'RayTraced',
    /**
     * @remarks
     * A graphics mode that refers to the Simple graphics mode
     * setting. Most graphics effects are turned off in this
     * setting.
     *
     */
    Simple = 'Simple',
}

/**
 * Specifies options related to the item currently being held
 * by an entity.
 */
export enum HeldItemOption {
    /**
     * @remarks
     * Any item is being held.
     *
     */
    AnyItem = 'AnyItem',
    /**
     * @remarks
     * No item is being held.
     *
     */
    NoItem = 'NoItem',
}

export enum HudElement {
    PaperDoll = 0,
    Armor = 1,
    ToolTips = 2,
    TouchControls = 3,
    Crosshair = 4,
    Hotbar = 5,
    Health = 6,
    ProgressBar = 7,
    Hunger = 8,
    AirBubbles = 9,
    HorseHealth = 10,
    StatusEffects = 11,
    ItemText = 12,
}

/**
 * Enumeration that specifies how to treat the visibility of a
 * HUD element.
 */
export enum HudVisibility {
    /**
     * @remarks
     * Specifies that this HUD element should be hidden.
     *
     */
    Hide = 0,
    /**
     * @remarks
     * Specifies that this HUD element should be reset to its
     * default state (while most HUD elements are visible, some HUD
     * elements can be hidden by the player via settings.)
     *
     */
    Reset = 1,
}

/**
 * All the different input buttons that are supported. Use with
 * {@link InputInfo.getButtonState} via {@link
 * Player.inputInfo} or {@link PlayerButtonInputAfterEvent} via
 * {@link WorldAfterEvents.playerButtonInput}
 */
export enum InputButton {
    /**
     * @remarks
     * This is mapped to the 'Jump' button on controllers,
     * keyboards, and touch interfaces.
     *
     */
    Jump = 'Jump',
    /**
     * @remarks
     * This is mapped to the 'Sneak' button on controllers,
     * keyboards, and touch interfaces. By default, this is shift
     * on a keyboard or B on an Xbox controller. On touch
     * interfaces this will only be pressed for 1 tick or less and
     * then it will be released immediately even if the player
     * holds their finger down. Dismounting a horse or exiting a
     * boat will not send a Sneak button change event.
     *
     */
    Sneak = 'Sneak',
}

/**
 * Describes the type of input of a device.
 */
export enum InputMode {
    /**
     * @remarks
     * Gamepad input.
     *
     */
    Gamepad = 'Gamepad',
    /**
     * @remarks
     * Keyboard and mouse input.
     *
     */
    KeyboardAndMouse = 'KeyboardAndMouse',
    /**
     * @remarks
     * Motion controller input.
     *
     */
    MotionController = 'MotionController',
    /**
     * @remarks
     * Touch input.
     *
     */
    Touch = 'Touch',
}

/**
 * Input permission categories. Used by {@link
 * PlayerInputPermissionCategoryChangeAfterEvent} to specify
 * which category was changed and {@link
 * PlayerInputPermissions} to get or set permissions.
 */
export enum InputPermissionCategory {
    /**
     * @remarks
     * Player input relating to camera movement.
     *
     */
    Camera = 1,
    /**
     * @remarks
     * Player input relating to all player movement. Disabling this
     * is equivalent to disabling jump, sneak, lateral movement,
     * mount, and dismount.
     *
     */
    Movement = 2,
    /**
     * @remarks
     * Player input for moving laterally in the world. This would
     * be WASD on a keyboard or the movement joystick on gamepad or
     * touch.
     *
     */
    LateralMovement = 4,
    /**
     * @remarks
     * Player input relating to sneak. This also affects flying
     * down.
     *
     */
    Sneak = 5,
    /**
     * @remarks
     * Player input relating to jumping. This also affects flying
     * up.
     *
     */
    Jump = 6,
    /**
     * @remarks
     * Player input relating to mounting vehicles.
     *
     */
    Mount = 7,
    /**
     * @remarks
     * Player input relating to dismounting. When disabled, the
     * player can still dismount vehicles by other means, for
     * example on horses players can still jump off and in boats
     * players can go into another boat.
     *
     */
    Dismount = 8,
    /**
     * @remarks
     * Player input relating to moving the player forward.
     *
     */
    MoveForward = 9,
    /**
     * @remarks
     * Player input relating to moving the player backward.
     *
     */
    MoveBackward = 10,
    /**
     * @remarks
     * Player input relating to moving the player left.
     *
     */
    MoveLeft = 11,
    /**
     * @remarks
     * Player input relating to moving the player right.
     *
     */
    MoveRight = 12,
}

/**
 * The types of item components that are accessible via
 * function ItemStack.getComponent.
 */
export enum ItemComponentTypes {
    /**
     * @beta
     */
    BlockDynamicProperties = 'minecraft:block_actor_dynamic_properties',
    /**
     * @remarks
     * The minecraft:book component.
     *
     */
    Book = 'minecraft:book',
    Compostable = 'minecraft:compostable',
    /**
     * @remarks
     * The minecraft:cooldown component.
     *
     */
    Cooldown = 'minecraft:cooldown',
    /**
     * @remarks
     * The minecraft:durability component.
     *
     */
    Durability = 'minecraft:durability',
    Dyeable = 'minecraft:dyeable',
    /**
     * @remarks
     * The minecraft:enchantable component.
     *
     */
    Enchantable = 'minecraft:enchantable',
    /**
     * @remarks
     * The minecraft:food component.
     *
     */
    Food = 'minecraft:food',
    Inventory = 'minecraft:inventory',
    Potion = 'minecraft:potion',
}

/**
 * Describes how an an item can be moved within a container.
 */
export enum ItemLockMode {
    /**
     * @remarks
     * The item cannot be dropped or crafted with.
     *
     */
    inventory = 'inventory',
    /**
     * @remarks
     * The item has no container restrictions.
     *
     */
    none = 'none',
    /**
     * @remarks
     * The item cannot be moved from its slot, dropped or crafted
     * with.
     *
     */
    slot = 'slot',
}

/**
 * Specifies how to handle waterloggable blocks overlapping
 * with existing liquid.
 */
export enum LiquidSettings {
    /**
     * @remarks
     * Causes a waterloggable block to become waterlogged, if it
     * overlaps with existing liquid.
     *
     */
    ApplyWaterlogging = 'ApplyWaterlogging',
    /**
     * @remarks
     * Do not waterlog any waterloggable blocks that overlap
     * existing liquid.
     *
     */
    IgnoreWaterlogging = 'IgnoreWaterlogging',
}

/**
 * Represents the type of liquid that can be placed on a block
 * or flow dynamically in the world.
 */
export enum LiquidType {
    /**
     * @remarks
     * Represents water as a type of liquid.
     *
     */
    Water = 'Water',
}

/**
 * Enum representing the different reasons why a locator bar
 * operation may fail.
 */
export enum LocatorBarErrorReason {
    /**
     * @remarks
     * The waypoint already exists in the locator bar and cannot be
     * added again.
     *
     */
    WaypointAlreadyExists = 'WaypointAlreadyExists',
    /**
     * @remarks
     * The maximum number of waypoints has been reached and no more
     * can be added.
     *
     */
    WaypointLimitExceeded = 'WaypointLimitExceeded',
    /**
     * @remarks
     * The specified waypoint does not exist in the locator bar.
     *
     */
    WaypointNotFound = 'WaypointNotFound',
}

/**
 * Describes the memory of a device.
 */
export enum MemoryTier {
    /**
     * @remarks
     * Max memory for Super Low Tier is 1.5GBs.
     *
     */
    SuperLow = 0,
    /**
     * @remarks
     *  Max memory for Low Tier is 2GBs.
     *
     */
    Low = 1,
    /**
     * @remarks
     * Max memory for Mid Tier is 4GBs.
     *
     */
    Mid = 2,
    /**
     * @remarks
     * Max memory for High Tier is 8GBs.
     *
     */
    High = 3,
    /**
     * @remarks
     * Memory for Super High Tier is above 8GBs.
     *
     */
    SuperHigh = 4,
}

/**
 * Enum containing the different phases of the moon based on
 * the current day.,Obtain the current MoonPhase using
 * world.getMoonPhase.
 *
 * The fullness of the moon controls various mob behaviors such
 * as the number of slimes that spawn in Swamp biomes, the
 * chance skeletons and zombies have to spawn with armor, as
 * well as the chance for spiders to spawn with certain status
 * effects.
 */
export enum MoonPhase {
    /**
     * @remarks
     * The brightest moon phase. During this phase, cats have a 50%
     * chance of spawning as black cats.
     *
     */
    FullMoon = 0,
    /**
     * @remarks
     * The phase following the Full Moon.
     *
     */
    WaningGibbous = 1,
    /**
     * @remarks
     * The phase following the Waxing Crescent.
     *
     */
    FirstQuarter = 2,
    /**
     * @remarks
     * The phase following the Last Quarter.
     *
     */
    WaningCrescent = 3,
    /**
     * @remarks
     * The darkest moon phase.
     *
     */
    NewMoon = 4,
    /**
     * @remarks
     * The phase following the New Moon.
     *
     */
    WaxingCrescent = 5,
    /**
     * @remarks
     * The phase following the Waning Gibbous.
     *
     */
    LastQuarter = 6,
    /**
     * @remarks
     * The phase following the First Quarter.
     *
     */
    WaxingGibbous = 7,
}

export enum MovementType {
    Immovable = 'Immovable',
    Popped = 'Popped',
    Push = 'Push',
    PushPull = 'PushPull',
}

/**
 * An enumeration describing the reason for the namespace name
 * error being thrown
 */
export enum NamespaceNameErrorReason {
    /**
     * @remarks
     * A restricted namespace was used as the namespace
     *
     */
    DisallowedNamespace = 'DisallowedNamespace',
    /**
     * @remarks
     * The name was missing a namespace when one is required
     *
     */
    NoNamespace = 'NoNamespace',
}

/**
 * 表示分数持有者列表的排序顺序。
 * 
 * Used for specifying a sort order for how to display an
 * objective and its list of participants.
 */
export enum ObjectiveSortOrder {
    /**
     * @remarks
     * 以升序显示分数持有者列表（例如：A-Z）。
     * 
     * Objective participant list is displayed in ascending (e.g.,
     * A-Z) order.
     *
     */
    Ascending = 0,
    /**
     * @remarks
     * 以降序显示分数持有者列表（例如：Z-A）。
     * 
     * Objective participant list is displayed in descending (e.g.,
     * Z-A) order.
     *
     */
    Descending = 1,
}

/**
 * Enum containing colors to be used with EntityColorComponent
 * and EntityColor2Component
 */
export enum PaletteColor {
    /**
     * @remarks
     * Hex color #f0f0f0
     *
     */
    White = 0,
    /**
     * @remarks
     * Hex color #F9801D
     *
     */
    Orange = 1,
    /**
     * @remarks
     * Hex color #C74EBD
     *
     */
    Magenta = 2,
    /**
     * @remarks
     * Hex color #3AB3DA
     *
     */
    LightBlue = 3,
    /**
     * @remarks
     * Hex color #FED83D
     *
     */
    Yellow = 4,
    /**
     * @remarks
     * Hex color #80C71F
     *
     */
    Lime = 5,
    /**
     * @remarks
     * Hex color #F38BAA
     *
     */
    Pink = 6,
    /**
     * @remarks
     * Hex color #474F52
     *
     */
    Gray = 7,
    /**
     * @remarks
     * Hex color #9D9D97
     *
     */
    Silver = 8,
    /**
     * @remarks
     * Hex color #169C9C
     *
     */
    Cyan = 9,
    /**
     * @remarks
     * Hex color #8932B8
     *
     */
    Purple = 10,
    /**
     * @remarks
     * Hex color #3C44AA
     *
     */
    Blue = 11,
    /**
     * @remarks
     * Hex color #835432
     *
     */
    Brown = 12,
    /**
     * @remarks
     * Hex color #5E7C16
     *
     */
    Green = 13,
    /**
     * @remarks
     * Hex color #B02E26
     *
     */
    Red = 14,
    /**
     * @remarks
     * Hex color #1D1D21
     *
     */
    Black = 15,
}

/**
 * Describes what kind of platform is a device.
 */
export enum PlatformType {
    /**
     * @remarks
     * Specialized gaming device.
     *
     */
    Console = 'Console',
    /**
     * @remarks
     * Personal Computer (PC).
     *
     */
    Desktop = 'Desktop',
    /**
     * @remarks
     *  Handheld device such smartphone or tablet.
     *
     */
    Mobile = 'Mobile',
}

/**
 * Specifies the player inventory type.
 */
export enum PlayerInventoryType {
    /**
     * @remarks
     * Hotbar inventory.
     *
     */
    Hotbar = 'Hotbar',
    /**
     * @remarks
     * Main inventory.
     *
     */
    Inventory = 'Inventory',
}

/**
 * The player permission level.
 */
export enum PlayerPermissionLevel {
    /**
     * @remarks
     * Visitors can only observe the world, not interact with it.
     *
     */
    Visitor = 0,
    /**
     * @remarks
     * Members can build and mine, attack players and mobs, and
     * interact with items and entities.
     *
     */
    Member = 1,
    /**
     * @remarks
     * Operators can teleport and use commands, in addition to
     * everything Members can do.
     *
     */
    Operator = 2,
    Custom = 3,
}

/**
 * @rc
 * The split screen slot of a player.
 */
export enum PlayerSplitScreenSlot {
    /**
     * @remarks
     * The first player in the split screen session. This is the
     * primary player.
     *
     */
    First = 'First',
    /**
     * @remarks
     * The fourth player in the split screen session.
     *
     */
    Fourth = 'Fourth',
    /**
     * @remarks
     * The second player in the split screen session.
     *
     */
    Second = 'Second',
    /**
     * @remarks
     * The third player in the split screen session.
     *
     */
    Third = 'Third',
}

/**
 * @beta
 */
export enum PlayerWaypointsMode {
    Everyone = 'Everyone',
    Off = 'Off',
}

/**
 * 表示分数持有者的类型。
 * 
 * Contains objectives and participants for the scoreboard.
 */
export enum ScoreboardIdentityType {
    /**
     * @remarks
     * 分数持有者是一个实体。
     * 
     * This scoreboard participant is tied to an entity.
     *
     */
    Entity = 'Entity',
    /**
     * @remarks
     * 分数持有者是一个虚拟玩家（通常用于存储数据或者作为抽象进度）。
     * 
     * This scoreboard participant is tied to a pseudo player
     * entity - typically this is used to store scores as data or
     * as abstract progress.
     *
     */
    FakePlayer = 'FakePlayer',
    /**
     * @remarks
     * 分数持有者是一位玩家。
     * 
     * This scoreboard participant is tied to a player.
     *
     */
    Player = 'Player',
}

/**
 * Describes where the script event originated from.
 */
export enum ScriptEventSource {
    /**
     * @remarks
     * The script event originated from a Block such as a Command
     * Block.
     *
     */
    Block = 'Block',
    /**
     * @remarks
     * The script event originated from an Entity such as a Player,
     * Command Block Minecart or Animation Controller.
     *
     */
    Entity = 'Entity',
    /**
     * @remarks
     * The script event originated from an NPC dialogue.
     *
     */
    NPCDialogue = 'NPCDialogue',
    /**
     * @remarks
     * The script event originated from the server, such as from a
     * runCommand API call or a dedicated server console.
     *
     */
    Server = 'Server',
}

/**
 * Represents a side of a sign.
 */
export enum SignSide {
    /**
     * @remarks
     * The back of the sign.
     *
     */
    Back = 'Back',
    /**
     * @remarks
     * The front of the sign.
     *
     */
    Front = 'Front',
}

export enum StickyType {
    None = 'None',
    Same = 'Same',
}

/**
 * Specifies how structure blocks should be animated when a
 * structure is placed.
 */
export enum StructureAnimationMode {
    /**
     * @remarks
     * Blocks will be randomly placed one at at time. Use
     * @minecraft/server.StructurePlaceOptions.animationSeconds to
     * control how long it takes for all blocks to be placed.
     *
     */
    Blocks = 'Blocks',
    /**
     * @remarks
     * Blocks will be placed one layer at a time from bottom to
     * top. Use
     * @minecraft/server.StructurePlaceOptions.animationSeconds to
     * control how long it takes for all blocks to be placed.
     *
     */
    Layers = 'Layers',
    /**
     * @remarks
     * All blocks will be placed immediately.
     *
     */
    None = 'None',
}

/**
 * Specifies how a structure should be mirrored when placed.
 */
export enum StructureMirrorAxis {
    /**
     * @remarks
     * No mirroring.
     *
     */
    None = 'None',
    /**
     * @remarks
     * Structure is mirrored across the X axis.
     *
     */
    X = 'X',
    /**
     * @remarks
     * Structure is mirrored across both the X and Z axes.
     *
     */
    XZ = 'XZ',
    /**
     * @remarks
     * Structure is mirrored across the Z axis.
     *
     */
    Z = 'Z',
}

/**
 * Enum describing a structure's placement rotation.
 */
export enum StructureRotation {
    /**
     * @remarks
     * No rotation.
     *
     */
    None = 'None',
    /**
     * @remarks
     * 180 degree rotation.
     *
     */
    Rotate180 = 'Rotate180',
    /**
     * @remarks
     * 270 degree rotation.
     *
     */
    Rotate270 = 'Rotate270',
    /**
     * @remarks
     * 90 degree rotation.
     *
     */
    Rotate90 = 'Rotate90',
}

/**
 * Specifies how a structure should be saved.
 */
export enum StructureSaveMode {
    /**
     * @remarks
     * The structure will be temporarily saved to memory. The
     * structure will persist until the world is shut down.
     *
     */
    Memory = 'Memory',
    /**
     * @remarks
     * The structure will be saved to the world file and persist
     * between world loads. A saved structure can be removed from
     * the world via @minecraft/server.StructureManager.delete.
     *
     */
    World = 'World',
}

/**
 * The reason that the {@link
 * @minecraft/server.TickingAreaError} was thrown.
 */
export enum TickingAreaErrorReason {
    /**
     * @remarks
     * Added a ticking area with an identifier that already exists.
     *
     */
    IdentifierAlreadyExists = 'IdentifierAlreadyExists',
    /**
     * @remarks
     *  Adding this ticking area pushed the ticking areas over the
     * limit specified by {@link TickingAreaManager.maxChunkCount}.
     *
     */
    OverChunkLimit = 'OverChunkLimit',
    /**
     * @remarks
     * Exceeded the 255 chunk limit for the length or width of the
     * ticking area.
     *
     */
    SideLengthExceeded = 'SideLengthExceeded',
    /**
     * @remarks
     * Tried to remove ticking area with identifier not registered
     * in the {@link TickingAreaManager}.
     *
     */
    UnknownIdentifier = 'UnknownIdentifier',
}

/**
 * Provides numeric values for common periods in the Minecraft
 * day.
 */
export enum TimeOfDay {
    /**
     * @remarks
     * Sets the time to the start of the day, which is time of the
     * day 1,000 (or the equivalent of 7am) in Minecraft.
     *
     */
    Day = 1000,
    /**
     * @remarks
     * Sets the time to noon, which is time of the day 6,000 in
     * Minecraft.
     *
     */
    Noon = 6000,
    /**
     * @remarks
     * Sets the time to sunset, which is time of the day 12,000 (or
     * the equivalent of 6pm) in Minecraft.
     *
     */
    Sunset = 12000,
    /**
     * @remarks
     * Sets the time to night, which is time of the day 13,000 (or
     * the equivalent of 7:00pm) in Minecraft.
     *
     */
    Night = 13000,
    /**
     * @remarks
     * Sets the time to midnight, which is time of the day 18,000
     * (or the equivalent of 12:00am) in Minecraft.
     *
     */
    Midnight = 18000,
    /**
     * @remarks
     * Sets the time to sunrise, which is time of the day 23,000
     * (or the equivalent of 5am) in Minecraft.
     *
     */
    Sunrise = 23000,
}

/**
 * Tint logic applied to a block or part of a block. The color
 * may vary when a world position is part of the context, as
 * biomes often have an influence on the resulting tint.
 */
export enum TintMethod {
    /**
     * @remarks
     * Specifies a birch foliage tint method.
     *
     */
    BirchFoliage = 'BirchFoliage',
    /**
     * @remarks
     * Specifies a default foliage tint method.
     *
     */
    DefaultFoliage = 'DefaultFoliage',
    /**
     * @remarks
     * Specifies a dry foliage tint method.
     *
     */
    DryFoliage = 'DryFoliage',
    /**
     * @remarks
     * Specifies an evergreen foliage tint method.
     *
     */
    EvergreenFoliage = 'EvergreenFoliage',
    /**
     * @remarks
     * Specifies a grass tint method.
     *
     */
    Grass = 'Grass',
    /**
     * @remarks
     * Specifies no tint method, resulting in a white tint.
     *
     */
    None = 'None',
    /**
     * @remarks
     * Specifies a water tint method.
     *
     */
    Water = 'Water',
}

/**
 * @beta
 * An enumeration with the reason that a watchdog is deciding
 * to terminate execution of a behavior packs' script.
 */
export enum WatchdogTerminateReason {
    /**
     * @remarks
     * Script runtime for a behavior pack is terminated due to
     * non-responsiveness from script (a hang or infinite loop).
     *
     */
    Hang = 'Hang',
    /**
     * @remarks
     * Script runtime for a behavior pack is terminated due to a
     * stack overflow (a long, and potentially infinite) chain of
     * function calls.
     *
     */
    StackOverflow = 'StackOverflow',
}

/**
 * Enum representing different texture icons that can be
 * displayed for waypoints on the locator bar.
 */
export enum WaypointTexture {
    /**
     * @remarks
     * Circle waypoint icon texture.
     *
     */
    Circle = 'minecraft:circle',
    /**
     * @remarks
     * Small square waypoint icon texture.
     *
     */
    SmallSquare = 'minecraft:small_square',
    /**
     * @remarks
     * Small star waypoint icon texture.
     *
     */
    SmallStar = 'minecraft:small_star',
    /**
     * @remarks
     * Square waypoint icon texture.
     *
     */
    Square = 'minecraft:square',
}

/**
 * Used to specify the type of weather condition within the
 * world.
 */
export enum WeatherType {
    /**
     * @remarks
     * Specifies a clear weather condition.
     *
     */
    Clear = 'Clear',
    /**
     * @remarks
     * Specifies a rain weather condition.
     *
     */
    Rain = 'Rain',
    /**
     * @remarks
     * Specifies a rain and thunder weather condition.
     *
     */
    Thunder = 'Thunder',
}

export type BlockComponentReturnType<T extends string> = T extends keyof BlockComponentTypeMap
    ? BlockComponentTypeMap[T]
    : BlockCustomComponentInstance;

export type BlockComponentTypeMap = {
    dynamic_properties: BlockDynamicPropertiesComponent;
    fluid_container: BlockFluidContainerComponent;
    instrument_sound: BlockInstrumentComponent;
    inventory: BlockInventoryComponent;
    map_color: BlockMapColorComponent;
    'minecraft:dynamic_properties': BlockDynamicPropertiesComponent;
    'minecraft:fluid_container': BlockFluidContainerComponent;
    'minecraft:instrument_sound': BlockInstrumentComponent;
    'minecraft:inventory': BlockInventoryComponent;
    'minecraft:map_color': BlockMapColorComponent;
    'minecraft:movable': BlockMovableComponent;
    'minecraft:piston': BlockPistonComponent;
    'minecraft:precipitation_interactions': BlockPrecipitationInteractionsComponent;
    'minecraft:record_player': BlockRecordPlayerComponent;
    'minecraft:redstone_producer': BlockRedstoneProducerComponent;
    'minecraft:sign': BlockSignComponent;
    movable: BlockMovableComponent;
    piston: BlockPistonComponent;
    precipitation_interactions: BlockPrecipitationInteractionsComponent;
    record_player: BlockRecordPlayerComponent;
    redstone_producer: BlockRedstoneProducerComponent;
    sign: BlockSignComponent;
};

/**
 * Type alias used by the {@link BlockPermutation} matches and
 * resolve functions to narrow block state argument types to
 * those mapped by {@link
 * @minecraft/vanilla-data.BlockStateMapping}.
 */
export type BlockStateArg<T> = T extends `${MinecraftBlockTypes}`
    ? T extends keyof BlockStateMapping
        ? BlockStateMapping[T]
        : never
    : Record<string, boolean | number | string>;

export type EntityComponentReturnType<T extends string> = T extends keyof EntityComponentTypeMap
    ? EntityComponentTypeMap[T]
    : EntityComponent;

export type EntityComponentTypeMap = {
    addrider: EntityAddRiderComponent;
    ageable: EntityAgeableComponent;
    breathable: EntityBreathableComponent;
    can_climb: EntityCanClimbComponent;
    can_fly: EntityCanFlyComponent;
    can_power_jump: EntityCanPowerJumpComponent;
    color: EntityColorComponent;
    color2: EntityColor2Component;
    cursor_inventory: PlayerCursorInventoryComponent;
    ender_inventory: EntityEnderInventoryComponent;
    equippable: EntityEquippableComponent;
    fire_immune: EntityFireImmuneComponent;
    floats_in_liquid: EntityFloatsInLiquidComponent;
    flying_speed: EntityFlyingSpeedComponent;
    friction_modifier: EntityFrictionModifierComponent;
    healable: EntityHealableComponent;
    health: EntityHealthComponent;
    inventory: EntityInventoryComponent;
    is_baby: EntityIsBabyComponent;
    is_charged: EntityIsChargedComponent;
    is_chested: EntityIsChestedComponent;
    is_dyeable: EntityIsDyeableComponent;
    is_hidden_when_invisible: EntityIsHiddenWhenInvisibleComponent;
    is_ignited: EntityIsIgnitedComponent;
    is_illager_captain: EntityIsIllagerCaptainComponent;
    is_saddled: EntityIsSaddledComponent;
    is_shaking: EntityIsShakingComponent;
    is_sheared: EntityIsShearedComponent;
    is_stackable: EntityIsStackableComponent;
    is_stunned: EntityIsStunnedComponent;
    is_tamed: EntityIsTamedComponent;
    item: EntityItemComponent;
    lava_movement: EntityLavaMovementComponent;
    leashable: EntityLeashableComponent;
    mark_variant: EntityMarkVariantComponent;
    'minecraft:addrider': EntityAddRiderComponent;
    'minecraft:ageable': EntityAgeableComponent;
    'minecraft:breathable': EntityBreathableComponent;
    'minecraft:can_climb': EntityCanClimbComponent;
    'minecraft:can_fly': EntityCanFlyComponent;
    'minecraft:can_power_jump': EntityCanPowerJumpComponent;
    'minecraft:color': EntityColorComponent;
    'minecraft:color2': EntityColor2Component;
    'minecraft:cursor_inventory': PlayerCursorInventoryComponent;
    'minecraft:ender_inventory': EntityEnderInventoryComponent;
    'minecraft:equippable': EntityEquippableComponent;
    'minecraft:fire_immune': EntityFireImmuneComponent;
    'minecraft:floats_in_liquid': EntityFloatsInLiquidComponent;
    'minecraft:flying_speed': EntityFlyingSpeedComponent;
    'minecraft:friction_modifier': EntityFrictionModifierComponent;
    'minecraft:healable': EntityHealableComponent;
    'minecraft:health': EntityHealthComponent;
    'minecraft:inventory': EntityInventoryComponent;
    'minecraft:is_baby': EntityIsBabyComponent;
    'minecraft:is_charged': EntityIsChargedComponent;
    'minecraft:is_chested': EntityIsChestedComponent;
    'minecraft:is_dyeable': EntityIsDyeableComponent;
    'minecraft:is_hidden_when_invisible': EntityIsHiddenWhenInvisibleComponent;
    'minecraft:is_ignited': EntityIsIgnitedComponent;
    'minecraft:is_illager_captain': EntityIsIllagerCaptainComponent;
    'minecraft:is_saddled': EntityIsSaddledComponent;
    'minecraft:is_shaking': EntityIsShakingComponent;
    'minecraft:is_sheared': EntityIsShearedComponent;
    'minecraft:is_stackable': EntityIsStackableComponent;
    'minecraft:is_stunned': EntityIsStunnedComponent;
    'minecraft:is_tamed': EntityIsTamedComponent;
    'minecraft:item': EntityItemComponent;
    'minecraft:lava_movement': EntityLavaMovementComponent;
    'minecraft:leashable': EntityLeashableComponent;
    'minecraft:mark_variant': EntityMarkVariantComponent;
    'minecraft:movement': EntityMovementComponent;
    'minecraft:movement.amphibious': EntityMovementAmphibiousComponent;
    'minecraft:movement.basic': EntityMovementBasicComponent;
    'minecraft:movement.fly': EntityMovementFlyComponent;
    'minecraft:movement.generic': EntityMovementGenericComponent;
    'minecraft:movement.glide': EntityMovementGlideComponent;
    'minecraft:movement.hover': EntityMovementHoverComponent;
    'minecraft:movement.jump': EntityMovementJumpComponent;
    'minecraft:movement.skip': EntityMovementSkipComponent;
    'minecraft:movement.sway': EntityMovementSwayComponent;
    'minecraft:navigation.climb': EntityNavigationClimbComponent;
    'minecraft:navigation.float': EntityNavigationFloatComponent;
    'minecraft:navigation.fly': EntityNavigationFlyComponent;
    'minecraft:navigation.generic': EntityNavigationGenericComponent;
    'minecraft:navigation.hover': EntityNavigationHoverComponent;
    'minecraft:navigation.walk': EntityNavigationWalkComponent;
    'minecraft:npc': EntityNpcComponent;
    'minecraft:onfire': EntityOnFireComponent;
    'minecraft:player.exhaustion': EntityExhaustionComponent;
    'minecraft:player.hunger': EntityHungerComponent;
    'minecraft:player.saturation': EntitySaturationComponent;
    'minecraft:projectile': EntityProjectileComponent;
    'minecraft:push_through': EntityPushThroughComponent;
    'minecraft:rideable': EntityRideableComponent;
    'minecraft:riding': EntityRidingComponent;
    'minecraft:scale': EntityScaleComponent;
    'minecraft:skin_id': EntitySkinIdComponent;
    'minecraft:strength': EntityStrengthComponent;
    'minecraft:tameable': EntityTameableComponent;
    'minecraft:tamemount': EntityTameMountComponent;
    'minecraft:type_family': EntityTypeFamilyComponent;
    'minecraft:underwater_movement': EntityUnderwaterMovementComponent;
    'minecraft:variant': EntityVariantComponent;
    'minecraft:wants_jockey': EntityWantsJockeyComponent;
    movement: EntityMovementComponent;
    'movement.amphibious': EntityMovementAmphibiousComponent;
    'movement.basic': EntityMovementBasicComponent;
    'movement.fly': EntityMovementFlyComponent;
    'movement.generic': EntityMovementGenericComponent;
    'movement.glide': EntityMovementGlideComponent;
    'movement.hover': EntityMovementHoverComponent;
    'movement.jump': EntityMovementJumpComponent;
    'movement.skip': EntityMovementSkipComponent;
    'movement.sway': EntityMovementSwayComponent;
    'navigation.climb': EntityNavigationClimbComponent;
    'navigation.float': EntityNavigationFloatComponent;
    'navigation.fly': EntityNavigationFlyComponent;
    'navigation.generic': EntityNavigationGenericComponent;
    'navigation.hover': EntityNavigationHoverComponent;
    'navigation.walk': EntityNavigationWalkComponent;
    npc: EntityNpcComponent;
    onfire: EntityOnFireComponent;
    'player.exhaustion': EntityExhaustionComponent;
    'player.hunger': EntityHungerComponent;
    'player.saturation': EntitySaturationComponent;
    projectile: EntityProjectileComponent;
    push_through: EntityPushThroughComponent;
    rideable: EntityRideableComponent;
    riding: EntityRidingComponent;
    scale: EntityScaleComponent;
    skin_id: EntitySkinIdComponent;
    strength: EntityStrengthComponent;
    tameable: EntityTameableComponent;
    tamemount: EntityTameMountComponent;
    type_family: EntityTypeFamilyComponent;
    underwater_movement: EntityUnderwaterMovementComponent;
    variant: EntityVariantComponent;
    wants_jockey: EntityWantsJockeyComponent;
};

/**
 * @beta
 */
export type EntityIdentifierType<T> = [T] extends [never]
    ? VanillaEntityIdentifier
    : T extends string
      ? VanillaEntityIdentifier | T
      : never;

export type ItemComponentReturnType<T extends string> = T extends keyof ItemComponentTypeMap
    ? ItemComponentTypeMap[T]
    : ItemCustomComponentInstance;

export type ItemComponentTypeMap = {
    block_actor_dynamic_properties: ItemBlockDynamicPropertiesComponent;
    book: ItemBookComponent;
    compostable: ItemCompostableComponent;
    cooldown: ItemCooldownComponent;
    durability: ItemDurabilityComponent;
    dyeable: ItemDyeableComponent;
    enchantable: ItemEnchantableComponent;
    food: ItemFoodComponent;
    inventory: ItemInventoryComponent;
    'minecraft:block_actor_dynamic_properties': ItemBlockDynamicPropertiesComponent;
    'minecraft:book': ItemBookComponent;
    'minecraft:compostable': ItemCompostableComponent;
    'minecraft:cooldown': ItemCooldownComponent;
    'minecraft:durability': ItemDurabilityComponent;
    'minecraft:dyeable': ItemDyeableComponent;
    'minecraft:enchantable': ItemEnchantableComponent;
    'minecraft:food': ItemFoodComponent;
    'minecraft:inventory': ItemInventoryComponent;
    'minecraft:potion': ItemPotionComponent;
    potion: ItemPotionComponent;
};

/**
 * @beta
 */
export type VanillaEntityIdentifier =
    | EntityType
    | MinecraftEntityTypes
    | `${MinecraftEntityTypes}`
    | `${MinecraftEntityTypes}<${string}>`;

/**
 * Handle to an aim-assist category that exists in the
 * world.aimAssist registry.
 */
export class AimAssistCategory {
    private constructor();
    /**
     * @remarks
     * Default targeting priority used for block types not found in
     * getBlockPriorities.
     *
     * @throws This property can throw when used.
     */
    readonly defaultBlockPriority: number;
    /**
     * @remarks
     * Default targeting priority used for entity types not found
     * in getEntityPriorities.
     *
     * @throws This property can throw when used.
     */
    readonly defaultEntityPriority: number;
    /**
     * @remarks
     * The unique Id associated with the category.
     *
     */
    readonly identifier: string;
    /**
     * @remarks
     * Gets the priority settings used for block targeting.
     *
     * @returns
     * The record mapping block Ids to their priority settings.
     * Larger numbers have greater priority.
     * @throws This function can throw errors.
     */
    getBlockPriorities(): Record<string, number>;
    /**
     * @remarks
     * Gets the priority settings used for block targeting.
     *
     * @returns
     * The record mapping block tags to their priority settings.
     * Larger numbers have greater priority.
     * @throws This function can throw errors.
     *
     * {@link EngineError}
     */
    getBlockTagPriorities(): Record<string, number>;
    /**
     * @remarks
     * Gets the priority settings used for entity targeting.
     *
     * @returns
     * The record mapping entity Ids to their priority settings.
     * Larger numbers have greater priority.
     * @throws This function can throw errors.
     */
    getEntityPriorities(): Record<string, number>;
    /**
     * @remarks
     * Gets the priority settings used for entity targeting.
     *
     * @returns
     * Map entity type families to their priority settings in a
     * Record. Larger numbers have greater priority.
     * @throws This function can throw errors.
     *
     * {@link EngineError}
     */
    getEntityTypeFamilyPriorities(): Record<string, number>;
}

/**
 * Settings used with AimAssistRegistry.addCategory for
 * creation of the AimAssistCategory.
 */
export class AimAssistCategorySettings {
    /**
     * @remarks
     * Optional. Default targeting priority used for block types
     * not provided to setBlockPriorities.
     *
     * @worldMutation
     *
     */
    defaultBlockPriority: number;
    /**
     * @remarks
     * Optional. Default targeting priority used for entity types
     * not provided to setEntityPriorities.
     *
     * @worldMutation
     *
     */
    defaultEntityPriority: number;
    /**
     * @remarks
     * The unique Id used to register the category with. Must have
     * a namespace.
     *
     */
    readonly identifier: string;
    /**
     * @remarks
     * Constructor that takes a unique Id to associate with the
     * created AimAssistCategory. Must have a namespace.
     *
     */
    constructor(identifier: string);
    /**
     * @remarks
     * Gets the priority settings used for block targeting.
     *
     * @returns
     * The record mapping block Ids to their priority settings.
     * Larger numbers have greater priority.
     */
    getBlockPriorities(): Record<string, number>;
    /**
     * @remarks
     * Gets the priority settings used for block targeting.
     *
     * @returns
     * The record mapping block tags to their priority settings.
     * Larger numbers have greater priority.
     */
    getBlockTagPriorities(): Record<string, number>;
    /**
     * @remarks
     * Gets the priority settings used for entity targeting.
     *
     * @returns
     * The record mapping entity Ids to their priority settings.
     * Larger numbers have greater priority.
     */
    getEntityPriorities(): Record<string, number>;
    /**
     * @remarks
     * Gets the priority settings used for entity targeting.
     *
     * @returns
     * Map entity type families to their priority settings in a
     * Record. Larger numbers have greater priority.
     */
    getEntityTypeFamilyPriorities(): Record<string, number>;
    /**
     * @remarks
     * Sets the priority settings used for block targeting.
     *
     * @worldMutation
     *
     * @param blockPriorities
     * A record mapping block Ids to their priority settings.
     * Larger numbers have greater priority.
     */
    setBlockPriorities(
        blockPriorities: Record<keyof typeof MinecraftBlockTypes | string, number>,
    ): void;
    /**
     * @remarks
     * Sets the priority settings used for block targeting.
     *
     * @worldMutation
     *
     */
    setBlockTagPriorities(blockTagPriorities: Record<string, number>): void;
    /**
     * @remarks
     * Sets the priority settings used for entity targeting.
     *
     * @worldMutation
     *
     * @param entityPriorities
     * A record mapping entity Ids to their priority settings.
     * Larger numbers have greater priority.
     */
    setEntityPriorities(
        entityPriorities: Record<keyof typeof MinecraftEntityTypes | string, number>,
    ): void;
    /**
     * @remarks
     * Sets the priority settings used for entity targeting.
     *
     * @worldMutation
     *
     */
    setEntityTypeFamilyPriorities(entityTypeFamilyPriorities: Record<string, number>): void;
}

/**
 * Handle to an aim-assist preset that exists in the
 * world.aimAssist registry.
 */
export class AimAssistPreset {
    private constructor();
    /**
     * @remarks
     * Optional. Default aim-assist category Id used for items not
     * provided to setItemSettings.
     *
     * @throws This property can throw when used.
     */
    readonly defaultItemSettings?: string;
    /**
     * @remarks
     * Optional. Aim-assist category Id used for an empty hand.
     *
     * @throws This property can throw when used.
     */
    readonly handSettings?: string;
    /**
     * @remarks
     * The unique Id associated with the preset.
     *
     */
    readonly identifier: string;
    /**
     * @remarks
     * Gets the list of block tags to exclude from aim assist
     * targeting.
     *
     * @returns
     * The array of block tags.
     * @throws This function can throw errors.
     *
     * {@link EngineError}
     */
    getExcludedBlockTagTargets(): string[];
    /**
     * @remarks
     * Gets the list of block Ids to exclude from aim assist
     * targeting.
     *
     * @returns
     * The array of block Ids.
     * @throws This function can throw errors.
     */
    getExcludedBlockTargets(): string[];
    /**
     * @remarks
     * Gets the list of entity Ids to exclude from aim assist
     * targeting.
     *
     * @returns
     * The array of entity Ids.
     * @throws This function can throw errors.
     */
    getExcludedEntityTargets(): string[];
    /**
     * @remarks
     * Gets the list of entity type families to exclude from aim
     * assist targeting.
     *
     * @returns
     * The array of entity type families.
     * @throws This function can throw errors.
     *
     * {@link EngineError}
     */
    getExcludedEntityTypeFamilyTargets(): string[];
    /**
     * @remarks
     * Gets the per-item aim-assist category Ids.
     *
     * @returns
     * The record mapping item Ids to aim-assist category Ids.
     * @throws This function can throw errors.
     */
    getItemSettings(): Record<string, string>;
    /**
     * @remarks
     * Gets the list of item Ids that will target liquid blocks
     * with aim-assist when being held.
     *
     * @returns
     * The array of item Ids.
     * @throws This function can throw errors.
     */
    getLiquidTargetingItems(): string[];
}

/**
 * Settings used with AimAssistRegistry.addPreset for creation
 * of the AimAssistPreset.
 */
export class AimAssistPresetSettings {
    /**
     * @remarks
     * Optional. Default aim-assist category Id used for items not
     * provided to setItemSettings.
     *
     * @worldMutation
     *
     */
    defaultItemSettings?: string;
    /**
     * @remarks
     * Optional. Aim-assist category Id used for an empty hand.
     *
     * @worldMutation
     *
     */
    handSettings?: string;
    /**
     * @remarks
     * The unique Id used to register the preset with. Must have a
     * namespace.
     *
     */
    readonly identifier: string;
    /**
     * @remarks
     * Constructor that takes a unique Id to associate with the
     * created AimAssistPreset. Must have a namespace.
     *
     */
    constructor(identifier: string);
    /**
     * @remarks
     * Gets the list of block tags to exclude from aim assist
     * targeting.
     *
     * @returns
     * The array of block tags.
     */
    getExcludedBlockTagTargets(): string[] | undefined;
    /**
     * @remarks
     * Gets the list of block Ids to exclude from aim assist
     * targeting.
     *
     * @returns
     * The array of block Ids.
     */
    getExcludedBlockTargets(): string[] | undefined;
    /**
     * @remarks
     * Gets the list of entity Ids to exclude from aim assist
     * targeting.
     *
     * @returns
     * The array of entity Ids.
     */
    getExcludedEntityTargets(): string[] | undefined;
    /**
     * @remarks
     * Gets the list of entity type families to exclude from aim
     * assist targeting.
     *
     * @returns
     * The array of entity type families.
     */
    getExcludedEntityTypeFamilyTargets(): string[] | undefined;
    /**
     * @remarks
     * Gets the per-item aim-assist category Ids.
     *
     * @returns
     * The record mapping item Ids to aim-assist category Ids.
     */
    getItemSettings(): Record<string, string>;
    /**
     * @remarks
     * Gets the list of item Ids that will target liquid blocks
     * with aim-assist when being held.
     *
     * @returns
     * The array of item Ids.
     */
    getLiquidTargetingItems(): string[] | undefined;
    /**
     * @remarks
     * Sets the list of block tags to exclude from aim assist
     * targeting.
     *
     * @worldMutation
     *
     * @param targets
     * An array of block tags.
     */
    setExcludedBlockTagTargets(targets?: string[]): void;
    /**
     * @remarks
     * Sets the list of block Ids to exclude from aim assist
     * targeting.
     *
     * @worldMutation
     *
     * @param targets
     * An array of block Ids.
     */
    setExcludedBlockTargets(targets?: (keyof typeof MinecraftBlockTypes | string)[]): void;
    /**
     * @remarks
     * Sets the list of entity Ids to exclude from aim assist
     * targeting.
     *
     * @worldMutation
     *
     * @param targets
     * An array of entity Ids.
     */
    setExcludedEntityTargets(targets?: (keyof typeof MinecraftEntityTypes | string)[]): void;
    /**
     * @remarks
     * Sets the list of entity type families to exclude from aim
     * assist targeting.
     *
     * @worldMutation
     *
     * @param targets
     * An array of entity type families.
     */
    setExcludedEntityTypeFamilyTargets(targets?: string[]): void;
    /**
     * @remarks
     * Sets the per-item aim-assist category Ids.
     *
     * @worldMutation
     *
     * @param itemSettings
     * A record mapping item Ids to aim-assist category Ids.
     * Category Ids must have a namespace.
     */
    setItemSettings(itemSettings: Record<keyof typeof MinecraftItemTypes | string, string>): void;
    /**
     * @remarks
     * Sets the list of item Ids that will target liquid blocks
     * with aim-assist when being held.
     *
     * @worldMutation
     *
     * @param items
     * An array of item Ids.
     */
    setLiquidTargetingItems(items?: (keyof typeof MinecraftItemTypes | string)[]): void;
}

/**
 * A container for APIs related to the world's aim-assist
 * settings.
 */
export class AimAssistRegistry {
    private constructor();
    /**
     * @remarks
     * The default aim-assist category Id that is used when not
     * otherwise specified.
     *
     */
    static readonly DefaultCategoryId = 'minecraft:default';
    /**
     * @remarks
     * The default aim-assist preset Id that is used when not
     * otherwise specified.
     *
     */
    static readonly DefaultPresetId = 'minecraft:aim_assist_default';
    /**
     * @remarks
     * Adds an aim-assist category to the registry.
     *
     * @worldMutation
     *
     * @param category
     * The category settings used to create the new category.
     * @returns
     * The created category handle.
     * @throws This function can throw errors.
     *
     * {@link EngineError}
     *
     * {@link Error}
     *
     * {@link InvalidArgumentError}
     *
     * {@link NamespaceNameError}
     */
    addCategory(category: AimAssistCategorySettings): AimAssistCategory;
    /**
     * @remarks
     * Adds an aim-assist preset to the registry.
     *
     * @worldMutation
     *
     * @param preset
     * The preset settings used to create the new preset.
     * @returns
     * The created preset handle.
     * @throws This function can throw errors.
     *
     * {@link EngineError}
     *
     * {@link Error}
     *
     * {@link InvalidArgumentError}
     *
     * {@link NamespaceNameError}
     */
    addPreset(preset: AimAssistPresetSettings): AimAssistPreset;
    /**
     * @remarks
     * Gets all available categories in the registry.
     *
     * @returns
     * An array of all available category objects.
     */
    getCategories(): AimAssistCategory[];
    /**
     * @remarks
     * Gets the category associated with the provided Id.
     *
     * @worldMutation
     *
     * @returns
     * The category object if it exists, otherwise returns
     * undefined.
     */
    getCategory(categoryId: string): AimAssistCategory | undefined;
    /**
     * @remarks
     * Gets the preset associated with the provided Id.
     *
     * @worldMutation
     *
     * @param presetId
     * The Id of the preset to retrieve. Must have a namespace.
     * @returns
     * The preset object if it exists, otherwise returns undefined.
     */
    getPreset(presetId: string): AimAssistPreset | undefined;
    /**
     * @remarks
     * Gets all available presets in the registry.
     *
     * @returns
     * An array of all available preset objects.
     */
    getPresets(): AimAssistPreset[];
}

/**
 * @beta
 * Describes a single banner pattern, which includes a colour
 * and a pattern type.
 */
export class BannerPattern {
    private constructor();
    /**
     * @remarks
     * The color to apply to this banner pattern.
     *
     */
    readonly color: string;
    /**
     * @remarks
     * The pattern type (e.g. gradient, chevron, cross, etc.) to
     * apply to the banner.
     *
     */
    readonly pattern: string;
}

/**
 * Describes a type of biome.
 */
export class BiomeType {
    private constructor();
    /**
     * @remarks
     * Identifier of the biome type.
     *
     */
    readonly id: string;
    /**
     * @remarks
     * Returns a list of the biome's tags.
     *
     */
    getTags(): string[];
    /**
     * @remarks
     * Checks if the biome has all of the provided tags.
     *
     * @param tags
     * The list of tags to check against the biome.
     */
    hasTags(tags: string[]): boolean;
}

/**
 * Supports a catalog of available biome types registered
 * within Minecraft.
 */
export class BiomeTypes {
    private constructor();
    /**
     * @remarks
     * Returns a specific biome type.
     *
     * @param typeName
     * Identifier of the biome.  Generally, namespaced identifiers
     * (e.g., minecraft:frozen_peaks) should be used.
     * @returns
     * If the biome exists, a BiomeType object is returned. If not,
     * undefined is returned.
     */
    static get(typeName: string): BiomeType | undefined;
    /**
     * @remarks
     * Returns all registered biome types within Minecraft
     *
     */
    static getAll(): BiomeType[];
}

/**
 * 表示世界维度中的特定位置的方块。
 * 方块对象对应了唯一的 X、Y、Z 与维度，可用于读取或修改此位置的方块状态。
 * 此类型在 1.17.10.21 有重大更新。
 * 
 * Represents a block in a dimension. A block represents a
 * unique X, Y, and Z within a dimension and get/sets the state
 * of the block at that location. This type was significantly
 * updated in version 1.17.10.21.
 */
export class Block {
    private constructor();
    /**
     * @remarks
     * 返回方块所在维度对象。
     *
     * Returns the dimension that the block is within.
     *
     * @returns
     * 方块所在维度对象。
     *
     */
    readonly dimension: Dimension;
    /**
     * @remarks
     * 返回 true 如果这个方块是空气方块（例如，空的空间）
     *
     * Returns true if this block is an air block (i.e., empty
     * space).
     *
     * @throws This property can throw when used.
     *
     * {@link LocationInUnloadedChunkError}
     *
     * {@link LocationOutOfWorldBoundariesError}
     */
    readonly isAir: boolean;
    /**
     * @remarks
     * 如果这个方块是液体方块，例如水方块和熔岩方块等，则返回 true。
     * 空气方块和石头方块等则不属于液体方块。
     * 含水方块不算作液体方块。
     *
     * Returns true if this block is a liquid block - (e.g., a
     * water block and a lava block are liquid, while an air block
     * and a stone block are not. Water logged blocks are not
     * liquid blocks).
     *
     * @throws This property can throw when used.
     *
     * {@link LocationInUnloadedChunkError}
     *
     * {@link LocationOutOfWorldBoundariesError}
     */
    readonly isLiquid: boolean;
    /**
     * @beta
     * @remarks
     * 如果该块是实心且不可通行的，则返回 true
     * -（例如，圆石和钻石块是实心方块，而梯子和栅栏则不是）。
     *
     * Returns true if this block is solid and impassible - (e.g.,
     * a cobblestone block and a diamond block are solid, while a
     * ladder block and a fence block are not).
     *
     * @throws This property can throw when used.
     *
     * {@link LocationInUnloadedChunkError}
     *
     * {@link LocationOutOfWorldBoundariesError}
     */
    readonly isSolid: boolean;
    /**
     * @remarks
     * Returns true if this reference to a block is still valid
     * (for example, if the block is unloaded, references to that
     * block will no longer be valid.)
     *
     */
    readonly isValid: boolean;
    /**
     * @remarks
     * 返回或设置该方块是否含水。
     *
     * Returns or sets whether this block has water on it.
     *
     * @throws This property can throw when used.
     *
     * {@link LocationInUnloadedChunkError}
     *
     * {@link LocationOutOfWorldBoundariesError}
     */
    readonly isWaterlogged: boolean;
    /**
     * @remarks
     * Key for the localization of this block's name used in .lang
     * files.
     *
     * @throws This property can throw when used.
     *
     * {@link LocationInUnloadedChunkError}
     *
     * {@link LocationOutOfWorldBoundariesError}
     */
    readonly localizationKey: string;
    /**
     * @remarks
     * 该方块的坐标。
     *
     * Coordinates of the specified block.
     *
     * @throws This property can throw when used.
     */
    readonly location: Vector3;
    /**
     * @remarks
     * 描述该方块的附加配置数据。
     * （常称为方块状态）
     *
     * Additional block configuration data that describes the
     * block.
     *
     * @throws This property can throw when used.
     *
     * {@link LocationInUnloadedChunkError}
     *
     * {@link LocationOutOfWorldBoundariesError}
     */
    readonly permutation: BlockPermutation;
    /**
     * @remarks
     * 获取方块的类型。
     *
     * Gets the type of block.
     *
     * @throws This property can throw when used.
     *
     * {@link LocationInUnloadedChunkError}
     *
     * {@link LocationOutOfWorldBoundariesError}
     */
    readonly 'type': BlockType;
    /**
     * @remarks
     * 该方块的类型标识符。
     *
     * Identifier of the type of block for this block. Warning:
     * Vanilla block names can be changed in future releases, try
     * using 'Block.matches' instead for block comparison.
     *
     * @throws This property can throw when used.
     *
     * {@link LocationInUnloadedChunkError}
     *
     * {@link LocationOutOfWorldBoundariesError}
     */
    readonly typeId: string;
    /**
     * @remarks
     * 方块的 X 坐标。
     *
     * X coordinate of the block.
     *
     */
    readonly x: number;
    /**
     * @remarks
     * 方块的 Y 坐标。
     *
     * Y coordinate of the block.
     *
     */
    readonly y: number;
    /**
     * @remarks
     * 方块的 Z 坐标。
     *
     * Z coordinate of the block.
     *
     */
    readonly z: number;
    /**
     * @remarks
     * 返回该方块上方的 {@link Block}（Y 方向正方向）。
     *
     * Returns the {@link Block} above this block (positive in the
     * Y direction).
     *
     * @param steps
     * 返回之前要执行的步骤数。
     * 留空默认为一。
     * （返回的方块在原方块上方的距离）。
     *
     * Number of steps above to step before returning.
     * Defaults to: 1
     * @throws This function can throw errors.
     *
     * {@link LocationInUnloadedChunkError}
     *
     * {@link LocationOutOfWorldBoundariesError}
     */
    above(steps?: number): Block | undefined;
    /**
     * @remarks
     * 返回该方块下方的{@link Block}（Y 方向为负）。
     *
     * Returns the {@link Block} below this block (negative in the
     * Y direction).
     *
     * @param steps
     * 向下的步数。
     * 留空默认为一。
     *
     * Number of steps below to step before returning.
     * Defaults to: 1
     * @throws This function can throw errors.
     *
     * {@link LocationInUnloadedChunkError}
     *
     * {@link LocationOutOfWorldBoundariesError}
     */
    below(steps?: number): Block | undefined;
    /**
     * @remarks
     * 返回该方块在 X 轴和 Z 轴上的中心的 {@link Vector3}。
     *
     * Returns the {@link Vector3} of the center of this block on
     * the X and Z axis.
     *
     */
    bottomCenter(): Vector3;
    /**
     * @remarks
     * Returns whether this block is removed when touched by
     * liquid.
     *
     * @param liquidType
     * The type of liquid this function should be called for.
     * @returns
     * Whether this block is removed when touched by liquid.
     * @throws This function can throw errors.
     *
     * {@link Error}
     *
     * {@link LocationInUnloadedChunkError}
     *
     * {@link LocationOutOfWorldBoundariesError}
     */
    canBeDestroyedByLiquidSpread(liquidType: LiquidType): boolean;
    /**
     * @remarks
     * Returns whether this block can have a liquid placed over it,
     * i.e. be waterlogged.
     *
     * @param liquidType
     * The type of liquid this function should be called for.
     * @returns
     * Whether this block can have a liquid placed over it.
     * @throws This function can throw errors.
     *
     * {@link Error}
     *
     * {@link LocationInUnloadedChunkError}
     *
     * {@link LocationOutOfWorldBoundariesError}
     */
    canContainLiquid(liquidType: LiquidType): boolean;
    /**
     * @beta
     * @remarks
     * Returns whether this block is removed when touched by
     * liquid.
     *
     * @param liquidType
     * The type of liquid this function should be called for.
     * @returns
     * Whether this block is removed when touched by liquid.
     * @throws This function can throw errors.
     *
     * {@link Error}
     *
     * {@link LocationInUnloadedChunkError}
     *
     * {@link LocationOutOfWorldBoundariesError}
     */
    canBeDestroyedByLiquidSpread(liquidType: LiquidType): boolean;
    /**
     * @beta
     * @remarks
     * Returns whether this block can have a liquid placed over it,
     * i.e. be waterlogged.
     *
     * @param liquidType
     * The type of liquid this function should be called for.
     * @returns
     * Whether this block can have a liquid placed over it.
     * @throws This function can throw errors.
     *
     * {@link Error}
     *
     * {@link LocationInUnloadedChunkError}
     *
     * {@link LocationOutOfWorldBoundariesError}
     */
    canContainLiquid(liquidType: LiquidType): boolean;
    /**
     * @beta
     * @remarks
     * 检查在该方块的指定面上放置{@link BlockPermutation}或{@link BlockType}或指定标识符的方块是否可行。
     *
     * Checks to see whether it is valid to place the specified
     * block type or block permutation, on a specified face on this
     * block.
     *
     * @param blockToPlace
     * 被检查放置可行性的{@link BlockPermutation}或{@link BlockType}或方块标识符。
     *
     * Block type or block permutation to check placement for.
     * @param faceToPlaceOn
     * 被检查放置的方向（可选）。
     *
     * Optional specific face of this block to check placement
     * against.
     * @returns
     * 如果在此面可以放置这样的方块则返回 `true` 。
     *
     * Returns `true` if the block type or permutation can be
     * placed on this block, else `false`.
     * @throws This function can throw errors.
     *
     * {@link Error}
     *
     * {@link LocationInUnloadedChunkError}
     *
     * {@link LocationOutOfWorldBoundariesError}
     */
    canPlace(blockToPlace: BlockPermutation | BlockType | string, faceToPlaceOn?: Direction): boolean;
    /**
     * @remarks
     * 返回该方块在 X、Y 和 Z 轴上中心的 {@link Vector3}。
     *
     * Returns the {@link Vector3} of the center of this block on
     * the X, Y, and Z axis.
     *
     */
    center(): Vector3;
    /**
     * @remarks
     * 返回位于该方块东侧（X轴正方向）的 {@link Block}。
     *
     * Returns the {@link Block} to the east of this block
     * (positive in the X direction).
     *
     * @param steps
     * 向东的步数。
     *
     * Number of steps to the east to step before returning.
     * Defaults to: 1
     * @throws This function can throw errors.
     *
     * {@link LocationInUnloadedChunkError}
     *
     * {@link LocationOutOfWorldBoundariesError}
     */
    east(steps?: number): Block | undefined;
    /**
     * @remarks
     * 获取一个方块的组件（代表附加功能），例如，一个箱子方块的库存组件。
     *
     * Gets a component (that represents additional capabilities)
     * for a block - for example, an inventory component of a chest
     * block.
     *
     * @param componentId
     * 组件的标识符 （例如 'minecraft:inventory'）。
     * 如果未指定命名空间前缀，将默认使用 'minecraft:'。
     * 可用的组件标识符可以在 {@link BlockComponentTypes} 枚举中找到。
     *
     * The identifier of the component (e.g.,
     * 'minecraft:inventory'). If no namespace prefix is specified,
     * 'minecraft:' is assumed. Available component IDs are those
     * in the {@link BlockComponentTypes} enum and custom component
     * IDs registered with the {@link BlockComponentRegistry}.
     * @returns
     * 如果该组件存在于该方块，则返回该组件。
     * 否则返回 undefined。
     *
     * Returns the component if it exists on the block, otherwise
     * undefined.
     * @throws This function can throw errors.
     *
     * {@link LocationInUnloadedChunkError}
     *
     * {@link LocationOutOfWorldBoundariesError}
     */
    getComponent<T extends string>(componentId: T): BlockComponentReturnType<T> | undefined;
    /**
     * @remarks
     * Returns all scripting components that are present on this
     * block.
     *
     * @throws This function can throw errors.
     *
     * {@link LocationInUnloadedChunkError}
     *
     * {@link LocationOutOfWorldBoundariesError}
     */
    getComponents(): BlockComponent[];
    /**
     * @remarks
     * 创建一个基于该方块的原型物品对象-{@link ItemStack}，可以与 {@link Container}/{@link ContainerSlot} 接口 一起使用。
     *
     * Creates a prototype item stack based on this block that can
     * be used with Container/ContainerSlot APIs.
     *
     * @param amount
     * 要设置在物品对象-{@link ItemStack}中的这个方块的数量。
     *
     * Number of instances of this block to place in the item
     * stack.
     * Defaults to: 1
     * Bounds: [1, 255]
     * @param withData
     * 是否包括物品对象的附加数据。
     *
     * Whether additional data facets of the item stack are
     * included.
     * Defaults to: false
     * @returns
     * 一个带有指定数量和数据的物品对象。
     * 如果方块类型不兼容，则返回 undefined。
     *
     * An itemStack with the specified amount of items and data.
     * Returns undefined if block type is incompatible.
     * @throws This function can throw errors.
     *
     * {@link LocationInUnloadedChunkError}
     *
     * {@link LocationOutOfWorldBoundariesError}
     */
    getItemStack(amount?: number, withData?: boolean): ItemStack | undefined;
    /**
     * @remarks
     * Returns the total brightness level of light shining on a
     * certain block.
     *
     * @worldMutation
     *
     * @returns
     * The brightness level on the block.
     * @throws This function can throw errors.
     *
     * {@link InvalidArgumentError}
     *
     * {@link LocationInUnloadedChunkError}
     */
    getLightLevel(): number;
    /**
     * @beta
     * @throws This function can throw errors.
     *
     * {@link LocationInUnloadedChunkError}
     *
     * {@link LocationOutOfWorldBoundariesError}
     */
    getMapColor(): RGBA;
    /**
     * @rc
     * @remarks
     * Returns array of all loaded block parts if this block has
     * the 'minecraft:multi_block' trait. If it does not have the
     * trait returns undefined
     *
     * @throws This function can throw errors.
     *
     * {@link LocationInUnloadedChunkError}
     *
     * {@link LocationOutOfWorldBoundariesError}
     */
    getParts(): Block[] | undefined;
    /**
     * @remarks
     * 返回该方块的净红石能量强度。
     * 考虑了所有输入和输出后的总红石能量强度。
     * 表示了一个方块与周围环境中所有红石元件的相互作用后的红石能量状态。
     *
     * Returns the net redstone power of this block.
     *
     * @returns
     * 如果这个方块不适用红石能量，返回 undefined。
     *
     * Returns undefined if redstone power is not applicable to
     * this block.
     * @throws This function can throw errors.
     *
     * {@link LocationInUnloadedChunkError}
     *
     * {@link LocationOutOfWorldBoundariesError}
     */
    getRedstonePower(): number | undefined;
    /**
     * @remarks
     * Returns the brightness level of light shining from the sky
     * on a certain block.
     *
     * @worldMutation
     *
     * @returns
     * The brightness level on the block.
     * @throws This function can throw errors.
     *
     * {@link InvalidArgumentError}
     *
     * {@link LocationInUnloadedChunkError}
     */
    getSkyLightLevel(): number;
    /**
     * @remarks
     * 返回方块被设置的标签的列表。
     *
     * Returns a set of tags for a block.
     *
     * @returns
     * 方块拥有的标签列表。
     *
     * The list of tags that the block has.
     * @throws This function can throw errors.
     *
     * {@link LocationInUnloadedChunkError}
     *
     * {@link LocationOutOfWorldBoundariesError}
     */
    getTags(): string[];
    /**
     * @remarks
     * Returns true if the specified component is present on this
     * block.
     *
     * @param componentId
     * The identifier of the component (e.g.,
     * 'minecraft:inventory') to retrieve. If no namespace prefix
     * is specified, 'minecraft:' is assumed.
     * @throws This function can throw errors.
     *
     * {@link LocationInUnloadedChunkError}
     *
     * {@link LocationOutOfWorldBoundariesError}
     */
    hasComponent(componentId: string): boolean;
    /**
     * @remarks
     * 检查该方块的{@link BlockPermutation}是否具有特定的标签。
     *
     * Checks to see if the permutation of this block has a
     * specific tag.
     *
     * @param tag
     * 要检查的标签。
     *
     * Tag to check for.
     * @returns
     * 如果该方块的{@link BlockPermutation}具有该标签，则返回 `true`，否则返回 `false`。
     *
     * Returns `true` if the permutation of this block has the tag,
     * else `false`.
     * @throws This function can throw errors.
     *
     * {@link LocationInUnloadedChunkError}
     *
     * {@link LocationOutOfWorldBoundariesError}
     * @seeExample checkBlockTags.ts
     */
    hasTag(tag: string): boolean;
    /**
     * @remarks
     * Returns whether this block stops liquid from flowing.
     *
     * @param liquidType
     * The type of liquid this function should be called for.
     * @returns
     * Whether this block stops liquid from flowing.
     * @throws This function can throw errors.
     *
     * {@link Error}
     *
     * {@link LocationInUnloadedChunkError}
     *
     * {@link LocationOutOfWorldBoundariesError}
     */
    isLiquidBlocking(liquidType: LiquidType): boolean;
    /**
     * @remarks
     * Returns whether liquid can flow into the block from the
     * provided direction, or flow out from the provided direction
     * when liquid is placed into it with a bucket.
     *
     * @param liquidType
     * The type of liquid this function should be called for.
     * @returns
     * Whether liquid can flow into the block from the provided
     * direction, or flow out from the provided direction when
     * liquid is placed into it with a bucket
     * @throws This function can throw errors.
     *
     * {@link Error}
     *
     * {@link LocationInUnloadedChunkError}
     *
     * {@link LocationOutOfWorldBoundariesError}
     */
    liquidCanFlowFromDirection(liquidType: LiquidType, flowDirection: Direction): boolean;
    /**
     * @remarks
     * Returns whether this block is removed and spawns its item
     * when touched by liquid.
     *
     * @param liquidType
     * The type of liquid this function should be called for.
     * @returns
     * Whether this block is removed and spawns its item when
     * touched by liquid.
     * @throws This function can throw errors.
     *
     * {@link Error}
     *
     * {@link LocationInUnloadedChunkError}
     *
     * {@link LocationOutOfWorldBoundariesError}
     */
    liquidSpreadCausesSpawn(liquidType: LiquidType): boolean;
    /**
     * @remarks
     * Tests whether this block matches a specific criteria.
     *
     * @param blockName
     * Block type identifier to match this API against.
     * @param states
     * Optional set of block states to test this block against.
     * @returns
     * Returns true if the block matches the specified criteria.
     * @throws This function can throw errors.
     *
     * {@link LocationInUnloadedChunkError}
     *
     * {@link LocationOutOfWorldBoundariesError}
     */
    matches(blockName: string, states?: Record<string, boolean | number | string>): boolean;
    /**
     * @remarks
     * 返回位于该方块北侧（Z轴负方向）的 {@link Block}。
     *
     * Returns the {@link Block} to the north of this block
     * (negative in the Z direction).
     *
     * @param steps
     * 在返回之前，向北移动的步数。
     *
     * Number of steps to the north to step before returning.
     * Defaults to: 1
     * @throws This function can throw errors.
     *
     * {@link LocationInUnloadedChunkError}
     *
     * {@link LocationOutOfWorldBoundariesError}
     */
    north(steps?: number): Block | undefined;
    /**
     * @remarks
     * Returns a block at an offset relative vector to this block.
     *
     * @param offset
     * The offset vector. For example, an offset of 0, 1, 0 will
     * return the block above the current block.
     * @returns
     * Block at the specified offset, or undefined if that block
     * could not be retrieved (for example, the block and its
     * relative chunk is not loaded yet.)
     * @throws This function can throw errors.
     *
     * {@link LocationInUnloadedChunkError}
     *
     * {@link LocationOutOfWorldBoundariesError}
     */
    offset(offset: Vector3): Block | undefined;
    /**
     * @remarks
     * 在维度中将方块设置为{@link BlockPermutation}的状态。
     *
     * Sets the block in the dimension to the state of the
     * permutation.
     *
     * @worldMutation
     *
     * @param permutation
     * 包含方块一组属性状态的{@link BlockPermutation}。
     *
     * Permutation that contains a set of property states for the
     * Block.
     * @throws This function can throw errors.
     *
     * {@link LocationInUnloadedChunkError}
     *
     * {@link LocationOutOfWorldBoundariesError}
     */
    setPermutation(permutation: BlockPermutation): void;
    /**
     * @remarks
     * 设置方块的类型。
     *
     * Sets the type of block.
     *
     * @worldMutation
     *
     * @param blockType
     * 要应用的方块类型的标识符或方块类型，
     * 例如，`minecraft:powered_repeater`。
     *
     * Identifier of the type of block to apply - for example,
     * minecraft:powered_repeater.
     * @throws This function can throw errors.
     *
     * {@link Error}
     *
     * {@link LocationInUnloadedChunkError}
     *
     * {@link LocationOutOfWorldBoundariesError}
     */
    setType(blockType: BlockType | string): void;
    /**
     * @remarks
     * Sets whether this block has a water logged state - for
     * example, whether stairs are submerged within water.
     *
     * @worldMutation
     *
     * @param isWaterlogged
     * true if the block should have water within it.
     * @throws This function can throw errors.
     *
     * {@link Error}
     *
     * {@link LocationInUnloadedChunkError}
     *
     * {@link LocationOutOfWorldBoundariesError}
     */
    setWaterlogged(isWaterlogged: boolean): void;
    /**
     * @remarks
     * 返回位于该方块南侧（Z轴正方向）的 {@link Block}。
     *
     * Returns the {@link Block} to the south of this block
     * (positive in the Z direction).
     *
     * @param steps
     * 在返回之前，向南移动的步数。
     *
     * Number of steps to the south to step before returning.
     * Defaults to: 1
     * @throws This function can throw errors.
     *
     * {@link LocationInUnloadedChunkError}
     *
     * {@link LocationOutOfWorldBoundariesError}
     */
    south(steps?: number): Block | undefined;
    /**
     * @beta
     * @remarks
     * 首先检查放置是否有效，
     * 如何尝试在维度中将方块设置为{@link BlockPermutation}的状态，
     *
     *
     * Tries to set the block in the dimension to the state of the
     * permutation by first checking if the placement is valid.
     *
     * @worldMutation
     *
     * @param permutation
     * 包含一组方块属性状态的{@link BlockPermutation}。
     *
     * Permutation that contains a set of property states for the
     * Block.
     * @returns
     * 如果成功设置了方块的{@link BlockPermutation}，
     * 则返回 `true`，否则返回 `false`。
     *
     * Returns `true` if the block permutation data was
     * successfully set, else `false`.
     * @throws This function can throw errors.
     *
     * {@link LocationInUnloadedChunkError}
     *
     * {@link LocationOutOfWorldBoundariesError}
     */
    trySetPermutation(permutation: BlockPermutation): boolean;
    /**
     * @remarks
     * 返回位于该方块西侧（X轴负方向）的 {@link Block}。
     *
     * Returns the {@link Block} to the west of this block
     * (negative in the X direction).
     *
     * @param steps
     * 在返回之前，向西移动的步数。
     *
     * Number of steps to the west to step before returning.
     * Defaults to: 1
     * @throws This function can throw errors.
     *
     * {@link LocationInUnloadedChunkError}
     *
     * {@link LocationOutOfWorldBoundariesError}
     */
    west(steps?: number): Block | undefined;
}

/**
 * @beta
 * Bounding Box Utils is a utility class that provides a number
 * of useful functions for the creation and utility of {@link
 * BlockBoundingBox} objects
 */
export class BlockBoundingBoxUtils {
    private constructor();
    /**
     * @remarks
     * Create a validated instance of a {@link BlockBoundingBox}
     * where the min and max components are guaranteed to be (min
     * <= max)
     *
     * @worldMutation
     *
     * @param min
     * A corner world location
     * @param max
     * A corner world location diametrically opposite
     */
    static createValid(min: Vector3, max: Vector3): BlockBoundingBox;
    /**
     * @remarks
     * Expand a {@link BlockBoundingBox} by a given amount along
     * each axis.
     * Sizes can be negative to perform contraction.
     * Note: corners can be inverted if the contraction size is
     * greater than the span, but the min/max relationship will
     * remain correct
     *
     * @worldMutation
     *
     * @returns
     * Return a new {@link BlockBoundingBox} object representing
     * the changes
     */
    static dilate(box: BlockBoundingBox, size: Vector3): BlockBoundingBox;
    /**
     * @remarks
     * Check if two {@link BlockBoundingBox} objects are identical
     *
     * @worldMutation
     *
     */
    static equals(box: BlockBoundingBox, other: BlockBoundingBox): boolean;
    /**
     * @remarks
     * Expand the initial box object bounds to include the 2nd box
     * argument.  The resultant {@link BlockBoundingBox} object
     * will be a BlockBoundingBox which exactly encompasses the two
     * boxes.
     *
     * @worldMutation
     *
     * @returns
     * A new {@link BlockBoundingBox} instance representing the
     * smallest possible bounding box which can encompass both
     */
    static expand(box: BlockBoundingBox, other: BlockBoundingBox): BlockBoundingBox;
    /**
     * @remarks
     * Calculate the center block of a given {@link
     * BlockBoundingBox} object.
     *
     * @worldMutation
     *
     * @returns
     * Note that {@link BlockBoundingBox} objects represent whole
     * blocks, so the center of boxes which have odd numbered
     * bounds are not mathematically centered...
     * i.e. a BlockBoundingBox( 0,0,0 -> 3,3,3 )  would have a
     * center of (1,1,1)  (not (1.5, 1.5, 1.5) as expected)
     */
    static getCenter(box: BlockBoundingBox): Vector3;
    /**
     * @remarks
     * Calculate the BlockBoundingBox which represents the union
     * area of two intersecting BlockBoundingBoxes
     *
     * @worldMutation
     *
     */
    static getIntersection(box: BlockBoundingBox, other: BlockBoundingBox): BlockBoundingBox | undefined;
    /**
     * @remarks
     * Get the Span of each of the BlockBoundingBox Axis components
     *
     * @worldMutation
     *
     */
    static getSpan(box: BlockBoundingBox): Vector3;
    /**
     * @remarks
     * Check to see if two BlockBoundingBox objects intersect
     *
     * @worldMutation
     *
     */
    static intersects(box: BlockBoundingBox, other: BlockBoundingBox): boolean;
    /**
     * @remarks
     * Check to see if a given coordinate is inside a
     * BlockBoundingBox
     *
     * @worldMutation
     *
     */
    static isInside(box: BlockBoundingBox, pos: Vector3): boolean;
    /**
     * @remarks
     * Check to see if a BlockBoundingBox is valid (i.e. (min <=
     * max))
     *
     * @worldMutation
     *
     */
    static isValid(box: BlockBoundingBox): boolean;
    /**
     * @remarks
     * Move a BlockBoundingBox by a given amount
     *
     * @worldMutation
     *
     * @returns
     * Return a new BlockBoundingBox object which represents the
     * change
     */
    static translate(box: BlockBoundingBox, delta: Vector3): BlockBoundingBox;
}

/**
 * Base type for components associated with blocks.
 */
// @ts-ignore Class inheritance allowed for native defined classes
export class BlockComponent extends Component {
    private constructor();
    /**
     * @remarks
     * Block instance that this component pertains to.
     *
     */
    readonly block: Block;
}

/**
 * Contains information regarding a specific block being
 * broken.
 */
// @ts-ignore Class inheritance allowed for native defined classes
export class BlockComponentBlockBreakEvent extends BlockEvent {
    private constructor();
    /**
     * @remarks
     * The block that caused destruction.
     *
     */
    readonly blockDestructionSource?: Block;
    /**
     * @remarks
     * Returns permutation information about this block before it
     * was broken.
     *
     */
    readonly brokenBlockPermutation: BlockPermutation;
    /**
     * @remarks
     * The Actor that caused destruction.
     *
     */
    readonly entitySource?: Entity;
}

/**
 * Contains information regarding a specific block permutation
 * that was changed from a previous permutation.
 */
// @ts-ignore Class inheritance allowed for native defined classes
export class BlockComponentBlockStateChangeEvent extends BlockEvent {
    private constructor();
    /**
     * @remarks
     * The previous BlockPermutation.
     *
     */
    readonly previousPermutation: BlockPermutation;
}

/**
 * Contains information regarding an event sent by an entity to
 * this block in the world.
 */
// @ts-ignore Class inheritance allowed for native defined classes
export class BlockComponentEntityEvent extends BlockEvent {
    private constructor();
    /**
     * @remarks
     * Returns permutation information about the block receiving
     * the event.
     *
     */
    readonly blockPermutation: BlockPermutation;
    /**
     * @remarks
     * The entity that sent the event.
     *
     */
    readonly entitySource: Entity;
    /**
     * @remarks
     * Name of the event fired by the entity.
     *
     */
    readonly name: string;
}

/**
 * Contains information regarding an entity falling onto a
 * specific block.
 */
// @ts-ignore Class inheritance allowed for native defined classes
export class BlockComponentEntityFallOnEvent extends BlockEvent {
    private constructor();
    /**
     * @remarks
     * The entity that fell onto the block.
     *
     */
    readonly entity?: Entity;
    /**
     * @remarks
     * The distance that the entity fell onto this block with.
     *
     */
    readonly fallDistance: number;
}

/**
 * Contains information regarding a specific block that was
 * placed.
 */
// @ts-ignore Class inheritance allowed for native defined classes
export class BlockComponentOnPlaceEvent extends BlockEvent {
    private constructor();
    /**
     * @remarks
     * Previous block at this location that was replaced.
     *
     */
    readonly previousBlock: BlockPermutation;
}

/**
 * Contains information regarding a specific block being broken
 * by a player.
 */
// @ts-ignore Class inheritance allowed for native defined classes
export class BlockComponentPlayerBreakEvent extends BlockEvent {
    private constructor();
    /**
     * @remarks
     * Returns permutation information about this block before it
     * was broken.
     *
     */
    readonly brokenBlockPermutation: BlockPermutation;
    /**
     * @remarks
     * The player that broke this block.
     *
     */
    readonly player?: Player;
}

/**
 * Contains information regarding a specific block being
 * interacted with.
 */
// @ts-ignore Class inheritance allowed for native defined classes
export class BlockComponentPlayerInteractEvent extends BlockEvent {
    private constructor();
    /**
     * @remarks
     * The block face that was interacted with.
     *
     */
    readonly face: Direction;
    /**
     * @remarks
     * Location relative to the bottom north-west corner of the
     * block that the player interacted with.
     *
     */
    readonly faceLocation?: Vector3;
    /**
     * @remarks
     * The player that interacted with this block.
     *
     */
    readonly player?: Player;
}

/**
 * Contains information regarding an event before a player
 * places a block.
 */
// @ts-ignore Class inheritance allowed for native defined classes
export class BlockComponentPlayerPlaceBeforeEvent extends BlockEvent {
    private constructor();
    /**
     * @remarks
     * If set to true, cancels the block place event.
     *
     */
    cancel: boolean;
    /**
     * @remarks
     * The block face that was placed onto.
     *
     */
    readonly face: Direction;
    /**
     * @remarks
     * The block permutation that will be placed if the event is
     * not cancelled. If set to a different block permutation, that
     * permutation will be placed instead.
     *
     */
    permutationToPlace: BlockPermutation;
    /**
     * @remarks
     * The player that is placing this block.
     *
     */
    readonly player?: Player;
}

/**
 * Contains information regarding a specific block randomly
 * ticking.
 */
// @ts-ignore Class inheritance allowed for native defined classes
export class BlockComponentRandomTickEvent extends BlockEvent {
    private constructor();
}

/**
 * Contains information regarding a specific block redstone
 * update event.
 */
// @ts-ignore Class inheritance allowed for native defined classes
export class BlockComponentRedstoneUpdateEvent extends BlockEvent {
    private constructor();
    /**
     * @beta
     * @remarks
     * The first update event for the redstone component.
     *
     */
    readonly firstUpdate: boolean;
    /**
     * @remarks
     * The redstone signal strength passing through this block. It
     * is guaranteed to be >= the `min_power` of the block's
     * 'minecraft:redstone_consumer' component.
     *
     */
    readonly powerLevel: number;
    /**
     * @remarks
     * The redstone signal strength from the last tick that was
     * passing through this block. It is guaranteed to be >= the
     * `min_power` of the block's 'minecraft:redstone_consumer'
     * component.
     *
     */
    readonly previousPowerLevel: number;
}

export class BlockComponentRegistry {
    private constructor();
    /**
     * @remarks
     * @earlyExecution
     *
     * @throws This function can throw errors.
     *
     * {@link BlockCustomComponentAlreadyRegisteredError}
     *
     * {@link BlockCustomComponentReloadNewComponentError}
     *
     * {@link BlockCustomComponentReloadNewEventError}
     *
     * {@link BlockCustomComponentReloadVersionError}
     *
     * {@link CustomComponentInvalidRegistryError}
     *
     * {@link EngineError}
     *
     * {@link NamespaceNameError}
     */
    registerCustomComponent(name: string, customComponent: BlockCustomComponent): void;
}

/**
 * Contains information regarding an entity stepping off a
 * specific block.
 */
// @ts-ignore Class inheritance allowed for native defined classes
export class BlockComponentStepOffEvent extends BlockEvent {
    private constructor();
    /**
     * @remarks
     * The entity that stepped off the block.
     *
     */
    readonly entity?: Entity;
}

/**
 * Contains information regarding an entity stepping onto a
 * specific block.
 */
// @ts-ignore Class inheritance allowed for native defined classes
export class BlockComponentStepOnEvent extends BlockEvent {
    private constructor();
    /**
     * @remarks
     * The entity that stepped on the block.
     *
     */
    readonly entity?: Entity;
}

/**
 * Contains information regarding a specific block ticking.
 */
// @ts-ignore Class inheritance allowed for native defined classes
export class BlockComponentTickEvent extends BlockEvent {
    private constructor();
}

/**
 * Contains information regarding a specific container block
 * being closed.
 */
// @ts-ignore Class inheritance allowed for native defined classes
export class BlockContainerClosedAfterEvent extends BlockEvent {
    private constructor();
    /**
     * @remarks
     * The source of the block container being closed.
     *
     * @worldMutation
     *
     */
    closeSource: ContainerAccessSource;
}

/**
 * Manages callbacks that are connected to when a block
 * container is closed.
 */
export class BlockContainerClosedAfterEventSignal {
    private constructor();
    /**
     * @remarks
     * Adds a callback that will be called when a block container
     * is closed.
     *
     * @worldMutation
     *
     * @earlyExecution
     *
     */
    subscribe(
        callback: (arg0: BlockContainerClosedAfterEvent) => void,
        options?: BlockContainerAccessEventOptions,
    ): (arg0: BlockContainerClosedAfterEvent) => void;
    /**
     * @remarks
     * Removes a callback from being called when a block container
     * is closed.
     *
     * @worldMutation
     *
     * @earlyExecution
     *
     */
    unsubscribe(callback: (arg0: BlockContainerClosedAfterEvent) => void): void;
}

/**
 * Contains information regarding a specific container block
 * being opened.
 */
// @ts-ignore Class inheritance allowed for native defined classes
export class BlockContainerOpenedAfterEvent extends BlockEvent {
    private constructor();
    /**
     * @remarks
     * The source of the block container being opened.
     *
     * @worldMutation
     *
     */
    openSource: ContainerAccessSource;
}

/**
 * Manages callbacks that are connected to when a block
 * container is opened.
 */
export class BlockContainerOpenedAfterEventSignal {
    private constructor();
    /**
     * @remarks
     * Adds a callback that will be called when a block container
     * is opened.
     *
     * @worldMutation
     *
     * @earlyExecution
     *
     */
    subscribe(
        callback: (arg0: BlockContainerOpenedAfterEvent) => void,
        options?: BlockContainerAccessEventOptions,
    ): (arg0: BlockContainerOpenedAfterEvent) => void;
    /**
     * @remarks
     * Removes a callback from being called when a block container
     * is opened.
     *
     * @worldMutation
     *
     * @earlyExecution
     *
     */
    unsubscribe(callback: (arg0: BlockContainerOpenedAfterEvent) => void): void;
}

/**
 * An instance of a custom component on a block.
 */
// @ts-ignore Class inheritance allowed for native defined classes
export class BlockCustomComponentInstance extends BlockComponent {
    private constructor();
    readonly customComponentParameters: CustomComponentParameters;
}

/**
 * @beta
 * Represents the dynamic properties of a block in the world.
 * Only available with block entities. Up to 1KB per content
 * pack, per block entity in their dynamic properties storage.
 * @seeExample rememberPlayerInteraction.ts
 */
// @ts-ignore Class inheritance allowed for native defined classes
export class BlockDynamicPropertiesComponent extends BlockComponent {
    private constructor();
    static readonly componentId = 'minecraft:dynamic_properties';
    /**
     * @remarks
     * Returns a DynamicProperty that was stored with the provided
     * key. Keys are unique to each content pack and cannot be used
     * to retrieve dynamic properties set from other content packs.
     * Returns undefined if the key was not found.
     *
     * @throws This function can throw errors.
     *
     * {@link Error}
     *
     * {@link InvalidBlockComponentError}
     *
     * {@link LocationInUnloadedChunkError}
     *
     * {@link LocationOutOfWorldBoundariesError}
     */
    get(key: string): boolean | number | string | Vector3 | undefined;
    /**
     * @remarks
     * Sets a dynamic property with the provided key and value.
     * Keys are unique to each content pack and cannot be used to
     * set dynamic properties for other content packs. Values can
     * be either a Number, a String or a Vector3. Setting a
     * property with an undefined value will remove it from the
     * storage. Storage size usage is counted towards the 1KBytes
     * limit per content pack.
     *
     * @worldMutation
     *
     * @throws This function can throw errors.
     *
     * {@link Error}
     *
     * {@link InvalidBlockComponentError}
     *
     * {@link LocationInUnloadedChunkError}
     *
     * {@link LocationOutOfWorldBoundariesError}
     */
    set(key: string, value?: boolean | number | string | Vector3): void;
    /**
     * @remarks
     * Returns the current size, in bytes, of the dynamic
     * properties storage for this block entity. The byte count
     * only accounts for properties set by your content pack. The
     * 1KBytes limit is per content pack.
     *
     * @throws This function can throw errors.
     *
     * {@link Error}
     *
     * {@link InvalidBlockComponentError}
     *
     * {@link LocationInUnloadedChunkError}
     *
     * {@link LocationOutOfWorldBoundariesError}
     */
    totalByteCount(): number;
}

/**
 * Contains information regarding an event that impacts a
 * specific block.
 */
export class BlockEvent {
    private constructor();
    /**
     * @remarks
     * Block currently in the world at the location of this event.
     *
     */
    readonly block: Block;
    /**
     * @remarks
     * Dimension that contains the block that is the subject of
     * this event.
     *
     */
    readonly dimension: Dimension;
}

/**
 * Contains information regarding an explosion that has
 * occurred for a specific block.
 */
// @ts-ignore Class inheritance allowed for native defined classes
export class BlockExplodeAfterEvent extends BlockEvent {
    private constructor();
    /**
     * @remarks
     * Description of the block that has exploded.
     *
     */
    readonly explodedBlockPermutation: BlockPermutation;
    /**
     * @remarks
     * Optional source of the explosion.
     *
     */
    readonly source?: Entity;
}

/**
 * Manages callbacks that are connected to when an explosion
 * occurs, as it impacts individual blocks.
 */
export class BlockExplodeAfterEventSignal {
    private constructor();
    /**
     * @remarks
     * Adds a callback that will be called when an explosion
     * occurs, as it impacts individual blocks.
     *
     * @worldMutation
     *
     * @earlyExecution
     *
     */
    subscribe(callback: (arg0: BlockExplodeAfterEvent) => void): (arg0: BlockExplodeAfterEvent) => void;
    /**
     * @remarks
     * Removes a callback from being called when an explosion
     * occurs, as it impacts individual blocks.
     *
     * @worldMutation
     *
     * @earlyExecution
     *
     */
    unsubscribe(callback: (arg0: BlockExplodeAfterEvent) => void): void;
}

/**
 * Represents the fluid container of a block in the world. Used
 * with blocks like cauldrons.
 */
// @ts-ignore Class inheritance allowed for native defined classes
export class BlockFluidContainerComponent extends BlockComponent {
    private constructor();
    /**
     * @remarks
     * Relative fill level of the fluid container.
     *
     * @worldMutation
     *
     */
    fillLevel: number;
    /**
     * @remarks
     * Custom color of the fluid in the container.
     *
     * @worldMutation
     *
     */
    fluidColor: RGBA;
    static readonly componentId = 'minecraft:fluid_container';
    /**
     * @remarks
     * Adds a dye to the fluid. The dye color is combined with any
     * existing custom color.
     *
     * @worldMutation
     *
     * @throws This function can throw errors.
     */
    addDye(dye: ItemType): void;
    /**
     * @remarks
     * Gets the current fluid type in the container.
     *
     * @worldMutation
     *
     * @throws This function can throw errors.
     */
    getFluidType(): FluidType;
    /**
     * @remarks
     * Sets the current fluid type in the container.
     *
     * @worldMutation
     *
     * @throws This function can throw errors.
     */
    setFluidType(fluidType: FluidType): void;
    /**
     * @remarks
     * Sets a potion item in the container. Changes the container's
     * fluid type to potion.
     *
     * @worldMutation
     *
     * @throws This function can throw errors.
     */
    setPotion(itemStack: ItemStack): void;
}

/**
 * @beta
 * Represents the instruments a block can have assigned to it's
 * up and down faces.
 */
// @ts-ignore Class inheritance allowed for native defined classes
export class BlockInstrumentComponent extends BlockComponent {
    private constructor();
    static readonly componentId = 'minecraft:instrument_sound';
    /**
     * @remarks
     * A getter method to get the name of an instrument for a given
     * valid face Direction.
     *
     * @param face
     * the face Direction to get the instrument name for.
     * @returns
     * Returns the name of the instrument for a given valid face
     * Direction.
     * @throws This function can throw errors.
     *
     * {@link InvalidArgumentError}
     *
     * {@link LocationInUnloadedChunkError}
     *
     * {@link LocationOutOfWorldBoundariesError}
     */
    getInstrumentName(face: Direction): string;
    /**
     * @remarks
     * plays the instrument sound for a given valid face Direction
     * at the components block location using optional
     * WorldSoundOptions.
     *
     * @worldMutation
     *
     * @param face
     * the face Direction for which instrument sound to play.
     * @param soundOptions
     * optional WorldSoundOptions to use when playing the
     * insturment sound; if omitted the default values are used.
     * @throws This function can throw errors.
     *
     * {@link InvalidArgumentError}
     *
     * {@link LocationInUnloadedChunkError}
     *
     * {@link LocationOutOfWorldBoundariesError}
     */
    playInstrumentSound(face: Direction, soundOptions?: WorldSoundOptions): void;
}

/**
 * Represents the inventory of a block in the world. Used with
 * blocks like chests.
 * @seeExample placeItemsInChest.ts
 */
// @ts-ignore Class inheritance allowed for native defined classes
export class BlockInventoryComponent extends BlockComponent {
    private constructor();
    /**
     * @remarks
     * The container which holds an {@link ItemStack}.
     *
     * @throws This property can throw when used.
     */
    readonly container?: Container;
    static readonly componentId = 'minecraft:inventory';
}

/**
 * A BlockLocationIterator returns the next block location of
 * the block volume across which it is iterating.
 * The BlockLocationIterator is used to abstract the shape of
 * the block volume it was fetched from (so it can represent
 * all the block locations that make up rectangles, cubes,
 * spheres, lines and complex shapes).
 * Each iteration pass returns the next valid block location in
 * the parent shape.
 * Unless otherwise specified by the parent shape - the
 * BlockLocationIterator will iterate over a 3D space in the
 * order of increasing X, followed by increasing Z followed by
 * increasing Y.
 * (Effectively stepping across the XZ plane, and when all the
 * locations in that plane are exhausted, increasing the Y
 * coordinate to the next XZ slice)
 */
export class BlockLocationIterator implements Iterable<Vector3> {
    private constructor();
    /**
     * @remarks
     * @worldMutation
     *
     */
    [Symbol.iterator](): Iterator<Vector3>;
    /**
     * @beta
     * @remarks
     * Checks if the underlining block volume has been invalidated.
     * Will return false if the block volume was modified between
     * creating the iterator and iterating it, and true otherwise.
     *
     * @throws This function can throw errors.
     *
     * {@link EngineError}
     */
    isValid(): boolean;
    /**
     * @remarks
     * @worldMutation
     *
     */
    next(): IteratorResult<Vector3>;
}

/**
 * Represents the color of a block when displayed on a map.
 */
// @ts-ignore Class inheritance allowed for native defined classes
export class BlockMapColorComponent extends BlockComponent {
    private constructor();
    /**
     * @remarks
     * Base map color defined for that block.
     *
     * @throws This property can throw when used.
     */
    readonly color: RGBA;
    /**
     * @remarks
     * Returns the base color multiplied to the evaluated tint at
     * the given position.
     *
     */
    readonly tintedColor: RGBA;
    /**
     * @remarks
     * Type of tint applied to the color.
     *
     * @throws This property can throw when used.
     */
    readonly tintMethod: TintMethod;
    static readonly componentId = 'minecraft:map_color';
}

/**
 * Represents a block that can move (such as a piston).
 */
// @ts-ignore Class inheritance allowed for native defined classes
export class BlockMovableComponent extends BlockComponent {
    private constructor();
    /**
     * @throws This property can throw when used.
     *
     * {@link LocationInUnloadedChunkError}
     *
     * {@link LocationOutOfWorldBoundariesError}
     */
    readonly movementType: MovementType;
    /**
     * @throws This property can throw when used.
     *
     * {@link LocationInUnloadedChunkError}
     *
     * {@link LocationOutOfWorldBoundariesError}
     */
    readonly stickyType: StickyType;
    static readonly componentId = 'minecraft:movable';
}

/**
 * Contains the combination of type {@link BlockType} and
 * properties (also sometimes called block state) which
 * describe a block (but does not belong to a specific {@link
 * Block}).
 * @seeExample addTranslatedSign.ts 9e2fd749
 */
export class BlockPermutation {
    private constructor();
    /**
     * @remarks
     * Key for the localization of this BlockPermutation's name
     * used in .lang files.
     *
     */
    readonly localizationKey: string;
    /**
     * @remarks
     * The {@link BlockType} that the permutation has.
     *
     */
    readonly 'type': BlockType;
    /**
     * @remarks
     * Returns whether this block is removed when touched by
     * liquid.
     *
     * @param liquidType
     * The type of liquid this function should be called for.
     * @returns
     * Whether this block is removed when touched by liquid.
     * @throws This function can throw errors.
     */
    canBeDestroyedByLiquidSpread(liquidType: LiquidType): boolean;
    /**
     * @remarks
     * Returns whether this block can have a liquid placed over it,
     * i.e. be waterlogged.
     *
     * @param liquidType
     * The type of liquid this function should be called for.
     * @returns
     * Whether this block can have a liquid placed over it.
     * @throws This function can throw errors.
     */
    canContainLiquid(liquidType: LiquidType): boolean;
    /**
     * @remarks
     * Returns all available block states associated with this
     * block.
     *
     * @returns
     * Returns the list of all of the block states that the
     * permutation has.
     */
    getAllStates(): Record<string, boolean | number | string>;
    /**
     * @remarks
     * Retrieves a prototype item stack based on this block
     * permutation that can be used with item
     * Container/ContainerSlot APIs.
     *
     * @param amount
     * Number of instances of this block to place in the prototype
     * item stack.
     * Defaults to: 1
     * Bounds: [1, 255]
     */
    getItemStack(amount?: number): ItemStack | undefined;
    /**
     * @remarks
     * Gets a state for the permutation.
     *
     * @param stateName
     * Name of the block state who's value is to be returned.
     * @returns
     * Returns the state if the permutation has it, else
     * `undefined`.
     */
    getState<T extends keyof BlockStateSuperset>(
        stateName: T,
    ): BlockStateSuperset[T] | undefined;
    /**
     * @remarks
     * Creates a copy of the permutation.
     *
     */
    getTags(): string[];
    /**
     * @remarks
     * Checks to see if the permutation has a specific tag.
     *
     * @returns
     * Returns `true` if the permutation has the tag, else `false`.
     * @seeExample checkBlockTags.ts
     */
    hasTag(tag: string): boolean;
    /**
     * @remarks
     * Returns whether this block stops liquid from flowing.
     *
     * @param liquidType
     * The type of liquid this function should be called for.
     * @returns
     * Whether this block stops liquid from flowing.
     * @throws This function can throw errors.
     */
    isLiquidBlocking(liquidType: LiquidType): boolean;
    /**
     * @remarks
     * Returns whether this block is removed and spawns its item
     * when touched by liquid.
     *
     * @param liquidType
     * The type of liquid this function should be called for.
     * @returns
     * Whether this block is removed and spawns its item when
     * touched by liquid.
     * @throws This function can throw errors.
     */
    liquidSpreadCausesSpawn(liquidType: LiquidType): boolean;
    /**
     * @remarks
     * Returns a boolean whether a specified permutation matches
     * this permutation. If states is not specified, matches checks
     * against the set of types more broadly.
     *
     * @param blockName
     * An optional set of states to compare against.
     */
    matches<T extends string = MinecraftBlockTypes>(
        blockName: T,
        states?: BlockStateArg<T>,
    ): boolean;
    /**
     * @remarks
     * Returns a derived BlockPermutation with a specific property
     * set.
     *
     * @param name
     * Identifier of the block property.
     * @param value
     * Value of the block property.
     * @throws This function can throw errors.
     */
    withState<T extends keyof BlockStateSuperset>(
        name: T,
        value: BlockStateSuperset[T],
    ): BlockPermutation;
    /**
     * @remarks
     * Given a type identifier and an optional set of properties,
     * will return a BlockPermutation object that is usable in
     * other block APIs (e.g., block.setPermutation)
     *
     * @param blockName
     * Identifier of the block to check.
     * @throws This function can throw errors.
     * @seeExample addBlockColorCube.ts
     */
    static resolve<T extends string = MinecraftBlockTypes>(
        blockName: T,
        states?: BlockStateArg<T>,
    ): BlockPermutation;
}

/**
 * When present, this block has piston-like behavior. Contains
 * additional properties for discovering block piston state.
 */
// @ts-ignore Class inheritance allowed for native defined classes
export class BlockPistonComponent extends BlockComponent {
    private constructor();
    /**
     * @remarks
     * Whether the piston is in the process of expanding or
     * retracting.
     *
     * @throws This property can throw when used.
     */
    readonly isMoving: boolean;
    /**
     * @remarks
     * The current state of the piston.
     *
     * @throws This property can throw when used.
     */
    readonly state: BlockPistonState;
    static readonly componentId = 'minecraft:piston';
    /**
     * @remarks
     * Retrieves a set of blocks that this piston is connected
     * with.
     *
     * @throws This function can throw errors.
     */
    getAttachedBlocks(): Block[];
    /**
     * @remarks
     * Retrieves a set of block locations that this piston is
     * connected with.
     *
     * @throws This function can throw errors.
     */
    getAttachedBlocksLocations(): Vector3[];
}

/**
 * Represents a how a block interacts with precipitation (such
 * as rain or snow).
 */
// @ts-ignore Class inheritance allowed for native defined classes
export class BlockPrecipitationInteractionsComponent extends BlockComponent {
    private constructor();
    static readonly componentId = 'minecraft:precipitation_interactions';
    /**
     * @remarks
     * Returns `true` if falling snow will accumulate naturally on
     * the block. Returns `false` if snow will not accumulate on
     * the block.
     *
     * @throws This function can throw errors.
     *
     * {@link LocationInUnloadedChunkError}
     *
     * {@link LocationOutOfWorldBoundariesError}
     */
    accumulatesSnow(): boolean;
    /**
     * @remarks
     * Returns `true` if this block can have snow within it, like a
     * flower submerged in snow. Returns `false` if this block
     * cannot have snow within it.
     *
     * @throws This function can throw errors.
     *
     * {@link LocationInUnloadedChunkError}
     *
     * {@link LocationOutOfWorldBoundariesError}
     */
    isSnowLoggable(): boolean;
    /**
     * @remarks
     * Returns `true` if rain will not go through the block.
     * Returns `false` if rain should go through the block.
     *
     * @throws This function can throw errors.
     *
     * {@link LocationInUnloadedChunkError}
     *
     * {@link LocationOutOfWorldBoundariesError}
     */
    obstructsRain(): boolean;
}

/**
 * Represents a block that can play a record.
 */
// @ts-ignore Class inheritance allowed for native defined classes
export class BlockRecordPlayerComponent extends BlockComponent {
    private constructor();
    static readonly componentId = 'minecraft:record_player';
    /**
     * @remarks
     * Ejects the currently set record of this record-playing
     * block.
     *
     * @worldMutation
     *
     * @throws This function can throw errors.
     */
    ejectRecord(): void;
    /**
     * @remarks
     * Gets the currently set record of this record-playing block.
     *
     * @throws This function can throw errors.
     */
    getRecord(): ItemStack | undefined;
    /**
     * @remarks
     * Returns true if the record-playing block is currently
     * playing a record.
     *
     * @throws This function can throw errors.
     */
    isPlaying(): boolean;
    /**
     * @remarks
     * Pauses the currently playing record of this record-playing
     * block.
     *
     * @worldMutation
     *
     * @throws This function can throw errors.
     */
    pauseRecord(): void;
    /**
     * @remarks
     * Plays the currently set record of this record-playing block.
     *
     * @worldMutation
     *
     * @throws This function can throw errors.
     */
    playRecord(): void;
    /**
     * @remarks
     * Sets and plays a record based on an item type.
     *
     * @worldMutation
     *
     * @param startPlaying
     * Defaults to: true
     * @throws This function can throw errors.
     */
    setRecord(recordItemType?: ItemType | string, startPlaying?: boolean): void;
}

/**
 * Represents a block that can output a redstone signal.
 */
// @ts-ignore Class inheritance allowed for native defined classes
export class BlockRedstoneProducerComponent extends BlockComponent {
    private constructor();
    /**
     * @remarks
     * Gets the power that this block outputs to circuit system.
     * Returns error if block is no longer valid or if block
     * doesn't have a 'minecraft:redstone_producer' component.
     *
     * @throws This property can throw when used.
     *
     * {@link InvalidBlockComponentError}
     */
    readonly power: number;
    static readonly componentId = 'minecraft:redstone_producer';
    /**
     * @remarks
     * Gets the faces of this block that can connect to the circuit
     * and output power. Returns error if block is no longer valid
     * or if block doesn't have a 'minecraft:redstone_producer'
     * component.
     *
     * @throws This function can throw errors.
     *
     * {@link InvalidBlockComponentError}
     */
    getConnectedFaces(): Direction[];
    /**
     * @remarks
     * Gets the block face that strongly powers the block touching
     * it. If the 'minecraft:redstone_producer' block component
     * hasn't defined a 'strongly_powered_face' then this method
     * returns 'undefined'. Returns error if block is no longer
     * valid or if block doesn't have a
     * 'minecraft:redstone_producer' component.
     *
     * @throws This function can throw errors.
     *
     * {@link InvalidBlockComponentError}
     */
    getStronglyPoweredFace(): Direction | undefined;
}

/**
 * Represents a block that can display text on it.
 * @seeExample addSign.ts
 * @seeExample addTwoSidedSign.ts
 * @seeExample updateSignText.ts
 * @seeExample addTranslatedSign.ts 9e2fd749
 */
// @ts-ignore Class inheritance allowed for native defined classes
export class BlockSignComponent extends BlockComponent {
    private constructor();
    /**
     * @remarks
     * Whether or not players can edit the sign. This happens if a
     * sign has had a honeycomb used on it or `setWaxed` was called
     * on the sign.
     *
     * @throws This property can throw when used.
     */
    readonly isWaxed: boolean;
    static readonly componentId = 'minecraft:sign';
    /**
     * @remarks
     * Returns the RawText of the sign if `setText` was called with
     * a RawMessage or a RawText object, otherwise returns
     * undefined.
     *
     * @param side
     * The side of the sign to read the message from. If not
     * provided, this will return the message from the front side
     * of the sign.
     * Defaults to: 0
     * @throws This function can throw errors.
     */
    getRawText(side?: SignSide): RawText | undefined;
    /**
     * @remarks
     * Returns the text of the sign if `setText` was called with a
     * string, otherwise returns undefined.
     *
     * @param side
     * The side of the sign to read the message from. If not
     * provided, this will return the message from the front side
     * of the sign.
     * Defaults to: 0
     * @throws This function can throw errors.
     */
    getText(side?: SignSide): string | undefined;
    /**
     * @remarks
     * Gets the dye that is on the text or undefined if the sign
     * has not been dyed.
     *
     * @param side
     * The side of the sign to read the dye from. If not provided,
     * this will return the dye on the front side of the sign.
     * Defaults to: 0
     * @throws This function can throw errors.
     */
    getTextDyeColor(side?: SignSide): DyeColor | undefined;
    /**
     * @remarks
     * Sets the text of the sign component.
     *
     * @worldMutation
     *
     * @param message
     * The message to set on the sign. If set to a string, then
     * call `getText` to read that string. If set to a RawMessage,
     * then calling `getRawText` will return a RawText.
     * @param side
     * The side of the sign the message will be set on. If not
     * provided, the message will be set on the front side of the
     * sign.
     * Defaults to: 0
     * @throws
     * Throws if the provided message is greater than 512
     * characters in length.
     */
    setText(message: RawMessage | string, side?: SignSide): void;
    /**
     * @remarks
     * Sets the dye color of the text.
     *
     * @worldMutation
     *
     * @param color
     * The dye color to apply to the sign or undefined to clear the
     * dye on the sign.
     * Defaults to: null
     * @param side
     * The side of the sign the color will be set on. If not
     * provided, the color will be set on the front side of the
     * sign.
     * Defaults to: 0
     * @throws This function can throw errors.
     */
    setTextDyeColor(color?: DyeColor, side?: SignSide): void;
    /**
     * @remarks
     * Makes it so players cannot edit this sign.
     *
     * @worldMutation
     *
     * @throws This function can throw errors.
     */
    setWaxed(waxed: boolean): void;
}

/**
 * Enumerates all {@link BlockStateType}s.
 */
export class BlockStates {
    private constructor();
    /**
     * @remarks
     * Retrieves a specific block state instance.
     *
     * @returns
     * Returns the {@link Block} state instance if it is found. If
     * the block state instance is not found returns undefined.
     */
    static get(stateName: string): BlockStateType | undefined;
    /**
     * @remarks
     * Retrieves a set of all available block states.
     *
     */
    static getAll(): BlockStateType[];
}

/**
 * Represents a configurable state value of a block instance.
 * For example, the facing direction of stairs is accessible as
 * a block state.
 */
export class BlockStateType {
    private constructor();
    /**
     * @remarks
     * Identifier of the block property.
     *
     */
    readonly id: string;
    /**
     * @remarks
     * A set of valid values for the block property.
     *
     */
    readonly validValues: (boolean | number | string)[];
}

/**
 * The type (or template) of a block. Does not contain
 * permutation data (state) other than the type of block it
 * represents. This type was introduced as of version
 * 1.17.10.21.
 */
export class BlockType {
    private constructor();
    /**
     * @remarks
     * Block type name - for example, `minecraft:acacia_stairs`.
     *
     */
    readonly id: string;
    /**
     * @remarks
     * Key for the localization of this BlockType's name used in
     * .lang files.
     *
     */
    readonly localizationKey: string;
}

/**
 * Contains a catalog of Minecraft Block Types that are
 * available in this world.
 */
export class BlockTypes {
    private constructor();
    /**
     * @remarks
     * Returns a BlockType object for the specified identifier.
     *
     * @param typeName
     * Identifier of the block type. Should follow a namespace:id
     * pattern, such as minecraft:dirt.
     * @returns
     * BlockType object, or undefined if the block type is not
     * available within this world.
     */
    static get(typeName: string): BlockType | undefined;
    /**
     * @remarks
     * Returns a collection of all available block types.
     *
     */
    static getAll(): BlockType[];
}

/**
 * A BlockVolume is a simple interface to an object which
 * represents a 3D rectangle of a given size (in blocks) at a
 * world block location.
 * Note that these are not analogous to "min" and "max" values,
 * in that the vector components are not guaranteed to be in
 * any order.
 * In addition, these vector positions are not interchangeable
 * with BlockLocation.
 * If you want to get this volume represented as range of of
 * BlockLocations, you can use the getBoundingBox utility
 * function.
 * This volume class will maintain the ordering of the corner
 * indexes as initially set. imagine that each corner is
 * assigned in Editor - as you move the corner around
 * (potentially inverting the min/max relationship of the
 * bounds) - what
 * you had originally selected as the top/left corner would
 * traditionally become the bottom/right.
 * When manually editing these kinds of volumes, you need to
 * maintain the identity of the corner as you edit - the
 * BlockVolume utility functions do this.
 *
 * Important to note that this measures block sizes (to/from) -
 * a normal AABB (0,0,0) to (0,0,0) would traditionally be of
 * size (0,0,0)
 * However, because we're measuring blocks - the size or span
 * of a BlockVolume would actually be (1,1,1)
 *
 */
// @ts-ignore Class inheritance allowed for native defined classes
export class BlockVolume extends BlockVolumeBase {
    /**
     * @remarks
     * A world block location that represents a corner in a 3D
     * rectangle
     *
     */
    'from': Vector3;
    /**
     * @remarks
     * A world block location that represents the opposite corner
     * in a 3D rectangle
     *
     */
    to: Vector3;
    constructor(from: Vector3, to: Vector3);
    /**
     * @remarks
     * Check to see if the given location is directly adjacent to
     * the outer surface of a BlockVolume.
     *
     *
     * @param pos
     * The world block location to test
     * @returns
     * If the location is either inside or more than 0 blocks away,
     * the function will return false.
     * If the location is directly contacting the outer surface of
     * the BlockVolume, the function will return true.
     */
    doesLocationTouchFaces(pos: Vector3): boolean;
    /**
     * @remarks
     * Check to see if a two block volumes are directly adjacent
     * and two faces touch.
     *
     * @param other
     * The volume to test
     * @returns
     * If the outer faces of both block volumes touch and are
     * directly adjacent at any point, return true.
     */
    doesVolumeTouchFaces(other: BlockVolume): boolean;
    /**
     * @remarks
     * Return an enumeration which represents the intersection
     * between two BlockVolume objects
     *
     */
    intersects(other: BlockVolume): BlockVolumeIntersection;
}

/**
 * Base type for BlockVolumes.
 */
export class BlockVolumeBase {
    private constructor();
    /**
     * @remarks
     * Fetch a {@link BlockLocationIterator} that represents all of
     * the block world locations within the specified volume
     *
     */
    getBlockLocationIterator(): BlockLocationIterator;
    /**
     * @beta
     * @remarks
     * Return a {@link BlockBoundingBox} object which represents
     * the validated min and max coordinates of the volume
     *
     * @throws This function can throw errors.
     */
    getBoundingBox(): BlockBoundingBox;
    /**
     * @remarks
     * Return the capacity (volume) of the BlockVolume (W*D*H)
     *
     */
    getCapacity(): number;
    /**
     * @beta
     * @remarks
     * Returns a list of block positions within the volume that are
     * closest to a given location, sorted by distance (nearest
     * first)
     *
     * @param count
     * Number of closest positions to return
     * @param location
     * Position to measure distance from
     * @throws This function can throw errors.
     *
     * {@link ArgumentOutOfBoundsError}
     */
    getClosest(count: number, location: Vector3): Vector3[];
    /**
     * @beta
     * @remarks
     * Returns a list of block positions within the volume that are
     * farthest from a given location, sorted by distance (farthest
     * first)
     *
     * @param count
     * Number of farthest positions to return
     * @param location
     * Position to measure distance from
     * @throws This function can throw errors.
     *
     * {@link ArgumentOutOfBoundsError}
     */
    getFarthest(count: number, location: Vector3): Vector3[];
    /**
     * @remarks
     * Get the largest corner position of the volume (guaranteed to
     * be >= min)
     *
     * @throws This function can throw errors.
     */
    getMax(): Vector3;
    /**
     * @remarks
     * Get the smallest corner position of the volume (guaranteed
     * to be <= max)
     *
     * @throws This function can throw errors.
     */
    getMin(): Vector3;
    /**
     * @remarks
     * Get a {@link Vector3} object where each component represents
     * the number of blocks along that axis
     *
     */
    getSpan(): Vector3;
    /**
     * @remarks
     * Check to see if a given world block location is inside a
     * BlockVolume
     *
     */
    isInside(location: Vector3): boolean;
    /**
     * @remarks
     * Move a BlockVolume by a specified amount
     *
     * @param delta
     * Amount of blocks to move by
     */
    translate(delta: Vector3): void;
}

/**
 * Contains information related to changes to a button push.
 * @seeExample buttonPushEvent.ts
 */
// @ts-ignore Class inheritance allowed for native defined classes
export class ButtonPushAfterEvent extends BlockEvent {
    private constructor();
    /**
     * @remarks
     * Optional source that triggered the button push.
     *
     */
    readonly source: Entity;
}

/**
 * Manages callbacks that are connected to when a button is
 * pushed.
 * @seeExample buttonPushEvent.ts
 */
export class ButtonPushAfterEventSignal {
    private constructor();
    /**
     * @remarks
     * Adds a callback that will be called when a button is pushed.
     *
     * @worldMutation
     *
     * @earlyExecution
     *
     */
    subscribe(callback: (arg0: ButtonPushAfterEvent) => void): (arg0: ButtonPushAfterEvent) => void;
    /**
     * @remarks
     * Removes a callback from being called when a button is
     * pushed.
     *
     * @worldMutation
     *
     * @earlyExecution
     *
     */
    unsubscribe(callback: (arg0: ButtonPushAfterEvent) => void): void;
}

/**
 * Contains methods relating to the active camera for the
 * specified player.
 */
export class Camera {
    private constructor();
    /**
     * @remarks
     * Returns whether the Camera is valid to access and use. A
     * Camera is considered valid when the owning Player of the
     * Camera is loaded and valid itself.
     *
     */
    readonly isValid: boolean;
    /**
     * @beta
     * @remarks
     * @worldMutation
     *
     * @throws This function can throw errors.
     *
     * {@link ArgumentOutOfBoundsError}
     *
     * {@link InvalidEntityError}
     */
    addShake(shakeCameraOptions: CameraShakeOptions): void;
    /**
     * @remarks
     * Attaches the camera to a non-player entity.
     *
     * @worldMutation
     *
     * @param attachCameraOptions
     * Options for the entity the camera is attaching to. Contains
     * the entity identifier and optional entity location.
     * @throws This function can throw errors.
     */
    attachToEntity(attachCameraOptions?: CameraAttachOptions): void;
    /**
     * @remarks
     * Clears the active camera for the specified player. Causes
     * the specified players to end any in-progress camera
     * perspectives, including any eased camera motions, and return
     * to their normal perspective.
     *
     * @worldMutation
     *
     * @throws This function can throw errors.
     */
    clear(): void;
    /**
     * @remarks
     * Begins a camera fade transition. A fade transition is a
     * full-screen color that fades-in, holds, and then fades-out.
     *
     * @worldMutation
     *
     * @param fadeCameraOptions
     * Additional options around camera fade operations.
     * @throws This function can throw errors.
     */
    fade(fadeCameraOptions?: CameraFadeOptions): void;
    /**
     * @remarks
     * @worldMutation
     *
     * @throws This function can throw errors.
     */
    playAnimation(splineType: CatmullRomSpline | LinearSpline, cameraAnimationOptions: AnimationOptions): void;
    /**
     * @remarks
     * Sets the current active camera for the specified player.
     *
     * @worldMutation
     *
     * @param cameraPreset
     * Identifier of a camera preset file defined within JSON.
     * @param setOptions
     * Additional options for the camera.
     * @throws This function can throw errors.
     */
    setCamera(
        cameraPreset: string,
        setOptions?:
            | CameraFixedBoomOptions
            | CameraSetFacingOptions
            | CameraSetLocationOptions
            | CameraSetPosOptions
            | CameraSetRotOptions
            | CameraTargetOptions,
    ): void;
    /**
     * @beta
     * @remarks
     * Sets the current active camera with easing.
     *
     * @worldMutation
     *
     * @param cameraPreset
     * Identifier of a camera preset file defined within JSON.
     * @param easeOptions
     * Options to ease the camera from the previous camera to the
     * current one.
     * @throws
     * Throws when easing to minecraft:first_person presets
     * currently without the experimental cameras toggle enabled.
     */
    setCameraWithEase(cameraPreset: string, easeOptions: EaseOptions): void;
    /**
     * @remarks
     * Sets the current active camera for the specified player and
     * resets the position and rotation to the values defined in
     * the JSON.
     *
     * @worldMutation
     *
     * @param cameraPreset
     * Identifier of a camera preset file defined within JSON.
     * @param easeOptions
     * Options to ease the camera back to its original position and
     * rotation.
     * @throws This function can throw errors.
     */
    setDefaultCamera(cameraPreset: string, easeOptions?: EaseOptions): void;
    /**
     * @remarks
     * @worldMutation
     *
     * @throws This function can throw errors.
     */
    setFov(fovCameraOptions?: CameraFovOptions): void;
    /**
     * @beta
     * @remarks
     * @worldMutation
     *
     * @throws This function can throw errors.
     *
     * {@link InvalidEntityError}
     */
    stopShaking(): void;
}

/**
 * @beta
 * Loot item function that will try to copy the block entity
 * data from the destroyed block to the dropped item.
 */
// @ts-ignore Class inheritance allowed for native defined classes
export class CarryOverBlockEntityDataFunction extends LootItemFunction {
    private constructor();
    /**
     * @remarks
     * If true, and if the block entity had dynamic_properties, the
     * function will copy the dynamic properties from the block
     * entity to the dropped item.
     *
     */
    readonly dynamicProperties: boolean;
}

/**
 * CatmullRom spline creation.
 */
export class CatmullRomSpline {
    /**
     * @remarks
     * Control points for the CatmullRom curve.
     *
     * @worldMutation
     *
     */
    controlPoints: Vector3[];
}

/**
 * @beta
 * An event that fires as players enter chat messages.
 */
export class ChatSendAfterEvent {
    private constructor();
    /**
     * @remarks
     * Message that is being broadcast.
     *
     */
    readonly message: string;
    /**
     * @remarks
     * Player that sent the chat message.
     *
     */
    readonly sender: Player;
    /**
     * @remarks
     * Optional list of players that will receive this message. If
     * defined, this message is directly targeted to one or more
     * players (i.e., is not broadcast.)
     *
     */
    readonly targets?: Player[];
}

/**
 * @beta
 * Manages callbacks that are connected to chat messages being
 * sent.
 */
export class ChatSendAfterEventSignal {
    private constructor();
    /**
     * @remarks
     * Adds a callback that will be called when new chat messages
     * are sent.
     *
     * @worldMutation
     *
     * @earlyExecution
     *
     */
    subscribe(callback: (arg0: ChatSendAfterEvent) => void): (arg0: ChatSendAfterEvent) => void;
    /**
     * @remarks
     * Removes a callback from being called when new chat messages
     * are sent.
     *
     * @worldMutation
     *
     * @earlyExecution
     *
     */
    unsubscribe(callback: (arg0: ChatSendAfterEvent) => void): void;
}

/**
 * @beta
 * An event that fires as players enter chat messages.
 */
export class ChatSendBeforeEvent {
    private constructor();
    /**
     * @remarks
     * If set to true in a beforeChat event handler, this message
     * is not broadcast out.
     *
     */
    cancel: boolean;
    /**
     * @remarks
     * Message that is being broadcast.
     *
     */
    readonly message: string;
    /**
     * @remarks
     * Player that sent the chat message.
     *
     */
    readonly sender: Player;
    /**
     * @remarks
     * Optional list of players that will receive this message. If
     * defined, this message is directly targeted to one or more
     * players (i.e., is not broadcast.)
     *
     */
    readonly targets?: Player[];
}

/**
 * @beta
 * Manages callbacks that are connected to an event that fires
 * before chat messages are sent.
 * @seeExample customCommand.ts
 */
export class ChatSendBeforeEventSignal {
    private constructor();
    /**
     * @remarks
     * Adds a callback that will be called before new chat messages
     * are sent.
     *
     * @worldMutation
     *
     * @earlyExecution
     *
     * @param callback
     * This closure is called with restricted-execution privilege.
     * @returns
     * Closure that is called with restricted-execution privilege.
     */
    subscribe(callback: (arg0: ChatSendBeforeEvent) => void): (arg0: ChatSendBeforeEvent) => void;
    /**
     * @remarks
     * Removes a callback from being called before new chat
     * messages are sent.
     *
     * @worldMutation
     *
     * @earlyExecution
     *
     * @param callback
     * This closure is called with restricted-execution privilege.
     */
    unsubscribe(callback: (arg0: ChatSendBeforeEvent) => void): void;
}

/**
 * Contains the device information for a client instance.
 */
// @ts-ignore Class inheritance allowed for native defined classes
export class ClientSystemInfo extends SystemInfo {
    private constructor();
    /**
     * @remarks
     * The locale selected by the client (e.g., en_US, fr_FR,
     * ja_JP). Note that in most cases, server scripts should not
     * use this property to manually localize text. Instead, use
     * {@link RawMessage} with a translate field to send
     * localization keys, allowing each client to resolve them in
     * their own language automatically. Direct use of locale for
     * localization is fragile and may produce unexpected results
     * when players with different languages are on the same
     * server.
     *
     */
    readonly locale: string;
    /**
     * @remarks
     * The max render distance for the device in chunks.
     *
     */
    readonly maxRenderDistance: number;
    /**
     * @remarks
     * The platform type of the device.
     *
     */
    readonly platformType: PlatformType;
}

/**
 * Contains return data on the result of a command execution.
 */
export class CommandResult {
    private constructor();
    /**
     * @remarks
     * If the command operates against a number of entities,
     * blocks, or items, this returns the number of successful
     * applications of this command.
     *
     */
    readonly successCount: number;
}

/**
 * Base class for downstream Component implementations.
 */
export class Component {
    private constructor();
    /**
     * @remarks
     * Returns whether the component is valid. A component is
     * considered valid if its owner is valid, in addition to any
     * addition to any additional validation required by the
     * component.
     *
     */
    readonly isValid: boolean;
    /**
     * @remarks
     * Identifier of the component.
     *
     */
    readonly typeId: string;
}

/**
 * @beta
 * The Compound Block Volume is a collection of individual
 * block volume definitions which, as a collection, define a
 * larger volume of (sometimes non-contiguous) irregular
 * shapes.
 * This class is loosely based on the concept of CSG
 * (Computational Solid Geometry) and allows a user to create
 * complex volumes by building a stack of volumes and voids to
 * make a larger single volume.
 * For example - normally a creator would create a hollow cube
 * by creating 6 "wall" surfaces for each face.
 * With a Compound Block Volume, a creator can define a hollow
 * cube by creating a single outer solid cube, and then
 * defining a further single 'void' cube inside the larger one.
 * Similarly, the Compound Block Volume can represent irregular
 * shaped volumes (e.g. a tree consists of a trunk and lots of
 * leaf cubes which are not necessarily contiguously placed).
 * Each of the volumes added to the CompoundBlockVolume are (by
 * default) relative to the origin set (either at construction
 * or via one of the set functions).
 * However, it is also possible to push volumes to the compound
 * collection which are absolute in nature and are not affected
 * by origin changes.
 */
export class CompoundBlockVolume {
    /**
     * @remarks
     * Return the 'capacity' of the bounding rectangle which
     * represents the collection of volumes in the stack
     *
     */
    readonly capacity: number;
    readonly items: CompoundBlockVolumeItem[];
    readonly itemsAbsolute: CompoundBlockVolumeItem[];
    /**
     * @remarks
     * Return the number of volumes (positive and negative) in the
     * volume stack
     *
     */
    readonly volumeCount: number;
    /**
     * @remarks
     * Create a CompoundBlockVolume object
     *
     * @param origin
     * An optional world space origin on which to center the
     * compound volume.
     * If not specified, the origin is set to (0,0,0)
     */
    constructor(origin?: Vector3);
    /**
     * @remarks
     * Clear the contents of the volume stack
     *
     * @worldMutation
     *
     */
    clear(): void;
    /**
     * @remarks
     * Fetch a Block Location Iterator for the Compound Block
     * Volume.  This iterator will allow a creator to iterate
     * across all of the selected volumes within the larger
     * bounding area.
     * Areas of a volume which have been overridden by a
     * subtractive volume will not be included in the iterator
     * step.
     * (i.e. if you push a cube to the stack, and then push a
     * subtractive volume to the same location, then the iterator
     * will step over the initial volume because it is considered
     * negative space)
     * Note that the Block Locations returned by this iterator are
     * in absolute world space (irrespective of whether the
     * compound volume items pushed are absolute or relative)
     *
     * @worldMutation
     *
     */
    getBlockLocationIterator(): BlockLocationIterator;
    /**
     * @remarks
     * Get the largest bounding box that represents a container for
     * all of the volumes on the stack
     * Note that the bounding box returned is represented in
     * absolute world space  (irrespective of whether the compound
     * volume items pushed are absolute or relative)
     *
     * @worldMutation
     *
     */
    getBoundingBox(): BlockBoundingBox;
    /**
     * @remarks
     * Get the max block location of the outermost bounding
     * rectangle which represents the volumes on the stack.
     * Note that the max location returned is in absolute world
     * space (irrespective of whether the compound volume items
     * pushed are absolute or relative)
     *
     * @worldMutation
     *
     */
    getMax(): Vector3;
    /**
     * @remarks
     * Get the min block location of the outermost bounding
     * rectangle which represents the volumes on the stack.
     * Note that the min location returned is in absolute world
     * space (irrespective of whether the compound volume items
     * pushed are absolute or relative)
     *
     * @worldMutation
     *
     */
    getMin(): Vector3;
    /**
     * @remarks
     * Fetch the origin in world space of the compound volume
     *
     * @worldMutation
     *
     */
    getOrigin(): Vector3;
    /**
     * @remarks
     * Return a boolean which signals if there are any volume items
     * pushed to the volume
     *
     * @worldMutation
     *
     */
    isEmpty(): boolean;
    /**
     * @remarks
     * Return a boolean representing whether or not a given
     * absolute world space block location is inside a positive
     * block volume.
     * E.g. if the stack contains a large cube followed by a
     * slightly smaller negative cube, and the test location is
     * within the negative cube - the function will return false
     * because it's not 'inside' a volume (it IS inside the
     * bounding rectangle, but it is not inside a positively
     * defined location)
     *
     * @worldMutation
     *
     */
    isInside(worldLocation: Vector3): boolean;
    /**
     * @remarks
     * Inspect the last entry pushed to the volume stack without
     * affecting the stack contents.
     *
     * @worldMutation
     *
     * @param forceRelativity
     * Determine whether the function returns a
     * CompoundBlockVolumeItem which is forced into either relative
     * or absolute coordinate system.
     * `true` = force returned item to be relative to volume origin
     * `false` = force returned item to be absolute world space
     * location
     *
     * If no flag is specified, the item returned retains whatever
     * relativity it had when it was pushed
     * @returns
     * Returns undefined if the stack is empty
     */
    peekLastVolume(forceRelativity?: CompoundBlockVolumePositionRelativity): CompoundBlockVolumeItem | undefined;
    /**
     * @remarks
     * Remove the last entry from the volume stack.  This will
     * reduce the stack size by one
     *
     * @worldMutation
     *
     */
    popVolume(): boolean;
    /**
     * @remarks
     * Push a volume item to the stack.  The volume item contains
     * an 'action' parameter which determines whether this volume
     * is a positive or negative space.
     * The item also contains a `locationRelativity` which
     * determines whether it is relative or absolute to the
     * compound volume origin
     *
     * @worldMutation
     *
     * @param item
     * Item to push to the end of the stack
     */
    pushVolume(item: CompoundBlockVolumeItem): void;
    /**
     * @remarks
     * If the volume stack is empty, this function will push the
     * specified item to the stack.
     * If the volume stack is NOT empty, this function will replace
     * the last item on the stack with the new item.
     *
     * @worldMutation
     *
     * @param item
     * Item to add or replace
     */
    replaceOrAddLastVolume(item: CompoundBlockVolumeItem): boolean;
    /**
     * @remarks
     * Set the origin of the compound volume to an absolute world
     * space location
     *
     * @worldMutation
     *
     * @param preserveExistingVolumes
     * This optional boolean flag determines whether the relative
     * `CompoundBlockVolumeItem`'s are frozen in place, or are
     * affected by the new origin.
     * Imagine a scenario where you have a series of relative
     * locations around an origin which make up a sphere; all of
     * these locations are in the range of -2 to 2.
     * Push each of these locations to the compound volume as
     * relative items.
     * Now, move the origin and all of the locations representing
     * the sphere move accordingly.
     * However, let's say you want to add a 2nd sphere next to the
     * 1st.
     * In this case, set the new origin a few locations over, but
     * 'preserveExistingVolumes' = true.
     * This will set a new origin, but the existing sphere
     * locations will remain relative to the original origin.
     * Now, you can push the relative sphere locations again (this
     * time they will be relative to the new origin) - resulting in
     * 2 spheres next to each other.
     */
    setOrigin(position: Vector3, preserveExistingVolumes?: boolean): void;
    /**
     * @remarks
     * Similar to {@link CompoundBlockVolume.setOrigin} - this
     * function will translate the origin by a given delta to a new
     * position
     *
     * @worldMutation
     *
     * @param preserveExistingVolumes
     * See the description for the arguments to {@link
     * CompoundBlockVolume.setOrigin}
     */
    translateOrigin(delta: Vector3, preserveExistingVolumes?: boolean): void;
}

/**
 * Represents a container that can hold sets of items. Used
 * with entities such as Players, Chest Minecarts, Llamas, and
 * more.
 * @seeExample containers.ts
 */
export class Container {
    private constructor();
    /**
     * @remarks
     * If these rules are defined other container operations will
     * throw if they cause these rules to be invalidated. For
     * example, adding a shulker box to a vanilla bundle.
     *
     */
    readonly containerRules?: ContainerRules;
    /**
     * @remarks
     * Count of the slots in the container that are empty.
     *
     * @throws
     * Throws if the container is invalid.
     */
    readonly emptySlotsCount: number;
    /**
     * @remarks
     * Returns whether a container object (or the entity or block
     * that this container is associated with) is still available
     * for use in this context.
     *
     */
    readonly isValid: boolean;
    /**
     * @remarks
     * The number of slots in this container. For example, a
     * standard single-block chest has a size of 27. Note, a
     * player's inventory container contains a total of 36 slots, 9
     * hotbar slots plus 27 inventory slots.
     *
     * @throws
     * Throws if the container is invalid.
     */
    readonly size: number;
    /**
     * @remarks
     * The combined weight of all items in the container.
     *
     * @throws This property can throw when used.
     *
     * {@link InvalidContainerError}
     */
    readonly weight: number;
    /**
     * @remarks
     * Adds an item to the container. The item is placed in the
     * first available slot(s) and can be stacked with existing
     * items of the same type. Note, use {@link Container.setItem}
     * if you wish to set the item in a particular slot.
     *
     * @worldMutation
     *
     * @param itemStack
     * The stack of items to add.
     * @throws
     * Won't throw {@link ContainerRules} error for over weight
     * limit but will instead add items up to the weight limit.
     *
     * {@link ContainerRulesError}
     *
     * {@link Error}
     */
    addItem(itemStack: ItemStack): ItemStack | undefined;
    /**
     * @remarks
     * Clears all inventory items in the container.
     *
     * @worldMutation
     *
     * @throws
     * Throws if the container is invalid.
     */
    clearAll(): void;
    /**
     * @remarks
     * Attempts to find an item inside the container
     *
     * @param itemStack
     * The item to find.
     * @throws This function can throw errors.
     *
     * {@link InvalidContainerError}
     */
    contains(itemStack: ItemStack): boolean;
    /**
     * @remarks
     * Find the index of the first instance of an item inside the
     * container
     *
     * @param itemStack
     * The item to find.
     * @throws This function can throw errors.
     *
     * {@link InvalidContainerError}
     */
    find(itemStack: ItemStack): number | undefined;
    /**
     * @remarks
     * Find the index of the last instance of an item inside the
     * container
     *
     * @param itemStack
     * The item to find.
     * @throws This function can throw errors.
     *
     * {@link InvalidContainerError}
     */
    findLast(itemStack: ItemStack): number | undefined;
    /**
     * @remarks
     * Finds the index of the first empty slot inside the container
     *
     * @throws This function can throw errors.
     *
     * {@link InvalidContainerError}
     */
    firstEmptySlot(): number | undefined;
    /**
     * @remarks
     * Finds the index of the first item inside the container
     *
     * @throws This function can throw errors.
     *
     * {@link InvalidContainerError}
     */
    firstItem(): number | undefined;
    /**
     * @remarks
     * Gets an {@link ItemStack} of the item at the specified slot.
     * If the slot is empty, returns `undefined`. This method does
     * not change or clear the contents of the specified slot. To
     * get a reference to a particular slot, see {@link
     * Container.getSlot}.
     *
     * @param slot
     * Zero-based index of the slot to retrieve items from.
     * Minimum value: 0
     * @throws
     * Throws if the container is invalid or if the `slot` index is
     * out of bounds.
     * @seeExample getFirstHotbarItem.ts
     */
    getItem(slot: number): ItemStack | undefined;
    /**
     * @remarks
     * Returns a container slot. This acts as a reference to a slot
     * at the given index for this container.
     *
     * @param slot
     * The index of the slot to return. This index must be within
     * the bounds of the container.
     * Minimum value: 0
     * @throws
     * Throws if the container is invalid or if the `slot` index is
     * out of bounds.
     */
    getSlot(slot: number): ContainerSlot;
    /**
     * @remarks
     * Moves an item from one slot to another, potentially across
     * containers.
     *
     * @worldMutation
     *
     * @param fromSlot
     * Zero-based index of the slot to transfer an item from, on
     * this container.
     * Minimum value: 0
     * @param toSlot
     * Zero-based index of the slot to transfer an item to, on
     * `toContainer`.
     * Minimum value: 0
     * @param toContainer
     * Target container to transfer to. Note this can be the same
     * container as the source.
     * @throws
     * Throws if either this container or `toContainer` are invalid
     * or if the `fromSlot` or `toSlot` indices out of bounds.
     *
     * {@link ContainerRulesError}
     *
     * {@link Error}
     * @seeExample moveBetweenContainers.ts
     */
    moveItem(fromSlot: number, toSlot: number, toContainer: Container): void;
    /**
     * @remarks
     * Sets an item stack within a particular slot.
     *
     * @worldMutation
     *
     * @param slot
     * Zero-based index of the slot to set an item at.
     * Minimum value: 0
     * @param itemStack
     * Stack of items to place within the specified slot. Setting
     * `itemStack` to undefined will clear the slot.
     * @throws
     * Throws if the container is invalid or if the `slot` index is
     * out of bounds.
     *
     * {@link ContainerRulesError}
     *
     * {@link Error}
     */
    setItem(slot: number, itemStack?: ItemStack): void;
    /**
     * @remarks
     * Swaps items between two different slots within containers.
     *
     * @worldMutation
     *
     * @param slot
     * Zero-based index of the slot to swap from this container.
     * Minimum value: 0
     * @param otherSlot
     * Zero-based index of the slot to swap with.
     * Minimum value: 0
     * @param otherContainer
     * Target container to swap with. Note this can be the same
     * container as this source.
     * @throws
     * Throws if either this container or `otherContainer` are
     * invalid or if the `slot` or `otherSlot` are out of bounds.
     *
     * {@link ContainerRulesError}
     *
     * {@link Error}
     */
    swapItems(slot: number, otherSlot: number, otherContainer: Container): void;
    /**
     * @remarks
     * Moves an item from one slot to another container, or to the
     * first available slot in the same container.
     *
     * @worldMutation
     *
     * @param fromSlot
     * Zero-based index of the slot to transfer an item from, on
     * this container.
     * Minimum value: 0
     * @param toContainer
     * Target container to transfer to. Note this can be the same
     * container as the source.
     * @returns
     * An itemStack with the items that couldn't be transferred.
     * Returns undefined if all items were transferred.
     * @throws
     * Throws if either this container or `toContainer` are invalid
     * or if the `fromSlot` or `toSlot` indices out of bounds.
     * Won't throw {@link ContainerRules} error for over weight
     * limit but will instead add items up to the weight limit.
     *
     * {@link ContainerRulesError}
     *
     * {@link Error}
     * @seeExample transferBetweenContainers.ts
     */
    transferItem(fromSlot: number, toContainer: Container): ItemStack | undefined;
}

/**
 * Represents a slot within a broader container (e.g., entity
 * inventory.)
 */
export class ContainerSlot {
    private constructor();
    /**
     * @remarks
     * Number of the items in the stack. Valid values range between
     * 1-255. The provided value will be clamped to the item's
     * maximum stack size.
     *
     * @worldMutation
     *
     * Bounds: [1, 255]
     * @throws
     * Throws if the value is outside the range of 1-255.
     */
    amount: number;
    /**
     * @remarks
     * Returns whether the item is stackable. An item is considered
     * stackable if the item's maximum stack size is greater than 1
     * and the item does not contain any custom data or properties.
     *
     * @throws
     * Throws if the slot's container is invalid.
     *
     * {@link InvalidContainerSlotError}
     */
    readonly isStackable: boolean;
    /**
     * @remarks
     * Returns whether the ContainerSlot is valid. The container
     * slot is valid if the container exists and is loaded, and the
     * slot index is valid.
     *
     */
    readonly isValid: boolean;
    /**
     * @remarks
     * Gets or sets whether the item is kept on death.
     *
     * @worldMutation
     *
     * @throws
     * Throws if the slot's container is invalid.
     */
    keepOnDeath: boolean;
    /**
     * @remarks
     * Gets or sets the item's lock mode. The default value is
     * `ItemLockMode.none`.
     *
     * @worldMutation
     *
     * @throws
     * Throws if the slot's container is invalid.
     */
    lockMode: ItemLockMode;
    /**
     * @remarks
     * The maximum stack size. This value varies depending on the
     * type of item. For example, torches have a maximum stack size
     * of 64, while eggs have a maximum stack size of 16.
     *
     * @throws
     * Throws if the slot's container is invalid.
     *
     * {@link InvalidContainerSlotError}
     */
    readonly maxAmount: number;
    /**
     * @remarks
     * Given name of this stack of items. The name tag is displayed
     * when hovering over the item. Setting the name tag to an
     * empty string or `undefined` will remove the name tag.
     *
     * @worldMutation
     *
     * @throws
     * Throws if the slot's container is invalid. Also throws if
     * the length exceeds 255 characters.
     */
    nameTag?: string;
    /**
     * @remarks
     * The type of the item.
     *
     * @throws
     * Throws if the slot's container is invalid.
     *
     * {@link EngineError}
     *
     * {@link InvalidContainerSlotError}
     */
    readonly 'type': ItemType;
    /**
     * @remarks
     * Identifier of the type of items for the stack. If a
     * namespace is not specified, 'minecraft:' is assumed.
     * Examples include 'wheat' or 'apple'.
     *
     * @throws
     * Throws if the slot's container is invalid.
     *
     * {@link InvalidContainerSlotError}
     */
    readonly typeId: string;
    /**
     * @remarks
     * Clears all dynamic properties that have been set on this
     * item stack.
     *
     * @throws
     * Throws if the slot's container is invalid.
     *
     * {@link InvalidContainerSlotError}
     */
    clearDynamicProperties(): void;
    /**
     * @remarks
     * Returns whether the item within this container slot can be
     * destroyed.
     *
     * @throws This function can throw errors.
     *
     * {@link InvalidContainerSlotError}
     */
    getCanDestroy(): string[];
    /**
     * @remarks
     * Returns if the item in this container slot can be placed on.
     *
     * @throws This function can throw errors.
     *
     * {@link InvalidContainerSlotError}
     */
    getCanPlaceOn(): string[];
    /**
     * @remarks
     * Returns a property value.
     *
     * @param identifier
     * The property identifier.
     * @returns
     * Returns the value for the property, or undefined if the
     * property has not been set.
     * @throws
     * Throws if the slot's container is invalid.
     *
     * {@link InvalidContainerSlotError}
     */
    getDynamicProperty(identifier: string): boolean | number | string | Vector3 | undefined;
    /**
     * @remarks
     * Returns the available set of dynamic property identifiers
     * that have been used on this item stack.
     *
     * @returns
     * A string array of the dynamic properties set on this entity.
     * @throws
     * Throws if the slot's container is invalid.
     *
     * {@link InvalidContainerSlotError}
     */
    getDynamicPropertyIds(): string[];
    /**
     * @remarks
     * Returns the total size, in bytes, of all the dynamic
     * properties that are currently stored for this entity. This
     * includes the size of both the key and the value.  This can
     * be useful for diagnosing performance warning signs - if, for
     * example, an entity has many megabytes of associated dynamic
     * properties, it may be slow to load on various devices.
     *
     * @throws
     * Throws if the slot's container is invalid.
     *
     * {@link InvalidContainerSlotError}
     */
    getDynamicPropertyTotalByteCount(): number;
    /**
     * @remarks
     * Creates an exact copy of the item stack, including any
     * custom data or properties.
     *
     * @returns
     * Returns a copy of the item in the slot. Returns undefined if
     * the slot is empty.
     * @throws
     * Throws if the slot's container is invalid.
     *
     * {@link InvalidContainerSlotError}
     */
    getItem(): ItemStack | undefined;
    /**
     * @remarks
     * Returns the lore value - a secondary display string - for an
     * ItemStack.
     *
     * @returns
     * An array of lore strings. If the item does not have lore,
     * returns an empty array.
     * @throws
     * Throws if the slot's container is invalid.
     *
     * {@link InvalidContainerSlotError}
     */
    getLore(): string[];
    /**
     * @remarks
     * Returns the lore value - a secondary display string - for an
     * ItemStack. String lore lines will be converted to a {@link
     * RawMessage} and put under {@link RawMessage.text}.
     *
     * @returns
     * An array of lore lines. If the item does not have lore,
     * returns an empty array.
     * @throws This function can throw errors.
     *
     * {@link InvalidContainerSlotError}
     */
    getRawLore(): RawMessage[];
    /**
     * @remarks
     * Returns all tags for the item in the slot.
     *
     * @returns
     * Returns all tags for the item in the slot. Return an empty
     * array if the the slot is empty.
     * @throws
     * Throws if the slot's container is invalid.
     *
     * {@link InvalidContainerSlotError}
     */
    getTags(): string[];
    /**
     * @remarks
     * Returns true if this slot has an item.
     *
     * @throws This function can throw errors.
     *
     * {@link InvalidContainerSlotError}
     */
    hasItem(): boolean;
    /**
     * @remarks
     * Returns whether the item in the slot slot has the given tag.
     *
     * @param tag
     * The item tag.
     * @returns
     * Returns false when the slot is empty or the item in the slot
     * does not have the given tag.
     * @throws
     * Throws if the slot's container is invalid.
     *
     * {@link InvalidContainerSlotError}
     */
    hasTag(tag: string): boolean;
    /**
     * @remarks
     * Returns whether this item stack can be stacked with the
     * given `itemStack`. This is determined by comparing the item
     * type and any custom data and properties associated with the
     * item stacks. The amount of each item stack is not taken into
     * consideration.
     *
     * @param itemStack
     * The ItemStack that is being compared.
     * @returns
     * Returns whether this item stack can be stacked with the
     * given `itemStack`.
     * @throws
     * Throws if the slot's container is invalid.
     *
     * {@link InvalidContainerSlotError}
     */
    isStackableWith(itemStack: ItemStack): boolean;
    /**
     * @remarks
     * The list of block types this item can break in Adventure
     * mode. The block names are displayed in the item's tooltip.
     * Setting the value to undefined will clear the list.
     *
     * @worldMutation
     *
     * @param blockIdentifiers
     * The list of blocks, given by their identifiers.
     * @throws
     * Throws if the slot's container is invalid. Also throws if
     * any of the provided block identifiers are invalid.
     *
     * {@link Error}
     *
     * {@link InvalidContainerSlotError}
     */
    setCanDestroy(blockIdentifiers?: string[]): void;
    /**
     * @remarks
     * The list of block types this item can be placed on in
     * Adventure mode. This is only applicable to block items. The
     * block names are displayed in the item's tooltip. Setting the
     * value to undefined will clear the list.
     *
     * @worldMutation
     *
     * @param blockIdentifiers
     * The list of blocks, given by their identifiers.
     * @throws
     * Throws if the slot's container is invalid. Also throws if
     * any of the provided block identifiers are invalid.
     *
     * {@link Error}
     *
     * {@link InvalidContainerSlotError}
     */
    setCanPlaceOn(blockIdentifiers?: string[]): void;
    /**
     * @remarks
     * Sets multiple dynamic properties with specific values.
     *
     * @param values
     * A Record of key value pairs of the dynamic properties to
     * set. If the data value is null, it will remove that property
     * instead.
     * @throws This function can throw errors.
     *
     * {@link ArgumentOutOfBoundsError}
     *
     * {@link InvalidContainerSlotError}
     *
     * {@link UnsupportedFunctionalityError}
     */
    setDynamicProperties(values: Record<string, boolean | number | string | Vector3 | undefined>): void;
    /**
     * @remarks
     * Sets a specified property to a value.
     *
     * @param identifier
     * The property identifier.
     * @param value
     * Data value of the property to set. If the value is null, it
     * will remove the property instead.
     * @throws
     * Throws if the slot's container is invalid.
     *
     * {@link ArgumentOutOfBoundsError}
     *
     * {@link InvalidContainerSlotError}
     *
     * {@link UnsupportedFunctionalityError}
     */
    setDynamicProperty(identifier: string, value?: boolean | number | string | Vector3): void;
    /**
     * @remarks
     * Sets the given ItemStack in the slot, replacing any existing
     * item.
     *
     * @worldMutation
     *
     * @param itemStack
     * The ItemStack to be placed in the slot.
     * @throws
     * Throws if the slot's container is invalid.
     *
     * {@link ContainerRulesError}
     *
     * {@link InvalidContainerSlotError}
     */
    setItem(itemStack?: ItemStack): void;
    /**
     * @remarks
     * Sets the lore value - a secondary display string - for an
     * ItemStack.
     *
     * @worldMutation
     *
     * @param loreList
     * A list of lore strings. Setting this argument to undefined
     * will clear the lore.
     * @throws
     * Throws if the slot's container is invalid.
     *
     * {@link ArgumentOutOfBoundsError}
     *
     * {@link Error}
     *
     * {@link InvalidContainerSlotError}
     */
    setLore(loreList?: (RawMessage | string)[]): void;
}

/**
 * Details about the origins of the command.
 */
export class CustomCommandOrigin {
    private constructor();
    /**
     * @remarks
     * If this command was initiated via an NPC, returns the entity
     * that initiated the NPC dialogue.
     *
     */
    readonly initiator?: Entity;
    /**
     * @remarks
     * Source block if this command was triggered via a block
     * (e.g., a commandblock.)
     *
     */
    readonly sourceBlock?: Block;
    /**
     * @remarks
     * Source entity if this command was triggered by an entity
     * (e.g., a NPC).
     *
     */
    readonly sourceEntity?: Entity;
    /**
     * @remarks
     * Returns the type of source that fired this command.
     *
     */
    readonly sourceType: CustomCommandSource;
}

/**
 * Provides the functionality for registering custom commands.
 */
export class CustomCommandRegistry {
    private constructor();
    /**
     * @remarks
     * Registers a custom command that when executed triggers a
     * script callback.
     *
     * @worldMutation
     *
     * @earlyExecution
     *
     * @param callback
     * The callback triggered when the command executes.
     * @throws This function can throw errors.
     *
     * {@link CustomCommandError}
     *
     * {@link EngineError}
     *
     * {@link NamespaceNameError}
     */
    registerCommand(
        customCommand: CustomCommand,
        callback: (origin: CustomCommandOrigin, ...args: any[]) => CustomCommandResult | undefined,
    ): void;
    /**
     * @remarks
     * Registers a custom command enum.
     *
     * @worldMutation
     *
     * @earlyExecution
     *
     * @throws This function can throw errors.
     *
     * {@link CustomCommandError}
     *
     * {@link EngineError}
     *
     * {@link NamespaceNameError}
     */
    registerEnum(name: string, values: string[]): void;
}

/**
 * Contains the custom component's JSON parameters
 */
export class CustomComponentParameters {
    private constructor();
    /**
     * @remarks
     * JSON object containing the parameters from the custom
     * component definition
     *
     */
    readonly params: unknown;
}

/**
 * Loot item condition that checks whether the loot source was
 * damaged by a specific type of entity.
 */
// @ts-ignore Class inheritance allowed for native defined classes
export class DamagedByEntityCondition extends LootItemCondition {
    private constructor();
    /**
     * @remarks
     * The entity type required for this condition to pass.
     *
     */
    readonly entityType: string;
}

/**
 * Contains information related to firing of a data driven
 * entity event - for example, the minecraft:ageable_grow_up
 * event on a chicken.
 */
export class DataDrivenEntityTriggerAfterEvent {
    private constructor();
    /**
     * @remarks
     * Entity that the event triggered on.
     *
     */
    readonly entity: Entity;
    /**
     * @remarks
     * Name of the data driven event being triggered.
     *
     */
    readonly eventId: string;
    /**
     * @remarks
     * An updateable list of modifications to component state that
     * are the effect of this triggered event.
     *
     */
    getModifiers(): DefinitionModifier[];
}

/**
 * Contains event registration related to firing of a data
 * driven entity event - for example, the
 * minecraft:ageable_grow_up event on a chicken.
 */
export class DataDrivenEntityTriggerAfterEventSignal {
    private constructor();
    /**
     * @remarks
     * Adds a callback that will be called after a data driven
     * entity event is triggered.
     *
     * @worldMutation
     *
     * @earlyExecution
     *
     */
    subscribe(
        callback: (arg0: DataDrivenEntityTriggerAfterEvent) => void,
        options?: EntityDataDrivenTriggerEventOptions,
    ): (arg0: DataDrivenEntityTriggerAfterEvent) => void;
    /**
     * @remarks
     * Removes a callback that will be called after a data driven
     * entity event is triggered.
     *
     * @worldMutation
     *
     * @earlyExecution
     *
     */
    unsubscribe(callback: (arg0: DataDrivenEntityTriggerAfterEvent) => void): void;
}

/**
 * A class that represents a particular dimension (e.g., The
 * End) within a world.
 */
export class Dimension {
    private constructor();
    /**
     * @remarks
     * Height range of the dimension.
     *
     * @throws This property can throw when used.
     */
    readonly heightRange: NumberRange;
    /**
     * @remarks
     * Identifier of the dimension.
     *
     */
    readonly id: string;
    /**
     * @remarks
     * Key for the localization of a dimension's name used by
     * language files.
     *
     */
    readonly localizationKey: string;
    /**
     * @rc
     * @remarks
     * Calculates the location of the closest biome of a particular
     * type from the world seed. Note that
     * calculateClosestBiomeFromSeed can be an expensive operation,
     * so avoid using many of these calls within a particular tick.
     * The result is derived purely from the world generation
     * algorithm and the world seed, so the returned location may
     * not reflect the actual current terrain if biomes have been
     * modified after generation.
     *
     * @param pos
     * Starting location to look for a biome to find.
     * @param biomeToFind
     * Identifier of the biome to look for.
     * @param options
     * Additional selection criteria for a biome search.
     * @returns
     * Returns a location of the biome, or undefined if a biome
     * could not be found.
     * @throws This function can throw errors.
     *
     * {@link EngineError}
     *
     * {@link Error}
     */
    calculateClosestBiomeFromSeed(
        pos: Vector3,
        biomeToFind: BiomeType | string,
        options?: BiomeSearchOptions,
    ): Vector3 | undefined;
    /**
     * @remarks
     * Checks if an area contains the specified biomes. If the area
     * is partially inside world boundaries, only the area that is
     * in bounds will be searched. This operation takes longer
     * proportional to both the area of the volume and the number
     * of biomes to check.
     *
     * @param volume
     * Area to check biomes in.
     * @param biomeFilter
     * A list of biomes to include and exclude. A list of tags to
     * include and exclude. Will return false if a biome is found
     * in the area that is in the excluded list or contains any of
     * the excluded tags.
     * @param isSuperset
     * Superset is used to determine the strictness of the filter.
     * If superset is set to true then the area must contain one or
     * more biomes in the included list or that contains all of the
     * included tags. If superset is set to false then the area
     * must contain only biomes in the included list and that
     * contain all of the included tags
     * @returns
     * Returns true if the biomes in the area match the filter
     * settings passed in. Otherwise, returns false.
     * @throws
     * An error will be thrown if the area provided includes
     * unloaded chunks.
     * An error will be thrown if the area provided is completely
     * outside the world boundaries.
     * An error will be thrown if an unknown biome name is
     * provided.
     *
     * {@link EngineError}
     *
     * {@link InvalidArgumentError}
     *
     * {@link LocationOutOfWorldBoundariesError}
     *
     * {@link UnloadedChunksError}
     */
    containsBiomes(volume: BlockVolumeBase, biomeFilter: BiomeFilter, isSuperset: boolean): boolean;
    /**
     * @remarks
     * Searches the block volume for a block that satisfies the
     * block filter.
     *
     * @param volume
     * Volume of blocks that will be checked.
     * @param filter
     * Block filter that will be checked against each block in the
     * volume.
     * @param allowUnloadedChunks
     * If set to true will suppress the UnloadedChunksError if some
     * or all of the block volume is outside of the loaded chunks.
     * Will only check the block locations that are within the
     * loaded chunks in the volume.
     * Defaults to: false
     * @returns
     * Returns true if at least one block in the volume satisfies
     * the filter, false otherwise.
     * @throws This function can throw errors.
     *
     * {@link Error}
     *
     * {@link UnloadedChunksError}
     */
    containsBlock(volume: BlockVolumeBase, filter: BlockFilter, allowUnloadedChunks?: boolean): boolean;
    /**
     * @remarks
     * Creates an explosion at the specified location.
     *
     * @worldMutation
     *
     * @param location
     * The location of the explosion.
     * @param radius
     * Radius, in blocks, of the explosion to create.
     * Bounds: [0, 1000]
     * @param explosionOptions
     * Additional configurable options for the explosion.
     * @throws This function can throw errors.
     *
     * {@link LocationInUnloadedChunkError}
     *
     * {@link LocationOutOfWorldBoundariesError}
     * @seeExample createExplosion.ts
     * @seeExample createNoBlockExplosion.ts
     * @seeExample createExplosions.ts
     */
    createExplosion(location: Vector3, radius: number, explosionOptions?: ExplosionOptions): boolean;
    /**
     * @remarks
     * Fills an area of blocks with a specific block type.
     *
     * @worldMutation
     *
     * @param volume
     * Volume of blocks to be filled.
     * @param block
     * Type of block to fill the volume with.
     * @param options
     * A set of additional options, such as a block filter which
     * can be used to include / exclude specific blocks in the
     * fill.
     * @returns
     * Returns a ListBlockVolume which contains all the blocks that
     * were placed.
     * @throws This function can throw errors.
     *
     * {@link EngineError}
     *
     * {@link Error}
     *
     * {@link UnloadedChunksError}
     */
    fillBlocks(
        volume: BlockVolumeBase,
        block: BlockPermutation | BlockType | string,
        options?: BlockFillOptions,
    ): ListBlockVolume;
    /**
     * @remarks
     * Returns the biome type at the specified location.
     *
     * @param location
     * Location at which to check the biome.
     * @throws
     * An error will be thrown if the location is out of world
     * bounds.
     * An error will be thrown if the location is in an unloaded
     * chunk.
     *
     * {@link LocationInUnloadedChunkError}
     *
     * {@link LocationOutOfWorldBoundariesError}
     */
    getBiome(location: Vector3): BiomeType;
    /**
     * @remarks
     * Returns a block instance at the given location.
     *
     * @param location
     * The location at which to return a block.
     * @returns
     * Block at the specified location, or 'undefined' if asking
     * for a block at an unloaded chunk.
     * @throws
     * PositionInUnloadedChunkError: Exception thrown when trying
     * to interact with a Block object that isn't in a loaded and
     * ticking chunk anymore
     *
     * PositionOutOfWorldBoundariesError: Exception thrown when
     * trying to interact with a position outside of dimension
     * height range
     *
     *
     * {@link LocationInUnloadedChunkError}
     *
     * {@link LocationOutOfWorldBoundariesError}
     */
    getBlock(location: Vector3): Block | undefined;
    /**
     * @remarks
     * Gets the first block found above a given block location
     * based on the given options (by default will find the first
     * solid block above).
     *
     * @param location
     * Location to retrieve the block above from.
     * @param options
     * The options to decide if a block is a valid result.
     * @throws This function can throw errors.
     */
    getBlockAbove(location: Vector3, options?: BlockRaycastOptions): Block | undefined;
    /**
     * @remarks
     * Gets the first block found below a given block location
     * based on the given options (by default will find the first
     * solid block below).
     *
     * @param location
     * Location to retrieve the block below from.
     * @param options
     * The options to decide if a block is a valid result.
     * @throws This function can throw errors.
     */
    getBlockBelow(location: Vector3, options?: BlockRaycastOptions): Block | undefined;
    /**
     * @remarks
     * Gets the first block that intersects with a vector emanating
     * from a location.
     *
     * @param location
     * Location from where to initiate the ray check.
     * @param direction
     * Vector direction to cast the ray.
     * @param options
     * Additional options for processing this raycast query.
     * @throws This function can throw errors.
     */
    getBlockFromRay(location: Vector3, direction: Vector3, options?: BlockRaycastOptions): BlockRaycastHit | undefined;
    /**
     * @remarks
     * Gets all the blocks in a volume that satisfy the block query
     * options.
     *
     * @param volume
     * Volume of blocks that will be checked.
     * @param options
     * Block query options including filter criteria and optional
     * closest/farthest distance sorting from a location.
     * @param allowUnloadedChunks
     * If set to true will suppress the UnloadedChunksError if some
     * or all of the block volume is outside of the loaded chunks.
     * Will only check the block locations that are within the
     * loaded chunks in the volume.
     * Defaults to: false
     * @returns
     * Returns the ListBlockVolume that contains all the block
     * locations that satisfied the block query options.
     * @throws This function can throw errors.
     *
     * {@link ArgumentOutOfBoundsError}
     *
     * {@link Error}
     *
     * {@link InvalidArgumentError}
     *
     * {@link UnloadedChunksError}
     */
    getBlocks(volume: BlockVolumeBase, options: BlockQueryOptions, allowUnloadedChunks?: boolean): ListBlockVolume;
    /**
     * @remarks
     * Returns a set of entities based on a set of conditions
     * defined via the EntityQueryOptions set of filter criteria.
     *
     * @param options
     * Additional options that can be used to filter the set of
     * entities returned.
     * @returns
     * An entity array.
     * @throws This function can throw errors.
     *
     * {@link CommandError}
     *
     * {@link InvalidArgumentError}
     * @seeExample bounceSkeletons.ts
     * @seeExample tagsQuery.ts
     * @seeExample testThatEntityIsFeatherItem.ts
     */
    getEntities(options?: EntityQueryOptions): Entity[];
    /**
     * @remarks
     * Returns a set of entities at a particular location.
     *
     * @param location
     * The location at which to return entities.
     * @returns
     * Zero or more entities at the specified location.
     */
    getEntitiesAtBlockLocation(location: Vector3): Entity[];
    /**
     * @remarks
     * Gets entities that intersect with a specified vector
     * emanating from a location.
     *
     * @param options
     * Additional options for processing this raycast query.
     * @throws This function can throw errors.
     *
     * {@link EngineError}
     *
     * {@link InvalidArgumentError}
     *
     * {@link InvalidEntityError}
     *
     * {@link UnsupportedFunctionalityError}
     */
    getEntitiesFromRay(location: Vector3, direction: Vector3, options?: EntityRaycastOptions): EntityRaycastHit[];
    /**
     * @beta
     * @remarks
     * Returns a vector of generated structures that contain the
     * specified location (ex: Pillager Outpost, Mineshaft, etc.).
     * The vector will be empty if no structures are found.
     *
     * @param location
     * Location at which to check for structures.
     * @throws
     * An error will be thrown if the location is out of world
     * bounds.
     * An error will be thrown if the location is in an unloaded
     * chunk.
     *
     * {@link LocationInUnloadedChunkError}
     *
     * {@link LocationOutOfWorldBoundariesError}
     */
    getGeneratedStructures(location: Vector3): (MinecraftFeatureTypes | string)[];
    /**
     * @remarks
     * Returns the total brightness level of light shining on a
     * certain block position.
     *
     * @param location
     * Location of the block we want to check the brightness of.
     * @returns
     * The brightness level on the block.
     * @throws This function can throw errors.
     *
     * {@link InvalidArgumentError}
     *
     * {@link LocationInUnloadedChunkError}
     */
    getLightLevel(location: Vector3): number;
    /**
     * @remarks
     * Returns a set of players based on a set of conditions
     * defined via the EntityQueryOptions set of filter criteria.
     *
     * @param options
     * Additional options that can be used to filter the set of
     * players returned.
     * @returns
     * A player array.
     * @throws This function can throw errors.
     *
     * {@link CommandError}
     *
     * {@link InvalidArgumentError}
     */
    getPlayers(options?: EntityQueryOptions): Player[];
    /**
     * @remarks
     * Returns the brightness level of light shining from the sky
     * on a certain block position.
     *
     * @param location
     * Position of the block we want to check the brightness of.
     * @returns
     * The brightness level on the block.
     * @throws This function can throw errors.
     *
     * {@link InvalidArgumentError}
     *
     * {@link LocationInUnloadedChunkError}
     */
    getSkyLightLevel(location: Vector3): number;
    /**
     * @remarks
     * Returns the highest block at the given XZ location.
     *
     * @param locationXZ
     * Location to retrieve the topmost block for.
     * @param minHeight
     * The Y height to begin the search from. Defaults to the
     * maximum dimension height.
     * @throws This function can throw errors.
     */
    getTopmostBlock(locationXZ: VectorXZ, minHeight?: number): Block | undefined;
    /**
     * @beta
     * @remarks
     * Returns the current weather.
     *
     * @returns
     * Returns a WeatherType that explains the broad category of
     * weather that is currently going on.
     */
    getWeather(): WeatherType;
    /**
     * @remarks
     * Returns true if the chunk at the given location is loaded
     * (and valid for use with scripting).
     *
     * @param location
     * Location to check if the chunk is loaded.
     */
    isChunkLoaded(location: Vector3): boolean;
    /**
     * @remarks
     * Places the given feature into the dimension at the specified
     * location.
     *
     * @worldMutation
     *
     * @param featureName
     * The string identifier for the feature.
     * @param location
     * Location to place the feature.
     * @param shouldThrow
     * Specifies if the function call will throw an error if the
     * feature could not be placed.
     * Note: The function call will always throw an error if using
     * an unknown feature name or trying to place in a unloaded
     * chunk.
     * Defaults to: false
     * @throws
     * An error will be thrown if the feature name is invalid.
     * An error will be thrown if the location is in an unloaded
     * chunk.
     *
     * {@link Error}
     *
     * {@link InvalidArgumentError}
     *
     * {@link LocationInUnloadedChunkError}
     */
    placeFeature(featureName: string, location: Vector3, shouldThrow?: boolean): boolean;
    /**
     * @remarks
     * Places the given feature rule into the dimension at the
     * specified location.
     *
     * @worldMutation
     *
     * @param featureRuleName
     * The string identifier for the feature rule.
     * @param location
     * Location to place the feature rule.
     * @throws
     * An error will be thrown if the feature rule name is invalid.
     * An error will be thrown if the location is in an unloaded
     * chunk.
     *
     * {@link InvalidArgumentError}
     *
     * {@link LocationInUnloadedChunkError}
     */
    placeFeatureRule(featureRuleName: string, location: Vector3): boolean;
    /**
     * @remarks
     * Plays a sound for all players.
     *
     * @worldMutation
     *
     * @param soundId
     * Identifier of the sound.
     * @param location
     * Location of the sound.
     * @param soundOptions
     * Additional options for configuring additional effects for
     * the sound.
     * @throws
     * An error will be thrown if volume is less than 0.0.
     * An error will be thrown if fade is less than 0.0.
     * An error will be thrown if pitch is less than 0.01.
     * An error will be thrown if volume is less than 0.0.
     *
     * {@link EngineError}
     *
     * {@link PropertyOutOfBoundsError}
     */
    playSound(soundId: string, location: Vector3, soundOptions?: WorldSoundOptions): SoundInstance;
    /**
     * @remarks
     * Runs a command synchronously using the context of the
     * broader dimenion.
     *
     * @worldMutation
     *
     * @param commandString
     * Command to run. Note that command strings should not start
     * with slash.
     * @returns
     * Returns a command result with a count of successful values
     * from the command.
     * @throws
     * Throws an exception if the command fails due to incorrect
     * parameters or command syntax, or in erroneous cases for the
     * command. Note that in many cases, if the command does not
     * operate (e.g., a target selector found no matches), this
     * method will not throw an exception.
     *
     * {@link CommandError}
     */
    runCommand(commandString: string): CommandResult;
    /**
     * @remarks
     * Sets a block in the world using a BlockPermutation.
     * BlockPermutations are blocks with a particular state.
     *
     * @worldMutation
     *
     * @param location
     * The location within the dimension to set the block.
     * @param permutation
     * The block permutation to set.
     * @throws
     * Throws if the location is within an unloaded chunk or
     * outside of the world bounds.
     *
     * {@link LocationInUnloadedChunkError}
     *
     * {@link LocationOutOfWorldBoundariesError}
     */
    setBlockPermutation(location: Vector3, permutation: BlockPermutation): void;
    /**
     * @remarks
     * Sets a block at a given location within the dimension.
     *
     * @worldMutation
     *
     * @param location
     * The location within the dimension to set the block.
     * @param blockType
     * The type of block to set. This can be either a string
     * identifier or a BlockType. The default block permutation is
     * used.
     * @throws
     * Throws if the location is within an unloaded chunk or
     * outside of the world bounds.
     *
     * {@link Error}
     *
     * {@link LocationInUnloadedChunkError}
     *
     * {@link LocationOutOfWorldBoundariesError}
     */
    setBlockType(location: Vector3, blockType: BlockType | string): void;
    /**
     * @remarks
     * Sets the current weather within the dimension
     *
     * @worldMutation
     *
     * @param weatherType
     * Set the type of weather to apply.
     * @param duration
     * Sets the duration of the weather (in ticks). If no duration
     * is provided, the duration will be set to a random duration
     * between 300 and 900 seconds.
     * Bounds: [1, 1000000]
     * @throws This function can throw errors.
     */
    setWeather(weatherType: WeatherType, duration?: number): void;
    /**
     * @remarks
     * Creates a new entity (e.g., a mob) at the specified
     * location.
     *
     * @worldMutation
     *
     * @param identifier
     * Identifier of the type of entity to spawn. If no namespace
     * is specified, 'minecraft:' is assumed.
     * @param location
     * The location at which to create the entity.
     * @returns
     * Newly created entity at the specified location.
     * @throws This function can throw errors.
     *
     * {@link EntitySpawnError}
     *
     * {@link InvalidArgumentError}
     *
     * {@link InvalidEntityError}
     *
     * {@link LocationInUnloadedChunkError}
     *
     * {@link LocationOutOfWorldBoundariesError}
     * @seeExample spawnAdultHorse.ts
     * @seeExample quickFoxLazyDog.ts
     * @seeExample triggerEvent.ts d45f49d2
     */
    spawnEntity<T = never>(
        identifier: EntityIdentifierType<NoInfer<T>>,
        location: Vector3,
        options?: SpawnEntityOptions,
    ): Entity;
    /**
     * @remarks
     * Creates a new item stack as an entity at the specified
     * location.
     *
     * @worldMutation
     *
     * @param location
     * The location at which to create the item stack.
     * @returns
     * Newly created item stack entity at the specified location.
     * @throws This function can throw errors.
     *
     * {@link LocationInUnloadedChunkError}
     *
     * {@link LocationOutOfWorldBoundariesError}
     * @seeExample itemStacks.ts
     * @seeExample spawnFeatherItem.ts
     */
    spawnItem(itemStack: ItemStack, location: Vector3): Entity;
    /**
     * @remarks
     * Creates a new particle emitter at a specified location in
     * the world.
     *
     * @worldMutation
     *
     * @param effectName
     * Identifier of the particle to create.
     * @param location
     * The location at which to create the particle emitter.
     * @param molangVariables
     * A set of optional, customizable variables that can be
     * adjusted for this particle.
     * @throws This function can throw errors.
     *
     * {@link LocationInUnloadedChunkError}
     *
     * {@link LocationOutOfWorldBoundariesError}
     * @seeExample spawnParticle.ts bba750fb
     */
    spawnParticle(effectName: string, location: Vector3, molangVariables?: MolangVariableMap): void;
    /**
     * @beta
     * @remarks
     * Spawns an experience orb at a specified location in the
     * dimension.
     *
     * @worldMutation
     *
     * @param location
     * The location at which to spawn the experience orb.
     * @param amount
     * The amount of experience to give the experience orb.
     * Bounds: [1, 12000]
     * @throws This function can throw errors.
     *
     * {@link LocationInUnloadedChunkError}
     *
     * {@link LocationOutOfWorldBoundariesError}
     */
    spawnXp(location: Vector3, amount: number): void;
    /**
     * @beta
     * @remarks
     * Stops all sounds from playing for all players.
     *
     * @worldMutation
     *
     */
    stopAllSounds(): void;
    /**
     * @beta
     * @remarks
     * Stops a sound from playing for all players.
     *
     * @worldMutation
     *
     * @param soundId
     * Identifier of the sound.
     */
    stopSound(soundId: string): void;
}

/**
 * Provides the functionality for registering custom
 * dimensions. Custom dimensions can only be registered during
 * the system startup event.
 */
export class DimensionRegistry {
    private constructor();
    /**
     * @remarks
     * Registers a new custom dimension type. Must be called during
     * the system startup event. The dimension will be created
     * using the void generator.
     *
     * @earlyExecution
     *
     * @param typeId
     * The namespaced identifier for the custom dimension (e.g.,
     * 'mypack:my_dimension'). Must include a namespace and use
     * only valid identifier characters.
     * @throws This function can throw errors.
     *
     * {@link CustomDimensionAlreadyRegisteredError}
     *
     * {@link CustomDimensionInvalidRegistryError}
     *
     * {@link CustomDimensionNameError}
     *
     * {@link CustomDimensionReloadNewDimensionError}
     *
     * {@link EngineError}
     *
     * {@link NamespaceNameError}
     */
    registerCustomDimension(typeId: string): void;
}

/**
 * Represents a type of dimension. Currently only works with
 * Vanilla dimensions.
 */
export class DimensionType {
    private constructor();
    /**
     * @remarks
     * Identifier of the dimension type. Currently only works with
     * Vanilla dimensions.
     *
     */
    readonly typeId: string;
}

/**
 * Used for accessing all available dimension types. Currently
 * only works with Vanilla dimensions.
 */
export class DimensionTypes {
    private constructor();
    /**
     * @remarks
     * Retrieves a dimension type using a string-based identifier.
     * Currently only works with Vanilla dimensions.
     *
     * @earlyExecution
     *
     */
    static get(dimensionTypeId: string): DimensionType | undefined;
    /**
     * @remarks
     * Retrieves an array of all dimension types. Currently only
     * works with Vanilla dimensions.
     *
     * @earlyExecution
     *
     */
    static getAll(): DimensionType[];
}

/**
 * Represents an effect - like poison - that has been added to
 * an Entity.
 */
export class Effect {
    private constructor();
    /**
     * @remarks
     * Gets an amplifier that may have been applied to this effect.
     * Sample values range typically from 0 to 4. Example: The
     * effect 'Jump Boost II' will have an amplifier value of 1.
     *
     * @throws This property can throw when used.
     */
    readonly amplifier: number;
    /**
     * @remarks
     * Gets the player-friendly name of this effect.
     *
     * @throws This property can throw when used.
     */
    readonly displayName: string;
    /**
     * @remarks
     * Gets the entire specified duration, in ticks, of this
     * effect. There are 20 ticks per second. Use {@link
     * TicksPerSecond} constant to convert between ticks and
     * seconds.
     *
     * @throws This property can throw when used.
     */
    readonly duration: number;
    /**
     * @remarks
     * Returns whether an effect instance is available for use in
     * this context.
     *
     */
    readonly isValid: boolean;
    /**
     * @remarks
     * Gets the type id of this effect.
     *
     * @throws This property can throw when used.
     */
    readonly typeId: string;
}

/**
 * Contains information related to changes to an effect - like
 * poison - being added to an entity.
 */
export class EffectAddAfterEvent {
    private constructor();
    /**
     * @remarks
     * Additional properties and details of the effect.
     *
     */
    readonly effect: Effect;
    /**
     * @remarks
     * Entity that the effect is being added to.
     *
     */
    readonly entity: Entity;
}

/**
 * Manages callbacks that are connected to when an effect is
 * added to an entity.
 */
export class EffectAddAfterEventSignal {
    private constructor();
    /**
     * @remarks
     * Adds a callback that will be called when an effect is added
     * to an entity.
     *
     * @worldMutation
     *
     * @earlyExecution
     *
     */
    subscribe(
        callback: (arg0: EffectAddAfterEvent) => void,
        options?: EntityEventOptions,
    ): (arg0: EffectAddAfterEvent) => void;
    /**
     * @remarks
     * Removes a callback from being called when an effect is added
     * to an entity.
     *
     * @worldMutation
     *
     * @earlyExecution
     *
     */
    unsubscribe(callback: (arg0: EffectAddAfterEvent) => void): void;
}

/**
 * Contains information related to changes to an effect - like
 * poison - being added to an entity.
 */
export class EffectAddBeforeEvent {
    private constructor();
    /**
     * @remarks
     * When set to true will cancel the event.
     *
     */
    cancel: boolean;
    /**
     * @remarks
     * Effect duration.
     *
     */
    duration: number;
    /**
     * @remarks
     * The type of the effect that is being added.
     *
     */
    readonly effectType: string;
    /**
     * @remarks
     * Entity that the effect is being added to.
     *
     */
    readonly entity: Entity;
}

/**
 * Manages callbacks that are connected to when an effect is
 * added to an entity.
 */
export class EffectAddBeforeEventSignal {
    private constructor();
    /**
     * @remarks
     * Adds a callback that will be called when an effect is added
     * to an entity.
     *
     * @worldMutation
     *
     * @earlyExecution
     *
     * @param callback
     * This closure is called with restricted-execution privilege.
     * @returns
     * Closure that is called with restricted-execution privilege.
     */
    subscribe(callback: (arg0: EffectAddBeforeEvent) => void): (arg0: EffectAddBeforeEvent) => void;
    /**
     * @remarks
     * Removes a callback from being called when an effect is added
     * to an entity.
     *
     * @worldMutation
     *
     * @earlyExecution
     *
     * @param callback
     * This closure is called with restricted-execution privilege.
     */
    unsubscribe(callback: (arg0: EffectAddBeforeEvent) => void): void;
}

/**
 * Represents a type of effect - like poison - that can be
 * applied to an entity.
 */
export class EffectType {
    private constructor();
    /**
     * @remarks
     * Identifier name of this effect type.
     *
     * @returns
     * Identifier of the effect type.
     */
    getName(): string;
}

/**
 * Represents a type of effect - like poison - that can be
 * applied to an entity.
 */
export class EffectTypes {
    private constructor();
    /**
     * @remarks
     * Effect type for the given identifier.
     *
     * @worldMutation
     *
     * @param identifier
     * The identifier for the effect.
     * @returns
     * Effect type for the given identifier or undefined if the
     * effect does not exist.
     */
    static get(identifier: string): EffectType | undefined;
    /**
     * @remarks
     * Gets all effects.
     *
     * @worldMutation
     *
     * @returns
     * A list of all effects.
     */
    static getAll(): EffectType[];
}

/**
 * Represents a completely empty entry in a loot pool. If this
 * entry is chosen, no items will drop.
 */
// @ts-ignore Class inheritance allowed for native defined classes
export class EmptyLootItem extends LootPoolEntry {
    private constructor();
}

export class EnchantInfo {
    private constructor();
    readonly enchantment: string;
    readonly range: NumberRange;
}

/**
 * Contains information on a type of enchantment.
 */
export class EnchantmentType {
    /**
     * @remarks
     * The name of the enchantment type.
     *
     */
    readonly id: string;
    /**
     * @remarks
     * The maximum level this type of enchantment can have.
     *
     */
    readonly maxLevel: number;
    /**
     * @throws This function can throw errors.
     */
    constructor(enchantmentType: string);
}

/**
 * Contains a catalog of Minecraft Enchantment Types that are
 * available in this world.
 */
export class EnchantmentTypes {
    private constructor();
    /**
     * @remarks
     * Retrieves an enchantment with the specified identifier.
     *
     * @param enchantmentId
     * Identifier of the enchantment.  For example,
     * "minecraft:flame".
     * @returns
     * If available, returns an EnchantmentType object that
     * represents the specified enchantment.
     */
    static get(enchantmentId: string): EnchantmentType | undefined;
    /**
     * @remarks
     * Returns a collection of all available enchantment types.
     *
     */
    static getAll(): EnchantmentType[];
}

/**
 * Loot item function that applies a random enchant to the
 * dropped item using the same algorithm used while enchanting
 * equipment vanilla mobs spawn with.
 */
// @ts-ignore Class inheritance allowed for native defined classes
export class EnchantRandomEquipmentFunction extends LootItemFunction {
    private constructor();
    /**
     * @remarks
     * Value that determines the likelihood of equipment being
     * enchanted.
     *
     */
    readonly chance: number;
}

/**
 * Loot item function that randomly enchants the dropped item.
 */
// @ts-ignore Class inheritance allowed for native defined classes
export class EnchantRandomlyFunction extends LootItemFunction {
    private constructor();
    /**
     * @remarks
     * Determines whether or not treasure enchantments are included
     * in the randomly chosen enchantments.
     *
     */
    readonly treasure: boolean;
}

/**
 * Loot item function that applies a random enchant to the
 * dropped item.
 */
// @ts-ignore Class inheritance allowed for native defined classes
export class EnchantWithLevelsFunction extends LootItemFunction {
    private constructor();
    /**
     * @remarks
     * The value range from which the function randomly chooses the
     * level of enchantment to apply. Contains minimum and maximum
     * values.
     *
     */
    readonly levels: NumberRange;
    /**
     * @remarks
     * Value that determines whether or not treasure enchants
     * should be included in the random enchant selection.
     *
     */
    readonly treasure: boolean;
}

/**
 * Represents the state of an entity (a mob, the player, or
 * other moving objects like minecarts) in the world.
 */
export class Entity {
    private constructor();
    /**
     * @remarks
     * Dimension that the entity is currently within.
     *
     * @throws This property can throw when used.
     *
     * {@link EngineError}
     *
     * {@link InvalidEntityError}
     */
    readonly dimension: Dimension;
    /**
     * @remarks
     * Unique identifier of the entity. This identifier is intended
     * to be consistent across loads of a world instance. No
     * meaning should be inferred from the value and structure of
     * this unique identifier - do not parse or interpret it. This
     * property is accessible even if {@link Entity.isValid} is
     * false.
     *
     */
    readonly id: string;
    /**
     * @remarks
     * Whether the entity is touching a climbable block. For
     * example, a player next to a ladder or a spider next to a
     * stone wall.
     *
     * @throws This property can throw when used.
     *
     * {@link InvalidEntityError}
     */
    readonly isClimbing: boolean;
    /**
     * @remarks
     * Whether the entity has a fall distance greater than 0, or
     * greater than 1 while gliding.
     *
     * @throws This property can throw when used.
     *
     * {@link InvalidEntityError}
     */
    readonly isFalling: boolean;
    /**
     * @remarks
     * Whether any part of the entity is inside a water block.
     *
     * @throws This property can throw when used.
     *
     * {@link InvalidEntityError}
     */
    readonly isInWater: boolean;
    /**
     * @remarks
     * Whether the entity is on top of a solid block. This property
     * may behave in unexpected ways. This property will always be
     * true when an Entity is first spawned, and if the Entity has
     * no gravity this property may be incorrect.
     *
     * @throws This property can throw when used.
     *
     * {@link InvalidEntityError}
     */
    readonly isOnGround: boolean;
    /**
     * @remarks
     * If true, the entity is currently sleeping.
     *
     * @throws This property can throw when used.
     *
     * {@link InvalidEntityError}
     */
    readonly isSleeping: boolean;
    /**
     * @remarks
     * Whether the entity is sneaking - that is, moving more slowly
     * and more quietly.
     *
     * @worldMutation
     *
     */
    isSneaking: boolean;
    /**
     * @remarks
     * Whether the entity is sprinting. For example, a player using
     * the sprint action, an ocelot running away or a pig boosting
     * with Carrot on a Stick.
     *
     * @throws This property can throw when used.
     *
     * {@link InvalidEntityError}
     */
    readonly isSprinting: boolean;
    /**
     * @remarks
     * Whether the entity is in the swimming state. For example, a
     * player using the swim action or a fish in water.
     *
     * @throws This property can throw when used.
     *
     * {@link InvalidEntityError}
     */
    readonly isSwimming: boolean;
    /**
     * @remarks
     * Returns whether the entity can be manipulated by script. A
     * Player is considered valid when it's EntityLifetimeState is
     * set to Loaded.
     *
     */
    readonly isValid: boolean;
    /**
     * @remarks
     * Key for the localization of this entity's name used in .lang
     * files.
     *
     * @throws This property can throw when used.
     *
     * {@link InvalidEntityError}
     */
    readonly localizationKey: string;
    /**
     * @remarks
     * Current location of the entity.
     *
     * @throws This property can throw when used.
     *
     * {@link InvalidEntityError}
     */
    readonly location: Vector3;
    /**
     * @remarks
     * Boolean which determines if the player nameplate should be
     * depth tested for visibility.
     *
     * @worldMutation
     *
     */
    nameplateDepthTested: boolean;
    /**
     * @remarks
     * Float that determines the render distance of this entity's
     * nameplate.
     *
     * @worldMutation
     *
     */
    nameplateRenderDistance: number;
    /**
     * @remarks
     * Given name of the entity.
     *
     * @worldMutation
     *
     */
    nameTag: string;
    /**
     * @remarks
     * Returns a scoreboard identity that represents this entity.
     * Will remain valid when the entity is killed.
     *
     */
    readonly scoreboardIdentity?: ScoreboardIdentity;
    /**
     * @beta
     * @remarks
     * Retrieves or sets an entity that is used as the target of
     * AI-related behaviors, like attacking. If the entity
     * currently has no target returns undefined.
     *
     * @throws This property can throw when used.
     *
     * {@link InvalidEntityError}
     */
    readonly target?: Entity;
    /**
     * @remarks
     * Identifier of the type of the entity - for example,
     * 'minecraft:skeleton'. This property is accessible even if
     * {@link Entity.isValid} is false.
     *
     */
    readonly typeId: string;
    /**
     * @remarks
     * Adds or updates an effect, like poison, to the entity.
     *
     * @worldMutation
     *
     * @param effectType
     * Type of effect to add to the entity.
     * @param duration
     * Amount of time, in ticks, for the effect to apply. There are
     * 20 ticks per second. Use {@link TicksPerSecond} constant to
     * convert between ticks and seconds. The value must be within
     * the range [0, 20000000].
     * Bounds: [1, 20000000]
     * @param options
     * Additional options for the effect.
     * @returns
     * Returns nothing if the effect was added or updated
     * successfully. This can throw an error if the duration or
     * amplifier are outside of the valid ranges, or if the effect
     * does not exist.
     * @throws This function can throw errors.
     *
     * {@link ArgumentOutOfBoundsError}
     *
     * {@link InvalidArgumentError}
     *
     * {@link InvalidEntityError}
     * @seeExample spawnPoisonedVillager.ts
     * @seeExample quickFoxLazyDog.ts
     */
    addEffect(effectType: EffectType | string, duration: number, options?: EntityEffectOptions): Effect | undefined;
    /**
     * @remarks
     * Adds an item to the entity's inventory.
     *
     * @worldMutation
     *
     * @returns
     * Returns undefined if the item was fully added or returns an
     * ItemStack with the remaining count.
     * @throws This function can throw errors.
     *
     * {@link ContainerRulesError}
     *
     * {@link Error}
     *
     * {@link InvalidEntityComponentError}
     *
     * {@link InvalidEntityError}
     */
    addItem(itemStack: ItemStack): ItemStack | undefined;
    /**
     * @remarks
     * Adds a specified tag to an entity.
     *
     * @worldMutation
     *
     * @param tag
     * Content of the tag to add. The tag must be less than 256
     * characters.
     * @returns
     * Returns true if the tag was added successfully. This can
     * fail if the tag already exists on the entity.
     * @throws This function can throw errors.
     *
     * {@link ArgumentOutOfBoundsError}
     *
     * {@link InvalidEntityError}
     * @seeExample tagsQuery.ts
     */
    addTag(tag: string): boolean;
    /**
     * @remarks
     * Applies a set of damage to an entity.
     *
     * @worldMutation
     *
     * @param amount
     * Amount of damage to apply.
     * @param options
     * Additional options about the source of damage, which may add
     * additional effects or spur additional behaviors on this
     * entity.
     * @returns
     * Whether the entity takes any damage. This can return false
     * if the entity is invulnerable or if the damage applied is
     * less than or equal to 0.
     * @throws This function can throw errors.
     *
     * {@link EngineError}
     *
     * {@link InvalidEntityError}
     *
     * {@link UnsupportedFunctionalityError}
     * @seeExample applyDamageThenHeal.ts
     */
    applyDamage(amount: number, options?: EntityApplyDamageByProjectileOptions | EntityApplyDamageOptions): boolean;
    /**
     * @remarks
     * Applies impulse vector to the current velocity of the
     * entity.
     *
     * @worldMutation
     *
     * @param vector
     * Impulse vector.
     * @throws This function can throw errors.
     *
     * {@link ArgumentOutOfBoundsError}
     *
     * {@link InvalidEntityError}
     * @seeExample applyImpulse.ts
     */
    applyImpulse(vector: Vector3): void;
    /**
     * @remarks
     * Applies impulse vector to the current velocity of the
     * entity.
     *
     * @worldMutation
     *
     * @param verticalStrength
     * Knockback strength for the vertical vector.
     * @throws This function can throw errors.
     *
     * {@link InvalidEntityError}
     *
     * {@link UnsupportedFunctionalityError}
     * @seeExample bounceSkeletons.ts
     */
    applyKnockback(horizontalForce: VectorXZ, verticalStrength: number): void;
    /**
     * @remarks
     * Clears all dynamic properties that have been set on this
     * entity.
     *
     * @throws This function can throw errors.
     *
     * {@link InvalidEntityError}
     */
    clearDynamicProperties(): void;
    /**
     * @remarks
     * Sets the current velocity of the Entity to zero.
     *
     * @worldMutation
     *
     * @throws This function can throw errors.
     *
     * {@link InvalidEntityError}
     * @seeExample applyImpulse.ts
     */
    clearVelocity(): void;
    /**
     * @remarks
     * Extinguishes the fire if the entity is on fire. Note that
     * you can call getComponent('minecraft:onfire') and, if
     * present, the entity is on fire.
     *
     * @worldMutation
     *
     * @param useEffects
     * Whether to show any visual effects connected to the
     * extinguishing.
     * Defaults to: true
     * @returns
     * Returns whether the entity was on fire.
     * @throws This function can throw errors.
     *
     * {@link InvalidEntityError}
     * @seeExample setOnFire.ts
     */
    extinguishFire(useEffects?: boolean): boolean;
    /**
     * @remarks
     * Gets the entity's collision bounds.
     *
     * @returns
     * An axis-aligned bounding box.
     * @throws This function can throw errors.
     *
     * {@link InvalidEntityError}
     */
    getAABB(): AABB;
    /**
     * @remarks
     * Gets the solid blocks that this entity is directly standing
     * on. Ignores pressure plates.
     *
     * @param options
     * Additional configuration options for what blocks are
     * returned.
     * @returns
     * The solid blocks that this entity is directly standing on.
     * Returns an empty list if the entity is jumping or flying.
     * @throws This function can throw errors.
     *
     * {@link InvalidEntityError}
     */
    getAllBlocksStandingOn(options?: GetBlocksStandingOnOptions): Block[];
    /**
     * @remarks
     * Returns the first intersecting block from the direction that
     * this entity is looking at.
     *
     * @param options
     * Additional configuration options for the ray cast.
     * @returns
     * Returns the first intersecting block from the direction that
     * this entity is looking at.
     * @throws This function can throw errors.
     *
     * {@link InvalidEntityError}
     */
    getBlockFromViewDirection(options?: BlockRaycastOptions): BlockRaycastHit | undefined;
    /**
     * @remarks
     * Gets a single solid block closest to the center of the
     * entity that this entity is directly standing on. Ignores
     * pressure plates.
     *
     * @param options
     * Additional configuration options for what block is returned.
     * @returns
     * A single solid block closest to the center of the entity
     * that this entity is directly standing on. Undefined if
     * entity is flying or jumping.
     * @throws This function can throw errors.
     *
     * {@link InvalidEntityError}
     */
    getBlockStandingOn(options?: GetBlocksStandingOnOptions): Block | undefined;
    /**
     * @remarks
     * Gets a component (that represents additional capabilities)
     * for an entity.
     *
     * @param componentId
     * The identifier of the component (e.g., 'minecraft:health').
     * If no namespace prefix is specified, 'minecraft:' is
     * assumed. Available component IDs can be found as part of the
     * {@link EntityComponentTypes} enum.
     * @returns
     * Returns the component if it exists on the entity, otherwise
     * undefined.
     * @throws This function can throw errors.
     *
     * {@link InvalidEntityError}
     */
    getComponent<T extends string>(componentId: T): EntityComponentReturnType<T> | undefined;
    /**
     * @remarks
     * Returns all scripting components that are present on this
     * entity.
     *
     * @throws This function can throw errors.
     *
     * {@link InvalidEntityError}
     */
    getComponents(): EntityComponent[];
    /**
     * @remarks
     * Returns a property value.
     *
     * @param identifier
     * The property identifier.
     * @returns
     * Returns the value for the property, or undefined if the
     * property has not been set.
     * @throws This function can throw errors.
     *
     * {@link InvalidEntityError}
     */
    getDynamicProperty(identifier: string): boolean | number | string | Vector3 | undefined;
    /**
     * @remarks
     * Returns the available set of dynamic property identifiers
     * that have been used on this entity.
     *
     * @returns
     * A string array of the dynamic properties set on this entity.
     * @throws This function can throw errors.
     *
     * {@link InvalidEntityError}
     */
    getDynamicPropertyIds(): string[];
    /**
     * @remarks
     * Returns the total size, in bytes, of all the dynamic
     * properties that are currently stored for this entity. This
     * includes the size of both the key and the value.  This can
     * be useful for diagnosing performance warning signs - if, for
     * example, an entity has many megabytes of associated dynamic
     * properties, it may be slow to load on various devices.
     *
     * @throws This function can throw errors.
     *
     * {@link InvalidEntityError}
     */
    getDynamicPropertyTotalByteCount(): number;
    /**
     * @remarks
     * Returns the effect for the specified EffectType on the
     * entity, undefined if the effect is not present, or throws an
     * error if the effect does not exist.
     *
     * @param effectType
     * The effect identifier.
     * @returns
     * Effect object for the specified effect, undefined if the
     * effect is not present, or throws an error if the effect does
     * not exist.
     * @throws This function can throw errors.
     *
     * {@link InvalidArgumentError}
     *
     * {@link InvalidEntityError}
     */
    getEffect(effectType: EffectType | string): Effect | undefined;
    /**
     * @remarks
     * Returns a set of effects applied to this entity.
     *
     * @returns
     * List of effects.
     * @throws This function can throw errors.
     *
     * {@link InvalidEntityError}
     */
    getEffects(): Effect[];
    /**
     * @remarks
     * Gets the entities that this entity is looking at by
     * performing a ray cast from the view of this entity.
     *
     * @param options
     * Additional configuration options for the ray cast.
     * @returns
     * Returns a set of entities from the direction that this
     * entity is looking at.
     * @throws This function can throw errors.
     *
     * {@link EngineError}
     *
     * {@link InvalidArgumentError}
     *
     * {@link InvalidEntityError}
     *
     * {@link UnsupportedFunctionalityError}
     */
    getEntitiesFromViewDirection(options?: EntityRaycastOptions): EntityRaycastHit[];
    /**
     * @remarks
     * Returns the current location of the head component of this
     * entity.
     *
     * @returns
     * Returns the current location of the head component of this
     * entity.
     * @throws This function can throw errors.
     *
     * {@link InvalidEntityError}
     */
    getHeadLocation(): Vector3;
    /**
     * @remarks
     * Gets an entity Property value. If the property was set using
     * the setProperty function within the same tick, the updated
     * value will not be reflected until the subsequent tick.
     *
     * @param identifier
     * The entity Property identifier.
     * @returns
     * Returns the current property value. For enum properties, a
     * string is returned. For float and int properties, a number
     * is returned. For undefined properties, undefined is
     * returned.
     * @throws
     * Throws if the entity is invalid.
     *
     * {@link InvalidEntityError}
     */
    getProperty(identifier: string): boolean | number | string | undefined;
    /**
     * @remarks
     * Returns the current rotation component of this entity.
     *
     * @returns
     * Returns a Vec2 containing the rotation of this entity (in
     * degrees).
     * @throws This function can throw errors.
     *
     * {@link InvalidEntityError}
     */
    getRotation(): Vector2;
    /**
     * @remarks
     * Returns all tags associated with the entity.
     *
     * @returns
     * An array containing all tags as strings.
     * @throws This function can throw errors.
     *
     * {@link InvalidEntityError}
     */
    getTags(): string[];
    /**
     * @remarks
     * Returns the current velocity vector of the entity.
     *
     * @returns
     * Returns the current velocity vector of the entity.
     * @throws This function can throw errors.
     *
     * {@link InvalidEntityError}
     * @seeExample getFireworkVelocity.ts
     */
    getVelocity(): Vector3;
    /**
     * @remarks
     * Returns the current view direction of the entity.
     *
     * @returns
     * Returns the current view direction of the entity.
     * @throws This function can throw errors.
     *
     * {@link InvalidEntityError}
     */
    getViewDirection(): Vector3;
    /**
     * @remarks
     * Returns true if the specified component is present on this
     * entity.
     *
     * @param componentId
     * The identifier of the component (e.g., 'minecraft:rideable')
     * to retrieve. If no namespace prefix is specified,
     * 'minecraft:' is assumed.
     * @returns
     * Returns true if the specified component is present on this
     * entity.
     * @throws This function can throw errors.
     *
     * {@link InvalidEntityError}
     */
    hasComponent(componentId: string): boolean;
    /**
     * @remarks
     * Returns whether an entity has a particular tag.
     *
     * @param tag
     * Identifier of the tag to test for.
     * @returns
     * Returns whether an entity has a particular tag.
     * @throws This function can throw errors.
     *
     * {@link InvalidEntityError}
     */
    hasTag(tag: string): boolean;
    /**
     * @remarks
     * Kills this entity. The entity will drop loot as normal.
     *
     * @worldMutation
     *
     * @returns
     * Returns true if entity can be killed (even if it is already
     * dead), otherwise it returns false.
     * @throws This function can throw errors.
     *
     * {@link InvalidEntityError}
     * @seeExample tagsQuery.ts
     */
    kill(): boolean;
    /**
     * @remarks
     * Sets the rotation of the entity to face a target location.
     * Both pitch and yaw will be set, if applicable, such as for
     * mobs where the pitch controls the head tilt and the yaw
     * controls the body rotation.
     *
     * @worldMutation
     *
     * @param targetLocation
     * The target location that this entity should face/look
     * towards.
     * @throws This function can throw errors.
     *
     * {@link InvalidEntityError}
     *
     * {@link UnsupportedFunctionalityError}
     */
    lookAt(targetLocation: Vector3): void;
    /**
     * @remarks
     * Matches the entity against the passed in options. Uses the
     * location of the entity for matching if the location is not
     * specified in the passed in EntityQueryOptions.
     *
     * @param options
     * The query to perform the match against.
     * @returns
     * Returns true if the entity matches the criteria in the
     * passed in EntityQueryOptions, otherwise it returns false.
     * @throws
     * Throws if the query options are misconfigured.
     *
     * {@link InvalidArgumentError}
     *
     * {@link InvalidEntityError}
     *
     * {@link UnsupportedFunctionalityError}
     */
    matches(options: EntityQueryOptions): boolean;
    /**
     * @remarks
     * Cause the entity to play the given animation.
     *
     * @worldMutation
     *
     * @param animationName
     * The animation identifier. e.g. animation.creeper.swelling
     * @param options
     * Additional options to control the playback and transitions
     * of the animation.
     * @throws This function can throw errors.
     *
     * {@link InvalidEntityError}
     */
    playAnimation(animationName: string, options?: PlayAnimationOptions): void;
    /**
     * @remarks
     * Immediately removes the entity from the world. The removed
     * entity will not perform a death animation or drop loot upon
     * removal.
     *
     * @worldMutation
     *
     * @throws This function can throw errors.
     *
     * {@link InvalidEntityError}
     *
     * {@link UnsupportedFunctionalityError}
     */
    remove(): void;
    /**
     * @remarks
     * Removes the specified EffectType on the entity, or returns
     * false if the effect is not present.
     *
     * @worldMutation
     *
     * @param effectType
     * The effect identifier.
     * @returns
     * Returns true if the effect has been removed. Returns false
     * if the effect is not found or does not exist.
     * @throws This function can throw errors.
     *
     * {@link InvalidArgumentError}
     *
     * {@link InvalidEntityError}
     */
    removeEffect(effectType: EffectType | string): boolean;
    /**
     * @remarks
     * Removes a specified tag from an entity.
     *
     * @worldMutation
     *
     * @param tag
     * Content of the tag to remove.
     * @returns
     * Returns whether the tag existed on the entity.
     * @throws This function can throw errors.
     *
     * {@link InvalidEntityError}
     */
    removeTag(tag: string): boolean;
    /**
     * @remarks
     * Resets an Entity Property back to its default value, as
     * specified in the Entity's definition. This property change
     * is not applied until the next tick.
     *
     * @worldMutation
     *
     * @param identifier
     * The Entity Property identifier.
     * @returns
     * Returns the default property value. For enum properties, a
     * string is returned. For float and int properties, a number
     * is returned. For undefined properties, undefined is
     * returned.
     * @throws
     * Throws if the entity is invalid.
     *
     * {@link EngineError}
     *
     * {@link Error}
     *
     * {@link InvalidEntityError}
     */
    resetProperty(identifier: string): boolean | number | string;
    /**
     * @remarks
     * Runs a synchronous command on the entity.
     *
     * @worldMutation
     *
     * @param commandString
     * The command string. Note: This should not include a leading
     * forward slash.
     * @returns
     * A command result containing whether the command was
     * successful.
     * @throws This function can throw errors.
     *
     * {@link CommandError}
     *
     * {@link InvalidEntityError}
     */
    runCommand(commandString: string): CommandResult;
    /**
     * @remarks
     * Sets multiple dynamic properties with specific values.
     *
     * @param values
     * A Record of key value pairs of the dynamic properties to
     * set. If the data value is null, it will remove that property
     * instead.
     * @throws This function can throw errors.
     *
     * {@link ArgumentOutOfBoundsError}
     *
     * {@link InvalidEntityError}
     */
    setDynamicProperties(values: Record<string, boolean | number | string | Vector3 | undefined>): void;
    /**
     * @remarks
     * Sets a specified property to a value.
     *
     * @param identifier
     * The property identifier.
     * @param value
     * Data value of the property to set. If the value is null, it
     * will remove the property instead.
     * @throws This function can throw errors.
     *
     * {@link ArgumentOutOfBoundsError}
     *
     * {@link InvalidEntityError}
     */
    setDynamicProperty(identifier: string, value?: boolean | number | string | Vector3): void;
    /**
     * @remarks
     * Sets an entity on fire (if it is not in water or rain). Note
     * that you can call getComponent('minecraft:onfire') and, if
     * present, the entity is on fire.
     *
     * @worldMutation
     *
     * @param seconds
     * Length of time to set the entity on fire.
     * @param useEffects
     * Whether side-effects should be applied (e.g. thawing freeze)
     * and other conditions such as rain or fire protection should
     * be taken into consideration.
     * Defaults to: true
     * @returns
     * Whether the entity was set on fire. This can fail if seconds
     * is less than or equal to zero, the entity is wet or the
     * entity is immune to fire.
     * @throws This function can throw errors.
     *
     * {@link InvalidEntityError}
     * @seeExample setOnFire.ts
     */
    setOnFire(seconds: number, useEffects?: boolean): boolean;
    /**
     * @remarks
     * Sets an Entity Property to the provided value. This property
     * change is not applied until the next tick.
     *
     * @worldMutation
     *
     * @param identifier
     * The Entity Property identifier.
     * @param value
     * The property value. The provided type must be compatible
     * with the type specified in the entity's definition.
     * @throws
     * Throws if the entity is invalid.
     * Throws if an invalid identifier is provided.
     * Throws if the provided value type does not match the
     * property type.
     * Throws if the provided value is outside the expected range
     * (int, float properties).
     * Throws if the provided string value does not match the set
     * of accepted enum values (enum properties
     *
     * {@link ArgumentOutOfBoundsError}
     *
     * {@link InvalidArgumentError}
     *
     * {@link InvalidEntityError}
     */
    setProperty(identifier: string, value: boolean | number | string): void;
    /**
     * @remarks
     * Sets the main rotation of the entity.
     *
     * @worldMutation
     *
     * @param rotation
     * The x and y rotation of the entity (in degrees). For most
     * mobs, the x rotation controls the head tilt and the y
     * rotation controls the body rotation.
     * @throws This function can throw errors.
     *
     * {@link InvalidEntityError}
     */
    setRotation(rotation: Vector2): void;
    /**
     * @remarks
     * Teleports the selected entity to a new location
     *
     * @worldMutation
     *
     * @param location
     * New location for the entity.
     * @param teleportOptions
     * Options regarding the teleport operation.
     * @throws This function can throw errors.
     *
     * {@link InvalidEntityError}
     *
     * {@link UnsupportedFunctionalityError}
     * @seeExample teleport.ts
     * @seeExample teleportMovement.ts
     */
    teleport(location: Vector3, teleportOptions?: TeleportOptions): void;
    /**
     * @remarks
     * Triggers an entity type event. For every entity, a number of
     * events are defined in an entities' definition for key entity
     * behaviors; for example, creepers have a
     * minecraft:start_exploding type event.
     *
     * @worldMutation
     *
     * @param eventName
     * Name of the entity type event to trigger. If a namespace is
     * not specified, minecraft: is assumed.
     * @throws
     * If the event is not defined in the definition of the entity,
     * an error will be thrown.
     *
     * {@link InvalidArgumentError}
     *
     * {@link InvalidEntityError}
     * @seeExample triggerEvent.ts e68d4331
     * @seeExample triggerEvent.ts d45f49d2
     */
    triggerEvent(eventName: string): void;
    /**
     * @remarks
     * Attempts to try a teleport, but may not complete the
     * teleport operation (for example, if there are blocks at the
     * destination.)
     *
     * @worldMutation
     *
     * @param location
     * Location to teleport the entity to.
     * @param teleportOptions
     * Options regarding the teleport operation.
     * @returns
     * Returns whether the teleport succeeded. This can fail if the
     * destination chunk is unloaded or if the teleport would
     * result in intersecting with blocks.
     * @throws This function can throw errors.
     *
     * {@link InvalidEntityError}
     *
     * {@link UnsupportedFunctionalityError}
     */
    tryTeleport(location: Vector3, teleportOptions?: TeleportOptions): boolean;
}

/**
 * When added, this component makes the entity spawn with a
 * rider of the specified entityType.
 */
// @ts-ignore Class inheritance allowed for native defined classes
export class EntityAddRiderComponent extends EntityComponent {
    private constructor();
    /**
     * @remarks
     * The type of entity that is added as a rider for this entity
     * when spawned under certain conditions.
     *
     * @throws This property can throw when used.
     */
    readonly entityType: string;
    /**
     * @remarks
     * Optional spawn event to trigger on the rider when that rider
     * is spawned for this entity.
     *
     * @throws This property can throw when used.
     */
    readonly spawnEvent: string;
    static readonly componentId = 'minecraft:addrider';
}

/**
 * Adds a timer for the entity to grow up. It can be
 * accelerated by giving the entity the items it likes as
 * defined by feedItems.
 */
// @ts-ignore Class inheritance allowed for native defined classes
export class EntityAgeableComponent extends EntityComponent {
    private constructor();
    /**
     * @remarks
     * Amount of time before the entity grows up, -1 for always a
     * baby.
     *
     * @throws This property can throw when used.
     */
    readonly duration: number;
    /**
     * @remarks
     * Event that runs when this entity grows up.
     *
     * @throws This property can throw when used.
     */
    readonly growUp: Trigger;
    /**
     * @remarks
     * The feed item used will transform into this item upon
     * successful interaction.
     *
     * @throws This property can throw when used.
     */
    readonly transformToItem: string;
    static readonly componentId = 'minecraft:ageable';
    /**
     * @remarks
     * List of items that the entity drops when it grows up.
     *
     * @throws This function can throw errors.
     */
    getDropItems(): string[];
    /**
     * @remarks
     * List of items that can be fed to the entity. Includes 'item'
     * for the item name and 'growth' to define how much time it
     * grows up by.
     *
     * @throws This function can throw errors.
     */
    getFeedItems(): EntityDefinitionFeedItem[];
}

/**
 * This is a base abstract class for any entity component that
 * centers around a number and can have a minimum, maximum, and
 * default defined value.
 */
// @ts-ignore Class inheritance allowed for native defined classes
export class EntityAttributeComponent extends EntityComponent {
    private constructor();
    /**
     * @remarks
     * Current value of this attribute for this instance.
     *
     * @throws This property can throw when used.
     */
    readonly currentValue: number;
    /**
     * @remarks
     * Returns the default defined value for this attribute.
     *
     * @throws This property can throw when used.
     */
    readonly defaultValue: number;
    /**
     * @remarks
     * Returns the effective max of this attribute given any other
     * ambient components or factors.
     *
     * @throws This property can throw when used.
     */
    readonly effectiveMax: number;
    /**
     * @remarks
     * Returns the effective min of this attribute given any other
     * ambient components or factors.
     *
     * @throws This property can throw when used.
     */
    readonly effectiveMin: number;
    /**
     * @remarks
     * Resets the current value of this attribute to the defined
     * default value.
     *
     * @worldMutation
     *
     * @throws This function can throw errors.
     */
    resetToDefaultValue(): void;
    /**
     * @remarks
     * Resets the current value of this attribute to the maximum
     * defined value.
     *
     * @worldMutation
     *
     * @throws This function can throw errors.
     */
    resetToMaxValue(): void;
    /**
     * @remarks
     * Resets the current value of this attribute to the minimum
     * defined value.
     *
     * @worldMutation
     *
     * @throws This function can throw errors.
     */
    resetToMinValue(): void;
    /**
     * @remarks
     * Sets the current value of this attribute.
     *
     * @worldMutation
     *
     * @throws
     * If the value is out of bounds, an ArgumentOutOfBounds Error
     * is thrown.
     *
     * {@link ArgumentOutOfBoundsError}
     *
     * {@link InvalidEntityError}
     */
    setCurrentValue(value: number): boolean;
}

/**
 * Base class for a family of entity movement events.
 */
// @ts-ignore Class inheritance allowed for native defined classes
export class EntityBaseMovementComponent extends EntityComponent {
    private constructor();
    /**
     * @remarks
     * Maximum turn rate for this movement modality of the mob.
     *
     * @throws This property can throw when used.
     */
    readonly maxTurn: number;
}

/**
 * Defines what blocks this entity can breathe in and gives
 * them the ability to suffocate.
 */
// @ts-ignore Class inheritance allowed for native defined classes
export class EntityBreathableComponent extends EntityComponent {
    private constructor();
    /**
     * @beta
     * @remarks
     * The current air supply of the entity.
     *
     * @worldMutation
     *
     * @throws
     * Will throw an error if the air supply is out of bounds
     * [suffocationTime, maxAirSupply].
     */
    airSupply: number;
    /**
     * @remarks
     * If true, this entity can breathe in air.
     *
     * @throws This property can throw when used.
     */
    readonly breathesAir: boolean;
    /**
     * @remarks
     * If true, this entity can breathe in lava.
     *
     * @throws This property can throw when used.
     */
    readonly breathesLava: boolean;
    /**
     * @remarks
     * If true, this entity can breathe in solid blocks.
     *
     * @throws This property can throw when used.
     */
    readonly breathesSolids: boolean;
    /**
     * @remarks
     * If true, this entity can breathe in water.
     *
     * @throws This property can throw when used.
     */
    readonly breathesWater: boolean;
    /**
     * @beta
     * @remarks
     * If true, the entity is able to breathe.
     *
     * @throws This property can throw when used.
     */
    readonly canBreathe: boolean;
    /**
     * @remarks
     * If true, this entity will have visible bubbles while in
     * water.
     *
     * @throws This property can throw when used.
     */
    readonly generatesBubbles: boolean;
    /**
     * @remarks
     * Time in seconds to recover breath to maximum.
     *
     * @throws This property can throw when used.
     */
    readonly inhaleTime: number;
    /**
     * @remarks
     * Time in seconds between suffocation damage.
     *
     * @throws This property can throw when used.
     */
    readonly suffocateTime: number;
    /**
     * @remarks
     * Time in seconds the entity can hold its breath.
     *
     * @throws This property can throw when used.
     */
    readonly totalSupply: number;
    static readonly componentId = 'minecraft:breathable';
    /**
     * @remarks
     * List of blocks this entity can breathe in, in addition to
     * the separate properties for classes of blocks.
     *
     * @throws This function can throw errors.
     */
    getBreatheBlocks(): BlockPermutation[];
    /**
     * @remarks
     * List of blocks this entity can't breathe in.
     *
     * @throws This function can throw errors.
     */
    getNonBreatheBlocks(): BlockPermutation[];
}

/**
 * When added, this component signifies that the entity can
 * climb up ladders.
 */
// @ts-ignore Class inheritance allowed for native defined classes
export class EntityCanClimbComponent extends EntityComponent {
    private constructor();
    static readonly componentId = 'minecraft:can_climb';
}

/**
 * When added, this component signifies that the entity can
 * fly, and the pathfinder won't be restricted to paths where a
 * solid block is required underneath it.
 */
// @ts-ignore Class inheritance allowed for native defined classes
export class EntityCanFlyComponent extends EntityComponent {
    private constructor();
    static readonly componentId = 'minecraft:can_fly';
}

/**
 * When added, this component signifies that the entity can
 * power jump like the horse does within Minecraft.
 */
// @ts-ignore Class inheritance allowed for native defined classes
export class EntityCanPowerJumpComponent extends EntityComponent {
    private constructor();
    static readonly componentId = 'minecraft:can_power_jump';
}

/**
 * Defines the entity's secondary color. Only works on certain
 * entities that have secondary predefined color values (e.g.,
 * tropical fish).
 */
// @ts-ignore Class inheritance allowed for native defined classes
export class EntityColor2Component extends EntityComponent {
    private constructor();
    /**
     * @remarks
     * Value of this particular color.
     *
     * @throws This property can throw when used.
     */
    readonly value: PaletteColor;
    static readonly componentId = 'minecraft:color2';
}

/**
 * Defines the entity's color. Only works on certain entities
 * that have predefined color values (e.g., sheep, llama,
 * shulker).
 */
// @ts-ignore Class inheritance allowed for native defined classes
export class EntityColorComponent extends EntityComponent {
    private constructor();
    /**
     * @remarks
     * Value of this particular color.
     *
     * @worldMutation
     *
     */
    value: number;
    static readonly componentId = 'minecraft:color';
}

/**
 * Base class for downstream entity components.
 */
// @ts-ignore Class inheritance allowed for native defined classes
export class EntityComponent extends Component {
    private constructor();
    /**
     * @remarks
     * The entity that owns this component. The entity will be
     * undefined if it has been removed.
     *
     * @throws This property can throw when used.
     *
     * {@link InvalidEntityError}
     */
    readonly entity: Entity;
}

/**
 * Contains information regarding a specific entity container
 * being closed.
 */
export class EntityContainerClosedAfterEvent {
    private constructor();
    /**
     * @remarks
     * The source of the entity container being closed.
     *
     */
    readonly closeSource: ContainerAccessSource;
    readonly entity: Entity;
}

/**
 * Manages callbacks that are connected to when an entity
 * container is closed.
 */
export class EntityContainerClosedAfterEventSignal {
    private constructor();
    /**
     * @remarks
     * Adds a callback that will be called when an entity container
     * is closed.
     *
     * @worldMutation
     *
     * @earlyExecution
     *
     */
    subscribe(
        callback: (arg0: EntityContainerClosedAfterEvent) => void,
        options?: EntityContainerAccessEventOptions,
    ): (arg0: EntityContainerClosedAfterEvent) => void;
    /**
     * @remarks
     * Removes a callback from being called when an entity
     * container is closed.
     *
     * @worldMutation
     *
     * @earlyExecution
     *
     */
    unsubscribe(callback: (arg0: EntityContainerClosedAfterEvent) => void): void;
}

/**
 * Contains information regarding a specific entity container
 * being opened.
 */
export class EntityContainerOpenedAfterEvent {
    private constructor();
    readonly entity: Entity;
    /**
     * @remarks
     * The source of the entity container being opened.
     *
     */
    readonly openSource: ContainerAccessSource;
}

/**
 * Manages callbacks that are connected to when an entity
 * container is opened.
 */
export class EntityContainerOpenedAfterEventSignal {
    private constructor();
    /**
     * @remarks
     * Adds a callback that will be called when an entity container
     * is opened.
     *
     * @worldMutation
     *
     * @earlyExecution
     *
     */
    subscribe(
        callback: (arg0: EntityContainerOpenedAfterEvent) => void,
        options?: EntityContainerAccessEventOptions,
    ): (arg0: EntityContainerOpenedAfterEvent) => void;
    /**
     * @remarks
     * Removes a callback from being called when an entity
     * container is opened.
     *
     * @worldMutation
     *
     * @earlyExecution
     *
     */
    unsubscribe(callback: (arg0: EntityContainerOpenedAfterEvent) => void): void;
}

/**
 * As part of the Ageable component, represents a set of items
 * that can be fed to an entity and the rate at which that
 * causes them to grow.
 */
export class EntityDefinitionFeedItem {
    private constructor();
    /**
     * @remarks
     * The amount by which an entity's age will increase when fed
     * this item. Values usually range between 0 and 1.
     *
     */
    readonly growth: number;
    /**
     * @remarks
     * Identifier of type of item that can be fed. If a namespace
     * is not specified, 'minecraft:' is assumed. Example values
     * include 'wheat' or 'golden_apple'.
     *
     */
    readonly item: string;
    /**
     * @remarks
     * Type ID of the resulting item after feeding has occurred.
     * This will usually be empty but is used for scenarios such as
     * feeding a Nautilus with a bucket of fish, where the result
     * item will be an empty bucket.
     *
     */
    readonly resultItem?: string;
}

/**
 * Contains data related to the death of an entity in the game.
 */
export class EntityDieAfterEvent {
    private constructor();
    /**
     * @remarks
     * If specified, provides more information on the source of
     * damage that caused the death of this entity.
     *
     */
    readonly damageSource: EntityDamageSource;
    /**
     * @remarks
     * Now-dead entity object.
     *
     */
    readonly deadEntity: Entity;
}

/**
 * Supports registering for an event that fires after an entity
 * has died.
 */
export class EntityDieAfterEventSignal {
    private constructor();
    /**
     * @remarks
     * Subscribes to an event that fires when an entity dies.
     *
     * @worldMutation
     *
     * @earlyExecution
     *
     * @param callback
     * Function to call when an entity dies.
     * @param options
     * Additional filtering options for when the subscription
     * fires.
     * @returns
     * Returns the closure that can be used in future downstream
     * calls to unsubscribe.
     */
    subscribe(
        callback: (arg0: EntityDieAfterEvent) => void,
        options?: EntityEventOptions,
    ): (arg0: EntityDieAfterEvent) => void;
    /**
     * @remarks
     * Stops this event from calling your function when an entity
     * dies.
     *
     * @worldMutation
     *
     * @earlyExecution
     *
     */
    unsubscribe(callback: (arg0: EntityDieAfterEvent) => void): void;
}

/**
 * Represents this entity's ender inventory properties. This
 * component is always present on players and any items in its
 * container will display for the player when they access an
 * ender chest.
 */
// @ts-ignore Class inheritance allowed for native defined classes
export class EntityEnderInventoryComponent extends EntityComponent {
    private constructor();
    /**
     * @remarks
     * Defines the ender inventory container for this entity. The
     * container will be undefined if the entity has been removed.
     *
     * @throws This property can throw when used.
     *
     * {@link InvalidEntityError}
     */
    readonly container: Container;
    static readonly componentId = 'minecraft:ender_inventory';
}

/**
 * Provides access to a mob's equipment slots. This component
 * exists on player entities.
 * @seeExample givePlayerElytra.ts
 */
// @ts-ignore Class inheritance allowed for native defined classes
export class EntityEquippableComponent extends EntityComponent {
    private constructor();
    /**
     * @remarks
     * Returns the total Armor level of the owner.
     *
     * @throws This property can throw when used.
     *
     * {@link InvalidEntityError}
     */
    readonly totalArmor: number;
    /**
     * @remarks
     * Returns the total Toughness level of the owner.
     *
     * @throws This property can throw when used.
     *
     * {@link InvalidEntityError}
     */
    readonly totalToughness: number;
    static readonly componentId = 'minecraft:equippable';
    /**
     * @remarks
     * Gets the equipped item for the given EquipmentSlot.
     *
     * @param equipmentSlot
     * The equipment slot. e.g. "head", "chest", "offhand"
     * @returns
     * Returns the item equipped to the given EquipmentSlot. If
     * empty, returns undefined.
     * @throws This function can throw errors.
     */
    getEquipment(equipmentSlot: EquipmentSlot): ItemStack | undefined;
    /**
     * @remarks
     * Gets the ContainerSlot corresponding to the given
     * EquipmentSlot.
     *
     * @param equipmentSlot
     * The equipment slot. e.g. "head", "chest", "offhand".
     * @returns
     * Returns the ContainerSlot corresponding to the given
     * EquipmentSlot.
     * @throws This function can throw errors.
     */
    getEquipmentSlot(equipmentSlot: EquipmentSlot): ContainerSlot;
    /**
     * @remarks
     * Replaces the item in the given EquipmentSlot.
     *
     * @worldMutation
     *
     * @param equipmentSlot
     * The equipment slot. e.g. "head", "chest", "offhand".
     * @param itemStack
     * The item to equip. If undefined, clears the slot.
     * @throws This function can throw errors.
     */
    setEquipment(equipmentSlot: EquipmentSlot, itemStack?: ItemStack): boolean;
}

/**
 * Defines the interactions with this entity for Exhaustion.
 * Wraps the `minecraft.player.exhaustion` attribute.
 */
// @ts-ignore Class inheritance allowed for native defined classes
export class EntityExhaustionComponent extends EntityAttributeComponent {
    private constructor();
    static readonly componentId = 'minecraft:player.exhaustion';
}

/**
 * When added, this component signifies that this entity
 * doesn't take damage from fire.
 */
// @ts-ignore Class inheritance allowed for native defined classes
export class EntityFireImmuneComponent extends EntityComponent {
    private constructor();
    static readonly componentId = 'minecraft:fire_immune';
}

/**
 * When added, this component signifies that this entity can
 * float in liquid blocks.
 */
// @ts-ignore Class inheritance allowed for native defined classes
export class EntityFloatsInLiquidComponent extends EntityComponent {
    private constructor();
    static readonly componentId = 'minecraft:floats_in_liquid';
}

/**
 * Represents the flying speed of an entity.
 */
// @ts-ignore Class inheritance allowed for native defined classes
export class EntityFlyingSpeedComponent extends EntityComponent {
    private constructor();
    /**
     * @remarks
     * Current value of the flying speed of the associated entity.
     *
     * @worldMutation
     *
     */
    value: number;
    static readonly componentId = 'minecraft:flying_speed';
}

/**
 * Defines how much friction affects this entity.
 */
// @ts-ignore Class inheritance allowed for native defined classes
export class EntityFrictionModifierComponent extends EntityComponent {
    private constructor();
    /**
     * @remarks
     * Current value of the friction modifier of the associated
     * entity.
     *
     * @throws This property can throw when used.
     */
    readonly value: number;
    static readonly componentId = 'minecraft:friction_modifier';
}

/**
 * Loot item condition that checks the value of the mark
 * variant of a mob as it drops its loot.
 */
// @ts-ignore Class inheritance allowed for native defined classes
export class EntityHasMarkVariantCondition extends LootItemCondition {
    private constructor();
    /**
     * @remarks
     * The mark variant value the mob must have for this condition
     * to pass.
     *
     */
    readonly value: number;
}

/**
 * Loot item condition that checks the variant value of a mob
 * as it drops its loot.
 */
// @ts-ignore Class inheritance allowed for native defined classes
export class EntityHasVariantCondition extends LootItemCondition {
    private constructor();
    /**
     * @remarks
     * The variant value the mob must have for this condition to
     * pass.
     *
     */
    readonly value: number;
}

/**
 * Defines the interactions with this entity for healing it.
 */
// @ts-ignore Class inheritance allowed for native defined classes
export class EntityHealableComponent extends EntityComponent {
    private constructor();
    /**
     * @remarks
     * Determines if an item can be used regardless of the entity
     * being at full health.
     *
     * @throws This property can throw when used.
     */
    readonly forceUse: boolean;
    static readonly componentId = 'minecraft:healable';
    /**
     * @remarks
     * A set of items that can specifically heal this entity.
     *
     * @returns
     * Entity that this component is associated with.
     * @throws This function can throw errors.
     */
    getFeedItems(): FeedItem[];
}

/**
 * Contains information related to an entity having been
 * healed.
 */
export class EntityHealAfterEvent {
    private constructor();
    /**
     * @remarks
     * Entity that was healed.
     *
     */
    readonly healedEntity: Entity;
    /**
     * @remarks
     * Describes the amount of healing.
     *
     */
    readonly healing: number;
    /**
     * @remarks
     * Information on the source of healing.
     *
     */
    readonly healSource: EntityHealSource;
}

/**
 * Manages callbacks that are connected to when an entity is
 * healed.
 */
export class EntityHealAfterEventSignal {
    private constructor();
    /**
     * @remarks
     * Adds a callback that will be called when an entity is
     * healed.
     *
     * @worldMutation
     *
     * @earlyExecution
     *
     */
    subscribe(
        callback: (arg0: EntityHealAfterEvent) => void,
        options?: EntityHealEventOptions,
    ): (arg0: EntityHealAfterEvent) => void;
    /**
     * @remarks
     * Removes a callback from being called when an entity is
     * healed.
     *
     * @worldMutation
     *
     * @earlyExecution
     *
     */
    unsubscribe(callback: (arg0: EntityHealAfterEvent) => void): void;
}

/**
 * Contains information related to an entity that will be
 * healed.
 */
export class EntityHealBeforeEvent {
    private constructor();
    cancel: boolean;
    /**
     * @remarks
     * Entity that will be healed.
     *
     */
    readonly healedEntity: Entity;
    /**
     * @remarks
     * Describes the amount of healing.
     *
     */
    healing: number;
    /**
     * @remarks
     * Information on the source of healing.
     *
     */
    readonly healSource: EntityHealSource;
}

/**
 * Manages callbacks that are connected to when an entity will
 * be healed.
 */
export class EntityHealBeforeEventSignal {
    private constructor();
    /**
     * @remarks
     * Adds a callback that will be called when an entity will be
     * healed.
     *
     * @worldMutation
     *
     * @earlyExecution
     *
     * @param callback
     * This closure is called with restricted-execution privilege.
     * @returns
     * Closure that is called with restricted-execution privilege.
     */
    subscribe(
        callback: (arg0: EntityHealBeforeEvent) => void,
        options?: EntityHealEventOptions,
    ): (arg0: EntityHealBeforeEvent) => void;
    /**
     * @remarks
     * Removes a callback from being called when an entity will be
     * healed.
     *
     * @worldMutation
     *
     * @earlyExecution
     *
     * @param callback
     * This closure is called with restricted-execution privilege.
     */
    unsubscribe(callback: (arg0: EntityHealBeforeEvent) => void): void;
}

/**
 * Provides information about how healing has been applied to
 * an entity.
 */
export class EntityHealSource {
    private constructor();
    /**
     * @remarks
     * Cause enumerator of the source of healing.
     *
     */
    readonly cause: EntityHealCause;
}

/**
 * Contains information related to an entity when its health
 * changes. Warning: don't change the health of an entity in
 * this event, or it will cause an infinite loop!
 */
export class EntityHealthChangedAfterEvent {
    private constructor();
    /**
     * @remarks
     * Entity whose health changed.
     *
     */
    readonly entity: Entity;
    /**
     * @remarks
     * New health value of the entity.
     *
     */
    readonly newValue: number;
    /**
     * @remarks
     * Old health value of the entity.
     *
     */
    readonly oldValue: number;
}

/**
 * Manages callbacks that are connected to when the health of
 * an entity changes.
 */
export class EntityHealthChangedAfterEventSignal {
    private constructor();
    /**
     * @remarks
     * Adds a callback that will be called when the health of an
     * entity changes.
     *
     * @worldMutation
     *
     * @earlyExecution
     *
     */
    subscribe(
        callback: (arg0: EntityHealthChangedAfterEvent) => void,
        options?: EntityEventOptions,
    ): (arg0: EntityHealthChangedAfterEvent) => void;
    /**
     * @remarks
     * Removes a callback from being called when the health of an
     * entity changes.
     *
     * @worldMutation
     *
     * @earlyExecution
     *
     */
    unsubscribe(callback: (arg0: EntityHealthChangedAfterEvent) => void): void;
}

/**
 * Defines the health properties of an entity.
 * @seeExample applyDamageThenHeal.ts
 */
// @ts-ignore Class inheritance allowed for native defined classes
export class EntityHealthComponent extends EntityAttributeComponent {
    private constructor();
    static readonly componentId = 'minecraft:health';
}

/**
 * Contains information related to an entity hitting a block.
 */
export class EntityHitBlockAfterEvent {
    private constructor();
    /**
     * @remarks
     * Face of the block that was hit.
     *
     */
    readonly blockFace: Direction;
    /**
     * @remarks
     * Entity that made the attack.
     *
     */
    readonly damagingEntity: Entity;
    /**
     * @remarks
     * Block that was hit by the attack.
     *
     */
    readonly hitBlock: Block;
    /**
     * @remarks
     * Block permutation that was hit by the attack.
     *
     */
    readonly hitBlockPermutation: BlockPermutation;
}

/**
 * Manages callbacks that are connected to when an entity hits
 * a block.
 */
export class EntityHitBlockAfterEventSignal {
    private constructor();
    /**
     * @remarks
     * Adds a callback that will be called when an entity hits a
     * block.
     *
     * @worldMutation
     *
     * @earlyExecution
     *
     */
    subscribe(
        callback: (arg0: EntityHitBlockAfterEvent) => void,
        options?: EntityEventOptions,
    ): (arg0: EntityHitBlockAfterEvent) => void;
    /**
     * @remarks
     * Removes a callback from being called when an entity hits a
     * block.
     *
     * @worldMutation
     *
     * @earlyExecution
     *
     */
    unsubscribe(callback: (arg0: EntityHitBlockAfterEvent) => void): void;
}

/**
 * Contains information related to an entity hitting (melee
 * attacking) another entity.
 */
export class EntityHitEntityAfterEvent {
    private constructor();
    /**
     * @remarks
     * Entity that made a hit/melee attack.
     *
     */
    readonly damagingEntity: Entity;
    /**
     * @remarks
     * Entity that was hit by the attack.
     *
     */
    readonly hitEntity: Entity;
}

/**
 * Manages callbacks that are connected to when an entity makes
 * a melee attack on another entity.
 */
export class EntityHitEntityAfterEventSignal {
    private constructor();
    /**
     * @remarks
     * Adds a callback that will be called when an entity hits
     * another entity.
     *
     * @worldMutation
     *
     * @earlyExecution
     *
     */
    subscribe(
        callback: (arg0: EntityHitEntityAfterEvent) => void,
        options?: EntityEventOptions,
    ): (arg0: EntityHitEntityAfterEvent) => void;
    /**
     * @remarks
     * Removes a callback from being called when an entity makes a
     * melee attack on another entity.
     *
     * @worldMutation
     *
     * @earlyExecution
     *
     */
    unsubscribe(callback: (arg0: EntityHitEntityAfterEvent) => void): void;
}

/**
 * Defines the interactions with this entity for hunger. Wraps
 * the `minecraft.player.hunger` attribute.
 */
// @ts-ignore Class inheritance allowed for native defined classes
export class EntityHungerComponent extends EntityAttributeComponent {
    private constructor();
    static readonly componentId = 'minecraft:player.hunger';
}

/**
 * Contains information related to an entity getting hurt.
 */
export class EntityHurtAfterEvent {
    private constructor();
    /**
     * @remarks
     * Describes the amount of damage caused.
     *
     */
    readonly damage: number;
    /**
     * @remarks
     * Source information on the entity that may have applied this
     * damage.
     *
     */
    readonly damageSource: EntityDamageSource;
    /**
     * @remarks
     * Entity that was hurt.
     *
     */
    readonly hurtEntity: Entity;
}

/**
 * Manages callbacks that are connected to when an entity is
 * hurt.
 */
export class EntityHurtAfterEventSignal {
    private constructor();
    /**
     * @remarks
     * Adds a callback that will be called when an entity is hurt.
     *
     * @worldMutation
     *
     * @earlyExecution
     *
     */
    subscribe(
        callback: (arg0: EntityHurtAfterEvent) => void,
        options?: EntityHurtAfterEventOptions,
    ): (arg0: EntityHurtAfterEvent) => void;
    /**
     * @remarks
     * Removes a callback from being called when an entity is hurt.
     *
     * @worldMutation
     *
     * @earlyExecution
     *
     */
    unsubscribe(callback: (arg0: EntityHurtAfterEvent) => void): void;
}

/**
 * Contains information related to an entity that will be hurt.
 */
export class EntityHurtBeforeEvent {
    private constructor();
    cancel: boolean;
    /**
     * @remarks
     * Describes the amount of damage that will be caused.
     *
     */
    damage: number;
    /**
     * @remarks
     * Source information on the entity that may have applied this
     * damage.
     *
     */
    readonly damageSource: EntityDamageSource;
    /**
     * @remarks
     * Entity that will be hurt.
     *
     */
    readonly hurtEntity: Entity;
}

/**
 * Manages callbacks that are connected to when an entity will
 * be hurt.
 */
export class EntityHurtBeforeEventSignal {
    private constructor();
    /**
     * @remarks
     * Adds a callback that will be called when an entity will be
     * hurt.
     *
     * @worldMutation
     *
     * @earlyExecution
     *
     * @param callback
     * This closure is called with restricted-execution privilege.
     * @returns
     * Closure that is called with restricted-execution privilege.
     */
    subscribe(
        callback: (arg0: EntityHurtBeforeEvent) => void,
        options?: EntityHurtBeforeEventOptions,
    ): (arg0: EntityHurtBeforeEvent) => void;
    /**
     * @remarks
     * Removes a callback from being called when an entity will be
     * hurt.
     *
     * @worldMutation
     *
     * @earlyExecution
     *
     * @param callback
     * This closure is called with restricted-execution privilege.
     */
    unsubscribe(callback: (arg0: EntityHurtBeforeEvent) => void): void;
}

/**
 * 表示一个实体的物品栏属性。
 * 
 * Defines this entity's inventory properties.
 */
// @ts-ignore Class inheritance allowed for native defined classes
export class EntityInventoryComponent extends EntityComponent {
    private constructor();
    /**
     * @remarks
     * 表示实体每层强度（实体组件 `minecraft:strength`）可以获得的额外槽数。
     * 
     * Number of slots that this entity can gain per extra
     * strength.
     *
     * @throws This property can throw when used.
     */
    readonly additionalSlotsPerStrength: number;
    /**
     * @remarks
     * 若为 `true`，则表示实体物品栏中的物品可被漏斗或漏斗矿车转移走。
     * 
     * If true, the contents of this inventory can be removed by a
     * hopper.
     *
     * @throws This property can throw when used.
     */
    readonly canBeSiphonedFrom: boolean;
    /**
     * @remarks
     * 表示实体的容器。若实体已被移除则为 `undefined`。
     * 
     * Defines the container for this entity. The container will be
     * undefined if the entity has been removed.
     *
     * @throws This property can throw when used.
     *
     * {@link InvalidEntityError}
     */
    readonly container: Container;
    /**
     * @remarks
     * 表示实体容器的种类。
     * 
     * Type of container this entity has.
     *
     * @throws This property can throw when used.
     */
    readonly containerType: string;
    /**
     * @remarks
     * 表示实体容器的槽位数量。
     * 
     * Number of slots the container has.
     *
     * @throws This property can throw when used.
     */
    readonly inventorySize: number;
    /**
     * @remarks
     * 若为 `true`，则表示实体死亡不会掉落物品栏的物品。
     * 
     * If true, the entity will not drop it's inventory on death.
     *
     * @throws This property can throw when used.
     */
    readonly 'private': boolean;
    /**
     * @remarks
     * 若为 `true`，则表示实体物品栏仅能被其主人或实体自己访问和操作。
     * 
     * If true, the entity's inventory can only be accessed by its
     * owner or itself.
     *
     * @throws This property can throw when used.
     */
    readonly restrictToOwner: boolean;
    static readonly componentId = 'minecraft:inventory';
}

/**
 * When added, this component signifies that this entity is a
 * baby.
 */
// @ts-ignore Class inheritance allowed for native defined classes
export class EntityIsBabyComponent extends EntityComponent {
    private constructor();
    static readonly componentId = 'minecraft:is_baby';
}

/**
 * When added, this component signifies that this entity is
 * charged.
 */
// @ts-ignore Class inheritance allowed for native defined classes
export class EntityIsChargedComponent extends EntityComponent {
    private constructor();
    static readonly componentId = 'minecraft:is_charged';
}

/**
 * When added, this component signifies that this entity is
 * currently carrying a chest.
 */
// @ts-ignore Class inheritance allowed for native defined classes
export class EntityIsChestedComponent extends EntityComponent {
    private constructor();
    static readonly componentId = 'minecraft:is_chested';
}

/**
 * When added, this component signifies that dyes can be used
 * on this entity to change its color.
 */
// @ts-ignore Class inheritance allowed for native defined classes
export class EntityIsDyeableComponent extends EntityComponent {
    private constructor();
    static readonly componentId = 'minecraft:is_dyeable';
}

/**
 * When added, this component signifies that this entity can
 * hide from hostile mobs while invisible.
 */
// @ts-ignore Class inheritance allowed for native defined classes
export class EntityIsHiddenWhenInvisibleComponent extends EntityComponent {
    private constructor();
    static readonly componentId = 'minecraft:is_hidden_when_invisible';
}

/**
 * When added, this component signifies that this entity this
 * currently on fire.
 */
// @ts-ignore Class inheritance allowed for native defined classes
export class EntityIsIgnitedComponent extends EntityComponent {
    private constructor();
    static readonly componentId = 'minecraft:is_ignited';
}

/**
 * When added, this component signifies that this entity is an
 * illager captain.
 */
// @ts-ignore Class inheritance allowed for native defined classes
export class EntityIsIllagerCaptainComponent extends EntityComponent {
    private constructor();
    static readonly componentId = 'minecraft:is_illager_captain';
}

/**
 * When added, this component signifies that this entity is
 * currently saddled.
 */
// @ts-ignore Class inheritance allowed for native defined classes
export class EntityIsSaddledComponent extends EntityComponent {
    private constructor();
    static readonly componentId = 'minecraft:is_saddled';
}

/**
 * When added, this component signifies that this entity is
 * currently shaking.
 */
// @ts-ignore Class inheritance allowed for native defined classes
export class EntityIsShakingComponent extends EntityComponent {
    private constructor();
    static readonly componentId = 'minecraft:is_shaking';
}

/**
 * When added, this component signifies that this entity is
 * currently sheared.
 */
// @ts-ignore Class inheritance allowed for native defined classes
export class EntityIsShearedComponent extends EntityComponent {
    private constructor();
    static readonly componentId = 'minecraft:is_sheared';
}

/**
 * When added, this component signifies that this entity can be
 * stacked.
 */
// @ts-ignore Class inheritance allowed for native defined classes
export class EntityIsStackableComponent extends EntityComponent {
    private constructor();
    static readonly componentId = 'minecraft:is_stackable';
}

/**
 * When added, this component signifies that this entity is
 * currently stunned.
 */
// @ts-ignore Class inheritance allowed for native defined classes
export class EntityIsStunnedComponent extends EntityComponent {
    private constructor();
    static readonly componentId = 'minecraft:is_stunned';
}

/**
 * When added, this component signifies that this entity is
 * currently tamed.
 */
// @ts-ignore Class inheritance allowed for native defined classes
export class EntityIsTamedComponent extends EntityComponent {
    private constructor();
    static readonly componentId = 'minecraft:is_tamed';
}

/**
 * If added onto the entity, this indicates that the entity
 * represents a free-floating item in the world. Lets you
 * retrieve the actual item stack contents via the itemStack
 * property.
 * @seeExample testThatEntityIsFeatherItem.ts
 */
// @ts-ignore Class inheritance allowed for native defined classes
export class EntityItemComponent extends EntityComponent {
    private constructor();
    /**
     * @remarks
     * Item stack represented by this entity in the world.
     *
     * @throws This property can throw when used.
     */
    readonly itemStack: ItemStack;
    static readonly componentId = 'minecraft:item';
}

/**
 * Contains information related to an entity having dropped
 * items.
 */
export class EntityItemDropAfterEvent {
    private constructor();
    /**
     * @remarks
     * The entity that has dropped the items.
     *
     */
    readonly entity: Entity;
    /**
     * @remarks
     * The list of items the entity has dropped.
     *
     */
    readonly items: Entity[];
}

/**
 * Manages callbacks that are connected to when an entity has
 * dropped items.
 */
export class EntityItemDropAfterEventSignal {
    private constructor();
    /**
     * @remarks
     * Adds a callback that will be called when an entity has
     * dropped items.
     *
     * @worldMutation
     *
     * @earlyExecution
     *
     */
    subscribe(
        callback: (arg0: EntityItemDropAfterEvent) => void,
        options?: EntityItemDropEventOptions,
    ): (arg0: EntityItemDropAfterEvent) => void;
    /**
     * @remarks
     * Removes a callback from being called when an entity has
     * dropped items.
     *
     * @worldMutation
     *
     * @earlyExecution
     *
     */
    unsubscribe(callback: (arg0: EntityItemDropAfterEvent) => void): void;
}

/**
 * Contains information related to an entity having picked up
 * items.
 */
export class EntityItemPickupAfterEvent {
    private constructor();
    /**
     * @remarks
     * The entity that has picked up the items.
     *
     */
    readonly entity: Entity;
    /**
     * @remarks
     * The list of items the entity has picked up.
     *
     */
    readonly items: ItemStack[];
}

/**
 * Manages callbacks that are connected to when an entity has
 * picked up items.
 */
export class EntityItemPickupAfterEventSignal {
    private constructor();
    /**
     * @remarks
     * Adds a callback that will be called when an entity has
     * picked up items.
     *
     * @worldMutation
     *
     * @earlyExecution
     *
     */
    subscribe(
        callback: (arg0: EntityItemPickupAfterEvent) => void,
        options?: EntityItemPickupEventOptions,
    ): (arg0: EntityItemPickupAfterEvent) => void;
    /**
     * @remarks
     * Removes a callback from being called when an entity has
     * picked up items.
     *
     * @worldMutation
     *
     * @earlyExecution
     *
     */
    unsubscribe(callback: (arg0: EntityItemPickupAfterEvent) => void): void;
}

/**
 * Contains information related to an entity picking up an
 * item.
 */
export class EntityItemPickupBeforeEvent {
    private constructor();
    /**
     * @remarks
     * If set to true the item will not be picked up.
     *
     */
    cancel: boolean;
    /**
     * @remarks
     * The entity that will pick up the item.
     *
     */
    readonly entity: Entity;
    /**
     * @remarks
     * The item that will be picked up.
     *
     */
    readonly item: Entity;
}

/**
 * Manages callbacks that are connected to when an entity will
 * pick up an item.
 */
export class EntityItemPickupBeforeEventSignal {
    private constructor();
    /**
     * @remarks
     * Adds a callback that will be called when an entity will pick
     * up an item.
     *
     * @worldMutation
     *
     * @earlyExecution
     *
     * @param callback
     * This closure is called with restricted-execution privilege.
     * @returns
     * Closure that is called with restricted-execution privilege.
     */
    subscribe(
        callback: (arg0: EntityItemPickupBeforeEvent) => void,
        options?: EntityItemPickupEventOptions,
    ): (arg0: EntityItemPickupBeforeEvent) => void;
    /**
     * @remarks
     * Removes a callback from being called when an entity will
     * pick up an item.
     *
     * @worldMutation
     *
     * @earlyExecution
     *
     * @param callback
     * This closure is called with restricted-execution privilege.
     */
    unsubscribe(callback: (arg0: EntityItemPickupBeforeEvent) => void): void;
}

/**
 * Loot item condition that checks the entity type of the
 * entity dropping its loot.
 */
// @ts-ignore Class inheritance allowed for native defined classes
export class EntityKilledCondition extends LootItemCondition {
    private constructor();
    /**
     * @remarks
     * The entity type required for this condition to pass.
     * Example: 'minecraft:skeleton'.
     *
     */
    readonly entityType: string;
}

/**
 * Defines the base movement speed in lava of this entity.
 */
// @ts-ignore Class inheritance allowed for native defined classes
export class EntityLavaMovementComponent extends EntityAttributeComponent {
    private constructor();
    static readonly componentId = 'minecraft:lava_movement';
}

/**
 * Allows the entity to be leashed. Defines the conditions and
 * events for when an entity is leashed.
 */
// @ts-ignore Class inheritance allowed for native defined classes
export class EntityLeashableComponent extends EntityComponent {
    private constructor();
    /**
     * @remarks
     * Returns true if another entity can 'steal' the leashed
     * entity by attaching their own leash to it.
     *
     * @throws This property can throw when used.
     */
    readonly canBeStolen: boolean;
    /**
     * @remarks
     * Distance in blocks at which the leash stiffens, restricting
     * movement.
     *
     * @throws This property can throw when used.
     */
    readonly hardDistance: number;
    /**
     * @remarks
     * Returns true if the entity is leashed.
     *
     * @throws This property can throw when used.
     */
    readonly isLeashed: boolean;
    /**
     * @remarks
     * Entity that is holding the leash.
     *
     * @throws This property can throw when used.
     */
    readonly leashHolder?: Entity;
    /**
     * @remarks
     * Identifier of entity that is holding the leash.
     *
     * @throws This property can throw when used.
     */
    readonly leashHolderEntityId?: string;
    /**
     * @remarks
     * Distance in blocks at which the leash breaks.
     *
     * @throws This property can throw when used.
     */
    readonly maxDistance: number;
    /**
     * @remarks
     * Distance in blocks at which the 'spring' effect starts
     * acting to keep this entity close to the entity that leashed
     * it.
     *
     * @throws This property can throw when used.
     */
    readonly softDistance: number;
    static readonly componentId = 'minecraft:leashable';
    /**
     * @remarks
     * Leashes this entity to another entity.
     *
     * @worldMutation
     *
     * @param leashHolder
     * The entity to leash this entity to.
     * @throws
     * Throws if the entity to leash to is over the max distance,
     * and if the player is dead or in spectator mode.
     */
    leashTo(leashHolder: Entity): void;
    /**
     * @remarks
     * Unleashes this entity if it is leashed to another entity.
     *
     * @worldMutation
     *
     * @throws This function can throw errors.
     */
    unleash(): void;
}

/**
 * Contains data related to an entity loaded within the world.
 * This could happen when an unloaded chunk is reloaded, or
 * when an entity changes dimensions.
 */
export class EntityLoadAfterEvent {
    private constructor();
    /**
     * @remarks
     * Entity that was loaded.
     *
     * @worldMutation
     *
     */
    entity: Entity;
}

/**
 * Registers a script-based event handler for handling what
 * happens when an entity loads.
 */
export class EntityLoadAfterEventSignal {
    private constructor();
    /**
     * @remarks
     * Method to register an event handler for what happens when an
     * entity loads.
     *
     * @worldMutation
     *
     * @earlyExecution
     *
     * @param callback
     * Function that handles the load event.
     */
    subscribe(callback: (arg0: EntityLoadAfterEvent) => void): (arg0: EntityLoadAfterEvent) => void;
    /**
     * @remarks
     * Unregisters a method that was previously subscribed to the
     * subscription event.
     *
     * @worldMutation
     *
     * @earlyExecution
     *
     * @param callback
     * Original function that was passed into the subscribe event,
     * that is to be unregistered.
     */
    unsubscribe(callback: (arg0: EntityLoadAfterEvent) => void): void;
}

/**
 * Additional variant value. Can be used to further
 * differentiate variants.
 */
// @ts-ignore Class inheritance allowed for native defined classes
export class EntityMarkVariantComponent extends EntityComponent {
    private constructor();
    /**
     * @remarks
     * Value of the mark variant value for this entity.
     *
     * @throws This property can throw when used.
     */
    readonly value: number;
    static readonly componentId = 'minecraft:mark_variant';
}

/**
 * When added, this movement control allows the mob to swim in
 * water and walk on land.
 */
// @ts-ignore Class inheritance allowed for native defined classes
export class EntityMovementAmphibiousComponent extends EntityBaseMovementComponent {
    private constructor();
    static readonly componentId = 'minecraft:movement.amphibious';
}

/**
 * This component accents the movement of an entity.
 */
// @ts-ignore Class inheritance allowed for native defined classes
export class EntityMovementBasicComponent extends EntityBaseMovementComponent {
    private constructor();
    static readonly componentId = 'minecraft:movement.basic';
}

/**
 * Defines the general movement speed of this entity.
 */
// @ts-ignore Class inheritance allowed for native defined classes
export class EntityMovementComponent extends EntityAttributeComponent {
    private constructor();
    static readonly componentId = 'minecraft:movement';
}

/**
 * When added, this move control causes the mob to fly.
 */
// @ts-ignore Class inheritance allowed for native defined classes
export class EntityMovementFlyComponent extends EntityBaseMovementComponent {
    private constructor();
    static readonly componentId = 'minecraft:movement.fly';
}

/**
 * When added, this move control allows a mob to fly, swim,
 * climb, etc.
 */
// @ts-ignore Class inheritance allowed for native defined classes
export class EntityMovementGenericComponent extends EntityBaseMovementComponent {
    private constructor();
    static readonly componentId = 'minecraft:movement.generic';
}

/**
 * When added, this movement control allows the mob to glide.
 */
// @ts-ignore Class inheritance allowed for native defined classes
export class EntityMovementGlideComponent extends EntityBaseMovementComponent {
    private constructor();
    /**
     * @remarks
     * Speed in effect when the entity is turning.
     *
     * @throws This property can throw when used.
     */
    readonly speedWhenTurning: number;
    /**
     * @remarks
     * Start speed during a glide.
     *
     * @throws This property can throw when used.
     */
    readonly startSpeed: number;
    static readonly componentId = 'minecraft:movement.glide';
}

/**
 * When added, this move control causes the mob to hover.
 */
// @ts-ignore Class inheritance allowed for native defined classes
export class EntityMovementHoverComponent extends EntityBaseMovementComponent {
    private constructor();
    static readonly componentId = 'minecraft:movement.hover';
}

/**
 * Move control that causes the mob to jump as it moves with a
 * specified delay between jumps.
 */
// @ts-ignore Class inheritance allowed for native defined classes
export class EntityMovementJumpComponent extends EntityBaseMovementComponent {
    private constructor();
    static readonly componentId = 'minecraft:movement.jump';
}

/**
 * When added, this move control causes the mob to hop as it
 * moves.
 */
// @ts-ignore Class inheritance allowed for native defined classes
export class EntityMovementSkipComponent extends EntityBaseMovementComponent {
    private constructor();
    static readonly componentId = 'minecraft:movement.skip';
}

/**
 * When added, this move control causes the mob to sway side to
 * side giving the impression it is swimming.
 */
// @ts-ignore Class inheritance allowed for native defined classes
export class EntityMovementSwayComponent extends EntityBaseMovementComponent {
    private constructor();
    /**
     * @remarks
     * Amplitude of the sway motion.
     *
     * @throws This property can throw when used.
     */
    readonly swayAmplitude: number;
    /**
     * @remarks
     * Amount of sway frequency.
     *
     * @throws This property can throw when used.
     */
    readonly swayFrequency: number;
    static readonly componentId = 'minecraft:movement.sway';
}

/**
 * Allows this entity to generate paths that include vertical
 * walls (for example, like Minecraft spiders do.)
 */
// @ts-ignore Class inheritance allowed for native defined classes
export class EntityNavigationClimbComponent extends EntityNavigationComponent {
    private constructor();
    static readonly componentId = 'minecraft:navigation.climb';
}

/**
 * Allows this entity to generate paths that include vertical
 * walls (for example, like Minecraft spiders do.)
 */
// @ts-ignore Class inheritance allowed for native defined classes
export class EntityNavigationComponent extends EntityComponent {
    private constructor();
    /**
     * @remarks
     * Tells the pathfinder to avoid blocks that cause damage when
     * finding a path.
     *
     * @throws This property can throw when used.
     */
    readonly avoidDamageBlocks: boolean;
    /**
     * @remarks
     * Tells the pathfinder to avoid portals (like nether portals)
     * when finding a path.
     *
     * @throws This property can throw when used.
     */
    readonly avoidPortals: boolean;
    /**
     * @remarks
     * Whether or not the pathfinder should avoid tiles that are
     * exposed to the sun when creating paths.
     *
     * @throws This property can throw when used.
     */
    readonly avoidSun: boolean;
    /**
     * @remarks
     * Tells the pathfinder to avoid water when creating a path.
     *
     * @throws This property can throw when used.
     */
    readonly avoidWater: boolean;
    /**
     * @remarks
     * Tells the pathfinder whether or not it can jump out of water
     * (like a dolphin).
     *
     * @throws This property can throw when used.
     */
    readonly canBreach: boolean;
    /**
     * @remarks
     * Tells the pathfinder that it can path through a closed door
     * and break it.
     *
     * @throws This property can throw when used.
     */
    readonly canBreakDoors: boolean;
    /**
     * @remarks
     * Tells the pathfinder whether or not it can float.
     *
     * @throws This property can throw when used.
     */
    readonly canFloat: boolean;
    /**
     * @remarks
     * Tells the pathfinder whether or not it can jump up blocks.
     *
     * @throws This property can throw when used.
     */
    readonly canJump: boolean;
    /**
     * @remarks
     * Tells the pathfinder that it can path through a closed door
     * assuming the AI will open the door.
     *
     * @throws This property can throw when used.
     */
    readonly canOpenDoors: boolean;
    /**
     * @remarks
     * Tells the pathfinder that it can path through a closed iron
     * door assuming the AI will open the door.
     *
     * @throws This property can throw when used.
     */
    readonly canOpenIronDoors: boolean;
    /**
     * @remarks
     * Whether a path can be created through a door.
     *
     * @throws This property can throw when used.
     */
    readonly canPassDoors: boolean;
    /**
     * @remarks
     * Tells the pathfinder that it can start pathing when in the
     * air.
     *
     * @throws This property can throw when used.
     */
    readonly canPathFromAir: boolean;
    /**
     * @remarks
     * Tells the pathfinder whether or not it can travel on the
     * surface of the lava.
     *
     * @throws This property can throw when used.
     */
    readonly canPathOverLava: boolean;
    /**
     * @remarks
     * Tells the pathfinder whether or not it can travel on the
     * surface of the water.
     *
     * @throws This property can throw when used.
     */
    readonly canPathOverWater: boolean;
    /**
     * @remarks
     * Tells the pathfinder whether or not it will be pulled down
     * by gravity while in water.
     *
     * @throws This property can throw when used.
     */
    readonly canSink: boolean;
    /**
     * @remarks
     * Tells the pathfinder whether or not it can path anywhere
     * through water and plays swimming animation along that path.
     *
     * @throws This property can throw when used.
     */
    readonly canSwim: boolean;
    /**
     * @remarks
     * Tells the pathfinder whether or not it can walk on the
     * ground outside water.
     *
     * @throws This property can throw when used.
     */
    readonly canWalk: boolean;
    /**
     * @remarks
     * Tells the pathfinder whether or not it can travel in lava
     * like walking on ground.
     *
     * @throws This property can throw when used.
     */
    readonly canWalkInLava: boolean;
    /**
     * @remarks
     * Tells the pathfinder whether or not it can walk on the
     * ground or go underwater.
     *
     * @throws This property can throw when used.
     */
    readonly isAmphibious: boolean;
}

/**
 * Allows this entity to generate paths by flying around the
 * air like the regular Ghast.
 */
// @ts-ignore Class inheritance allowed for native defined classes
export class EntityNavigationFloatComponent extends EntityNavigationComponent {
    private constructor();
    static readonly componentId = 'minecraft:navigation.float';
}

/**
 * Allows this entity to generate paths in the air (for
 * example, like Minecraft parrots do.)
 */
// @ts-ignore Class inheritance allowed for native defined classes
export class EntityNavigationFlyComponent extends EntityNavigationComponent {
    private constructor();
    static readonly componentId = 'minecraft:navigation.fly';
}

/**
 * Allows this entity to generate paths by walking, swimming,
 * flying and/or climbing around and jumping up and down a
 * block.
 */
// @ts-ignore Class inheritance allowed for native defined classes
export class EntityNavigationGenericComponent extends EntityNavigationComponent {
    private constructor();
    static readonly componentId = 'minecraft:navigation.generic';
}

/**
 * Allows this entity to generate paths in the air (for
 * example, like the Minecraft Bees do.) Keeps them from
 * falling out of the skies and doing predictive movement.
 */
// @ts-ignore Class inheritance allowed for native defined classes
export class EntityNavigationHoverComponent extends EntityNavigationComponent {
    private constructor();
    static readonly componentId = 'minecraft:navigation.hover';
}

/**
 * Allows this entity to generate paths by walking around and
 * jumping up and down a block like regular mobs.
 */
// @ts-ignore Class inheritance allowed for native defined classes
export class EntityNavigationWalkComponent extends EntityNavigationComponent {
    private constructor();
    static readonly componentId = 'minecraft:navigation.walk';
}

/**
 * @beta
 * Adds NPC capabilities to an entity such as custom skin,
 * name, and dialogue interactions.
 */
// @ts-ignore Class inheritance allowed for native defined classes
export class EntityNpcComponent extends EntityComponent {
    private constructor();
    /**
     * @remarks
     * The DialogueScene that is opened when players first interact
     * with the NPC.
     *
     * @worldMutation
     *
     */
    defaultScene: string;
    /**
     * @remarks
     * The name of the NPC as it is displayed to players.
     *
     * @worldMutation
     *
     */
    name: string;
    /**
     * @remarks
     * The index of the skin the NPC will use.
     *
     * @worldMutation
     *
     */
    skinIndex: number;
    static readonly componentId = 'minecraft:npc';
}

/**
 * When present on an entity, this entity is on fire.
 * @seeExample setOnFire.ts
 */
// @ts-ignore Class inheritance allowed for native defined classes
export class EntityOnFireComponent extends EntityComponent {
    private constructor();
    /**
     * @remarks
     * The number of ticks remaining before the fire goes out.
     *
     */
    readonly onFireTicksRemaining: number;
    static readonly componentId = 'minecraft:onfire';
}

/**
 * The projectile component controls the properties of a
 * projectile entity and allows it to be shot in a given
 * direction.
 * This component is present when the entity has the
 * minecraft:projectile component.
 * @seeExample shootArrow.ts
 */
// @ts-ignore Class inheritance allowed for native defined classes
export class EntityProjectileComponent extends EntityComponent {
    private constructor();
    /**
     * @remarks
     * The fraction of the projectile's speed maintained every tick
     * while traveling through air.
     *
     * @worldMutation
     *
     */
    airInertia: number;
    /**
     * @remarks
     * If true, the entity will be set on fire when hurt. The
     * default burn duration is 5 seconds. This duration can be
     * modified via the onFireTime property. The entity will not
     * catch fire if immune or if the entity is wet.
     *
     * @worldMutation
     *
     */
    catchFireOnHurt: boolean;
    /**
     * @remarks
     * If true, the projectile will spawn crit particles when hit
     * by a player. E.g. Player attacking a Shulker bullet.
     *
     * @worldMutation
     *
     */
    critParticlesOnProjectileHurt: boolean;
    /**
     * @remarks
     * If true, the projectile will be destroyed when it takes
     * damage. E.g. Player attacking a Shulker bullet.
     *
     * @worldMutation
     *
     */
    destroyOnProjectileHurt: boolean;
    /**
     * @remarks
     * The gravity applied to the projectile. When the entity is
     * not on the ground, subtracts this amount from the
     * projectile’s change in vertical position every tick. The
     * higher the value, the faster the projectile falls. If
     * negative, the entity will rise instead of fall.
     *
     * @worldMutation
     *
     */
    gravity: number;
    /**
     * @remarks
     * The sound that plays when the projectile hits an entity.
     *
     * @worldMutation
     *
     */
    hitEntitySound?: string;
    /**
     * @remarks
     * The sound that plays when the projectile hits a block.
     *
     * @worldMutation
     *
     */
    hitGroundSound?: string;
    /**
     * @remarks
     * The particle that spawns when the projectile hits something.
     *
     * @worldMutation
     *
     */
    hitParticle?: string;
    /**
     * @remarks
     * If true and the weather is thunder and the entity has line
     * of sight to the sky, the entity will be struck by lightning
     * when hit. E.g. A thrown Trident with the Channeling
     * enchantment.
     *
     * @worldMutation
     *
     */
    lightningStrikeOnHit: boolean;
    /**
     * @remarks
     * The fraction of the projectile's speed maintained every tick
     * while traveling through a liquid.
     *
     * @worldMutation
     *
     */
    liquidInertia: number;
    /**
     * @remarks
     * Duration in seconds that the entity hit will be on fire for
     * when catchFireOnHurt is set to true.
     *
     * @worldMutation
     *
     */
    onFireTime: number;
    /**
     * @remarks
     * The owner of the projectile. This is used to determine what
     * the projectile can collide with and damage. It also
     * determines which entity is assigned as the attacker.
     *
     * @worldMutation
     *
     */
    owner?: Entity;
    /**
     * @remarks
     * If true, the projectile will bounce off mobs when no damage
     * is taken. E.g. A spawning wither.
     *
     * @worldMutation
     *
     */
    shouldBounceOnHit: boolean;
    /**
     * @remarks
     * If true, the projectile will stop moving when an entity is
     * hit as thought it had been blocked. E.g. Thrown trident on
     * hit behavior.
     *
     * @worldMutation
     *
     */
    stopOnHit: boolean;
    static readonly componentId = 'minecraft:projectile';
    /**
     * @remarks
     * Shoots the projectile with a given velocity. The projectile
     * will be shot from its current location.
     *
     * @worldMutation
     *
     * @param velocity
     * The velocity to fire the projectile. This controls both the
     * speed and direction which which the projectile will be shot.
     * @param options
     * Optional configuration for the shoot.
     * @throws
     * Throws if the component or entity no longer exist.
     */
    shoot(velocity: Vector3, options?: ProjectileShootOptions): void;
}

/**
 * Sets the distance through which the entity can push through.
 */
// @ts-ignore Class inheritance allowed for native defined classes
export class EntityPushThroughComponent extends EntityComponent {
    private constructor();
    /**
     * @remarks
     * Value of the push through distances of this entity.
     *
     * @throws This property can throw when used.
     */
    readonly value: number;
    static readonly componentId = 'minecraft:push_through';
}

/**
 * Data for an event that happens when an entity is removed
 * from the world (for example, the entity is unloaded because
 * it is not close to players.)
 */
export class EntityRemoveAfterEvent {
    private constructor();
    /**
     * @remarks
     * Id of the entity that was removed.
     *
     */
    readonly removedEntityId: string;
    /**
     * @remarks
     * Identifier of the type of the entity removed - for example,
     * 'minecraft:skeleton'.
     *
     */
    readonly typeId: string;
}

/**
 * Allows registration for an event that fires when an entity
 * is removed from  the game (for example, unloaded, or a few
 * seconds after they are dead.)
 */
export class EntityRemoveAfterEventSignal {
    private constructor();
    /**
     * @remarks
     * Will call your function every time an entity is removed from
     * the game.
     *
     * @worldMutation
     *
     * @earlyExecution
     *
     * @param callback
     * Function to call.
     * @param options
     * Additional filtering options for this event.
     * @returns
     * Returns a closure that can be used in subsequent unsubscribe
     * operations.
     */
    subscribe(
        callback: (arg0: EntityRemoveAfterEvent) => void,
        options?: EntityEventOptions,
    ): (arg0: EntityRemoveAfterEvent) => void;
    /**
     * @remarks
     * Unsubscribes your function from subsequent calls when an
     * entity is removed.
     *
     * @worldMutation
     *
     * @earlyExecution
     *
     */
    unsubscribe(callback: (arg0: EntityRemoveAfterEvent) => void): void;
}

/**
 * Data for an event that happens when an entity is being
 * removed from the world (for example, the entity is unloaded
 * because it is not close to players.)
 */
export class EntityRemoveBeforeEvent {
    private constructor();
    /**
     * @remarks
     * Reference to an entity that is being removed.
     *
     */
    readonly removedEntity: Entity;
}

/**
 * Allows registration for an event that fires when an entity
 * is being removed from  the game (for example, unloaded, or a
 * few seconds after they are dead.)
 */
export class EntityRemoveBeforeEventSignal {
    private constructor();
    /**
     * @remarks
     * Will call your function every time an entity is being
     * removed from the game.
     *
     * @worldMutation
     *
     * @earlyExecution
     *
     * @param callback
     * Function to call.
     * This closure is called with restricted-execution privilege.
     * @returns
     * Returns a closure that can be used in subsequent unsubscribe
     * operations.
     * Closure that is called with restricted-execution privilege.
     */
    subscribe(callback: (arg0: EntityRemoveBeforeEvent) => void): (arg0: EntityRemoveBeforeEvent) => void;
    /**
     * @remarks
     * Unsubscribes your function from subsequent calls when an
     * entity is being removed.
     *
     * @worldMutation
     *
     * @earlyExecution
     *
     * @param callback
     * This closure is called with restricted-execution privilege.
     */
    unsubscribe(callback: (arg0: EntityRemoveBeforeEvent) => void): void;
}

/**
 * When added, this component adds the capability that an
 * entity can be ridden by another entity.
 * @seeExample minibiomes.ts
 */
// @ts-ignore Class inheritance allowed for native defined classes
export class EntityRideableComponent extends EntityComponent {
    private constructor();
    /**
     * @remarks
     * Zero-based index of the seat that can used to control this
     * entity.
     *
     * @throws This property can throw when used.
     */
    readonly controllingSeat: number;
    /**
     * @remarks
     * Determines whether interactions are not supported if the
     * entity is crouching.
     *
     * @throws This property can throw when used.
     */
    readonly crouchingSkipInteract: boolean;
    /**
     * @remarks
     * Set of text that should be displayed when a player is
     * looking to ride on this entity (commonly with touch-screen
     * controls).
     *
     * @throws This property can throw when used.
     */
    readonly interactText: string;
    /**
     * @remarks
     * The max width a mob can be to be a passenger.
     *
     * @throws This property can throw when used.
     */
    readonly passengerMaxWidth: number;
    /**
     * @remarks
     * If true, this entity will pull in entities that are in the
     * correct family_types into any available seat.
     *
     * @throws This property can throw when used.
     */
    readonly pullInEntities: boolean;
    /**
     * @remarks
     * If true, this entity will be picked when looked at by the
     * rider.
     *
     * @throws This property can throw when used.
     */
    readonly riderCanInteract: boolean;
    /**
     * @remarks
     * Number of seats for riders defined for this entity.
     *
     * @throws This property can throw when used.
     */
    readonly seatCount: number;
    static readonly componentId = 'minecraft:rideable';
    /**
     * @remarks
     * Adds an entity to this entity as a rider.
     *
     * @worldMutation
     *
     * @param rider
     * Entity that will become the rider of this entity.
     * @returns
     * True if the rider entity was successfully added.
     * @throws This function can throw errors.
     * @seeExample minibiomes.ts
     */
    addRider(rider: Entity): boolean;
    /**
     * @remarks
     * Ejects the specified rider of this entity.
     *
     * @worldMutation
     *
     * @param rider
     * Entity that should be ejected from this entity.
     * @throws This function can throw errors.
     */
    ejectRider(rider: Entity): void;
    /**
     * @remarks
     * Ejects all riders of this entity.
     *
     * @worldMutation
     *
     * @throws This function can throw errors.
     */
    ejectRiders(): void;
    /**
     * @remarks
     * A string-list of entity types that this entity can support
     * as riders.
     *
     * @throws This function can throw errors.
     */
    getFamilyTypes(): string[];
    /**
     * @remarks
     * Gets a list of the all the entities currently riding this
     * entity.
     *
     * @throws This function can throw errors.
     */
    getRiders(): Entity[];
    /**
     * @remarks
     * Gets a list of positions and number of riders for each
     * position for entities riding this entity.
     *
     * @throws This function can throw errors.
     */
    getSeats(): Seat[];
}

/**
 * This component is added to any entity when it is riding
 * another entity.
 */
// @ts-ignore Class inheritance allowed for native defined classes
export class EntityRidingComponent extends EntityComponent {
    private constructor();
    /**
     * @remarks
     * The entity this entity is currently riding on.
     *
     * @throws This property can throw when used.
     */
    readonly entityRidingOn: Entity;
    static readonly componentId = 'minecraft:riding';
}

/**
 * Defines the interactions with this entity for Saturation.
 * Wraps the `minecraft.player.saturation` attribute.
 */
// @ts-ignore Class inheritance allowed for native defined classes
export class EntitySaturationComponent extends EntityAttributeComponent {
    private constructor();
    static readonly componentId = 'minecraft:player.saturation';
}

/**
 * Sets the entity's visual size.
 */
// @ts-ignore Class inheritance allowed for native defined classes
export class EntityScaleComponent extends EntityComponent {
    private constructor();
    /**
     * @remarks
     * Current value for the scale property set on entities.
     *
     * @throws This property can throw when used.
     */
    readonly value: number;
    static readonly componentId = 'minecraft:scale';
}

/**
 * Skin Id value. Can be used to differentiate skins, such as
 * base skins for villagers.
 */
// @ts-ignore Class inheritance allowed for native defined classes
export class EntitySkinIdComponent extends EntityComponent {
    private constructor();
    /**
     * @remarks
     * Returns the value of the skin Id identifier of the entity.
     *
     * @throws This property can throw when used.
     */
    readonly value: number;
    static readonly componentId = 'minecraft:skin_id';
}

/**
 * Contains data related to an entity spawning within the
 * world.
 * @seeExample logEntitySpawnEvent.ts
 */
export class EntitySpawnAfterEvent {
    private constructor();
    /**
     * @remarks
     * Initialization cause (Spawned, Born ...).
     *
     */
    readonly cause: EntityInitializationCause;
    /**
     * @remarks
     * Entity that was spawned.
     *
     * @worldMutation
     *
     */
    entity: Entity;
}

/**
 * Registers a script-based event handler for handling what
 * happens when an entity spawns.
 */
export class EntitySpawnAfterEventSignal {
    private constructor();
    /**
     * @remarks
     * Method to register an event handler for what happens when an
     * entity spawns.
     *
     * @worldMutation
     *
     * @earlyExecution
     *
     * @param callback
     * Function that handles the spawn event.
     * @seeExample logEntitySpawnEvent.ts
     */
    subscribe(callback: (arg0: EntitySpawnAfterEvent) => void): (arg0: EntitySpawnAfterEvent) => void;
    /**
     * @remarks
     * Unregisters a method that was previously subscribed to the
     * subscription event.
     *
     * @worldMutation
     *
     * @earlyExecution
     *
     * @param callback
     * Original function that was passed into the subscribe event,
     * that is to be unregistered.
     */
    unsubscribe(callback: (arg0: EntitySpawnAfterEvent) => void): void;
}

/**
 * @beta
 * Contains data related to an entity beginning to sneak.
 */
export class EntityStartSneakingAfterEvent {
    private constructor();
    /**
     * @remarks
     * Entity that has started sneaking.
     *
     */
    readonly entity: Entity;
}

/**
 * @beta
 * Manages callbacks that are connected to when an entity
 * begins sneaking.
 */
export class EntityStartSneakingAfterEventSignal {
    private constructor();
    /**
     * @remarks
     * Adds a callback that will be called when an entity begins
     * sneaking.
     *
     * @worldMutation
     *
     * @earlyExecution
     *
     */
    subscribe(
        callback: (arg0: EntityStartSneakingAfterEvent) => void,
        options?: EntitySneakingChangedEventOptions,
    ): (arg0: EntityStartSneakingAfterEvent) => void;
    /**
     * @remarks
     * Removes a callback from being called when an entity begins
     * sneaking.
     *
     * @worldMutation
     *
     * @earlyExecution
     *
     */
    unsubscribe(callback: (arg0: EntityStartSneakingAfterEvent) => void): void;
}

/**
 * @beta
 * Contains data related to an entity stopping sneaking.
 */
export class EntityStopSneakingAfterEvent {
    private constructor();
    /**
     * @remarks
     * Entity that has stopped sneaking.
     *
     */
    readonly entity: Entity;
}

/**
 * @beta
 * Manages callbacks that are connected to when an entity stops
 * sneaking.
 */
export class EntityStopSneakingAfterEventSignal {
    private constructor();
    /**
     * @remarks
     * Adds a callback that will be called when an entity stops
     * sneaking.
     *
     * @worldMutation
     *
     * @earlyExecution
     *
     */
    subscribe(
        callback: (arg0: EntityStopSneakingAfterEvent) => void,
        options?: EntitySneakingChangedEventOptions,
    ): (arg0: EntityStopSneakingAfterEvent) => void;
    /**
     * @remarks
     * Removes a callback from being called when an entity stops
     * sneaking.
     *
     * @worldMutation
     *
     * @earlyExecution
     *
     */
    unsubscribe(callback: (arg0: EntityStopSneakingAfterEvent) => void): void;
}

/**
 * Defines the entity's ability to carry items. An entity with
 * a higher strength would have higher potential carry capacity
 * and more item slots.
 */
// @ts-ignore Class inheritance allowed for native defined classes
export class EntityStrengthComponent extends EntityComponent {
    private constructor();
    /**
     * @remarks
     * Maximum strength of this entity, as defined in the entity
     * type definition.
     *
     * @throws This property can throw when used.
     */
    readonly max: number;
    /**
     * @remarks
     * Current value of the strength component that has been set
     * for entities.
     *
     * @throws This property can throw when used.
     */
    readonly value: number;
    static readonly componentId = 'minecraft:strength';
}

/**
 * Defines the rules for an entity to be tamed by the player.
 */
// @ts-ignore Class inheritance allowed for native defined classes
export class EntityTameableComponent extends EntityComponent {
    private constructor();
    /**
     * @remarks
     * Returns a set of items that can be used to tame this entity.
     *
     * @throws This property can throw when used.
     */
    readonly getTameItems: ItemStack[];
    /**
     * @remarks
     * Returns true if the entity is tamed by player.
     *
     * @throws This property can throw when used.
     */
    readonly isTamed: boolean;
    /**
     * @remarks
     * The chance of taming the entity with each item use between
     * 0.0 and 1.0, where 1.0 is 100%
     *
     * @throws This property can throw when used.
     */
    readonly probability: number;
    /**
     * @remarks
     * Returns the player that has tamed the entity, or 'undefined'
     * if entity is not tamed.
     *
     * @throws This property can throw when used.
     */
    readonly tamedToPlayer?: Player;
    /**
     * @remarks
     * Returns the id of player that has tamed the entity, or
     * 'undefined' if entity is not tamed.
     *
     * @throws This property can throw when used.
     */
    readonly tamedToPlayerId?: string;
    static readonly componentId = 'minecraft:tameable';
    /**
     * @remarks
     * Set this entity as tamed by the given player.
     *
     * @worldMutation
     *
     * @param player
     * The player that this entity should be tamed by.
     * @returns
     * Returns true if the entity was tamed.
     * @throws This function can throw errors.
     */
    tame(player: Player): boolean;
}

/**
 * @beta
 * Contains data related to an entity being tamed.
 */
export class EntityTamedAfterEvent {
    private constructor();
    readonly entity: Entity;
    readonly tamingEntity: Entity;
}

/**
 * @beta
 * Manages callbacks that are connected to when an entity is
 * tamed.
 */
export class EntityTamedAfterEventSignal {
    private constructor();
    /**
     * @remarks
     * @worldMutation
     *
     * @earlyExecution
     *
     */
    subscribe(
        callback: (arg0: EntityTamedAfterEvent) => void,
        options?: EntityTamedEventFilter,
    ): (arg0: EntityTamedAfterEvent) => void;
    /**
     * @remarks
     * @worldMutation
     *
     * @earlyExecution
     *
     */
    unsubscribe(callback: (arg0: EntityTamedAfterEvent) => void): void;
}

/**
 * @beta
 * Contains information regarding an event before an entity is
 * tamed.
 */
export class EntityTamedBeforeEvent {
    private constructor();
    /**
     * @remarks
     * When set to true will cancel the event.
     *
     */
    cancel: boolean;
    /**
     * @remarks
     * The entity that is being tamed.
     *
     */
    readonly entity: Entity;
    /**
     * @remarks
     * The entity that is attempting to tame the entity.
     *
     */
    readonly tamingEntity: Entity;
}

/**
 * @beta
 * Manages callbacks that are connected to before an entity is
 * tamed.
 */
export class EntityTamedBeforeEventSignal {
    private constructor();
    /**
     * @remarks
     * @worldMutation
     *
     * @earlyExecution
     *
     * @param callback
     * This closure is called with restricted-execution privilege.
     * @returns
     * Closure that is called with restricted-execution privilege.
     */
    subscribe(
        callback: (arg0: EntityTamedBeforeEvent) => void,
        options?: EntityTamedEventFilter,
    ): (arg0: EntityTamedBeforeEvent) => void;
    /**
     * @remarks
     * @worldMutation
     *
     * @earlyExecution
     *
     * @param callback
     * This closure is called with restricted-execution privilege.
     */
    unsubscribe(callback: (arg0: EntityTamedBeforeEvent) => void): void;
}

/**
 * Contains options for taming a rideable entity based on the
 * entity that mounts it.
 */
// @ts-ignore Class inheritance allowed for native defined classes
export class EntityTameMountComponent extends EntityComponent {
    private constructor();
    /**
     * @remarks
     * Returns true if the entity is tamed.
     *
     * @throws This property can throw when used.
     */
    readonly isTamed: boolean;
    /**
     * @remarks
     * Returns true if the entity is tamed by a player.
     *
     * @throws This property can throw when used.
     */
    readonly isTamedToPlayer: boolean;
    /**
     * @remarks
     * Returns the player that has tamed the entity, or 'undefined'
     * if entity is not tamed by a player.
     *
     * @throws This property can throw when used.
     */
    readonly tamedToPlayer?: Player;
    /**
     * @remarks
     * Returns the id of player that has tamed the entity, or
     * 'undefined' if entity is not tamed.
     *
     * @throws This property can throw when used.
     */
    readonly tamedToPlayerId?: string;
    static readonly componentId = 'minecraft:tamemount';
    /**
     * @remarks
     * Sets this rideable entity as tamed.
     *
     * @worldMutation
     *
     * @param showParticles
     * Whether to show effect particles when this entity is tamed.
     * @throws This function can throw errors.
     */
    tame(showParticles: boolean): void;
    /**
     * @remarks
     * Sets this rideable entity as tamed by the given player.
     *
     * @worldMutation
     *
     * @param showParticles
     * Whether to show effect particles when this entity is tamed.
     * @param player
     * The player that this entity should be tamed by.
     * @returns
     * Returns true if the entity was tamed.
     * @throws This function can throw errors.
     */
    tameToPlayer(showParticles: boolean, player: Player): boolean;
}

/**
 * Represents information about a type of entity.
 */
export class EntityType {
    private constructor();
    /**
     * @remarks
     * Identifier of this entity type - for example,
     * 'minecraft:skeleton'.
     *
     */
    readonly id: string;
    /**
     * @remarks
     * Key for the localization of this EntityType's name used in
     * .lang files.
     *
     */
    readonly localizationKey: string;
}

// @ts-ignore Class inheritance allowed for native defined classes
export class EntityTypeFamilyComponent extends EntityComponent {
    private constructor();
    static readonly componentId = 'minecraft:type_family';
    /**
     * @throws This function can throw errors.
     */
    getTypeFamilies(): string[];
    /**
     * @throws This function can throw errors.
     */
    hasTypeFamily(typeFamily: string): boolean;
}

/**
 * Used for accessing all entity types currently available for
 * use within the world.
 */
export class EntityTypes {
    private constructor();
    /**
     * @remarks
     * Retrieves an entity type using a string-based identifier.
     *
     */
    static get<T = never>(identifier: EntityIdentifierType<NoInfer<T>>): EntityType | undefined;
    /**
     * @remarks
     * Retrieves a set of all entity types within this world.
     *
     */
    static getAll(): EntityType[];
}

/**
 * Defines the general movement speed underwater of this
 * entity.
 */
// @ts-ignore Class inheritance allowed for native defined classes
export class EntityUnderwaterMovementComponent extends EntityAttributeComponent {
    private constructor();
    static readonly componentId = 'minecraft:underwater_movement';
}

/**
 * Contains information related to firing of a data driven
 * entity version upgrade.
 */
export class EntityUpgradeAfterEvent {
    private constructor();
    /**
     * @remarks
     * Entity that the upgrade triggered on.
     *
     */
    readonly entity: Entity;
    /**
     * @remarks
     * Name of the data driven upgrade being triggered.
     *
     */
    readonly upgradeId: string;
    /**
     * @remarks
     * An updateable list of modifications to component state that
     * are the effect of this triggered upgrade.
     *
     */
    getModifiers(): DefinitionModifier[];
}

/**
 * Contains event registration related to firing of a data
 * driven entity version upgrade.
 */
export class EntityUpgradeAfterEventSignal {
    private constructor();
    /**
     * @remarks
     * Adds a callback that will be called after a data driven
     * entity version upgrade is triggered.
     *
     * @worldMutation
     *
     * @earlyExecution
     *
     */
    subscribe(
        callback: (arg0: EntityUpgradeAfterEvent) => void,
        options?: EntityDataDrivenTriggerEventOptions,
    ): (arg0: EntityUpgradeAfterEvent) => void;
    /**
     * @remarks
     * Removes a callback that will be called after a data driven
     * entity version upgrade is triggered.
     *
     * @worldMutation
     *
     * @earlyExecution
     *
     */
    unsubscribe(callback: (arg0: EntityUpgradeAfterEvent) => void): void;
}

/**
 * Used to differentiate the component group of a variant of an
 * entity from others. (e.g. ocelot, villager).
 */
// @ts-ignore Class inheritance allowed for native defined classes
export class EntityVariantComponent extends EntityComponent {
    private constructor();
    /**
     * @remarks
     * Current value for variant for this entity, as specified via
     * components.
     *
     * @throws This property can throw when used.
     */
    readonly value: number;
    static readonly componentId = 'minecraft:variant';
}

/**
 * When added, this component signifies that this entity wants
 * to become a jockey.
 */
// @ts-ignore Class inheritance allowed for native defined classes
export class EntityWantsJockeyComponent extends EntityComponent {
    private constructor();
    static readonly componentId = 'minecraft:wants_jockey';
}

/**
 * Waypoint that tracks an entity's position. The waypoint
 * automatically updates as the entity moves and becomes
 * invalid when the entity is removed.
 */
// @ts-ignore Class inheritance allowed for native defined classes
export class EntityWaypoint extends Waypoint {
    /**
     * @remarks
     * The entity being tracked by this waypoint.
     *
     * @throws This property can throw when used.
     *
     * {@link InvalidWaypointError}
     *
     * {@link InvalidWaypointTextureSelectorError}
     */
    readonly entity: Entity;
    /**
     * @remarks
     * The visibility rules that control when the waypoint is shown
     * based on the entity's state (e.g., sneaking, invisible,
     * dead).
     *
     * @throws This property can throw when used.
     *
     * {@link InvalidWaypointError}
     *
     * {@link InvalidWaypointTextureSelectorError}
     */
    readonly entityRules: EntityVisibilityRules;
    /**
     * @throws This function can throw errors.
     *
     * {@link InvalidWaypointTextureSelectorError}
     */
    constructor(
        entity: Entity,
        textureSelector: WaypointTextureSelector,
        entityRules: EntityVisibilityRules,
        color?: RGB,
    );
}

/**
 * Loot item function that modifies a dropped treasure map to
 * mark a location.
 */
// @ts-ignore Class inheritance allowed for native defined classes
export class ExplorationMapFunction extends LootItemFunction {
    private constructor();
    /**
     * @remarks
     * Determines which type of treasure map will drop.
     *
     */
    readonly destination: string;
}

/**
 * Contains information regarding an explosion that has
 * happened.
 */
export class ExplosionAfterEvent {
    private constructor();
    /**
     * @remarks
     * Dimension where the explosion has occurred.
     *
     */
    readonly dimension: Dimension;
    /**
     * @remarks
     * Optional source of the explosion.
     *
     */
    readonly source?: Entity;
    /**
     * @remarks
     * A collection of blocks impacted by this explosion event.
     *
     */
    getImpactedBlocks(): Block[];
}

/**
 * Manages callbacks that are connected to when an explosion
 * occurs.
 */
export class ExplosionAfterEventSignal {
    private constructor();
    /**
     * @remarks
     * Adds a callback that will be called when an explosion
     * occurs.
     *
     * @worldMutation
     *
     * @earlyExecution
     *
     */
    subscribe(callback: (arg0: ExplosionAfterEvent) => void): (arg0: ExplosionAfterEvent) => void;
    /**
     * @remarks
     * Removes a callback from being called when an explosion
     * occurs.
     *
     * @worldMutation
     *
     * @earlyExecution
     *
     */
    unsubscribe(callback: (arg0: ExplosionAfterEvent) => void): void;
}

/**
 * Contains information regarding an explosion that has
 * happened.
 */
// @ts-ignore Class inheritance allowed for native defined classes
export class ExplosionBeforeEvent extends ExplosionAfterEvent {
    private constructor();
    /**
     * @remarks
     * If set to true, cancels the explosion event.
     *
     */
    cancel: boolean;
    /**
     * @remarks
     * Updates a collection of blocks impacted by this explosion
     * event.
     *
     * @param blocks
     * New list of blocks that are impacted by this explosion.
     */
    setImpactedBlocks(blocks: Block[]): void;
}

/**
 * Manages callbacks that are connected to before an explosion
 * occurs.
 */
export class ExplosionBeforeEventSignal {
    private constructor();
    /**
     * @remarks
     * Adds a callback that will be called when before an explosion
     * occurs. The callback can optionally change or cancel
     * explosion behavior.
     *
     * @worldMutation
     *
     * @earlyExecution
     *
     * @param callback
     * This closure is called with restricted-execution privilege.
     * @returns
     * Closure that is called with restricted-execution privilege.
     */
    subscribe(callback: (arg0: ExplosionBeforeEvent) => void): (arg0: ExplosionBeforeEvent) => void;
    /**
     * @remarks
     * Removes a callback from being called from before when an
     * explosion would occur.
     *
     * @worldMutation
     *
     * @earlyExecution
     *
     * @param callback
     * This closure is called with restricted-execution privilege.
     */
    unsubscribe(callback: (arg0: ExplosionBeforeEvent) => void): void;
}

/**
 * Loot item function that determines whether or not loot drops
 * should be destroyed by explosions.
 */
// @ts-ignore Class inheritance allowed for native defined classes
export class ExplosionDecayFunction extends LootItemFunction {
    private constructor();
}

/**
 * As part of the Healable component, represents a specific
 * item that can be fed to an entity to cause health effects.
 */
export class FeedItem {
    private constructor();
    /**
     * @remarks
     * The amount of health this entity gains when fed this item.
     * This number is an integer starting at 0. Sample values can
     * go as high as 40.
     *
     */
    readonly healAmount: number;
    /**
     * @remarks
     * Identifier of type of item that can be fed. If a namespace
     * is not specified, 'minecraft:' is assumed. Example values
     * include 'wheat' or 'golden_apple'.
     *
     */
    readonly item: string;
    /**
     * @remarks
     * Type ID of the resulting item after feeding has occurred.
     * This will usually be empty but is used for scenarios such as
     * feeding a Nautilus with a bucket of fish, where the result
     * item will be an empty bucket.
     *
     */
    readonly resultItem?: string;
    /**
     * @remarks
     * As part of the Healable component, an optional collection of
     * side effects that can occur from being fed an item.
     *
     */
    getEffects(): FeedItemEffect[];
}

/**
 * Represents an effect that is applied as a result of a food
 * item being fed to an entity.
 */
export class FeedItemEffect {
    private constructor();
    /**
     * @remarks
     * Gets an amplifier that may have been applied to this effect.
     * Valid values are integers starting at 0 and up - but usually
     * ranging between 0 and 4.
     *
     */
    readonly amplifier: number;
    /**
     * @remarks
     * Chance that this effect is applied as a result of the entity
     * being fed this item. Valid values range between 0 and 1.
     *
     */
    readonly chance: number;
    /**
     * @remarks
     * Gets the duration, in ticks, of this effect.
     *
     */
    readonly duration: number;
    /**
     * @remarks
     * Gets the identifier of the effect to apply. Example values
     * include 'fire_resistance' or 'regeneration'.
     *
     */
    readonly name: string;
}

/**
 * Loot item function that populates a dropped container item
 * using another loot table.
 */
// @ts-ignore Class inheritance allowed for native defined classes
export class FillContainerFunction extends LootItemFunction {
    private constructor();
    /**
     * @remarks
     * The path to the loot table with which the container will be
     * filled.
     *
     */
    readonly lootTable: string;
}

/**
 * Represents constants related to fluid containers.
 */
export class FluidContainer {
    private constructor();
    /**
     * @remarks
     * Constant that represents the maximum fill level of a fluid
     * container.
     *
     */
    static readonly maxFillLevel = 6;
    /**
     * @remarks
     * Constant that represents the minimum fill level of a fluid
     * container.
     *
     */
    static readonly minFillLevel = 0;
}

/**
 * @beta
 * Provides access to the fog definitions stack of a player
 * entity, allowing scripts to push, pop, remove, and query
 * active fog definitions.
 */
export class FogSettings {
    private constructor();
    /**
     * @remarks
     * Returns the list of fog identifiers currently on the
     * player's fog stack, ordered from bottom to top.
     *
     * @worldMutation
     *
     * @returns
     * An array of fog definition identifiers currently on the
     * stack.
     * @throws
     * Throws if the entity is invalid.
     *
     * {@link InvalidEntityError}
     */
    getStack(): string[];
    /**
     * @remarks
     * Returns the list of tags currently present on the player's
     * fog stack.
     *
     * @worldMutation
     *
     * @returns
     * An array of tag strings associated with fog settings on the
     * stack.
     * @throws
     * Throws if the entity is invalid.
     *
     * {@link InvalidEntityError}
     */
    getTags(): string[];
    /**
     * @remarks
     * Removes the most recently pushed fog definition from the
     * player's fog stack.
     *
     * @worldMutation
     *
     * @param tag
     * An optional tag identifying which entry to pop. If provided,
     * searches the stack from top to bottom and removes the most
     * recently pushed entry with this tag. If omitted, removes the
     * most recently pushed entry regardless of tag.
     * @returns
     * Returns the identifier of the popped fog definition, or
     * undefined if the stack was unchanged.
     * @throws
     * Throws if the entity is invalid.
     *
     * {@link InvalidEntityError}
     */
    pop(tag?: string): string | undefined;
    /**
     * @remarks
     * Pushes a new fog definition onto the player's fog stack.
     *
     * @worldMutation
     *
     * @param fogId
     * The identifier of the fog definition to push onto the stack
     * (e.g. 'minecraft:fog_bamboo_jungle').
     * @param tag
     * An optional tag used to label this fog definition on the
     * stack, allowing it to be targeted by pop or remove. If
     * omitted, the entry is stored with the tag 'untagged'.
     * @returns
     * Returns the zero-based index at which the fog definition was
     * inserted into the stack.
     * @throws
     * Throws if the entity is invalid, the fog identifier is
     * invalid, or if the stack limit of 16 has been exceeded.
     *
     * {@link FogSettingsError}
     *
     * {@link InvalidEntityError}
     */
    push(fogId: string, tag?: string): number;
    /**
     * @remarks
     * Removes all fog definitions with the given tag from the
     * player's fog stack. If no tag is provided, clears all fog
     * definitions.
     *
     * @worldMutation
     *
     * @param tag
     * An optional tag identifying which the entries to remove. If
     * omitted, clears all fog definitions regardless of tag.
     * @returns
     * Returns true if at least one entry was removed, or false if
     * the stack was unchanged.
     * @throws
     * Throws if the entity is invalid.
     *
     * {@link InvalidEntityError}
     */
    remove(tag?: string): boolean;
    /**
     * @remarks
     * Sets the player's fog stack to the given list of fog
     * identifiers, replacing any existing entries.
     *
     * @worldMutation
     *
     * @param fogIds
     * A stack of fog definition identifiers to set on the player's
     * fog stack (e.g. ['minecraft:fog_bamboo_jungle']). Maximum of
     * 16 entries.
     * @param tag
     * An optional tag to associate with the new entries, used to
     * target them with pop or remove.
     * @throws
     * Throws if the entity is invalid, if more than 16 fog
     * identifiers are provided, or if any fog identifier is
     * invalid.
     *
     * {@link FogSettingsError}
     *
     * {@link InvalidEntityError}
     */
    setStack(fogIds: string[], tag?: string): void;
}

/**
 * Contains information regarding a changed world.gameRules
 * property.
 */
export class GameRuleChangeAfterEvent {
    private constructor();
    /**
     * @remarks
     * The rule identifier pertaining to the changed
     * world.gameRules property.
     *
     */
    readonly rule: GameRule;
    /**
     * @remarks
     * The value of the world.gameRules property after being
     * changed.
     *
     */
    readonly value: boolean | number;
}

/**
 * Manages callbacks that are connected to when a
 * world.gameRules property has changed.
 */
export class GameRuleChangeAfterEventSignal {
    private constructor();
    /**
     * @remarks
     * Adds a callback that will be called when a world.gameRules
     * property is changed.
     *
     * @worldMutation
     *
     * @earlyExecution
     *
     */
    subscribe(callback: (arg0: GameRuleChangeAfterEvent) => void): (arg0: GameRuleChangeAfterEvent) => void;
    /**
     * @remarks
     * Removes a callback from being called when a world.gameRules
     * property is changed.
     *
     * @worldMutation
     *
     * @earlyExecution
     *
     */
    unsubscribe(callback: (arg0: GameRuleChangeAfterEvent) => void): void;
}

/**
 * Represents the game rules for a world experience.
 */
export class GameRules {
    private constructor();
    /**
     * @remarks
     * @worldMutation
     *
     */
    commandBlockOutput: boolean;
    /**
     * @remarks
     * @worldMutation
     *
     */
    commandBlocksEnabled: boolean;
    /**
     * @remarks
     * @worldMutation
     *
     */
    doDayLightCycle: boolean;
    /**
     * @remarks
     * @worldMutation
     *
     */
    doEntityDrops: boolean;
    /**
     * @remarks
     * @worldMutation
     *
     */
    doFireTick: boolean;
    /**
     * @remarks
     * @worldMutation
     *
     */
    doImmediateRespawn: boolean;
    /**
     * @remarks
     * @worldMutation
     *
     */
    doInsomnia: boolean;
    /**
     * @remarks
     * @worldMutation
     *
     */
    doLimitedCrafting: boolean;
    /**
     * @remarks
     * @worldMutation
     *
     */
    doMobLoot: boolean;
    /**
     * @remarks
     * @worldMutation
     *
     */
    doMobSpawning: boolean;
    /**
     * @remarks
     * @worldMutation
     *
     */
    doTileDrops: boolean;
    /**
     * @remarks
     * @worldMutation
     *
     */
    doWeatherCycle: boolean;
    /**
     * @remarks
     * @worldMutation
     *
     */
    drowningDamage: boolean;
    /**
     * @remarks
     * @worldMutation
     *
     */
    fallDamage: boolean;
    /**
     * @remarks
     * @worldMutation
     *
     */
    fireDamage: boolean;
    /**
     * @remarks
     * @worldMutation
     *
     */
    freezeDamage: boolean;
    /**
     * @remarks
     * @worldMutation
     *
     */
    functionCommandLimit: number;
    /**
     * @remarks
     * @worldMutation
     *
     */
    keepInventory: boolean;
    /**
     * @remarks
     * @worldMutation
     *
     */
    maxCommandChainLength: number;
    /**
     * @remarks
     * @worldMutation
     *
     */
    mobGriefing: boolean;
    /**
     * @remarks
     * @worldMutation
     *
     */
    naturalRegeneration: boolean;
    /**
     * @remarks
     * @worldMutation
     *
     */
    playersSleepingPercentage: number;
    /**
     * @beta
     * @remarks
     * @worldMutation
     *
     */
    playerWaypoints: PlayerWaypointsMode;
    /**
     * @remarks
     * @worldMutation
     *
     */
    projectilesCanBreakBlocks: boolean;
    /**
     * @remarks
     * @worldMutation
     *
     */
    pvp: boolean;
    /**
     * @remarks
     * @worldMutation
     *
     */
    randomTickSpeed: number;
    /**
     * @remarks
     * @worldMutation
     *
     */
    recipesUnlock: boolean;
    /**
     * @remarks
     * @worldMutation
     *
     */
    respawnBlocksExplode: boolean;
    /**
     * @remarks
     * @worldMutation
     *
     */
    sendCommandFeedback: boolean;
    /**
     * @remarks
     * @worldMutation
     *
     */
    showBorderEffect: boolean;
    /**
     * @remarks
     * @worldMutation
     *
     */
    showCoordinates: boolean;
    /**
     * @remarks
     * @worldMutation
     *
     */
    showDaysPlayed: boolean;
    /**
     * @remarks
     * @worldMutation
     *
     */
    showDeathMessages: boolean;
    /**
     * @remarks
     * @worldMutation
     *
     */
    showRecipeMessages: boolean;
    /**
     * @remarks
     * @worldMutation
     *
     */
    showTags: boolean;
    /**
     * @remarks
     * @worldMutation
     *
     */
    spawnRadius: number;
    /**
     * @remarks
     * @worldMutation
     *
     */
    tntExplodes: boolean;
    /**
     * @remarks
     * @worldMutation
     *
     */
    tntExplosionDropDecay: boolean;
}

/**
 * Contains the input information for a client instance.
 */
export class InputInfo {
    private constructor();
    /**
     * @remarks
     * The last input mode used by the player.
     *
     * @throws This property can throw when used.
     *
     * {@link EngineError}
     *
     * {@link InvalidEntityError}
     */
    readonly lastInputModeUsed: InputMode;
    /**
     * @remarks
     * Whether the player touch input only affects the touchbar or
     * not.
     *
     * @throws This property can throw when used.
     *
     * {@link InvalidEntityError}
     */
    readonly touchOnlyAffectsHotbar: boolean;
    /**
     * @throws This function can throw errors.
     *
     * {@link EngineError}
     *
     * {@link InvalidEntityError}
     */
    getButtonState(button: InputButton): ButtonState;
    /**
     * @throws This function can throw errors.
     *
     * {@link InvalidEntityError}
     */
    getMovementVector(): Vector2;
}

/**
 * Loot item condition that checks whether or not the entity
 * dropping loot is a baby.
 */
// @ts-ignore Class inheritance allowed for native defined classes
export class IsBabyCondition extends LootItemCondition {
    private constructor();
}

export class ISerializable {
    private constructor();
}

/**
 * @beta
 * Represents the dynamic properties of a block. Only available
 * from block entities. Up to 1KBytes of data can be stored per
 * content pack per block entity in their dynamic properties
 * storage.
 */
// @ts-ignore Class inheritance allowed for native defined classes
export class ItemBlockDynamicPropertiesComponent extends ItemComponent {
    private constructor();
    static readonly componentId = 'minecraft:block_actor_dynamic_properties';
    /**
     * @remarks
     * Returns a DynamicProperty that was stored with the provided
     * key. Keys are unique to each content pack and cannot be used
     * to retrieve dynamic properties set from other content packs.
     * Returns undefined if the key was not found.
     *
     * @throws This function can throw errors.
     *
     * {@link Error}
     *
     * {@link InvalidItemStackError}
     */
    get(key: string): boolean | number | string | Vector3 | undefined;
    /**
     * @remarks
     * Sets a dynamic property with the provided key and value.
     * Keys are unique to each content pack and cannot be used to
     * set dynamic properties for other content packs. Values can
     * be either a Number, a String or a Vector3. Setting a
     * property with an undefined value will remove it from the
     * storage. Storage size usage is counted towards the 1KBytes
     * limit per content pack.
     *
     * @throws This function can throw errors.
     *
     * {@link Error}
     *
     * {@link InvalidItemStackError}
     */
    set(key: string, value?: boolean | number | string | Vector3): void;
    /**
     * @remarks
     * Returns the current size, in bytes, of the dynamic
     * properties storage for this block. The byte count only
     * accounts for properties set by your content pack. The
     * 1KBytes limit is per content pack.
     *
     * @throws This function can throw errors.
     *
     * {@link InvalidItemStackError}
     */
    totalByteCount(): number;
}

/**
 * When present on an item, this item is a book item. Can
 * access and modify the contents of the book and sign it.
 */
// @ts-ignore Class inheritance allowed for native defined classes
export class ItemBookComponent extends ItemComponent {
    private constructor();
    /**
     * @remarks
     * The name of the author of the book if it is signed,
     * otherwise undefined.
     *
     * @throws This property can throw when used.
     *
     * {@link InvalidItemStackError}
     */
    readonly author?: string;
    /**
     * @remarks
     * The contents of pages in the book that are in string format.
     * Entries not in string format will be undefined.
     *
     * @throws This property can throw when used.
     *
     * {@link InvalidItemStackError}
     */
    readonly contents: (string | undefined)[];
    /**
     * @remarks
     * Determines whether the book has been signed or not.
     *
     * @throws This property can throw when used.
     *
     * {@link InvalidItemStackError}
     */
    readonly isSigned: boolean;
    /**
     * @remarks
     * The amount of pages the book has.
     *
     * @throws This property can throw when used.
     *
     * {@link InvalidItemStackError}
     */
    readonly pageCount: number;
    /**
     * @remarks
     * The contents of pages in the book that are in {@link
     * RawMessage} format. Entries not in {@link RawMessage} format
     * will be undefined.
     *
     * @throws This property can throw when used.
     *
     * {@link InvalidItemStackError}
     */
    readonly rawContents: (RawMessage | undefined)[];
    /**
     * @remarks
     * The title of the book if it is signed, otherwise undefined.
     *
     * @throws This property can throw when used.
     *
     * {@link InvalidItemStackError}
     */
    readonly title?: string;
    static readonly componentId = 'minecraft:book';
    /**
     * @remarks
     * Gets the string format content of a page for a given index.
     *
     * @param pageIndex
     * The index of the page.
     * @returns
     * The content of the page if a valid index is provided and it
     * is in string format, otherwise returns undefined.
     * @throws This function can throw errors.
     *
     * {@link InvalidItemStackError}
     */
    getPageContent(pageIndex: number): string | undefined;
    /**
     * @remarks
     * Gets the {@link RawMessage} format content of a page for a
     * given index.
     *
     * @param pageIndex
     * The index of the page.
     * @returns
     * The content of the page if a valid index is provided and it
     * is in {@link RawMessage} format, otherwise returns
     * undefined.
     * @throws This function can throw errors.
     *
     * {@link InvalidItemStackError}
     */
    getRawPageContent(pageIndex: number): RawMessage | undefined;
    /**
     * @remarks
     * Inserts a page at a given index. Empty pages will be created
     * if the index is greater than the current book size.
     * Pages have a maximum limit of 256 characters for strings as
     * well as the JSON representation of a {@link RawMessage}.
     * Books have a maximum limit of 50 pages.
     *
     * @worldMutation
     *
     * @param pageIndex
     * The index of the page.
     * @param content
     * The content to set for the page. Can be a single string or
     * {@link RawMessage} or an array of strings and/or {@link
     * RawMessage}s
     * @throws This function can throw errors.
     *
     * {@link BookError}
     *
     * {@link BookPageContentError}
     *
     * {@link InvalidItemStackError}
     */
    insertPage(pageIndex: number, content: (RawMessage | string)[] | RawMessage | string): void;
    /**
     * @remarks
     * Removes a page at a given index. Existing pages following
     * this page will be moved backward to fill the empty space.
     *
     * @worldMutation
     *
     * @param pageIndex
     * The index of the page.
     * @throws This function can throw errors.
     *
     * {@link InvalidItemStackError}
     */
    removePage(pageIndex: number): void;
    /**
     * @remarks
     * Sets the contents of the book's pages. Pre-existing pages
     * will be cleared.
     * Pages have a maximum limit of 256 characters for strings as
     * well as the JSON representation of a {@link RawMessage}.
     * Books have a maximum limit of 50 pages.
     *
     * @worldMutation
     *
     * @param contents
     * An array of each page's contents. Each page can be a single
     * string or {@link RawMessage} or an array of strings and/or
     * {@link RawMessage}s.
     * @throws This function can throw errors.
     *
     * {@link BookError}
     *
     * {@link BookPageContentError}
     *
     * {@link InvalidItemStackError}
     */
    setContents(contents: ((RawMessage | string)[] | RawMessage | string)[]): void;
    /**
     * @remarks
     * Sets or creates the content of a specific page. Empty pages
     * will be created if the index is greater than the current
     * book size.
     * Pages have a maximum limit of 256 characters for strings as
     * well as the JSON representation of a {@link RawMessage}.
     * Books have a maximum limit of 50 pages.
     *
     * @worldMutation
     *
     * @param pageIndex
     * The index of the page.
     * @param content
     * The content to set for the page. Can be a single string or
     * {@link RawMessage} or an array of strings and/or {@link
     * RawMessage}s
     * @throws This function can throw errors.
     *
     * {@link BookError}
     *
     * {@link BookPageContentError}
     *
     * {@link InvalidItemStackError}
     */
    setPageContent(pageIndex: number, content: (RawMessage | string)[] | RawMessage | string): void;
    /**
     * @remarks
     * Signs a book giving it a title and author name. Once signed
     * players can no longer directly edit the book.
     * Titles have a maximum character limit of 16.
     *
     * @worldMutation
     *
     * @param title
     * The title to give the book.
     * @param author
     * The name of the book's author.
     * @throws This function can throw errors.
     *
     * {@link BookError}
     *
     * {@link InvalidEntityError}
     *
     * {@link InvalidItemStackError}
     */
    signBook(title: string, author: string): void;
}

/**
 * Contains information related to a chargeable item completing
 * being charged.
 */
export class ItemCompleteUseAfterEvent {
    private constructor();
    /**
     * @remarks
     * Returns the item stack that has completed charging.
     *
     */
    readonly itemStack: ItemStack;
    /**
     * @remarks
     * Returns the source entity that triggered this item event.
     *
     */
    readonly source: Player;
    /**
     * @remarks
     * Returns the time, in ticks, for the remaining duration left
     * before the charge completes its cycle.
     *
     */
    readonly useDuration: number;
}

/**
 * Manages callbacks that are connected to the completion of
 * charging for a chargeable item.
 */
export class ItemCompleteUseAfterEventSignal {
    private constructor();
    /**
     * @remarks
     * Adds a callback that will be called when a chargeable item
     * completes charging.
     *
     * @worldMutation
     *
     * @earlyExecution
     *
     */
    subscribe(callback: (arg0: ItemCompleteUseAfterEvent) => void): (arg0: ItemCompleteUseAfterEvent) => void;
    /**
     * @remarks
     * Removes a callback from being called when a chargeable item
     * completes charging.
     *
     * @worldMutation
     *
     * @earlyExecution
     *
     */
    unsubscribe(callback: (arg0: ItemCompleteUseAfterEvent) => void): void;
}

/**
 * Contains information related to a chargeable item completing
 * being charged.
 */
export class ItemCompleteUseEvent {
    private constructor();
    /**
     * @remarks
     * Returns the item stack that has completed charging.
     *
     */
    readonly itemStack: ItemStack;
    /**
     * @remarks
     * Returns the source entity that triggered this item event.
     *
     */
    readonly source: Player;
}

/**
 * Base class for item components.
 */
// @ts-ignore Class inheritance allowed for native defined classes
export class ItemComponent extends Component {
    private constructor();
}

/**
 * Contains information regarding an item before it is damaged
 * from hitting an entity.
 */
export class ItemComponentBeforeDurabilityDamageEvent {
    private constructor();
    /**
     * @remarks
     * The attacking entity.
     *
     */
    readonly attackingEntity: Entity;
    /**
     * @remarks
     * The damage applied to the item's durability when the event
     * occurs.
     *
     */
    durabilityDamage: number;
    /**
     * @remarks
     * The entity being hit.
     *
     */
    readonly hitEntity: Entity;
    /**
     * @remarks
     * The item stack used to hit the entity.
     *
     */
    itemStack?: ItemStack;
}

/**
 * Contains information related to a chargeable item completing
 * being charged via a component.
 */
// @ts-ignore Class inheritance allowed for native defined classes
export class ItemComponentCompleteUseEvent extends ItemCompleteUseEvent {
    private constructor();
}

/**
 * Contains information related to a food item being consumed.
 */
export class ItemComponentConsumeEvent {
    private constructor();
    /**
     * @remarks
     * The item stack that was consumed.
     *
     */
    readonly itemStack: ItemStack;
    /**
     * @remarks
     * The source entity that consumed the item.
     *
     */
    readonly source: Entity;
}

/**
 * Contains information regarding when an item is used to hit
 * an entity.
 */
export class ItemComponentHitEntityEvent {
    private constructor();
    /**
     * @remarks
     * The attacking entity.
     *
     */
    readonly attackingEntity: Entity;
    /**
     * @remarks
     * Whether the hit landed or had any effect.
     *
     */
    readonly hadEffect: boolean;
    /**
     * @remarks
     * The entity being hit.
     *
     */
    readonly hitEntity: Entity;
    /**
     * @remarks
     * The item stack used to hit the entity.
     *
     */
    readonly itemStack?: ItemStack;
}

/**
 * Contains information regarding the mining of a block using
 * an item.
 */
export class ItemComponentMineBlockEvent {
    private constructor();
    /**
     * @remarks
     * The block impacted by this event.
     *
     */
    readonly block: Block;
    /**
     * @remarks
     * The item stack used to mine the block.
     *
     */
    readonly itemStack?: ItemStack;
    /**
     * @remarks
     * The block permutation that was mined.
     *
     */
    readonly minedBlockPermutation: BlockPermutation;
    /**
     * @remarks
     * The entity that mined the block.
     *
     */
    readonly source: Entity;
}

/**
 * Provides the functionality for registering custom components
 * for items.
 */
export class ItemComponentRegistry {
    private constructor();
    /**
     * @remarks
     * Registers an item custom component that can be used in item
     * JSON configuration.
     *
     * @earlyExecution
     *
     * @param name
     * The id that represents this custom component. Must have a
     * namespace. This id can be specified in a item's JSON
     * configuration under the 'minecraft:custom_components' item
     * component.
     * @param itemCustomComponent
     * The collection of event functions that will be called when
     * the event occurs on an item using this custom component id.
     * @throws This function can throw errors.
     *
     * {@link CustomComponentInvalidRegistryError}
     *
     * {@link EngineError}
     *
     * {@link ItemCustomComponentAlreadyRegisteredError}
     *
     * {@link ItemCustomComponentReloadNewComponentError}
     *
     * {@link ItemCustomComponentReloadNewEventError}
     *
     * {@link ItemCustomComponentReloadVersionError}
     *
     * {@link NamespaceNameError}
     */
    registerCustomComponent(name: string, itemCustomComponent: ItemCustomComponent): void;
}

/**
 * Contains information regarding the use of an item.
 */
export class ItemComponentUseEvent {
    private constructor();
    /**
     * @remarks
     * The item stack when the item was used.
     *
     */
    readonly itemStack?: ItemStack;
    /**
     * @remarks
     * The player who used the item.
     *
     */
    readonly source: Player;
}

/**
 * Contains information regarding the use of an item on a block
 * via a component.
 */
// @ts-ignore Class inheritance allowed for native defined classes
export class ItemComponentUseOnEvent extends ItemUseOnEvent {
    private constructor();
    /**
     * @remarks
     * The entity that used the item on the block.
     *
     */
    readonly source: Entity;
    /**
     * @remarks
     * The block permutation that the item was used on.
     *
     */
    readonly usedOnBlockPermutation: BlockPermutation;
}

/**
 * When present, the item can be composted in the composter
 * block if the composting chance is in the range [1 - 100].
 */
// @ts-ignore Class inheritance allowed for native defined classes
export class ItemCompostableComponent extends ItemComponent {
    private constructor();
    /**
     * @remarks
     * This is the percent chance of the item composting in the
     * composter block and generating a compost layer. Note this
     * api will also return the composting chance for vanilla items
     * that are compostable but do not use the compostable item
     * component.
     *
     * @throws
     * Throws if value outside the range [1 - 100]
     */
    readonly compostingChance: number;
    static readonly componentId = 'minecraft:compostable';
}

/**
 * 表示物品使用冷却组件。当出现在物品上时，表示该物品被实体使用后会有冷却效果。
 * 注意，若使用后不会进入冷却，原版物品会获取到没有实际作用的该组件。
 * 
 * When present on an item, this item has a cooldown effect
 * when used by entities.
 */
// @ts-ignore Class inheritance allowed for native defined classes
export class ItemCooldownComponent extends ItemComponent {
    private constructor();
    /**
     * @remarks
     * 表示物品的冷却类别。
     * 
     * Represents the cooldown category that this item is
     * associated with.
     *
     * @throws This property can throw when used.
     */
    readonly cooldownCategory: string;
    /**
     * @remarks
     * 物品冷却所需的时间，单位为刻。
     * 
     * Amount of time, in ticks, it will take this item to
     * cooldown.
     *
     * @throws This property can throw when used.
     */
    readonly cooldownTicks: number;
    static readonly componentId = 'minecraft:cooldown';
    /**
     * @remarks
     * @worldMutation
     *
     * @throws This function can throw errors.
     */
    getCooldownTicksRemaining(player: Player): number;
    /**
     * @remarks
     * Will return true if the item is the cooldown category passed
     * in and false otherwise.
     *
     * @worldMutation
     *
     * @param cooldownCategory
     * The cooldown category that might be associated with this
     * item.
     * @returns
     * True if the item is the given cooldown category.
     * @throws This function can throw errors.
     */
    isCooldownCategory(cooldownCategory: string): boolean;
    /**
     * @remarks
     * 无法在只读模式下调用此函数，详见 {@link WorldBeforeEvents}。
     *
     * @throws This function can throw errors.
     */
    getCooldownTicksRemaining(player: Player): number;
    /**
     * @remarks
     * Will return true if the item is the cooldown category passed
     * in and false otherwise.
     *
     * 无法在只读模式下调用此函数，详见 {@link WorldBeforeEvents}。
     *
     * @param cooldownCategory
     * The cooldown category that might be associated with this
     * item.
     * @returns
     * True if the item is the given cooldown category.
     * @throws This function can throw errors.
     */
    isCooldownCategory(cooldownCategory: string): boolean;
    /**
     * @remarks
     * 开始物品的冷却周期。
     * 如果物品已在冷却中，将重新开始冷却。
     * 
     * Starts a new cooldown period for this item.
     *
     * @worldMutation
     *
     * @throws This function can throw errors.
     */
    startCooldown(player: Player): void;
}

/**
 * An instance of a custom component on an item.
 */
// @ts-ignore Class inheritance allowed for native defined classes
export class ItemCustomComponentInstance extends ItemComponent {
    private constructor();
    readonly customComponentParameters: CustomComponentParameters;
}

/**
 * 表示物品耐久组件。当出现在物品上时，表示该物品可以在使用中受到损坏。
 * 注意，只能在数驱物品上获取和使用该组件。
 * 
 * When present on an item, this item can take damage in the
 * process of being used. Note that this component only applies
 * to data-driven items.
 * @seeExample giveHurtDiamondSword.ts
 */
// @ts-ignore Class inheritance allowed for native defined classes
export class ItemDurabilityComponent extends ItemComponent {
    private constructor();
    /**
     * @remarks
     * 此物品当前的损坏值。
     * 物品当前耐久度为 `maxDurability - damage`。
     * 当被设置为 负数，`Infinity`，`NaN` 等值时，值为 0。
     * 
     * Returns the current damage level of this particular item.
     *
     * @worldMutation
     *
     */
    damage: number;
    /**
     * @remarks
     * 表示该物品在损坏前可以承受的损坏值。
     * 
     * Represents the amount of damage that this item can take
     * before breaking.
     *
     * @throws This property can throw when used.
     */
    readonly maxDurability: number;
    /**
     * @remarks
     * Whether an item breaks or loses durability. Setting to true
     * temporarily removes item's durability HUD, and freezes
     * durability loss on item.
     *
     * @worldMutation
     *
     */
    unbreakable: boolean;
    static readonly componentId = 'minecraft:durability';
    /**
     * @remarks
     * 返回根据 `damageRange` 属性生成的最大损坏概率，
     * 附带一个耐久附魔等级作为可选参数。
     * 
     * Returns the maximum chance that this item would be damaged
     * using the damageRange property, given an unbreaking
     * enchantment level.
     *
     * @worldMutation
     *
     * @param unbreakingEnchantmentLevel
     * 耐久魔咒等级，在计算损坏概率时受到此参数的影响。
     * 传入的 `unbreakingEnchantmentLevel` 参数必须介于 [0, 3]。
     * 
     * Unbreaking factor to consider in factoring the damage
     * chance. Incoming unbreaking parameter must be within the
     * range [0, 3].
     * Defaults to: 0
     * Bounds: [0, 3]
     * @returns 使用时的最大损坏概率。
     * @throws
     * 若 `unbreakingEnchantmentLevel` 参数未在范围内时，抛出 `TypeError` 。
     */
    getDamageChance(unbreakingEnchantmentLevel?: number): number;
    /**
     * @remarks
     * 返回用于计算物品损失耐久的概率范围。最终物品损失耐久的概率将落在此范围中。
     * 
     * A range of numbers that is used to calculate the damage
     * chance for an item. The damage chance will fall within this
     * range.
     *
     * @worldMutation
     *
     * @throws This function can throw errors.
     */
    getDamageChanceRange(): NumberRange;
}

/**
 * When present on an item, this item can be dyed.
 */
// @ts-ignore Class inheritance allowed for native defined classes
export class ItemDyeableComponent extends ItemComponent {
    private constructor();
    /**
     * @remarks
     * Sets and returns the current color of the item.
     *
     * @worldMutation
     *
     */
    color?: RGB;
    /**
     * @remarks
     * Returns the default color of the item.
     *
     * @throws This property can throw when used.
     */
    readonly defaultColor?: RGB;
    static readonly componentId = 'minecraft:dyeable';
}

/**
 * When present on an item, this item can have enchantments
 * applied to it.
 */
// @ts-ignore Class inheritance allowed for native defined classes
export class ItemEnchantableComponent extends ItemComponent {
    private constructor();
    /**
     * @throws This property can throw when used.
     */
    readonly slots: EnchantmentSlot[];
    static readonly componentId = 'minecraft:enchantable';
    /**
     * @remarks
     * Adds an enchantment to the item stack.
     *
     * @worldMutation
     *
     * @param enchantment
     * The enchantment interface to be added.
     * @throws
     * ScriptItemEnchantmentUnknownIdError: Exception thrown if the
     * enchantment type does not exist.
     *
     * ScriptItemEnchantmentLevelOutOfBoundsError: Exception thrown
     * if the enchantment level is outside the allowable range for
     * the given enchantment type.
     *
     * ScriptItemEnchantmentTypeNotCompatibleError: Exception
     * thrown if the enchantment is not compatible with the item
     * stack.
     *
     *
     * {@link EnchantmentLevelOutOfBoundsError}
     *
     * {@link EnchantmentTypeNotCompatibleError}
     *
     * {@link EnchantmentTypeUnknownIdError}
     *
     * {@link Error}
     */
    addEnchantment(enchantment: Enchantment): void;
    /**
     * @remarks
     * Adds a list of enchantments to the item stack.
     *
     * @worldMutation
     *
     * @param enchantments
     * The list of enchantments to be added.
     * @throws
     * ScriptItemEnchantmentUnknownIdError: Exception thrown if any
     * enchantment type does not exist.
     *
     * ScriptItemEnchantmentLevelOutOfBoundsError: Exception thrown
     * if any enchantment level is outside the allowable range for
     * the given enchantment type.
     *
     * ScriptItemEnchantmentTypeNotCompatibleError: Exception
     * thrown if any enchantment is not compatible with the item
     * stack.
     *
     *
     * {@link EnchantmentLevelOutOfBoundsError}
     *
     * {@link EnchantmentTypeNotCompatibleError}
     *
     * {@link EnchantmentTypeUnknownIdError}
     *
     * {@link Error}
     */
    addEnchantments(enchantments: Enchantment[]): void;
    /**
     * @remarks
     * Checks whether an enchantment can be added to the item
     * stack.
     *
     * @param enchantment
     * The enchantment interface to be added.
     * @returns
     * Returns true if the enchantment can be added to the item
     * stack.
     * @throws
     * ScriptItemEnchantmentUnknownIdError: Exception thrown if the
     * enchantment type does not exist.
     *
     * ScriptItemEnchantmentLevelOutOfBoundsError: Exception thrown
     * if the enchantment level is outside the allowable range for
     * the given enchantment type.
     *
     *
     * {@link EnchantmentLevelOutOfBoundsError}
     *
     * {@link EnchantmentTypeUnknownIdError}
     */
    canAddEnchantment(enchantment: Enchantment): boolean;
    /**
     * @remarks
     * Gets the enchantment of a given type from the item stack.
     *
     * @param enchantmentType
     * The enchantment type to get.
     * @returns
     * Returns the enchantment if it exists on the item stack.
     * @throws
     * ScriptItemEnchantmentUnknownIdError: Exception thrown if the
     * enchantment type does not exist.
     *
     *
     * {@link EnchantmentTypeUnknownIdError}
     */
    getEnchantment(enchantmentType: EnchantmentType | string): Enchantment | undefined;
    /**
     * @remarks
     * Gets all enchantments on the item stack.
     *
     * @returns
     * Returns a list of enchantments on the item stack.
     * @throws This function can throw errors.
     */
    getEnchantments(): Enchantment[];
    /**
     * @remarks
     * Checks whether an item stack has a given enchantment type.
     *
     * @param enchantmentType
     * The enchantment type to check for.
     * @returns
     * Returns true if the item stack has the enchantment type.
     * @throws
     * ScriptItemEnchantmentUnknownIdError: Exception thrown if the
     * enchantment type does not exist.
     *
     *
     * {@link EnchantmentTypeUnknownIdError}
     */
    hasEnchantment(enchantmentType: EnchantmentType | string): boolean;
    /**
     * @remarks
     * Removes all enchantments applied to this item stack.
     *
     * @worldMutation
     *
     * @throws This function can throw errors.
     */
    removeAllEnchantments(): void;
    /**
     * @remarks
     * Removes an enchantment of the given type.
     *
     * @worldMutation
     *
     * @param enchantmentType
     * The enchantment type to remove.
     * @throws
     * ScriptItemEnchantmentUnknownIdError: Exception thrown if the
     * enchantment type does not exist.
     *
     *
     * {@link EnchantmentTypeUnknownIdError}
     *
     * {@link Error}
     */
    removeEnchantment(enchantmentType: EnchantmentType | string): void;
}

/**
 * 表示物品食物组件。当出现在物品上时，实体可以消耗此物品。
 * 注意，只能在数驱物品上获取和使用该组件。
 * 
 * When present on an item, this item is consumable by
 * entities. Note that this component only applies to
 * data-driven items.
 */
// @ts-ignore Class inheritance allowed for native defined classes
export class ItemFoodComponent extends ItemComponent {
    private constructor();
    /**
     * @remarks
     * 若为 `true` ，则无论饥饿值是否已满，玩家始终可以食用该物品。
     * 
     * If true, the player can always eat this item (even when not
     * hungry).
     *
     * @throws This property can throw when used.
     */
    readonly canAlwaysEat: boolean;
    /**
     * @remarks
     * 表示实体在食用该物品后恢复的饥饿值，即营养值。
     * 
     * Represents how much nutrition this food item will give an
     * entity when eaten.
     *
     * @throws This property can throw when used.
     */
    readonly nutrition: number;
    /**
     * @remarks
     * 当一个物品被食用，
     * 将根据公式 `nutrition * saturation_modifier * 2`
     * 来为玩家添加饱和状态。
     * 
     * When an item is eaten, this value is used according to this
     * formula (nutrition * saturation_modifier * 2) to apply a
     * saturation buff.
     *
     * @throws This property can throw when used.
     */
    readonly saturationModifier: number;
    /**
     * @remarks
     * 若给出，则使用时将该物品转换为标识符指定的物品。
     * 
     * When specified, converts the active item to the one
     * specified by this property.
     *
     * @throws This property can throw when used.
     */
    readonly usingConvertsTo: string;
    static readonly componentId = 'minecraft:food';
}

/**
 * This component is added to items with the `Storage Item`
 * component. Can access and modify this items inventory
 * container.
 */
// @ts-ignore Class inheritance allowed for native defined classes
export class ItemInventoryComponent extends ItemComponent {
    private constructor();
    /**
     * @throws This property can throw when used.
     *
     * {@link InvalidContainerError}
     */
    readonly container: Container;
    static readonly componentId = 'minecraft:inventory';
}

/**
 * When present on an item, this item is a potion item.
 */
// @ts-ignore Class inheritance allowed for native defined classes
export class ItemPotionComponent extends ItemComponent {
    private constructor();
    /**
     * @remarks
     * The PotionDeliveryType associated with the potion item.
     *
     * @throws This property can throw when used.
     *
     * {@link EngineError}
     *
     * {@link Error}
     */
    readonly potionDeliveryType: PotionDeliveryType;
    /**
     * @remarks
     * The PotionEffectType associated with the potion item.
     *
     * @throws This property can throw when used.
     *
     * {@link EngineError}
     *
     * {@link Error}
     */
    readonly potionEffectType: PotionEffectType;
    static readonly componentId = 'minecraft:potion';
}

/**
 * Contains information related to a chargeable item when the
 * player has finished using the item and released the build
 * action.
 */
export class ItemReleaseUseAfterEvent {
    private constructor();
    /**
     * @remarks
     * Returns the item stack that triggered this item event.
     *
     */
    readonly itemStack?: ItemStack;
    /**
     * @remarks
     * Returns the source entity that triggered this item event.
     *
     */
    readonly source: Player;
    /**
     * @remarks
     * Returns the time, in ticks, for the remaining duration left
     * before the charge completes its cycle.
     *
     */
    readonly useDuration: number;
}

/**
 * Manages callbacks that are connected to the releasing of
 * charging for a chargeable item.
 */
export class ItemReleaseUseAfterEventSignal {
    private constructor();
    /**
     * @remarks
     * Adds a callback that will be called when a chargeable item
     * is released from charging.
     *
     * @worldMutation
     *
     * @earlyExecution
     *
     */
    subscribe(callback: (arg0: ItemReleaseUseAfterEvent) => void): (arg0: ItemReleaseUseAfterEvent) => void;
    /**
     * @remarks
     * Removes a callback from being called when a chargeable item
     * is released from charging.
     *
     * @worldMutation
     *
     * @earlyExecution
     *
     */
    unsubscribe(callback: (arg0: ItemReleaseUseAfterEvent) => void): void;
}

/**
 * Defines a collection of items.
 * @seeExample itemStacks.ts
 * @seeExample givePlayerEquipment.ts
 * @seeExample spawnFeatherItem.ts
 */
export class ItemStack {
    /**
     * @remarks
     * Number of the items in the stack. Valid values range between
     * 1-255. The provided value will be clamped to the item's
     * maximum stack size.
     *
     * @worldMutation
     *
     * Bounds: [1, 255]
     * @throws
     * Throws if the value is outside the range of 1-255.
     */
    amount: number;
    /**
     * @remarks
     * Returns whether the item is stackable. An item is considered
     * stackable if the item's maximum stack size is greater than 1
     * and the item does not contain any custom data or properties.
     *
     */
    readonly isStackable: boolean;
    /**
     * @remarks
     * Gets or sets whether the item is kept on death.
     *
     * @worldMutation
     *
     */
    keepOnDeath: boolean;
    /**
     * @remarks
     * Key for the localization of this items's name used in .lang
     * files.
     *
     * @throws This property can throw when used.
     *
     * {@link EngineError}
     */
    readonly localizationKey: string;
    /**
     * @remarks
     * Gets or sets the item's lock mode. The default value is
     * `ItemLockMode.none`.
     *
     * @worldMutation
     *
     */
    lockMode: ItemLockMode;
    /**
     * @remarks
     * The maximum stack size. This value varies depending on the
     * type of item. For example, torches have a maximum stack size
     * of 64, while eggs have a maximum stack size of 16.
     *
     */
    readonly maxAmount: number;
    /**
     * @remarks
     * Given name of this stack of items. The name tag is displayed
     * when hovering over the item. Setting the name tag to an
     * empty string or `undefined` will remove the name tag.
     *
     * @worldMutation
     *
     * @throws
     * Throws if the length exceeds 255 characters.
     */
    nameTag?: string;
    /**
     * @remarks
     * The type of the item.
     *
     */
    readonly 'type': ItemType;
    /**
     * @remarks
     * Identifier of the type of items for the stack. If a
     * namespace is not specified, 'minecraft:' is assumed.
     * Examples include 'wheat' or 'apple'.
     *
     */
    readonly typeId: string;
    /**
     * @remarks
     * The total weight of all items in the stack plus the weight
     * of all items in the items container which is defined with
     * the `Storage Item` component. The weight per item can be
     * modified by the `Storage Weight Modifier` component.
     *
     */
    readonly weight: number;
    /**
     * @remarks
     * Creates a new instance of a stack of items for use in the
     * world.
     *
     * @param itemType
     * Type of item to create. See the {@link
     * @minecraft/vanilla-data.MinecraftItemTypes} enumeration for
     * a list of standard item types in Minecraft experiences.
     * @param amount
     * Number of items to place in the stack, between 1-255. The
     * provided value will be clamped to the item's maximum stack
     * size. Note that certain items can only have one item in the
     * stack.
     * Defaults to: 1
     * Bounds: [1, 255]
     * @throws
     * Throws if `itemType` is invalid, or if `amount` is outside
     * the range of 1-255.
     */
    constructor(itemType: ItemType | string, amount?: number);
    /**
     * @remarks
     * Clears all dynamic properties that have been set on this
     * item stack.
     *
     */
    clearDynamicProperties(): void;
    /**
     * @remarks
     * Creates an exact copy of the item stack, including any
     * custom data or properties.
     *
     * @returns
     * Returns a copy of this item stack.
     */
    clone(): ItemStack;
    /**
     * @remarks
     * Get the list of block types this item can break in Adventure
     * mode.
     *
     * @worldMutation
     *
     */
    getCanDestroy(): string[];
    /**
     * @remarks
     * Get the list of block types this item can be placed on in
     * Adventure mode.
     *
     * @worldMutation
     *
     */
    getCanPlaceOn(): string[];
    /**
     * @remarks
     * Gets a component (that represents additional capabilities)
     * for an item stack.
     *
     * @param componentId
     * The identifier of the component (e.g., 'minecraft:food'). If
     * no namespace prefix is specified, 'minecraft:' is assumed.
     * Available component IDs are those in the {@link
     * ItemComponentTypes} enum and custom component IDs registered
     * with the {@link ItemComponentRegistry}.
     * @returns
     * Returns the component if it exists on the item stack,
     * otherwise undefined.
     * @seeExample giveHurtDiamondSword.ts
     */
    getComponent<T extends string>(componentId: T): ItemComponentReturnType<T> | undefined;
    /**
     * @remarks
     * Returns all scripting components that are present on this
     * item stack.
     *
     */
    getComponents(): ItemComponent[];
    /**
     * @remarks
     * Returns a property value.
     *
     * @param identifier
     * The property identifier.
     * @returns
     * Returns the value for the property, or undefined if the
     * property has not been set.
     */
    getDynamicProperty(identifier: string): boolean | number | string | Vector3 | undefined;
    /**
     * @remarks
     * Returns the available set of dynamic property identifiers
     * that have been used on this entity.
     *
     * @returns
     * A string array of the dynamic properties set on this entity.
     */
    getDynamicPropertyIds(): string[];
    /**
     * @remarks
     * Returns the total size, in bytes, of all the dynamic
     * properties that are currently stored for this entity. This
     * includes the size of both the key and the value.  This can
     * be useful for diagnosing performance warning signs - if, for
     * example, an entity has many megabytes of associated dynamic
     * properties, it may be slow to load on various devices.
     *
     */
    getDynamicPropertyTotalByteCount(): number;
    /**
     * @remarks
     * Returns the lore value - a secondary display string - for an
     * ItemStack.
     *
     * @returns
     * An array of lore lines. If the item does not have lore,
     * returns an empty array.
     */
    getLore(): string[];
    /**
     * @remarks
     * Returns the lore value - a secondary display string - for an
     * ItemStack. String lore lines will be converted to a {@link
     * RawMessage} and put under {@link RawMessage.text}.
     *
     * @returns
     * An array of lore lines. If the item does not have lore,
     * returns an empty array.
     */
    getRawLore(): RawMessage[];
    /**
     * @remarks
     * Returns a set of tags associated with this item stack.
     *
     */
    getTags(): string[];
    /**
     * @remarks
     * Returns true if the specified component is present on this
     * item stack.
     *
     * @param componentId
     * The identifier of the component (e.g., 'minecraft:food') to
     * retrieve. If no namespace prefix is specified, 'minecraft:'
     * is assumed.
     */
    hasComponent(componentId: string): boolean;
    /**
     * @remarks
     * Checks whether this item stack has a particular tag
     * associated with it.
     *
     * @param tag
     * Tag to search for.
     * @returns
     * True if the Item Stack has the tag associated with it, else
     * false.
     */
    hasTag(tag: string): boolean;
    /**
     * @remarks
     * Returns whether this item stack can be stacked with the
     * given `itemStack`. This is determined by comparing the item
     * type and any custom data and properties associated with the
     * item stacks. The amount of each item stack is not taken into
     * consideration, but for non-stackable items this will always
     * return false.
     *
     * @param itemStack
     * ItemStack to check stacking compatibility with.
     * @returns
     * True if the Item Stack is stackable with the itemStack
     * passed in. False for non-stackable items.
     */
    isStackableWith(itemStack: ItemStack): boolean;
    /**
     * @remarks
     * Version safe way of checking if an item matches.
     *
     * @param itemName
     * Identifier of the item.
     * @param states
     *  Applicable only for blocks. An optional set of states to
     * compare against. If states is not specified, matches checks
     * against the set of types more broadly.
     * @returns
     * Returns a boolean whether the specified item matches.
     */
    matches(itemName: string, states?: Record<string, boolean | number | string>): boolean;
    /**
     * @remarks
     * The list of block types this item can break in Adventure
     * mode. The block names are displayed in the item's tooltip.
     * Setting the value to undefined will clear the list.
     *
     * @worldMutation
     *
     * @param blockIdentifiers
     * String list of block types that the item can destroy.
     * @throws
     * Throws if any of the provided block identifiers are invalid.
     * @seeExample giveDestroyRestrictedPickaxe.ts
     */
    setCanDestroy(blockIdentifiers?: string[]): void;
    /**
     * @remarks
     * The list of block types this item can be placed on in
     * Adventure mode. This is only applicable to block items. The
     * block names are displayed in the item's tooltip. Setting the
     * value to undefined will clear the list.
     *
     * @worldMutation
     *
     * @param blockIdentifiers
     * String list of block types that the item can be placed on.
     * @throws
     * Throws if any of the provided block identifiers are invalid.
     * @seeExample givePlaceRestrictedGoldBlock.ts
     */
    setCanPlaceOn(blockIdentifiers?: string[]): void;
    /**
     * @remarks
     * Sets multiple dynamic properties with specific values.
     *
     * @param values
     * A Record of key value pairs of the dynamic properties to
     * set. If the data value is null, it will remove that property
     * instead.
     * @throws This function can throw errors.
     *
     * {@link ArgumentOutOfBoundsError}
     *
     * {@link UnsupportedFunctionalityError}
     */
    setDynamicProperties(values: Record<string, boolean | number | string | Vector3 | undefined>): void;
    /**
     * @remarks
     * Sets a specified property to a value. Note: This function
     * only works with non-stackable items.
     *
     * @param identifier
     * The property identifier.
     * @param value
     * Data value of the property to set. If the value is null, it
     * will remove the property instead.
     * @throws
     * Throws if the item stack is stackable.
     *
     * {@link ArgumentOutOfBoundsError}
     *
     * {@link UnsupportedFunctionalityError}
     */
    setDynamicProperty(identifier: string, value?: boolean | number | string | Vector3): void;
    /**
     * @remarks
     * Sets the lore value - a secondary display string - for an
     * ItemStack. The lore list is cleared if set to an empty
     * string or undefined.
     *
     * @worldMutation
     *
     * @param loreList
     * List of lore lines. Each element in the list represents a
     * new line. The maximum lore line count is 20. The maximum
     * lore line length is 50 characters.
     * @throws This function can throw errors.
     *
     * {@link ArgumentOutOfBoundsError}
     *
     * {@link Error}
     * @seeExample diamondAwesomeSword.ts
     */
    setLore(loreList?: (RawMessage | string)[]): void;
}

/**
 * Contains information related to a chargeable item starting
 * to be charged.
 */
export class ItemStartUseAfterEvent {
    private constructor();
    /**
     * @remarks
     * The impacted item stack that is starting to be charged.
     *
     */
    readonly itemStack: ItemStack;
    /**
     * @remarks
     * Returns the source entity that triggered this item event.
     *
     */
    readonly source: Player;
    /**
     * @remarks
     * Returns the time, in ticks, for the remaining duration left
     * before the charge completes its cycle.
     *
     */
    readonly useDuration: number;
}

/**
 * Manages callbacks that are connected to the start of
 * charging for a chargeable item.
 */
export class ItemStartUseAfterEventSignal {
    private constructor();
    /**
     * @remarks
     * Adds a callback that will be called when a chargeable item
     * starts charging.
     *
     * @worldMutation
     *
     * @earlyExecution
     *
     */
    subscribe(callback: (arg0: ItemStartUseAfterEvent) => void): (arg0: ItemStartUseAfterEvent) => void;
    /**
     * @remarks
     * Removes a callback from being called when a chargeable item
     * starts charging.
     *
     * @worldMutation
     *
     * @earlyExecution
     *
     */
    unsubscribe(callback: (arg0: ItemStartUseAfterEvent) => void): void;
}

/**
 * Contains information related to an item being used on a
 * block. This event fires when a player presses the the Use
 * Item / Place Block button to successfully use an item or
 * place a block. Fires for the first block that is interacted
 * with when performing a build action. Note: This event cannot
 * be used with Hoe or Axe items.
 */
export class ItemStartUseOnAfterEvent {
    private constructor();
    /**
     * @remarks
     * The block that the item is used on.
     *
     */
    readonly block: Block;
    /**
     * @remarks
     * The face of the block that an item is being used on.
     *
     */
    readonly blockFace: Direction;
    /**
     * @remarks
     * The impacted item stack that is starting to be used. Can be
     * undefined in some gameplay scenarios like pushing a button
     * with an empty hand.
     *
     */
    readonly itemStack?: ItemStack;
    /**
     * @remarks
     * Returns the source entity that triggered this item event.
     *
     */
    readonly source: Player;
}

/**
 * Manages callbacks that are connected to an item starting
 * being used on a block event.
 */
export class ItemStartUseOnAfterEventSignal {
    private constructor();
    /**
     * @remarks
     * Adds a callback that will be called when an item is used on
     * a block.
     *
     * @worldMutation
     *
     * @earlyExecution
     *
     */
    subscribe(callback: (arg0: ItemStartUseOnAfterEvent) => void): (arg0: ItemStartUseOnAfterEvent) => void;
    /**
     * @remarks
     * Removes a callback from being called when an item is used on
     * a block.
     *
     * @worldMutation
     *
     * @earlyExecution
     *
     */
    unsubscribe(callback: (arg0: ItemStartUseOnAfterEvent) => void): void;
}

/**
 * Contains information related to a chargeable item has
 * finished an items use cycle, or when the player has released
 * the use action with the item.
 */
export class ItemStopUseAfterEvent {
    private constructor();
    /**
     * @remarks
     * The impacted item stack that is stopping being charged.
     * ItemStopUseAfterEvent can be called when teleporting to a
     * different dimension and this can be undefined.
     *
     */
    readonly itemStack?: ItemStack;
    /**
     * @remarks
     * Returns the source entity that triggered this item event.
     *
     */
    readonly source: Player;
    /**
     * @remarks
     * Returns the time, in ticks, for the remaining duration left
     * before the charge completes its cycle.
     *
     */
    readonly useDuration: number;
}

/**
 * Manages callbacks that are connected to the stopping of
 * charging for an item that has a registered
 * minecraft:chargeable component.
 */
export class ItemStopUseAfterEventSignal {
    private constructor();
    /**
     * @remarks
     * Adds a callback that will be called when a chargeable item
     * stops charging.
     *
     * @worldMutation
     *
     * @earlyExecution
     *
     */
    subscribe(callback: (arg0: ItemStopUseAfterEvent) => void): (arg0: ItemStopUseAfterEvent) => void;
    /**
     * @remarks
     * Removes a callback from being called when a chargeable item
     * stops charging.
     *
     * @worldMutation
     *
     * @earlyExecution
     *
     */
    unsubscribe(callback: (arg0: ItemStopUseAfterEvent) => void): void;
}

/**
 * Contains information related to an item that has stopped
 * being used on a block. This event fires when a player
 * successfully uses an item or places a block by pressing the
 * Use Item / Place Block button. If multiple blocks are
 * placed, this event will only occur once at the beginning of
 * the block placement. Note: This event cannot be used with
 * Hoe or Axe items.
 */
export class ItemStopUseOnAfterEvent {
    private constructor();
    /**
     * @remarks
     * The block that the item is used on.
     *
     */
    readonly block: Block;
    /**
     * @remarks
     * The impacted item stack that is being used on a block.
     *
     */
    readonly itemStack?: ItemStack;
    /**
     * @remarks
     * Returns the source entity that triggered this item event.
     *
     */
    readonly source: Player;
}

/**
 * Manages callbacks that are connected to an item stops used
 * on a block event.
 */
export class ItemStopUseOnAfterEventSignal {
    private constructor();
    /**
     * @remarks
     * Adds a callback that will be called when an item stops being
     * used on a block.
     *
     * @worldMutation
     *
     * @earlyExecution
     *
     */
    subscribe(callback: (arg0: ItemStopUseOnAfterEvent) => void): (arg0: ItemStopUseOnAfterEvent) => void;
    /**
     * @remarks
     * Removes a callback from being called when an item is used on
     * a block.
     *
     * @worldMutation
     *
     * @earlyExecution
     *
     */
    unsubscribe(callback: (arg0: ItemStopUseOnAfterEvent) => void): void;
}

/**
 * Represents the type of an item - for example, Wool.
 */
export class ItemType {
    private constructor();
    /**
     * @remarks
     * Returns the identifier of the item type - for example,
     * 'minecraft:apple'.
     *
     */
    readonly id: string;
    /**
     * @remarks
     * Key for the localization of this ItemType's name used in
     * .lang files.
     *
     */
    readonly localizationKey: string;
}

/**
 * Returns the set of item types registered within Minecraft.
 */
export class ItemTypes {
    private constructor();
    /**
     * @remarks
     * Returns a specific item type, if available within Minecraft.
     *
     */
    static get(itemId: string): ItemType | undefined;
    /**
     * @remarks
     * Retrieves all available item types registered within
     * Minecraft.
     *
     */
    static getAll(): ItemType[];
}

/**
 * Contains information related to an item being used on a
 * block. This event fires when an item used by a player
 * successfully triggers an entity interaction.
 */
export class ItemUseAfterEvent {
    private constructor();
    /**
     * @remarks
     * The impacted item stack that is being used.
     *
     */
    itemStack: ItemStack;
    /**
     * @remarks
     * Returns the source entity that triggered this item event.
     *
     */
    readonly source: Player;
}

/**
 * Manages callbacks that are connected to an item use event.
 */
export class ItemUseAfterEventSignal {
    private constructor();
    /**
     * @remarks
     * Adds a callback that will be called when an item is used.
     *
     * @worldMutation
     *
     * @earlyExecution
     *
     */
    subscribe(callback: (arg0: ItemUseAfterEvent) => void): (arg0: ItemUseAfterEvent) => void;
    /**
     * @remarks
     * Removes a callback from being called when an item is used.
     *
     * @worldMutation
     *
     * @earlyExecution
     *
     */
    unsubscribe(callback: (arg0: ItemUseAfterEvent) => void): void;
}

/**
 * Contains information related to an item being used.
 */
// @ts-ignore Class inheritance allowed for native defined classes
export class ItemUseBeforeEvent extends ItemUseAfterEvent {
    private constructor();
    /**
     * @remarks
     * If set to true, this will cancel the item use behavior.
     *
     */
    cancel: boolean;
}

/**
 * Manages callbacks that fire before an item is used.
 */
export class ItemUseBeforeEventSignal {
    private constructor();
    /**
     * @remarks
     * Adds a callback that will be called before an item is used.
     *
     * @worldMutation
     *
     * @earlyExecution
     *
     * @param callback
     * This closure is called with restricted-execution privilege.
     * @returns
     * Closure that is called with restricted-execution privilege.
     */
    subscribe(callback: (arg0: ItemUseBeforeEvent) => void): (arg0: ItemUseBeforeEvent) => void;
    /**
     * @remarks
     * Removes a callback from being called before an item is used.
     *
     * @worldMutation
     *
     * @earlyExecution
     *
     * @param callback
     * This closure is called with restricted-execution privilege.
     */
    unsubscribe(callback: (arg0: ItemUseBeforeEvent) => void): void;
}

/**
 * Contains information regarding the use of an item on a
 * block.
 */
export class ItemUseOnEvent {
    private constructor();
    /**
     * @remarks
     * The block impacted by this event.
     *
     */
    readonly block: Block;
    /**
     * @remarks
     * The face of the block that the item was used on.
     *
     */
    readonly blockFace: Direction;
    /**
     * @remarks
     * Location relative to the bottom north-west corner of the
     * block that the item was used on.
     *
     */
    readonly faceLocation: Vector3;
    /**
     * @remarks
     * The item stack used on the block.
     *
     */
    readonly itemStack: ItemStack;
}

/**
 * Loot item condition that checks whether or not the drop
 * source was killed by a specific type of entity.
 */
// @ts-ignore Class inheritance allowed for native defined classes
export class KilledByEntityCondition extends LootItemCondition {
    private constructor();
    /**
     * @remarks
     * The entity type required for this condition to pass.
     * Example: 'minecraft:skeleton'.
     *
     */
    readonly entityType: string;
}

/**
 * Loot item condition that checks whether or not the source of
 * the loot drop was killed by the player.
 */
// @ts-ignore Class inheritance allowed for native defined classes
export class KilledByPlayerCondition extends LootItemCondition {
    private constructor();
}

/**
 * Loot item condition that checks whether or not the source of
 * the loot drop was killed by the player or any of the
 * player's pets.
 */
// @ts-ignore Class inheritance allowed for native defined classes
export class KilledByPlayerOrPetsCondition extends LootItemCondition {
    private constructor();
}

/**
 * Contains information related to changes to a lever
 * activating or deactivating.
 * @seeExample leverActionEvent.ts
 */
// @ts-ignore Class inheritance allowed for native defined classes
export class LeverActionAfterEvent extends BlockEvent {
    private constructor();
    /**
     * @remarks
     * True if the lever is activated (that is, transmitting
     * power).
     *
     */
    readonly isPowered: boolean;
    /**
     * @remarks
     * Optional player that triggered the lever activation.
     *
     */
    readonly player: Player;
}

/**
 * Manages callbacks that are connected to lever moves
 * (activates or deactivates).
 * @seeExample leverActionEvent.ts
 */
export class LeverActionAfterEventSignal {
    private constructor();
    /**
     * @remarks
     * Adds a callback that will be called when a lever is moved
     * (activates or deactivates).
     *
     * @worldMutation
     *
     * @earlyExecution
     *
     */
    subscribe(callback: (arg0: LeverActionAfterEvent) => void): (arg0: LeverActionAfterEvent) => void;
    /**
     * @remarks
     * Removes a callback from being called when a lever is moved
     * (activates or deactivates).
     *
     * @worldMutation
     *
     * @earlyExecution
     *
     */
    unsubscribe(callback: (arg0: LeverActionAfterEvent) => void): void;
}

/**
 * A spline that linearly interpolates between points.
 */
export class LinearSpline {
    /**
     * @remarks
     * Control points for the Linear spline.
     *
     * @worldMutation
     *
     */
    controlPoints: Vector3[];
}

/**
 * Volume composed of an unordered container of unique block
 * locations.
 */
// @ts-ignore Class inheritance allowed for native defined classes
export class ListBlockVolume extends BlockVolumeBase {
    /**
     * @remarks
     * Creates a new instance of ListBlockVolume.
     *
     * @param locations
     * Initial array of block locations that ListBlockVolume will
     * be constructed with.
     */
    constructor(locations: Vector3[]);
    /**
     * @remarks
     * Insert block locations into container.
     *
     * @param locations
     * Array of block locations to be inserted into container.
     */
    add(locations: Vector3[]): void;
    /**
     * @remarks
     * Remove block locations from container.
     *
     * @param locations
     * Array of block locations to be removed from container.
     */
    remove(locations: Vector3[]): void;
}

/**
 * Waypoint that points to a fixed location in the world.
 * Unlike entity waypoints, location waypoints always remain
 * valid and their position can be updated.
 */
// @ts-ignore Class inheritance allowed for native defined classes
export class LocationWaypoint extends Waypoint {
    /**
     * @throws This function can throw errors.
     *
     * {@link InvalidWaypointTextureSelectorError}
     */
    constructor(dimensionLocation: DimensionLocation, textureSelector: WaypointTextureSelector, color?: RGB);
    /**
     * @remarks
     * Updates the dimension and location that this waypoint points
     * to.
     *
     * @worldMutation
     *
     * @param dimensionLocation
     * The new {@link DimensionLocation} (dimension and
     * coordinates) for the waypoint.
     */
    setDimensionLocation(dimensionLocation: DimensionLocation): void;
}

/**
 * Manages the collection of waypoints displayed on a player's
 * locator bar. Allows adding, removing, and querying waypoints
 * with a maximum capacity limit.
 *
 * Invalid waypoints in the locator bar will be automatically
 * removed in the next tick. This includes waypoints tied to
 * entities that have been removed from the world.
 *
 * Note: You can control whether vanilla player waypoints are
 * automatically added to the locator bar using the
 * `playerWaypoints` {@link GameRule}. Accepted values are
 * `off` (players are not shown on the locator bar) and
 * `everyone` (all players are visible on the locator bar).
 *
 * Note: You can only modify, remove, or query waypoints that
 * were added by this pack.
 * @seeExample sharedWaypoint.ts
 */
export class LocatorBar {
    private constructor();
    /**
     * @remarks
     * The current number of waypoints in the locator bar.
     *
     */
    readonly count: number;
    /**
     * @remarks
     * The maximum number of waypoints that can be added to the
     * locator bar.
     *
     */
    readonly maxCount: number;
    /**
     * @remarks
     * Adds a waypoint to the locator bar. Throws an error if the
     * waypoint already exists, the maximum waypoint limit has been
     * reached, or the waypoint is invalid.
     *
     * @worldMutation
     *
     * @param waypoint
     * The {@link Waypoint} to add to the locator bar.
     * @throws This function can throw errors.
     *
     * {@link EngineError}
     *
     * {@link InvalidWaypointError}
     *
     * {@link LocatorBarError}
     */
    addWaypoint(waypoint: Waypoint): void;
    /**
     * @remarks
     * Returns an array of all waypoints currently in the locator
     * bar.
     *
     * @worldMutation
     *
     */
    getAllWaypoints(): Waypoint[];
    /**
     * @remarks
     * Checks whether the specified waypoint exists in the locator
     * bar.
     *
     * @worldMutation
     *
     * @param waypoint
     * The {@link Waypoint} to check for.
     */
    hasWaypoint(waypoint: Waypoint): boolean;
    /**
     * @remarks
     * Removes all waypoints from the locator bar, clearing it
     * completely.
     *
     * @worldMutation
     *
     * @throws This function can throw errors.
     *
     * {@link EngineError}
     */
    removeAllWaypoints(): void;
    /**
     * @remarks
     * Removes a specific waypoint from the locator bar. Returns an
     * error if the waypoint does not exist in the locator bar.
     *
     * @worldMutation
     *
     * @param waypoint
     * The {@link Waypoint} to remove from the locator bar.
     * @throws This function can throw errors.
     *
     * {@link EngineError}
     *
     * {@link LocatorBarError}
     */
    removeWaypoint(waypoint: Waypoint): void;
}

/**
 * Loot item function that drops extra items if the provided
 * tool has the looting enchant.
 */
// @ts-ignore Class inheritance allowed for native defined classes
export class LootingEnchantFunction extends LootItemFunction {
    private constructor();
    /**
     * @remarks
     * The value range from which the function randomly chooses the
     * number of extra items to drop. Contains minimum and maximum
     * values.
     *
     */
    readonly count: NumberRange;
}

/**
 * Represents a loot pool entry containing an item to drop.
 */
// @ts-ignore Class inheritance allowed for native defined classes
export class LootItem extends LootPoolEntry {
    private constructor();
    /**
     * @beta
     */
    readonly conditions: LootItemCondition[];
    readonly functions: LootItemFunction[];
    /**
     * @remarks
     * The name of the item contained in this entry.
     *
     */
    readonly name?: ItemType;
}

/**
 * An abstract base class from which all loot item conditions
 * are derived. A loot item condition is a set of rules or
 * requirements which must be met for a loot drop to happen.
 */
export class LootItemCondition {
    private constructor();
}

/**
 * An abstract base class from which all loot item functions
 * are derived. Loot item functions can modify loot drops in a
 * variety of ways as they happen, optionally dependent on a
 * set of conditions which must be met.
 */
export class LootItemFunction {
    private constructor();
    readonly conditions: LootItemCondition[];
}

/**
 * A collection of entries which individually determine loot
 * drops. Can contain values determining drop outcomes,
 * including rolls, bonus rolls and tiers.
 */
export class LootPool {
    private constructor();
    /**
     * @remarks
     * Returns the number of extra times a loot pool will be rolled
     * based on the player's luck level, represented as a range
     * from minimum to maximum rolls.
     *
     */
    readonly bonusRolls: NumberRange;
    readonly conditions: LootItemCondition[];
    /**
     * @remarks
     * Gets a complete list of all loot pool entries contained in
     * the loot pool.
     *
     */
    readonly entries: LootPoolEntry[];
    /**
     * @remarks
     * Returns the number of times a loot pool will be rolled,
     * represented as a range from minimum to maximum rolls.
     *
     */
    readonly rolls: NumberRange;
    /**
     * @remarks
     * Gets the loot pool tier values for a given table if they
     * exist.
     *
     */
    readonly tiers?: LootPoolTiers;
}

/**
 * Represents one entry within Loot Table, which describes one
 * possible drop when a loot drop occurs. Can contain an item,
 * another loot table, a path to another loot table, or an
 * empty drop.
 */
export class LootPoolEntry {
    private constructor();
    /**
     * @remarks
     * Gets the quality of a given loot pool entry.
     *
     */
    readonly quality: number;
    /**
     * @remarks
     * Gets the subtable of a given loot pool entry.
     *
     */
    readonly subTable?: LootPoolEntry;
    /**
     * @remarks
     * Gets the weight of a given loot pool entry.
     *
     */
    readonly weight: number;
}

/**
 * Represents the values which determine loot drops in a tiered
 * loot pool. Potential drops from tiered loot pools are
 * ordered, and chosen via logic controlled by the values in
 * this object.
 */
export class LootPoolTiers {
    private constructor();
    /**
     * @remarks
     * The chance for each bonus roll attempt to upgrade the tier
     * of the dropped item.
     *
     */
    readonly bonusChance: number;
    /**
     * @remarks
     * The number of attempts for the loot drop to upgrade its
     * tier, thereby incrementing its position in the loot pool
     * entry array, resulting in a higher tier drop.
     *
     */
    readonly bonusRolls: number;
    /**
     * @remarks
     * Represents the upper bound for the starting point in
     * determining which tier of loot to drop. The lower bound is
     * always 1. For example, a value of 3 would result in the tier
     * drop logic starting at a randomly selected position in the
     * loot pool entry array between 1 and 3.
     *
     */
    readonly initialRange: number;
}

/**
 * Represents a single Loot Table, which determines what items
 * are generated when killing a mob, breaking a block, filling
 * a container, and more.
 */
export class LootTable {
    private constructor();
    /**
     * @remarks
     * Returns the path to the JSON file that represents this loot
     * table. Does not include file extension, or 'loot_tables/'
     * folder prefix. Example: `entities/creeper`.
     *
     */
    readonly path: string;
    /**
     * @remarks
     * Returns the array of loot pools on a given loot table.
     *
     */
    readonly pools: LootPool[];
}

/**
 * Represents a loot pool entry containing another separate,
 * nested loot table.
 */
// @ts-ignore Class inheritance allowed for native defined classes
export class LootTableEntry extends LootPoolEntry {
    private constructor();
    /**
     * @remarks
     * Gets the loot table stored as a subtable in the parent loot
     * pool.
     *
     */
    readonly lootTable: LootTable;
}

/**
 * Manager for Loot Table related APIs. Allows for generation
 * of drops from blocks and entities according to their loot
 * tables.
 */
export class LootTableManager {
    private constructor();
    /**
     * @remarks
     * Generates loot from a given block as if it had been mined.
     *
     * @param block
     * The block to generate loot from.
     * @param tool
     * Optional. The tool to use in the looting operation.
     * @returns
     * An array of item stacks dropped from the loot drop event.
     * Can be empty if no loot dropped, or undefined if the
     * provided tool is insufficient to mine the block.
     * @throws
     * Throws if the block is in an unloaded chunk, or if the
     * block's position is outside of world bounds.
     *
     * {@link LocationInUnloadedChunkError}
     *
     * {@link LocationOutOfWorldBoundariesError}
     *
     * {@link UnloadedChunksError}
     */
    generateLootFromBlock(block: Block, tool?: ItemStack): ItemStack[] | undefined;
    /**
     * @remarks
     * Generates loot from a given block permutation as if it had
     * been mined.
     *
     * @param tool
     * Optional. The tool to use in the looting operation.
     * @returns
     * An array of item stacks dropped from the loot drop event.
     * Can be empty if no loot dropped, or undefined if the
     * provided tool is insufficient to mine the block.
     */
    generateLootFromBlockPermutation(blockPermutation: BlockPermutation, tool?: ItemStack): ItemStack[] | undefined;
    /**
     * @remarks
     * Generates loot from a given block type as if it had been
     * mined.
     *
     * @param tool
     * Optional. The tool to use in the looting operation.
     * @returns
     * An array of item stacks dropped from the loot drop event.
     * Can be empty if no loot dropped, or undefined if the
     * provided tool is insufficient to mine the block.
     */
    generateLootFromBlockType(scriptBlockType: BlockType, tool?: ItemStack): ItemStack[] | undefined;
    /**
     * @remarks
     * Generates loot from given a entity as if it had been killed.
     *
     * @param tool
     * Optional. The tool to use in the looting operation.
     * @returns
     * An array of item stacks dropped from the loot drop event.
     * Can be empty if no loot dropped, or undefined if the entity
     * was invalid.
     * @throws This function can throw errors.
     *
     * {@link InvalidEntityError}
     */
    generateLootFromEntity(entity: Entity, tool?: ItemStack): ItemStack[] | undefined;
    /**
     * @remarks
     * Generates loot from given a entity type as if it had been
     * killed.
     *
     * @param tool
     * Optional. The tool to use in the looting operation.
     * @returns
     * An array of item stacks dropped from the loot drop event.
     * Can be empty if no loot dropped.
     */
    generateLootFromEntityType(entityType: EntityType, tool?: ItemStack): ItemStack[] | undefined;
    /**
     * @remarks
     * Generates loot from a given LootTable.
     *
     * @param tool
     * Optional. The tool to use in the looting operation.
     * @returns
     * An array of item stacks dropped from the loot drop event.
     * Can be empty if no loot dropped, or undefined if the
     * provided tool is insufficient to mine the block.
     */
    generateLootFromTable(lootTable: LootTable, tool?: ItemStack): ItemStack[] | undefined;
    /**
     * @remarks
     * Retrieves a single loot table from the level's current
     * registry.
     *
     * @param path
     * Path to the table to retrieve. Does not include file
     * extension, or 'loot_tables/' folder prefix. Example:
     * `entities/creeper`.
     * @returns
     * Returns a LootTable if one is found, or `undefined` if the
     * provided path does not correspond to an existing loot table.
     */
    getLootTable(path: string): LootTable | undefined;
}

/**
 * Represents a loot pool entry containing a reference to
 * another loot table, described by its path.
 */
// @ts-ignore Class inheritance allowed for native defined classes
export class LootTableReference extends LootPoolEntry {
    private constructor();
    /**
     * @remarks
     * The path to the referenced loot table. Example:
     * `loot_tables/chests/village/village_bundle.json`
     *
     */
    readonly path: string;
}

/**
 * Loot item condition that checks whether an appropriate tool
 * was used to trigger the loot event. Can describe item type,
 * count, durability, enchantments, or arrays of item tags to
 * compare against.
 */
// @ts-ignore Class inheritance allowed for native defined classes
export class MatchToolCondition extends LootItemCondition {
    private constructor();
    /**
     * @remarks
     * The stack size, or count, required for this condition to
     * pass.
     *
     */
    readonly count: NumberRange;
    /**
     * @remarks
     * The durability value required for this condition to pass.
     *
     */
    readonly durability: NumberRange;
    /**
     * @remarks
     * Array of enchantments required for this condition to pass.
     *
     */
    readonly enchantments: EnchantInfo[];
    /**
     * @remarks
     * The name of the tool item required for this condition to
     * pass.
     *
     */
    readonly itemName: string;
    /**
     * @remarks
     * Array of item tags which ALL must be matched for this
     * condition to pass.
     *
     */
    readonly itemTagsAll: string[];
    /**
     * @remarks
     * Array of item tags, from which at least 1 must be matched
     * for this condition to pass.
     *
     */
    readonly itemTagsAny: string[];
    /**
     * @remarks
     * Array of item tags, from which exactly zero must match for
     * this condition to pass.
     *
     */
    readonly itemTagsNone: string[];
}

/**
 * @beta
 * A specific currently-internal event used for passing
 * messages from client to server.
 */
export class MessageReceiveAfterEvent {
    private constructor();
    /**
     * @remarks
     * The message identifier.
     *
     */
    readonly id: string;
    /**
     * @remarks
     * The message.
     *
     */
    readonly message: string;
    /**
     * @remarks
     * The player who sent the message.
     *
     */
    readonly player: Player;
}

/**
 * Contains a set of additional variable values for further
 * defining how rendering and animations function.
 */
export class MolangVariableMap {
    /**
     * @remarks
     * Adds the following variables to Molang:
     * - `<variable_name>.r` - Red color value [0-1]
     * - `<variable_name>.g` - Green color value [0-1]
     * - `<variable_name>.b` - Blue color value [0-1]
     *
     * @throws This function can throw errors.
     */
    setColorRGB(variableName: string, color: RGB): void;
    /**
     * @remarks
     * Adds the following variables to Molang:
     * - `<variable_name>.r` - Red color value [0-1]
     * - `<variable_name>.g` - Green color value [0-1]
     * - `<variable_name>.b` - Blue color value [0-1]
     * - `<variable_name>.a` - Alpha (transparency) color value
     * [0-1]
     *
     * @throws This function can throw errors.
     */
    setColorRGBA(variableName: string, color: RGBA): void;
    /**
     * @remarks
     * Sets a numeric (decimal) value within the Molang variable
     * map.
     *
     * @param variableName
     * Name of the float-based number to set.
     * @param number
     * Value for the Molang-based variable to set.
     * @throws This function can throw errors.
     */
    setFloat(variableName: string, number: number): void;
    /**
     * @remarks
     * Adds the following variables to Molang:
     * - `<variable_name>.speed` - Speed number provided
     * - `<variable_name>.direction_x` - X value from the {@link
     * Vector3} provided
     * - `<variable_name>.direction_y` - Y value from the {@link
     * Vector3} provided
     * - `<variable_name>.direction_z` - Z value from the {@link
     * Vector3} provided
     *
     * @throws This function can throw errors.
     */
    setSpeedAndDirection(variableName: string, speed: number, direction: Vector3): void;
    /**
     * @remarks
     * Adds the following variables to Molang:
     * - `<variable_name>.x` - X value from the {@link Vector3}
     * provided
     * - `<variable_name>.y` - Y value from the {@link Vector3}
     * provided
     * - `<variable_name>.z` - Z value from the {@link Vector3}
     * provided
     *
     * @throws This function can throw errors.
     */
    setVector3(variableName: string, vector: Vector3): void;
}

/**
 * @beta
 * Pack setting name and value that changed.
 */
export class PackSettingChangeAfterEvent {
    private constructor();
    /**
     * @remarks
     * The name of the setting.
     *
     */
    readonly settingName: string;
    /**
     * @remarks
     * The value of the setting.
     *
     */
    readonly settingValue: boolean | number | string;
}

/**
 * @beta
 */
export class PackSettingChangeAfterEventSignal {
    private constructor();
    /**
     * @remarks
     * @worldMutation
     *
     * @earlyExecution
     *
     */
    subscribe(callback: (arg0: PackSettingChangeAfterEvent) => void): (arg0: PackSettingChangeAfterEvent) => void;
    /**
     * @remarks
     * @worldMutation
     *
     * @earlyExecution
     *
     */
    unsubscribe(callback: (arg0: PackSettingChangeAfterEvent) => void): void;
}

/**
 * Loot item condition that checks whether the looting entity
 * is currently a passenger of a specific type of entity.
 */
// @ts-ignore Class inheritance allowed for native defined classes
export class PassengerOfEntityCondition extends LootItemCondition {
    private constructor();
    /**
     * @remarks
     * The entity type required for this condition to pass.
     *
     */
    readonly entityType: string;
}

/**
 * Contains information related to changes to a piston
 * expanding or retracting.
 * @seeExample pistonAfterEvent.ts
 */
// @ts-ignore Class inheritance allowed for native defined classes
export class PistonActivateAfterEvent extends BlockEvent {
    private constructor();
    /**
     * @remarks
     * True if the piston is the process of expanding.
     *
     */
    readonly isExpanding: boolean;
    /**
     * @remarks
     * Contains additional properties and details of the piston.
     *
     */
    readonly piston: BlockPistonComponent;
}

/**
 * Manages callbacks that are connected to piston activations.
 */
export class PistonActivateAfterEventSignal {
    private constructor();
    /**
     * @remarks
     * @worldMutation
     *
     * @earlyExecution
     *
     * @seeExample pistonAfterEvent.ts
     */
    subscribe(callback: (arg0: PistonActivateAfterEvent) => void): (arg0: PistonActivateAfterEvent) => void;
    /**
     * @remarks
     * Removes a callback from being called when a piston expands
     * or retracts.
     *
     * @worldMutation
     *
     * @earlyExecution
     *
     */
    unsubscribe(callback: (arg0: PistonActivateAfterEvent) => void): void;
}

/**
 * Represents a player within the world.
 */
// @ts-ignore Class inheritance allowed for native defined classes
export class Player extends Entity {
    private constructor();
    /**
     * @remarks
     * The player's Camera.
     *
     * @throws This property can throw when used.
     */
    readonly camera: Camera;
    /**
     * @beta
     * @remarks
     * The player's chat display name, composed from {@link
     * Player.chatNamePrefix} + {@link Player.name} + {@link
     * Player.chatNameSuffix}. This is the name shown as the author
     * of chat messages sent by this player. To change the name
     * shown above the player's head, use {@link Entity.nameTag}.
     *
     * @throws This property can throw when used.
     *
     * {@link InvalidEntityError}
     */
    readonly chatDisplayName: string;
    /**
     * @beta
     * @remarks
     * An optional string that, when set, is prepended to the text
     * of chat messages sent by this player. Useful for applying
     * formatting or color codes to a player's messages (e.g., '§a'
     * to make their messages green). Does not affect the player's
     * name display - use {@link Player.chatNamePrefix} for the
     * name shown in chat, or {@link Entity.nameTag} for the name
     * above the player's head. Set to undefined to clear.
     *
     * @worldMutation
     *
     */
    chatMessagePrefix?: string;
    /**
     * @beta
     * @remarks
     * An optional string that, when set, is prepended to the
     * player's name in chat messages. Does not affect the name tag
     * above the player's head or the player list - use {@link
     * Entity.nameTag} for that. To prefix the message text itself,
     * use {@link Player.chatMessagePrefix}. Set to undefined to
     * clear.
     *
     * @worldMutation
     *
     */
    chatNamePrefix?: string;
    /**
     * @beta
     * @remarks
     * An optional string that, when set, is appended to the
     * player's name in chat messages. Does not affect the name tag
     * above the player's head or the player list - use {@link
     * Entity.nameTag} for that. See also {@link
     * Player.chatNamePrefix}. Set to undefined to clear.
     *
     * @worldMutation
     *
     */
    chatNameSuffix?: string;
    /**
     * @remarks
     * Contains the player's device information.
     *
     * @throws This property can throw when used.
     */
    readonly clientSystemInfo: ClientSystemInfo;
    /**
     * @remarks
     * @worldMutation
     *
     */
    commandPermissionLevel: CommandPermissionLevel;
    /**
     * @beta
     * @remarks
     * Contains methods for manipulating the render distance fog
     * settings of a Player.
     *
     */
    readonly fogSettings: FogSettings;
    /**
     * @remarks
     * Gets the current graphics mode of the player's client. This
     * can be changed in the Video section of the settings menu
     * based on what hardware is available.
     *
     * @throws This property can throw when used.
     *
     * {@link InvalidEntityError}
     */
    readonly graphicsMode: GraphicsMode;
    /**
     * @remarks
     * Contains the player's input information.
     *
     */
    readonly inputInfo: InputInfo;
    /**
     * @remarks
     * Input permissions of the player.
     *
     */
    readonly inputPermissions: PlayerInputPermissions;
    /**
     * @remarks
     * If true, the player is currently emoting.
     *
     * @throws This property can throw when used.
     */
    readonly isEmoting: boolean;
    /**
     * @remarks
     * Whether the player is flying. For example, in Creative or
     * Spectator mode.
     *
     * @throws This property can throw when used.
     */
    readonly isFlying: boolean;
    /**
     * @remarks
     * Whether the player is gliding with Elytra.
     *
     * @throws This property can throw when used.
     */
    readonly isGliding: boolean;
    /**
     * @remarks
     * Whether the player is jumping. This will remain true while
     * the player is holding the jump action.
     *
     * @throws This property can throw when used.
     */
    readonly isJumping: boolean;
    /**
     * @remarks
     * The current overall level for the player, based on their
     * experience.
     *
     * @throws This property can throw when used.
     */
    readonly level: number;
    /**
     * @remarks
     * The player's Locator Bar. This property is used for managing
     * waypoints displayed on the HUD.
     *
     */
    readonly locatorBar: LocatorBar;
    /**
     * @remarks
     * Name of the player.
     *
     * @throws This property can throw when used.
     */
    readonly name: string;
    /**
     * @remarks
     * Contains methods for manipulating the on-screen display of a
     * Player.
     *
     * @throws This property can throw when used.
     */
    readonly onScreenDisplay: ScreenDisplay;
    /**
     * @beta
     * @remarks
     * An identifier that can be used to identify a player across
     * sessions.
     *
     * @throws This property can throw when used.
     *
     * {@link EngineError}
     *
     * {@link InvalidEntityError}
     */
    readonly persistentId: string;
    /**
     * @throws This property can throw when used.
     *
     * {@link InvalidEntityError}
     */
    readonly playerPermissionLevel: PlayerPermissionLevel;
    /**
     * @remarks
     * @worldMutation
     *
     */
    selectedSlotIndex: number;
    /**
     * @remarks
     * The overall total set of experience needed to achieve the
     * next level for a player.
     *
     * @throws This property can throw when used.
     */
    readonly totalXpNeededForNextLevel: number;
    /**
     * @remarks
     * The current set of experience achieved for the player.
     *
     * @throws This property can throw when used.
     */
    readonly xpEarnedAtCurrentLevel: number;
    /**
     * @remarks
     * Adds/removes experience to/from the Player and returns the
     * current experience of the Player.
     *
     * @worldMutation
     *
     * @param amount
     * Amount of experience to add. Note that this can be negative.
     * Min/max bounds at -2^24 ~ 2^24
     * Bounds: [-16777216, 16777216]
     * @returns
     * Returns the current experience of the Player.
     * @throws This function can throw errors.
     */
    addExperience(amount: number): number;
    /**
     * @remarks
     * Adds/removes level to/from the Player and returns the
     * current level of the Player.
     *
     * @worldMutation
     *
     * @param amount
     * Amount to add to the player. Min/max bounds at -2^24 ~ 2^24
     * Bounds: [-16777216, 16777216]
     * @returns
     * Returns the current level of the Player.
     * @throws This function can throw errors.
     */
    addLevels(amount: number): number;
    /**
     * @remarks
     * For this player, removes all overrides of any Entity
     * Properties on the target Entity. This change is not applied
     * until the next tick and will not apply to other players.
     *
     * @worldMutation
     *
     * @param targetEntity
     * The Entity or the ID of the Entity whose Entity Property
     * overrides are being cleared.
     * @throws
     * Throws if the Entity or Entity ID is invalid.
     */
    clearPropertyOverridesForEntity(targetEntity: Entity | string): void;
    /**
     * @beta
     * @remarks
     * Eats an item, providing the item's hunger and saturation
     * effects to the player. Can only be used on food items.
     *
     * @worldMutation
     *
     * @param itemStack
     * The item to eat.
     * @throws
     * Throws if the item is not a food item.
     */
    eatItem(itemStack: ItemStack): void;
    /**
     * @remarks
     * The player's aim-assist settings.
     *
     */
    getAimAssist(): PlayerAimAssist;
    /**
     * @remarks
     * Returns the player's current control scheme.
     *
     * @throws This function can throw errors.
     *
     * {@link InvalidEntityError}
     */
    getControlScheme(): ControlScheme;
    /**
     * @remarks
     * Retrieves the active gamemode for this player, if specified.
     *
     * @throws This function can throw errors.
     */
    getGameMode(): GameMode;
    /**
     * @remarks
     * Gets the current item cooldown time for a particular
     * cooldown category.
     *
     * @param cooldownCategory
     * Specifies the cooldown category to retrieve the current
     * cooldown for.
     * @throws This function can throw errors.
     */
    getItemCooldown(cooldownCategory: string): number;
    /**
     * @beta
     * @remarks
     * Gets the player's ping in milliseconds.
     *
     * @returns
     * The player's ping in milliseconds.
     * @throws This function can throw errors.
     *
     * {@link EngineError}
     *
     * {@link InvalidEntityError}
     */
    getPing(): number;
    /**
     * @remarks
     * Gets the current spawn point of the player.
     *
     * @throws This function can throw errors.
     */
    getSpawnPoint(): DimensionLocation | undefined;
    /**
     * @beta
     * @remarks
     * Returns the split screen slot of the player.
     *
     * @returns
     * The split screen slot of the player or undefined if the
     * player is not in a split screen session.
     * @throws This function can throw errors.
     *
     * {@link EngineError}
     *
     * {@link InvalidEntityError}
     */
    getSplitScreenSlot(): PlayerSplitScreenSlot | undefined;
    /**
     * @remarks
     *  Gets the total experience of the Player.
     *
     * @throws This function can throw errors.
     */
    getTotalXp(): number;
    /**
     * @remarks
     * Plays a music track that only this particular player can
     * hear.
     *
     * @worldMutation
     *
     * @param trackId
     * Identifier of the music track to play.
     * @param musicOptions
     * Additional options for the music track.
     * @throws This function can throw errors.
     */
    playMusic(trackId: string, musicOptions?: MusicOptions): void;
    /**
     * @remarks
     * Plays a sound that only this particular player can hear.
     *
     * @worldMutation
     *
     * @param soundOptions
     * Additional optional options for the sound.
     * @throws This function can throw errors.
     *
     * {@link EngineError}
     *
     * {@link Error}
     * @seeExample playMusicAndSound.ts
     */
    playSound(soundId: SoundDefinition | string, soundOptions?: PlayerSoundOptions): SoundInstance;
    /**
     * @beta
     * @remarks
     * This is an internal-facing method for posting a system
     * message to downstream clients.
     *
     * @worldMutation
     *
     * @throws This function can throw errors.
     */
    postClientMessage(id: string, value: string): void;
    /**
     * @remarks
     * Queues an additional music track that only this particular
     * player can hear. If a track is not playing, a music track
     * will play.
     *
     * @worldMutation
     *
     * @param trackId
     * Identifier of the music track to play.
     * @param musicOptions
     * Additional options for the music track.
     * @throws
     * An error will be thrown if volume is less than 0.0.
     * An error will be thrown if fade is less than 0.0.
     *
     */
    queueMusic(trackId: string, musicOptions?: MusicOptions): void;
    /**
     * @remarks
     * For this player, removes the override on an Entity Property.
     * This change is not applied until the next tick and will not
     * apply to other players.
     *
     * @worldMutation
     *
     * @param targetEntity
     * The Entity whose Entity Property override is being removed.
     * @param identifier
     * The Entity Property identifier.
     * @throws
     * Throws if the entity is invalid.
     * Throws if an invalid identifier is provided.
     * Throws if the provided value type does not match the
     * property type.
     */
    removePropertyOverrideForEntity(targetEntity: Entity, identifier: string): void;
    /**
     * @remarks
     * Resets the level of the player.
     *
     * @worldMutation
     *
     * @throws This function can throw errors.
     */
    resetLevel(): void;
    /**
     * @remarks
     * Sends a message to the player.
     *
     * @param message
     * The message to be displayed.
     * @throws
     * This method can throw if the provided {@link RawMessage} is
     * in an invalid format. For example, if an empty `name` string
     * is provided to `score`.
     *
     * {@link InvalidEntityError}
     *
     * {@link RawMessageError}
     * @seeExample nestedTranslation.ts
     * @seeExample scoreWildcard.ts
     * @seeExample sendBasicMessage.ts
     * @seeExample sendPlayerMessages.ts
     * @seeExample sendTranslatedMessage.ts
     */
    sendMessage(message: (RawMessage | string)[] | RawMessage | string): void;
    /**
     * @remarks
     * Set a player's control scheme. The player's active camera
     * preset must be set by scripts like with camera.setCamera()
     * or commands.
     *
     * @worldMutation
     *
     * @param controlScheme
     * Control scheme type. If this argument is undefined, this
     * method will clear the player's control scheme back to the
     * player camera's default control scheme.
     * @returns
     * Returns nothing if the control scheme was added or updated
     * successfully. This can throw an InvalidArgumentError if the
     * control scheme is not allowed by the player's current
     * camera.
     * @throws This function can throw errors.
     *
     * {@link EngineError}
     *
     * {@link InvalidArgumentError}
     *
     * {@link InvalidEntityError}
     */
    setControlScheme(controlScheme?: ControlScheme): void;
    /**
     * @remarks
     * Sets a gamemode override for this player.
     *
     * @worldMutation
     *
     * @param gameMode
     * Active gamemode.
     * @throws This function can throw errors.
     */
    setGameMode(gameMode?: GameMode): void;
    /**
     * @remarks
     * For this player, overrides an Entity Property on the target
     * Entity to the provided value. This property must be client
     * synced. This change is not applied until the next tick and
     * will not apply to other players.
     *
     * @worldMutation
     *
     * @param targetEntity
     * The Entity whose Entity Property is being overriden.
     * @param identifier
     * The Entity Property identifier.
     * @param value
     * The override value. The provided type must be compatible
     * with the type specified in the entity's definition.
     * @throws
     * Throws if the entity is invalid.
     * Throws if an invalid identifier is provided.
     * Throws if the provided value type does not match the
     * property type.
     * Throws if the provided value is outside the expected range
     * (int, float properties).
     * Throws if the provided string value does not match the set
     * of accepted enum values (enum properties)
     */
    setPropertyOverrideForEntity(targetEntity: Entity, identifier: string, value: boolean | number | string): void;
    /**
     * @remarks
     * Sets the current starting spawn point for this particular
     * player.
     *
     * @worldMutation
     *
     * @throws This function can throw errors.
     *
     * {@link Error}
     *
     * {@link LocationOutOfWorldBoundariesError}
     */
    setSpawnPoint(spawnPoint?: DimensionLocation): void;
    /**
     * @remarks
     * Creates a new particle emitter at a specified location in
     * the world. Only visible to the target player.
     *
     * @worldMutation
     *
     * @param effectName
     * Identifier of the particle to create.
     * @param location
     * The location at which to create the particle emitter.
     * @param molangVariables
     * A set of optional, customizable variables that can be
     * adjusted for this particle.
     * @throws This function can throw errors.
     *
     * {@link Error}
     *
     * {@link LocationInUnloadedChunkError}
     *
     * {@link LocationOutOfWorldBoundariesError}
     * @seeExample spawnParticle.ts bd8c7a07
     */
    spawnParticle(effectName: string, location: Vector3, molangVariables?: MolangVariableMap): void;
    /**
     * @remarks
     * Sets the item cooldown time for a particular cooldown
     * category.
     *
     * @worldMutation
     *
     * @param cooldownCategory
     * Specifies the cooldown category to retrieve the current
     * cooldown for.
     * @param tickDuration
     * Duration in ticks of the item cooldown.
     * Bounds: [0, 32767]
     * @throws This function can throw errors.
     */
    startItemCooldown(cooldownCategory: string, tickDuration: number): void;
    /**
     * @beta
     * @remarks
     * Stops all sounds from playing for this particular player.
     *
     * @worldMutation
     *
     * @throws This function can throw errors.
     *
     * {@link InvalidEntityError}
     */
    stopAllSounds(): void;
    /**
     * @remarks
     * Stops any music tracks from playing for this particular
     * player.
     *
     * @worldMutation
     *
     * @throws This function can throw errors.
     */
    stopMusic(): void;
    /**
     * @beta
     * @remarks
     * Stops a sound from playing for this particular player.
     *
     * @worldMutation
     *
     * @param soundId
     * Identifier of the sound.
     * @throws This function can throw errors.
     *
     * {@link InvalidEntityError}
     */
    stopSound(soundId: string): void;
}

/**
 * A container for APIs related to player aim-assist.
 */
export class PlayerAimAssist {
    private constructor();
    /**
     * @remarks
     * The player's currently active aim-assist settings, or
     * undefined if not active.
     *
     */
    readonly settings?: PlayerAimAssistSettings;
    /**
     * @remarks
     * Sets the player's aim-assist settings.
     *
     * @worldMutation
     *
     * @param settings
     * Aim-assist settings to activate for the player, if undefined
     * aim-assist will be disabled.
     * @throws This function can throw errors.
     *
     * {@link ArgumentOutOfBoundsError}
     *
     * {@link EngineError}
     *
     * {@link Error}
     *
     * {@link InvalidArgumentError}
     *
     * {@link InvalidEntityError}
     *
     * {@link NamespaceNameError}
     */
    set(settings?: PlayerAimAssistSettings): void;
}

/**
 * Contains information regarding an event after a player
 * breaks a block.
 */
// @ts-ignore Class inheritance allowed for native defined classes
export class PlayerBreakBlockAfterEvent extends BlockEvent {
    private constructor();
    /**
     * @remarks
     * Returns permutation information about this block before it
     * was broken.
     *
     */
    readonly brokenBlockPermutation: BlockPermutation;
    /**
     * @remarks
     * The item stack that was used to break the block after the
     * block was broken, or undefined if empty hand.
     *
     */
    readonly itemStackAfterBreak?: ItemStack;
    /**
     * @remarks
     * The item stack that was used to break the block before the
     * block was broken, or undefined if empty hand.
     *
     */
    readonly itemStackBeforeBreak?: ItemStack;
    /**
     * @remarks
     * Player that broke the block for this event.
     *
     */
    readonly player: Player;
}

/**
 * Manages callbacks that are connected to when a player breaks
 * a block.
 */
export class PlayerBreakBlockAfterEventSignal {
    private constructor();
    /**
     * @remarks
     * Adds a callback that will be called when a block is broken
     * by a player.
     *
     * @worldMutation
     *
     * @earlyExecution
     *
     */
    subscribe(
        callback: (arg0: PlayerBreakBlockAfterEvent) => void,
        options?: BlockEventOptions,
    ): (arg0: PlayerBreakBlockAfterEvent) => void;
    /**
     * @remarks
     * Removes a callback from being called when a player breaks a
     * block.
     *
     * @worldMutation
     *
     * @earlyExecution
     *
     */
    unsubscribe(callback: (arg0: PlayerBreakBlockAfterEvent) => void): void;
}

/**
 * Contains information regarding an event before a player
 * breaks a block.
 */
// @ts-ignore Class inheritance allowed for native defined classes
export class PlayerBreakBlockBeforeEvent extends BlockEvent {
    private constructor();
    /**
     * @remarks
     * If set to true, cancels the block break event.
     *
     */
    cancel: boolean;
    /**
     * @remarks
     * The item stack that is being used to break the block, or
     * undefined if empty hand.
     *
     */
    itemStack?: ItemStack;
    /**
     * @remarks
     * Player breaking the block for this event.
     *
     */
    readonly player: Player;
}

/**
 * Manages callbacks that are connected to before a player
 * breaks a block.
 */
export class PlayerBreakBlockBeforeEventSignal {
    private constructor();
    /**
     * @remarks
     * Adds a callback that will be called before a block is broken
     * by a player.
     *
     * @worldMutation
     *
     * @earlyExecution
     *
     * @param callback
     * This closure is called with restricted-execution privilege.
     * @returns
     * Closure that is called with restricted-execution privilege.
     */
    subscribe(
        callback: (arg0: PlayerBreakBlockBeforeEvent) => void,
        options?: BlockEventOptions,
    ): (arg0: PlayerBreakBlockBeforeEvent) => void;
    /**
     * @remarks
     * Removes a callback from being called before a player breaks
     * a block.
     *
     * @worldMutation
     *
     * @earlyExecution
     *
     * @param callback
     * This closure is called with restricted-execution privilege.
     */
    unsubscribe(callback: (arg0: PlayerBreakBlockBeforeEvent) => void): void;
}

/**
 * Event data for when a player presses a button.
 */
export class PlayerButtonInputAfterEvent {
    private constructor();
    /**
     * @remarks
     * The button this event is about.
     *
     */
    readonly button: InputButton;
    /**
     * @remarks
     * The state that this button transferred to.
     *
     */
    readonly newButtonState: ButtonState;
    /**
     * @remarks
     * The player that performed the input event.
     *
     */
    readonly player: Player;
}

/**
 * Manages callbacks that are connected to player inputs.
 */
export class PlayerButtonInputAfterEventSignal {
    private constructor();
    /**
     * @remarks
     * Adds a callback that will be called after the player
     * performs an input.
     *
     * @worldMutation
     *
     * @earlyExecution
     *
     */
    subscribe(
        callback: (arg0: PlayerButtonInputAfterEvent) => void,
        options?: InputEventOptions,
    ): (arg0: PlayerButtonInputAfterEvent) => void;
    /**
     * @remarks
     * Removes a callback from being called after the player
     * performs an input.
     *
     * @worldMutation
     *
     * @earlyExecution
     *
     */
    unsubscribe(callback: (arg0: PlayerButtonInputAfterEvent) => void): void;
}

/**
 * @rc
 * Contains information regarding an event after a player
 * cancels breaking a block.
 */
// @ts-ignore Class inheritance allowed for native defined classes
export class PlayerCancelBreakingBlockAfterEvent extends BlockEvent {
    private constructor();
    /**
     * @remarks
     * The permutation of the block that the player cancelled
     * breaking.
     *
     */
    readonly blockPermutation: BlockPermutation;
    /**
     * @remarks
     * The progress of breaking the block when the player cancelled
     * in the exclusive range (0, 1).
     *
     */
    readonly breakProgress: number;
    /**
     * @remarks
     * The face of the block that was being broken.
     *
     */
    readonly face: Direction;
    /**
     * @remarks
     * The item stack that the player was using to break the block,
     * or undefined if empty hand.
     *
     */
    readonly heldItemStack?: ItemStack;
    /**
     * @remarks
     * Player that cancelled breaking the block for this event.
     *
     */
    readonly player: Player;
}

/**
 * @rc
 * Manages callbacks that are connected to when a player
 * cancels breaking a block.
 */
export class PlayerCancelBreakingBlockAfterEventSignal {
    private constructor();
    /**
     * @remarks
     * Adds a callback that will be called when a player cancels
     * breaking a block.
     *
     * @worldMutation
     *
     * @earlyExecution
     *
     */
    subscribe(
        callback: (arg0: PlayerCancelBreakingBlockAfterEvent) => void,
        options?: PlayerBreakingBlockEventOptions,
    ): (arg0: PlayerCancelBreakingBlockAfterEvent) => void;
    /**
     * @remarks
     * Removes a callback from being called when a player cancels
     * breaking a block.
     *
     * @worldMutation
     *
     * @earlyExecution
     *
     */
    unsubscribe(callback: (arg0: PlayerCancelBreakingBlockAfterEvent) => void): void;
}

/**
 * Represents the players cursor inventory. Used when moving
 * items between between containers in the inventory UI. Not
 * used with touch controls.
 */
// @ts-ignore Class inheritance allowed for native defined classes
export class PlayerCursorInventoryComponent extends EntityComponent {
    private constructor();
    /**
     * @remarks
     * The ItemStack currently in the players cursor inventory.
     *
     * @throws This property can throw when used.
     */
    readonly item?: ItemStack;
    static readonly componentId = 'minecraft:cursor_inventory';
    /**
     * @remarks
     * Clears the players cursor inventory.
     *
     * @worldMutation
     *
     * @throws This function can throw errors.
     */
    clear(): void;
}

/**
 * Contains information related to changes to a player's
 * dimension having been changed.
 */
export class PlayerDimensionChangeAfterEvent {
    private constructor();
    /**
     * @remarks
     * The dimension the player is changing from.
     *
     */
    readonly fromDimension: Dimension;
    /**
     * @remarks
     * The location the player was at before changing dimensions.
     *
     */
    readonly fromLocation: Vector3;
    /**
     * @remarks
     * Handle to the player that is changing dimensions.
     *
     */
    readonly player: Player;
    /**
     * @remarks
     * The dimension that the player is changing to.
     *
     */
    readonly toDimension: Dimension;
    /**
     * @remarks
     * The location the player will spawn to after changing
     * dimensions.
     *
     */
    readonly toLocation: Vector3;
}

/**
 * Manages callbacks that are connected to successful player
 * dimension changes.
 */
export class PlayerDimensionChangeAfterEventSignal {
    private constructor();
    /**
     * @remarks
     * Subscribes the specified callback to a player dimension
     * change after event.
     *
     * @worldMutation
     *
     * @earlyExecution
     *
     */
    subscribe(
        callback: (arg0: PlayerDimensionChangeAfterEvent) => void,
    ): (arg0: PlayerDimensionChangeAfterEvent) => void;
    /**
     * @remarks
     * Removes the specified callback from a player dimension
     * change after event.
     *
     * @worldMutation
     *
     * @earlyExecution
     *
     */
    unsubscribe(callback: (arg0: PlayerDimensionChangeAfterEvent) => void): void;
}

export class PlayerEmoteAfterEvent {
    private constructor();
    readonly personaPieceId: string;
    readonly player: Player;
}

export class PlayerEmoteAfterEventSignal {
    private constructor();
    /**
     * @remarks
     * @worldMutation
     *
     * @earlyExecution
     *
     */
    subscribe(callback: (arg0: PlayerEmoteAfterEvent) => void): (arg0: PlayerEmoteAfterEvent) => void;
    /**
     * @remarks
     * @worldMutation
     *
     * @earlyExecution
     *
     */
    unsubscribe(callback: (arg0: PlayerEmoteAfterEvent) => void): void;
}

/**
 * Contains information regarding an event after a players game
 * mode is changed.
 */
export class PlayerGameModeChangeAfterEvent {
    private constructor();
    /**
     * @remarks
     * The previous game mode before the change.
     *
     */
    readonly fromGameMode: GameMode;
    /**
     * @remarks
     * Source Player for this event.
     *
     */
    readonly player: Player;
    /**
     * @remarks
     * The current game mode after the change.
     *
     */
    readonly toGameMode: GameMode;
}

/**
 * Manages callbacks that are connected to after a players game
 * mode is changed.
 */
export class PlayerGameModeChangeAfterEventSignal {
    private constructor();
    /**
     * @remarks
     * Adds a callback that will be called after a players game
     * mode is changed.
     *
     * @worldMutation
     *
     * @earlyExecution
     *
     */
    subscribe(callback: (arg0: PlayerGameModeChangeAfterEvent) => void): (arg0: PlayerGameModeChangeAfterEvent) => void;
    /**
     * @remarks
     * Removes a callback from being called after a players game
     * mode is changed.
     *
     * @worldMutation
     *
     * @earlyExecution
     *
     */
    unsubscribe(callback: (arg0: PlayerGameModeChangeAfterEvent) => void): void;
}

/**
 * Contains information regarding an event before a player
 * interacts with an entity.
 */
export class PlayerGameModeChangeBeforeEvent {
    private constructor();
    /**
     * @remarks
     * If set to true the game mode change will be cancelled.
     *
     */
    cancel: boolean;
    /**
     * @remarks
     * The current game mode.
     *
     */
    readonly fromGameMode: GameMode;
    /**
     * @remarks
     * Source Player for this event.
     *
     */
    readonly player: Player;
    /**
     * @remarks
     * The game mode being changed to.
     *
     */
    toGameMode: GameMode;
}

/**
 * Manages callbacks that are connected to before a players
 * game mode is changed.
 */
export class PlayerGameModeChangeBeforeEventSignal {
    private constructor();
    /**
     * @remarks
     * Adds a callback that will be called before a players game
     * mode is changed.
     *
     * @worldMutation
     *
     * @earlyExecution
     *
     * @param callback
     * This closure is called with restricted-execution privilege.
     * @returns
     * Closure that is called with restricted-execution privilege.
     */
    subscribe(
        callback: (arg0: PlayerGameModeChangeBeforeEvent) => void,
    ): (arg0: PlayerGameModeChangeBeforeEvent) => void;
    /**
     * @remarks
     * Removes a callback from being called before a players game
     * mode is changed.
     *
     * @worldMutation
     *
     * @earlyExecution
     *
     * @param callback
     * This closure is called with restricted-execution privilege.
     */
    unsubscribe(callback: (arg0: PlayerGameModeChangeBeforeEvent) => void): void;
}

/**
 * Contains information regarding an event after changing the
 * selected hotbar slot for a player.
 */
export class PlayerHotbarSelectedSlotChangeAfterEvent {
    private constructor();
    /**
     * @remarks
     * The item stack of the new slot selected.
     *
     */
    readonly itemStack?: ItemStack;
    /**
     * @remarks
     * The new hotbar slot index selected.
     *
     */
    readonly newSlotSelected: number;
    /**
     * @remarks
     * Source Player for this event.
     *
     */
    readonly player: Player;
    /**
     * @remarks
     * The previous hotbar slot index selected.
     *
     */
    readonly previousSlotSelected: number;
}

/**
 * Manages callbacks that are connected after a player selected
 * hotbar slot is changed.
 */
export class PlayerHotbarSelectedSlotChangeAfterEventSignal {
    private constructor();
    /**
     * @remarks
     * Adds a callback that will be called after a player selected
     * hotbar slot is changed.
     *
     * @worldMutation
     *
     * @earlyExecution
     *
     * @param callback
     * Function callback that is called when this event fires.
     * @param options
     * Additional filtering options for the event subscription.
     */
    subscribe(
        callback: (arg0: PlayerHotbarSelectedSlotChangeAfterEvent) => void,
        options?: HotbarEventOptions,
    ): (arg0: PlayerHotbarSelectedSlotChangeAfterEvent) => void;
    /**
     * @remarks
     * Removes a callback from being called after a player selected
     * hotbar slot is changed.
     *
     * @worldMutation
     *
     * @earlyExecution
     *
     */
    unsubscribe(callback: (arg0: PlayerHotbarSelectedSlotChangeAfterEvent) => void): void;
}

/**
 * Event data for when a player input mode changes.
 */
export class PlayerInputModeChangeAfterEvent {
    private constructor();
    /**
     * @remarks
     * The new input mode used by the player.
     *
     */
    readonly newInputModeUsed: InputMode;
    /**
     * @remarks
     * The player that had an input mode change.
     *
     */
    readonly player: Player;
    /**
     * @remarks
     * The previous input mode used by the player.
     *
     */
    readonly previousInputModeUsed: InputMode;
}

/**
 * Manages callbacks that are connected to player input mode.
 */
export class PlayerInputModeChangeAfterEventSignal {
    private constructor();
    /**
     * @remarks
     * Adds a callback that will be called after the player input
     * mode changes.
     *
     * @worldMutation
     *
     * @earlyExecution
     *
     */
    subscribe(
        callback: (arg0: PlayerInputModeChangeAfterEvent) => void,
    ): (arg0: PlayerInputModeChangeAfterEvent) => void;
    /**
     * @remarks
     * Removes a callback from being called after the player input
     * mode changes.
     *
     * @worldMutation
     *
     * @earlyExecution
     *
     */
    unsubscribe(callback: (arg0: PlayerInputModeChangeAfterEvent) => void): void;
}

/**
 * Contains information regarding an event after a players
 * input permissions change.
 */
export class PlayerInputPermissionCategoryChangeAfterEvent {
    private constructor();
    /**
     * @remarks
     * The category of input permissions that have changed.
     *
     */
    readonly category: InputPermissionCategory;
    /**
     * @remarks
     * The enabled/disabled state of the players input permissions.
     *
     */
    readonly enabled: boolean;
    /**
     * @remarks
     * The player that has had their input permissions changed.
     *
     */
    readonly player: Player;
}

/**
 * Manages callbacks that are connected to after a players
 * input permissions change.
 */
export class PlayerInputPermissionCategoryChangeAfterEventSignal {
    private constructor();
    /**
     * @remarks
     * Adds a callback that will be called after a players input
     * permissions change.
     *
     * @worldMutation
     *
     * @earlyExecution
     *
     */
    subscribe(
        callback: (arg0: PlayerInputPermissionCategoryChangeAfterEvent) => void,
    ): (arg0: PlayerInputPermissionCategoryChangeAfterEvent) => void;
    /**
     * @remarks
     * Removes a callback from being called after a players input
     * permissions change.
     *
     * @worldMutation
     *
     * @earlyExecution
     *
     */
    unsubscribe(callback: (arg0: PlayerInputPermissionCategoryChangeAfterEvent) => void): void;
}

/**
 * Contains APIs to enable/disable player input permissions.
 */
export class PlayerInputPermissions {
    private constructor();
    /**
     * @remarks
     * Returns true if an input permission is enabled.
     *
     * @worldMutation
     *
     * @throws This function can throw errors.
     */
    isPermissionCategoryEnabled(permissionCategory: InputPermissionCategory): boolean;
    /**
     * @remarks
     * Enable or disable an input permission. When enabled the
     * input will work, when disabled will not work.
     *
     * @worldMutation
     *
     * @throws This function can throw errors.
     */
    setPermissionCategory(permissionCategory: InputPermissionCategory, isEnabled: boolean): void;
}

/**
 * Contains information regarding an event after a player
 * successfully interacts with a block.
 */
export class PlayerInteractWithBlockAfterEvent {
    private constructor();
    /**
     * @remarks
     * The ItemStack before the interaction succeeded, or undefined
     * if hand is empty.
     *
     */
    readonly beforeItemStack?: ItemStack;
    /**
     * @remarks
     * The block that will be interacted with.
     *
     */
    readonly block: Block;
    /**
     * @remarks
     * The face of the block that is being interacted with.
     *
     */
    readonly blockFace: Direction;
    /**
     * @remarks
     * Location relative to the bottom north-west corner of the
     * block where the item is placed.
     *
     */
    readonly faceLocation: Vector3;
    /**
     * @remarks
     * This value will be true if the event was triggered on
     * players initial interaction button press and false on events
     * triggered from holding the interaction button.
     *
     */
    readonly isFirstEvent: boolean;
    /**
     * @remarks
     * The ItemStack after the interaction succeeded, or undefined
     * if hand is empty.
     *
     */
    readonly itemStack?: ItemStack;
    /**
     * @remarks
     * Source Player for this event.
     *
     */
    readonly player: Player;
}

/**
 * Manages callbacks that are connected to after a player
 * interacts with a block.
 */
export class PlayerInteractWithBlockAfterEventSignal {
    private constructor();
    /**
     * @remarks
     * Adds a callback that will be called after a player interacts
     * with a block.
     *
     * @worldMutation
     *
     * @earlyExecution
     *
     */
    subscribe(
        callback: (arg0: PlayerInteractWithBlockAfterEvent) => void,
    ): (arg0: PlayerInteractWithBlockAfterEvent) => void;
    /**
     * @remarks
     * Removes a callback from being called after a player
     * interacts with a block.
     *
     * @worldMutation
     *
     * @earlyExecution
     *
     */
    unsubscribe(callback: (arg0: PlayerInteractWithBlockAfterEvent) => void): void;
}

/**
 * Contains information regarding an event before a player
 * interacts with a block.
 */
export class PlayerInteractWithBlockBeforeEvent {
    private constructor();
    /**
     * @remarks
     * The block that will be interacted with.
     *
     */
    readonly block: Block;
    /**
     * @remarks
     * The face of the block that is being interacted with.
     *
     */
    readonly blockFace: Direction;
    /**
     * @remarks
     * If set to true the interaction will be cancelled.
     *
     */
    cancel: boolean;
    /**
     * @remarks
     * Location relative to the bottom north-west corner of the
     * block where the item is placed.
     *
     */
    readonly faceLocation: Vector3;
    /**
     * @remarks
     * This value will be true if the event was triggered on
     * players initial interaction button press and false on events
     * triggered from holding the interaction button.
     *
     */
    readonly isFirstEvent: boolean;
    /**
     * @remarks
     * The item stack that is being used in the interaction, or
     * undefined if empty hand.
     *
     */
    readonly itemStack?: ItemStack;
    /**
     * @remarks
     * Source Player for this event.
     *
     */
    readonly player: Player;
}

/**
 * Manages callbacks that are connected to before a player
 * interacts with a block.
 */
export class PlayerInteractWithBlockBeforeEventSignal {
    private constructor();
    /**
     * @remarks
     * Adds a callback that will be called before a player
     * interacts with a block.
     *
     * @worldMutation
     *
     * @earlyExecution
     *
     * @param callback
     * This closure is called with restricted-execution privilege.
     * @returns
     * Closure that is called with restricted-execution privilege.
     */
    subscribe(
        callback: (arg0: PlayerInteractWithBlockBeforeEvent) => void,
    ): (arg0: PlayerInteractWithBlockBeforeEvent) => void;
    /**
     * @remarks
     * Removes a callback from being called before a player
     * interacts with a block.
     *
     * @worldMutation
     *
     * @earlyExecution
     *
     * @param callback
     * This closure is called with restricted-execution privilege.
     */
    unsubscribe(callback: (arg0: PlayerInteractWithBlockBeforeEvent) => void): void;
}

/**
 * Contains information regarding an event after a player
 * successfully interacts with an entity.
 */
export class PlayerInteractWithEntityAfterEvent {
    private constructor();
    /**
     * @remarks
     * The ItemStack before the interaction succeeded, or undefined
     * if hand is empty.
     *
     */
    readonly beforeItemStack?: ItemStack;
    /**
     * @remarks
     * The ItemStack after the interaction succeeded, or undefined
     * if hand is empty.
     *
     */
    readonly itemStack?: ItemStack;
    /**
     * @remarks
     * Source Player for this event.
     *
     */
    readonly player: Player;
    /**
     * @remarks
     * The entity that will be interacted with.
     *
     */
    readonly target: Entity;
}

/**
 * Manages callbacks that are connected to after a player
 * interacts with an entity.
 */
export class PlayerInteractWithEntityAfterEventSignal {
    private constructor();
    /**
     * @remarks
     * Adds a callback that will be called after a player interacts
     * with an entity.
     *
     * @worldMutation
     *
     * @earlyExecution
     *
     */
    subscribe(
        callback: (arg0: PlayerInteractWithEntityAfterEvent) => void,
    ): (arg0: PlayerInteractWithEntityAfterEvent) => void;
    /**
     * @remarks
     * Removes a callback from being called after a player
     * interacts with an entity.
     *
     * @worldMutation
     *
     * @earlyExecution
     *
     */
    unsubscribe(callback: (arg0: PlayerInteractWithEntityAfterEvent) => void): void;
}

/**
 * Contains information regarding an event before a player
 * interacts with an entity.
 */
export class PlayerInteractWithEntityBeforeEvent {
    private constructor();
    /**
     * @remarks
     * If set to true the interaction will be cancelled.
     *
     */
    cancel: boolean;
    /**
     * @remarks
     * The item stack that is being used in the interaction, or
     * undefined if empty hand.
     *
     */
    readonly itemStack?: ItemStack;
    /**
     * @remarks
     * Source Player for this event.
     *
     */
    readonly player: Player;
    /**
     * @remarks
     * The entity that will be interacted with.
     *
     */
    readonly target: Entity;
}

/**
 * Manages callbacks that are connected to before a player
 * interacts with an entity.
 */
export class PlayerInteractWithEntityBeforeEventSignal {
    private constructor();
    /**
     * @remarks
     * Adds a callback that will be called before a player
     * interacts with an entity.
     *
     * @worldMutation
     *
     * @earlyExecution
     *
     * @param callback
     * This closure is called with restricted-execution privilege.
     * @returns
     * Closure that is called with restricted-execution privilege.
     */
    subscribe(
        callback: (arg0: PlayerInteractWithEntityBeforeEvent) => void,
    ): (arg0: PlayerInteractWithEntityBeforeEvent) => void;
    /**
     * @remarks
     * Removes a callback from being called before a player
     * interacts with an entity.
     *
     * @worldMutation
     *
     * @earlyExecution
     *
     * @param callback
     * This closure is called with restricted-execution privilege.
     */
    unsubscribe(callback: (arg0: PlayerInteractWithEntityBeforeEvent) => void): void;
}

/**
 * Contains information regarding an event after a player's
 * inventory item changes.
 */
export class PlayerInventoryItemChangeAfterEvent {
    private constructor();
    /**
     * @remarks
     * The previous item stack.
     *
     */
    readonly beforeItemStack?: ItemStack;
    /**
     * @remarks
     * Inventory type.
     *
     */
    readonly inventoryType: PlayerInventoryType;
    /**
     * @remarks
     * The new item stack.
     *
     */
    readonly itemStack?: ItemStack;
    /**
     * @remarks
     * Source Player for this event.
     *
     */
    readonly player: Player;
    /**
     * @remarks
     * The slot index with the change.
     *
     */
    readonly slot: number;
}

/**
 * Manages callbacks that are connected after a player's
 * inventory item is changed.
 */
export class PlayerInventoryItemChangeAfterEventSignal {
    private constructor();
    /**
     * @remarks
     * Adds a callback that will be called after a player's
     * inventory item is changed.
     *
     * @worldMutation
     *
     * @earlyExecution
     *
     * @param callback
     * Function callback that is called when this event fires.
     * @param options
     * Additional filtering options for the event subscription.
     */
    subscribe(
        callback: (arg0: PlayerInventoryItemChangeAfterEvent) => void,
        options?: InventoryItemEventOptions,
    ): (arg0: PlayerInventoryItemChangeAfterEvent) => void;
    /**
     * @remarks
     * Removes a callback from being called after a player's
     * inventory item is changed.
     *
     * @worldMutation
     *
     * @earlyExecution
     *
     */
    unsubscribe(callback: (arg0: PlayerInventoryItemChangeAfterEvent) => void): void;
}

/**
 * Contains information regarding a player that has joined.
 * See the playerSpawn event for more detailed information that
 * could be returned after the first time a player has spawned
 * within the game.
 */
export class PlayerJoinAfterEvent {
    private constructor();
    /**
     * @remarks
     * Opaque string identifier of the player that joined the game.
     *
     */
    readonly playerId: string;
    /**
     * @remarks
     * Name of the player that has joined.
     *
     */
    readonly playerName: string;
}

/**
 * Manages callbacks that are connected to a player joining the
 * world.
 */
export class PlayerJoinAfterEventSignal {
    private constructor();
    /**
     * @remarks
     * Adds a callback that will be called when a player joins the
     * world.
     *
     * @worldMutation
     *
     * @earlyExecution
     *
     */
    subscribe(callback: (arg0: PlayerJoinAfterEvent) => void): (arg0: PlayerJoinAfterEvent) => void;
    /**
     * @remarks
     * Removes a callback from being called when a player joins the
     * world.
     *
     * @worldMutation
     *
     * @earlyExecution
     *
     */
    unsubscribe(callback: (arg0: PlayerJoinAfterEvent) => void): void;
}

/**
 * Contains information regarding a player that has left the
 * world.
 */
export class PlayerLeaveAfterEvent {
    private constructor();
    /**
     * @remarks
     * Opaque string identifier of the player that has left the
     * event.
     *
     */
    readonly playerId: string;
    /**
     * @remarks
     * Player that has left the world.
     *
     */
    readonly playerName: string;
}

/**
 * Manages callbacks that are connected to a player leaving the
 * world.
 */
export class PlayerLeaveAfterEventSignal {
    private constructor();
    /**
     * @remarks
     * Adds a callback that will be called when a player leaves the
     * world.
     *
     * @worldMutation
     *
     * @earlyExecution
     *
     */
    subscribe(callback: (arg0: PlayerLeaveAfterEvent) => void): (arg0: PlayerLeaveAfterEvent) => void;
    /**
     * @remarks
     * Removes a callback from being called when a player leaves
     * the world.
     *
     * @worldMutation
     *
     * @earlyExecution
     *
     */
    unsubscribe(callback: (arg0: PlayerLeaveAfterEvent) => void): void;
}

/**
 * Contains information regarding a player that is leaving the
 * world.
 */
export class PlayerLeaveBeforeEvent {
    private constructor();
    /**
     * @remarks
     * The leaving player.
     *
     */
    readonly player: Player;
}

/**
 * Manages callbacks that are connected to a player leaving the
 * world.
 */
export class PlayerLeaveBeforeEventSignal {
    private constructor();
    /**
     * @remarks
     * Adds a callback that will be called when a player leaves the
     * world.
     *
     * @worldMutation
     *
     * @earlyExecution
     *
     * @param callback
     * This closure is called with restricted-execution privilege.
     * @returns
     * Closure that is called with restricted-execution privilege.
     */
    subscribe(callback: (arg0: PlayerLeaveBeforeEvent) => void): (arg0: PlayerLeaveBeforeEvent) => void;
    /**
     * @remarks
     * Removes a callback that will be called when a player leaves
     * the world.
     *
     * @worldMutation
     *
     * @earlyExecution
     *
     * @param callback
     * This closure is called with restricted-execution privilege.
     */
    unsubscribe(callback: (arg0: PlayerLeaveBeforeEvent) => void): void;
}

/**
 * Contains information regarding an event where a player
 * places a block.
 */
// @ts-ignore Class inheritance allowed for native defined classes
export class PlayerPlaceBlockAfterEvent extends BlockEvent {
    private constructor();
    /**
     * @remarks
     * Player that placed the block for this event.
     *
     */
    readonly player: Player;
}

/**
 * Manages callbacks that are connected to when a block is
 * placed by a player.
 */
export class PlayerPlaceBlockAfterEventSignal {
    private constructor();
    /**
     * @remarks
     * Adds a callback that will be called when a block is placed
     * by a player.
     *
     * @worldMutation
     *
     * @earlyExecution
     *
     */
    subscribe(
        callback: (arg0: PlayerPlaceBlockAfterEvent) => void,
        options?: BlockEventOptions,
    ): (arg0: PlayerPlaceBlockAfterEvent) => void;
    /**
     * @remarks
     * Removes a callback from being called when an block is placed
     * by a player.
     *
     * @worldMutation
     *
     * @earlyExecution
     *
     */
    unsubscribe(callback: (arg0: PlayerPlaceBlockAfterEvent) => void): void;
}

/**
 * @beta
 * Contains information regarding an event before a player
 * places a block.
 */
// @ts-ignore Class inheritance allowed for native defined classes
export class PlayerPlaceBlockBeforeEvent extends BlockEvent {
    private constructor();
    /**
     * @remarks
     * If set to true, cancels the block place event.
     *
     */
    cancel: boolean;
    /**
     * @remarks
     * The face of the block that the new block is being placed on.
     *
     */
    readonly face: Direction;
    /**
     * @remarks
     * Location relative to the bottom north-west corner of the
     * block where the new block is being placed onto.
     *
     */
    readonly faceLocation: Vector3;
    /**
     * @remarks
     * The block permutation that will be placed if the event is
     * not cancelled.
     *
     */
    readonly permutationToPlace: BlockPermutation;
    /**
     * @remarks
     * Player that is placing the block for this event.
     *
     */
    readonly player: Player;
}

/**
 * @beta
 * Manages callbacks that are connected to before a block is
 * placed by a player.
 */
export class PlayerPlaceBlockBeforeEventSignal {
    private constructor();
    /**
     * @remarks
     * Adds a callback that will be called before a block is placed
     * by a player.
     *
     * @worldMutation
     *
     * @earlyExecution
     *
     * @param callback
     * This closure is called with restricted-execution privilege.
     * @returns
     * Closure that is called with restricted-execution privilege.
     */
    subscribe(
        callback: (arg0: PlayerPlaceBlockBeforeEvent) => void,
        options?: BlockEventOptions,
    ): (arg0: PlayerPlaceBlockBeforeEvent) => void;
    /**
     * @remarks
     * Removes a callback from being called before an block is
     * placed by a player.
     *
     * @worldMutation
     *
     * @earlyExecution
     *
     * @param callback
     * This closure is called with restricted-execution privilege.
     */
    unsubscribe(callback: (arg0: PlayerPlaceBlockBeforeEvent) => void): void;
}

/**
 * An event that contains more information about a player
 * spawning.
 */
export class PlayerSpawnAfterEvent {
    private constructor();
    /**
     * @remarks
     * If true, this is the initial spawn of a player after joining
     * the game.
     *
     * @worldMutation
     *
     */
    initialSpawn: boolean;
    /**
     * @remarks
     * Object that represents the player that joined the game.
     *
     * @worldMutation
     *
     */
    player: Player;
}

/**
 * Registers an event when a player is spawned (or re-spawned
 * after death) and fully ready within the world.
 */
export class PlayerSpawnAfterEventSignal {
    private constructor();
    /**
     * @remarks
     * Registers a new event receiver for this particular type of
     * event.
     *
     * @worldMutation
     *
     * @earlyExecution
     *
     */
    subscribe(callback: (arg0: PlayerSpawnAfterEvent) => void): (arg0: PlayerSpawnAfterEvent) => void;
    /**
     * @remarks
     * De-registers an event receiver for the player spawn event.
     *
     * @worldMutation
     *
     * @earlyExecution
     *
     */
    unsubscribe(callback: (arg0: PlayerSpawnAfterEvent) => void): void;
}

/**
 * @rc
 * Contains information regarding an event after a player
 * starts breaking a block.
 */
// @ts-ignore Class inheritance allowed for native defined classes
export class PlayerStartBreakingBlockAfterEvent extends BlockEvent {
    private constructor();
    /**
     * @remarks
     * The permutation of the block that the player is starting to
     * break.
     *
     */
    readonly blockPermutation: BlockPermutation;
    /**
     * @remarks
     * The face of the block being broken.
     *
     */
    readonly face: Direction;
    /**
     * @remarks
     * The item stack that the player is using to break the block,
     * or undefined if empty hand.
     *
     */
    readonly heldItemStack?: ItemStack;
    /**
     * @remarks
     * Player that started breaking the block for this event.
     *
     */
    readonly player: Player;
}

/**
 * @rc
 * Manages callbacks that are connected to when a player starts
 * breaking a block.
 */
export class PlayerStartBreakingBlockAfterEventSignal {
    private constructor();
    /**
     * @remarks
     * Adds a callback that will be called when a player starts
     * breaking a block.
     *
     * @worldMutation
     *
     * @earlyExecution
     *
     */
    subscribe(
        callback: (arg0: PlayerStartBreakingBlockAfterEvent) => void,
        options?: PlayerBreakingBlockEventOptions,
    ): (arg0: PlayerStartBreakingBlockAfterEvent) => void;
    /**
     * @remarks
     * Removes a callback from being called when a player starts
     * breaking a block.
     *
     * @worldMutation
     *
     * @earlyExecution
     *
     */
    unsubscribe(callback: (arg0: PlayerStartBreakingBlockAfterEvent) => void): void;
}

/**
 * Contains information regarding a player starting to swing
 * their arm.
 */
export class PlayerSwingStartAfterEvent {
    private constructor();
    /**
     * @remarks
     * The item stack being held by the player at the start of
     * their swing.
     *
     */
    readonly heldItemStack?: ItemStack;
    /**
     * @remarks
     * Source Player for this event.
     *
     */
    readonly player: Player;
    /**
     * @remarks
     * The source of the Player swing, see {@link
     * EntitySwingSource}.
     *
     */
    readonly swingSource: EntitySwingSource;
}

/**
 * Manages callbacks that are connected to when a player starts
 * to swing their arm (e.g. attacking, using an item,
 * interacting).
 */
export class PlayerSwingStartAfterEventSignal {
    private constructor();
    /**
     * @remarks
     * Adds a callback that will be called when a player starts to
     * swing their arm (e.g. attacking, using an item,
     * interacting).
     *
     * @worldMutation
     *
     * @earlyExecution
     *
     */
    subscribe(
        callback: (arg0: PlayerSwingStartAfterEvent) => void,
        options?: PlayerSwingEventOptions,
    ): (arg0: PlayerSwingStartAfterEvent) => void;
    /**
     * @remarks
     * Removes a callback from being called when a player starts to
     * swing their arm (e.g. attacking, using an item,
     * interacting).
     *
     * @worldMutation
     *
     * @earlyExecution
     *
     */
    unsubscribe(callback: (arg0: PlayerSwingStartAfterEvent) => void): void;
}

/**
 * @beta
 * Contains information related to when a player successfully
 * names an Entity with a named Name Tag item.
 */
export class PlayerUseNameTagAfterEvent {
    private constructor();
    /**
     * @remarks
     * The entity that was named by the player.
     *
     * @worldMutation
     *
     */
    entityNamed: Entity;
    /**
     * @remarks
     * The new name that the player has given to the entity.
     *
     * @worldMutation
     *
     */
    newName: string;
    /**
     * @remarks
     * Handle to the player that used the name tag.
     *
     * @worldMutation
     *
     */
    player: Player;
    /**
     * @remarks
     * The previous name of the entity before the player used the
     * name tag. This will be undefined if the entity was not
     * previously named.
     *
     * @worldMutation
     *
     */
    previousName?: string;
}

/**
 * @beta
 * Manages callbacks that are connected to when a player
 * successfully names an Entity with a named Name Tag item.
 */
export class PlayerUseNameTagAfterEventSignal {
    private constructor();
    /**
     * @remarks
     * Subscribes the specified callback to a player use name tag
     * after event.
     *
     * @worldMutation
     *
     * @earlyExecution
     *
     */
    subscribe(callback: (arg0: PlayerUseNameTagAfterEvent) => void): (arg0: PlayerUseNameTagAfterEvent) => void;
    /**
     * @remarks
     * Removes the specified callback from a player use name tag
     * after event.
     *
     * @worldMutation
     *
     * @earlyExecution
     *
     */
    unsubscribe(callback: (arg0: PlayerUseNameTagAfterEvent) => void): void;
}

/**
 * Waypoint that tracks a player's position. Extends {@link
 * EntityWaypoint} with additional player-specific visibility
 * rules such as hidden state and spectator mode.
 */
// @ts-ignore Class inheritance allowed for native defined classes
export class PlayerWaypoint extends EntityWaypoint {
    /**
     * @remarks
     * The {@link PlayerVisibilityRules} that control when the
     * waypoint is shown based on the player's state (e.g., hidden,
     * spectator mode, spectator viewing another spectator).
     *
     * @throws This property can throw when used.
     *
     * {@link InvalidWaypointError}
     *
     * {@link InvalidWaypointTextureSelectorError}
     */
    readonly playerRules: PlayerVisibilityRules;
    /**
     * @throws This function can throw errors.
     *
     * {@link InvalidWaypointTextureSelectorError}
     */
    constructor(
        player: Player,
        textureSelector: WaypointTextureSelector,
        playerRules: PlayerVisibilityRules,
        color?: RGB,
    );
}

/**
 * Represents how the potion effect is delivered.
 */
export class PotionDeliveryType {
    private constructor();
    readonly id: string;
}

/**
 * Represents a type of potion effect - like healing or
 * leaping.
 */
export class PotionEffectType {
    private constructor();
    /**
     * @remarks
     * Duration of the effect when applied to an entity in ticks.
     * Undefined means the effect does not expire.
     *
     * @throws This property can throw when used.
     *
     * {@link EngineError}
     */
    readonly durationTicks?: number;
    readonly id: string;
}

/**
 * Used for accessing all potion effect types, delivery types,
 * and creating potions.
 */
export class Potions {
    private constructor();
    /**
     * @remarks
     * Retrieves handles for all registered potion delivery types.
     *
     * @returns
     * Array of all registered delivery type handles.
     */
    static getAllDeliveryTypes(): PotionDeliveryType[];
    /**
     * @remarks
     * Retrieves all type handle for all registered potion effects.
     *
     * @returns
     * Array of all registered effect type handles.
     */
    static getAllEffectTypes(): PotionEffectType[];
    /**
     * @remarks
     * Retrieves a type handle for a specified potion delivery id.
     *
     * @returns
     * A type handle wrapping the valid delivery id, or undefined
     * for an invalid delivery id.
     */
    static getDeliveryType(potionDeliveryId: string): PotionDeliveryType | undefined;
    /**
     * @remarks
     * Retrieves a type handle for a specified potion effect id.
     *
     * @param potionEffectId
     * A valid potion effect id. See
     * @minecraft/vanilla-data.MinecraftPotionEffectTypes
     * @returns
     * A type handle wrapping the valid effect id, or undefined for
     * an invalid effect id.
     */
    static getEffectType(potionEffectId: string): PotionEffectType | undefined;
    /**
     * @remarks
     * Creates a potion given an effect and delivery type.
     *
     * @throws This function can throw errors.
     *
     * {@link EngineError}
     *
     * {@link InvalidPotionDeliveryTypeError}
     *
     * {@link InvalidPotionEffectTypeError}
     */
    static resolve<
        T extends string = MinecraftPotionEffectTypes,
        U extends string = MinecraftPotionDeliveryTypes,
    >(potionEffectType: PotionEffectType | T, potionDeliveryType: PotionDeliveryType | U): ItemStack;
}

/**
 * Contains information related to changes to a pressure plate
 * pop.
 */
// @ts-ignore Class inheritance allowed for native defined classes
export class PressurePlatePopAfterEvent extends BlockEvent {
    private constructor();
    /**
     * @remarks
     * The redstone power of the pressure plate before it was
     * popped.
     *
     */
    readonly previousRedstonePower: number;
    /**
     * @remarks
     * The redstone power of the pressure plate at the time of the
     * pop.
     *
     */
    readonly redstonePower: number;
}

/**
 * Manages callbacks that are connected to when a pressure
 * plate is popped.
 */
export class PressurePlatePopAfterEventSignal {
    private constructor();
    /**
     * @remarks
     * Adds a callback that will be called when a pressure plate is
     * popped.
     *
     * @worldMutation
     *
     * @earlyExecution
     *
     */
    subscribe(callback: (arg0: PressurePlatePopAfterEvent) => void): (arg0: PressurePlatePopAfterEvent) => void;
    /**
     * @remarks
     * Removes a callback from being called when a pressure plate
     * is popped.
     *
     * @worldMutation
     *
     * @earlyExecution
     *
     */
    unsubscribe(callback: (arg0: PressurePlatePopAfterEvent) => void): void;
}

/**
 * Contains information related to changes to a pressure plate
 * push.
 */
// @ts-ignore Class inheritance allowed for native defined classes
export class PressurePlatePushAfterEvent extends BlockEvent {
    private constructor();
    /**
     * @remarks
     * The redstone power of the pressure plate before it was
     * pushed.
     *
     */
    readonly previousRedstonePower: number;
    /**
     * @remarks
     * The redstone power of the pressure plate at the time of the
     * push.
     *
     */
    readonly redstonePower: number;
    /**
     * @remarks
     * Source that triggered the pressure plate push.
     *
     */
    readonly source: Entity;
}

/**
 * Manages callbacks that are connected to when a pressure
 * plate is pushed.
 */
export class PressurePlatePushAfterEventSignal {
    private constructor();
    /**
     * @remarks
     * Adds a callback that will be called when a pressure plate is
     * pushed.
     *
     * @worldMutation
     *
     * @earlyExecution
     *
     */
    subscribe(callback: (arg0: PressurePlatePushAfterEvent) => void): (arg0: PressurePlatePushAfterEvent) => void;
    /**
     * @remarks
     * Removes a callback from being called when a pressure plate
     * is pushed.
     *
     * @worldMutation
     *
     * @earlyExecution
     *
     */
    unsubscribe(callback: (arg0: PressurePlatePushAfterEvent) => void): void;
}

/**
 * The base class for a text primitive. Represents an object in
 * the world and its base properties.
 */
export class PrimitiveShape {
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
     * The location of the shape.
     *
     */
    readonly location: Vector3;
    /**
     * @remarks
     * If defined, this distance will be used to determine how far
     * away this primitive will be rendered for each client. By
     * default the distance will match the client's render distance
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
     * The scale of the shape.
     *
     * Bounds: [-1000, 1000]
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
     * via the PrimitiveShapesManager's addText method.
     *
     */
    remove(): void;
    /**
     * @remarks
     * Set the location and dimension of the shape. If the
     * dimension is undefined, it will display in all dimensions.
     *
     */
    setLocation(location: DimensionLocation | Vector3): void;
}

/**
 * Primitive Shapes class used to allow adding and removing
 * text primitives to the world.
 */
export class PrimitiveShapesManager {
    private constructor();
    /**
     * @remarks
     * This is the maximum number of allowed primitive shapes.
     *
     */
    readonly maxShapes: number;
    /**
     * @remarks
     * Adds a new text primitive to the world.
     *
     * @param text
     * The text primitive to be added.
     * @throws This function can throw errors.
     *
     * {@link EngineError}
     *
     * {@link PrimitiveShapeError}
     */
    addText(text: TextPrimitive, dimension?: Dimension): void;
    /**
     * @remarks
     * Removes all text primitives from the world.
     *
     */
    removeAll(): void;
    /**
     * @remarks
     * Removes an instance of a text primitive from the world. This
     * is equivalent to calling remove on the text itself.
     *
     */
    removeText(text: TextPrimitive): void;
}

/**
 * Contains information related to a projectile hitting a
 * block.
 */
export class ProjectileHitBlockAfterEvent {
    private constructor();
    /**
     * @remarks
     * Dimension where this projectile hit took place.
     *
     */
    readonly dimension: Dimension;
    /**
     * @remarks
     * Direction vector of the projectile as it hit a block.
     *
     */
    readonly hitVector: Vector3;
    /**
     * @remarks
     * Location where the projectile hit occurred.
     *
     */
    readonly location: Vector3;
    /**
     * @remarks
     * Entity for the projectile that hit a block.
     *
     */
    readonly projectile: Entity;
    /**
     * @remarks
     * Optional source entity that fired the projectile.
     *
     */
    readonly source?: Entity;
    /**
     * @remarks
     * Contains additional information about the block that was hit
     * by the projectile.
     *
     * @worldMutation
     *
     */
    getBlockHit(): BlockHitInformation;
}

/**
 * Manages callbacks that are connected to when a projectile
 * hits a block.
 */
export class ProjectileHitBlockAfterEventSignal {
    private constructor();
    /**
     * @remarks
     * Adds a callback that will be called when a projectile hits a
     * block.
     *
     * @worldMutation
     *
     * @earlyExecution
     *
     */
    subscribe(callback: (arg0: ProjectileHitBlockAfterEvent) => void): (arg0: ProjectileHitBlockAfterEvent) => void;
    /**
     * @remarks
     * Removes a callback from being called when a projectile hits
     * a block.
     *
     * @worldMutation
     *
     * @earlyExecution
     *
     */
    unsubscribe(callback: (arg0: ProjectileHitBlockAfterEvent) => void): void;
}

/**
 * Contains information related to a projectile hitting an
 * entity.
 */
export class ProjectileHitEntityAfterEvent {
    private constructor();
    /**
     * @remarks
     * Dimension where this projectile hit took place.
     *
     */
    readonly dimension: Dimension;
    /**
     * @remarks
     * Direction vector of the projectile as it hit an entity.
     *
     */
    readonly hitVector: Vector3;
    /**
     * @remarks
     * Location where the projectile hit occurred.
     *
     */
    readonly location: Vector3;
    /**
     * @remarks
     * Entity for the projectile that hit an entity.
     *
     */
    readonly projectile: Entity;
    /**
     * @remarks
     * Optional source entity that fired the projectile.
     *
     */
    readonly source?: Entity;
    /**
     * @remarks
     * Contains additional information about an entity that was
     * hit.
     *
     * @worldMutation
     *
     */
    getEntityHit(): EntityHitInformation;
}

/**
 * Manages callbacks that are connected to when a projectile
 * hits an entity.
 */
export class ProjectileHitEntityAfterEventSignal {
    private constructor();
    /**
     * @remarks
     * Adds a callback that will be called when a projectile hits
     * an entity.
     *
     * @worldMutation
     *
     * @earlyExecution
     *
     */
    subscribe(callback: (arg0: ProjectileHitEntityAfterEvent) => void): (arg0: ProjectileHitEntityAfterEvent) => void;
    /**
     * @remarks
     * Removes a callback from being called when a projectile hits
     * an entity.
     *
     * @worldMutation
     *
     * @earlyExecution
     *
     */
    unsubscribe(callback: (arg0: ProjectileHitEntityAfterEvent) => void): void;
}

/**
 * Loot item function that randomly modifies the data value of
 * the item dropped.
 */
// @ts-ignore Class inheritance allowed for native defined classes
export class RandomAuxValueFunction extends LootItemFunction {
    private constructor();
    /**
     * @remarks
     * The value range from which the function randomly chooses the
     * data value to assign. Contains minimum and maximum values.
     *
     */
    readonly values: NumberRange;
}

/**
 * Loot item function that randomly modifies the block state of
 * the item dropped.
 */
// @ts-ignore Class inheritance allowed for native defined classes
export class RandomBlockStateFunction extends LootItemFunction {
    private constructor();
    readonly blockState: string;
    /**
     * @remarks
     * The range from which the function randomly chooses the value
     * to assign to the given block state. Contains minimum and
     * maximum values.
     *
     */
    readonly values: NumberRange;
}

/**
 * Loot item condition that applies a given value to the
 * chances that loot will drop.
 */
// @ts-ignore Class inheritance allowed for native defined classes
export class RandomChanceCondition extends LootItemCondition {
    private constructor();
    /**
     * @remarks
     * The chance, from 0.0-1.0, that loot will drop.
     *
     */
    readonly chance: number;
}

/**
 * Loot item condition that applies a given value to the
 * chances that loot will drop, modified by the level of
 * looting enchantment on the tool used.
 */
// @ts-ignore Class inheritance allowed for native defined classes
export class RandomChanceWithLootingCondition extends LootItemCondition {
    private constructor();
    /**
     * @remarks
     * The base chance, from 0.0-1.0, that loot will drop. Will be
     * modified by the 'lootingMultiplier' value.
     *
     */
    readonly chance: number;
    /**
     * @remarks
     * The increase in drop chance per looting enchant level.
     *
     */
    readonly lootingMultiplier: number;
}

/**
 * Loot item condition that applies given values to the chances
 * that loot will drop based on the current difficulty level.
 */
// @ts-ignore Class inheritance allowed for native defined classes
export class RandomDifficultyChanceCondition extends LootItemCondition {
    private constructor();
    /**
     * @remarks
     * A four-element array containing the chance of a loot drop
     * occurring for each difficulty level, in order: Peaceful,
     * Easy, Normal, Hard.
     *
     */
    readonly chances: number[];
}

/**
 * Loot item function that applies a randomly dye to the
 * dropped item.
 */
// @ts-ignore Class inheritance allowed for native defined classes
export class RandomDyeFunction extends LootItemFunction {
    private constructor();
}

/**
 * Loot item condition that applies a given value to the
 * chances that loot will drop, modified by the region the drop
 * is happening within.
 */
// @ts-ignore Class inheritance allowed for native defined classes
export class RandomRegionalDifficultyChanceCondition extends LootItemCondition {
    private constructor();
    /**
     * @remarks
     * The base chance, from 0.0-1.0, that loot will drop. Will be
     * modified by the current region's multiplier.
     *
     */
    readonly maxChance: number;
}

/**
 * 表示记分板。其上包含了记分项和分数持有者。
 * 
 * Contains objectives and participants for the scoreboard.
 * @seeExample updateScoreboard.ts
 */
export class Scoreboard {
    private constructor();
    /**
     * @remarks
     * 在记分板上添加一个新的记分项。
     * 
     * Adds a new objective to the scoreboard.
     *
     * @worldMutation
     * @param objectiveId 记分项名称。
     * @param displayName 记分项的显示名称。
     * @returns 创建的记分项对象。
     * @throws
     * 若同名记分项已存在时，抛出 `"Failed to add objective 'objectiveId' as it is already being tracked"`。
     * @seeExample updateScoreboard.ts
     */
    addObjective(objectiveId: string, displayName?: string): ScoreboardObjective;
    /**
     * @remarks
     * 清除显示位置上正在显示的记分项。
     * 
     * Clears the objective that occupies a display slot.
     *
     * @worldMutation
     * @param displaySlotId 显示位置。
     * @returns 先前正显示的记分项，为空时返回 `null`。
     *
     */
    clearObjectiveAtDisplaySlot(displaySlotId: DisplaySlotId): ScoreboardObjective | undefined;
    /**
     * @remarks
     * 获取名称为 `objectiveId` 的记分项对象。
     * 
     * Returns a specific objective (by id).
     *
     * @param objectiveId
     * 记分项名称。
     * 
     * Identifier of the objective.
     * @returns 指定的记分项对象。不存在时返回 `null`。
     */
    getObjective(objectiveId: string): ScoreboardObjective | undefined;
    /**
     * @remarks
     * 获取位于指定显示位置上的记分项对象与显示配置。
     * 
     * Returns an objective that occupies the specified display
     * slot.
     * @param displaySlotId 显示位置。
     * @returns 位于指定显示位置的记分项显示配置。为空时返回 `null`。
     *
     */
    getObjectiveAtDisplaySlot(displaySlotId: DisplaySlotId): ScoreboardObjectiveDisplayOptions | undefined;
    /**
     * @remarks
     * 获取记分板上的已定义的所有记分项。
     * 
     * Returns all defined objectives.
     * @returns 所有记分项对象组成的数组。
     *
     */
    getObjectives(): ScoreboardObjective[];
    /**
     * @remarks
     * 获取所有已经定义的分数持有者。
     * 
     * Returns all defined scoreboard identities.
     * @returns 所有分数持有者对象组成的数组。
     *
     */
    getParticipants(): ScoreboardIdentity[];
    /**
     * @remarks
     * 从记分板上移除指定的记分项。
     * 
     * Removes an objective from the scoreboard.
     *
     * @worldMutation
     * @param objectiveId 记分项对象或名称。
     * @returns 总是返回 `true`。
     * @throws
     * 若指定的记分项不存在时，抛出 `"Failed to find the objective specified"`。
     */
    removeObjective(objectiveId: ScoreboardObjective | string): boolean;
    /**
     * @remarks
     * 设置指定的显示位置显示的记分项与其他显示配置。
     * 
     * Sets an objective into a display slot with specified
     * additional display settings.
     *
     * @worldMutation
     * @param displaySlotId 显示位置。
     * @param objectiveDisplaySetting 记分项显示配置。
     * @returns
     * 显示位上先前显示的记分项对象。先前未显示记分项时，返回 `undefined`。
     * 
     * Returns the previous `ScoreboardObjective` set at the
     * display slot, if no objective was previously set it returns
     * `undefined`.
     * @throws
     * 若记分项无效，抛出 `"Failed to set invalid objective at DisplaySlot"`。
     */
    setObjectiveAtDisplaySlot(
        displaySlotId: DisplaySlotId,
        objectiveDisplaySetting: ScoreboardObjectiveDisplayOptions,
    ): ScoreboardObjective | undefined;
}

/**
 * 表示记分板上的分数持有者。
 * 
 * Contains an identity of the scoreboard item.
 */
export class ScoreboardIdentity {
    private constructor();
    /**
     * @remarks
     * 此分数持有者在玩家视角显示的名称。
     * 
     * Returns the player-visible name of this identity.
     *
     */
    readonly displayName: string;
    /**
     * @remarks
     * 此分数持有者的 ID。
     * 
     * Identifier of the scoreboard identity.
     *
     */
    readonly id: number;
    /**
     * @remarks
     * Returns true if the ScoreboardIdentity reference is still
     * valid.
     *
     */
    readonly isValid: boolean;
    /**
     * @remarks
     * 此分数持有者的类型。
     * 
     * Type of the scoreboard identity.
     *
     */
    readonly 'type': ScoreboardIdentityType;
    /**
     * @remarks
     * 返回此分数持有者对应的实体对象（仅当此分数持有者是一个实体或者玩家时）。
     * 
     * If the scoreboard identity is an entity or player, returns
     * the entity that this scoreboard item corresponds to.
     * @returns 对应的实体对象。虚拟玩家类型的分数持有者会返回 `undefined`。
     * @throws
     * 若实体不存在时，抛出。
     */
    getEntity(): Entity | undefined;
}

/**
 * 表示记分板上的记分项。包含了分数持有者和各自的分数。
 * 
 * Contains objectives and participants for the scoreboard.
 */
export class ScoreboardObjective {
    private constructor();
    /**
     * @remarks
     * 此记分项的显示名称。
     * 
     * Returns the player-visible name of this scoreboard
     * objective.
     *
     * @throws
     * 若记分项无效，则抛出。
     */
    readonly displayName: string;
    /**
     * @remarks
     * 此记分项的名称。
     * 
     * Identifier of the scoreboard objective.
     *
     * @throws
     * 若记分项无效，则抛出。
     */
    readonly id: string;
    /**
     * @remarks
     * Returns true if the ScoreboardObjective reference is still
     * valid.
     *
     */
    readonly isValid: boolean;
    /**
     * @remarks
     * Adds a score to the given participant and objective.
     *
     * @worldMutation
     *
     * @param participant
     * Participant to apply the scoreboard value addition to.
     * @throws This function can throw errors.
     */
    addScore(participant: Entity | ScoreboardIdentity | string, scoreToAdd: number): number;
    /**
     * @remarks
     * 返回此记分项上已记录的分数持有者。
     * 
     * Returns all objective participant identities.
     * @returns 由分数持有者对象组成的数组。
     * @throws
     * 若记分项无效时，抛出。
     */
    getParticipants(): ScoreboardIdentity[];
    /**
     * @remarks
     * 获取指定分数持有者的分数。
     * 
     * Returns a specific score for a participant.
     *
     * @param participant
     * 分数持有者。
     * 
     * Identifier of the participant to retrieve a score for.
     * @returns 指定分数持有者的分数。
     * @throws
     * 若此记分项上未记录分数持有者的分数，抛出 `"Failed to retrieve score for '<participant>'"`。
     * 若记分项无效时，抛出。
     */
    getScore(participant: Entity | ScoreboardIdentity | string): number | undefined;
    /**
     * @remarks
     * 获取记分项上记录的所有分数信息。
     * 
     * Returns specific scores for this objective for all
     * participants.
     * @returns 由分数信息对象组成的数组。
     * @throws
     * 若记分项无效时，抛出。
     */
    getScores(): ScoreboardScoreInfo[];
    /**
     * @remarks
     * Returns if the specified identity is a participant of the
     * scoreboard objective.
     *
     * @throws This function can throw errors.
     */
    hasParticipant(participant: Entity | ScoreboardIdentity | string): boolean;
    /**
     * @remarks
     * Removes a participant from this scoreboard objective.
     *
     * @worldMutation
     *
     * @param participant
     * Participant to remove from being tracked with this
     * objective.
     * @throws This function can throw errors.
     */
    removeParticipant(participant: Entity | ScoreboardIdentity | string): boolean;
    /**
     * @remarks
     * Sets a score for a participant.
     *
     * @worldMutation
     *
     * @param participant
     * Identity of the participant.
     * @param score
     * New value of the score.
     * @throws This function can throw errors.
     */
    setScore(participant: Entity | ScoreboardIdentity | string, score: number): void;
}

/**
 * 表示分数信息。包含了分数持有者以及它在记分板上的对应的分数。
 * 
 * Contains a pair of a scoreboard participant and its
 * respective score.
 */
export class ScoreboardScoreInfo {
    private constructor();
    /**
     * @remarks
     * 分数归属的分数持有者。
     * 
     * This scoreboard participant for this score.
     *
     */
    readonly participant: ScoreboardIdentity;
    /**
     * @remarks
     * 该记分项上分数持有者的分数。
     * 
     * Score value of the identity for this objective.
     *
     */
    readonly score: number;
}

/**
 * Contains information about user interface elements that are
 * showing up on the screen.
 * @seeExample setTitle.ts
 * @seeExample setTitleAndSubtitle.ts
 * @seeExample countdown.ts
 */
export class ScreenDisplay {
    private constructor();
    /**
     * @remarks
     * Returns true if the current reference to this screen display
     * manager object is valid and functional.
     *
     */
    readonly isValid: boolean;
    /**
     * @remarks
     * @worldMutation
     *
     * @throws This function can throw errors.
     *
     * {@link InvalidEntityError}
     */
    getHiddenHudElements(): HudElement[];
    /**
     * @remarks
     * @worldMutation
     *
     * @throws This function can throw errors.
     *
     * {@link InvalidEntityError}
     */
    hideAllExcept(hudElements?: HudElement[]): void;
    /**
     * @remarks
     * @worldMutation
     *
     * @throws This function can throw errors.
     *
     * {@link InvalidEntityError}
     */
    isForcedHidden(hudElement: HudElement): boolean;
    /**
     * @remarks
     * @worldMutation
     *
     * @throws This function can throw errors.
     *
     * {@link InvalidEntityError}
     */
    resetHudElementsVisibility(): void;
    /**
     * @remarks
     * Set the action bar text - a piece of text that displays
     * beneath the title and above the hot-bar.
     *
     * @worldMutation
     *
     * @param text
     * New value for the action bar text.
     * @throws This function can throw errors.
     *
     * {@link InvalidEntityError}
     *
     * {@link RawMessageError}
     */
    setActionBar(text: (RawMessage | string)[] | RawMessage | string): void;
    /**
     * @remarks
     * Sets visibility of a particular element of the heads up
     * display (HUD).
     *
     * @worldMutation
     *
     * @param visible
     * Whether to set the HUD element to invisible, or to reset it
     * back to its default.
     * @param hudElements
     * Optional list of HUD elements to configure visibility for.
     * @throws This function can throw errors.
     *
     * {@link InvalidEntityError}
     */
    setHudVisibility(visible: HudVisibility, hudElements?: HudElement[]): void;
    /**
     * @remarks
     * Will cause a title to show up on the player's on screen
     * display. Will clear the title if set to empty string. You
     * can optionally specify an additional subtitle as well as
     * fade in, stay and fade out times.
     *
     * @worldMutation
     *
     * @throws This function can throw errors.
     *
     * {@link ArgumentOutOfBoundsError}
     *
     * {@link InvalidEntityError}
     *
     * {@link RawMessageError}
     * @seeExample setTitle.ts
     * @seeExample setTitleAndSubtitle.ts
     * @seeExample countdown.ts
     */
    setTitle(title: (RawMessage | string)[] | RawMessage | string, options?: TitleDisplayOptions): void;
    /**
     * @remarks
     * Updates the subtitle if the subtitle was previously
     * displayed via the setTitle method.
     *
     * @worldMutation
     *
     * @throws This function can throw errors.
     *
     * {@link InvalidEntityError}
     *
     * {@link RawMessageError}
     * @seeExample countdown.ts
     */
    updateSubtitle(subtitle: (RawMessage | string)[] | RawMessage | string): void;
}

/**
 * Returns additional data about a /scriptevent command
 * invocation.
 */
export class ScriptEventCommandMessageAfterEvent {
    private constructor();
    /**
     * @remarks
     * Identifier of this ScriptEvent command message.
     *
     */
    readonly id: string;
    /**
     * @remarks
     * If this command was initiated via an NPC, returns the entity
     * that initiated the NPC dialogue.
     *
     */
    readonly initiator?: Entity;
    /**
     * @remarks
     * Optional additional data passed in with the script event
     * command.
     *
     */
    readonly message: string;
    /**
     * @remarks
     * Source block if this command was triggered via a block
     * (e.g., a commandblock.)
     *
     */
    readonly sourceBlock?: Block;
    /**
     * @remarks
     * Source entity if this command was triggered by an entity
     * (e.g., a NPC).
     *
     */
    readonly sourceEntity?: Entity;
    /**
     * @remarks
     * Returns the type of source that fired this command.
     *
     */
    readonly sourceType: ScriptEventSource;
}

/**
 * Allows for registering an event handler that responds to
 * inbound /scriptevent commands.
 */
export class ScriptEventCommandMessageAfterEventSignal {
    private constructor();
    /**
     * @remarks
     * Registers a new ScriptEvent handler.
     *
     * @worldMutation
     *
     * @earlyExecution
     *
     */
    subscribe(
        callback: (arg0: ScriptEventCommandMessageAfterEvent) => void,
        options?: ScriptEventMessageFilterOptions,
    ): (arg0: ScriptEventCommandMessageAfterEvent) => void;
    /**
     * @remarks
     * Unsubscribes a particular handler for a ScriptEvent event.
     *
     * @worldMutation
     *
     * @earlyExecution
     *
     */
    unsubscribe(callback: (arg0: ScriptEventCommandMessageAfterEvent) => void): void;
}

/**
 * Describes a particular seating position on this rideable
 * entity.
 */
export class Seat {
    private constructor();
    /**
     * @remarks
     * Angle in degrees that a rider is allowed to rotate while
     * riding this entity.
     *
     */
    readonly lockRiderRotation: number;
    /**
     * @remarks
     * A maximum number of riders that this seat can support.
     *
     */
    readonly maxRiderCount: number;
    /**
     * @remarks
     * A minimum number of riders that can be placed in this seat
     * position, if this seat is to be filled.
     *
     */
    readonly minRiderCount: number;
    /**
     * @remarks
     * Physical location of this seat, relative to the entity's
     * location.
     *
     */
    readonly position: Vector3;
    /**
     * @remarks
     * Angle in degrees to rotate riders by.
     *
     */
    readonly seatRotation: number;
}

/**
 * @beta
 * Manages callbacks that are message passing to a server. This
 * event is not currently fully implemented, and should not be
 * used.
 */
export class ServerMessageAfterEventSignal {
    private constructor();
    /**
     * @remarks
     * Adds a callback that will be called when an internal message
     * is passed.
     *
     * @worldMutation
     *
     * @earlyExecution
     *
     */
    subscribe(callback: (arg0: MessageReceiveAfterEvent) => void): (arg0: MessageReceiveAfterEvent) => void;
    /**
     * @remarks
     * Removes a callback from being called when an internal
     * message is passed.
     *
     * @worldMutation
     *
     * @earlyExecution
     *
     */
    unsubscribe(callback: (arg0: MessageReceiveAfterEvent) => void): void;
}

/**
 * Loot item function that modifies the trim on a dropped armor
 * item.
 */
// @ts-ignore Class inheritance allowed for native defined classes
export class SetArmorTrimFunction extends LootItemFunction {
    private constructor();
    /**
     * @remarks
     * The material to apply to the armor trim.
     *
     */
    readonly material: string;
    /**
     * @remarks
     * The pattern to apply to the armor trim.
     *
     */
    readonly pattern: string;
}

/**
 * Loot item function that modifies the type of a banner that
 * drops.
 */
// @ts-ignore Class inheritance allowed for native defined classes
export class SetBannerDetailsFunction extends LootItemFunction {
    private constructor();
    /**
     * @beta
     * @remarks
     * The base color for the dropped banner.
     *
     */
    readonly baseColor: string;
    /**
     * @beta
     * @remarks
     * An array of {@link BannerPattern} objects used to decorate
     * the banner, including color and pattern type.
     *
     */
    readonly patterns: BannerPattern[];
    /**
     * @remarks
     * The type of banner to drop.
     *
     */
    readonly 'type': number;
}

/**
 * Loot item function that modifies the contents of a dropped
 * book.
 */
// @ts-ignore Class inheritance allowed for native defined classes
export class SetBookContentsFunction extends LootItemFunction {
    private constructor();
    /**
     * @remarks
     * The name of the book's author.
     *
     */
    readonly author: string;
    /**
     * @remarks
     * An array of text to be placed in the pages of the book.
     *
     */
    readonly pages: string[];
    /**
     * @remarks
     * The book's title.
     *
     */
    readonly title: string;
}

/**
 * Loot item function that modifies the dropped item's data
 * value based on its color index. Defaults to zero if no color
 * index is set.
 */
// @ts-ignore Class inheritance allowed for native defined classes
export class SetDataFromColorIndexFunction extends LootItemFunction {
    private constructor();
}

/**
 * Loot item function that modifies the number items that drop
 * from the loot pool entry.
 */
// @ts-ignore Class inheritance allowed for native defined classes
export class SetItemCountFunction extends LootItemFunction {
    private constructor();
    /**
     * @remarks
     * The value range from which the function randomly chooses the
     * number of items to drop. Contains minimum and maximum
     * values.
     *
     */
    readonly count: NumberRange;
}

/**
 * Loot item function that modifies the durability value of the
 * item dropped.
 */
// @ts-ignore Class inheritance allowed for native defined classes
export class SetItemDamageFunction extends LootItemFunction {
    private constructor();
    /**
     * @remarks
     * The value range from which the function randomly chooses the
     * durability value to assign. Contains minimum and maximum
     * values. Must always be between 0.0 and 1.0.
     *
     */
    readonly damage: NumberRange;
}

/**
 * Loot item function that modifies the data value of the item
 * dropped.
 */
// @ts-ignore Class inheritance allowed for native defined classes
export class SetItemDataFunction extends LootItemFunction {
    private constructor();
    /**
     * @remarks
     * The value range from which the function randomly chooses the
     * data value to assign. Contains minimum and maximum values.
     *
     */
    readonly data: NumberRange;
}

/**
 * Loot item function that modifies the lore of the item
 * dropped.
 */
// @ts-ignore Class inheritance allowed for native defined classes
export class SetItemLoreFunction extends LootItemFunction {
    private constructor();
    /**
     * @remarks
     * The lore to apply to the dropped item.
     *
     */
    readonly lore: string[];
}

/**
 * Loot item function that modifies the name of the item
 * dropped.
 */
// @ts-ignore Class inheritance allowed for native defined classes
export class SetItemNameFunction extends LootItemFunction {
    private constructor();
    /**
     * @remarks
     * The name to apply to the dropped item.
     *
     */
    readonly name: string;
}

/**
 * Loot item function that modifies an ominous bottle's
 * amplifier value.
 */
// @ts-ignore Class inheritance allowed for native defined classes
export class SetOminousBottleFunction extends LootItemFunction {
    private constructor();
    /**
     * @remarks
     * The value range from which the function randomly chooses the
     * amplifier value to assign. Contains minimum and maximum
     * values.
     *
     */
    readonly amplifier: NumberRange;
}

/**
 * Loot item function that assigns a type to a dropped potion.
 */
// @ts-ignore Class inheritance allowed for native defined classes
export class SetPotionFunction extends LootItemFunction {
    private constructor();
    /**
     * @remarks
     * The id to be assigned to the dropped potion.
     *
     */
    readonly id: string;
}

/**
 * Loot item function that assigns an entity type to a dropped
 * spawn egg. Does not work on any items other than spawn eggs.
 */
// @ts-ignore Class inheritance allowed for native defined classes
export class SetSpawnEggFunction extends LootItemFunction {
    private constructor();
    /**
     * @remarks
     * The entity to be assigned to the dropped egg.
     *
     */
    readonly id: string;
}

/**
 * Loot item function that modifies the effects of a dropped
 * stew item.
 */
// @ts-ignore Class inheritance allowed for native defined classes
export class SetStewEffectFunction extends LootItemFunction {
    private constructor();
    /**
     * @remarks
     * An array of integers corresponding to stew effects to be
     * randomly chosen from and applied to the dropped item.
     *
     */
    readonly effects: number[];
}

/**
 * Provides an adaptable interface for callers to subscribe to
 * an event that fires before the game world shuts down. This
 * event occurs after players have left, but before the world
 * has closed.
 */
export class ShutdownBeforeEventSignal {
    private constructor();
    /**
     * @remarks
     * Adds a new subscriber callback to this event.
     *
     * @worldMutation
     *
     * @earlyExecution
     *
     * @param callback
     * Function callback that is called when this event fires.
     * This closure is called with restricted-execution privilege.
     * @returns
     * Closure that is called with restricted-execution privilege.
     */
    subscribe(callback: (arg0: ShutdownEvent) => void): (arg0: ShutdownEvent) => void;
    /**
     * @remarks
     * Removes a subscriber callback previously subscribed to via
     * the subscribe method.
     *
     * @worldMutation
     *
     * @earlyExecution
     *
     * @param callback
     * Function closure that was previously passed to the subscribe
     * method.
     * This closure is called with restricted-execution privilege.
     */
    unsubscribe(callback: (arg0: ShutdownEvent) => void): void;
}

/**
 * The event object that gets dispatched when the game world is
 * shutting down.
 */
export class ShutdownEvent {
    private constructor();
}

/**
 * Loot item function that processes the dropped item as if it
 * was smelted or cooked in a furnace.
 */
// @ts-ignore Class inheritance allowed for native defined classes
export class SmeltItemFunction extends LootItemFunction {
    private constructor();
}

/**
 * @beta
 * Contains information about a sound thats declared duration
 * elapsed.
 */
export class SoundCompletedAfterEvent {
    private constructor();
    /**
     * @remarks
     * Identifier of the sound instance that completed. Matches the
     * `id` property of the `SoundInstance` returned when the sound
     * was played.
     *
     */
    readonly soundInstanceId: string;
}

/**
 * @beta
 * Manages callbacks that are invoked when a tracked sound's
 * declared duration elapses.
 */
export class SoundCompletedAfterEventSignal {
    private constructor();
    /**
     * @remarks
     * Adds a callback that will be invoked when a tracked sound's
     * declared duration elapses.
     *
     * @worldMutation
     *
     * @earlyExecution
     *
     */
    subscribe(callback: (arg0: SoundCompletedAfterEvent) => void): (arg0: SoundCompletedAfterEvent) => void;
    /**
     * @remarks
     * Removes a callback from being invoked when a tracked sound's
     * declared duration elapses.
     *
     * @worldMutation
     *
     * @earlyExecution
     *
     */
    unsubscribe(callback: (arg0: SoundCompletedAfterEvent) => void): void;
}

/**
 * @beta
 * Static metadata about a sound declared in a
 * sound_definitions.json file.
 */
export class SoundDefinition {
    private constructor();
    /**
     * @remarks
     * Duration metadata declared for this sound. Undefined when
     * the sound definition does not specify a duration.
     *
     */
    readonly durationInfo?: SoundDefinitionDurationInfo;
    /**
     * @remarks
     * Music metadata declared for this sound. Undefined when the
     * sound definition does not specify a music_info block.
     *
     */
    readonly musicInfo?: SoundDefinitionMusicInfo;
    /**
     * @remarks
     * Identifier of the sound event this definition declares, in
     * the form 'namespace:name'.
     *
     */
    readonly soundEventId: string;
    /**
     * @remarks
     * Tag metadata declared for this sound, as a record mapping
     * each tag name to its declared values. A tag declared with a
     * single string value is exposed as a single-element array.
     * Undefined when the sound definition does not specify any
     * tags.
     *
     */
    readonly tags?: Record<string, string[]>;
}

/**
 * @beta
 * Provides read-only access to the sound definitions loaded
 * for the current world.
 */
export class SoundDefinitionRegistry {
    private constructor();
    /**
     * @remarks
     * Returns the sound definitions in the registry, optionally
     * narrowed by a filter.
     *
     * @param filter
     * Optional filter applied to each definition. When omitted,
     * every definition is returned.
     * @returns
     * All sound definitions matching the filter, or every sound
     * definition when no filter is supplied.
     * @throws
     * An error will be thrown if filter.minDuration is greater
     * than filter.maxDuration.
     *
     * {@link InvalidArgumentError}
     */
    getDefinitions(filter?: SoundDefinitionFilter): SoundDefinition[];
}

/**
 * @beta
 * Provides duration and playback information for a sound whose
 * definition declares a duration.
 */
export class SoundDurationInfo {
    private constructor();
    /**
     * @remarks
     * Gets the total duration of the sound in seconds.
     *
     */
    readonly duration: number;
    /**
     * @remarks
     * Gets whether the sound is still being tracked.
     *
     */
    readonly isActive: boolean;
    /**
     * @remarks
     * Returns the elapsed playback time of the sound, in seconds,
     * since it started playing.
     *
     * @worldMutation
     *
     * @returns
     * Elapsed playback time in seconds.
     */
    getPlaybackPosition(): number;
}

/**
 * Represents a handle to a sound that has been played. The
 * handle is required to control the sound while it is playing
 * (for example, to call `stop`, `setVolume`, `setPitch`,
 * `fade`, or `seekTo`). Infinitely-looping sounds (started
 * with `loop: -1`) stop automatically when the last
 * `SoundInstance` reference is dropped; retain the handle for
 * as long as the sound should keep playing.
 */
export class SoundInstance {
    private constructor();
    /**
     * @beta
     * @remarks
     * Gets duration and playback information for this sound.
     *
     */
    readonly durationInfo?: SoundDurationInfo;
    /**
     * @beta
     * @remarks
     * Unique identifier of this sound instance.
     *
     */
    readonly id: string;
    /**
     * @beta
     * @remarks
     * Gets the player this sound was played for.
     *
     */
    readonly recipient?: Player;
    /**
     * @beta
     * @remarks
     * Gets the identifier of the sound event this instance was
     * started with.
     *
     */
    readonly soundEventId: string;
    /**
     * @beta
     * @remarks
     * Fades this sound instance from its current volume to the
     * target volume over the specified duration. To fade in from
     * silence, call `setVolume(0.0)` first; to fade out, pass a
     * target volume of `0.0`.
     *
     * @worldMutation
     *
     * @param duration
     * Duration of the fade in seconds. Must be non-negative.
     * Minimum value: 0
     * @param targetVolume
     * Volume to fade to. Must be non-negative.
     * Minimum value: 0
     */
    fade(duration: number, targetVolume: number): void;
    /**
     * @beta
     * @remarks
     * Pauses this sound.
     *
     * @worldMutation
     *
     */
    pause(): void;
    /**
     * @beta
     * @remarks
     * Resumes this sound after a pause.
     *
     * @worldMutation
     *
     */
    resume(): void;
    /**
     * @beta
     * @remarks
     * Sets the playback position of this sound instance.
     *
     * @worldMutation
     *
     * @param seconds
     * Position to seek to in seconds. Must be non-negative.
     * Minimum value: 0
     */
    seekTo(seconds: number): void;
    /**
     * @beta
     * @remarks
     * Sets the pitch of this sound instance.
     *
     * @worldMutation
     *
     * @param pitch
     * Pitch multiplier between 0.01 and 10.0. A value of 1.0 is
     * normal pitch.
     * Bounds: [0.009999999776482582, 10]
     */
    setPitch(pitch: number): void;
    /**
     * @beta
     * @remarks
     * Sets the volume of this sound instance.
     *
     * @worldMutation
     *
     * @param volume
     * Volume level between 0.0 and 10.0.
     * Bounds: [0, 10]
     */
    setVolume(volume: number): void;
    /**
     * @rc
     * @remarks
     * Stops this sound instance from playing.
     *
     * @worldMutation
     *
     */
    stop(): void;
}

/**
 * Loot item function that applies one or several predefined
 * enchants to the dropped item.
 */
// @ts-ignore Class inheritance allowed for native defined classes
export class SpecificEnchantFunction extends LootItemFunction {
    private constructor();
    readonly enchantments: EnchantInfo[];
}

export class StartupBeforeEventSignal {
    private constructor();
    /**
     * @remarks
     * @worldMutation
     *
     * @earlyExecution
     *
     * @param callback
     * This closure is called with early-execution privilege.
     * @returns
     * Closure that is called with early-execution privilege.
     */
    subscribe(callback: (arg0: StartupEvent) => void): (arg0: StartupEvent) => void;
    /**
     * @remarks
     * @worldMutation
     *
     * @earlyExecution
     *
     * @param callback
     * This closure is called with early-execution privilege.
     */
    unsubscribe(callback: (arg0: StartupEvent) => void): void;
}

export class StartupEvent {
    private constructor();
    /**
     * @remarks
     * @earlyExecution
     *
     */
    readonly blockComponentRegistry: BlockComponentRegistry;
    /**
     * @remarks
     * @earlyExecution
     *
     */
    readonly customCommandRegistry: CustomCommandRegistry;
    /**
     * @remarks
     * @earlyExecution
     *
     */
    readonly dimensionRegistry: DimensionRegistry;
    /**
     * @remarks
     * @earlyExecution
     *
     */
    readonly itemComponentRegistry: ItemComponentRegistry;
}

/**
 * Represents a loaded structure template (.mcstructure file).
 * Structures can be placed in a world using the /structure
 * command or the {@link StructureManager} APIs.
 */
// @ts-ignore Class inheritance allowed for native defined classes
export class Structure extends ISerializable {
    private constructor();
    /**
     * @remarks
     * The name of the structure. The identifier must include a
     * namespace. For structures created via the /structure command
     * or structure blocks, this namespace defaults to
     * "mystructure".
     *
     */
    readonly id: string;
    /**
     * @remarks
     * Returns whether the Structure is valid. The Structure may
     * become invalid if it is deleted.
     *
     */
    readonly isValid: boolean;
    /**
     * @remarks
     * The dimensions of the structure. For example, a single block
     * structure will have a size of {x:1, y:1, z:1}
     *
     * @throws This property can throw when used.
     *
     * {@link InvalidStructureError}
     */
    readonly size: Vector3;
    /**
     * @remarks
     * Returns a BlockPermutation representing the block contained
     * within the Structure at the given location.
     *
     * @param location
     * The block location relative to the Structure's origin.
     * @returns
     * Returns a BlockPermutation. Returns undefined if a block
     * does not exist at the given location.
     * @throws
     * Throws if the location is outside the structure's bounds.
     * Throws if the Structure has been deleted.
     *
     * {@link InvalidArgumentError}
     *
     * {@link InvalidStructureError}
     */
    getBlockPermutation(location: Vector3): BlockPermutation | undefined;
    /**
     * @remarks
     * Returns whether the block at the given location is
     * waterlogged.
     *
     * @param location
     * The block location relative to the Structure's origin.
     * @returns
     * Returns whether the block at the given location is
     * waterlogged. Returns false if a block does not exist at the
     * given location.
     * @throws
     * Throws if the location is outside the structure's bounds.
     * Throws if the Structure has been deleted.
     *
     * {@link InvalidArgumentError}
     *
     * {@link InvalidStructureError}
     */
    getIsWaterlogged(location: Vector3): boolean;
    /**
     * @remarks
     * Creates a copy of a Structure and saves it with a new name.
     *
     * @worldMutation
     *
     * @param identifier
     * The name of the newly created Structure.
     * @param saveMode
     * Determines how the Structure should be saved. Defaults to
     * saving to the world.
     * Defaults to: 1
     * @returns
     * Returns the newly created structure.
     * @throws
     * Throws if the identifier is invalid. A valid identifier must
     * include a namespace and must be unique.
     * Throws if the Structure has been deleted.
     *
     * {@link EngineError}
     *
     * {@link InvalidArgumentError}
     *
     * {@link InvalidStructureError}
     */
    saveAs(identifier: string, saveMode?: StructureSaveMode): Structure;
    /**
     * @remarks
     * Saves a modified Structure to the world file.
     *
     * @worldMutation
     *
     * @throws
     * Throws if the Structure has been deleted.
     *
     * {@link InvalidStructureError}
     */
    saveToWorld(): void;
    /**
     * @remarks
     * Sets a BlockPermutation within a Structure.
     *
     * @worldMutation
     *
     * @param location
     * The block location relative to the Structure's origin.
     * @param blockPermutation
     * The BlockPermutation to set.
     * Defaults to: null
     * @param waterlogged
     * Specifies whether the block should be waterlogged. Air and
     * undefined blocks cannot be waterlogged.
     * Defaults to: false
     * @throws
     * Throws if the type of block is StructureVoid.
     * Throws if the block is undefined and waterlogged is set to
     * true.
     * Throws if the block is air and waterlogged is set to true.
     * Throws if the location is outside the structure's bounds.
     * Throws if the Structure has been deleted.
     *
     * {@link InvalidArgumentError}
     *
     * {@link InvalidStructureError}
     */
    setBlockPermutation(location: Vector3, blockPermutation?: BlockPermutation, waterlogged?: boolean): void;
}

/**
 * Manager for Structure related APIs. Includes APIs for
 * creating, getting, placing and deleting Structures.
 */
export class StructureManager {
    private constructor();
    /**
     * @remarks
     * Creates an empty Structure in memory. Use {@link
     * Structure.setBlockPermutation} to populate the structure
     * with blocks and save changes with {@link Structure.saveAs}.
     *
     * @worldMutation
     *
     * @param identifier
     * The name of the structure. A valid identifier must include a
     * namespace and must be unique.
     * @param size
     * The size of the structure. For example, to create a single
     * block structure the size should be {x:1, y:1, z:1}.
     * @param saveMode
     * How the Structure should be saved upon creation. Defaults to
     * StructureSaveMode.Memory.
     * Defaults to: 0
     * @returns
     * Returns the newly created Structure.
     * @throws
     * Throws if the identifier is invalid. A valid identifier must
     * include a namespace and must be unique.
     *
     * {@link EngineError}
     *
     * {@link InvalidArgumentError}
     */
    createEmpty(identifier: string, size: Vector3, saveMode?: StructureSaveMode): Structure;
    /**
     * @remarks
     * Creates a new Structure from blocks in the world. This is
     * functionally equivalent to the /structure save command.
     *
     * @worldMutation
     *
     * @param identifier
     * The name of the structure. A valid identifier must include a
     * namespace and must be unique.
     * @param dimension
     * The dimension where the blocks should be read from.
     * @param options
     * Additional options for creating a structure from the world.
     * @returns
     * Returns the newly created Structure.
     * @throws
     * Throws if the identifier is invalid. A valid identifier must
     * include a namespace and must be unique.
     * Throws if the structure bounds exceed the maximum size.
     * Throws if the structure bounds contains blocks outside the
     * world bounds.
     *
     *
     * {@link InvalidArgumentError}
     */
    createFromWorld(
        identifier: string,
        dimension: Dimension,
        from: Vector3,
        to: Vector3,
        options?: StructureCreateOptions,
    ): Structure;
    /**
     * @remarks
     * Deletes a structure from memory and from the world if it
     * exists.
     *
     * @worldMutation
     *
     * @param structure
     * The structure identifier or Structure object that should be
     * deleted. Note, a Structure object will become invalid after
     * it is deleted.
     * @returns
     * Returns whether the structure was removed.
     * @throws
     * Throws if a structure cannot be removed. For example, a
     * structure loaded from a Behavior Pack.
     *
     * {@link InvalidArgumentError}
     */
    delete(structure: string | Structure): boolean;
    /**
     * @remarks
     * Gets a Structure that is saved to memory or the world.
     *
     * @worldMutation
     *
     * @param identifier
     * The name of the structure to get.
     * @returns
     * Returns a Structure if it exists, otherwise undefined.
     */
    get(identifier: string): Structure | undefined;
    /**
     * @remarks
     * Returns a list of all structures contained in behavior
     * packs. Does not include structures saved to the world or in
     * memory.
     *
     * @worldMutation
     *
     * @returns
     * The list of structure identifiers.
     */
    getPackStructureIds(): string[];
    /**
     * @remarks
     * Returns a list of all structures saved to the world and to
     * memory. Does not include structures contained in behavior
     * packs.
     *
     * @worldMutation
     *
     * @returns
     * The list of structure identifiers.
     */
    getWorldStructureIds(): string[];
    /**
     * @remarks
     * Places a structure in the world. Structures placed in
     * unloaded chunks will be queued for loading.
     *
     * @worldMutation
     *
     * @param structure
     * The structure's identifier or a Structure object.
     * @param dimension
     * The dimension where the Structure should be placed.
     * @param location
     * The location within the dimension where the Structure should
     * be placed.
     * @param options
     * Additional options for Structure placement.
     * @throws
     * Throws if the integrity value is outside of the range [0,1]
     * Throws if the integrity seed is invalid.
     * Throws if the placement location contains blocks that are
     * outside the world bounds.
     *
     * {@link ArgumentOutOfBoundsError}
     *
     * {@link InvalidArgumentError}
     *
     * {@link InvalidStructureError}
     */
    place(
        structure: string | Structure,
        dimension: Dimension,
        location: Vector3,
        options?: StructurePlaceOptions,
    ): void;
    /**
     * @remarks
     * Places a partial jigsaw structure in the world. This is
     * useful for debugging connections between jigsaw blocks.
     *
     * @worldMutation
     *
     * @param pool
     * The identifier of the template pool to start from.
     * @param targetJigsaw
     * The name of the jigsaw block to start from. This block must
     * be included in at least one of the starting pool structure
     * templates.
     * @param maxDepth
     * The maximum recursion depth for the jigsaw structure.
     * Bounds: [1, 20]
     * @param dimension
     * The dimension to place the jigsaw structure in.
     * @param location
     * The location where the jigsaw structure will begin
     * generating relative to the targetJigsaw block.
     * @param options
     * Optional settings to use when generating the jigsaw
     * structure.
     * @returns
     * Returns a {@link BlockBoundingBox} object which represents
     * the maximum bounds of the jigsaw structure.
     * @throws
     * Throws if maxDepth is outside of the range [1,20]
     * Throws if generation fails due to invalid parameters or
     * jigsaw configuration.
     * Throws if the placement location contains blocks that are
     * outside the world bounds.
     *
     * {@link PlaceJigsawError}
     */
    placeJigsaw(
        pool: string,
        targetJigsaw: string,
        maxDepth: number,
        dimension: Dimension,
        location: Vector3,
        options?: JigsawPlaceOptions,
    ): BlockBoundingBox;
    /**
     * @remarks
     * Places a jigsaw structure in the world.
     *
     * @worldMutation
     *
     * @param identifier
     * The identifier of the jigsaw structure.
     * @param dimension
     * The dimension to place the jigsaw structure in.
     * @param location
     * The location where the jigsaw structure will begin
     * generating. Note that the y value will be overridden by the
     * structure's start height unless the
     * ignoreStarJigsawStructurePlaceOptions ignoreStartHeight
     * option is set.
     * @param options
     * Optional settings to use when generating the jigsaw
     * structure.
     * @returns
     * Returns a {@link BlockBoundingBox} object which represents
     * the maximum bounds of the jigsaw structure.
     * @throws
     * Throws if generation fails due to invalid parameters or
     * jigsaw configuration.
     * Throws if the placement location contains blocks that are
     * outside the world bounds.
     *
     * {@link PlaceJigsawError}
     */
    placeJigsawStructure(
        identifier: string,
        dimension: Dimension,
        location: Vector3,
        options?: JigsawStructurePlaceOptions,
    ): BlockBoundingBox;
}

/**
 * A class that provides system-level events and functions.
 */
export class System {
    private constructor();
    /**
     * @remarks
     * Returns a collection of after-events for system-level
     * operations.
     *
     * @earlyExecution
     *
     */
    readonly afterEvents: SystemAfterEvents;
    /**
     * @remarks
     * Returns a collection of before-events for system-level
     * operations.
     *
     * @earlyExecution
     *
     */
    readonly beforeEvents: SystemBeforeEvents;
    /**
     * @remarks
     * Represents the current world tick of the server.
     *
     * @earlyExecution
     *
     */
    readonly currentTick: number;
    /**
     * @remarks
     * Returns true if this is a world where the editor is
     * currently loaded, returns false otherwise.
     *
     * @earlyExecution
     *
     */
    readonly isEditorWorld: boolean;
    /**
     * @remarks
     * Contains the device information for the server.
     *
     * @earlyExecution
     *
     */
    readonly serverSystemInfo: SystemInfo;
    /**
     * @remarks
     * Cancels the execution of a job queued via {@link
     * System.runJob}.
     *
     * @earlyExecution
     *
     * @param jobId
     * The job ID returned from {@link System.runJob}.
     */
    clearJob(jobId: number): void;
    /**
     * @remarks
     * Cancels the execution of a function run that was previously
     * scheduled via {@link System.run}.
     *
     * @earlyExecution
     *
     */
    clearRun(runId: number): void;
    /**
     * @remarks
     * Runs a specified function at the next available future time.
     * This is frequently used to implement delayed behaviors and
     * game loops. When run within the context of an event handler,
     * this will generally run the code at the end of the same tick
     * where the event occurred. When run in other code (a
     * system.run callout), this will run the function in the next
     * tick. Note, however, that depending on load on the system,
     * running in the same or next tick is not guaranteed.
     *
     * @earlyExecution
     *
     * @param callback
     * Function callback to run at the next game tick.
     * @returns
     * An opaque identifier that can be used with the `clearRun`
     * function to cancel the execution of this run.
     * @seeExample trapTick.ts
     */
    run(callback: () => void): number;
    /**
     * @remarks
     * Runs a set of code on an interval.
     *
     * @earlyExecution
     *
     * @param callback
     * Functional code that will run when this interval occurs.
     * @param tickInterval
     * An interval of every N ticks that the callback will be
     * called upon.
     * @returns
     * An opaque handle that can be used with the clearRun method
     * to stop the run of this function on an interval.
     * @seeExample every30Seconds.ts
     */
    runInterval(callback: () => void, tickInterval?: number): number;
    /**
     * @remarks
     * Queues a generator to run until completion.  The generator
     * will be given a time slice each tick, and will be run until
     * it yields or completes.
     *
     * @earlyExecution
     *
     * @param generator
     * The instance of the generator to run.
     * @returns
     * An opaque handle that can be used with {@link
     * System.clearJob} to stop the run of this generator.
     * @seeExample cubeGenerator.ts
     */
    runJob(generator: Generator<void, void, void>): number;
    /**
     * @remarks
     * Runs a set of code at a future time specified by tickDelay.
     *
     * @earlyExecution
     *
     * @param callback
     * Functional code that will run when this timeout occurs.
     * @param tickDelay
     * Amount of time, in ticks, before the interval will be
     * called.
     * @returns
     * An opaque handle that can be used with the clearRun method
     * to stop the run of this function on an interval.
     */
    runTimeout(callback: () => void, tickDelay?: number): number;
    /**
     * @remarks
     * Causes an event to fire within script with the specified
     * message ID and payload.
     *
     * @param id
     * Identifier of the message to send. This is custom and
     * dependent on the kinds of behavior packs and content you may
     * have installed within the world.
     * @param message
     * Data component of the message to send. This is custom and
     * dependent on the kinds of behavior packs and content you may
     * have installed within the world.
     * @throws This function can throw errors.
     *
     * {@link EngineError}
     *
     * {@link InvalidArgumentError}
     *
     * {@link NamespaceNameError}
     */
    sendScriptEvent(id: string, message: string): void;
    /**
     * @remarks
     * waitTicks returns a promise that resolves after the
     * requested number of ticks.
     *
     * @earlyExecution
     *
     * @param ticks
     * The amount of ticks to wait. Minimum value is 1.
     * @returns
     * A promise that is resolved when the specified amount of
     * ticks have occurred.
     * @throws This function can throw errors.
     *
     * {@link EngineError}
     */
    waitTicks(ticks: number): Promise<void>;
}

/**
 * Provides a set of events that fire within the broader
 * scripting system within Minecraft.
 */
export class SystemAfterEvents {
    private constructor();
    /**
     * @remarks
     * An event that fires when a /scriptevent command is set. This
     * provides a way for commands and other systems to trigger
     * behavior within script.
     *
     * @earlyExecution
     *
     */
    readonly scriptEventReceive: ScriptEventCommandMessageAfterEventSignal;
}

/**
 * A set of events that fire before an actual action occurs. In
 * most cases, you can potentially cancel or modify the
 * impending event. Note that in before events any APIs that
 * modify gameplay state will not function and will throw an
 * error.
 */
export class SystemBeforeEvents {
    private constructor();
    /**
     * @remarks
     * @earlyExecution
     *
     */
    readonly shutdown: ShutdownBeforeEventSignal;
    /**
     * @remarks
     * @earlyExecution
     *
     */
    readonly startup: StartupBeforeEventSignal;
    /**
     * @beta
     * @remarks
     * Fires when the scripting watchdog shuts down the server. The
     * can be due to using too much memory, or by causing
     * significant slowdown or hang.
     * To prevent shutdown, set the event's cancel property to
     * true.
     *
     * @earlyExecution
     *
     */
    readonly watchdogTerminate: WatchdogTerminateBeforeEventSignal;
}

/**
 * Contains device information, like memory tier.
 */
export class SystemInfo {
    private constructor();
    /**
     * @remarks
     * Describes the memory of the device.
     *
     */
    readonly memoryTier: MemoryTier;
}

/**
 * Contains information related to changes to a target block
 * hit.
 */
// @ts-ignore Class inheritance allowed for native defined classes
export class TargetBlockHitAfterEvent extends BlockEvent {
    private constructor();
    /**
     * @remarks
     * The position where the source hit the block.
     *
     */
    readonly hitVector: Vector3;
    /**
     * @remarks
     * The redstone power before the block is hit.
     *
     */
    readonly previousRedstonePower: number;
    /**
     * @remarks
     * The redstone power at the time the block is hit.
     *
     */
    readonly redstonePower: number;
    /**
     * @remarks
     * Optional source that hit the target block.
     *
     */
    readonly source: Entity;
}

/**
 * Manages callbacks that are connected to when a target block
 * is hit.
 */
export class TargetBlockHitAfterEventSignal {
    private constructor();
    /**
     * @remarks
     * Adds a callback that will be called when a target block is
     * hit.
     *
     * @worldMutation
     *
     * @earlyExecution
     *
     */
    subscribe(callback: (arg0: TargetBlockHitAfterEvent) => void): (arg0: TargetBlockHitAfterEvent) => void;
    /**
     * @remarks
     * Removes a callback from being called when a target block is
     * hit.
     *
     * @worldMutation
     *
     * @earlyExecution
     *
     */
    unsubscribe(callback: (arg0: TargetBlockHitAfterEvent) => void): void;
}

/**
 * A primitive shape class that represents a text label in the
 * world with a background.
 */
// @ts-ignore Class inheritance allowed for native defined classes
export class TextPrimitive extends PrimitiveShape {
    /**
     * @remarks
     * If set to true, the text primitive will render the back-face
     * of the background. Defaults to true but will always be false
     * if 'useRotation' is set to false.
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
     * If set to true, the text primitive will render the back-face
     * of the text. Defaults to true but will always be false if
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
    constructor(location: DimensionLocation | Vector3, text: RawMessage | string);
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

/**
 * This manager is used to add, remove or query temporary
 * ticking areas to a dimension. These ticking areas are
 * limited by a fixed amount of ticking chunks per pack
 * independent of the command limits. Cannot modify or query
 * ticking areas added by other packs or commands.
 */
export class TickingAreaManager {
    private constructor();
    /**
     * @remarks
     * The number of currently ticking chunks in this manager.
     *
     */
    readonly chunkCount: number;
    /**
     * @remarks
     * The maximum number of allowed ticking chunks. Overlapping
     * ticking area chunks do count towards total.
     *
     */
    readonly maxChunkCount: number;
    /**
     * @remarks
     * Creates a ticking area. Promise will return when all the
     * chunks in the area are loaded and ticking.
     *
     * @worldMutation
     *
     * @throws This function can throw errors.
     *
     * {@link EngineError}
     *
     * {@link TickingAreaError}
     */
    createTickingArea(identifier: string, options: TickingAreaOptions): Promise<void>;
    /**
     * @remarks
     * Gets all ticking areas added by this manager.
     *
     * @worldMutation
     *
     * @throws This function can throw errors.
     *
     * {@link EngineError}
     */
    getAllTickingAreas(): TickingArea[];
    /**
     * @remarks
     * Tries to get specific ticking area by identifier.
     *
     * @worldMutation
     *
     * @throws This function can throw errors.
     *
     * {@link EngineError}
     */
    getTickingArea(identifier: string | TickingArea): TickingArea | undefined;
    /**
     * @remarks
     * Returns true if the manager has enough chunk capacity for
     * the ticking area and false otherwise. Will also return false
     * if the length or width exceeds the 255 chunk limit.
     *
     * @worldMutation
     *
     */
    hasCapacity(options: TickingAreaOptions): boolean;
    /**
     * @remarks
     * Returns true if the identifier is already in the manager and
     * false otherwise.
     *
     * @worldMutation
     *
     */
    hasTickingArea(identifier: string): boolean;
    /**
     * @remarks
     * Removes all ticking areas added by this manager.
     *
     * @worldMutation
     *
     * @throws This function can throw errors.
     *
     * {@link EngineError}
     */
    removeAllTickingAreas(): void;
    /**
     * @remarks
     * Removes specific ticking area by unique identifier.
     *
     * @worldMutation
     *
     * @throws This function can throw errors.
     *
     * {@link EngineError}
     *
     * {@link TickingAreaError}
     */
    removeTickingArea(identifier: string | TickingArea): void;
}

/**
 * Represents a trigger for firing an event.
 */
export class Trigger {
    /**
     * @remarks
     * Event name of the trigger.
     *
     */
    eventName: string;
    /**
     * @remarks
     * Creates a new trigger.
     *
     */
    constructor(eventName: string);
}

/**
 * Contains information related to changes to a trip wire trip.
 * @seeExample tripWireTripEvent.ts
 */
// @ts-ignore Class inheritance allowed for native defined classes
export class TripWireTripAfterEvent extends BlockEvent {
    private constructor();
    /**
     * @remarks
     * Whether or not the block has redstone power.
     *
     */
    readonly isPowered: boolean;
    /**
     * @remarks
     * The sources that triggered the trip wire to trip.
     *
     */
    readonly sources: Entity[];
}

/**
 * Manages callbacks that are connected to when a trip wire is
 * tripped.
 * @seeExample tripWireTripEvent.ts
 */
export class TripWireTripAfterEventSignal {
    private constructor();
    /**
     * @remarks
     * Adds a callback that will be called when a trip wire is
     * tripped.
     *
     * @worldMutation
     *
     * @earlyExecution
     *
     */
    subscribe(callback: (arg0: TripWireTripAfterEvent) => void): (arg0: TripWireTripAfterEvent) => void;
    /**
     * @remarks
     * Removes a callback from being called when a trip wire is
     * tripped.
     *
     * @worldMutation
     *
     * @earlyExecution
     *
     */
    unsubscribe(callback: (arg0: TripWireTripAfterEvent) => void): void;
}

/**
 * @beta
 * Contains information related to a script watchdog
 * termination.
 */
export class WatchdogTerminateBeforeEvent {
    private constructor();
    /**
     * @remarks
     * If set to true, cancels the termination of the script
     * runtime. Note that depending on server configuration
     * settings, cancellation of the termination may not be
     * allowed.
     *
     */
    cancel: boolean;
    /**
     * @remarks
     * Contains the reason why a script runtime is to be
     * terminated.
     *
     */
    readonly terminateReason: WatchdogTerminateReason;
}

/**
 * @beta
 * Manages callbacks that are connected to a callback that will
 * be called when a script runtime is being terminated due to a
 * violation of the performance watchdog system.
 */
export class WatchdogTerminateBeforeEventSignal {
    private constructor();
    /**
     * @remarks
     * Adds a callback that will be called when a script runtime is
     * being terminated due to a violation of the performance
     * watchdog system.
     *
     * @worldMutation
     *
     * @earlyExecution
     *
     * @param callback
     * This closure is called with restricted-execution privilege.
     * @returns
     * Closure that is called with restricted-execution privilege.
     */
    subscribe(callback: (arg0: WatchdogTerminateBeforeEvent) => void): (arg0: WatchdogTerminateBeforeEvent) => void;
    /**
     * @remarks
     * Removes a callback from being called when a script runtime
     * is being terminated due to a violation of the performance
     * watchdog system.
     *
     * @worldMutation
     *
     * @earlyExecution
     *
     * @param callback
     * This closure is called with restricted-execution privilege.
     */
    unsubscribe(callback: (arg0: WatchdogTerminateBeforeEvent) => void): void;
}

/**
 * Base class for waypoints displayed on the player's locator
 * bar. Waypoints can track locations or entities and are
 * rendered with customizable textures and colors.
 *
 * Waypoints act as shared handles that can be added to
 * multiple players' locator bars. When you modify a waypoint's
 * properties (such as color, texture, or enabled state), the
 * changes are reflected for all players who have that waypoint
 * in their locator bar. This allows you to efficiently manage
 * waypoints across multiple players without creating separate
 * instances for each player.
 */
export class Waypoint {
    private constructor();
    /**
     * @remarks
     * Optional {@link RGB} color tint applied to the waypoint
     * icon. If not specified, the waypoint uses its default color.
     *
     * @worldMutation
     *
     */
    color?: RGB;
    /**
     * @remarks
     * Controls whether the waypoint is currently displayed on the
     * player's screen. When disabled, the waypoint is hidden but
     * remains valid.
     *
     * @worldMutation
     *
     */
    isEnabled: boolean;
    /**
     * @remarks
     * Returns whether the waypoint is currently valid. A waypoint
     * becomes invalid when its tracked entity is no longer valid.
     *
     */
    readonly isValid: boolean;
    /**
     * @remarks
     * The {@link WaypointTextureSelector} that determines which
     * icon texture is displayed for the waypoint based on distance
     * or other criteria.
     *
     * @worldMutation
     *
     */
    textureSelector: WaypointTextureSelector;
    /**
     * @remarks
     * Gets the current {@link DimensionLocation} of the waypoint.
     * For entity waypoints, this returns the entity's current
     * position. For location waypoints, this returns the stored
     * location.
     *
     * @worldMutation
     *
     * @throws This function can throw errors.
     *
     * {@link InvalidWaypointError}
     *
     * {@link InvalidWaypointTextureSelectorError}
     */
    getDimensionLocation(): DimensionLocation;
    /**
     * @remarks
     * Removes the waypoint from all locator bars it has been added
     * to. This affects all players who have this waypoint in their
     * locator bar.
     *
     * @worldMutation
     *
     */
    remove(): void;
}

/**
 * Contains information related to changes in weather in the
 * environment.
 */
export class WeatherChangeAfterEvent {
    private constructor();
    /**
     * @remarks
     * Dimension in which the weather has changed.
     *
     */
    readonly dimension: string;
    /**
     * @remarks
     * The weather type after the weather was changed.
     *
     */
    readonly newWeather: WeatherType;
    /**
     * @remarks
     * The weather type before the weather was changed.
     *
     */
    readonly previousWeather: WeatherType;
}

/**
 * Manages callbacks that are connected to weather changing.
 */
export class WeatherChangeAfterEventSignal {
    private constructor();
    /**
     * @remarks
     * Adds a callback that will be called when weather changes.
     *
     * @worldMutation
     *
     * @earlyExecution
     *
     */
    subscribe(callback: (arg0: WeatherChangeAfterEvent) => void): (arg0: WeatherChangeAfterEvent) => void;
    /**
     * @remarks
     * Removes a callback from being called when weather changes.
     *
     * @worldMutation
     *
     * @earlyExecution
     *
     */
    unsubscribe(callback: (arg0: WeatherChangeAfterEvent) => void): void;
}

/**
 * Contains information related to changes in weather in the
 * environment.
 */
export class WeatherChangeBeforeEvent {
    private constructor();
    /**
     * @remarks
     * If set to true the weather change will be cancelled.
     *
     */
    cancel: boolean;
    /**
     * @remarks
     * Sets the duration of the new weather (in ticks).
     *
     */
    duration: number;
    /**
     * @remarks
     * The type of weather that will be applied.
     *
     */
    newWeather: WeatherType;
    /**
     * @remarks
     * The type of weather that it was prior to the event being
     * fired.
     *
     */
    readonly previousWeather: WeatherType;
}

/**
 * Manages callbacks that are connected to before weather
 * changing.
 */
export class WeatherChangeBeforeEventSignal {
    private constructor();
    /**
     * @remarks
     * Adds a callback that will be called before weather changes.
     *
     * @worldMutation
     *
     * @earlyExecution
     *
     * @param callback
     * This closure is called with restricted-execution privilege.
     * @returns
     * Closure that is called with restricted-execution privilege.
     */
    subscribe(callback: (arg0: WeatherChangeBeforeEvent) => void): (arg0: WeatherChangeBeforeEvent) => void;
    /**
     * @remarks
     * Removes a callback from being called before weather changes.
     *
     * @worldMutation
     *
     * @earlyExecution
     *
     * @param callback
     * This closure is called with restricted-execution privilege.
     */
    unsubscribe(callback: (arg0: WeatherChangeBeforeEvent) => void): void;
}

/**
 * 表示一个世界。包含了世界的各种状态，即一系列维度以及 Minecraft 的环境。
 * 
 * A class that wraps the state of a world - a set of
 * dimensions and the environment of Minecraft.
 */
export class World {
    private constructor();
    /**
     * @remarks
     * Contains a set of events that are applicable to the entirety
     * of the world.  Event callbacks are called in a deferred
     * manner. Event callbacks are executed in read-write mode.
     *
     * @earlyExecution
     *
     */
    readonly afterEvents: WorldAfterEvents;
    /**
     * @beta
     * @remarks
     * Enables or disables cheats.
     *
     * @worldMutation
     *
     */
    allowCheats: boolean;
    /**
     * @remarks
     * Contains a set of events that are applicable to the entirety
     * of the world. Event callbacks are called immediately. Event
     * callbacks are executed in read-only mode.
     *
     * @earlyExecution
     *
     * @seeExample customCommand.ts
     */
    readonly beforeEvents: WorldBeforeEvents;
    /**
     * @remarks
     * The game rules that apply to the world.
     *
     */
    readonly gameRules: GameRules;
    readonly isHardcore: boolean;
    /**
     * @remarks
     * Manager for adding and removing primitive text objects in
     * the world.
     *
     */
    readonly primitiveShapesManager: PrimitiveShapesManager;
    /**
     * @remarks
     * 全局的、唯一的记分板对象。
     * 
     * Returns the general global scoreboard that applies to the
     * world.
     *
     */
    readonly scoreboard: Scoreboard;
    /**
     * @remarks
     * The world seed.
     *
     */
    readonly seed: string;
    /**
     * @beta
     * @remarks
     * Provides read-only access to the sound definitions loaded
     * for this world.
     *
     */
    readonly soundDefinitionRegistry: SoundDefinitionRegistry;
    /**
     * @remarks
     * Returns the manager for {@link Structure} related APIs.
     *
     */
    readonly structureManager: StructureManager;
    /**
     * @remarks
     * Manager for adding, removing and querying pack specific
     * ticking areas.
     *
     */
    readonly tickingAreaManager: TickingAreaManager;
    /**
     * @beta
     * @remarks
     * A method that is internal-only, used for broadcasting
     * specific messages between client and server.
     *
     * @worldMutation
     *
     * @param id
     * The message identifier.
     * @param value
     * The message.
     */
    broadcastClientMessage(id: string, value: string): void;
    /**
     * @remarks
     * Clears the set of dynamic properties declared for this
     * behavior pack within the world.
     *
     */
    clearDynamicProperties(): void;
    /**
     * @remarks
     * 获取自游戏开始以来流逝的时间（计算公式：`day*24000+daytime`）。
     * 时间的流逝受到游戏规则 `dodaylightcycle` 的影响。
     * 
     * Returns the absolute time since the start of the world.
     * @returns 自游戏开始以来流逝的时间，以刻为单位。
     *
     */
    getAbsoluteTime(): number;
    /**
     * @remarks
     * The aim-assist presets and categories that can be used in
     * the world.
     *
     */
    getAimAssist(): AimAssistRegistry;
    /**
     * @remarks
     * 获取一个包含了游戏中所有玩家的对象的数组。
     * 
     * Returns an array of all active players within the world.
     * @returns 返回包含了游戏中所有玩家的对象的数组。
     * @throws This function can throw errors.
     *
     * {@link CommandError}
     *
     * {@link InvalidArgumentError}
     */
    getAllPlayers(): Player[];
    /**
     * @remarks
     * Returns the current day.
     *
     * @returns
     * The current day, determined by the world time divided by the
     * number of ticks per day. New worlds start at day 0.
     */
    getDay(): number;
    /**
     * @remarks
     * Returns the default Overworld spawn location.
     *
     * @returns
     * The default Overworld spawn location. By default, the Y
     * coordinate is 32767, indicating a player's spawn height is
     * not fixed and will be determined by surrounding blocks.
     */
    getDefaultSpawnLocation(): Vector3;
    /**
     * @remarks
     * Gets the difficulty from the world.
     *
     * @returns
     * Returns the world difficulty.
     */
    getDifficulty(): Difficulty;
    /**
     * @remarks
     * 由 `dimensionId` 获取维度对象。
     * 
     * Returns a dimension object.
     *
     * @param dimensionId
     * 要获取的维度的标识符。
     * 
     * The name of the dimension. For example, "overworld",
     * "nether" or "the_end".
     * @returns
     * 与 `dimensionId` 关联的维度对象。
     * 
     * The requested dimension
     * @throws
     * 若 `dimensionId` 不与任何维度关联，抛出 `"Dimension '<dimensionId>' is invalid"`。
     *
     * Throws if the given dimension name is invalid
     */
    getDimension(dimensionId: string): Dimension;
    /**
     * @remarks
     * 获取由 `identifier` 指定的世界中已定义的动态属性的值。
     * 
     * Returns a property value.
     *
     * @param identifier
     * 动态属性的标识符。
     * 
     * The property identifier.
     * @returns
     * 返回动态属性 `identifier` 的值。属性的值尚未设定时，返回 `undefined`。
     * 
     * Returns the value for the property, or undefined if the
     * property has not been set.
     * @throws
     * 若并未注册以 `identifier` 为标识符的动态属性，抛出 `"Dynamic Property '<identifier>' is not defined"` 。
     * 
     * Throws if the given dynamic property identifier is not
     * defined.
     * @seeExample incrementDynamicProperty.ts
     * @seeExample incrementDynamicPropertyInJsonBlob.ts
     */
    getDynamicProperty(identifier: string): boolean | number | string | Vector3 | undefined;
    /**
     * @remarks
     * Gets a set of dynamic property identifiers that have been
     * set in this world.
     *
     * @returns
     * A string array of active dynamic property identifiers.
     */
    getDynamicPropertyIds(): string[];
    /**
     * @remarks
     * Gets the total byte count of dynamic properties. This could
     * potentially be used for your own analytics to ensure you're
     * not storing gigantic sets of dynamic properties.
     *
     */
    getDynamicPropertyTotalByteCount(): number;
    /**
     * @remarks
     * Returns an entity based on the provided id.
     *
     * @param id
     * The id of the entity.
     * @returns
     * The requested entity object.
     * @throws
     * Throws if the given entity id is invalid.
     */
    getEntity(id: string): Entity | undefined;
    /**
     * @remarks
     * Returns a manager capable of generating loot from an
     * assortment of sources.
     *
     * @returns
     * A loot table manager with a variety of loot generation
     * methods.
     */
    getLootTableManager(): LootTableManager;
    /**
     * @remarks
     * Returns the MoonPhase for the current time.
     *
     */
    getMoonPhase(): MoonPhase;
    /**
     * @remarks
     * Returns a map of pack setting name and value pairs.
     *
     * @earlyExecution
     *
     */
    getPackSettings(): Record<string, boolean | number | string>;
    /**
     * @remarks
     * 列出世界上的玩家，可使用 `options` 指定的实体查询选项对其进行筛选。
     * 
     * Returns a set of players based on a set of conditions
     * defined via the EntityQueryOptions set of filter criteria.
     *
     * @param options
     * 可选的参数，用作于筛选指定条件的玩家。
     *
     * 注意，不能使用接口中的 `type`、`location`、`maxDistance`、`minDistance` 或 `volume` 属性。
     * 
     * Additional options that can be used to filter the set of
     * players returned.
     * @returns
     * A player array.
     * @throws
     * 若向 `options` 传入的对象含有 `type` 属性，抛出 `"command.generic.invalidPlayerType"`。
     * 
     * 若向 `options` 传入的对象含有 `location`、`maxDistance`、`minDistance` 或 `volume` 属性，抛出 `"EntityQueryOptions property '<property>' is incompatible with function world.getPlayers"`。
     * 
     * Throws if the provided EntityQueryOptions are invalid.
     *
     * {@link CommandError}
     *
     * {@link InvalidArgumentError}
     */
    getPlayers(options?: EntityQueryOptions): Player[];
    /**
     * @remarks
     * 返回当前一天中的时间。
     * 
     * Returns the time of day.
     *
     * @returns
     * 当前一天中的时间，以刻为单位，为 `0` 至 `24000` 之间的整数。
     * 
     * The time of day, in ticks, between 0 and 24000.
     */
    getTimeOfDay(): number;
    /**
     * @remarks
     * 停止正在播放的音乐，并开始向玩家播放指定音乐。播放类别不为音乐的声音项目不会有任何效果。
     * 
     * Plays a particular music track for all players.
     *
     * @worldMutation
     *
     * @param trackId 声音项目的标识符，要求声音项目的类别为音乐（`category: music`）。
     * @param musicOptions 可选，指定播放音乐使用的附加参数。
     * @throws This function can throw errors.
     *
     * {@link PropertyOutOfBoundsError}
     * @seeExample playMusicAndSound.ts
     */
    playMusic(trackId: string, musicOptions?: MusicOptions): void;
    /**
     * @remarks
     * 将音乐添加到播放列表。如果没有任何正在播放的音乐，将会开始播放音乐。播放列表中的音乐将会按照添加顺序播放（需要更多测试）。
     * 
     * Queues an additional music track for players. If a track is
     * not playing, a music track will play.
     *
     * @worldMutation
     *
     * @param trackId
     * 声音项目的标识符，要求声音项目的类别为音乐（`category: music`）。
     * 
     * Identifier of the music track to play.
     * @param musicOptions
     * 可选，指定播放音乐使用的附加参数。
     * 
     * Additional options for the music track.
     * @throws
     * An error will be thrown if volume is less than 0.0.
     * An error will be thrown if fade is less than 0.0.
     *
     *
     * {@link PropertyOutOfBoundsError}
     */
    queueMusic(trackId: string, musicOptions?: MusicOptions): void;
    /**
     * @remarks
     * 向所有玩家广播一条消息。
     * 
     * Sends a message to all players.
     *
     * @param message
     * 将要广播的一段消息。
     * 这段消息可能是一段字符串，或者符合 `RawMessage` 接口的对象，或是这两种类型的组合。
     * 
     * The message to be displayed.
     * @throws
     * 该方法在 `message` 格式不正确时会抛出错误。例如 `score` 的 `name` 为空字符串时。
     * 
     * This method can throw if the provided {@link RawMessage} is
     * in an invalid format. For example, if an empty `name` string
     * is provided to `score`.
     */
    sendMessage(message: (RawMessage | string)[] | RawMessage | string): void;
    /**
     * @remarks
     * Sets the world time.
     *
     * @worldMutation
     *
     * @param absoluteTime
     * The world time, in ticks.
     */
    setAbsoluteTime(absoluteTime: number): void;
    /**
     * @remarks
     * Sets a default spawn location for all players.
     *
     * @worldMutation
     *
     * @param spawnLocation
     * Location of the spawn point. Note that this is assumed to be
     * within the overworld dimension.
     * @throws
     * Throws if the provided spawn location is out of bounds.
     *
     * {@link Error}
     *
     * {@link LocationOutOfWorldBoundariesError}
     */
    setDefaultSpawnLocation(spawnLocation: Vector3): void;
    /**
     * @remarks
     * Sets the worlds difficulty.
     *
     * @worldMutation
     *
     * @param difficulty
     * The difficulty we want to set the world to.
     */
    setDifficulty(difficulty: Difficulty): void;
    /**
     * @remarks
     * Sets multiple dynamic properties with specific values.
     *
     * @param values
     * A Record of key value pairs of the dynamic properties to
     * set. If the data value is null, it will remove that property
     * instead.
     * @throws This function can throw errors.
     *
     * {@link ArgumentOutOfBoundsError}
     */
    setDynamicProperties(values: Record<string, boolean | number | string | Vector3 | undefined>): void;
    /**
     * @remarks
     * 为世界动态属性 `identifier` 设置一个值。
     * 
     * Sets a specified property to a value.
     *
     * @param identifier
     * 动态属性的标识符。
     * 
     * The property identifier.
     * @param value
     * 要设定的值，值的类型必须与动态属性注册的类型相同。若值为 null，该属性将被移除。
     * 
     * Data value of the property to set. If the value is null, it
     * will remove the property instead.
     * @throws
     * 若并未注册以 `identifier` 为标识符的动态属性，抛出 `"Dynamic Property '<identifier>' is not defined"`。
     * 
     * 若动态属性的类型不符合值的类型，抛出 `"Type mismatch for dynamic property '<identifier>'"`。
     * 
     * 若动态属性的类型为字符串，且值在使用 UTF-8 编码后的字节长度大于动态属性所允许的最大长度，抛出 `"Maximum string length exceeded (<length>/<maxLength>) for dynamic property '<identifier>'"`。
     * 
     * Throws if the given dynamic property identifier is not
     * defined.
     *
     * {@link ArgumentOutOfBoundsError}
     * @seeExample incrementDynamicProperty.ts
     * @seeExample incrementDynamicPropertyInJsonBlob.ts
     */
    setDynamicProperty(identifier: string, value?: boolean | number | string | Vector3): void;
    /**
     * @remarks
     * Sets the time of day.
     *
     * @worldMutation
     *
     * @param timeOfDay
     * The time of day, in ticks, between 0 and 24000.
     * @throws
     * Throws if the provided time of day is not within the valid
     * range.
     */
    setTimeOfDay(timeOfDay: number | TimeOfDay): void;
    /**
     * @remarks
     * 停止客户端中正在播放的所有音乐曲目（需要更多测试）。
     * 
     * Stops any music tracks from playing.
     *
     * @worldMutation
     *
     */
    stopMusic(): void;
}

/**
 * Contains a set of events that are available across the scope
 * of the World.
 */
export class WorldAfterEvents {
    private constructor();
    /**
     * @remarks
     * This event fires when a block container is closed.
     *
     * @earlyExecution
     *
     */
    readonly blockContainerClosed: BlockContainerClosedAfterEventSignal;
    /**
     * @remarks
     * This event fires when a block container is opened.
     *
     * @earlyExecution
     *
     */
    readonly blockContainerOpened: BlockContainerOpenedAfterEventSignal;
    /**
     * @remarks
     * This event fires for each BlockLocation destroyed by an
     * explosion. It is fired after the blocks have already been
     * destroyed.
     *
     * @earlyExecution
     *
     */
    readonly blockExplode: BlockExplodeAfterEventSignal;
    /**
     * @remarks
     * This event fires when a button is pushed.
     *
     * @earlyExecution
     *
     */
    readonly buttonPush: ButtonPushAfterEventSignal;
    /**
     * @beta
     * @remarks
     * This event is triggered after a chat message has been
     * broadcast or sent to players.
     *
     * @earlyExecution
     *
     */
    readonly chatSend: ChatSendAfterEventSignal;
    /**
     * @remarks
     * This event is fired when an entity event has been triggered
     * that will update the component definition state of an
     * entity.
     *
     * @earlyExecution
     *
     */
    readonly dataDrivenEntityTrigger: DataDrivenEntityTriggerAfterEventSignal;
    /**
     * @remarks
     * This event fires when an effect, like poisoning, is added to
     * an entity.
     *
     * @earlyExecution
     *
     */
    readonly effectAdd: EffectAddAfterEventSignal;
    /**
     * @remarks
     * This event fires when an entity container is closed.
     *
     * @earlyExecution
     *
     */
    readonly entityContainerClosed: EntityContainerClosedAfterEventSignal;
    /**
     * @remarks
     * This event fires when an entity container is opened.
     *
     * @earlyExecution
     *
     */
    readonly entityContainerOpened: EntityContainerOpenedAfterEventSignal;
    /**
     * @remarks
     * This event fires when an entity dies.
     *
     * @earlyExecution
     *
     */
    readonly entityDie: EntityDieAfterEventSignal;
    /**
     * @remarks
     * @earlyExecution
     *
     */
    readonly entityHeal: EntityHealAfterEventSignal;
    /**
     * @remarks
     * This event fires when entity health changes in any degree.
     *
     * @earlyExecution
     *
     */
    readonly entityHealthChanged: EntityHealthChangedAfterEventSignal;
    /**
     * @remarks
     * This event fires when an entity hits (that is, melee
     * attacks) a block.
     *
     * @earlyExecution
     *
     */
    readonly entityHitBlock: EntityHitBlockAfterEventSignal;
    /**
     * @remarks
     * This event fires when an entity hits (that is, melee
     * attacks) another entity.
     *
     * @earlyExecution
     *
     */
    readonly entityHitEntity: EntityHitEntityAfterEventSignal;
    /**
     * @remarks
     * This event fires when an entity is hurt (takes damage).
     *
     * @earlyExecution
     *
     */
    readonly entityHurt: EntityHurtAfterEventSignal;
    /**
     * @remarks
     * This event fires when an entity drops items.
     *
     * @earlyExecution
     *
     */
    readonly entityItemDrop: EntityItemDropAfterEventSignal;
    /**
     * @remarks
     * This event fires when an entity picks up items.
     *
     * @earlyExecution
     *
     */
    readonly entityItemPickup: EntityItemPickupAfterEventSignal;
    /**
     * @remarks
     * Fires when an entity is loaded.
     *
     * @earlyExecution
     *
     */
    readonly entityLoad: EntityLoadAfterEventSignal;
    /**
     * @remarks
     * Fires when an entity is removed (for example, potentially
     * unloaded, or removed after being killed).
     *
     * @earlyExecution
     *
     */
    readonly entityRemove: EntityRemoveAfterEventSignal;
    /**
     * @remarks
     * This event fires when an entity is spawned.
     *
     * @earlyExecution
     *
     */
    readonly entitySpawn: EntitySpawnAfterEventSignal;
    /**
     * @beta
     * @remarks
     * This event fires when an entity starts sneaking.
     *
     * @earlyExecution
     *
     */
    readonly entityStartSneaking: EntityStartSneakingAfterEventSignal;
    /**
     * @beta
     * @remarks
     * This event fires when an entity stops sneaking.
     *
     * @earlyExecution
     *
     */
    readonly entityStopSneaking: EntityStopSneakingAfterEventSignal;
    /**
     * @beta
     * @remarks
     * This event fires when an entity is tamed.
     *
     * @earlyExecution
     *
     */
    readonly entityTamed: EntityTamedAfterEventSignal;
    /**
     * @remarks
     * @earlyExecution
     *
     */
    readonly entityUpgrade: EntityUpgradeAfterEventSignal;
    /**
     * @remarks
     * This event is fired after an explosion occurs.
     *
     * @earlyExecution
     *
     */
    readonly explosion: ExplosionAfterEventSignal;
    /**
     * @remarks
     * This event fires when a world.gameRules property has
     * changed.
     *
     * @earlyExecution
     *
     */
    readonly gameRuleChange: GameRuleChangeAfterEventSignal;
    /**
     * @remarks
     * This event fires when a chargeable item completes charging.
     *
     * @earlyExecution
     *
     */
    readonly itemCompleteUse: ItemCompleteUseAfterEventSignal;
    /**
     * @remarks
     * This event fires when a chargeable item is released from
     * charging.
     *
     * @earlyExecution
     *
     */
    readonly itemReleaseUse: ItemReleaseUseAfterEventSignal;
    /**
     * @remarks
     * This event fires when a chargeable item starts charging.
     *
     * @earlyExecution
     *
     */
    readonly itemStartUse: ItemStartUseAfterEventSignal;
    /**
     * @remarks
     * This event fires when a player successfully uses an item or
     * places a block by pressing the Use Item / Place Block
     * button. If multiple blocks are placed, this event will only
     * occur once at the beginning of the block placement. Note:
     * This event cannot be used with Hoe or Axe items.
     *
     * @earlyExecution
     *
     */
    readonly itemStartUseOn: ItemStartUseOnAfterEventSignal;
    /**
     * @remarks
     * This event fires when a chargeable item stops charging.
     *
     * @earlyExecution
     *
     */
    readonly itemStopUse: ItemStopUseAfterEventSignal;
    /**
     * @remarks
     * This event fires when a player releases the Use Item / Place
     * Block button after successfully using an item. Note: This
     * event cannot be used with Hoe or Axe items.
     *
     * @earlyExecution
     *
     */
    readonly itemStopUseOn: ItemStopUseOnAfterEventSignal;
    /**
     * @remarks
     * This event fires when an item is successfully used by a
     * player.
     *
     * @earlyExecution
     *
     */
    readonly itemUse: ItemUseAfterEventSignal;
    /**
     * @remarks
     * A lever has been pulled.
     *
     * @earlyExecution
     *
     */
    readonly leverAction: LeverActionAfterEventSignal;
    /**
     * @beta
     * @remarks
     * This event is an internal implementation detail, and is
     * otherwise not currently functional.
     *
     * @earlyExecution
     *
     */
    readonly messageReceive: ServerMessageAfterEventSignal;
    /**
     * @beta
     * @remarks
     * This event is triggered when a pack setting is changed.
     *
     * @earlyExecution
     *
     */
    readonly packSettingChange: PackSettingChangeAfterEventSignal;
    /**
     * @remarks
     * This event fires when a piston expands or retracts.
     *
     * @earlyExecution
     *
     */
    readonly pistonActivate: PistonActivateAfterEventSignal;
    /**
     * @remarks
     * This event fires for a block that is broken by a player.
     *
     * @earlyExecution
     *
     */
    readonly playerBreakBlock: PlayerBreakBlockAfterEventSignal;
    /**
     * @remarks
     * This event fires when an {@link InputButton} state is
     * changed.
     *
     * @earlyExecution
     *
     */
    readonly playerButtonInput: PlayerButtonInputAfterEventSignal;
    /**
     * @rc
     * @remarks
     * This event fires when a player cancels breaking a block.
     *
     * @earlyExecution
     *
     */
    readonly playerCancelBreakingBlock: PlayerCancelBreakingBlockAfterEventSignal;
    /**
     * @remarks
     * Fires when a player moved to a different dimension.
     *
     * @earlyExecution
     *
     */
    readonly playerDimensionChange: PlayerDimensionChangeAfterEventSignal;
    /**
     * @remarks
     * @earlyExecution
     *
     */
    readonly playerEmote: PlayerEmoteAfterEventSignal;
    /**
     * @remarks
     * @earlyExecution
     *
     */
    readonly playerGameModeChange: PlayerGameModeChangeAfterEventSignal;
    /**
     * @remarks
     * This event fires when a player's selected slot changes.
     *
     * @earlyExecution
     *
     */
    readonly playerHotbarSelectedSlotChange: PlayerHotbarSelectedSlotChangeAfterEventSignal;
    /**
     * @remarks
     * This event fires when a player's {@link InputMode} changes.
     *
     * @earlyExecution
     *
     */
    readonly playerInputModeChange: PlayerInputModeChangeAfterEventSignal;
    /**
     * @remarks
     * This event fires when a players input permissions change.
     *
     * @earlyExecution
     *
     */
    readonly playerInputPermissionCategoryChange: PlayerInputPermissionCategoryChangeAfterEventSignal;
    /**
     * @remarks
     * An event for when a player interacts with a block.
     *
     * @earlyExecution
     *
     */
    readonly playerInteractWithBlock: PlayerInteractWithBlockAfterEventSignal;
    /**
     * @remarks
     * This event fires when a player interacts with an entity.
     *
     * @earlyExecution
     *
     */
    readonly playerInteractWithEntity: PlayerInteractWithEntityAfterEventSignal;
    /**
     * @remarks
     * This event fires when an item gets added or removed to the
     * player's inventory.
     *
     * @earlyExecution
     *
     */
    readonly playerInventoryItemChange: PlayerInventoryItemChangeAfterEventSignal;
    /**
     * @remarks
     * This event fires when a player joins a world.  See also
     * playerSpawn for another related event you can trap for when
     * a player is spawned the first time within a world.
     *
     * @earlyExecution
     *
     */
    readonly playerJoin: PlayerJoinAfterEventSignal;
    /**
     * @remarks
     * This event fires when a player leaves a world.
     *
     * @earlyExecution
     *
     */
    readonly playerLeave: PlayerLeaveAfterEventSignal;
    /**
     * @remarks
     * This event fires for a block that is placed by a player.
     *
     * @earlyExecution
     *
     */
    readonly playerPlaceBlock: PlayerPlaceBlockAfterEventSignal;
    /**
     * @remarks
     * This event fires when a player spawns or respawns. Note that
     * an additional flag within this event will tell you whether
     * the player is spawning right after join vs. a respawn.
     *
     * @earlyExecution
     *
     */
    readonly playerSpawn: PlayerSpawnAfterEventSignal;
    /**
     * @rc
     * @remarks
     * This event fires when a player starts breaking a block.
     *
     * @earlyExecution
     *
     */
    readonly playerStartBreakingBlock: PlayerStartBreakingBlockAfterEventSignal;
    /**
     * @remarks
     * @earlyExecution
     *
     */
    readonly playerSwingStart: PlayerSwingStartAfterEventSignal;
    /**
     * @beta
     * @remarks
     * An event for when a player uses a named name tag on an
     * entity.
     *
     * @earlyExecution
     *
     */
    readonly playerUseNameTag: PlayerUseNameTagAfterEventSignal;
    /**
     * @remarks
     * A pressure plate has popped back up (i.e., there are no
     * entities on the pressure plate.)
     *
     * @earlyExecution
     *
     */
    readonly pressurePlatePop: PressurePlatePopAfterEventSignal;
    /**
     * @remarks
     * A pressure plate has pushed (at least one entity has moved
     * onto a pressure plate.)
     *
     * @earlyExecution
     *
     */
    readonly pressurePlatePush: PressurePlatePushAfterEventSignal;
    /**
     * @remarks
     * This event fires when a projectile hits a block.
     *
     * @earlyExecution
     *
     */
    readonly projectileHitBlock: ProjectileHitBlockAfterEventSignal;
    /**
     * @remarks
     * This event fires when a projectile hits an entity.
     *
     * @earlyExecution
     *
     */
    readonly projectileHitEntity: ProjectileHitEntityAfterEventSignal;
    /**
     * @beta
     * @remarks
     * A tracked sound's declared duration elapsed.
     *
     * @earlyExecution
     *
     */
    readonly soundCompleted: SoundCompletedAfterEventSignal;
    /**
     * @remarks
     * A target block was hit.
     *
     * @earlyExecution
     *
     */
    readonly targetBlockHit: TargetBlockHitAfterEventSignal;
    /**
     * @remarks
     * A trip wire was tripped.
     *
     * @earlyExecution
     *
     */
    readonly tripWireTrip: TripWireTripAfterEventSignal;
    /**
     * @remarks
     * This event will be triggered when the weather changes within
     * Minecraft.
     *
     * @earlyExecution
     *
     */
    readonly weatherChange: WeatherChangeAfterEventSignal;
    /**
     * @remarks
     * @earlyExecution
     *
     */
    readonly worldLoad: WorldLoadAfterEventSignal;
}

/**
 * 表示一系列触发于实际动作发生之前的事件。通常来说，即将触发的事件可被修改或取消。
 * 但请注意，在 before 类事件过程中，能够改变游戏状态的 API 将失效，试图调用将会抛出错误。
 * （例如：dimension.spawnEntity）
 * 
 * A set of events that fire before an actual action occurs. In
 * most cases, you can potentially cancel or modify the
 * impending event. Note that in before events any APIs that
 * modify gameplay state will not function and will throw an
 * error. (e.g., dimension.spawnEntity)
 */
export class WorldBeforeEvents {
    private constructor();
    /**
     * @beta
     * @remarks
     * This event is triggered after a chat message has been
     * broadcast or sent to players.
     *
     * @earlyExecution
     *
     * @seeExample customCommand.ts
     */
    readonly chatSend: ChatSendBeforeEventSignal;
    /**
     * @remarks
     * This event is triggered after an event has been added to an
     * entity.
     *
     * @earlyExecution
     *
     */
    readonly effectAdd: EffectAddBeforeEventSignal;
    /**
     * @remarks
     * @earlyExecution
     *
     */
    readonly entityHeal: EntityHealBeforeEventSignal;
    /**
     * @remarks
     * @earlyExecution
     *
     */
    readonly entityHurt: EntityHurtBeforeEventSignal;
    /**
     * @remarks
     * This event fires before an entity picks up an item.
     *
     * @earlyExecution
     *
     */
    readonly entityItemPickup: EntityItemPickupBeforeEventSignal;
    /**
     * @remarks
     * Fires before an entity is removed from the world (for
     * example, unloaded or removed after being killed.)
     *
     * @earlyExecution
     *
     */
    readonly entityRemove: EntityRemoveBeforeEventSignal;
    /**
     * @beta
     * @remarks
     * Fires before an entity is tamed.
     *
     * @earlyExecution
     *
     */
    readonly entityTamed: EntityTamedBeforeEventSignal;
    /**
     * @remarks
     * This event is fired after an explosion occurs.
     *
     * @earlyExecution
     *
     */
    readonly explosion: ExplosionBeforeEventSignal;
    /**
     * @remarks
     * This event fires when an item is successfully used by a
     * player.
     *
     * @earlyExecution
     *
     */
    readonly itemUse: ItemUseBeforeEventSignal;
    /**
     * @remarks
     * This event fires before a block is broken by a player.
     *
     * @earlyExecution
     *
     */
    readonly playerBreakBlock: PlayerBreakBlockBeforeEventSignal;
    /**
     * @remarks
     * @earlyExecution
     *
     */
    readonly playerGameModeChange: PlayerGameModeChangeBeforeEventSignal;
    /**
     * @remarks
     * Fires before a player interacts with a block.
     *
     * @earlyExecution
     *
     */
    readonly playerInteractWithBlock: PlayerInteractWithBlockBeforeEventSignal;
    /**
     * @remarks
     * Fires before a player interacts with an entity.
     *
     * @earlyExecution
     *
     */
    readonly playerInteractWithEntity: PlayerInteractWithEntityBeforeEventSignal;
    /**
     * @remarks
     * Fires when a player leaves the game.
     *
     * @earlyExecution
     *
     */
    readonly playerLeave: PlayerLeaveBeforeEventSignal;
    /**
     * @beta
     * @remarks
     * This event fires before a block is placed by a player.
     *
     * @earlyExecution
     *
     */
    readonly playerPlaceBlock: PlayerPlaceBlockBeforeEventSignal;
    /**
     * @remarks
     * @earlyExecution
     *
     */
    readonly weatherChange: WeatherChangeBeforeEventSignal;
}

export class WorldLoadAfterEvent {
    private constructor();
}

export class WorldLoadAfterEventSignal {
    private constructor();
    /**
     * @remarks
     * @worldMutation
     *
     * @earlyExecution
     *
     */
    subscribe(callback: (arg0: WorldLoadAfterEvent) => void): (arg0: WorldLoadAfterEvent) => void;
    /**
     * @remarks
     * @worldMutation
     *
     * @earlyExecution
     *
     */
    unsubscribe(callback: (arg0: WorldLoadAfterEvent) => void): void;
}

/**
 * Axis-aligned bounding box.
 */
export interface AABB {
    /**
     * @remarks
     * The centerpoint of the box.
     *
     */
    center: Vector3;
    /**
     * @remarks
     * Absolute distance from the centerpoint to the bounds of the
     * box. Equivalent to half of the box's length, height and
     * width. Will always be treated as positive.
     *
     */
    extent: Vector3;
}

/**
 * Used to create camera animations.
 */
export interface AnimationOptions {
    /**
     * @remarks
     * Key frames for the camera animation.
     *
     */
    animation: SplineAnimation;
    /**
     * @remarks
     * Total time of the camera animation in seconds.
     *
     */
    totalTimeSeconds: number;
}

export interface BiomeFilter {
    excludeBiomes?: string[];
    excludeTags?: string[];
    includeBiomes?: string[];
    includeTags?: string[];
}

/**
 * @rc
 * Contains additional options for searches for the
 * dimension.findNearestBiome API.
 */
export interface BiomeSearchOptions {
    /**
     * @remarks
     * Bounding volume size to look within.
     *
     */
    boundingSize?: Vector3;
}

/**
 * A BlockBoundingBox is an interface to an object which
 * represents an AABB aligned rectangle.
 * The BlockBoundingBox assumes that it was created in a valid
 * state (min <= max) but cannot guarantee it (unless it was
 * created using the associated {@link BlockBoundingBoxUtils}
 * utility functions.
 * The min/max coordinates represent the diametrically opposite
 * corners of the rectangle.
 * The BlockBoundingBox is not a representation of blocks - it
 * has no association with any type, it is just a mathematical
 * construct - so a rectangle with
 * ( 0,0,0 ) -> ( 0,0,0 )
 * has a size of ( 0,0,0 ) (unlike the very similar {@link
 * BlockVolume} object)
 */
export interface BlockBoundingBox {
    /**
     * @remarks
     * A {@link Vector3} that represents the largest corner of the
     * rectangle
     *
     */
    max: Vector3;
    /**
     * @remarks
     * A {@link Vector3} that represents the smallest corner of the
     * rectangle
     *
     */
    min: Vector3;
}

/**
 * Options used to filter block container access events.
 */
export interface BlockContainerAccessEventOptions {
    /**
     * @remarks
     * If present will filter which container access sources can
     * trigger the event.
     *
     */
    accessSourceFilter?: ContainerAccessSourceFilter;
    /**
     * @remarks
     * If present will filter which container blocks can trigger
     * the event.
     *
     */
    blockFilter?: BlockFilter;
}

/**
 * Contains a set of events that will be raised for a block.
 * This object must be bound using the BlockRegistry.
 */
export interface BlockCustomComponent {
    /**
     * @remarks
     * This function will be called before a player places the
     * block.
     *
     */
    beforeOnPlayerPlace?: (arg0: BlockComponentPlayerPlaceBeforeEvent, arg1: CustomComponentParameters) => void;
    onBlockStateChange?: (arg0: BlockComponentBlockStateChangeEvent, arg1: CustomComponentParameters) => void;
    /**
     * @remarks
     * This function will be called when a specific block is
     * destroyed.
     * Changes in block permutations will not trigger this event.
     * Fill Command and SetBlock Command can trigger this event
     * when changing a block permutation only when using destroy
     * mode.
     * Custom blocks with the "minecraft:replaceable" component
     * will not trigger the event when replaced.
     *
     */
    onBreak?: (arg0: BlockComponentBlockBreakEvent, arg1: CustomComponentParameters) => void;
    /**
     * @remarks
     * This function will be called when an entity fires an event
     * to this block in the world.
     *
     */
    onEntity?: (arg0: BlockComponentEntityEvent, arg1: CustomComponentParameters) => void;
    /**
     * @remarks
     * This function will be called when an entity falls onto the
     * block that this custom component is bound to.
     *
     */
    onEntityFallOn?: (arg0: BlockComponentEntityFallOnEvent, arg1: CustomComponentParameters) => void;
    /**
     * @remarks
     * This function will be called when the block that this custom
     * component is bound to is placed.
     *
     */
    onPlace?: (arg0: BlockComponentOnPlaceEvent, arg1: CustomComponentParameters) => void;
    onPlayerBreak?: (arg0: BlockComponentPlayerBreakEvent, arg1: CustomComponentParameters) => void;
    /**
     * @remarks
     * This function will be called when a player sucessfully
     * interacts with the block that this custom component is bound
     * to.
     *
     */
    onPlayerInteract?: (arg0: BlockComponentPlayerInteractEvent, arg1: CustomComponentParameters) => void;
    /**
     * @remarks
     * This function will be called when a block randomly ticks.
     *
     */
    onRandomTick?: (arg0: BlockComponentRandomTickEvent, arg1: CustomComponentParameters) => void;
    /**
     * @remarks
     * This function will be called when an 'onRedstoneUpdate'
     * engine event occurs if the block has a
     * `minecraft:redstone_consumer` component and the redstone
     * signal strength is >= to the components `min_power` field.
     *
     */
    onRedstoneUpdate?: (arg0: BlockComponentRedstoneUpdateEvent, arg1: CustomComponentParameters) => void;
    /**
     * @remarks
     * This function will be called when an entity steps off the
     * block that this custom component is bound to.
     *
     */
    onStepOff?: (arg0: BlockComponentStepOffEvent, arg1: CustomComponentParameters) => void;
    /**
     * @remarks
     * This function will be called when an entity steps onto the
     * block that this custom component is bound to.
     *
     */
    onStepOn?: (arg0: BlockComponentStepOnEvent, arg1: CustomComponentParameters) => void;
    /**
     * @remarks
     * This function will be called when a block ticks.
     *
     */
    onTick?: (arg0: BlockComponentTickEvent, arg1: CustomComponentParameters) => void;
}

/**
 * Contains optional parameters for registering a block event.
 */
export interface BlockEventOptions {
    /**
     * @remarks
     * If this value is set, this event will only fire if the
     * impacted block's type matches this parameter.
     *
     */
    blockTypes?: string[];
    /**
     * @remarks
     * If this value is set, this event will only fire if the
     * impacted block's permutation matches this parameter.
     *
     */
    permutations?: BlockPermutation[];
}

/**
 * Contains additional options for a block fill operation.
 */
export interface BlockFillOptions {
    /**
     * @remarks
     * When specified, the fill operation will include / exclude
     * the blocks added to the block filter.
     *
     */
    blockFilter?: BlockFilter;
    /**
     * @remarks
     * When true fillBlocks will not error if part of the fill
     * volume is outside of loaded chunks bounds. Instead it will
     * just fill the blocks that are inside the loaded chunk bounds
     * and ignoring blocks outside.
     *
     */
    ignoreChunkBoundErrors?: boolean;
}

/**
 * Options to include or exclude blocks based on type, tag or
 * permutation. If no include options are added it will select
 * all blocks that are not rejected by the exclude options. If
 * at least one include option is added the block must match
 * one of the include options to not be rejected.
 */
export interface BlockFilter {
    /**
     * @remarks
     * Array of block permutations that the filter should reject if
     * any matches.
     *
     */
    excludePermutations?: BlockPermutation[];
    /**
     * @remarks
     * Array of block tags that the filter should reject if any
     * matches.
     *
     */
    excludeTags?: string[];
    /**
     * @remarks
     * Array of block types that the filter should reject if any
     * matches.
     *
     */
    excludeTypes?: string[];
    /**
     * @remarks
     * Array of block permutations that the filter should select if
     * at least one matches.
     *
     */
    includePermutations?: BlockPermutation[];
    /**
     * @remarks
     * Array of block tags that the filter should select if at
     * least one matches.
     *
     */
    includeTags?: string[];
    /**
     * @remarks
     * Array of block types that the filter should select if at
     * least one matches.
     *
     */
    includeTypes?: string[];
}

/**
 * Contains more information for events where a block is hit.
 */
export interface BlockHitInformation {
    /**
     * @remarks
     * Block that was hit.
     *
     */
    block: Block;
    /**
     * @remarks
     * Face of the block that was hit.
     *
     */
    face: Direction;
    /**
     * @remarks
     * Location relative to the bottom north-west corner of the
     * block.
     *
     */
    faceLocation: Vector3;
}

/**
 * @beta
 * Options for querying blocks in a volume. Extends BlockFilter
 * with additional sorting and limiting options based on
 * distance from a location.
 */
// @ts-ignore Class inheritance allowed for native defined classes
export interface BlockQueryOptions extends BlockFilter {
    /**
     * @remarks
     * If specified, returns the closest N blocks to the location.
     * Must be greater than 0. Cannot be used with farthest.
     * Requires location to be set.
     *
     */
    closest?: number;
    /**
     * @remarks
     * If specified, returns the farthest N blocks from the
     * location. Must be greater than 0. Cannot be used with
     * closest. Requires location to be set.
     *
     */
    farthest?: number;
    /**
     * @remarks
     * Location used as the reference point for closest or farthest
     * distance calculations. Required when closest or farthest is
     * specified.
     *
     */
    location?: Vector3;
}

/**
 * Contains information for block raycast hit results.
 */
export interface BlockRaycastHit {
    /**
     * @remarks
     * Block that was hit.
     *
     */
    block: Block;
    /**
     * @remarks
     * Face of the block that was hit.
     *
     */
    face: Direction;
    /**
     * @remarks
     * Hit location relative to the bottom north-west corner of the
     * block.
     *
     */
    faceLocation: Vector3;
}

/**
 * Contains additional options for configuring a block raycast
 * query.
 */
// @ts-ignore Class inheritance allowed for native defined classes
export interface BlockRaycastOptions extends BlockFilter {
    /**
     * @remarks
     * If true, liquid blocks will be considered as blocks that
     * 'stop' the raycast.
     *
     */
    includeLiquidBlocks?: boolean;
    /**
     * @remarks
     * If true, passable blocks like vines and flowers will be
     * considered as blocks that 'stop' the raycast.
     *
     */
    includePassableBlocks?: boolean;
    /**
     * @remarks
     * Maximum distance, in blocks, to process the raycast.
     *
     */
    maxDistance?: number;
}

/**
 * Used to attach the camera to a non player entity.
 */
export interface CameraAttachOptions {
    /**
     * @remarks
     * Set a non player entity for the camera to target.
     *
     */
    entity: Entity;
    /**
     * @remarks
     * The location of the entity that you want to target (eg.
     * head, feet, eyes).
     *
     */
    locator: EntityAttachPoint;
}

/**
 * Used to initiate a full-screen color fade.
 */
export interface CameraFadeOptions {
    /**
     * @remarks
     * Fade color to use.
     *
     */
    fadeColor?: RGB;
    /**
     * @remarks
     * Time in seconds for the fade-in, hold, and fade-out seconds.
     *
     */
    fadeTime?: CameraFadeTimeOptions;
}

/**
 * Contains timings for a fade transition.
 */
export interface CameraFadeTimeOptions {
    /**
     * @remarks
     * Time, in seconds, for a fade-in.
     *
     */
    fadeInTime: number;
    /**
     * @remarks
     * Time, in seconds, for a fade-out.
     *
     */
    fadeOutTime: number;
    /**
     * @remarks
     * Time, in seconds, to hold the full screen color.
     *
     */
    holdTime: number;
}

/**
 * Options to control pivot points and offsets of the third
 * person boom preset.
 */
export interface CameraFixedBoomOptions {
    /**
     * @remarks
     * Changes the pivot point to be <x, y, z> away from the
     * player.
     *
     */
    entityOffset?: Vector3;
    /**
     * @remarks
     * Offsets the camera from center by <x, y>.
     *
     */
    viewOffset?: Vector2;
}

/**
 * Used to change the field of view of the current camera.
 */
export interface CameraFovOptions {
    easeOptions?: EaseOptions;
    /**
     * @remarks
     * Set a value for the field of view.
     *
     */
    fov?: number;
}

export interface CameraSetFacingOptions {
    easeOptions?: EaseOptions;
    facingEntity: Entity;
    location?: Vector3;
}

export interface CameraSetLocationOptions {
    easeOptions?: EaseOptions;
    location: Vector3;
}

export interface CameraSetPosOptions {
    easeOptions?: EaseOptions;
    facingLocation: Vector3;
    location?: Vector3;
}

export interface CameraSetRotOptions {
    easeOptions?: EaseOptions;
    location?: Vector3;
    rotation: Vector2;
}

/**
 * @beta
 * Options for applying a camera shake effect to a player's
 * camera via `Camera.addShake`. Each call to `addShake` queues
 * a new independent shake event for the specified `type`;
 * positional and rotational shakes are tracked in separate
 * queues and run concurrently. The rendered intensity at any
 * moment is the sum of all active events' intensities for that
 * type, capped at `4.0`. Events expire naturally when their
 * `duration` elapses.
 */
export interface CameraShakeOptions {
    /**
     * @remarks
     * How long this shake event lasts, in seconds. Must be a
     * positive value.
     *
     */
    duration: number;
    /**
     * @remarks
     * The intensity of this shake event. Must be a positive value
     * with a maximum of `4.0`. Multiple active events of the same
     * `type` are summed, capped at `4.0`.
     *
     */
    intensity: number;
    /**
     * @remarks
     * The type of camera shake to apply. Positional and rotational
     * shakes maintain separate event queues and are applied
     * concurrently, so adding a shake of each type does not cause
     * them to interfere with one another.
     *
     */
    type: CameraShakeType;
}

/**
 * Used to target an entity with a free camera.
 */
export interface CameraTargetOptions {
    /**
     * @remarks
     * Set an <x, y, z> offset from the target entity's center.
     *
     */
    offsetFromTargetCenter?: Vector3;
    /**
     * @remarks
     * The singular entity you want to target.
     *
     */
    targetEntity: Entity;
}

/**
 * @beta
 * This interface defines an entry into the {@link
 * CompoundBlockVolume} which represents a volume of positive
 * or negative space.
 *
 */
export interface CompoundBlockVolumeItem {
    /**
     * @remarks
     * The 'action' defines how the block volume is represented in
     * the compound block volume stack.
     * 'Add' creates a block volume which is positively selected
     * 'Subtract' creates a block volume which represents a hole or
     * negative space in the overall compound block volume.
     *
     */
    action?: CompoundBlockVolumeAction;
    /**
     * @remarks
     * The relativity enumeration determines whether the
     * BlockVolume specified is positioned relative to the parent
     * compound block volume origin, or in absolute world space.
     *
     */
    locationRelativity?: CompoundBlockVolumePositionRelativity;
    /**
     * @remarks
     * The volume of space
     *
     */
    volume: BlockVolume;
}

/**
 * Represents the source of a container access.
 */
export interface ContainerAccessSource {
    /**
     * @remarks
     * The entity that triggered the container access.
     *
     */
    entity?: Entity;
}

/**
 * Options for use when filtering container access sources.
 */
export interface ContainerAccessSourceFilter {
    /**
     * @remarks
     * Filter options for the source entity accessing the
     * container.
     *
     */
    entityFilter?: EntityFilter;
}

/**
 * Rules that if broken on container operations will throw an
 * error.
 */
export interface ContainerRules {
    /**
     * @remarks
     * Defines the items that are exclusively allowed in the
     * container. If empty all items are allowed in the container.
     *
     */
    allowedItems: string[];
    /**
     * @remarks
     * Determines whether other storage items can be placed into
     * the container.
     *
     */
    allowNestedStorageItems: boolean;
    /**
     * @remarks
     * Defines the items that are not allowed in the container.
     *
     */
    bannedItems: string[];
    /**
     * @remarks
     * Defines the maximum allowed total weight of all items in the
     * storage item container. If undefined container has no weight
     * limit.
     *
     */
    weightLimit?: number;
}

/**
 * Define the custom command, including name, permissions, and
 * parameters.
 */
export interface CustomCommand {
    /**
     * @remarks
     * Cheats must be enabled to run this command. Defaults to
     * true.
     *
     */
    cheatsRequired?: boolean;
    /**
     * @remarks
     * Command description as seen on the command line.
     *
     */
    description: string;
    /**
     * @remarks
     * List of mandatory command parameters.
     *
     */
    mandatoryParameters?: CustomCommandParameter[];
    /**
     * @remarks
     * The name of the command. A namespace is required.
     *
     */
    name: string;
    /**
     * @remarks
     * List of optional command parameters.
     *
     */
    optionalParameters?: CustomCommandParameter[];
    /**
     * @remarks
     * The permission level required to execute the command.
     *
     */
    permissionLevel: CommandPermissionLevel;
}

/**
 * Definition for each parameter expected by the custom
 * command.
 */
export interface CustomCommandParameter {
    /**
     * @beta
     * @remarks
     * Can be used to reference the enum name when {@link
     * CustomCommandParamType} is 'Enum'. Allows the parameter name
     * to be different from the enum name.
     *
     */
    enumName?: string;
    /**
     * @remarks
     * The name of parameter as it appears on the command line.
     *
     */
    name: string;
    /**
     * @remarks
     * The data type of the parameter.
     *
     */
    type: CustomCommandParamType;
}

/**
 * Interface returned from custom command callback function.
 */
export interface CustomCommandResult {
    /**
     * @remarks
     * Message displayed to chat after command execution.
     *
     */
    message?: string;
    /**
     * @remarks
     * Command execution Success or Failure. Determines how the
     * status message is displayed.
     *
     */
    status: CustomCommandStatus;
}

export interface CustomTexture {
    /**
     * @remarks
     * The height of the icon, in relative units. Value must be
     * between 0.0 and 1.0, inclusive.
     *
     * Bounds: [0, 1]
     */
    iconHeight: number;
    /**
     * @remarks
     * The width of the icon, in relative units. Value must be
     * between 0.0 and 1.0, inclusive.
     *
     * Bounds: [0, 1]
     */
    iconWidth: number;
    /**
     * @remarks
     * The resource path to the custom texture. This should be a
     * valid string path to a texture asset.
     *
     */
    path: string;
}

/**
 * Contains a set of updates to the component definition state
 * of an entity.
 */
export interface DefinitionModifier {
    /**
     * @remarks
     * Retrieves the list of component groups that will be added
     * via this definition modification.
     *
     */
    addedComponentGroups: string[];
    /**
     * @remarks
     * The list of component groups that will be removed via this
     * definition modification.
     *
     */
    removedComponentGroups: string[];
    /**
     * @beta
     * @remarks
     * The list of entity definition events that will be fired via
     * this update.
     *
     */
    triggers: Trigger[];
}

/**
 * An exact coordinate within the world, including its
 * dimension and location.
 */
export interface DimensionLocation {
    /**
     * @remarks
     * Dimension that this coordinate is associated with.
     *
     */
    dimension: Dimension;
    /**
     * @remarks
     * X component of this dimension-location.
     *
     */
    x: number;
    /**
     * @remarks
     * Y component of this dimension-location.
     *
     */
    y: number;
    /**
     * @remarks
     * Z component of this dimension-location.
     *
     */
    z: number;
}

/**
 * Contains options associated with easing between positions
 * and/or rotations.
 */
export interface EaseOptions {
    /**
     * @remarks
     * Time for the ease operation.
     *
     */
    easeTime?: number;
    /**
     * @remarks
     * Type of ease operation to use.
     *
     */
    easeType?: EasingType;
}

/**
 * This interface represents a specific leveled enchantment
 * that is applied to an item.
 */
export interface Enchantment {
    /**
     * @remarks
     * The level of this enchantment instance.
     *
     */
    level: number;
    /**
     * @remarks
     * The enchantment type of this instance.
     *
     */
    type: EnchantmentType;
}

/**
 * Additional options for when damage has been applied via a
 * projectile.
 */
export interface EntityApplyDamageByProjectileOptions {
    /**
     * @remarks
     * Optional entity that fired the projectile.
     *
     */
    damagingEntity?: Entity;
    /**
     * @remarks
     * Projectile that caused damage.
     *
     */
    damagingProjectile: Entity;
}

/**
 * Additional descriptions and metadata for a damage event.
 */
export interface EntityApplyDamageOptions {
    /**
     * @remarks
     * Underlying cause of the damage.
     *
     */
    cause: EntityDamageCause;
    /**
     * @remarks
     * Optional entity that caused the damage.
     *
     */
    damagingEntity?: Entity;
}

/**
 * Options used to filter entity container access events.
 */
export interface EntityContainerAccessEventOptions {
    /**
     * @remarks
     * If present will filter which container access sources can
     * trigger the event.
     *
     */
    accessSourceFilter?: ContainerAccessSourceFilter;
    /**
     * @remarks
     * If present will filter which entity containers can trigger
     * the event.
     *
     */
    entityFilter?: EntityFilter;
}

/**
 * Provides information about how damage has been applied to an
 * entity.
 */
export interface EntityDamageSource {
    /**
     * @remarks
     * Cause enumeration of damage.
     *
     */
    cause: EntityDamageCause;
    /**
     * @remarks
     * Optional entity that caused the damage.
     *
     */
    damagingEntity?: Entity;
    /**
     * @remarks
     * Optional projectile that may have caused damage.
     *
     */
    damagingProjectile?: Entity;
}

/**
 * Specifies additional filters that are used in registering a
 * data driven trigger event for entities.
 */
export interface EntityDataDrivenTriggerEventOptions {
    /**
     * @remarks
     * If this value is set, this event will only fire for entities
     * that match the entities within this collection.
     *
     */
    entities?: Entity[];
    /**
     * @remarks
     * If this value is set, this event will only fire if the
     * impacted entities' type matches this parameter.
     *
     */
    entityTypes?: string[];
    /**
     * @remarks
     * If this value is set, this event will only fire if the
     * impacted triggered event matches one of the events listed in
     * this parameter.
     *
     */
    eventTypes?: string[];
}

/**
 * Contains additional options for entity effects.
 */
export interface EntityEffectOptions {
    /**
     * @remarks
     * The strength of the effect.
     *
     */
    amplifier?: number;
    /**
     * @remarks
     * If true, will show particles when effect is on the entity.
     *
     */
    showParticles?: boolean;
}

/**
 * Contains optional parameters for registering an entity
 * event.
 */
export interface EntityEventOptions {
    /**
     * @remarks
     * If this value is set, this event will only fire for entities
     * that match the entities within this collection.
     *
     */
    entities?: Entity[];
    /**
     * @remarks
     * If this value is set, this event will only fire if the
     * impacted entities' type matches this parameter.
     *
     */
    entityTypes?: string[];
}

/**
 * Contains options for filtering entities.
 */
export interface EntityFilter {
    /**
     * @remarks
     * Excludes entities that match one or more of the specified
     * families.
     *
     */
    excludeFamilies?: string[];
    /**
     * @remarks
     * Excludes entities if have a specific gamemode that matches
     * the specified gamemode.
     *
     */
    excludeGameModes?: GameMode[];
    /**
     * @remarks
     * Excludes entities that have a name that match one of the
     * specified values.
     *
     */
    excludeNames?: string[];
    /**
     * @remarks
     * Excludes entities with a tag that matches one of the
     * specified values.
     *
     */
    excludeTags?: string[];
    /**
     * @remarks
     * Excludes entities if they are one of the specified types.
     *
     */
    excludeTypes?: string[];
    /**
     * @remarks
     * If specified, includes entities that match all of the
     * specified families.
     *
     */
    families?: string[];
    /**
     * @remarks
     * If specified, includes entities with a gamemode that matches
     * the specified gamemode.
     *
     */
    gameMode?: GameMode;
    /**
     * @remarks
     * If specified, will only include entities that have at most
     * this horizontal rotation.
     *
     */
    maxHorizontalRotation?: number;
    /**
     * @remarks
     * If defined, only players that have at most this level are
     * returned.
     *
     */
    maxLevel?: number;
    /**
     * @remarks
     * If specified, only entities that have at most this vertical
     * rotation are returned.
     *
     */
    maxVerticalRotation?: number;
    /**
     * @remarks
     * If specified, will only include entities that have at a
     * minimum this horizontal rotation.
     *
     */
    minHorizontalRotation?: number;
    /**
     * @remarks
     * If defined, only players that have at least this level are
     * returned.
     *
     */
    minLevel?: number;
    /**
     * @remarks
     * If specified, will only include entities that have at least
     * this vertical rotation.
     *
     */
    minVerticalRotation?: number;
    /**
     * @remarks
     * Includes entities with the specified name.
     *
     */
    name?: string;
    propertyOptions?: EntityQueryPropertyOptions[];
    /**
     * @remarks
     * Gets/sets a collection of EntityQueryScoreOptions objects
     * with filters for specific scoreboard objectives.
     *
     */
    scoreOptions?: EntityQueryScoreOptions[];
    /**
     * @remarks
     * Includes entities that match all of the specified tags.
     *
     */
    tags?: string[];
    /**
     * @remarks
     * If defined, entities that match this type are included.
     *
     */
    type?: string;
}

/**
 * Contains optional parameters for registering an entity heal
 * event.
 */
export interface EntityHealEventOptions {
    /**
     * @remarks
     * If this value is set, this event will only fire for healing
     * causes that match.
     *
     */
    allowedHealCauses?: EntityHealCause[];
    /**
     * @remarks
     * If this value is set, this event will only fire for entities
     * that match.
     *
     */
    entityFilter?: EntityFilter;
}

/**
 * Contains additional information about an entity that was
 * hit.
 */
export interface EntityHitInformation {
    /**
     * @remarks
     * Entity that was hit.
     *
     */
    entity?: Entity;
}

/**
 * Contains optional parameters for registering an entity hurt
 * after event.
 */
export interface EntityHurtAfterEventOptions {
    /**
     * @remarks
     * If this value is set, this event will only fire for damage
     * causes that match.
     *
     */
    allowedDamageCauses?: EntityDamageCause[];
    /**
     * @remarks
     * If this value is set, this event will only fire for entities
     * that match the entities within this collection.
     *
     */
    entities?: Entity[];
    /**
     * @remarks
     * If this value is set, this event will only fire for entities
     * that match.
     *
     */
    entityFilter?: EntityFilter;
    /**
     * @remarks
     * If this value is set, this event will only fire if the
     * impacted entities' type matches this parameter.
     *
     */
    entityTypes?: string[];
}

/**
 * Contains optional parameters for registering an entity hurt
 * before event.
 */
export interface EntityHurtBeforeEventOptions {
    /**
     * @remarks
     * If this value is set, this event will only fire for damage
     * causes that match.
     *
     */
    allowedDamageCauses?: EntityDamageCause[];
    /**
     * @remarks
     * If this value is set, this event will only fire for entities
     * that match.
     *
     */
    entityFilter?: EntityFilter;
}

/**
 * An interface that is passed into {@link
 * EntityItemDropAfterEventSignal.subscribe} that filters out
 * which events are passed to the provided callback.
 */
export interface EntityItemDropEventOptions {
    /**
     * @remarks
     * If this value is set, this event will only fire for entities
     * that match.
     *
     */
    entityFilter?: EntityFilter;
    /**
     * @remarks
     * If this value is set, this event will only fire if an item
     * in the event matches.
     *
     */
    itemFilter?: ItemFilter;
}

/**
 * An interface that is passed into {@link
 * EntityItemPickupAfterEventSignal.subscribe} and {@link
 * EntityItemPickupBeforeEventSignal.subscribe} that filters
 * out which events are passed to the provided callback.
 */
export interface EntityItemPickupEventOptions {
    /**
     * @remarks
     * If this value is set, this event will only fire for entities
     * that match.
     *
     */
    entityFilter?: EntityFilter;
    /**
     * @remarks
     * If this value is set, this event will only fire if an item
     * in the event matches.
     *
     */
    itemFilter?: ItemFilter;
}

/**
 * Contains options for selecting entities within an area.
 * @seeExample blockConditional.ts
 * @seeExample findEntitiesHavingPropertyEqualsTo.ts
 * @seeExample playSoundChained.ts
 * @seeExample setScoreboardChained.ts
 * @seeExample summonMobChained.ts
 * @seeExample bounceSkeletons.ts
 * @seeExample tagsQuery.ts
 * @seeExample testThatEntityIsFeatherItem.ts
 */
// @ts-ignore Class inheritance allowed for native defined classes
export interface EntityQueryOptions extends EntityFilter {
    /**
     * @remarks
     * Limits the number of entities to return, opting for the
     * closest N entities as specified by this property. The
     * location value must also be specified on the query options
     * object.
     *
     */
    closest?: number;
    /**
     * @remarks
     * Limits the number of entities to return, opting for the
     * farthest N entities as specified by this property. The
     * location value must also be specified on the query options
     * object.
     *
     */
    farthest?: number;
    /**
     * @remarks
     * Adds a seed location to the query that is used in
     * conjunction with closest, farthest, limit, volume, and
     * distance properties.
     *
     */
    location?: Vector3;
    /**
     * @remarks
     * If specified, includes entities that are less than this
     * distance away from the location specified in the location
     * property.
     *
     */
    maxDistance?: number;
    /**
     * @remarks
     * If specified, includes entities that are least this distance
     * away from the location specified in the location property.
     *
     */
    minDistance?: number;
    /**
     * @remarks
     * In conjunction with location, specified a cuboid volume of
     * entities to include.
     *
     */
    volume?: Vector3;
}

export interface EntityQueryPropertyOptions {
    exclude?: boolean;
    propertyId: string;
    value?:
        | boolean
        | string
        | EqualsComparison
        | GreaterThanComparison
        | GreaterThanOrEqualsComparison
        | LessThanComparison
        | LessThanOrEqualsComparison
        | NotEqualsComparison
        | RangeComparison;
}

/**
 * Contains additional options for filtering players based on
 * their score for an objective.
 */
export interface EntityQueryScoreOptions {
    /**
     * @remarks
     * If set to true, entities and players within this score range
     * are excluded from query results.
     *
     */
    exclude?: boolean;
    /**
     * @remarks
     * If defined, only players that have a score equal to or under
     * maxScore are included.
     *
     */
    maxScore?: number;
    /**
     * @remarks
     * If defined, only players that have a score equal to or over
     * minScore are included.
     *
     */
    minScore?: number;
    /**
     * @remarks
     * Identifier of the scoreboard objective to filter on.
     *
     */
    objective?: string;
}

/**
 * Contains information for entity raycast hit results.
 */
export interface EntityRaycastHit {
    /**
     * @remarks
     * Distance from ray origin to entity bounds.
     *
     */
    distance: number;
    /**
     * @remarks
     * Entity that was hit.
     *
     */
    entity: Entity;
}

/**
 * Contains additional options for an entity raycast operation.
 */
// @ts-ignore Class inheritance allowed for native defined classes
export interface EntityRaycastOptions extends EntityFilter {
    /**
     * @remarks
     * If true, blocks will not be considered as blocks that 'stop'
     * the raycast.
     *
     */
    ignoreBlockCollision?: boolean;
    /**
     * @remarks
     * If true, liquid blocks will be considered as blocks that
     * 'stop' the raycast.
     *
     */
    includeLiquidBlocks?: boolean;
    /**
     * @remarks
     * If true, passable blocks like vines and flowers will be
     * considered as blocks that 'stop' the raycast.
     *
     */
    includePassableBlocks?: boolean;
    /**
     * @remarks
     * Maximum distance, in blocks, to process the raycast.
     *
     */
    maxDistance?: number;
}

/**
 * @beta
 * Options used to filter entity start sneaking and stop
 * sneaking events.
 */
export interface EntitySneakingChangedEventOptions {
    entityFilter?: EntityFilter;
}

/**
 * @beta
 * Contains options for filtering entity tamed events.
 */
export interface EntityTamedEventFilter {
    entityFilter?: EntityFilter;
    tamingEntityFilter?: EntityFilter;
}

/**
 * Controls when a waypoint is visible based on the state of
 * the entity it tracks. These rules allow filtering waypoint
 * visibility by entity conditions like sneaking, invisibility,
 * and death state.
 */
export interface EntityVisibilityRules {
    /**
     * @remarks
     * Controls whether the waypoint is shown when the tracked
     * entity is dead. If undefined, defaults to true.
     *
     */
    showDead?: boolean;
    /**
     * @remarks
     * Controls whether the waypoint is shown when the tracked
     * entity is invisible. If undefined, defaults to true.
     *
     */
    showInvisible?: boolean;
    /**
     * @remarks
     * Controls whether the waypoint is shown when the tracked
     * entity is sneaking. If undefined, defaults to true.
     *
     */
    showSneaking?: boolean;
}

/**
 * Equal to operator.
 */
export interface EqualsComparison {
    /**
     * @remarks
     * Threshold value compared against.
     *
     */
    equals: boolean | number | string;
}

/**
 * Additional configuration options for the {@link
 * Dimension.createExplosion} method.
 * @seeExample createNoBlockExplosion.ts
 * @seeExample createExplosions.ts
 */
export interface ExplosionOptions {
    /**
     * @remarks
     * Whether parts of the explosion also impact underwater.
     *
     */
    allowUnderwater?: boolean;
    /**
     * @remarks
     * Whether the explosion will break blocks within the blast
     * radius.
     *
     */
    breaksBlocks?: boolean;
    /**
     * @remarks
     * If true, the explosion is accompanied by fires within or
     * near the blast radius.
     *
     */
    causesFire?: boolean;
    /**
     * @remarks
     * Optional source of the explosion.
     *
     */
    source?: Entity;
}

/**
 * Contains additional options for getBlockStandingOn and
 * getAllBlocksStandingOn.
 */
export interface GetBlocksStandingOnOptions {
    /**
     * @remarks
     * When specified, the function will include / exclude what
     * block(s) are returned based on the block filter.
     *
     */
    blockFilter?: BlockFilter;
    /**
     * @remarks
     * If true, all blocks of height 0.2 or lower like trapdoors
     * and carpets will be ignored, and the block underneath will
     * be returned.
     *
     */
    ignoreThinBlocks?: boolean;
}

/**
 * Greater than operator.
 */
export interface GreaterThanComparison {
    /**
     * @remarks
     * Threshold value compared against.
     *
     */
    greaterThan: number;
}

/**
 * Greater than or equal to operator.
 */
export interface GreaterThanOrEqualsComparison {
    /**
     * @remarks
     * Threshold value compared against.
     *
     */
    greaterThanOrEquals: number;
}

/**
 * Contains additional filtering options for hotbar events.
 */
export interface HotbarEventOptions {
    /**
     * @remarks
     * The slot indexes to consider. Values should be between 0 and
     * 8, inclusive. If not specified, all slots are considered.
     *
     * Bounds: [0, 8]
     */
    allowedSlots?: number[];
}

/**
 * An interface that is passed into {@link
 * PlayerButtonInputAfterEventSignal.subscribe} that filters
 * out which events are passed to the provided callback.
 */
export interface InputEventOptions {
    /**
     * @remarks
     * The buttons the callback should be called for. If undefined,
     * the callback will be called for all buttons.
     *
     */
    buttons?: InputButton[];
    /**
     * @remarks
     * The state the callback should be called for. If undefined,
     * the callback will be called for all button states.
     *
     */
    state?: ButtonState;
}

/**
 * Contains additional filtering options for inventory item
 * events.
 */
export interface InventoryItemEventOptions {
    /**
     * @remarks
     * The slot indexes to consider. Values should be positive
     * numbers. If not specified, all slots are considered.
     *
     * Bounds: [0, 1000]
     */
    allowedSlots?: number[];
    /**
     * @remarks
     * The names for the items to exclude.
     *
     */
    excludeItems?: string[];
    /**
     * @remarks
     * The item tags to exclude.
     *
     */
    excludeTags?: string[];
    /**
     * @remarks
     * Flag to specify to ignore quantity changes only. True to
     * ignore quantity changes, false to not ignore quantity
     * changes.
     *
     */
    ignoreQuantityChange?: boolean;
    /**
     * @remarks
     * The item names to consider.
     *
     */
    includeItems?: string[];
    /**
     * @remarks
     * The item tags to consider.
     *
     */
    includeTags?: string[];
    /**
     * @remarks
     * The player inventory type to consider.
     *
     */
    inventoryType?: PlayerInventoryType;
}

/**
 * Contains a set of events that will be raised for an item.
 * This object must be bound using the ItemComponentRegistry.
 */
export interface ItemCustomComponent {
    /**
     * @remarks
     * This function will be called when an item containing this
     * component is hitting an entity and about to take durability
     * damage.
     *
     */
    onBeforeDurabilityDamage?: (
        arg0: ItemComponentBeforeDurabilityDamageEvent,
        arg1: CustomComponentParameters,
    ) => void;
    /**
     * @remarks
     * This function will be called when an item containing this
     * component's use duration was completed.
     *
     */
    onCompleteUse?: (arg0: ItemComponentCompleteUseEvent, arg1: CustomComponentParameters) => void;
    /**
     * @remarks
     * This function will be called when an item containing this
     * component is eaten by an entity.
     *
     */
    onConsume?: (arg0: ItemComponentConsumeEvent, arg1: CustomComponentParameters) => void;
    /**
     * @remarks
     * This function will be called when an item containing this
     * component is used to hit another entity.
     *
     */
    onHitEntity?: (arg0: ItemComponentHitEntityEvent, arg1: CustomComponentParameters) => void;
    /**
     * @remarks
     * This function will be called when an item containing this
     * component is used to mine a block.
     *
     */
    onMineBlock?: (arg0: ItemComponentMineBlockEvent, arg1: CustomComponentParameters) => void;
    /**
     * @remarks
     * This function will be called when an item containing this
     * component is used by a player.
     *
     */
    onUse?: (arg0: ItemComponentUseEvent, arg1: CustomComponentParameters) => void;
    /**
     * @remarks
     * This function will be called when an item containing this
     * component is used on a block.
     *
     */
    onUseOn?: (arg0: ItemComponentUseOnEvent, arg1: CustomComponentParameters) => void;
}

/**
 * Contains options for filtering items.
 */
export interface ItemFilter {
    /**
     * @remarks
     * If defined, items that match these types are included.
     *
     */
    includeTypes?: (ItemType | string)[];
}

/**
 * Provides additional options for {@link
 * StructureManager.placeJigsaw}.
 */
export interface JigsawPlaceOptions {
    /**
     * @remarks
     * Whether entities should be included in the structure.
     * Defaults to true.
     *
     */
    includeEntities?: boolean;
    /**
     * @remarks
     * Whether the jigsaw blocks should be kept when generating the
     * structure. Defaults to false.
     *
     */
    keepJigsaws?: boolean;
    /**
     * @remarks
     * Specifies how to handle waterloggable blocks overlapping
     * with existing liquid. Defaults to `ApplyWaterlogging`.
     *
     */
    liquidSettings?: LiquidSettings;
}

/**
 * Provides additional options for {@link
 * StructureManager.placeJigsawStructure}.
 */
export interface JigsawStructurePlaceOptions {
    /**
     * @remarks
     * Whether the start height defined in the jigsaw structure
     * definition should be ignored and overridden with the
     * specified y coordinate. Defaults to false.
     *
     */
    ignoreStartHeight?: boolean;
    /**
     * @remarks
     * Whether entities should be included in the structure.
     * Defaults to true.
     *
     */
    includeEntities?: boolean;
    /**
     * @remarks
     * Whether the jigsaw blocks should be kept when generating the
     * structure. Defaults to false.
     *
     */
    keepJigsaws?: boolean;
    /**
     * @remarks
     * Specifies how to handle waterloggable blocks overlapping
     * with existing liquid. Defaults to `ApplyWaterlogging`.
     *
     */
    liquidSettings?: LiquidSettings;
}

/**
 * Less than operator.
 */
export interface LessThanComparison {
    /**
     * @remarks
     * Threshold value compared against.
     *
     */
    lessThan: number;
}

/**
 * Less than or equal to operator.
 */
export interface LessThanOrEqualsComparison {
    /**
     * @remarks
     * Threshold value compared against.
     *
     */
    lessThanOrEquals: number;
}

/**
 * Additional configuration options for {@link
 * World.playMusic}/{@link World.queueMusic} methods.
 */
export interface MusicOptions {
    /**
     * @remarks
     * Specifies a fade overlap for music at the end of play.
     *
     */
    fade?: number;
    /**
     * @remarks
     * If set to true, this music track will play repeatedly.
     *
     */
    loop?: boolean;
    /**
     * @remarks
     * Relative volume level of the music.
     *
     */
    volume?: number;
}

/**
 * Not equal to operator.
 */
export interface NotEqualsComparison {
    /**
     * @remarks
     * Threshold value compared against.
     *
     */
    notEquals: boolean | number | string;
}

/**
 * Contains additional options for how an animation is played.
 */
export interface PlayAnimationOptions {
    /**
     * @remarks
     * Amount of time to fade out after an animation stops.
     *
     */
    blendOutTime?: number;
    /**
     * @remarks
     * Specifies a controller to use that has been defined on the
     * entity.
     *
     */
    controller?: string;
    /**
     * @remarks
     * Specifies the state to transition to.
     *
     */
    nextState?: string;
    /**
     * @remarks
     * A list of players the animation will be visible to.
     *
     */
    players?: Player[];
    /**
     * @remarks
     * Specifies a Molang expression for when this animation should
     * complete.
     *
     */
    stopExpression?: string;
}

/**
 * Settings relating to a player's aim-assist targeting.
 */
export interface PlayerAimAssistSettings {
    /**
     * @remarks
     * The view distance limit to use for aim-assist targeting.
     *
     */
    distance?: number;
    /**
     * @remarks
     * The Id of the aim-assist preset to activate. Must have a
     * namespace.
     *
     */
    presetId: string;
    /**
     * @remarks
     * The mode to use for aim-assist targeting.
     *
     */
    targetMode?: AimAssistTargetMode;
    /**
     * @remarks
     * The view angle limit to use for aim-assist targeting.
     *
     */
    viewAngle?: Vector2;
}

/**
 * @rc
 * An interface that is passed into {@link
 * PlayerStartBreakingBlockAfterEventSignal.subscribe} or
 * {@link PlayerCancelBreakingBlockAfterEventSignal.subscribe}
 * that filters out which events are passed to the provided
 * callback.
 */
export interface PlayerBreakingBlockEventOptions {
    /**
     * @remarks
     * The {@link BlockFilter} that the callback should be called
     * for. If undefined, the callback will be called for all
     * blocks.
     *
     */
    blockFilter?: BlockFilter;
    /**
     * @remarks
     * The {@link EntityFilter} that the callback should be called
     * for. If undefined, the callback will be called for all
     * players.
     *
     */
    playerFilter?: EntityFilter;
}

/**
 * Additional options for how a sound plays for a player.
 */
export interface PlayerSoundOptions {
    /**
     * @remarks
     * Location of the sound; if not specified, the sound is played
     * near a player.
     *
     */
    location?: Vector3;
    /**
     * @beta
     * @remarks
     * Number of additional times to repeat the sound after the
     * initial play. `0` (the default) plays the sound once, `-1`
     * loops it forever, and a positive integer `N` plays the sound
     * `N + 1` times in total. For example, `loopCount: 1` plays
     * the sound twice. The loop count is fixed when the sound
     * starts and cannot be changed afterward. When using `-1`, see
     * `SoundInstance` for handle lifetime requirements.
     *
     */
    loopCount?: number;
    /**
     * @remarks
     * Optional pitch of the sound.
     *
     */
    pitch?: number;
    /**
     * @remarks
     * Optional volume of the sound.
     *
     */
    volume?: number;
}

/**
 * An interface that is passed into {@link
 * @minecraft/server.PlayerSwingStartAfterEventSignal.subscribe} that
 * filters out which events are passed to the provided
 * callback.
 */
export interface PlayerSwingEventOptions {
    /**
     * @remarks
     * The held item option that the callback should be called for.
     * If undefined, the callback will be called whether or not the
     * player is holding an item in their hand.
     *
     */
    heldItemOption?: HeldItemOption;
    /**
     * @remarks
     * The {@link EntitySwingSource} that the callback should be
     * called for. If undefined, the callback will be called for
     * all swing sources.
     *
     */
    swingSource?: EntitySwingSource;
}

/**
 * Controls when a waypoint is visible based on player-specific
 * states. Extends {@link EntityVisibilityRules} with
 * additional rules for player-only states like hidden mode and
 * spectator mode.
 */
// @ts-ignore Class inheritance allowed for native defined classes
export interface PlayerVisibilityRules extends EntityVisibilityRules {
    /**
     * @remarks
     * Controls whether the waypoint is shown when the tracked
     * player is hidden. If undefined, defaults to true.
     *
     */
    showHidden?: boolean;
    /**
     * @remarks
     * Controls whether the waypoint is shown when the tracked
     * player is in spectator mode. If undefined, defaults to true.
     *
     */
    showSpectator?: boolean;
    /**
     * @remarks
     * Controls whether the waypoint is shown when a spectator is
     * viewing another spectator player. If undefined, defaults to
     * true.
     *
     */
    showSpectatorToSpectator?: boolean;
}

/**
 * Key frame that holds the progress of the camera animation.
 */
export interface ProgressKeyFrame {
    /**
     * @remarks
     * Value to denote how far along the curve the camera will be.
     * Values are [0.0, 1.0] inclusive.
     *
     */
    alpha: number;
    /**
     * @remarks
     * The optional easing type that the frame will use for
     * position.
     *
     */
    easingFunc?: EasingType;
    /**
     * @remarks
     * Time value that the camera will be at the given alpha.
     *
     */
    timeSeconds: number;
}

/**
 * Optional arguments for
 * @minecraft/server.EntityProjectileComponent.shoot.
 */
export interface ProjectileShootOptions {
    /**
     * @remarks
     * Controls the accuracy of the shot. A value of 0 is perfect
     * accuracy.
     *
     */
    uncertainty?: number;
}

/**
 * Operator represents a lower/upper bound structure for
 * expressing a potential range of numbers.
 */
export interface RangeComparison {
    /**
     * @remarks
     * Lower bound within a range.
     *
     */
    lowerBound: number;
    /**
     * @remarks
     * Upper bound within a range.
     *
     */
    upperBound: number;
}

/**
 * Defines a JSON structure that is used for more flexible.
 * @seeExample addTranslatedSign.ts c0399cc7
 * @seeExample showTranslatedMessageForm.ts
 * @seeExample addTranslatedSign.ts 9e2fd749
 */
export interface RawMessage {
    /**
     * @remarks
     * Provides a raw-text equivalent of the current message.
     *
     */
    rawtext?: RawMessage[];
    /**
     * @remarks
     * Provides a token that will get replaced with the value of a
     * score.
     *
     */
    score?: RawMessageScore;
    /**
     * @remarks
     * Provides a string literal value to use.
     *
     */
    text?: string;
    /**
     * @remarks
     * Provides a translation token where, if the client has an
     * available resource in the players' language which matches
     * the token, will get translated on the client.
     *
     */
    translate?: string;
    /**
     * @remarks
     * Arguments for the translation token. Can be either an array
     * of strings or RawMessage containing an array of raw text
     * objects.
     *
     */
    with?: string[] | RawMessage;
}

/**
 * Provides a description of a score token to use within a raw
 * message.
 */
export interface RawMessageScore {
    /**
     * @remarks
     * Name of the score value to match.
     *
     */
    name?: string;
    /**
     * @remarks
     * Name of the score value to match.
     *
     */
    objective?: string;
}

/**
 * A `RawMessage` with only the `rawtext` property. When a
 * `RawMessage` is serialized the contents are put into a
 * rawtext property, so this is useful when reading saved
 * RawMessages. See `BlockSignComponent.setText` and
 * `BlockSignComponent.getRawText` for examples.
 */
export interface RawText {
    /**
     * @remarks
     * A serialization of the current value of an associated sign.
     *
     */
    rawtext?: RawMessage[];
}

/**
 * Represents a fully customizable color within Minecraft.
 */
export interface RGB {
    /**
     * @remarks
     * Determines a color's blue component. Valid values are
     * between 0 and 1.0.
     *
     */
    blue: number;
    /**
     * @remarks
     * Determines a color's green component. Valid values are
     * between 0 and 1.0.
     *
     */
    green: number;
    /**
     * @remarks
     * Determines a color's red component. Valid values are between
     * 0 and 1.0.
     *
     */
    red: number;
}

/**
 * Represents a fully customizable color within Minecraft.
 */
// @ts-ignore Class inheritance allowed for native defined classes
export interface RGBA extends RGB {
    /**
     * @remarks
     * Determines a color's alpha (opacity) component. Valid values
     * are between 0 (transparent) and 1.0 (opaque).
     *
     */
    alpha: number;
}

/**
 * Key frame that holds the rotation of the camera animation.
 */
export interface RotationKeyFrame {
    /**
     * @remarks
     * The optional easing type that the frame will use for
     * rotation.
     *
     */
    easingFunc?: EasingType;
    /**
     * @remarks
     * Value of the rotation of the camera.
     *
     */
    rotation: Vector3;
    /**
     * @remarks
     * Time value that the camera will be at the given rotation.
     *
     */
    timeSeconds: number;
}

/**
 * 表示记分项显示位置配置。描述了如何在记分板显示位置上显示记分项。
 * 
 * Contains additional options for how a scoreboard should be
 * displayed within its display slot.
 */
export interface ScoreboardObjectiveDisplayOptions {
    /**
     * @remarks
     * 要显示的记分项。
     * 
     * Objective to be displayed.
     *
     */
    objective: ScoreboardObjective;
    /**
     * @remarks
     * 记分项条目的显示排序顺序。
     * 
     * The sort order to display the objective items within.
     *
     */
    sortOrder?: ObjectiveSortOrder;
}

/**
 * Contains additional options for registering a script event
 * event callback.
 */
export interface ScriptEventMessageFilterOptions {
    /**
     * @remarks
     * Optional list of namespaces to filter inbound script event
     * messages.
     *
     */
    namespaces: string[];
}

/**
 * @beta
 * Duration metadata declared in a sound definition.
 */
export interface SoundDefinitionDurationInfo {
    /**
     * @remarks
     * Total duration of the sound in seconds, as declared in the
     * sound definition.
     *
     */
    duration: number;
}

/**
 * @beta
 * Criteria used to narrow a set of sound definitions. Each
 * field is optional and applies its constraint only when
 * defined; a definition must satisfy every defined field to
 * pass.
 */
export interface SoundDefinitionFilter {
    /**
     * @remarks
     * Artist names to match against the definition's
     * music_info.artist. Comparison is case-insensitive. When
     * defined as a non-empty array, a definition passes only when
     * its declared artist matches one of the supplied values. When
     * undefined, no constraint on artist is applied.
     *
     */
    artists?: string[];
    /**
     * @remarks
     * Genres to match against the definition's music_info.genres.
     * Comparison is case-insensitive. When defined as a non-empty
     * array, a definition passes only when at least one of its
     * declared genres matches one of the supplied values. When
     * undefined, no constraint on genres is applied.
     *
     */
    genres?: string[];
    /**
     * @remarks
     * Upper bound in seconds, inclusive. When defined, definitions
     * with a longer duration and definitions without a declared
     * duration are excluded. When undefined, no upper bound is
     * applied.
     *
     */
    maxDuration?: number;
    /**
     * @remarks
     * Lower bound in seconds, inclusive. When defined, definitions
     * with a shorter duration and definitions without a declared
     * duration are excluded. When undefined, no lower bound is
     * applied.
     *
     */
    minDuration?: number;
    /**
     * @remarks
     * Moods to match against the definition's music_info.moods.
     * Comparison is case-insensitive. When defined as a non-empty
     * array, a definition passes only when at least one of its
     * declared moods matches one of the supplied values. When
     * undefined, no constraint on moods is applied.
     *
     */
    moods?: string[];
    /**
     * @remarks
     * Tag constraints to match against the definition's tags.
     * Comparisons of tag names and values are case-insensitive.
     * When defined as a non-empty record, a definition passes only
     * when, for each entry with a non-empty value array, the tag
     * name is present on the definition with at least one matching
     * value. When undefined, no constraint on tags is applied.
     *
     */
    tags?: Record<string, string[]>;
    /**
     * @remarks
     * Titles to match against the definition's music_info.title.
     * Comparison is case-insensitive. When defined as a non-empty
     * array, a definition passes only when its declared title
     * matches one of the supplied values. When undefined, no
     * constraint on title is applied.
     *
     */
    titles?: string[];
}

/**
 * @beta
 * Music metadata declared on a sound definition. Each field is
 * optional and is undefined when the sound definition does not
 * declare a value for it.
 */
export interface SoundDefinitionMusicInfo {
    /**
     * @remarks
     * Artist declared for this sound. Undefined when no artist was
     * declared.
     *
     */
    artist?: string;
    /**
     * @remarks
     * Genres declared for this sound. Undefined when no genres
     * were declared.
     *
     */
    genres?: string[];
    /**
     * @remarks
     * Moods declared for this sound. Undefined when no moods were
     * declared.
     *
     */
    moods?: string[];
    /**
     * @remarks
     * Title declared for this sound. Undefined when no title was
     * declared.
     *
     */
    title?: string;
}

/**
 * Contains additional options for spawning an Entity.
 */
export interface SpawnEntityOptions {
    /**
     * @remarks
     * Optional boolean which determines if this entity should
     * persist in the game world. Persistence prevents the entity
     * from automatically despawning.
     *
     */
    initialPersistence?: boolean;
    /**
     * @remarks
     * Optional initial rotation, in degrees, to set on the entity
     * when it spawns.
     *
     */
    initialRotation?: number;
    /**
     * @remarks
     * Optional spawn event to send to the entity after it is
     * spawned.
     *
     */
    spawnEvent?: string;
}

/**
 * Collection of key frames for camera animation.
 */
export interface SplineAnimation {
    /**
     * @remarks
     * Key frames for camera progress along a given curve.
     *
     */
    progressKeyFrames: ProgressKeyFrame[];
    /**
     * @remarks
     * Key frames for camera rotation.
     *
     */
    rotationKeyFrames: RotationKeyFrame[];
}

/**
 * Provides additional options for {@link
 * StructureManager.createFromWorld}
 */
export interface StructureCreateOptions {
    /**
     * @remarks
     * Whether blocks should be included in the structure. Defaults
     * to true.
     *
     */
    includeBlocks?: boolean;
    /**
     * @remarks
     * Whether entities should be included in the structure.
     * Defaults to true.
     *
     */
    includeEntities?: boolean;
    /**
     * @remarks
     * How the Structure should be saved. Defaults to
     * StructureSaveMode.World.
     *
     */
    saveMode?: StructureSaveMode;
}

/**
 * Provides additional options for {@link
 * StructureManager.place}
 */
export interface StructurePlaceOptions {
    /**
     * @remarks
     * How the Structure should be animated when placed.
     *
     */
    animationMode?: StructureAnimationMode;
    /**
     * @remarks
     * How many seconds the animation should take.
     *
     */
    animationSeconds?: number;
    /**
     * @remarks
     * Whether blocks should be included in the structure. Defaults
     * to true.
     *
     */
    includeBlocks?: boolean;
    /**
     * @remarks
     * Whether entities should be included in the structure.
     * Defaults to true.
     *
     */
    includeEntities?: boolean;
    /**
     * @remarks
     * What percentage of blocks should be placed. A value of 1
     * will place 100% of the blocks while a value of 0 will place
     * none. The blocks are chosen randomly based on the {@link
     * StructurePlaceOptions.integritySeed}.
     *
     */
    integrity?: number;
    /**
     * @remarks
     * Seed that determines which blocks are randomly chosen to be
     * placed. Defaults to a random seed.
     *
     */
    integritySeed?: string;
    /**
     * @remarks
     * Which axes the Structure should be mirrored on when placed.
     * Defaults to StructureMirrorAxis.None.
     *
     */
    mirror?: StructureMirrorAxis;
    /**
     * @remarks
     * How the Structure should be rotated when placed. Defaults to
     * AxisAlignedRotation.None.
     *
     */
    rotation?: StructureRotation;
    /**
     * @remarks
     * Whether the structure should be waterlogged when placed.
     * Defaults to false. If true, blocks will become waterlogged
     * when placed in water.
     *
     */
    waterlogged?: boolean;
}

/**
 * Contains additional options for teleporting an entity.
 * @seeExample teleport.ts
 * @seeExample teleportMovement.ts
 */
export interface TeleportOptions {
    /**
     * @remarks
     * Whether to check whether blocks will block the entity after
     * teleport.
     *
     */
    checkForBlocks?: boolean;
    /**
     * @remarks
     * Dimension to potentially move the entity to.  If not
     * specified, the entity is teleported within the dimension
     * that they reside.
     *
     */
    dimension?: Dimension;
    /**
     * @remarks
     * Location that the entity should be facing after teleport.
     *
     */
    facingLocation?: Vector3;
    /**
     * @remarks
     * Whether to retain the entities velocity after teleport.
     *
     */
    keepVelocity?: boolean;
    /**
     * @remarks
     * Rotation of the entity after teleport.
     *
     */
    rotation?: Vector2;
}

/**
 * A context which provides information about a specific
 * ticking area.
 */
export interface TickingArea {
    /**
     * @remarks
     * The box which contains all the ticking blocks in the ticking
     * area.
     *
     */
    boundingBox: BlockBoundingBox;
    /**
     * @remarks
     * The number of chunks that the ticking area contains.
     *
     */
    chunkCount: number;
    /**
     * @remarks
     * The dimension the ticking area is located.
     *
     */
    dimension: Dimension;
    /**
     * @remarks
     * The unique identifier of the ticking area.
     *
     */
    identifier: string;
    /**
     * @remarks
     * Will be true if all the ticking areas chunks are loaded in
     * ticking and false otherwise.
     *
     */
    isFullyLoaded: boolean;
}

/**
 * Options to create a ticking area using the {@link
 * TickingAreaManager}.
 */
export interface TickingAreaOptions {
    /**
     * @remarks
     * The dimension the ticking area will be in.
     *
     */
    dimension: Dimension;
    /**
     * @remarks
     * Corner block location of the bounding box.
     *
     */
    from: Vector3;
    /**
     * @remarks
     * Opposite corner block location of the bounding box.
     *
     */
    to: Vector3;
}

/**
 * Contains additional options for displaying a title and
 * optional subtitle.
 */
export interface TitleDisplayOptions {
    /**
     * @remarks
     * Fade-in duration for the title and subtitle, in ticks. There
     * are 20 ticks per second. Use {@link TicksPerSecond} constant
     * to convert between ticks and seconds.
     *
     */
    fadeInDuration: number;
    /**
     * @remarks
     * Fade-out time for the title and subtitle, in ticks. There
     * are 20 ticks per second. Use {@link TicksPerSecond} constant
     * to convert between ticks and seconds.
     *
     */
    fadeOutDuration: number;
    /**
     * @remarks
     * Amount of time for the title and subtitle to stay in place,
     * in ticks. There are 20 ticks per second. Use {@link
     * TicksPerSecond} constant to convert between ticks and
     * seconds.
     *
     */
    stayDuration: number;
    /**
     * @remarks
     * Optional subtitle text.
     *
     */
    subtitle?: (RawMessage | string)[] | RawMessage | string;
}

/**
 * Represents a two-directional vector.
 */
export interface Vector2 {
    /**
     * @remarks
     * X component of the two-dimensional vector.
     *
     */
    x: number;
    /**
     * @remarks
     * Y component of the two-dimensional vector.
     *
     */
    y: number;
}

/**
 * Contains a description of a vector.
 */
export interface Vector3 {
    /**
     * @remarks
     * X component of this vector.
     *
     */
    x: number;
    /**
     * @remarks
     * Y component of this vector.
     *
     */
    y: number;
    /**
     * @remarks
     * Z component of this vector.
     *
     */
    z: number;
}

export interface VectorXZ {
    x: number;
    z: number;
}

/**
 * Defines a texture and the distance range in which it should
 * be displayed. Used within a {@link WaypointTextureSelector}
 * to create distance-based texture switching.
 */
export interface WaypointTextureBounds {
    /**
     * @remarks
     * The lower distance bound for this texture. The texture is
     * displayed when the distance to the waypoint is greater than
     * this value. Value must be greater than or equal to 0.
     *
     * Minimum Value: 0
     */
    lowerBound: number;
    /**
     * @remarks
     * The {@link WaypointTexture} or {@link CustomTexture} to
     * display within this distance range.
     *
     */
    texture: CustomTexture | WaypointTexture;
    /**
     * @remarks
     * The upper distance bound for this texture. The texture is
     * displayed when the distance to the waypoint is less than or
     * equal to this value. If undefined, there is no upper limit.
     * Value must be greater than or equal to 0.
     *
     * Minimum Value: 0
     */
    upperBound?: number;
}

/**
 * Defines how waypoint textures change based on distance.
 * Contains a list of texture bounds that determine which
 * texture is displayed at different distance ranges.
 */
export interface WaypointTextureSelector {
    /**
     * @remarks
     * An array of {@link WaypointTextureBounds} that define which
     * textures are displayed at different distance ranges. The
     * system evaluates these bounds to determine the appropriate
     * texture based on the current distance to the waypoint. The
     * list has a maximum size limit of 16.
     *
     */
    textureBoundsList: WaypointTextureBounds[];
}

/**
 * Contains additional options for a playSound occurrence.
 */
export interface WorldSoundOptions {
    /**
     * @beta
     * @remarks
     * Number of additional times to repeat the sound after the
     * initial play. `0` (the default) plays the sound once, `-1`
     * loops it forever, and a positive integer `N` plays the sound
     * `N + 1` times in total. For example, `loopCount: 1` plays
     * the sound twice. The loop count is fixed when the sound
     * starts and cannot be changed afterward. When using `-1`, see
     * `SoundInstance` for handle lifetime requirements.
     *
     */
    loopCount?: number;
    /**
     * @remarks
     * Pitch of the sound played.
     *
     */
    pitch?: number;
    /**
     * @remarks
     * Relative volume and space by which this sound is heard.
     *
     */
    volume?: number;
}

// @ts-ignore Class inheritance allowed for native defined classes
export class BlockCustomComponentAlreadyRegisteredError extends Error {
    private constructor();
}

// @ts-ignore Class inheritance allowed for native defined classes
export class BlockCustomComponentReloadNewComponentError extends Error {
    private constructor();
}

// @ts-ignore Class inheritance allowed for native defined classes
export class BlockCustomComponentReloadNewEventError extends Error {
    private constructor();
}

// @ts-ignore Class inheritance allowed for native defined classes
export class BlockCustomComponentReloadVersionError extends Error {
    private constructor();
}

/**
 * Errors that can be thrown when using {@link
 * ItemBookComponent}.
 */
// @ts-ignore Class inheritance allowed for native defined classes
export class BookError extends Error {
    private constructor();
    /**
     * @remarks
     * The reason for the error.
     *
     * @earlyExecution
     *
     */
    readonly reason: BookErrorReason;
}

/**
 * The error called if page content being set on an {@link
 * ItemBookComponent} are invalid ie. exceeding the maximum
 * page length.
 */
// @ts-ignore Class inheritance allowed for native defined classes
export class BookPageContentError extends Error {
    private constructor();
    /**
     * @remarks
     * The index of the page requested to be modified.
     *
     * @earlyExecution
     *
     */
    readonly pageIndex: number;
    /**
     * @remarks
     * The reason for the error.
     *
     * @earlyExecution
     *
     */
    readonly reason: BookErrorReason;
}

// @ts-ignore Class inheritance allowed for native defined classes
export class CommandError extends Error {
    private constructor();
}

/**
 * Error thrown if {@link ContainerRules} are broken on
 * container operations.
 */
// @ts-ignore Class inheritance allowed for native defined classes
export class ContainerRulesError extends Error {
    private constructor();
    /**
     * @remarks
     * The specific reason the error was thrown.
     *
     * @earlyExecution
     *
     */
    readonly reason: ContainerRulesErrorReason;
}

/**
 * Error object thrown when CustomCommandRegistry errors occur.
 */
// @ts-ignore Class inheritance allowed for native defined classes
export class CustomCommandError extends Error {
    private constructor();
    /**
     * @remarks
     * Reason for the error.
     *
     * @earlyExecution
     *
     */
    readonly reason: CustomCommandErrorReason;
}

// @ts-ignore Class inheritance allowed for native defined classes
export class CustomComponentInvalidRegistryError extends Error {
    private constructor();
}

// @ts-ignore Class inheritance allowed for native defined classes
export class CustomComponentNameError extends Error {
    private constructor();
    /**
     * @remarks
     * @earlyExecution
     *
     */
    readonly reason: CustomComponentNameErrorReason;
}

/**
 * Thrown when trying to register a custom dimension with a
 * name that has already been registered.
 */
// @ts-ignore Class inheritance allowed for native defined classes
export class CustomDimensionAlreadyRegisteredError extends Error {
    private constructor();
}

/**
 * Thrown when trying to register a custom dimension outside of
 * the system startup event.
 */
// @ts-ignore Class inheritance allowed for native defined classes
export class CustomDimensionInvalidRegistryError extends Error {
    private constructor();
}

/**
 * Thrown when trying to register a custom dimension with a
 * name that contains invalid characters.
 */
// @ts-ignore Class inheritance allowed for native defined classes
export class CustomDimensionNameError extends Error {
    private constructor();
}

/**
 * Thrown after using the /reload command when trying to
 * register a custom dimension that was not previously
 * registered. New custom dimensions cannot be added during a
 * reload.
 */
// @ts-ignore Class inheritance allowed for native defined classes
export class CustomDimensionReloadNewDimensionError extends Error {
    private constructor();
}

// @ts-ignore Class inheritance allowed for native defined classes
export class EnchantmentLevelOutOfBoundsError extends Error {
    private constructor();
}

// @ts-ignore Class inheritance allowed for native defined classes
export class EnchantmentTypeNotCompatibleError extends Error {
    private constructor();
}

// @ts-ignore Class inheritance allowed for native defined classes
export class EnchantmentTypeUnknownIdError extends Error {
    private constructor();
}

// @ts-ignore Class inheritance allowed for native defined classes
export class EntitySpawnError extends Error {
    private constructor();
}

/**
 * @beta
 * Error thrown by {@link FogSettings} operations when the fog
 * stack limit is exceeded or an invalid fog identifier is
 * provided.
 */
// @ts-ignore Class inheritance allowed for native defined classes
export class FogSettingsError extends Error {
    private constructor();
}

/**
 * The error can occur when a block is invalid. This can also
 * occur when accessing components on a block that doesn't have
 * them.
 */
// @ts-ignore Class inheritance allowed for native defined classes
export class InvalidBlockComponentError extends Error {
    private constructor();
}

/**
 * The container is invalid. This can occur if the container is
 * missing or deleted.
 */
// @ts-ignore Class inheritance allowed for native defined classes
export class InvalidContainerError extends Error {
    private constructor();
}

/**
 * The container slot is invalid. This can occur when the
 * owning container is destroyed or unloaded.
 */
// @ts-ignore Class inheritance allowed for native defined classes
export class InvalidContainerSlotError extends Error {
    private constructor();
}

/**
 * This error can occur when accessing components on an entity
 * that doesn't have them.
 */
// @ts-ignore Class inheritance allowed for native defined classes
export class InvalidEntityComponentError extends Error {
    private constructor();
}

/**
 * The error called when an entity is invalid. This can occur
 * when accessing components on a removed entity.
 */
// @ts-ignore Class inheritance allowed for native defined classes
export class InvalidEntityError extends Error {
    private constructor();
    /**
     * @remarks
     * The id of the entity that is now invalid.
     *
     * @earlyExecution
     *
     */
    readonly id: string;
    /**
     * @remarks
     * The type of the entity that is now invalid.
     *
     * @earlyExecution
     *
     */
    readonly type: string;
}

/**
 * The error called when an item is invalid. This can occur
 * when accessing components on a removed item.
 */
// @ts-ignore Class inheritance allowed for native defined classes
export class InvalidItemStackError extends Error {
    private constructor();
    /**
     * @remarks
     * The type of the item that is now invalid.
     *
     * @earlyExecution
     *
     */
    readonly itemType: ItemType;
}

// @ts-ignore Class inheritance allowed for native defined classes
export class InvalidIteratorError extends Error {
    private constructor();
}

// @ts-ignore Class inheritance allowed for native defined classes
export class InvalidPotionDeliveryTypeError extends Error {
    private constructor();
}

// @ts-ignore Class inheritance allowed for native defined classes
export class InvalidPotionEffectTypeError extends Error {
    private constructor();
}

/**
 * Thrown when a Structure is invalid. A structure becomes
 * invalid when it is deleted.
 */
// @ts-ignore Class inheritance allowed for native defined classes
export class InvalidStructureError extends Error {
    private constructor();
}

/**
 * Error thrown when attempting to perform operations on an
 * invalid waypoint. A waypoint becomes invalid when it is
 * removed or when the entity it tracks is no longer valid.
 */
// @ts-ignore Class inheritance allowed for native defined classes
export class InvalidWaypointError extends Error {
    private constructor();
}

// @ts-ignore Class inheritance allowed for native defined classes
export class InvalidWaypointTextureSelectorError extends Error {
    private constructor();
}

/**
 * Thrown when trying to register an item custom component with
 * a name that has already been registered.
 */
// @ts-ignore Class inheritance allowed for native defined classes
export class ItemCustomComponentAlreadyRegisteredError extends Error {
    private constructor();
}

/**
 * Thrown after using the /reload command when trying to
 * register a previously unregistered item custom component.
 */
// @ts-ignore Class inheritance allowed for native defined classes
export class ItemCustomComponentReloadNewComponentError extends Error {
    private constructor();
}

/**
 * Thrown after using the /reload command when trying to
 * register a previously registered item custom component that
 * handles a new event.
 */
// @ts-ignore Class inheritance allowed for native defined classes
export class ItemCustomComponentReloadNewEventError extends Error {
    private constructor();
}

/**
 * Thrown after using the /reload command when trying to
 * register a previously registered item custom component with
 * a newer API version.
 */
// @ts-ignore Class inheritance allowed for native defined classes
export class ItemCustomComponentReloadVersionError extends Error {
    private constructor();
}

/**
 * Thrown when the chunk for provided location or bounding area
 * is not loaded.
 */
// @ts-ignore Class inheritance allowed for native defined classes
export class LocationInUnloadedChunkError extends Error {
    private constructor();
}

/**
 * Thrown when a provided location or bounding area is outside
 * the minimum or maximum dimension height.
 */
// @ts-ignore Class inheritance allowed for native defined classes
export class LocationOutOfWorldBoundariesError extends Error {
    private constructor();
}

/**
 * Error thrown when a locator bar operation fails. Contains a
 * reason code indicating the specific cause of the error.
 */
// @ts-ignore Class inheritance allowed for native defined classes
export class LocatorBarError extends Error {
    private constructor();
    /**
     * @remarks
     * The {@link LocatorBarErrorReason} code that indicates why
     * the locator bar operation failed.
     *
     * @earlyExecution
     *
     */
    readonly reason: LocatorBarErrorReason;
}

/**
 * Thrown when a name requires a namespace and an error occurs
 * when validating that namespace
 */
// @ts-ignore Class inheritance allowed for native defined classes
export class NamespaceNameError extends Error {
    private constructor();
    /**
     * @remarks
     * @earlyExecution
     *
     */
    readonly reason: NamespaceNameErrorReason;
}

// @ts-ignore Class inheritance allowed for native defined classes
export class PlaceJigsawError extends Error {
    private constructor();
}

// @ts-ignore Class inheritance allowed for native defined classes
export class PrimitiveShapeError extends Error {
    private constructor();
}

// @ts-ignore Class inheritance allowed for native defined classes
export class RawMessageError extends Error {
    private constructor();
}

/**
 * The error returned from invalid {@link TickingAreaManager}
 * method calls.
 */
// @ts-ignore Class inheritance allowed for native defined classes
export class TickingAreaError extends Error {
    private constructor();
    /**
     * @remarks
     * The specific reason that the error was thrown.
     *
     * @earlyExecution
     *
     */
    readonly reason: TickingAreaErrorReason;
}

/**
 * Error thrown when the specified area contains one or more
 * unloaded chunks.
 */
// @ts-ignore Class inheritance allowed for native defined classes
export class UnloadedChunksError extends Error {
    private constructor();
}

export const HudElementsCount = 13;
export const HudVisibilityCount = 2;
/**
 * @remarks
 * Holds the number of MoonPhases
 *
 */
export const MoonPhaseCount = 8;
/**
 * @remarks
 * How many times the server ticks in one in-game day.
 *
 */
export const TicksPerDay = 24000;
/**
 * @remarks
 * How many times the server ticks per second of real time.
 *
 */
export const TicksPerSecond = 20;
/**
 * @remarks
 * A class that provides system-level events and functions.
 *
 */
export const system: System;
/**
 * @remarks
 * A class that wraps the state of a world - a set of
 * dimensions and the environment of Minecraft.
 *
 */
export const world: World;
