/**
 * All possible MinecraftDimensionTypes
 */
export declare enum MinecraftDimensionTypes {
    /** 下界。 */
    Nether = "minecraft:nether",
    /** 主世界。 */
    Overworld = "minecraft:overworld",
    /** 末地。 */
    TheEnd = "minecraft:the_end"
}
/**
 * Union type equivalent of the MinecraftDimensionTypes enum.
 */
export type MinecraftDimensionTypesUnion = keyof typeof MinecraftDimensionTypes;
