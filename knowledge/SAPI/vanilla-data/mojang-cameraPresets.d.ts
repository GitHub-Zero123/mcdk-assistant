/**
 * All possible MinecraftCameraPresetsTypes
 */
export declare enum MinecraftCameraPresetsTypes {
    ControlSchemeCamera = "minecraft:control_scheme_camera",
    /** 第一人称视角。 */
    FirstPerson = "minecraft:first_person",
    /** 第三人称固定视角。 */
    FixedBoom = "minecraft:fixed_boom",
    /** 第三人称越肩视角。 */
    FollowOrbit = "minecraft:follow_orbit",
    /** 自由视角。 */
    Free = "minecraft:free",
    /** 第三人称背面视角。 */
    ThirdPerson = "minecraft:third_person",
    /** 第三人称正面视角。 */
    ThirdPersonFront = "minecraft:third_person_front"
}
/**
 * Union type equivalent of the MinecraftCameraPresetsTypes enum.
 */
export type MinecraftCameraPresetsTypesUnion = keyof typeof MinecraftCameraPresetsTypes;
