/**
 * All possible MinecraftFeatureTypes
 */
export declare enum MinecraftFeatureTypes {
    /** 远古城市。 */
    AncientCity = "minecraft:ancient_city",
    /** 堡垒遗迹。 */
    BastionRemnant = "minecraft:bastion_remnant",
    /** 埋藏的宝藏。 */
    BuriedTreasure = "minecraft:buried_treasure",
    /** 末地城。 */
    EndCity = "minecraft:end_city",
    /** 下界要塞。 */
    Fortress = "minecraft:fortress",
    /** 林地府邸。 */
    Mansion = "minecraft:mansion",
    /** 废弃矿井。 */
    Mineshaft = "minecraft:mineshaft",
    /** 海底神殿。 */
    Monument = "minecraft:monument",
    /** 掠夺者前哨站。 */
    PillagerOutpost = "minecraft:pillager_outpost",
    /** 废弃传送门。 */
    RuinedPortal = "minecraft:ruined_portal",
    /** 海底废墟。 */
    Ruins = "minecraft:ruins",
    /** 沉船。 */
    Shipwreck = "minecraft:shipwreck",
    /** 要塞。 */
    Stronghold = "minecraft:stronghold",
    /** 沙漠神殿/雪屋/丛林神庙/沼泽小屋。 */
    Temple = "minecraft:temple",
    /** 古迹废墟。 */
    TrailRuins = "minecraft:trail_ruins",
    /** 试炼密室。 */
    TrialChambers = "minecraft:trial_chambers",
    /** 村庄。 */
    Village = "minecraft:village"
}
/**
 * Union type equivalent of the MinecraftFeatureTypes enum.
 */
export type MinecraftFeatureTypesUnion = keyof typeof MinecraftFeatureTypes;
