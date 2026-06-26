/**
 * All possible MinecraftEffectTypes
 */
export declare enum MinecraftEffectTypes {
    /** 伤害吸收。 */
    Absorption = "minecraft:absorption",
    /** 不祥之兆。 */
    BadOmen = "minecraft:bad_omen",
    /** 失明。 */
    Blindness = "minecraft:blindness",
    /** 鹦鹉螺之息。 */
    BreathOfTheNautilus = "minecraft:breath_of_the_nautilus",
    /** 潮涌能量。 */
    ConduitPower = "minecraft:conduit_power",
    /** 黑暗。 */
    Darkness = "minecraft:darkness",
    /** 中毒（致命）。 */
    FatalPoison = "minecraft:fatal_poison",
    /** 抗火。 */
    FireResistance = "minecraft:fire_resistance",
    /** 急迫。 */
    Haste = "minecraft:haste",
    /** 生命提升。 */
    HealthBoost = "minecraft:health_boost",
    /** 饥饿。 */
    Hunger = "minecraft:hunger",
    /** 寄生。 */
    Infested = "minecraft:infested",
    /** 瞬间伤害。 */
    InstantDamage = "minecraft:instant_damage",
    /** 瞬间治疗。 */
    InstantHealth = "minecraft:instant_health",
    /** 隐身。 */
    Invisibility = "minecraft:invisibility",
    /** 跳跃提升。 */
    JumpBoost = "minecraft:jump_boost",
    /** 飘浮。 */
    Levitation = "minecraft:levitation",
    /** 挖掘疲劳。 */
    MiningFatigue = "minecraft:mining_fatigue",
    /** 反胃。 */
    Nausea = "minecraft:nausea",
    /** 夜视。 */
    NightVision = "minecraft:night_vision",
    /** 渗浆。 */
    Oozing = "minecraft:oozing",
    /** 中毒。 */
    Poison = "minecraft:poison",
    /** 袭击之兆。 */
    RaidOmen = "minecraft:raid_omen",
    /** 生命恢复。 */
    Regeneration = "minecraft:regeneration",
    /** 抗性提升。 */
    Resistance = "minecraft:resistance",
    /** 饱和。 */
    Saturation = "minecraft:saturation",
    /** 缓降。 */
    SlowFalling = "minecraft:slow_falling",
    /** 缓慢。 */
    Slowness = "minecraft:slowness",
    /** 迅捷。 */
    Speed = "minecraft:speed",
    /** 力量。 */
    Strength = "minecraft:strength",
    /** 试炼之兆。 */
    TrialOmen = "minecraft:trial_omen",
    /** 村庄英雄。 */
    VillageHero = "minecraft:village_hero",
    /** 水下呼吸。 */
    WaterBreathing = "minecraft:water_breathing",
    /** 虚弱。 */
    Weakness = "minecraft:weakness",
    /** 盘丝。 */
    Weaving = "minecraft:weaving",
    /** 蓄风。 */
    WindCharged = "minecraft:wind_charged",
    /** 凋零。 */
    Wither = "minecraft:wither"
}
/**
 * Union type equivalent of the MinecraftEffectTypes enum.
 */
export type MinecraftEffectTypesUnion = keyof typeof MinecraftEffectTypes;
