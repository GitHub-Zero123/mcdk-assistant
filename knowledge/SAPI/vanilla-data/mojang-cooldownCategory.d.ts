/**
 * All possible MinecraftCooldownCategoryTypes
 */
export declare enum MinecraftCooldownCategoryTypes {
    /** 紫颂果。 */
    Chorusfruit = "minecraft:chorusfruit",
    /** 末影珍珠。 */
    EnderPearl = "minecraft:ender_pearl",
    /** 山羊角。 */
    GoatHorn = "minecraft:goat_horn",
    /** 盾牌。 */
    Shield = "minecraft:shield",
    /** 矛。 */
    Spear = "minecraft:spear",
    /** 风弹。 */
    WindCharge = "minecraft:wind_charge"
}
/**
 * Union type equivalent of the MinecraftCooldownCategoryTypes enum.
 */
export type MinecraftCooldownCategoryTypesUnion = keyof typeof MinecraftCooldownCategoryTypes;
