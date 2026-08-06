# ENTITIES DOCUMENTATION Version: 1.21.120.4


## Index


# Client Entity Documentation


Client entity definitions are contained within a Resource Pack.


To start, create a new folder and name it "entity" inside the root of the Resource Pack. In the entity folder create a JSON file and give it a name.The JSON file needs a format version and minecraft:client_entity information.


The minecraft:client_entity section contains the description for the entity. Under description there are a number of things that you can set about an entity. Generally, this file is defining what resources a mob requires and gives a friendly name to each, that the other definition files can use.


Example client entity definitions JSON for the pig


```json
"format_version": "1.8.0",
  "minecraft:client_entity": {
     "description": {
       "identifier": "minecraft:pig",
       "min_engine_version": "1.8.0",
       "materials": { "default": "pig" },
       "textures": {
         "default": "textures/entity/pig/pig",
         "saddled": "textures/entity/pig/pig_saddle"
       },
       "geometry": {
         "default": "geometry.pig.v1.8"
       },
       "animations": {
         "setup": "animation.pig.setup",
         "walk": "animation.quadruped.walk",
         "look_at_target": "animation.common.look_at_target",
         "baby_transform": "animation.pig.baby_transform"
       },
       "animation_controllers": [
         { "setup": "controller.animation.pig.setup" },
         { "move": "controller.animation.pig.move" },
         { "baby": "controller.animation.pig.baby" }
       ],
       "render_controllers": [ "controller.render.pig" ],
       "locators": {
         "lead": { "head": [ 0.0, 14.0, -6.0 ] }
       },
       "spawn_egg": {
         "texture": "spawn_egg",
         "texture_index": 2
       }
     }
   }
```


# Spawn Egg


This sets the color or texture used for the entity Spawn Egg. There are 2 ways to do this. The first is to use the hex value for the base color and the overlay color.


## Hex Value


Example Spawn Egg using hex value


```json
"spawn_egg": {
  "base_color": "#53443E",
  "overlay_color": "#2E6854"
}
```


## Texture


The other way is to specify a texture.


Example Spawn Egg specifying a texture. The texture is located in the "items_texture" JSON in the textures folder of the Resource Pack. When there are more than one texture associated with a texture name you can use an index to pick the one that you want. If no index is specified than it is assumed to be 0 and the first texture in the list is used.


Example Spawn Egg specifying a texture


```json
"spawn_egg": {
  "texture": "spawn_egg", 
  "texture_index": 2
}
```


# animation_controllers


Animation controllers decide which animations to play when. Each Controller contains a list of states that play one or more animations. Allows the player to assign names to reference the long names for animation controllers. Names are required and need to be unique from all other names in the animation controllers for that mob. Players can reference animation controllers from the vanilla Minecraft Resource Pack or create their own. Custom animation controllers should be in the animation_controllers folder at the root of the Resource Pack.


# animations


Allows the player to assign names to reference the long name for animations. These names are used by the animation controller JSON. Players can reference animations from the vanilla Minecraft Resource Pack or create their own. Custom animations should be in the animation folder at the root of the Resource Pack.


# enable_attachables


This determines if the entity can equip attachables when this is set to true. This allows the entity to render armor and weapons.


```json
"enable_attachables": true
```


# held_item_ignores_lighting


This determines if the item held by an entity should render fully lit up (if true), or depending on surrounding lighting.


```json
"held_item_ignores_lighting": true
```


# hide_armor


This determines if the armor attached to an entity should be hidden when set to true. This overrides the rendering settings specified by 'enable_attachables'


```json
"hide_armor": true
```


# identifier


The identifier is used to register the entity with the server. In the Client Entity Definitions JSON the identifier sets the appearance of the entity(materials, textures, geometry, etc.) The matching identifier in the Entity Behavior JSON in the Behavior Pack is what gives the entity its behaviors.


# locators


Locator offsets are specified in model space. An example of a locator is the "lead" locator used to specify where the lead will attach to graphically.


```json
"locators": {
  "lead": { "head": [ 0.0, 14.0, -6.0 ] }
}
```


# materials, textures, animations


Players can set the materials, texture and geometry used for the entity in this section. Players can set one or more materials, textures, and geometries that can be used by the mob. Players must set user defined names for them. These names are used in the Render Controllers JSON. Players can reference materials, textures, and geometry from the vanilla Minecraft Resource Pack or create their own. Custom materials, textures, and geometry should be in the corresponding folder at the root of the Resource Pack.


# min_engine_version


When present, players can set the min version needed to allow the JSON to be parsed. The version in the definition is compared to the engine version for which the top resource pack was built.If a definition's min_engine_version is newer than that pack's engine version then the definition is not parsed.


Multiple definition files may use the same identifier, in which case only one of those definitions will be loaded. The definition with the same or closest and not greater min_engine_version, as compared to the top resource pack's engine version, will be parsed; all other definitions with the same identifier will not be parsed.


This can be useful for continuing to support an older version of an entity, when an older resource pack is used at the top of the resource pack stack, while also supporting a newer version of the entity in all other cases.


# particle


Allows the player to assign a key to reference the long name for particles. When these are present, the particle is created when the entity is spawned. Keys are required and need to be unique from all other keys in the animation controllers. Players can reference particles from the vanilla Minecraft Resource Pack or create their own. Custom particles should be in the particle folder at the root of the Resource Pack.


# render_controllers


Specifies the names of render controllers. This name needs to match the name of a corresponding JSON located in the Render Controllers folder. Players can reference Render Controllers from the vanilla Minecraft Resource Pack or create their own. Custom Render Controllers should be in the textures folder at the root of the Resource Pack.


# scripts


Scripts allow players to use Molang to compute calculations once and store that value. This value than can be used over and over again without the need to constantly recompute the calculations. Scripts currently support pre - animation and scale.More script types will be added later.


-Pre-animation scripts are evaluated immediately before animations are processed.


-Scale sets the scale of the mob's geometry.


Example pre-animation script for cod


```json
"scripts": {
  "pre_animation": [
    "variable.ZRot = !query.is_in_water ? Math.cos((query.time_stamp + global.frame_alpha) * 14.32) * 90 : 0.0;",
    "variable.AnimationAmountBlend = Math.lerp(variable.AnimationAmountPrev, variable.AnimationAmount, global.frame_alpha);"
  ]
},
```


Example scale script for the bat


```json
"scripts": {
  "scale": "0.35"
},
```


# Data-Driven Spawning


Data-Driven spawning allows you to adjust the spawn conditions of mobs, including new mobs added to the game to spawn naturally in biomes, add / remove a mob's spawn egg to the creative inventory and set the mob's spawn egg, and add / remove a to the / summon command


# Spawn Rules


## Biome Tags


Each biome in the game has one or more tags. These are used to determine what biomes mobs spawn in. Here is the list of Biome tags that can be used:


| Tag |
| --- |
| animal |
| beach |
| birch |
| cold |
| dark_oak |
| deep |
| desert |
| edge |
| extreme_hills |
| flower_forest |
| forest |
| frozen |
| hills |
| ice |
| ice_plains |
| jungle |
| lakes |
| lukewarm |
| mega |
| mesa |
| monster |
| mooshroom_island |
| mountain |
| mutated |
| nether |
| ocean |
| plains |
| plateau |
| river |
| roofed |
| savanna |
| shore |
| stone |
| swamp |
| taiga |
| the_end |
| warm |


## Conditions


Conditions contain different components that players can use to customize natural biome spawning.


### Components


| Name | Description |
| --- | --- |
| minecraft:biome_filter | This component allows the players to specify which biomes the mob spawns in. Check below to see which Biome Tags exist, and what each biome is tagged as. |
| minecraft:brightness_filter | This component allows players to set the light level range that causes the mob to spawn. |
| minecraft:density_limit | This component allows players to determine the density cap limits for the specified mob type. |
| minecraft:difficulty_filter | This component allows players determine what mobs spawn when certain difficulty levels are set. |
| minecraft:herd | This component allows players to determine the herd size of animals. |
| minecraft:spawns_on_surface | This component allows the mob to spawn on the ground. Adding the component causes it to be true, removing it causes the mob to stop spawning on the surface. |
| minecraft:spawns_underwater | This component allows the mob to spawn underwater. Adding the component causes it to be true, removing it causes the mob to stop spawning underwater. |
| minecraft:weight | This component allows players to give a priority to how often that mob should spawn. Mobs with lower weight values have a higher chance to spawn than mobs with higher weight values. |


> **minecraft:brightness_filter** 子参数


| Name | Type | Default Value | Description |
| --- | --- | --- | --- |
| adjust_for_weather | Boolean | false | This determines if weather can affect the light level conditions that cause the mob to spawn (e.g. Allowing hostile mobs to spawn during the day when it rains.) |
| max | Decimal | 15.0 | This is the maximum light level value that allows the mob to spawn |
| min | Decimal | 0.0 | This is the minimum light level value that allows the mob to spawn |


> **minecraft:density_limit** 子参数


| Name | Type | Default Value | Description |
| --- | --- | --- | --- |
| surface | Integer |  | This is the maximum number of mobs of this type spawnable on the surface |
| underground | Integer |  | This is the maximum number of mobs of this type spawnable underground |


> **minecraft:difficulty_filter** 子参数


| Name | Type | Default Value | Description |
| --- | --- | --- | --- |
| max | String |  | This is the maximum difficulty level that a mob spawns |
| min | String |  | This is the minimum difficulty level that a mob spawns |


> **minecraft:herd** 子参数


| Name | Type | Default Value | Description |
| --- | --- | --- | --- |
| event | String |  | This is an event that can be triggered from spawning |
| event_skip_count | Decimal |  | This is the number of mobs spawned before the specified event is triggered |
| max_size | Decimal |  | This is the maximum number of mobs that spawn in a herd |
| min_size | Decimal |  | This is the minimum number of mobs that spawn in a herd |


> **minecraft:weight** 子参数


| Name | Type | Default Value | Description |
| --- | --- | --- | --- |
| default | Decimal | 0.0 | This is the priority of the mob spawning |


## Getting Started


New Spawn Rules are contained within a Behavior Pack. To begin, create a new folder named "spawn_rules" in the root of the Behavior Pack that you want to add the new biome spawn rules in. In the spawn_rules folder, create a JSON file and give it a name. The JSON file needs a format, description and conditions.Spawn rules contain description and conditionsAll Spawn Rules JSON need to have an ID (located under the description section). Similar to other identifiers, it follows the convention "namespace:name". The minecraft namespace is reserved for the vanilla Minecraft rules.When changing an existing mob use the ID that appears in the entity JSON for that entity. When creating your own mob, make sure the mobs have the same ID in all the entity's JSON files.


Spawn Rules also needs to define the pool that is used for population control.Each pool has their own spawn limit, By setting an entity to a pool it will spawn as long as that pool hasn't reached the spawn limit.


There are 3 pools that entities can be assigned to :


-animal


-water_animal


-monster


## Tagged Biomes


| Biome | Tags |
| --- | --- |
| Beach | monster, beach, warm |
| Birch Forest | animal, monster, birch, forest |
| Birch Forest Hills | animal, monster, birch, forest, hills |
| Birch Forest Hills M | animal, monster, forest, birch, mutated, hills |
| Birch Forest M | animal, monster, forest, birch, mutated |
| Cold Ocean | ocean, cold, monster |
| Cold Taiga M | animal, monster, taiga, cold, mutated |
| Dark Forest | animal, monster, forest, roofed |
| Dark Forest M | animal, monster, roofed, forest, mutated |
| Deep Cold Ocean | ocean, cold, monster, deep |
| Deep Frozen Ocean | ocean, frozen, monster, deep |
| Deep Lukewarm Ocean | ocean, lukewarm, monster, deep |
| Deep Ocean | ocean, monster, deep |
| Deep Warm Ocean | ocean, warm, monster, deep |
| Desert | monster, desert |
| Desert Hills | monster, desert, hills |
| Desert M | monster, desert, mutated |
| Forest | animal, monster, forest |
| Forest Hills | animal, monster, hills |
| Forest M | monster, flower_forest, forest, mutated |
| Frozen Ocean | ocean, frozen, monster |
| Frozen River | river, frozen |
| Giant Tree Taiga | animal, monster, taiga, mega |
| Giant Tree Taiga Hills | animal, monster, taiga, mega, hills |
| Giant Tree Taiga Hills M | animal, monster, taiga, mega, hills, mutated |
| Giant Trees Taiga M | animal, monster, mutated, mega |
| Jungle | animal, monster, jungle |
| Jungle Edge | animal, monster, jungle, edge |
| Jungle Edge M | animal, monster, jungle, edge, mutated |
| Jungle Hills | animal, monster, jungle, hills |
| Jungle M | animal, monster, jungle, mutated |
| Lukewarm Ocean | ocean, lukewarm, monster |
| Mesa | monster |
| Mesa M | animal, monster, mesa, mutated |
| Mesa Plateau | monster, mesa |
| Mesa Plateau M | monster, mesa, plateau, mutated |
| Mesa Plateau Stone | monster, plateau |
| Mesa Plateau Stone M | monster, mesa, plateau, mutated, stone |
| Mountain | animal, monster, extreme_hills |
| Mountain + M | animal, monster, extreme_hills, mutated, forest |
| Mountain Edge | animal, monster, extreme_hills, edge, mountain |
| Mountain M | animal, monster, extreme_hills, mutated |
| Mushroom Fields | mooshroom_island |
| Mushroom Fields Shore | mooshroom_island, shore |
| Nether | nether |
| Ocean | ocean, monster |
| Plains | animal, monster, plains |
| Plains M | animal, monster, plains, mutated |
| River | river |
| Savanna | animal, monster, savanna |
| Savanna M | animal, monster, savanna, mutated |
| Savanna Plateau | animal, monster, savanna |
| Savanna Plateau M | animal, monster, savanna, plateau, mutated |
| Snowy Beach | monster, beach, cold |
| Snowy Mountains | frozen, ice, mountain |
| Snowy Taiga | animal, monster, taiga, cold |
| Snowy Taiga Hills | animal, monster, taiga, cold, hills |
| Snowy Tundra | frozen, ice_plain, ice |
| Snowy Tundra M | monster, frozen, ice_plains, mutated |
| Stone Shore | monster, beach, stone |
| Swamp | animal, monster, swamp |
| Swamp M | animal, monster, swamp, mutated |
| Taiga | animal, monster, taiga |
| Taiga Hills | animal, monster, taiga, hills |
| Taiga M | animal, monster, taiga, mutated |
| The End | the_end |
| Warm Ocean | ocean, warm, monster |
| Wooded Mountain | animal, monster, extreme_hills, forest, mountain |


Example Spawn Rules for the zombie


```json
"format_version": "1.8.0",
  "minecraft:spawn_rules": {
    "description": {
      "identifier": "minecraft:zombie",
      "population_control": "monster"
    },
    "conditions": [
      {
        "minecraft:spawns_on_surface": {},
        "minecraft:brightness_filter": {
          "min": 0,
          "max": 7,
          "adjust_for_weather": true
        },
        "minecraft:difficulty_filter": {
          "min": "easy",
          "max": "hard"
        },
        "minecraft:weight": {
          "default": 100
        },
        "minecraft:herd": {
          "min_size": 2,
          "max_size": 4
        },
        "minecraft:permute_type": [
          {
            "weight": 95
          },
          {
            "weight": 5,
            "entity_type": "minecraft:zombie_villager"
          }
        ],
        "minecraft:biome_filter": {
          "test": "has_biome_tag", "operator": "==", "value": "monster"
        }
      }
    ]
  }
```


# Filters


Filters allow data objects to specify test criteria which allows their use.


For example, a model that includes a filter will only be used when the filter criteria is true.


A typical filter consists of four parameters:


name: the name of the test to apply.


domain: the domain the test should be performed in. An armor slot, for example. This parameter is only used by a few tests.


operator: the comparison to apply with the value, such as 'equal' or 'greater'.


value: the value being compared with the test.


A typical filter looks like the following:


{ "test" : "moon_intensity", "subject" : "self", "operator" : "greater", "value" : "0.5" }


Which results in the calling entity (self) calculating the moon_intensity at its location and returning true if the result is greater than 0.5.


Tests can be combined into groups using the collections 'all_of', 'any_of', or 'none_of'.


All tests in an 'all_of' group must pass in order for the group to pass.


One or more tests in an 'any_of' group must pass in order for the group to pass.


All tests in a 'none_of' group must fail in order for the group to pass.


# actor_health


Tests the health of the subject.


| Name | Type | Default | Description |
| --- | --- | --- | --- |
| operator | String | equals | (Optional) The comparison to apply with 'value'. |
| subject | String | self | (Optional) The subject of this filter test. |
| value | Integer |  | (Required) An integer value. |


> **operator** 子参数


| Options | Description |
| --- | --- |
| != | Test for inequality. |
| < | Test for less-than the value. |
| <= | Test for less-than or equal to the value. |
| <> | Test for inequality. |
| = | Test for equality. |
| == | Test for equality. |
| > | Test for greater-than the value. |
| >= | Test for greater-than or equal to the value. |
| equals | Test for equality. |
| not | Test for inequality. |


> **subject** 子参数


| Options | Description |
| --- | --- |
| block | The block involved with the interaction. |
| damager | The damaging actor involved with the interaction. |
| other | The other member of an interaction, not the caller. |
| parent | The caller's current parent. |
| player | The player involved with the interaction. |
| self | The entity or object calling the test |
| target | The caller's current target. |


## Examples


Full..


```json
{ "test": "actor_health", "subject": "self", "operator": "equals", "value": "0" }
```


Short (using Defaults)..


```json
{ "test": "actor_health", "value": "0" }
```


# all_slots_empty


Returns true when the designated equipment location for the subject entity is completely empty.


| Name | Type | Default | Description |
| --- | --- | --- | --- |
| operator | String | equals | (Optional) The comparison to apply with 'value'. |
| subject | String | self | (Optional) The subject of this filter test. |
| value | String | any | (Optional) The equipment location to test |


> **operator** 子参数


| Options | Description |
| --- | --- |
| != | Test for inequality. |
| < | Test for less-than the value. |
| <= | Test for less-than or equal to the value. |
| <> | Test for inequality. |
| = | Test for equality. |
| == | Test for equality. |
| > | Test for greater-than the value. |
| >= | Test for greater-than or equal to the value. |
| equals | Test for equality. |
| not | Test for inequality. |


> **subject** 子参数


| Options | Description |
| --- | --- |
| block | The block involved with the interaction. |
| damager | The damaging actor involved with the interaction. |
| other | The other member of an interaction, not the caller. |
| parent | The caller's current parent. |
| player | The player involved with the interaction. |
| self | The entity or object calling the test |
| target | The caller's current target. |


> **value** 子参数


| Options | Description |
| --- | --- |
| any |  |
| armor |  |
| body |  |
| feet |  |
| hand |  |
| head |  |
| inventory |  |
| leg |  |
| main_hand |  |
| torso |  |


## Examples


Full..


```json
{ "test": "all_slots_empty", "subject": "self", "operator": "equals", "value": "any" }
```


Short (using Defaults)..


```json
{ "test": "all_slots_empty" }
```


# any_slot_empty


Returns true when the designated equipment location for the subject entity has any empty slot.


| Name | Type | Default | Description |
| --- | --- | --- | --- |
| operator | String | equals | (Optional) The comparison to apply with 'value'. |
| subject | String | self | (Optional) The subject of this filter test. |
| value | String | any | (Optional) The equipment location to test |


> **operator** 子参数


| Options | Description |
| --- | --- |
| != | Test for inequality. |
| < | Test for less-than the value. |
| <= | Test for less-than or equal to the value. |
| <> | Test for inequality. |
| = | Test for equality. |
| == | Test for equality. |
| > | Test for greater-than the value. |
| >= | Test for greater-than or equal to the value. |
| equals | Test for equality. |
| not | Test for inequality. |


> **subject** 子参数


| Options | Description |
| --- | --- |
| block | The block involved with the interaction. |
| damager | The damaging actor involved with the interaction. |
| other | The other member of an interaction, not the caller. |
| parent | The caller's current parent. |
| player | The player involved with the interaction. |
| self | The entity or object calling the test |
| target | The caller's current target. |


> **value** 子参数


| Options | Description |
| --- | --- |
| any |  |
| armor |  |
| body |  |
| feet |  |
| hand |  |
| head |  |
| inventory |  |
| leg |  |
| main_hand |  |
| torso |  |


## Examples


Full..


```json
{ "test": "any_slot_empty", "subject": "self", "operator": "equals", "value": "any" }
```


Short (using Defaults)..


```json
{ "test": "any_slot_empty" }
```


# bool_property


Returns true when the bool actor property matches the value provided.


| Name | Type | Default | Description |
| --- | --- | --- | --- |
| domain | String |  | (Required) The property name to look for |
| operator | String | equals | (Optional) The comparison to apply with 'value'. |
| subject | String | self | (Optional) The subject of this filter test. |
| value | Boolean | true | (Optional) true or false. |


> **operator** 子参数


| Options | Description |
| --- | --- |
| != | Test for inequality. |
| < | Test for less-than the value. |
| <= | Test for less-than or equal to the value. |
| <> | Test for inequality. |
| = | Test for equality. |
| == | Test for equality. |
| > | Test for greater-than the value. |
| >= | Test for greater-than or equal to the value. |
| equals | Test for equality. |
| not | Test for inequality. |


> **subject** 子参数


| Options | Description |
| --- | --- |
| block | The block involved with the interaction. |
| damager | The damaging actor involved with the interaction. |
| other | The other member of an interaction, not the caller. |
| parent | The caller's current parent. |
| player | The player involved with the interaction. |
| self | The entity or object calling the test |
| target | The caller's current target. |


## Examples


Full..


```json
{ "test": "bool_property", "subject": "self", "domain": "minecraft:can_climb", "operator": "equals", "value": "true" }
```


Short (using Defaults)..


```json
{ "test": "bool_property", "domain": "minecraft:can_climb" }
```


# clock_time


Compares the current time with a float value in the range (0.0, 1.0). 0.0= Noon 0.25= Sunset 0.5= Midnight 0.75= Sunrise


| Name | Type | Default | Description |
| --- | --- | --- | --- |
| operator | String | equals | (Optional) The comparison to apply with 'value'. |
| subject | String | self | (Optional) The subject of this filter test. |
| value | Decimal |  | (Required) A floating point value. |


> **operator** 子参数


| Options | Description |
| --- | --- |
| != | Test for inequality. |
| < | Test for less-than the value. |
| <= | Test for less-than or equal to the value. |
| <> | Test for inequality. |
| = | Test for equality. |
| == | Test for equality. |
| > | Test for greater-than the value. |
| >= | Test for greater-than or equal to the value. |
| equals | Test for equality. |
| not | Test for inequality. |


> **subject** 子参数


| Options | Description |
| --- | --- |
| block | The block involved with the interaction. |
| damager | The damaging actor involved with the interaction. |
| other | The other member of an interaction, not the caller. |
| parent | The caller's current parent. |
| player | The player involved with the interaction. |
| self | The entity or object calling the test |
| target | The caller's current target. |


## Examples


Full..


```json
{ "test": "clock_time", "subject": "self", "operator": "equals", "value": "0.00" }
```


Short (using Defaults)..


```json
{ "test": "clock_time", "value": "0.00" }
```


# distance_to_nearest_player


Compares the distance to the nearest Player with a float value.


| Name | Type | Default | Description |
| --- | --- | --- | --- |
| operator | String | equals | (Optional) The comparison to apply with 'value'. |
| subject | String | self | (Optional) The subject of this filter test. |
| value | Decimal |  | (Required) A floating point value. |


> **operator** 子参数


| Options | Description |
| --- | --- |
| != | Test for inequality. |
| < | Test for less-than the value. |
| <= | Test for less-than or equal to the value. |
| <> | Test for inequality. |
| = | Test for equality. |
| == | Test for equality. |
| > | Test for greater-than the value. |
| >= | Test for greater-than or equal to the value. |
| equals | Test for equality. |
| not | Test for inequality. |


> **subject** 子参数


| Options | Description |
| --- | --- |
| block | The block involved with the interaction. |
| damager | The damaging actor involved with the interaction. |
| other | The other member of an interaction, not the caller. |
| parent | The caller's current parent. |
| player | The player involved with the interaction. |
| self | The entity or object calling the test |
| target | The caller's current target. |


## Examples


Full..


```json
{ "test": "distance_to_nearest_player", "subject": "self", "operator": "equals", "value": "0.00" }
```


Short (using Defaults)..


```json
{ "test": "distance_to_nearest_player", "value": "0.00" }
```


# enum_property


Returns true when the enum actor property matches the value provided.


| Name | Type | Default | Description |
| --- | --- | --- | --- |
| domain | String |  | (Required) The property name to look for |
| operator | String | equals | (Optional) The comparison to apply with 'value'. |
| subject | String | self | (Optional) The subject of this filter test. |
| value | String |  | (Required) A string value. |


> **operator** 子参数


| Options | Description |
| --- | --- |
| != | Test for inequality. |
| < | Test for less-than the value. |
| <= | Test for less-than or equal to the value. |
| <> | Test for inequality. |
| = | Test for equality. |
| == | Test for equality. |
| > | Test for greater-than the value. |
| >= | Test for greater-than or equal to the value. |
| equals | Test for equality. |
| not | Test for inequality. |


> **subject** 子参数


| Options | Description |
| --- | --- |
| block | The block involved with the interaction. |
| damager | The damaging actor involved with the interaction. |
| other | The other member of an interaction, not the caller. |
| parent | The caller's current parent. |
| player | The player involved with the interaction. |
| self | The entity or object calling the test |
| target | The caller's current target. |


## Examples


Full..


```json
{ "test": "enum_property", "subject": "self", "domain": "minecraft:can_climb", "operator": "equals", "value": "" }
```


Short (using Defaults)..


```json
{ "test": "enum_property", "domain": "minecraft:can_climb", "value": "" }
```


# float_property


Returns true when the float actor property matches the value provided.


| Name | Type | Default | Description |
| --- | --- | --- | --- |
| domain | String |  | (Required) The property name to look for |
| operator | String | equals | (Optional) The comparison to apply with 'value'. |
| subject | String | self | (Optional) The subject of this filter test. |
| value | Decimal |  | (Required) A floating point value. |


> **operator** 子参数


| Options | Description |
| --- | --- |
| != | Test for inequality. |
| < | Test for less-than the value. |
| <= | Test for less-than or equal to the value. |
| <> | Test for inequality. |
| = | Test for equality. |
| == | Test for equality. |
| > | Test for greater-than the value. |
| >= | Test for greater-than or equal to the value. |
| equals | Test for equality. |
| not | Test for inequality. |


> **subject** 子参数


| Options | Description |
| --- | --- |
| block | The block involved with the interaction. |
| damager | The damaging actor involved with the interaction. |
| other | The other member of an interaction, not the caller. |
| parent | The caller's current parent. |
| player | The player involved with the interaction. |
| self | The entity or object calling the test |
| target | The caller's current target. |


## Examples


Full..


```json
{ "test": "float_property", "subject": "self", "domain": "minecraft:can_climb", "operator": "equals", "value": "0.00" }
```


Short (using Defaults)..


```json
{ "test": "float_property", "domain": "minecraft:can_climb", "value": "0.00" }
```


# has_ability


Returns true when the subject entity has the named ability.


| Name | Type | Default | Description |
| --- | --- | --- | --- |
| operator | String | equals | (Optional) The comparison to apply with 'value'. |
| subject | String | self | (Optional) The subject of this filter test. |
| value | String |  | (Required) The Ability type to test |


> **operator** 子参数


| Options | Description |
| --- | --- |
| != | Test for inequality. |
| < | Test for less-than the value. |
| <= | Test for less-than or equal to the value. |
| <> | Test for inequality. |
| = | Test for equality. |
| == | Test for equality. |
| > | Test for greater-than the value. |
| >= | Test for greater-than or equal to the value. |
| equals | Test for equality. |
| not | Test for inequality. |


> **subject** 子参数


| Options | Description |
| --- | --- |
| block | The block involved with the interaction. |
| damager | The damaging actor involved with the interaction. |
| other | The other member of an interaction, not the caller. |
| parent | The caller's current parent. |
| player | The player involved with the interaction. |
| self | The entity or object calling the test |
| target | The caller's current target. |


> **value** 子参数


| Options | Description |
| --- | --- |
| flySpeed |  |
| flying |  |
| instabuild |  |
| invulnerable |  |
| lightning |  |
| mayfly |  |
| mute |  |
| noclip |  |
| verticalFlySpeed |  |
| walkSpeed |  |
| worldbuilder |  |


## Examples


Full..


```json
{ "test": "has_ability", "subject": "self", "operator": "equals", "value": "instabuild" }
```


Short (using Defaults)..


```json
{ "test": "has_ability", "value": "instabuild" }
```


# has_biome_tag


Tests whether the biome the subject is in has the specified tag.


| Name | Type | Default | Description |
| --- | --- | --- | --- |
| operator | String | equals | (Optional) The comparison to apply with 'value'. |
| subject | String | self | (Optional) The subject of this filter test. |
| value | String |  | (Required) The tag to look for |


> **operator** 子参数


| Options | Description |
| --- | --- |
| != | Test for inequality. |
| < | Test for less-than the value. |
| <= | Test for less-than or equal to the value. |
| <> | Test for inequality. |
| = | Test for equality. |
| == | Test for equality. |
| > | Test for greater-than the value. |
| >= | Test for greater-than or equal to the value. |
| equals | Test for equality. |
| not | Test for inequality. |


> **subject** 子参数


| Options | Description |
| --- | --- |
| block | The block involved with the interaction. |
| damager | The damaging actor involved with the interaction. |
| other | The other member of an interaction, not the caller. |
| parent | The caller's current parent. |
| player | The player involved with the interaction. |
| self | The entity or object calling the test |
| target | The caller's current target. |


## Examples


Full..


```json
{ "test": "has_biome_tag", "subject": "self", "operator": "equals", "value": " " }
```


Short (using Defaults)..


```json
{ "test": "has_biome_tag", "value": " " }
```


# has_component


Returns true when the subject entity contains the named component.


| Name | Type | Default | Description |
| --- | --- | --- | --- |
| operator | String | equals | (Optional) The comparison to apply with 'value'. |
| subject | String | self | (Optional) The subject of this filter test. |
| value | String |  | (Required) The component name to look for |


> **operator** 子参数


| Options | Description |
| --- | --- |
| != | Test for inequality. |
| < | Test for less-than the value. |
| <= | Test for less-than or equal to the value. |
| <> | Test for inequality. |
| = | Test for equality. |
| == | Test for equality. |
| > | Test for greater-than the value. |
| >= | Test for greater-than or equal to the value. |
| equals | Test for equality. |
| not | Test for inequality. |


> **subject** 子参数


| Options | Description |
| --- | --- |
| block | The block involved with the interaction. |
| damager | The damaging actor involved with the interaction. |
| other | The other member of an interaction, not the caller. |
| parent | The caller's current parent. |
| player | The player involved with the interaction. |
| self | The entity or object calling the test |
| target | The caller's current target. |


## Examples


Full..


```json
{ "test": "has_component", "subject": "self", "operator": "equals", "value": "minecraft:explode" }
```


Short (using Defaults)..


```json
{ "test": "has_component", "value": "minecraft:explode" }
```


# has_container_open


Returns true when the subject Player entity has opened a container.


| Name | Type | Default | Description |
| --- | --- | --- | --- |
| operator | String | equals | (Optional) The comparison to apply with 'value'. |
| subject | String | self | (Optional) The subject of this filter test. |
| value | Boolean | true | (Optional) true or false. |


> **operator** 子参数


| Options | Description |
| --- | --- |
| != | Test for inequality. |
| < | Test for less-than the value. |
| <= | Test for less-than or equal to the value. |
| <> | Test for inequality. |
| = | Test for equality. |
| == | Test for equality. |
| > | Test for greater-than the value. |
| >= | Test for greater-than or equal to the value. |
| equals | Test for equality. |
| not | Test for inequality. |


> **subject** 子参数


| Options | Description |
| --- | --- |
| block | The block involved with the interaction. |
| damager | The damaging actor involved with the interaction. |
| other | The other member of an interaction, not the caller. |
| parent | The caller's current parent. |
| player | The player involved with the interaction. |
| self | The entity or object calling the test |
| target | The caller's current target. |


## Examples


Full..


```json
{ "test": "has_container_open", "subject": "self", "operator": "equals", "value": "true" }
```


Short (using Defaults)..


```json
{ "test": "has_container_open" }
```


# has_damage


Returns true when the subject entity receives the named damage type.


| Name | Type | Default | Description |
| --- | --- | --- | --- |
| operator | String | equals | (Optional) The comparison to apply with 'value'. |
| subject | String | self | (Optional) The subject of this filter test. |
| value | String |  | (Required) The Damage type to test |


> **operator** 子参数


| Options | Description |
| --- | --- |
| != | Test for inequality. |
| < | Test for less-than the value. |
| <= | Test for less-than or equal to the value. |
| <> | Test for inequality. |
| = | Test for equality. |
| == | Test for equality. |
| > | Test for greater-than the value. |
| >= | Test for greater-than or equal to the value. |
| equals | Test for equality. |
| not | Test for inequality. |


> **subject** 子参数


| Options | Description |
| --- | --- |
| block | The block involved with the interaction. |
| damager | The damaging actor involved with the interaction. |
| other | The other member of an interaction, not the caller. |
| parent | The caller's current parent. |
| player | The player involved with the interaction. |
| self | The entity or object calling the test |
| target | The caller's current target. |


> **value** 子参数


| Options | Description |
| --- | --- |
| anvil |  |
| attack |  |
| block_explosion |  |
| contact |  |
| drowning |  |
| entity_explosion |  |
| fall |  |
| falling_block |  |
| fatal | Any damage which kills the subject |
| fire |  |
| fire_tick |  |
| fly_into_wall |  |
| lava |  |
| magic |  |
| none |  |
| override |  |
| piston |  |
| projectile |  |
| self_destruct |  |
| sonic_boom |  |
| stalactite |  |
| stalagmite |  |
| starve |  |
| suffocation |  |
| thorns |  |
| void |  |
| wither |  |


## Examples


Full..


```json
{ "test": "has_damage", "subject": "self", "operator": "equals", "value": "fatal" }
```


Short (using Defaults)..


```json
{ "test": "has_damage", "value": "fatal" }
```


# has_damaged_equipment


Tests for the presence of a damaged named item in the designated slot of the subject entity.


| Name | Type | Default | Description |
| --- | --- | --- | --- |
| domain | String | any | (Optional) The equipment location to test |
| operator | String | equals | (Optional) The comparison to apply with 'value'. |
| subject | String | self | (Optional) The subject of this filter test. |
| value | String |  | (Required) The item name to look for |


> **domain** 子参数


| Options | Description |
| --- | --- |
| any |  |
| armor |  |
| body |  |
| feet |  |
| hand |  |
| head |  |
| inventory |  |
| leg |  |
| main_hand |  |
| torso |  |


> **operator** 子参数


| Options | Description |
| --- | --- |
| != | Test for inequality. |
| < | Test for less-than the value. |
| <= | Test for less-than or equal to the value. |
| <> | Test for inequality. |
| = | Test for equality. |
| == | Test for equality. |
| > | Test for greater-than the value. |
| >= | Test for greater-than or equal to the value. |
| equals | Test for equality. |
| not | Test for inequality. |


> **subject** 子参数


| Options | Description |
| --- | --- |
| block | The block involved with the interaction. |
| damager | The damaging actor involved with the interaction. |
| other | The other member of an interaction, not the caller. |
| parent | The caller's current parent. |
| player | The player involved with the interaction. |
| self | The entity or object calling the test |
| target | The caller's current target. |


## Examples


Full..


```json
{ "test": "has_damaged_equipment", "subject": "self", "domain": "any", "operator": "equals", "value": "dirt" }
```


Short (using Defaults)..


```json
{ "test": "has_damaged_equipment", "value": "dirt" }
```


# has_equipment


Tests for the presence of a named item in the designated slot of the subject entity.


| Name | Type | Default | Description |
| --- | --- | --- | --- |
| domain | String | any | (Optional) The equipment location to test |
| operator | String | equals | (Optional) The comparison to apply with 'value'. |
| subject | String | self | (Optional) The subject of this filter test. |
| value | String |  | (Required) The item name to look for |


> **domain** 子参数


| Options | Description |
| --- | --- |
| any |  |
| armor |  |
| body |  |
| feet |  |
| hand |  |
| head |  |
| inventory |  |
| leg |  |
| main_hand |  |
| torso |  |


> **operator** 子参数


| Options | Description |
| --- | --- |
| != | Test for inequality. |
| < | Test for less-than the value. |
| <= | Test for less-than or equal to the value. |
| <> | Test for inequality. |
| = | Test for equality. |
| == | Test for equality. |
| > | Test for greater-than the value. |
| >= | Test for greater-than or equal to the value. |
| equals | Test for equality. |
| not | Test for inequality. |


> **subject** 子参数


| Options | Description |
| --- | --- |
| block | The block involved with the interaction. |
| damager | The damaging actor involved with the interaction. |
| other | The other member of an interaction, not the caller. |
| parent | The caller's current parent. |
| player | The player involved with the interaction. |
| self | The entity or object calling the test |
| target | The caller's current target. |


## Examples


Full..


```json
{ "test": "has_equipment", "subject": "self", "domain": "any", "operator": "equals", "value": "dirt" }
```


Short (using Defaults)..


```json
{ "test": "has_equipment", "value": "dirt" }
```


# has_equipment_tag


Tests for the presence of an item with the named tag in the designated slot of the subject entity.


| Name | Type | Default | Description |
| --- | --- | --- | --- |
| domain | String | any | (Optional) The equipment location to test |
| operator | String | equals | (Optional) The comparison to apply with 'value'. |
| subject | String | self | (Optional) The subject of this filter test. |
| value | String |  | (Required) The item name to look for |


> **domain** 子参数


| Options | Description |
| --- | --- |
| any |  |
| armor |  |
| body |  |
| feet |  |
| hand |  |
| head |  |
| inventory |  |
| leg |  |
| main_hand |  |
| torso |  |


> **operator** 子参数


| Options | Description |
| --- | --- |
| != | Test for inequality. |
| < | Test for less-than the value. |
| <= | Test for less-than or equal to the value. |
| <> | Test for inequality. |
| = | Test for equality. |
| == | Test for equality. |
| > | Test for greater-than the value. |
| >= | Test for greater-than or equal to the value. |
| equals | Test for equality. |
| not | Test for inequality. |


> **subject** 子参数


| Options | Description |
| --- | --- |
| block | The block involved with the interaction. |
| damager | The damaging actor involved with the interaction. |
| other | The other member of an interaction, not the caller. |
| parent | The caller's current parent. |
| player | The player involved with the interaction. |
| self | The entity or object calling the test |
| target | The caller's current target. |


## Examples


Full..


```json
{ "test": "has_equipment_tag", "subject": "self", "domain": "any", "operator": "equals", "value": "dirt" }
```


Short (using Defaults)..


```json
{ "test": "has_equipment_tag", "value": "dirt" }
```


# has_mob_effect


Tests whether the Subject has the specified mob effect.


| Name | Type | Default | Description |
| --- | --- | --- | --- |
| operator | String | equals | (Optional) The comparison to apply with 'value'. |
| subject | String | self | (Optional) The subject of this filter test. |
| value | String |  | (Optional) A string value. |


> **operator** 子参数


| Options | Description |
| --- | --- |
| != | Test for inequality. |
| < | Test for less-than the value. |
| <= | Test for less-than or equal to the value. |
| <> | Test for inequality. |
| = | Test for equality. |
| == | Test for equality. |
| > | Test for greater-than the value. |
| >= | Test for greater-than or equal to the value. |
| equals | Test for equality. |
| not | Test for inequality. |


> **subject** 子参数


| Options | Description |
| --- | --- |
| block | The block involved with the interaction. |
| damager | The damaging actor involved with the interaction. |
| other | The other member of an interaction, not the caller. |
| parent | The caller's current parent. |
| player | The player involved with the interaction. |
| self | The entity or object calling the test |
| target | The caller's current target. |


## Examples


Full..


```json
{ "test": "has_mob_effect", "subject": "self", "operator": "equals", "value": "" }
```


Short (using Defaults)..


```json
{ "test": "has_mob_effect" }
```


# has_nametag


Tests if the subject has been given a custom name.


| Name | Type | Default | Description |
| --- | --- | --- | --- |
| operator | String | equals | (Optional) The comparison to apply with 'value'. |
| subject | String | self | (Optional) The subject of this filter test. |
| value | Boolean | true | (Optional) true or false. |


> **operator** 子参数


| Options | Description |
| --- | --- |
| != | Test for inequality. |
| < | Test for less-than the value. |
| <= | Test for less-than or equal to the value. |
| <> | Test for inequality. |
| = | Test for equality. |
| == | Test for equality. |
| > | Test for greater-than the value. |
| >= | Test for greater-than or equal to the value. |
| equals | Test for equality. |
| not | Test for inequality. |


> **subject** 子参数


| Options | Description |
| --- | --- |
| block | The block involved with the interaction. |
| damager | The damaging actor involved with the interaction. |
| other | The other member of an interaction, not the caller. |
| parent | The caller's current parent. |
| player | The player involved with the interaction. |
| self | The entity or object calling the test |
| target | The caller's current target. |


## Examples


Full..


```json
{ "test": "has_nametag", "subject": "self", "operator": "equals", "value": "true" }
```


Short (using Defaults)..


```json
{ "test": "has_nametag" }
```


# has_property


Tests for the presence of a property of the subject entity.


| Name | Type | Default | Description |
| --- | --- | --- | --- |
| operator | String | equals | (Optional) The comparison to apply with 'value'. |
| subject | String | self | (Optional) The subject of this filter test. |
| value | String |  | (Required) The property name to look for |


> **operator** 子参数


| Options | Description |
| --- | --- |
| != | Test for inequality. |
| < | Test for less-than the value. |
| <= | Test for less-than or equal to the value. |
| <> | Test for inequality. |
| = | Test for equality. |
| == | Test for equality. |
| > | Test for greater-than the value. |
| >= | Test for greater-than or equal to the value. |
| equals | Test for equality. |
| not | Test for inequality. |


> **subject** 子参数


| Options | Description |
| --- | --- |
| block | The block involved with the interaction. |
| damager | The damaging actor involved with the interaction. |
| other | The other member of an interaction, not the caller. |
| parent | The caller's current parent. |
| player | The player involved with the interaction. |
| self | The entity or object calling the test |
| target | The caller's current target. |


## Examples


Full..


```json
{ "test": "has_property", "subject": "self", "operator": "equals", "value": "minecraft:can_climb" }
```


Short (using Defaults)..


```json
{ "test": "has_property", "value": "minecraft:can_climb" }
```


# has_ranged_weapon


Returns true when the subject entity is holding a ranged weapon like a bow or crossbow.


| Name | Type | Default | Description |
| --- | --- | --- | --- |
| operator | String | equals | (Optional) The comparison to apply with 'value'. |
| subject | String | self | (Optional) The subject of this filter test. |
| value | Boolean | true | (Optional) true or false. |


> **operator** 子参数


| Options | Description |
| --- | --- |
| != | Test for inequality. |
| < | Test for less-than the value. |
| <= | Test for less-than or equal to the value. |
| <> | Test for inequality. |
| = | Test for equality. |
| == | Test for equality. |
| > | Test for greater-than the value. |
| >= | Test for greater-than or equal to the value. |
| equals | Test for equality. |
| not | Test for inequality. |


> **subject** 子参数


| Options | Description |
| --- | --- |
| block | The block involved with the interaction. |
| damager | The damaging actor involved with the interaction. |
| other | The other member of an interaction, not the caller. |
| parent | The caller's current parent. |
| player | The player involved with the interaction. |
| self | The entity or object calling the test |
| target | The caller's current target. |


## Examples


Full..


```json
{ "test": "has_ranged_weapon", "subject": "self", "operator": "equals", "value": "true" }
```


Short (using Defaults)..


```json
{ "test": "has_ranged_weapon" }
```


# has_silk_touch


Tests if the subject is holding an item with silk touch.


| Name | Type | Default | Description |
| --- | --- | --- | --- |
| operator | String | equals | (Optional) The comparison to apply with 'value'. |
| subject | String | self | (Optional) The subject of this filter test. |
| value | Boolean | true | (Optional) true or false. |


> **operator** 子参数


| Options | Description |
| --- | --- |
| != | Test for inequality. |
| < | Test for less-than the value. |
| <= | Test for less-than or equal to the value. |
| <> | Test for inequality. |
| = | Test for equality. |
| == | Test for equality. |
| > | Test for greater-than the value. |
| >= | Test for greater-than or equal to the value. |
| equals | Test for equality. |
| not | Test for inequality. |


> **subject** 子参数


| Options | Description |
| --- | --- |
| block | The block involved with the interaction. |
| damager | The damaging actor involved with the interaction. |
| other | The other member of an interaction, not the caller. |
| parent | The caller's current parent. |
| player | The player involved with the interaction. |
| self | The entity or object calling the test |
| target | The caller's current target. |


## Examples


Full..


```json
{ "test": "has_silk_touch", "subject": "self", "operator": "equals", "value": "true" }
```


Short (using Defaults)..


```json
{ "test": "has_silk_touch" }
```


# has_tag


Returns true if the subject entity has the tag provided.


| Name | Type | Default | Description |
| --- | --- | --- | --- |
| operator | String | equals | (Optional) The comparison to apply with 'value'. |
| subject | String | self | (Optional) The subject of this filter test. |
| value | String |  | (Optional) A string value. |


> **operator** 子参数


| Options | Description |
| --- | --- |
| != | Test for inequality. |
| < | Test for less-than the value. |
| <= | Test for less-than or equal to the value. |
| <> | Test for inequality. |
| = | Test for equality. |
| == | Test for equality. |
| > | Test for greater-than the value. |
| >= | Test for greater-than or equal to the value. |
| equals | Test for equality. |
| not | Test for inequality. |


> **subject** 子参数


| Options | Description |
| --- | --- |
| block | The block involved with the interaction. |
| damager | The damaging actor involved with the interaction. |
| other | The other member of an interaction, not the caller. |
| parent | The caller's current parent. |
| player | The player involved with the interaction. |
| self | The entity or object calling the test |
| target | The caller's current target. |


## Examples


Full..


```json
{ "test": "has_tag", "subject": "self", "operator": "equals", "value": "" }
```


Short (using Defaults)..


```json
{ "test": "has_tag" }
```


# has_target


Returns true if the subject entity has a valid target.


| Name | Type | Default | Description |
| --- | --- | --- | --- |
| operator | String | equals | (Optional) The comparison to apply with 'value'. |
| subject | String | self | (Optional) The subject of this filter test. |
| value | Boolean | true | (Optional) true or false. |


> **operator** 子参数


| Options | Description |
| --- | --- |
| != | Test for inequality. |
| < | Test for less-than the value. |
| <= | Test for less-than or equal to the value. |
| <> | Test for inequality. |
| = | Test for equality. |
| == | Test for equality. |
| > | Test for greater-than the value. |
| >= | Test for greater-than or equal to the value. |
| equals | Test for equality. |
| not | Test for inequality. |


> **subject** 子参数


| Options | Description |
| --- | --- |
| block | The block involved with the interaction. |
| damager | The damaging actor involved with the interaction. |
| other | The other member of an interaction, not the caller. |
| parent | The caller's current parent. |
| player | The player involved with the interaction. |
| self | The entity or object calling the test |
| target | The caller's current target. |


## Examples


Full..


```json
{ "test": "has_target", "subject": "self", "operator": "equals", "value": "true" }
```


Short (using Defaults)..


```json
{ "test": "has_target" }
```


# has_trade_supply


Tests whether the target has any trade supply left. Will return false if the target cannot be traded with.


| Name | Type | Default | Description |
| --- | --- | --- | --- |
| operator | String | equals | (Optional) The comparison to apply with 'value'. |
| subject | String | self | (Optional) The subject of this filter test. |
| value | Boolean | true | (Optional) true or false. |


> **operator** 子参数


| Options | Description |
| --- | --- |
| != | Test for inequality. |
| < | Test for less-than the value. |
| <= | Test for less-than or equal to the value. |
| <> | Test for inequality. |
| = | Test for equality. |
| == | Test for equality. |
| > | Test for greater-than the value. |
| >= | Test for greater-than or equal to the value. |
| equals | Test for equality. |
| not | Test for inequality. |


> **subject** 子参数


| Options | Description |
| --- | --- |
| block | The block involved with the interaction. |
| damager | The damaging actor involved with the interaction. |
| other | The other member of an interaction, not the caller. |
| parent | The caller's current parent. |
| player | The player involved with the interaction. |
| self | The entity or object calling the test |
| target | The caller's current target. |


## Examples


Full..


```json
{ "test": "has_trade_supply", "subject": "self", "operator": "equals", "value": "true" }
```


Short (using Defaults)..


```json
{ "test": "has_trade_supply" }
```


# home_distance


Tests the distance between the subject and its home. Returns false if the subject has no home or if their home is in a different dimension.


| Name | Type | Default | Description |
| --- | --- | --- | --- |
| operator | String | equals | (Optional) The comparison to apply with 'value'. |
| subject | String | self | (Optional) The subject of this filter test. |
| value | Decimal |  | (Required) A floating point value. |


> **operator** 子参数


| Options | Description |
| --- | --- |
| != | Test for inequality. |
| < | Test for less-than the value. |
| <= | Test for less-than or equal to the value. |
| <> | Test for inequality. |
| = | Test for equality. |
| == | Test for equality. |
| > | Test for greater-than the value. |
| >= | Test for greater-than or equal to the value. |
| equals | Test for equality. |
| not | Test for inequality. |


> **subject** 子参数


| Options | Description |
| --- | --- |
| block | The block involved with the interaction. |
| damager | The damaging actor involved with the interaction. |
| other | The other member of an interaction, not the caller. |
| parent | The caller's current parent. |
| player | The player involved with the interaction. |
| self | The entity or object calling the test |
| target | The caller's current target. |


## Examples


Full..


```json
{ "test": "home_distance", "subject": "self", "operator": "equals", "value": "0.00" }
```


Short (using Defaults)..


```json
{ "test": "home_distance", "value": "0.00" }
```


# hourly_clock_time


Compares the current 24 hour time with an int value in the range[0, 24000]


| Name | Type | Default | Description |
| --- | --- | --- | --- |
| operator | String | equals | (Optional) The comparison to apply with 'value'. |
| subject | String | self | (Optional) The subject of this filter test. |
| value | Integer |  | (Required) An integer value. |


> **operator** 子参数


| Options | Description |
| --- | --- |
| != | Test for inequality. |
| < | Test for less-than the value. |
| <= | Test for less-than or equal to the value. |
| <> | Test for inequality. |
| = | Test for equality. |
| == | Test for equality. |
| > | Test for greater-than the value. |
| >= | Test for greater-than or equal to the value. |
| equals | Test for equality. |
| not | Test for inequality. |


> **subject** 子参数


| Options | Description |
| --- | --- |
| block | The block involved with the interaction. |
| damager | The damaging actor involved with the interaction. |
| other | The other member of an interaction, not the caller. |
| parent | The caller's current parent. |
| player | The player involved with the interaction. |
| self | The entity or object calling the test |
| target | The caller's current target. |


## Examples


Full..


```json
{ "test": "hourly_clock_time", "subject": "self", "operator": "equals", "value": "0" }
```


Short (using Defaults)..


```json
{ "test": "hourly_clock_time", "value": "0" }
```


# in_block


Returns true when the subject entity is inside a specified Block type.


| Name | Type | Default | Description |
| --- | --- | --- | --- |
| operator | String | equals | (Optional) The comparison to apply with 'value'. |
| subject | String | self | (Optional) The subject of this filter test. |
| value | String |  | (Optional) A string value. |


> **operator** 子参数


| Options | Description |
| --- | --- |
| != | Test for inequality. |
| < | Test for less-than the value. |
| <= | Test for less-than or equal to the value. |
| <> | Test for inequality. |
| = | Test for equality. |
| == | Test for equality. |
| > | Test for greater-than the value. |
| >= | Test for greater-than or equal to the value. |
| equals | Test for equality. |
| not | Test for inequality. |


> **subject** 子参数


| Options | Description |
| --- | --- |
| block | The block involved with the interaction. |
| damager | The damaging actor involved with the interaction. |
| other | The other member of an interaction, not the caller. |
| parent | The caller's current parent. |
| player | The player involved with the interaction. |
| self | The entity or object calling the test |
| target | The caller's current target. |


## Examples


Full..


```json
{ "test": "in_block", "subject": "self", "operator": "equals", "value": "" }
```


Short (using Defaults)..


```json
{ "test": "in_block" }
```


# in_caravan


Returns true if the subject entity is in a caravan.


| Name | Type | Default | Description |
| --- | --- | --- | --- |
| operator | String | equals | (Optional) The comparison to apply with 'value'. |
| subject | String | self | (Optional) The subject of this filter test. |
| value | Boolean | true | (Optional) true or false. |


> **operator** 子参数


| Options | Description |
| --- | --- |
| != | Test for inequality. |
| < | Test for less-than the value. |
| <= | Test for less-than or equal to the value. |
| <> | Test for inequality. |
| = | Test for equality. |
| == | Test for equality. |
| > | Test for greater-than the value. |
| >= | Test for greater-than or equal to the value. |
| equals | Test for equality. |
| not | Test for inequality. |


> **subject** 子参数


| Options | Description |
| --- | --- |
| block | The block involved with the interaction. |
| damager | The damaging actor involved with the interaction. |
| other | The other member of an interaction, not the caller. |
| parent | The caller's current parent. |
| player | The player involved with the interaction. |
| self | The entity or object calling the test |
| target | The caller's current target. |


## Examples


Full..


```json
{ "test": "in_caravan", "subject": "self", "operator": "equals", "value": "true" }
```


Short (using Defaults)..


```json
{ "test": "in_caravan" }
```


# in_clouds


Returns true when the subject entity is in the clouds.


| Name | Type | Default | Description |
| --- | --- | --- | --- |
| operator | String | equals | (Optional) The comparison to apply with 'value'. |
| subject | String | self | (Optional) The subject of this filter test. |
| value | Boolean | true | (Optional) true or false. |


> **operator** 子参数


| Options | Description |
| --- | --- |
| != | Test for inequality. |
| < | Test for less-than the value. |
| <= | Test for less-than or equal to the value. |
| <> | Test for inequality. |
| = | Test for equality. |
| == | Test for equality. |
| > | Test for greater-than the value. |
| >= | Test for greater-than or equal to the value. |
| equals | Test for equality. |
| not | Test for inequality. |


> **subject** 子参数


| Options | Description |
| --- | --- |
| block | The block involved with the interaction. |
| damager | The damaging actor involved with the interaction. |
| other | The other member of an interaction, not the caller. |
| parent | The caller's current parent. |
| player | The player involved with the interaction. |
| self | The entity or object calling the test |
| target | The caller's current target. |


## Examples


Full..


```json
{ "test": "in_clouds", "subject": "self", "operator": "equals", "value": "true" }
```


Short (using Defaults)..


```json
{ "test": "in_clouds" }
```


# in_contact_with_water


Returns true when the subject entity in contact with any water: water, rain, splash water bottle.


| Name | Type | Default | Description |
| --- | --- | --- | --- |
| operator | String | equals | (Optional) The comparison to apply with 'value'. |
| subject | String | self | (Optional) The subject of this filter test. |
| value | Boolean | true | (Optional) true or false. |


> **operator** 子参数


| Options | Description |
| --- | --- |
| != | Test for inequality. |
| < | Test for less-than the value. |
| <= | Test for less-than or equal to the value. |
| <> | Test for inequality. |
| = | Test for equality. |
| == | Test for equality. |
| > | Test for greater-than the value. |
| >= | Test for greater-than or equal to the value. |
| equals | Test for equality. |
| not | Test for inequality. |


> **subject** 子参数


| Options | Description |
| --- | --- |
| block | The block involved with the interaction. |
| damager | The damaging actor involved with the interaction. |
| other | The other member of an interaction, not the caller. |
| parent | The caller's current parent. |
| player | The player involved with the interaction. |
| self | The entity or object calling the test |
| target | The caller's current target. |


## Examples


Full..


```json
{ "test": "in_contact_with_water", "subject": "self", "operator": "equals", "value": "true" }
```


Short (using Defaults)..


```json
{ "test": "in_contact_with_water" }
```


# in_lava


Returns true when the subject entity is in lava.


| Name | Type | Default | Description |
| --- | --- | --- | --- |
| operator | String | equals | (Optional) The comparison to apply with 'value'. |
| subject | String | self | (Optional) The subject of this filter test. |
| value | Boolean | true | (Optional) true or false. |


> **operator** 子参数


| Options | Description |
| --- | --- |
| != | Test for inequality. |
| < | Test for less-than the value. |
| <= | Test for less-than or equal to the value. |
| <> | Test for inequality. |
| = | Test for equality. |
| == | Test for equality. |
| > | Test for greater-than the value. |
| >= | Test for greater-than or equal to the value. |
| equals | Test for equality. |
| not | Test for inequality. |


> **subject** 子参数


| Options | Description |
| --- | --- |
| block | The block involved with the interaction. |
| damager | The damaging actor involved with the interaction. |
| other | The other member of an interaction, not the caller. |
| parent | The caller's current parent. |
| player | The player involved with the interaction. |
| self | The entity or object calling the test |
| target | The caller's current target. |


## Examples


Full..


```json
{ "test": "in_lava", "subject": "self", "operator": "equals", "value": "true" }
```


Short (using Defaults)..


```json
{ "test": "in_lava" }
```


# in_nether


Returns true when the subject entity is in Nether.


| Name | Type | Default | Description |
| --- | --- | --- | --- |
| operator | String | equals | (Optional) The comparison to apply with 'value'. |
| subject | String | self | (Optional) The subject of this filter test. |
| value | Boolean | true | (Optional) true or false. |


> **operator** 子参数


| Options | Description |
| --- | --- |
| != | Test for inequality. |
| < | Test for less-than the value. |
| <= | Test for less-than or equal to the value. |
| <> | Test for inequality. |
| = | Test for equality. |
| == | Test for equality. |
| > | Test for greater-than the value. |
| >= | Test for greater-than or equal to the value. |
| equals | Test for equality. |
| not | Test for inequality. |


> **subject** 子参数


| Options | Description |
| --- | --- |
| block | The block involved with the interaction. |
| damager | The damaging actor involved with the interaction. |
| other | The other member of an interaction, not the caller. |
| parent | The caller's current parent. |
| player | The player involved with the interaction. |
| self | The entity or object calling the test |
| target | The caller's current target. |


## Examples


Full..


```json
{ "test": "in_nether", "subject": "self", "operator": "equals", "value": "true" }
```


Short (using Defaults)..


```json
{ "test": "in_nether" }
```


# in_overworld


Returns true when the subject entity is in Overworld.


| Name | Type | Default | Description |
| --- | --- | --- | --- |
| operator | String | equals | (Optional) The comparison to apply with 'value'. |
| subject | String | self | (Optional) The subject of this filter test. |
| value | Boolean | true | (Optional) true or false. |


> **operator** 子参数


| Options | Description |
| --- | --- |
| != | Test for inequality. |
| < | Test for less-than the value. |
| <= | Test for less-than or equal to the value. |
| <> | Test for inequality. |
| = | Test for equality. |
| == | Test for equality. |
| > | Test for greater-than the value. |
| >= | Test for greater-than or equal to the value. |
| equals | Test for equality. |
| not | Test for inequality. |


> **subject** 子参数


| Options | Description |
| --- | --- |
| block | The block involved with the interaction. |
| damager | The damaging actor involved with the interaction. |
| other | The other member of an interaction, not the caller. |
| parent | The caller's current parent. |
| player | The player involved with the interaction. |
| self | The entity or object calling the test |
| target | The caller's current target. |


## Examples


Full..


```json
{ "test": "in_overworld", "subject": "self", "operator": "equals", "value": "true" }
```


Short (using Defaults)..


```json
{ "test": "in_overworld" }
```


# in_water


Returns true when the subject entity is in water.


| Name | Type | Default | Description |
| --- | --- | --- | --- |
| operator | String | equals | (Optional) The comparison to apply with 'value'. |
| subject | String | self | (Optional) The subject of this filter test. |
| value | Boolean | true | (Optional) true or false. |


> **operator** 子参数


| Options | Description |
| --- | --- |
| != | Test for inequality. |
| < | Test for less-than the value. |
| <= | Test for less-than or equal to the value. |
| <> | Test for inequality. |
| = | Test for equality. |
| == | Test for equality. |
| > | Test for greater-than the value. |
| >= | Test for greater-than or equal to the value. |
| equals | Test for equality. |
| not | Test for inequality. |


> **subject** 子参数


| Options | Description |
| --- | --- |
| block | The block involved with the interaction. |
| damager | The damaging actor involved with the interaction. |
| other | The other member of an interaction, not the caller. |
| parent | The caller's current parent. |
| player | The player involved with the interaction. |
| self | The entity or object calling the test |
| target | The caller's current target. |


## Examples


Full..


```json
{ "test": "in_water", "subject": "self", "operator": "equals", "value": "true" }
```


Short (using Defaults)..


```json
{ "test": "in_water" }
```


# in_water_or_rain


Returns true when the subject entity is in water or rain.


| Name | Type | Default | Description |
| --- | --- | --- | --- |
| operator | String | equals | (Optional) The comparison to apply with 'value'. |
| subject | String | self | (Optional) The subject of this filter test. |
| value | Boolean | true | (Optional) true or false. |


> **operator** 子参数


| Options | Description |
| --- | --- |
| != | Test for inequality. |
| < | Test for less-than the value. |
| <= | Test for less-than or equal to the value. |
| <> | Test for inequality. |
| = | Test for equality. |
| == | Test for equality. |
| > | Test for greater-than the value. |
| >= | Test for greater-than or equal to the value. |
| equals | Test for equality. |
| not | Test for inequality. |


> **subject** 子参数


| Options | Description |
| --- | --- |
| block | The block involved with the interaction. |
| damager | The damaging actor involved with the interaction. |
| other | The other member of an interaction, not the caller. |
| parent | The caller's current parent. |
| player | The player involved with the interaction. |
| self | The entity or object calling the test |
| target | The caller's current target. |


## Examples


Full..


```json
{ "test": "in_water_or_rain", "subject": "self", "operator": "equals", "value": "true" }
```


Short (using Defaults)..


```json
{ "test": "in_water_or_rain" }
```


# inactivity_timer


Tests if the specified duration in seconds of inactivity for despawning has been reached.


| Name | Type | Default | Description |
| --- | --- | --- | --- |
| operator | String | equals | (Optional) The comparison to apply with 'value'. |
| subject | String | self | (Optional) The subject of this filter test. |
| value | Integer |  | (Required) An integer value. |


> **operator** 子参数


| Options | Description |
| --- | --- |
| != | Test for inequality. |
| < | Test for less-than the value. |
| <= | Test for less-than or equal to the value. |
| <> | Test for inequality. |
| = | Test for equality. |
| == | Test for equality. |
| > | Test for greater-than the value. |
| >= | Test for greater-than or equal to the value. |
| equals | Test for equality. |
| not | Test for inequality. |


> **subject** 子参数


| Options | Description |
| --- | --- |
| block | The block involved with the interaction. |
| damager | The damaging actor involved with the interaction. |
| other | The other member of an interaction, not the caller. |
| parent | The caller's current parent. |
| player | The player involved with the interaction. |
| self | The entity or object calling the test |
| target | The caller's current target. |


## Examples


Full..


```json
{ "test": "inactivity_timer", "subject": "self", "operator": "equals", "value": "0" }
```


Short (using Defaults)..


```json
{ "test": "inactivity_timer", "value": "0" }
```


# int_property


Returns true when the int actor property matches the value provided.


| Name | Type | Default | Description |
| --- | --- | --- | --- |
| domain | String |  | (Required) The property name to look for |
| operator | String | equals | (Optional) The comparison to apply with 'value'. |
| subject | String | self | (Optional) The subject of this filter test. |
| value | Integer |  | (Required) An integer value. |


> **operator** 子参数


| Options | Description |
| --- | --- |
| != | Test for inequality. |
| < | Test for less-than the value. |
| <= | Test for less-than or equal to the value. |
| <> | Test for inequality. |
| = | Test for equality. |
| == | Test for equality. |
| > | Test for greater-than the value. |
| >= | Test for greater-than or equal to the value. |
| equals | Test for equality. |
| not | Test for inequality. |


> **subject** 子参数


| Options | Description |
| --- | --- |
| block | The block involved with the interaction. |
| damager | The damaging actor involved with the interaction. |
| other | The other member of an interaction, not the caller. |
| parent | The caller's current parent. |
| player | The player involved with the interaction. |
| self | The entity or object calling the test |
| target | The caller's current target. |


## Examples


Full..


```json
{ "test": "int_property", "subject": "self", "domain": "minecraft:can_climb", "operator": "equals", "value": "0" }
```


Short (using Defaults)..


```json
{ "test": "int_property", "domain": "minecraft:can_climb", "value": "0" }
```


# is_altitude


Tests the current altitude against a provided value. 0= bedrock elevation.


| Name | Type | Default | Description |
| --- | --- | --- | --- |
| operator | String | equals | (Optional) The comparison to apply with 'value'. |
| subject | String | self | (Optional) The subject of this filter test. |
| value | Integer |  | (Required) The altitude value to compare with |


> **operator** 子参数


| Options | Description |
| --- | --- |
| != | Test for inequality. |
| < | Test for less-than the value. |
| <= | Test for less-than or equal to the value. |
| <> | Test for inequality. |
| = | Test for equality. |
| == | Test for equality. |
| > | Test for greater-than the value. |
| >= | Test for greater-than or equal to the value. |
| equals | Test for equality. |
| not | Test for inequality. |


> **subject** 子参数


| Options | Description |
| --- | --- |
| block | The block involved with the interaction. |
| damager | The damaging actor involved with the interaction. |
| other | The other member of an interaction, not the caller. |
| parent | The caller's current parent. |
| player | The player involved with the interaction. |
| self | The entity or object calling the test |
| target | The caller's current target. |


## Examples


Full..


```json
{ "test": "is_altitude", "subject": "self", "operator": "equals", "value": "0" }
```


Short (using Defaults)..


```json
{ "test": "is_altitude", "value": "0" }
```


# is_avoiding_mobs


Returns true if the subject entity is fleeing from other mobs.


| Name | Type | Default | Description |
| --- | --- | --- | --- |
| operator | String | equals | (Optional) The comparison to apply with 'value'. |
| subject | String | self | (Optional) The subject of this filter test. |
| value | Boolean | true | (Optional) true or false. |


> **operator** 子参数


| Options | Description |
| --- | --- |
| != | Test for inequality. |
| < | Test for less-than the value. |
| <= | Test for less-than or equal to the value. |
| <> | Test for inequality. |
| = | Test for equality. |
| == | Test for equality. |
| > | Test for greater-than the value. |
| >= | Test for greater-than or equal to the value. |
| equals | Test for equality. |
| not | Test for inequality. |


> **subject** 子参数


| Options | Description |
| --- | --- |
| block | The block involved with the interaction. |
| damager | The damaging actor involved with the interaction. |
| other | The other member of an interaction, not the caller. |
| parent | The caller's current parent. |
| player | The player involved with the interaction. |
| self | The entity or object calling the test |
| target | The caller's current target. |


## Examples


Full..


```json
{ "test": "is_avoiding_mobs", "subject": "self", "operator": "equals", "value": "true" }
```


Short (using Defaults)..


```json
{ "test": "is_avoiding_mobs" }
```


# is_baby


Returns true when the subject entity is a baby.


| Name | Type | Default | Description |
| --- | --- | --- | --- |
| operator | String | equals | (Optional) The comparison to apply with 'value'. |
| subject | String | self | (Optional) The subject of this filter test. |
| value | Boolean | true | (Optional) true or false. |


> **operator** 子参数


| Options | Description |
| --- | --- |
| != | Test for inequality. |
| < | Test for less-than the value. |
| <= | Test for less-than or equal to the value. |
| <> | Test for inequality. |
| = | Test for equality. |
| == | Test for equality. |
| > | Test for greater-than the value. |
| >= | Test for greater-than or equal to the value. |
| equals | Test for equality. |
| not | Test for inequality. |


> **subject** 子参数


| Options | Description |
| --- | --- |
| block | The block involved with the interaction. |
| damager | The damaging actor involved with the interaction. |
| other | The other member of an interaction, not the caller. |
| parent | The caller's current parent. |
| player | The player involved with the interaction. |
| self | The entity or object calling the test |
| target | The caller's current target. |


## Examples


Full..


```json
{ "test": "is_baby", "subject": "self", "operator": "equals", "value": "true" }
```


Short (using Defaults)..


```json
{ "test": "is_baby" }
```


# is_biome


Tests whether the Subject is currently in the named biome.


| Name | Type | Default | Description |
| --- | --- | --- | --- |
| operator | String | equals | (Optional) The comparison to apply with 'value'. |
| subject | String | self | (Optional) The subject of this filter test. |
| value | String |  | (Required) The Biome type to test |


> **operator** 子参数


| Options | Description |
| --- | --- |
| != | Test for inequality. |
| < | Test for less-than the value. |
| <= | Test for less-than or equal to the value. |
| <> | Test for inequality. |
| = | Test for equality. |
| == | Test for equality. |
| > | Test for greater-than the value. |
| >= | Test for greater-than or equal to the value. |
| equals | Test for equality. |
| not | Test for inequality. |


> **subject** 子参数


| Options | Description |
| --- | --- |
| block | The block involved with the interaction. |
| damager | The damaging actor involved with the interaction. |
| other | The other member of an interaction, not the caller. |
| parent | The caller's current parent. |
| player | The player involved with the interaction. |
| self | The entity or object calling the test |
| target | The caller's current target. |


> **value** 子参数


| Options | Description |
| --- | --- |
| beach |  |
| desert |  |
| extreme_hills |  |
| flat |  |
| forest |  |
| ice |  |
| jungle |  |
| mesa |  |
| mushroom_island |  |
| ocean |  |
| plain |  |
| river |  |
| savanna |  |
| stone_beach |  |
| swamp |  |
| taiga |  |
| the_end |  |
| the_nether |  |


## Examples


Full..


```json
{ "test": "is_biome", "subject": "self", "operator": "equals", "value": "beach" }
```


Short (using Defaults)..


```json
{ "test": "is_biome", "value": "beach" }
```


# is_block


Returns true when the block has the given name.


| Name | Type | Default | Description |
| --- | --- | --- | --- |
| operator | String | equals | (Optional) The comparison to apply with 'value'. |
| subject | String | self | (Optional) The subject of this filter test. |
| value | String |  | (Required) The Family name to look for |


> **operator** 子参数


| Options | Description |
| --- | --- |
| != | Test for inequality. |
| < | Test for less-than the value. |
| <= | Test for less-than or equal to the value. |
| <> | Test for inequality. |
| = | Test for equality. |
| == | Test for equality. |
| > | Test for greater-than the value. |
| >= | Test for greater-than or equal to the value. |
| equals | Test for equality. |
| not | Test for inequality. |


> **subject** 子参数


| Options | Description |
| --- | --- |
| block | The block involved with the interaction. |
| damager | The damaging actor involved with the interaction. |
| other | The other member of an interaction, not the caller. |
| parent | The caller's current parent. |
| player | The player involved with the interaction. |
| self | The entity or object calling the test |
| target | The caller's current target. |


## Examples


Full..


```json
{ "test": "is_block", "subject": "self", "operator": "equals", "value": "player" }
```


Short (using Defaults)..


```json
{ "test": "is_block", "value": "player" }
```


# is_bound_to_creaking_heart


Tests that the Creaking Heart that spawned the subject Creaking still exists.


| Name | Type | Default | Description |
| --- | --- | --- | --- |
| operator | String | equals | (Optional) The comparison to apply with 'value'. |
| subject | String | self | (Optional) The subject of this filter test. |
| value | Boolean | true | (Optional) true or false. |


> **operator** 子参数


| Options | Description |
| --- | --- |
| != | Test for inequality. |
| < | Test for less-than the value. |
| <= | Test for less-than or equal to the value. |
| <> | Test for inequality. |
| = | Test for equality. |
| == | Test for equality. |
| > | Test for greater-than the value. |
| >= | Test for greater-than or equal to the value. |
| equals | Test for equality. |
| not | Test for inequality. |


> **subject** 子参数


| Options | Description |
| --- | --- |
| block | The block involved with the interaction. |
| damager | The damaging actor involved with the interaction. |
| other | The other member of an interaction, not the caller. |
| parent | The caller's current parent. |
| player | The player involved with the interaction. |
| self | The entity or object calling the test |
| target | The caller's current target. |


## Examples


Full..


```json
{ "test": "is_bound_to_creaking_heart", "subject": "self", "operator": "equals", "value": "true" }
```


Short (using Defaults)..


```json
{ "test": "is_bound_to_creaking_heart" }
```


# is_brightness


Tests the current brightness against a provided value in the range (0.0f, 1.0f).


| Name | Type | Default | Description |
| --- | --- | --- | --- |
| operator | String | equals | (Optional) The comparison to apply with 'value'. |
| subject | String | self | (Optional) The subject of this filter test. |
| value | Decimal |  | (Required) The brightness value to compare with. |


> **operator** 子参数


| Options | Description |
| --- | --- |
| != | Test for inequality. |
| < | Test for less-than the value. |
| <= | Test for less-than or equal to the value. |
| <> | Test for inequality. |
| = | Test for equality. |
| == | Test for equality. |
| > | Test for greater-than the value. |
| >= | Test for greater-than or equal to the value. |
| equals | Test for equality. |
| not | Test for inequality. |


> **subject** 子参数


| Options | Description |
| --- | --- |
| block | The block involved with the interaction. |
| damager | The damaging actor involved with the interaction. |
| other | The other member of an interaction, not the caller. |
| parent | The caller's current parent. |
| player | The player involved with the interaction. |
| self | The entity or object calling the test |
| target | The caller's current target. |


## Examples


Full..


```json
{ "test": "is_brightness", "subject": "self", "operator": "equals", "value": "0.50" }
```


Short (using Defaults)..


```json
{ "test": "is_brightness", "value": "0.50" }
```


# is_climbing


Returns true if the subject entity is climbing.


| Name | Type | Default | Description |
| --- | --- | --- | --- |
| operator | String | equals | (Optional) The comparison to apply with 'value'. |
| subject | String | self | (Optional) The subject of this filter test. |
| value | Boolean | true | (Optional) true or false. |


> **operator** 子参数


| Options | Description |
| --- | --- |
| != | Test for inequality. |
| < | Test for less-than the value. |
| <= | Test for less-than or equal to the value. |
| <> | Test for inequality. |
| = | Test for equality. |
| == | Test for equality. |
| > | Test for greater-than the value. |
| >= | Test for greater-than or equal to the value. |
| equals | Test for equality. |
| not | Test for inequality. |


> **subject** 子参数


| Options | Description |
| --- | --- |
| block | The block involved with the interaction. |
| damager | The damaging actor involved with the interaction. |
| other | The other member of an interaction, not the caller. |
| parent | The caller's current parent. |
| player | The player involved with the interaction. |
| self | The entity or object calling the test |
| target | The caller's current target. |


## Examples


Full..


```json
{ "test": "is_climbing", "subject": "self", "operator": "equals", "value": "true" }
```


Short (using Defaults)..


```json
{ "test": "is_climbing" }
```


# is_color


Returns true if the subject entity is the named color.


| Name | Type | Default | Description |
| --- | --- | --- | --- |
| operator | String | equals | (Optional) The comparison to apply with 'value'. |
| subject | String | self | (Optional) The subject of this filter test. |
| value | String |  | (Required) The Palette Color to test |


> **operator** 子参数


| Options | Description |
| --- | --- |
| != | Test for inequality. |
| < | Test for less-than the value. |
| <= | Test for less-than or equal to the value. |
| <> | Test for inequality. |
| = | Test for equality. |
| == | Test for equality. |
| > | Test for greater-than the value. |
| >= | Test for greater-than or equal to the value. |
| equals | Test for equality. |
| not | Test for inequality. |


> **subject** 子参数


| Options | Description |
| --- | --- |
| block | The block involved with the interaction. |
| damager | The damaging actor involved with the interaction. |
| other | The other member of an interaction, not the caller. |
| parent | The caller's current parent. |
| player | The player involved with the interaction. |
| self | The entity or object calling the test |
| target | The caller's current target. |


> **value** 子参数


| Options | Description |
| --- | --- |
| black |  |
| blue |  |
| brown |  |
| cyan |  |
| gray |  |
| green |  |
| light_blue |  |
| light_green |  |
| magenta |  |
| orange |  |
| pink |  |
| purple |  |
| red |  |
| silver |  |
| white |  |
| yellow |  |


## Examples


Full..


```json
{ "test": "is_color", "subject": "self", "operator": "equals", "value": "white" }
```


Short (using Defaults)..


```json
{ "test": "is_color", "value": "white" }
```


# is_daytime


Returns true during the daylight hours.


| Name | Type | Default | Description |
| --- | --- | --- | --- |
| operator | String | equals | (Optional) The comparison to apply with 'value'. |
| subject | String | self | (Optional) The subject of this filter test. |
| value | Boolean | true | (Optional) true or false. |


> **operator** 子参数


| Options | Description |
| --- | --- |
| != | Test for inequality. |
| < | Test for less-than the value. |
| <= | Test for less-than or equal to the value. |
| <> | Test for inequality. |
| = | Test for equality. |
| == | Test for equality. |
| > | Test for greater-than the value. |
| >= | Test for greater-than or equal to the value. |
| equals | Test for equality. |
| not | Test for inequality. |


> **subject** 子参数


| Options | Description |
| --- | --- |
| block | The block involved with the interaction. |
| damager | The damaging actor involved with the interaction. |
| other | The other member of an interaction, not the caller. |
| parent | The caller's current parent. |
| player | The player involved with the interaction. |
| self | The entity or object calling the test |
| target | The caller's current target. |


## Examples


Full..


```json
{ "test": "is_daytime", "subject": "self", "operator": "equals", "value": "true" }
```


Short (using Defaults)..


```json
{ "test": "is_daytime" }
```


# is_difficulty


Tests the current difficulty level of the game.


| Name | Type | Default | Description |
| --- | --- | --- | --- |
| operator | String | equals | (Optional) The comparison to apply with 'value'. |
| subject | String | self | (Optional) The subject of this filter test. |
| value | String |  | (Required) The game's difficulty level to test |


> **operator** 子参数


| Options | Description |
| --- | --- |
| != | Test for inequality. |
| < | Test for less-than the value. |
| <= | Test for less-than or equal to the value. |
| <> | Test for inequality. |
| = | Test for equality. |
| == | Test for equality. |
| > | Test for greater-than the value. |
| >= | Test for greater-than or equal to the value. |
| equals | Test for equality. |
| not | Test for inequality. |


> **subject** 子参数


| Options | Description |
| --- | --- |
| block | The block involved with the interaction. |
| damager | The damaging actor involved with the interaction. |
| other | The other member of an interaction, not the caller. |
| parent | The caller's current parent. |
| player | The player involved with the interaction. |
| self | The entity or object calling the test |
| target | The caller's current target. |


> **value** 子参数


| Options | Description |
| --- | --- |
| easy |  |
| hard |  |
| normal |  |
| peaceful |  |


## Examples


Full..


```json
{ "test": "is_difficulty", "subject": "self", "operator": "equals", "value": "normal" }
```


Short (using Defaults)..


```json
{ "test": "is_difficulty", "value": "normal" }
```


# is_family


Returns true when the subject entity is a member of the named family.


| Name | Type | Default | Description |
| --- | --- | --- | --- |
| operator | String | equals | (Optional) The comparison to apply with 'value'. |
| subject | String | self | (Optional) The subject of this filter test. |
| value | String |  | (Required) The Family name to look for |


> **operator** 子参数


| Options | Description |
| --- | --- |
| != | Test for inequality. |
| < | Test for less-than the value. |
| <= | Test for less-than or equal to the value. |
| <> | Test for inequality. |
| = | Test for equality. |
| == | Test for equality. |
| > | Test for greater-than the value. |
| >= | Test for greater-than or equal to the value. |
| equals | Test for equality. |
| not | Test for inequality. |


> **subject** 子参数


| Options | Description |
| --- | --- |
| block | The block involved with the interaction. |
| damager | The damaging actor involved with the interaction. |
| other | The other member of an interaction, not the caller. |
| parent | The caller's current parent. |
| player | The player involved with the interaction. |
| self | The entity or object calling the test |
| target | The caller's current target. |


## Examples


Full..


```json
{ "test": "is_family", "subject": "self", "operator": "equals", "value": "player" }
```


Short (using Defaults)..


```json
{ "test": "is_family", "value": "player" }
```


# is_game_rule


Tests whether a named game rule is active.


| Name | Type | Default | Description |
| --- | --- | --- | --- |
| domain | String |  | (Required) The Game Rule to test. |
| operator | String | equals | (Optional) The comparison to apply with 'value'. |
| subject | String | self | (Optional) The subject of this filter test. |
| value | Boolean | true | (Optional) true or false. |


> **operator** 子参数


| Options | Description |
| --- | --- |
| != | Test for inequality. |
| < | Test for less-than the value. |
| <= | Test for less-than or equal to the value. |
| <> | Test for inequality. |
| = | Test for equality. |
| == | Test for equality. |
| > | Test for greater-than the value. |
| >= | Test for greater-than or equal to the value. |
| equals | Test for equality. |
| not | Test for inequality. |


> **subject** 子参数


| Options | Description |
| --- | --- |
| block | The block involved with the interaction. |
| damager | The damaging actor involved with the interaction. |
| other | The other member of an interaction, not the caller. |
| parent | The caller's current parent. |
| player | The player involved with the interaction. |
| self | The entity or object calling the test |
| target | The caller's current target. |


## Examples


Full..


```json
{ "test": "is_game_rule", "subject": "self", "domain": "domobspawning", "operator": "equals", "value": "true" }
```


Short (using Defaults)..


```json
{ "test": "is_game_rule", "domain": "domobspawning" }
```


# is_humid


Tests whether the Subject is in an area with humidity


| Name | Type | Default | Description |
| --- | --- | --- | --- |
| operator | String | equals | (Optional) The comparison to apply with 'value'. |
| subject | String | self | (Optional) The subject of this filter test. |
| value | Boolean | true | (Optional) true or false. |


> **operator** 子参数


| Options | Description |
| --- | --- |
| != | Test for inequality. |
| < | Test for less-than the value. |
| <= | Test for less-than or equal to the value. |
| <> | Test for inequality. |
| = | Test for equality. |
| == | Test for equality. |
| > | Test for greater-than the value. |
| >= | Test for greater-than or equal to the value. |
| equals | Test for equality. |
| not | Test for inequality. |


> **subject** 子参数


| Options | Description |
| --- | --- |
| block | The block involved with the interaction. |
| damager | The damaging actor involved with the interaction. |
| other | The other member of an interaction, not the caller. |
| parent | The caller's current parent. |
| player | The player involved with the interaction. |
| self | The entity or object calling the test |
| target | The caller's current target. |


## Examples


Full..


```json
{ "test": "is_humid", "subject": "self", "operator": "equals", "value": "true" }
```


Short (using Defaults)..


```json
{ "test": "is_humid" }
```


# is_immobile


Returns true if the subject entity is immobile. An entity is immobile if it lacks AI goals, has just changed dimensions or if it is a mob and has no health.


| Name | Type | Default | Description |
| --- | --- | --- | --- |
| operator | String | equals | (Optional) The comparison to apply with 'value'. |
| subject | String | self | (Optional) The subject of this filter test. |
| value | Boolean | true | (Optional) true or false. |


> **operator** 子参数


| Options | Description |
| --- | --- |
| != | Test for inequality. |
| < | Test for less-than the value. |
| <= | Test for less-than or equal to the value. |
| <> | Test for inequality. |
| = | Test for equality. |
| == | Test for equality. |
| > | Test for greater-than the value. |
| >= | Test for greater-than or equal to the value. |
| equals | Test for equality. |
| not | Test for inequality. |


> **subject** 子参数


| Options | Description |
| --- | --- |
| block | The block involved with the interaction. |
| damager | The damaging actor involved with the interaction. |
| other | The other member of an interaction, not the caller. |
| parent | The caller's current parent. |
| player | The player involved with the interaction. |
| self | The entity or object calling the test |
| target | The caller's current target. |


## Examples


Full..


```json
{ "test": "is_immobile", "subject": "self", "operator": "equals", "value": "true" }
```


Short (using Defaults)..


```json
{ "test": "is_immobile" }
```


# is_in_village


Tests whether the Subject is inside the bounds of a village.


| Name | Type | Default | Description |
| --- | --- | --- | --- |
| operator | String | equals | (Optional) The comparison to apply with 'value'. |
| subject | String | self | (Optional) The subject of this filter test. |
| value | Boolean | true | (Optional) true or false. |


> **operator** 子参数


| Options | Description |
| --- | --- |
| != | Test for inequality. |
| < | Test for less-than the value. |
| <= | Test for less-than or equal to the value. |
| <> | Test for inequality. |
| = | Test for equality. |
| == | Test for equality. |
| > | Test for greater-than the value. |
| >= | Test for greater-than or equal to the value. |
| equals | Test for equality. |
| not | Test for inequality. |


> **subject** 子参数


| Options | Description |
| --- | --- |
| block | The block involved with the interaction. |
| damager | The damaging actor involved with the interaction. |
| other | The other member of an interaction, not the caller. |
| parent | The caller's current parent. |
| player | The player involved with the interaction. |
| self | The entity or object calling the test |
| target | The caller's current target. |


## Examples


Full..


```json
{ "test": "is_in_village", "subject": "self", "operator": "equals", "value": "true" }
```


Short (using Defaults)..


```json
{ "test": "is_in_village" }
```


# is_leashed


Returns true if the subject entity is leashed.


| Name | Type | Default | Description |
| --- | --- | --- | --- |
| operator | String | equals | (Optional) The comparison to apply with 'value'. |
| subject | String | self | (Optional) The subject of this filter test. |
| value | Boolean | true | (Optional) true or false. |


> **operator** 子参数


| Options | Description |
| --- | --- |
| != | Test for inequality. |
| < | Test for less-than the value. |
| <= | Test for less-than or equal to the value. |
| <> | Test for inequality. |
| = | Test for equality. |
| == | Test for equality. |
| > | Test for greater-than the value. |
| >= | Test for greater-than or equal to the value. |
| equals | Test for equality. |
| not | Test for inequality. |


> **subject** 子参数


| Options | Description |
| --- | --- |
| block | The block involved with the interaction. |
| damager | The damaging actor involved with the interaction. |
| other | The other member of an interaction, not the caller. |
| parent | The caller's current parent. |
| player | The player involved with the interaction. |
| self | The entity or object calling the test |
| target | The caller's current target. |


## Examples


Full..


```json
{ "test": "is_leashed", "subject": "self", "operator": "equals", "value": "true" }
```


Short (using Defaults)..


```json
{ "test": "is_leashed" }
```


# is_leashed_to


Returns true if the subject entity leashed to the calling entity.


| Name | Type | Default | Description |
| --- | --- | --- | --- |
| operator | String | equals | (Optional) The comparison to apply with 'value'. |
| subject | String | self | (Optional) The subject of this filter test. |
| value | Boolean | true | (Optional) true or false. |


> **operator** 子参数


| Options | Description |
| --- | --- |
| != | Test for inequality. |
| < | Test for less-than the value. |
| <= | Test for less-than or equal to the value. |
| <> | Test for inequality. |
| = | Test for equality. |
| == | Test for equality. |
| > | Test for greater-than the value. |
| >= | Test for greater-than or equal to the value. |
| equals | Test for equality. |
| not | Test for inequality. |


> **subject** 子参数


| Options | Description |
| --- | --- |
| block | The block involved with the interaction. |
| damager | The damaging actor involved with the interaction. |
| other | The other member of an interaction, not the caller. |
| parent | The caller's current parent. |
| player | The player involved with the interaction. |
| self | The entity or object calling the test |
| target | The caller's current target. |


## Examples


Full..


```json
{ "test": "is_leashed_to", "subject": "self", "operator": "equals", "value": "true" }
```


Short (using Defaults)..


```json
{ "test": "is_leashed_to" }
```


# is_mark_variant


Returns true if the subject entity is the mark variant number provided.


| Name | Type | Default | Description |
| --- | --- | --- | --- |
| operator | String | equals | (Optional) The comparison to apply with 'value'. |
| subject | String | self | (Optional) The subject of this filter test. |
| value | Integer |  | (Required) An integer value. |


> **operator** 子参数


| Options | Description |
| --- | --- |
| != | Test for inequality. |
| < | Test for less-than the value. |
| <= | Test for less-than or equal to the value. |
| <> | Test for inequality. |
| = | Test for equality. |
| == | Test for equality. |
| > | Test for greater-than the value. |
| >= | Test for greater-than or equal to the value. |
| equals | Test for equality. |
| not | Test for inequality. |


> **subject** 子参数


| Options | Description |
| --- | --- |
| block | The block involved with the interaction. |
| damager | The damaging actor involved with the interaction. |
| other | The other member of an interaction, not the caller. |
| parent | The caller's current parent. |
| player | The player involved with the interaction. |
| self | The entity or object calling the test |
| target | The caller's current target. |


## Examples


Full..


```json
{ "test": "is_mark_variant", "subject": "self", "operator": "equals", "value": "0" }
```


Short (using Defaults)..


```json
{ "test": "is_mark_variant", "value": "0" }
```


# is_missing_health


Tests if the subject is not at full health.


| Name | Type | Default | Description |
| --- | --- | --- | --- |
| operator | String | equals | (Optional) The comparison to apply with 'value'. |
| subject | String | self | (Optional) The subject of this filter test. |
| value | Boolean | true | (Optional) true or false. |


> **operator** 子参数


| Options | Description |
| --- | --- |
| != | Test for inequality. |
| < | Test for less-than the value. |
| <= | Test for less-than or equal to the value. |
| <> | Test for inequality. |
| = | Test for equality. |
| == | Test for equality. |
| > | Test for greater-than the value. |
| >= | Test for greater-than or equal to the value. |
| equals | Test for equality. |
| not | Test for inequality. |


> **subject** 子参数


| Options | Description |
| --- | --- |
| block | The block involved with the interaction. |
| damager | The damaging actor involved with the interaction. |
| other | The other member of an interaction, not the caller. |
| parent | The caller's current parent. |
| player | The player involved with the interaction. |
| self | The entity or object calling the test |
| target | The caller's current target. |


## Examples


Full..


```json
{ "test": "is_missing_health", "subject": "self", "operator": "equals", "value": "true" }
```


Short (using Defaults)..


```json
{ "test": "is_missing_health" }
```


# is_moving


Returns true if the subject entity is moving.


| Name | Type | Default | Description |
| --- | --- | --- | --- |
| operator | String | equals | (Optional) The comparison to apply with 'value'. |
| subject | String | self | (Optional) The subject of this filter test. |
| value | Boolean | true | (Optional) true or false. |


> **operator** 子参数


| Options | Description |
| --- | --- |
| != | Test for inequality. |
| < | Test for less-than the value. |
| <= | Test for less-than or equal to the value. |
| <> | Test for inequality. |
| = | Test for equality. |
| == | Test for equality. |
| > | Test for greater-than the value. |
| >= | Test for greater-than or equal to the value. |
| equals | Test for equality. |
| not | Test for inequality. |


> **subject** 子参数


| Options | Description |
| --- | --- |
| block | The block involved with the interaction. |
| damager | The damaging actor involved with the interaction. |
| other | The other member of an interaction, not the caller. |
| parent | The caller's current parent. |
| player | The player involved with the interaction. |
| self | The entity or object calling the test |
| target | The caller's current target. |


## Examples


Full..


```json
{ "test": "is_moving", "subject": "self", "operator": "equals", "value": "true" }
```


Short (using Defaults)..


```json
{ "test": "is_moving" }
```


# is_navigating


Tests if the subject is currently pathfinding. Requires a "minecraft:navigation" component.


| Name | Type | Default | Description |
| --- | --- | --- | --- |
| operator | String | equals | (Optional) The comparison to apply with 'value'. |
| subject | String | self | (Optional) The subject of this filter test. |
| value | Boolean | true | (Optional) true or false. |


> **operator** 子参数


| Options | Description |
| --- | --- |
| != | Test for inequality. |
| < | Test for less-than the value. |
| <= | Test for less-than or equal to the value. |
| <> | Test for inequality. |
| = | Test for equality. |
| == | Test for equality. |
| > | Test for greater-than the value. |
| >= | Test for greater-than or equal to the value. |
| equals | Test for equality. |
| not | Test for inequality. |


> **subject** 子参数


| Options | Description |
| --- | --- |
| block | The block involved with the interaction. |
| damager | The damaging actor involved with the interaction. |
| other | The other member of an interaction, not the caller. |
| parent | The caller's current parent. |
| player | The player involved with the interaction. |
| self | The entity or object calling the test |
| target | The caller's current target. |


## Examples


Full..


```json
{ "test": "is_navigating", "subject": "self", "operator": "equals", "value": "true" }
```


Short (using Defaults)..


```json
{ "test": "is_navigating" }
```


# is_owner


Returns true if the subject entity is the owner of the calling entity.


| Name | Type | Default | Description |
| --- | --- | --- | --- |
| operator | String | equals | (Optional) The comparison to apply with 'value'. |
| subject | String | self | (Optional) The subject of this filter test. |
| value | Boolean | true | (Optional) true or false. |


> **operator** 子参数


| Options | Description |
| --- | --- |
| != | Test for inequality. |
| < | Test for less-than the value. |
| <= | Test for less-than or equal to the value. |
| <> | Test for inequality. |
| = | Test for equality. |
| == | Test for equality. |
| > | Test for greater-than the value. |
| >= | Test for greater-than or equal to the value. |
| equals | Test for equality. |
| not | Test for inequality. |


> **subject** 子参数


| Options | Description |
| --- | --- |
| block | The block involved with the interaction. |
| damager | The damaging actor involved with the interaction. |
| other | The other member of an interaction, not the caller. |
| parent | The caller's current parent. |
| player | The player involved with the interaction. |
| self | The entity or object calling the test |
| target | The caller's current target. |


## Examples


Full..


```json
{ "test": "is_owner", "subject": "self", "operator": "equals", "value": "true" }
```


Short (using Defaults)..


```json
{ "test": "is_owner" }
```


# is_panicking


Tests if the subject is panicking.


| Name | Type | Default | Description |
| --- | --- | --- | --- |
| operator | String | equals | (Optional) The comparison to apply with 'value'. |
| subject | String | self | (Optional) The subject of this filter test. |
| value | Boolean | true | (Optional) true or false. |


> **operator** 子参数


| Options | Description |
| --- | --- |
| != | Test for inequality. |
| < | Test for less-than the value. |
| <= | Test for less-than or equal to the value. |
| <> | Test for inequality. |
| = | Test for equality. |
| == | Test for equality. |
| > | Test for greater-than the value. |
| >= | Test for greater-than or equal to the value. |
| equals | Test for equality. |
| not | Test for inequality. |


> **subject** 子参数


| Options | Description |
| --- | --- |
| block | The block involved with the interaction. |
| damager | The damaging actor involved with the interaction. |
| other | The other member of an interaction, not the caller. |
| parent | The caller's current parent. |
| player | The player involved with the interaction. |
| self | The entity or object calling the test |
| target | The caller's current target. |


## Examples


Full..


```json
{ "test": "is_panicking", "subject": "self", "operator": "equals", "value": "true" }
```


Short (using Defaults)..


```json
{ "test": "is_panicking" }
```


# is_persistent


Tests if the subject's persistence matches the bool value passed in.


| Name | Type | Default | Description |
| --- | --- | --- | --- |
| operator | String | equals | (Optional) The comparison to apply with 'value'. |
| subject | String | self | (Optional) The subject of this filter test. |
| value | Boolean | true | (Optional) true or false. |


> **operator** 子参数


| Options | Description |
| --- | --- |
| != | Test for inequality. |
| < | Test for less-than the value. |
| <= | Test for less-than or equal to the value. |
| <> | Test for inequality. |
| = | Test for equality. |
| == | Test for equality. |
| > | Test for greater-than the value. |
| >= | Test for greater-than or equal to the value. |
| equals | Test for equality. |
| not | Test for inequality. |


> **subject** 子参数


| Options | Description |
| --- | --- |
| block | The block involved with the interaction. |
| damager | The damaging actor involved with the interaction. |
| other | The other member of an interaction, not the caller. |
| parent | The caller's current parent. |
| player | The player involved with the interaction. |
| self | The entity or object calling the test |
| target | The caller's current target. |


## Examples


Full..


```json
{ "test": "is_persistent", "subject": "self", "operator": "equals", "value": "true" }
```


Short (using Defaults)..


```json
{ "test": "is_persistent" }
```


# is_raider


Tests if the subject is a raider.


| Name | Type | Default | Description |
| --- | --- | --- | --- |
| operator | String | equals | (Optional) The comparison to apply with 'value'. |
| subject | String | self | (Optional) The subject of this filter test. |
| value | Boolean | true | (Optional) true or false. |


> **operator** 子参数


| Options | Description |
| --- | --- |
| != | Test for inequality. |
| < | Test for less-than the value. |
| <= | Test for less-than or equal to the value. |
| <> | Test for inequality. |
| = | Test for equality. |
| == | Test for equality. |
| > | Test for greater-than the value. |
| >= | Test for greater-than or equal to the value. |
| equals | Test for equality. |
| not | Test for inequality. |


> **subject** 子参数


| Options | Description |
| --- | --- |
| block | The block involved with the interaction. |
| damager | The damaging actor involved with the interaction. |
| other | The other member of an interaction, not the caller. |
| parent | The caller's current parent. |
| player | The player involved with the interaction. |
| self | The entity or object calling the test |
| target | The caller's current target. |


## Examples


Full..


```json
{ "test": "is_raider", "subject": "self", "operator": "equals", "value": "true" }
```


Short (using Defaults)..


```json
{ "test": "is_raider" }
```


# is_riding


Returns true if the subject entity is riding on another entity.


| Name | Type | Default | Description |
| --- | --- | --- | --- |
| operator | String | equals | (Optional) The comparison to apply with 'value'. |
| subject | String | self | (Optional) The subject of this filter test. |
| value | Boolean | true | (Optional) true or false. |


> **operator** 子参数


| Options | Description |
| --- | --- |
| != | Test for inequality. |
| < | Test for less-than the value. |
| <= | Test for less-than or equal to the value. |
| <> | Test for inequality. |
| = | Test for equality. |
| == | Test for equality. |
| > | Test for greater-than the value. |
| >= | Test for greater-than or equal to the value. |
| equals | Test for equality. |
| not | Test for inequality. |


> **subject** 子参数


| Options | Description |
| --- | --- |
| block | The block involved with the interaction. |
| damager | The damaging actor involved with the interaction. |
| other | The other member of an interaction, not the caller. |
| parent | The caller's current parent. |
| player | The player involved with the interaction. |
| self | The entity or object calling the test |
| target | The caller's current target. |


## Examples


Full..


```json
{ "test": "is_riding", "subject": "self", "operator": "equals", "value": "true" }
```


Short (using Defaults)..


```json
{ "test": "is_riding" }
```


# is_riding_self


Returns true if the subject entity is riding the calling entity.


| Name | Type | Default | Description |
| --- | --- | --- | --- |
| operator | String | equals | (Optional) The comparison to apply with 'value'. |
| subject | String | self | (Optional) The subject of this filter test. |
| value | Boolean | true | (Optional) true or false. |


> **operator** 子参数


| Options | Description |
| --- | --- |
| != | Test for inequality. |
| < | Test for less-than the value. |
| <= | Test for less-than or equal to the value. |
| <> | Test for inequality. |
| = | Test for equality. |
| == | Test for equality. |
| > | Test for greater-than the value. |
| >= | Test for greater-than or equal to the value. |
| equals | Test for equality. |
| not | Test for inequality. |


> **subject** 子参数


| Options | Description |
| --- | --- |
| block | The block involved with the interaction. |
| damager | The damaging actor involved with the interaction. |
| other | The other member of an interaction, not the caller. |
| parent | The caller's current parent. |
| player | The player involved with the interaction. |
| self | The entity or object calling the test |
| target | The caller's current target. |


## Examples


Full..


```json
{ "test": "is_riding_self", "subject": "self", "operator": "equals", "value": "true" }
```


Short (using Defaults)..


```json
{ "test": "is_riding_self" }
```


# is_sitting


Tests if the subject is sitting.


| Name | Type | Default | Description |
| --- | --- | --- | --- |
| operator | String | equals | (Optional) The comparison to apply with 'value'. |
| subject | String | self | (Optional) The subject of this filter test. |
| value | Boolean | true | (Optional) true or false. |


> **operator** 子参数


| Options | Description |
| --- | --- |
| != | Test for inequality. |
| < | Test for less-than the value. |
| <= | Test for less-than or equal to the value. |
| <> | Test for inequality. |
| = | Test for equality. |
| == | Test for equality. |
| > | Test for greater-than the value. |
| >= | Test for greater-than or equal to the value. |
| equals | Test for equality. |
| not | Test for inequality. |


> **subject** 子参数


| Options | Description |
| --- | --- |
| block | The block involved with the interaction. |
| damager | The damaging actor involved with the interaction. |
| other | The other member of an interaction, not the caller. |
| parent | The caller's current parent. |
| player | The player involved with the interaction. |
| self | The entity or object calling the test |
| target | The caller's current target. |


## Examples


Full..


```json
{ "test": "is_sitting", "subject": "self", "operator": "equals", "value": "true" }
```


Short (using Defaults)..


```json
{ "test": "is_sitting" }
```


# is_skin_id


Returns true if the subject entity is the skin id number provided.


| Name | Type | Default | Description |
| --- | --- | --- | --- |
| operator | String | equals | (Optional) The comparison to apply with 'value'. |
| subject | String | self | (Optional) The subject of this filter test. |
| value | Integer |  | (Required) An integer value. |


> **operator** 子参数


| Options | Description |
| --- | --- |
| != | Test for inequality. |
| < | Test for less-than the value. |
| <= | Test for less-than or equal to the value. |
| <> | Test for inequality. |
| = | Test for equality. |
| == | Test for equality. |
| > | Test for greater-than the value. |
| >= | Test for greater-than or equal to the value. |
| equals | Test for equality. |
| not | Test for inequality. |


> **subject** 子参数


| Options | Description |
| --- | --- |
| block | The block involved with the interaction. |
| damager | The damaging actor involved with the interaction. |
| other | The other member of an interaction, not the caller. |
| parent | The caller's current parent. |
| player | The player involved with the interaction. |
| self | The entity or object calling the test |
| target | The caller's current target. |


## Examples


Full..


```json
{ "test": "is_skin_id", "subject": "self", "operator": "equals", "value": "0" }
```


Short (using Defaults)..


```json
{ "test": "is_skin_id", "value": "0" }
```


# is_sleeping


Tests whether the Subject is sleeping.


| Name | Type | Default | Description |
| --- | --- | --- | --- |
| operator | String | equals | (Optional) The comparison to apply with 'value'. |
| subject | String | self | (Optional) The subject of this filter test. |
| value | Boolean | true | (Optional) true or false. |


> **operator** 子参数


| Options | Description |
| --- | --- |
| != | Test for inequality. |
| < | Test for less-than the value. |
| <= | Test for less-than or equal to the value. |
| <> | Test for inequality. |
| = | Test for equality. |
| == | Test for equality. |
| > | Test for greater-than the value. |
| >= | Test for greater-than or equal to the value. |
| equals | Test for equality. |
| not | Test for inequality. |


> **subject** 子参数


| Options | Description |
| --- | --- |
| block | The block involved with the interaction. |
| damager | The damaging actor involved with the interaction. |
| other | The other member of an interaction, not the caller. |
| parent | The caller's current parent. |
| player | The player involved with the interaction. |
| self | The entity or object calling the test |
| target | The caller's current target. |


## Examples


Full..


```json
{ "test": "is_sleeping", "subject": "self", "operator": "equals", "value": "true" }
```


Short (using Defaults)..


```json
{ "test": "is_sleeping" }
```


# is_sneak_held


Returns true if the subject entity has the sneak input held.


| Name | Type | Default | Description |
| --- | --- | --- | --- |
| operator | String | equals | (Optional) The comparison to apply with 'value'. |
| subject | String | self | (Optional) The subject of this filter test. |
| value | Boolean | true | (Optional) true or false. |


> **operator** 子参数


| Options | Description |
| --- | --- |
| != | Test for inequality. |
| < | Test for less-than the value. |
| <= | Test for less-than or equal to the value. |
| <> | Test for inequality. |
| = | Test for equality. |
| == | Test for equality. |
| > | Test for greater-than the value. |
| >= | Test for greater-than or equal to the value. |
| equals | Test for equality. |
| not | Test for inequality. |


> **subject** 子参数


| Options | Description |
| --- | --- |
| block | The block involved with the interaction. |
| damager | The damaging actor involved with the interaction. |
| other | The other member of an interaction, not the caller. |
| parent | The caller's current parent. |
| player | The player involved with the interaction. |
| self | The entity or object calling the test |
| target | The caller's current target. |


## Examples


Full..


```json
{ "test": "is_sneak_held", "subject": "self", "operator": "equals", "value": "true" }
```


Short (using Defaults)..


```json
{ "test": "is_sneak_held" }
```


# is_sneaking


Returns true if the subject entity is sneaking.


| Name | Type | Default | Description |
| --- | --- | --- | --- |
| operator | String | equals | (Optional) The comparison to apply with 'value'. |
| subject | String | self | (Optional) The subject of this filter test. |
| value | Boolean | true | (Optional) true or false. |


> **operator** 子参数


| Options | Description |
| --- | --- |
| != | Test for inequality. |
| < | Test for less-than the value. |
| <= | Test for less-than or equal to the value. |
| <> | Test for inequality. |
| = | Test for equality. |
| == | Test for equality. |
| > | Test for greater-than the value. |
| >= | Test for greater-than or equal to the value. |
| equals | Test for equality. |
| not | Test for inequality. |


> **subject** 子参数


| Options | Description |
| --- | --- |
| block | The block involved with the interaction. |
| damager | The damaging actor involved with the interaction. |
| other | The other member of an interaction, not the caller. |
| parent | The caller's current parent. |
| player | The player involved with the interaction. |
| self | The entity or object calling the test |
| target | The caller's current target. |


## Examples


Full..


```json
{ "test": "is_sneaking", "subject": "self", "operator": "equals", "value": "true" }
```


Short (using Defaults)..


```json
{ "test": "is_sneaking" }
```


# is_snow_covered


Tests whether the Subject is in an area with snow cover


| Name | Type | Default | Description |
| --- | --- | --- | --- |
| operator | String | equals | (Optional) The comparison to apply with 'value'. |
| subject | String | self | (Optional) The subject of this filter test. |
| value | Boolean | true | (Optional) true or false. |


> **operator** 子参数


| Options | Description |
| --- | --- |
| != | Test for inequality. |
| < | Test for less-than the value. |
| <= | Test for less-than or equal to the value. |
| <> | Test for inequality. |
| = | Test for equality. |
| == | Test for equality. |
| > | Test for greater-than the value. |
| >= | Test for greater-than or equal to the value. |
| equals | Test for equality. |
| not | Test for inequality. |


> **subject** 子参数


| Options | Description |
| --- | --- |
| block | The block involved with the interaction. |
| damager | The damaging actor involved with the interaction. |
| other | The other member of an interaction, not the caller. |
| parent | The caller's current parent. |
| player | The player involved with the interaction. |
| self | The entity or object calling the test |
| target | The caller's current target. |


## Examples


Full..


```json
{ "test": "is_snow_covered", "subject": "self", "operator": "equals", "value": "true" }
```


Short (using Defaults)..


```json
{ "test": "is_snow_covered" }
```


# is_sprinting


Tests if the subject is sprinting.


| Name | Type | Default | Description |
| --- | --- | --- | --- |
| operator | String | equals | (Optional) The comparison to apply with 'value'. |
| subject | String | self | (Optional) The subject of this filter test. |
| value | Boolean | true | (Optional) true or false. |


> **operator** 子参数


| Options | Description |
| --- | --- |
| != | Test for inequality. |
| < | Test for less-than the value. |
| <= | Test for less-than or equal to the value. |
| <> | Test for inequality. |
| = | Test for equality. |
| == | Test for equality. |
| > | Test for greater-than the value. |
| >= | Test for greater-than or equal to the value. |
| equals | Test for equality. |
| not | Test for inequality. |


> **subject** 子参数


| Options | Description |
| --- | --- |
| block | The block involved with the interaction. |
| damager | The damaging actor involved with the interaction. |
| other | The other member of an interaction, not the caller. |
| parent | The caller's current parent. |
| player | The player involved with the interaction. |
| self | The entity or object calling the test |
| target | The caller's current target. |


## Examples


Full..


```json
{ "test": "is_sprinting", "subject": "self", "operator": "equals", "value": "true" }
```


Short (using Defaults)..


```json
{ "test": "is_sprinting" }
```


# is_target


Returns true if the subject entity is the target of the calling entity.


| Name | Type | Default | Description |
| --- | --- | --- | --- |
| operator | String | equals | (Optional) The comparison to apply with 'value'. |
| subject | String | self | (Optional) The subject of this filter test. |
| value | Boolean | true | (Optional) true or false. |


> **operator** 子参数


| Options | Description |
| --- | --- |
| != | Test for inequality. |
| < | Test for less-than the value. |
| <= | Test for less-than or equal to the value. |
| <> | Test for inequality. |
| = | Test for equality. |
| == | Test for equality. |
| > | Test for greater-than the value. |
| >= | Test for greater-than or equal to the value. |
| equals | Test for equality. |
| not | Test for inequality. |


> **subject** 子参数


| Options | Description |
| --- | --- |
| block | The block involved with the interaction. |
| damager | The damaging actor involved with the interaction. |
| other | The other member of an interaction, not the caller. |
| parent | The caller's current parent. |
| player | The player involved with the interaction. |
| self | The entity or object calling the test |
| target | The caller's current target. |


## Examples


Full..


```json
{ "test": "is_target", "subject": "self", "operator": "equals", "value": "true" }
```


Short (using Defaults)..


```json
{ "test": "is_target" }
```


# is_temperature_type


Tests whether the current temperature is a given type.


| Name | Type | Default | Description |
| --- | --- | --- | --- |
| operator | String | equals | (Optional) The comparison to apply with 'value'. |
| subject | String | self | (Optional) The subject of this filter test. |
| value | String |  | (Required) The Biome temperature catagory to test |


> **operator** 子参数


| Options | Description |
| --- | --- |
| != | Test for inequality. |
| < | Test for less-than the value. |
| <= | Test for less-than or equal to the value. |
| <> | Test for inequality. |
| = | Test for equality. |
| == | Test for equality. |
| > | Test for greater-than the value. |
| >= | Test for greater-than or equal to the value. |
| equals | Test for equality. |
| not | Test for inequality. |


> **subject** 子参数


| Options | Description |
| --- | --- |
| block | The block involved with the interaction. |
| damager | The damaging actor involved with the interaction. |
| other | The other member of an interaction, not the caller. |
| parent | The caller's current parent. |
| player | The player involved with the interaction. |
| self | The entity or object calling the test |
| target | The caller's current target. |


> **value** 子参数


| Options | Description |
| --- | --- |
| cold |  |
| mild |  |
| ocean |  |
| warm |  |


## Examples


Full..


```json
{ "test": "is_temperature_type", "subject": "self", "operator": "equals", "value": "cold" }
```


Short (using Defaults)..


```json
{ "test": "is_temperature_type", "value": "cold" }
```


# is_temperature_value


Tests the current temperature against a provided value in the range (0.0, 1.0) where 0.0f is the coldest temp and 1.0f is the hottest.


| Name | Type | Default | Description |
| --- | --- | --- | --- |
| operator | String | equals | (Optional) The comparison to apply with 'value'. |
| subject | String | self | (Optional) The subject of this filter test. |
| value | Decimal |  | (Required) The Biome temperature value to compare with. |


> **operator** 子参数


| Options | Description |
| --- | --- |
| != | Test for inequality. |
| < | Test for less-than the value. |
| <= | Test for less-than or equal to the value. |
| <> | Test for inequality. |
| = | Test for equality. |
| == | Test for equality. |
| > | Test for greater-than the value. |
| >= | Test for greater-than or equal to the value. |
| equals | Test for equality. |
| not | Test for inequality. |


> **subject** 子参数


| Options | Description |
| --- | --- |
| block | The block involved with the interaction. |
| damager | The damaging actor involved with the interaction. |
| other | The other member of an interaction, not the caller. |
| parent | The caller's current parent. |
| player | The player involved with the interaction. |
| self | The entity or object calling the test |
| target | The caller's current target. |


## Examples


Full..


```json
{ "test": "is_temperature_value", "subject": "self", "operator": "equals", "value": "0.50" }
```


Short (using Defaults)..


```json
{ "test": "is_temperature_value", "value": "0.50" }
```


# is_underground


Returns true when the subject entity is underground. An entity is considered underground if there are non-solid blocks above it.


| Name | Type | Default | Description |
| --- | --- | --- | --- |
| operator | String | equals | (Optional) The comparison to apply with 'value'. |
| subject | String | self | (Optional) The subject of this filter test. |
| value | Boolean | true | (Optional) true or false. |


> **operator** 子参数


| Options | Description |
| --- | --- |
| != | Test for inequality. |
| < | Test for less-than the value. |
| <= | Test for less-than or equal to the value. |
| <> | Test for inequality. |
| = | Test for equality. |
| == | Test for equality. |
| > | Test for greater-than the value. |
| >= | Test for greater-than or equal to the value. |
| equals | Test for equality. |
| not | Test for inequality. |


> **subject** 子参数


| Options | Description |
| --- | --- |
| block | The block involved with the interaction. |
| damager | The damaging actor involved with the interaction. |
| other | The other member of an interaction, not the caller. |
| parent | The caller's current parent. |
| player | The player involved with the interaction. |
| self | The entity or object calling the test |
| target | The caller's current target. |


## Examples


Full..


```json
{ "test": "is_underground", "subject": "self", "operator": "equals", "value": "true" }
```


Short (using Defaults)..


```json
{ "test": "is_underground" }
```


# is_underwater


Returns true when the subject entity is under water. An entity is considered underwater if it is completely submerged in water blocks.


| Name | Type | Default | Description |
| --- | --- | --- | --- |
| operator | String | equals | (Optional) The comparison to apply with 'value'. |
| subject | String | self | (Optional) The subject of this filter test. |
| value | Boolean | true | (Optional) true or false. |


> **operator** 子参数


| Options | Description |
| --- | --- |
| != | Test for inequality. |
| < | Test for less-than the value. |
| <= | Test for less-than or equal to the value. |
| <> | Test for inequality. |
| = | Test for equality. |
| == | Test for equality. |
| > | Test for greater-than the value. |
| >= | Test for greater-than or equal to the value. |
| equals | Test for equality. |
| not | Test for inequality. |


> **subject** 子参数


| Options | Description |
| --- | --- |
| block | The block involved with the interaction. |
| damager | The damaging actor involved with the interaction. |
| other | The other member of an interaction, not the caller. |
| parent | The caller's current parent. |
| player | The player involved with the interaction. |
| self | The entity or object calling the test |
| target | The caller's current target. |


## Examples


Full..


```json
{ "test": "is_underwater", "subject": "self", "operator": "equals", "value": "true" }
```


Short (using Defaults)..


```json
{ "test": "is_underwater" }
```


# is_variant


Returns true if the subject entity is the variant number provided.


| Name | Type | Default | Description |
| --- | --- | --- | --- |
| operator | String | equals | (Optional) The comparison to apply with 'value'. |
| subject | String | self | (Optional) The subject of this filter test. |
| value | Integer |  | (Required) An integer value. |


> **operator** 子参数


| Options | Description |
| --- | --- |
| != | Test for inequality. |
| < | Test for less-than the value. |
| <= | Test for less-than or equal to the value. |
| <> | Test for inequality. |
| = | Test for equality. |
| == | Test for equality. |
| > | Test for greater-than the value. |
| >= | Test for greater-than or equal to the value. |
| equals | Test for equality. |
| not | Test for inequality. |


> **subject** 子参数


| Options | Description |
| --- | --- |
| block | The block involved with the interaction. |
| damager | The damaging actor involved with the interaction. |
| other | The other member of an interaction, not the caller. |
| parent | The caller's current parent. |
| player | The player involved with the interaction. |
| self | The entity or object calling the test |
| target | The caller's current target. |


## Examples


Full..


```json
{ "test": "is_variant", "subject": "self", "operator": "equals", "value": "0" }
```


Short (using Defaults)..


```json
{ "test": "is_variant", "value": "0" }
```


# is_vehicle_family


Returns true when the subject entity's vehicle is a member of the named family.


| Name | Type | Default | Description |
| --- | --- | --- | --- |
| operator | String | equals | (Optional) The comparison to apply with 'value'. |
| subject | String | self | (Optional) The subject of this filter test. |
| value | String |  | (Required) The Family name to look for |


> **operator** 子参数


| Options | Description |
| --- | --- |
| != | Test for inequality. |
| < | Test for less-than the value. |
| <= | Test for less-than or equal to the value. |
| <> | Test for inequality. |
| = | Test for equality. |
| == | Test for equality. |
| > | Test for greater-than the value. |
| >= | Test for greater-than or equal to the value. |
| equals | Test for equality. |
| not | Test for inequality. |


> **subject** 子参数


| Options | Description |
| --- | --- |
| block | The block involved with the interaction. |
| damager | The damaging actor involved with the interaction. |
| other | The other member of an interaction, not the caller. |
| parent | The caller's current parent. |
| player | The player involved with the interaction. |
| self | The entity or object calling the test |
| target | The caller's current target. |


## Examples


Full..


```json
{ "test": "is_vehicle_family", "subject": "self", "operator": "equals", "value": "player" }
```


Short (using Defaults)..


```json
{ "test": "is_vehicle_family", "value": "player" }
```


# is_visible


Returns true if the subject entity is visible.


| Name | Type | Default | Description |
| --- | --- | --- | --- |
| operator | String | equals | (Optional) The comparison to apply with 'value'. |
| subject | String | self | (Optional) The subject of this filter test. |
| value | Boolean | true | (Optional) true or false. |


> **operator** 子参数


| Options | Description |
| --- | --- |
| != | Test for inequality. |
| < | Test for less-than the value. |
| <= | Test for less-than or equal to the value. |
| <> | Test for inequality. |
| = | Test for equality. |
| == | Test for equality. |
| > | Test for greater-than the value. |
| >= | Test for greater-than or equal to the value. |
| equals | Test for equality. |
| not | Test for inequality. |


> **subject** 子参数


| Options | Description |
| --- | --- |
| block | The block involved with the interaction. |
| damager | The damaging actor involved with the interaction. |
| other | The other member of an interaction, not the caller. |
| parent | The caller's current parent. |
| player | The player involved with the interaction. |
| self | The entity or object calling the test |
| target | The caller's current target. |


## Examples


Full..


```json
{ "test": "is_visible", "subject": "self", "operator": "equals", "value": "true" }
```


Short (using Defaults)..


```json
{ "test": "is_visible" }
```


# is_waterlogged


Tests if the subject block is submerged in water.


| Name | Type | Default | Description |
| --- | --- | --- | --- |
| operator | String | equals | (Optional) The comparison to apply with 'value'. |
| subject | String | self | (Optional) The subject of this filter test. |
| value | Boolean |  | (Required) true or false. |


> **operator** 子参数


| Options | Description |
| --- | --- |
| != | Test for inequality. |
| < | Test for less-than the value. |
| <= | Test for less-than or equal to the value. |
| <> | Test for inequality. |
| = | Test for equality. |
| == | Test for equality. |
| > | Test for greater-than the value. |
| >= | Test for greater-than or equal to the value. |
| equals | Test for equality. |
| not | Test for inequality. |


> **subject** 子参数


| Options | Description |
| --- | --- |
| block | The block involved with the interaction. |
| damager | The damaging actor involved with the interaction. |
| other | The other member of an interaction, not the caller. |
| parent | The caller's current parent. |
| player | The player involved with the interaction. |
| self | The entity or object calling the test |
| target | The caller's current target. |


## Examples


Full..


```json
{ "test": "is_waterlogged", "subject": "self", "operator": "equals", "value": "true" }
```


Short (using Defaults)..


```json
{ "test": "is_waterlogged", "value": "true" }
```


# is_weather


DEPRECATED


| Name | Type | Default | Description |
| --- | --- | --- | --- |
| operator | String | equals | (Optional) The comparison to apply with 'value'. |
| subject | String | self | (Optional) The subject of this filter test. |
| value | String |  | (Required) The Family name to look for |


> **operator** 子参数


| Options | Description |
| --- | --- |
| != | Test for inequality. |
| < | Test for less-than the value. |
| <= | Test for less-than or equal to the value. |
| <> | Test for inequality. |
| = | Test for equality. |
| == | Test for equality. |
| > | Test for greater-than the value. |
| >= | Test for greater-than or equal to the value. |
| equals | Test for equality. |
| not | Test for inequality. |


> **subject** 子参数


| Options | Description |
| --- | --- |
| block | The block involved with the interaction. |
| damager | The damaging actor involved with the interaction. |
| other | The other member of an interaction, not the caller. |
| parent | The caller's current parent. |
| player | The player involved with the interaction. |
| self | The entity or object calling the test |
| target | The caller's current target. |


## Examples


Full..


```json
{ "test": "is_weather", "subject": "self", "operator": "equals", "value": "player" }
```


Short (using Defaults)..


```json
{ "test": "is_weather", "value": "player" }
```


# light_level


Tests is the mob is outside of the specified light level range (0, 16).


| Name | Type | Default | Description |
| --- | --- | --- | --- |
| operator | String | equals | (Optional) The comparison to apply with 'value'. |
| subject | String | self | (Optional) The subject of this filter test. |
| value | Integer |  | (Required) An integer value. |


> **operator** 子参数


| Options | Description |
| --- | --- |
| != | Test for inequality. |
| < | Test for less-than the value. |
| <= | Test for less-than or equal to the value. |
| <> | Test for inequality. |
| = | Test for equality. |
| == | Test for equality. |
| > | Test for greater-than the value. |
| >= | Test for greater-than or equal to the value. |
| equals | Test for equality. |
| not | Test for inequality. |


> **subject** 子参数


| Options | Description |
| --- | --- |
| block | The block involved with the interaction. |
| damager | The damaging actor involved with the interaction. |
| other | The other member of an interaction, not the caller. |
| parent | The caller's current parent. |
| player | The player involved with the interaction. |
| self | The entity or object calling the test |
| target | The caller's current target. |


## Examples


Full..


```json
{ "test": "light_level", "subject": "self", "operator": "equals", "value": "0" }
```


Short (using Defaults)..


```json
{ "test": "light_level", "value": "0" }
```


# moon_intensity


Compares the current moon intensity with a float value in the range (0.0, 1.0)


| Name | Type | Default | Description |
| --- | --- | --- | --- |
| operator | String | equals | (Optional) The comparison to apply with 'value'. |
| subject | String | self | (Optional) The subject of this filter test. |
| value | Decimal |  | (Required) A floating point value. |


> **operator** 子参数


| Options | Description |
| --- | --- |
| != | Test for inequality. |
| < | Test for less-than the value. |
| <= | Test for less-than or equal to the value. |
| <> | Test for inequality. |
| = | Test for equality. |
| == | Test for equality. |
| > | Test for greater-than the value. |
| >= | Test for greater-than or equal to the value. |
| equals | Test for equality. |
| not | Test for inequality. |


> **subject** 子参数


| Options | Description |
| --- | --- |
| block | The block involved with the interaction. |
| damager | The damaging actor involved with the interaction. |
| other | The other member of an interaction, not the caller. |
| parent | The caller's current parent. |
| player | The player involved with the interaction. |
| self | The entity or object calling the test |
| target | The caller's current target. |


## Examples


Full..


```json
{ "test": "moon_intensity", "subject": "self", "operator": "equals", "value": "0.00" }
```


Short (using Defaults)..


```json
{ "test": "moon_intensity", "value": "0.00" }
```


# moon_phase


Compares the current moon phase with an integer value in the range (0, 7).


| Name | Type | Default | Description |
| --- | --- | --- | --- |
| operator | String | equals | (Optional) The comparison to apply with 'value'. |
| subject | String | self | (Optional) The subject of this filter test. |
| value | Integer |  | (Required) An integer value. |


> **operator** 子参数


| Options | Description |
| --- | --- |
| != | Test for inequality. |
| < | Test for less-than the value. |
| <= | Test for less-than or equal to the value. |
| <> | Test for inequality. |
| = | Test for equality. |
| == | Test for equality. |
| > | Test for greater-than the value. |
| >= | Test for greater-than or equal to the value. |
| equals | Test for equality. |
| not | Test for inequality. |


> **subject** 子参数


| Options | Description |
| --- | --- |
| block | The block involved with the interaction. |
| damager | The damaging actor involved with the interaction. |
| other | The other member of an interaction, not the caller. |
| parent | The caller's current parent. |
| player | The player involved with the interaction. |
| self | The entity or object calling the test |
| target | The caller's current target. |


## Examples


Full..


```json
{ "test": "moon_phase", "subject": "self", "operator": "equals", "value": "0" }
```


Short (using Defaults)..


```json
{ "test": "moon_phase", "value": "0" }
```


# on_fire


Tests if the subject is on fire.


| Name | Type | Default | Description |
| --- | --- | --- | --- |
| operator | String | equals | (Optional) The comparison to apply with 'value'. |
| subject | String | self | (Optional) The subject of this filter test. |
| value | Boolean | true | (Optional) true or false. |


> **operator** 子参数


| Options | Description |
| --- | --- |
| != | Test for inequality. |
| < | Test for less-than the value. |
| <= | Test for less-than or equal to the value. |
| <> | Test for inequality. |
| = | Test for equality. |
| == | Test for equality. |
| > | Test for greater-than the value. |
| >= | Test for greater-than or equal to the value. |
| equals | Test for equality. |
| not | Test for inequality. |


> **subject** 子参数


| Options | Description |
| --- | --- |
| block | The block involved with the interaction. |
| damager | The damaging actor involved with the interaction. |
| other | The other member of an interaction, not the caller. |
| parent | The caller's current parent. |
| player | The player involved with the interaction. |
| self | The entity or object calling the test |
| target | The caller's current target. |


## Examples


Full..


```json
{ "test": "on_fire", "subject": "self", "operator": "equals", "value": "true" }
```


Short (using Defaults)..


```json
{ "test": "on_fire" }
```


# on_ground


Returns true when the subject entity is on ground.


| Name | Type | Default | Description |
| --- | --- | --- | --- |
| operator | String | equals | (Optional) The comparison to apply with 'value'. |
| subject | String | self | (Optional) The subject of this filter test. |
| value | Boolean | true | (Optional) true or false. |


> **operator** 子参数


| Options | Description |
| --- | --- |
| != | Test for inequality. |
| < | Test for less-than the value. |
| <= | Test for less-than or equal to the value. |
| <> | Test for inequality. |
| = | Test for equality. |
| == | Test for equality. |
| > | Test for greater-than the value. |
| >= | Test for greater-than or equal to the value. |
| equals | Test for equality. |
| not | Test for inequality. |


> **subject** 子参数


| Options | Description |
| --- | --- |
| block | The block involved with the interaction. |
| damager | The damaging actor involved with the interaction. |
| other | The other member of an interaction, not the caller. |
| parent | The caller's current parent. |
| player | The player involved with the interaction. |
| self | The entity or object calling the test |
| target | The caller's current target. |


## Examples


Full..


```json
{ "test": "on_ground", "subject": "self", "operator": "equals", "value": "true" }
```


Short (using Defaults)..


```json
{ "test": "on_ground" }
```


# on_hot_block


Tests if the subject is on a hot block.


| Name | Type | Default | Description |
| --- | --- | --- | --- |
| operator | String | equals | (Optional) The comparison to apply with 'value'. |
| subject | String | self | (Optional) The subject of this filter test. |
| value | Boolean | true | (Optional) true or false. |


> **operator** 子参数


| Options | Description |
| --- | --- |
| != | Test for inequality. |
| < | Test for less-than the value. |
| <= | Test for less-than or equal to the value. |
| <> | Test for inequality. |
| = | Test for equality. |
| == | Test for equality. |
| > | Test for greater-than the value. |
| >= | Test for greater-than or equal to the value. |
| equals | Test for equality. |
| not | Test for inequality. |


> **subject** 子参数


| Options | Description |
| --- | --- |
| block | The block involved with the interaction. |
| damager | The damaging actor involved with the interaction. |
| other | The other member of an interaction, not the caller. |
| parent | The caller's current parent. |
| player | The player involved with the interaction. |
| self | The entity or object calling the test |
| target | The caller's current target. |


## Examples


Full..


```json
{ "test": "on_hot_block", "subject": "self", "operator": "equals", "value": "true" }
```


Short (using Defaults)..


```json
{ "test": "on_hot_block" }
```


# on_ladder


Returns true when the subject entity is on a ladder.


| Name | Type | Default | Description |
| --- | --- | --- | --- |
| operator | String | equals | (Optional) The comparison to apply with 'value'. |
| subject | String | self | (Optional) The subject of this filter test. |
| value | Boolean | true | (Optional) true or false. |


> **operator** 子参数


| Options | Description |
| --- | --- |
| != | Test for inequality. |
| < | Test for less-than the value. |
| <= | Test for less-than or equal to the value. |
| <> | Test for inequality. |
| = | Test for equality. |
| == | Test for equality. |
| > | Test for greater-than the value. |
| >= | Test for greater-than or equal to the value. |
| equals | Test for equality. |
| not | Test for inequality. |


> **subject** 子参数


| Options | Description |
| --- | --- |
| block | The block involved with the interaction. |
| damager | The damaging actor involved with the interaction. |
| other | The other member of an interaction, not the caller. |
| parent | The caller's current parent. |
| player | The player involved with the interaction. |
| self | The entity or object calling the test |
| target | The caller's current target. |


## Examples


Full..


```json
{ "test": "on_ladder", "subject": "self", "operator": "equals", "value": "true" }
```


Short (using Defaults)..


```json
{ "test": "on_ladder" }
```


# owner_distance


Tests the distance between the subject and its owner. Returns false if there is no owner.


| Name | Type | Default | Description |
| --- | --- | --- | --- |
| operator | String | equals | (Optional) The comparison to apply with 'value'. |
| subject | String | self | (Optional) The subject of this filter test. |
| value | Decimal |  | (Required) A floating point value. |


> **operator** 子参数


| Options | Description |
| --- | --- |
| != | Test for inequality. |
| < | Test for less-than the value. |
| <= | Test for less-than or equal to the value. |
| <> | Test for inequality. |
| = | Test for equality. |
| == | Test for equality. |
| > | Test for greater-than the value. |
| >= | Test for greater-than or equal to the value. |
| equals | Test for equality. |
| not | Test for inequality. |


> **subject** 子参数


| Options | Description |
| --- | --- |
| block | The block involved with the interaction. |
| damager | The damaging actor involved with the interaction. |
| other | The other member of an interaction, not the caller. |
| parent | The caller's current parent. |
| player | The player involved with the interaction. |
| self | The entity or object calling the test |
| target | The caller's current target. |


## Examples


Full..


```json
{ "test": "owner_distance", "subject": "self", "operator": "equals", "value": "0.00" }
```


Short (using Defaults)..


```json
{ "test": "owner_distance", "value": "0.00" }
```


# random_chance


Returns true if the random chance rolls 0 out of a specified max range.


| Name | Type | Default | Description |
| --- | --- | --- | --- |
| operator | String | equals | (Optional) The comparison to apply with 'value'. |
| subject | String | self | (Optional) The subject of this filter test. |
| value | Integer |  | (Required) An integer value. |


> **operator** 子参数


| Options | Description |
| --- | --- |
| != | Test for inequality. |
| < | Test for less-than the value. |
| <= | Test for less-than or equal to the value. |
| <> | Test for inequality. |
| = | Test for equality. |
| == | Test for equality. |
| > | Test for greater-than the value. |
| >= | Test for greater-than or equal to the value. |
| equals | Test for equality. |
| not | Test for inequality. |


> **subject** 子参数


| Options | Description |
| --- | --- |
| block | The block involved with the interaction. |
| damager | The damaging actor involved with the interaction. |
| other | The other member of an interaction, not the caller. |
| parent | The caller's current parent. |
| player | The player involved with the interaction. |
| self | The entity or object calling the test |
| target | The caller's current target. |


## Examples


Full..


```json
{ "test": "random_chance", "subject": "self", "operator": "equals", "value": "0" }
```


Short (using Defaults)..


```json
{ "test": "random_chance", "value": "0" }
```


# rider_count


Returns the number of riders on this entity.


| Name | Type | Default | Description |
| --- | --- | --- | --- |
| operator | String | equals | (Optional) The comparison to apply with 'value'. |
| subject | String | self | (Optional) The subject of this filter test. |
| value | Integer |  | (Required) An integer value. |


> **operator** 子参数


| Options | Description |
| --- | --- |
| != | Test for inequality. |
| < | Test for less-than the value. |
| <= | Test for less-than or equal to the value. |
| <> | Test for inequality. |
| = | Test for equality. |
| == | Test for equality. |
| > | Test for greater-than the value. |
| >= | Test for greater-than or equal to the value. |
| equals | Test for equality. |
| not | Test for inequality. |


> **subject** 子参数


| Options | Description |
| --- | --- |
| block | The block involved with the interaction. |
| damager | The damaging actor involved with the interaction. |
| other | The other member of an interaction, not the caller. |
| parent | The caller's current parent. |
| player | The player involved with the interaction. |
| self | The entity or object calling the test |
| target | The caller's current target. |


## Examples


Full..


```json
{ "test": "rider_count", "subject": "self", "operator": "equals", "value": "0" }
```


Short (using Defaults)..


```json
{ "test": "rider_count", "value": "0" }
```


# surface_mob


Tests if the subject is a surface mob.


| Name | Type | Default | Description |
| --- | --- | --- | --- |
| operator | String | equals | (Optional) The comparison to apply with 'value'. |
| subject | String | self | (Optional) The subject of this filter test. |
| value | Boolean | true | (Optional) true or false. |


> **operator** 子参数


| Options | Description |
| --- | --- |
| != | Test for inequality. |
| < | Test for less-than the value. |
| <= | Test for less-than or equal to the value. |
| <> | Test for inequality. |
| = | Test for equality. |
| == | Test for equality. |
| > | Test for greater-than the value. |
| >= | Test for greater-than or equal to the value. |
| equals | Test for equality. |
| not | Test for inequality. |


> **subject** 子参数


| Options | Description |
| --- | --- |
| block | The block involved with the interaction. |
| damager | The damaging actor involved with the interaction. |
| other | The other member of an interaction, not the caller. |
| parent | The caller's current parent. |
| player | The player involved with the interaction. |
| self | The entity or object calling the test |
| target | The caller's current target. |


## Examples


Full..


```json
{ "test": "surface_mob", "subject": "self", "operator": "equals", "value": "true" }
```


Short (using Defaults)..


```json
{ "test": "surface_mob" }
```


# taking_fire_damage


Tests if the subject is taking fire damage. Requires the damage_sensor component


| Name | Type | Default | Description |
| --- | --- | --- | --- |
| operator | String | equals | (Optional) The comparison to apply with 'value'. |
| subject | String | self | (Optional) The subject of this filter test. |
| value | Boolean | true | (Optional) true or false. |


> **operator** 子参数


| Options | Description |
| --- | --- |
| != | Test for inequality. |
| < | Test for less-than the value. |
| <= | Test for less-than or equal to the value. |
| <> | Test for inequality. |
| = | Test for equality. |
| == | Test for equality. |
| > | Test for greater-than the value. |
| >= | Test for greater-than or equal to the value. |
| equals | Test for equality. |
| not | Test for inequality. |


> **subject** 子参数


| Options | Description |
| --- | --- |
| block | The block involved with the interaction. |
| damager | The damaging actor involved with the interaction. |
| other | The other member of an interaction, not the caller. |
| parent | The caller's current parent. |
| player | The player involved with the interaction. |
| self | The entity or object calling the test |
| target | The caller's current target. |


## Examples


Full..


```json
{ "test": "taking_fire_damage", "subject": "self", "operator": "equals", "value": "true" }
```


Short (using Defaults)..


```json
{ "test": "taking_fire_damage" }
```


# target_distance


Tests the distance between the calling entity and its target.


| Name | Type | Default | Description |
| --- | --- | --- | --- |
| operator | String | equals | (Optional) The comparison to apply with 'value'. |
| subject | String | self | (Optional) The subject of this filter test. |
| value | Decimal |  | (Required) A floating point value. |


> **operator** 子参数


| Options | Description |
| --- | --- |
| != | Test for inequality. |
| < | Test for less-than the value. |
| <= | Test for less-than or equal to the value. |
| <> | Test for inequality. |
| = | Test for equality. |
| == | Test for equality. |
| > | Test for greater-than the value. |
| >= | Test for greater-than or equal to the value. |
| equals | Test for equality. |
| not | Test for inequality. |


> **subject** 子参数


| Options | Description |
| --- | --- |
| block | The block involved with the interaction. |
| damager | The damaging actor involved with the interaction. |
| other | The other member of an interaction, not the caller. |
| parent | The caller's current parent. |
| player | The player involved with the interaction. |
| self | The entity or object calling the test |
| target | The caller's current target. |


## Examples


Full..


```json
{ "test": "target_distance", "subject": "self", "operator": "equals", "value": "0.00" }
```


Short (using Defaults)..


```json
{ "test": "target_distance", "value": "0.00" }
```


# trusts


Returns true if the subject is trusted by entity.


| Name | Type | Default | Description |
| --- | --- | --- | --- |
| operator | String | equals | (Optional) The comparison to apply with 'value'. |
| subject | String | self | (Optional) The subject of this filter test. |
| value | Boolean | true | (Optional) true or false. |


> **operator** 子参数


| Options | Description |
| --- | --- |
| != | Test for inequality. |
| < | Test for less-than the value. |
| <= | Test for less-than or equal to the value. |
| <> | Test for inequality. |
| = | Test for equality. |
| == | Test for equality. |
| > | Test for greater-than the value. |
| >= | Test for greater-than or equal to the value. |
| equals | Test for equality. |
| not | Test for inequality. |


> **subject** 子参数


| Options | Description |
| --- | --- |
| block | The block involved with the interaction. |
| damager | The damaging actor involved with the interaction. |
| other | The other member of an interaction, not the caller. |
| parent | The caller's current parent. |
| player | The player involved with the interaction. |
| self | The entity or object calling the test |
| target | The caller's current target. |


## Examples


Full..


```json
{ "test": "trusts", "subject": "self", "operator": "equals", "value": "true" }
```


Short (using Defaults)..


```json
{ "test": "trusts" }
```


# was_last_hurt_by


Tests if the subject is the last player who attacked the entity in the last 400 seconds, or the last mob to do so in the last 60 seconds.


| Name | Type | Default | Description |
| --- | --- | --- | --- |
| operator | String | equals | (Optional) The comparison to apply with 'value'. |
| subject | String | self | (Optional) The subject of this filter test. |
| value | Boolean | true | (Optional) true or false. |


> **operator** 子参数


| Options | Description |
| --- | --- |
| != | Test for inequality. |
| < | Test for less-than the value. |
| <= | Test for less-than or equal to the value. |
| <> | Test for inequality. |
| = | Test for equality. |
| == | Test for equality. |
| > | Test for greater-than the value. |
| >= | Test for greater-than or equal to the value. |
| equals | Test for equality. |
| not | Test for inequality. |


> **subject** 子参数


| Options | Description |
| --- | --- |
| block | The block involved with the interaction. |
| damager | The damaging actor involved with the interaction. |
| other | The other member of an interaction, not the caller. |
| parent | The caller's current parent. |
| player | The player involved with the interaction. |
| self | The entity or object calling the test |
| target | The caller's current target. |


## Examples


Full..


```json
{ "test": "was_last_hurt_by", "subject": "self", "operator": "equals", "value": "true" }
```


Short (using Defaults)..


```json
{ "test": "was_last_hurt_by" }
```


# weather


Tests the current weather in the dimension against a provided weather value.


| Name | Type | Default | Description |
| --- | --- | --- | --- |
| operator | String | equals | (Optional) The comparison to apply with 'value'. |
| subject | String | self | (Optional) The subject of this filter test. |
| value | String |  | (Required) The Family name to look for |


> **operator** 子参数


| Options | Description |
| --- | --- |
| != | Test for inequality. |
| < | Test for less-than the value. |
| <= | Test for less-than or equal to the value. |
| <> | Test for inequality. |
| = | Test for equality. |
| == | Test for equality. |
| > | Test for greater-than the value. |
| >= | Test for greater-than or equal to the value. |
| equals | Test for equality. |
| not | Test for inequality. |


> **subject** 子参数


| Options | Description |
| --- | --- |
| block | The block involved with the interaction. |
| damager | The damaging actor involved with the interaction. |
| other | The other member of an interaction, not the caller. |
| parent | The caller's current parent. |
| player | The player involved with the interaction. |
| self | The entity or object calling the test |
| target | The caller's current target. |


## Examples


Full..


```json
{ "test": "weather", "subject": "self", "operator": "equals", "value": "player" }
```


Short (using Defaults)..


```json
{ "test": "weather", "value": "player" }
```


# weather_at_position


Tests the current weather, at the actor's position, against a provided weather value.


| Name | Type | Default | Description |
| --- | --- | --- | --- |
| operator | String | equals | (Optional) The comparison to apply with 'value'. |
| subject | String | self | (Optional) The subject of this filter test. |
| value | String |  | (Required) The Family name to look for |


> **operator** 子参数


| Options | Description |
| --- | --- |
| != | Test for inequality. |
| < | Test for less-than the value. |
| <= | Test for less-than or equal to the value. |
| <> | Test for inequality. |
| = | Test for equality. |
| == | Test for equality. |
| > | Test for greater-than the value. |
| >= | Test for greater-than or equal to the value. |
| equals | Test for equality. |
| not | Test for inequality. |


> **subject** 子参数


| Options | Description |
| --- | --- |
| block | The block involved with the interaction. |
| damager | The damaging actor involved with the interaction. |
| other | The other member of an interaction, not the caller. |
| parent | The caller's current parent. |
| player | The player involved with the interaction. |
| self | The entity or object calling the test |
| target | The caller's current target. |


## Examples


Full..


```json
{ "test": "weather_at_position", "subject": "self", "operator": "equals", "value": "player" }
```


Short (using Defaults)..


```json
{ "test": "weather_at_position", "value": "player" }
```


# y_rotation


Returns the Y rotation of this entity.


| Name | Type | Default | Description |
| --- | --- | --- | --- |
| operator | String | equals | (Optional) The comparison to apply with 'value'. |
| subject | String | self | (Optional) The subject of this filter test. |
| value | Decimal |  | (Required) A floating point value. |


> **operator** 子参数


| Options | Description |
| --- | --- |
| != | Test for inequality. |
| < | Test for less-than the value. |
| <= | Test for less-than or equal to the value. |
| <> | Test for inequality. |
| = | Test for equality. |
| == | Test for equality. |
| > | Test for greater-than the value. |
| >= | Test for greater-than or equal to the value. |
| equals | Test for equality. |
| not | Test for inequality. |


> **subject** 子参数


| Options | Description |
| --- | --- |
| block | The block involved with the interaction. |
| damager | The damaging actor involved with the interaction. |
| other | The other member of an interaction, not the caller. |
| parent | The caller's current parent. |
| player | The player involved with the interaction. |
| self | The entity or object calling the test |
| target | The caller's current target. |


## Examples


Full..


```json
{ "test": "y_rotation", "subject": "self", "operator": "equals", "value": "0.00" }
```


Short (using Defaults)..


```json
{ "test": "y_rotation", "value": "0.00" }
```


## Example:


This filter group will pass only when the moon_intensity is greater than 0.5 AND the caller's target entity is standing in water.


```json
"all_of" : [

:   { "test" : "moon_intensity", "subject" : "self", "operator" : "greater", "value" : "0.5" }, 

:   { "test" : "in_water", "subject" : "target", "operator" : "equal", "value" : "true" } 

: ]
```


# Server Entity Documentation


# AI Goals


## minecraft:behavior.admire_item


Enables the mob to admire items that have been configured as admirable. Must be used in combination with the admire_item component


| Name | Type | Default Value | Description |
| --- | --- | --- | --- |
| admire_item_sound | String |  | The sound event to play when admiring the item |
| sound_interval | Range [a, b] | 0 | The range of time in seconds to randomly wait before playing the sound again. |


## minecraft:behavior.avoid_block (See JSON Schema since 1.21.120)


Allows this entity to avoid certain blocks.


| Name | Type | Default Value | Description |
| --- | --- | --- | --- |
| avoid_block_sound | String |  | The sound event to play when the mob is avoiding a block. |
| on_escape | Array |  | Escape trigger. |
| search_height | Integer | 0 | Maximum distance to look for a block in y. |
| search_range | Integer | 0 | Maximum distance to look for a block in xz. |
| sound_interval | Range [a, b] | [3, 8] | The range of time in seconds to randomly wait before playing the sound again. |
| sprint_speed_modifier | Decimal | 1.0 | Modifier for sprint speed. 1.0 means keep the regular speed, while higher numbers make the sprint speed faster. |
| target_blocks | Array |  | List of block types this mob avoids. |
| target_selection_method | String | nearest | Block search method. |
| tick_interval | Integer | 1 | Should start tick interval. |
| walk_speed_modifier | Decimal | 1.0 | Modifier for walking speed. 1.0 means keep the regular speed, while higher numbers make the walking speed faster. |


## minecraft:behavior.avoid_mob_type (See JSON Schema since 1.21.120)


Allows the entity to run away from other entities that meet the criteria specified.


| Name | Type | Default Value | Description |
| --- | --- | --- | --- |
| avoid_mob_sound | String |  | The sound event to play when the mob is avoiding another mob. |
| avoid_target_xz | Integer | 16 | The next target position the entity chooses to avoid another entity will be chosen within this XZ Distance. |
| avoid_target_y | Integer | 7 | The next target position the entity chooses to avoid another entity will be chosen within this Y Distance. |
| entity_types | Minecraft Filter |  | The list of conditions another entity must meet to be a valid target to avoid. |
| ignore_visibility | Boolean | false | Whether or not to ignore direct line of sight while this entity is running away from other specified entities. |
| max_dist | Decimal | 3.0 | Maximum distance to look for an avoid target for the entity. |
| max_flee | Decimal | 10.0 | How many blocks away from its avoid target the entity must be for it to stop fleeing from the avoid target. |
| on_escape_event | Trigger |  | Event that is triggered when escaping from a mob. |
| probability_per_strength | Decimal | 1.0 | Percent chance this entity will stop avoiding another entity based on that entity's strength, where 1.0 = 100%. |
| remove_target | Boolean | false | Determine if we should remove target when fleeing or not. |
| sound_interval | Range [a, b] | [3, 8] | The range of time in seconds to randomly wait before playing the sound again. |
| sprint_distance | Decimal | 7.0 | How many blocks within range of its avoid target the entity must be for it to begin sprinting away from the avoid target. |
| sprint_speed_multiplier | Decimal | 1.0 | Multiplier for sprint speed. 1.0 means keep the regular speed, while higher numbers make the sprint speed faster. |
| walk_speed_multiplier | Decimal | 1.0 | Multiplier for walking speed. 1.0 means keep the regular speed, while higher numbers make the walking speed faster. |


## minecraft:behavior.barter


Enables the mob to barter for items that have been configured as barter currency. Must be used in combination with the barter component


## minecraft:behavior.beg


Allows this mob to look at and follow the player that holds food they like.


| Name | Type | Default Value | Description |
| --- | --- | --- | --- |
| items | List |  | List of items that this mob likes |
| look_distance | Decimal | 8.0 | Distance in blocks the mob will beg from |
| look_time | Range [a, b] | [2, 4] | The range of time in seconds this mob will stare at the player holding a food they like, begging for it |


## minecraft:behavior.break_door


Allows this mob to break doors.


## minecraft:behavior.breed


Allows this mob to breed with other mobs.


| Name | Type | Default Value | Description |
| --- | --- | --- | --- |
| speed_multiplier | Decimal | 1.0 | Movement speed multiplier of the mob when using this AI Goal |


## minecraft:behavior.celebrate


Allows this entity to celebrate surviving a raid by making celebration sounds and jumping.


| Name | Type | Default Value | Description |
| --- | --- | --- | --- |
| celebration_sound | String |  | The sound event to trigger during the celebration. |
| duration | Decimal | 30.000000 | The duration in seconds that the celebration lasts for. |
| jump_interval | Range [a, b] | [1, 3.5] | Minimum and maximum time between jumping (positive, in seconds). |
| on_celebration_end_event | Trigger |  | The event to trigger when the goal's duration expires. |
| sound_interval | Range [a, b] | [2, 7] | Minimum and maximum time between sound events (positive, in seconds). |


## minecraft:behavior.celebrate_survive


Allows this entity to celebrate surviving a raid by shooting fireworks.


| Name | Type | Default Value | Description |
| --- | --- | --- | --- |
| duration | Decimal | 30.000000 | The duration in seconds that the celebration lasts for. |
| fireworks_interval | Range [a, b] | [10, 20] | Minimum and maximum time between firework (positive, in seconds). |
| on_celebration_end_event | Trigger |  | The event to trigger when the goal's duration expires. |


## minecraft:behavior.charge_attack (See JSON Schema since 1.21.100)


Allows this entity to damage a target by using a running attack.


| Name | Type | Default Value | Description |
| --- | --- | --- | --- |
| max_distance | Decimal | 3 | A charge attack cannot start if the entity is farther than this distance to the target. |
| min_distance | Decimal | 2 | A charge attack cannot start if the entity is closer than this distance to the target. |
| speed_multiplier | Decimal | 1 | Modifies the entity's speed when charging toward the target. |
| success_rate | Decimal | 0.1428 | Percent chance this entity will start a charge attack, if not already attacking (1.0 = 100%) |


## minecraft:behavior.charge_held_item


Allows an entity to charge and use their held item.


| Name | Type | Default Value | Description |
| --- | --- | --- | --- |
| items | Array | NA | The list of items that can be used to charge the held item. This list is required and must have at least one item in it. |


## minecraft:behavior.circle_around_anchor (See JSON Schema since 1.21.100)


Causes an entity to circle around an anchor point placed near a point or target.


| Name | Type | Default Value | Description |
| --- | --- | --- | --- |
| angle_change | Decimal | 15.0 | Number of degrees to change this entity's facing by, when the entity selects its next anchor point. |
| goal_radius | Decimal | 0.5 | Maximum distance from the anchor-point in which this entity considers itself to have reached the anchor point. This is to prevent the entity from bouncing back and forth trying to reach a specific spot. |
| height_above_target_range | Range [a, b] | [0, 0] | The number of blocks above the target that the next anchor point can be set. This value is used only when the entity is tracking a target. |
| height_adjustment_chance | Decimal | 0.002857 | Percent chance to determine how often to increase or decrease the current height around the anchor point. 1 = 100%. "height_change_chance" is deprecated and has been replaced with "height_adjustment_chance". |
| height_offset_range | Range [a, b] | [0, 0] | Vertical distance from the anchor point this entity must stay within, upon a successful height adjustment. |
| radius_adjustment_chance | Decimal | 0.004 | Percent chance to determine how often to increase the size of the current movement radius around the anchor point. 1 = 100%. "radius_change_chance" is deprecated and has been replaced with "radius_adjustment_chance". |
| radius_change | Decimal | 1.0 | The number of blocks to increase the current movement radius by, upon successful "radius_adjustment_chance". If the current radius increases over the range maximum, the current radius will be set back to the range minimum and the entity will change between clockwise and counter-clockwise movement.. |
| radius_range | Range [a, b] | [5, 15] | Horizontal distance from the anchor point this entity must stay within upon a successful radius adjustment. |
| speed_multiplier | Decimal | 1.0 | Multiplies the speed at which this entity travels to its next desired position. |


## minecraft:behavior.controlled_by_player (See JSON Schema since 1.21.100)


Allows the entity to be controlled by the player using an item in the item_controllable property (required). Also requires the minecraft:movement property, and the minecraft:rideable property. On every tick, the entity will attempt to rotate towards where the player is facing with the control item whilst simultaneously moving forward.


| Name | Type | Default Value | Description |
| --- | --- | --- | --- |
| fractional_rotation | Decimal | 0.5 | The entity will attempt to rotate to face where the player is facing each tick. The entity will target this percentage of their difference in their current facing angles each tick (from 0.0 to 1.0 where 1.0 = 100%). This is limited by FractionalRotationLimit. A value of 0.0 will result in the entity no longer turning to where the player is facing. |
| fractional_rotation_limit | Decimal | 5.0 | Limits the total degrees the entity can rotate to face where the player is facing on each tick. |
| mount_speed_multiplier | Decimal | 1.0 | Speed multiplier of mount when controlled by player. |


## minecraft:behavior.croak


Allows the entity to croak at a random time interval with configurable conditions.


| Name | Type | Default Value | Description |
| --- | --- | --- | --- |
| duration | Range [a, b] | [4.5, 4.5] | Random range in seconds after which the croaking stops. Can also be a constant. |
| filters | Minecraft Filter |  | Conditions for the behavior to start and keep running. The interval between runs only starts after passing the filters. |
| interval | Range [a, b] | [10, 20] | Random range in seconds between runs of this behavior. Can also be a constant. |


## minecraft:behavior.defend_trusted_target


Allows the mob to target another mob that hurts an entity it trusts.


| Name | Type | Default Value | Description |
| --- | --- | --- | --- |
| aggro_sound | String |  | Sound to occasionally play while defending. |
| attack_interval | Integer | 0 | Time in seconds between attacks |
| entity_types | JSON Object |  | List of entity types that this mob considers valid targets |
| must_see | Boolean | false | If true, only entities in this mob's viewing range can be selected as targets |
| must_see_forget_duration | Decimal | 3.0 | Determines the amount of time in seconds that this mob will look for a target before forgetting about it and looking for a new one when the target isn't visible any more |
| within_radius | Decimal | 0.0 | Distance in blocks that the target can be within to launch an attack |


> **entity_types** 子参数


| Name | Type | Default Value | Description |
| --- | --- | --- | --- |
| cooldown | Decimal | 0.0 | The amount of time in seconds that the mob has to wait before selecting a target of the same type again |
| filters | Minecraft Filter |  | Conditions that make this entry in the list valid |
| max_dist | Decimal | 16 | Maximum distance this mob can be away to be a valid choice |
| must_see | Boolean | false | If true, the mob has to be visible to be a valid choice |
| must_see_forget_duration | Decimal | 3.0 | Determines the amount of time in seconds that this mob will look for a target before forgetting about it and looking for a new one when the target isn't visible any more |
| reevaluate_description | Boolean | false | If true, the mob will stop being targeted if it stops meeting any conditions. |
| sprint_speed_multiplier | Decimal | 1.0 | Multiplier for the running speed. A value of 1.0 means the speed is unchanged |
| walk_speed_multiplier | Decimal | 1.0 | Multiplier for the walking speed. A value of 1.0 means the speed is unchanged |


## minecraft:behavior.defend_village_target


Allows the entity to stay in a village and defend the village from aggressors. If a player is in bad standing with the village this goal will cause the entity to attack the player regardless of filter conditions.


| Name | Type | Default Value | Description |
| --- | --- | --- | --- |
| attack_chance | Decimal | 0.05 | The percentage chance that the entity has to attack aggressors of its village, where 1.0 = 100%. |
| attack_owner | Boolean | false | If true, this entity can attack its owner. |
| entity_types | Minecraft Filter |  | Filters which types of targets are valid for this entity. |
| must_reach | Boolean | false | If true, this entity requires a path to the target. |
| must_see | Boolean | false | Determines if target-validity requires this entity to be in range only, or both in range and in sight. |
| must_see_forget_duration | Decimal | 3.0 | Time (in seconds) the target must not be seen by this entity to become invalid. Used only if "must_see" is true. |
| persist_time | Decimal | 0.0 | Time (in seconds) this entity can continue attacking the target after the target is no longer valid. |
| within_radius | Decimal | 0.0 | Maximum distance this entity can be from the target when following it, otherwise the target becomes invalid. This value is only used if the entity doesn't declare "minecraft:follow_range". |


## minecraft:behavior.delayed_attack


Allows an entity to attack, while also delaying the damage-dealt until a specific time in the attack animation.


| Name | Type | Default Value | Description |
| --- | --- | --- | --- |
| attack_duration | Decimal | 0.75 | The entity's attack animation will play out over this duration (in seconds). Also controls attack cooldown. |
| attack_once | Boolean | false | Allows the entity to use this attack behavior, only once EVER. |
| attack_types | String | N/A | Defines the entity types this entity will attack. |
| can_spread_on_fire | Boolean | false | If the entity is on fire, this allows the entity's target to catch on fire after being hit. |
| hit_delay_pct | Decimal | 0.5 | The percentage into the attack animation to apply the damage of the attack (1.0 = 100%). |
| inner_boundary_time_increase | Decimal | 0.25 | Time (in seconds) to add to attack path recalculation when the target is beyond the "path_inner_boundary". |
| max_path_time | Decimal | 0.55 | Maximum base time (in seconds) to recalculate new attack path to target (before increases applied). |
| melee_fov | Decimal | 90 | Field of view (in degrees) when using the sensing component to detect an attack target. |
| min_path_time | Decimal | 0.2 | Minimum base time (in seconds) to recalculate new attack path to target (before increases applied). |
| on_attack | Trigger | N/A | Defines the event to trigger when this entity successfully attacks. |
| on_kill | Trigger | N/A | Defines the event to trigger when this entity kills the target. |
| outer_boundary_time_increase | Decimal | 0.5 | Time (in seconds) to add to attack path recalculation when the target is beyond the "path_outer_boundary". |
| path_fail_time_increase | Decimal | 0.75 | Time (in seconds) to add to attack path recalculation when this entity cannot move along the current path. |
| path_inner_boundary | Decimal | 16 | Distance at which to increase attack path recalculation by "inner_boundary_tick_increase". |
| path_outer_boundary | Decimal | 32 | Distance at which to increase attack path recalculation by "outer_boundary_tick_increase". |
| random_stop_interval | Integer | 0 | This entity will have a 1 in N chance to stop it's current attack, where N = "random_stop_interval". |
| reach_multiplier | Decimal | 1.5 | Used with the base size of the entity to determine minimum target-distance before trying to deal attack damage. |
| require_complete_path | Boolean | false | Toggles (on/off) the need to have a full path from the entity to the target when using this melee attack behavior. |
| speed_multiplier | Decimal | 1 | This multiplier modifies the attacking entity's speed when moving toward the target. |
| track_target | Boolean | true | Allows the entity to track the attack target, even if the entity has no sensing. |
| x_max_rotation | Decimal | 30 | Maximum rotation (in degrees), on the X-axis, this entity can rotate while trying to look at the target. |
| y_max_head_rotation | Decimal | 30 | Maximum rotation (in degrees), on the Y-axis, this entity can rotate its head while trying to look at the target. |


## minecraft:behavior.dig (See JSON Schema since 1.21.120)


Allows this entity to dig into the ground before despawning.


| Name | Type | Default Value | Description |
| --- | --- | --- | --- |
| allow_dig_when_named | Boolean | false | If true, this behavior can run when this entity is named. Otherwise not. |
| digs_in_daylight | Boolean | false | Indicates that the actor should start digging when it sees daylight |
| duration | Decimal | 0.0 | Goal duration in seconds |
| idle_time | Decimal | 0.0 | The minimum idle time in seconds between the last detected disturbance to the start of digging. |
| on_start | Trigger |  | Event(s) to run when the goal starts. |
| suspicion_is_disturbance | Boolean | false | If true, finding new suspicious locations count as disturbances that may delay the start of this goal. |
| vibration_is_disturbance | Boolean | false | If true, vibrations count as disturbances that may delay the start of this goal. |


## minecraft:behavior.door_interact


Allows the mob to open and close doors.


## minecraft:behavior.dragonchargeplayer


Allows this entity to attack a player by charging at them. The player is chosen by the "minecraft:behavior.dragonscanning". Can only be used by the Ender Dragon.


| Name | Type | Default Value | Description |
| --- | --- | --- | --- |
| active_speed | Decimal | 3 | The speed this entity moves when this behavior has started or while it's active. |
| continue_charge_threshold_time | Decimal | 0.5 | If the dragon is outside the "target_zone" for longer than "continue_charge_threshold_time" seconds, the charge is canceled. |
| flight_speed | Decimal | 0.6 | The speed this entity moves while this behavior is not active. |
| target_zone | Range [a, b] | [10, 150] | Minimum and maximum distance, from the target, this entity can use this behavior. |
| turn_speed | Decimal | 0.7 | The speed at which this entity turns while using this behavior. |


## minecraft:behavior.dragondeath


Allows the dragon to go out with glory. This controls the Ender Dragon's death animation and can't be used by other mobs.


## minecraft:behavior.dragonflaming (See JSON Schema since 1.21.100)


Allows this entity to use a flame-breath attack. Can only be used by the Ender Dragon.


| Name | Type | Default Value | Description |
| --- | --- | --- | --- |
| cooldown_time | Decimal | 10 | Time (in seconds) between each start of the cycle to roar, then breath flame. |
| flame_time | Decimal | 0.5 | Time (in seconds), after roar, to breath flame. |
| ground_flame_count | Integer | 4 | Number of ground flame-breath attacks to use before flight-takeoff. |
| roar_time | Decimal | 2 | Time (in seconds) to roar, before breathing flame. |


## minecraft:behavior.dragonholdingpattern


Allows the Dragon to fly around in a circle around the center podium. Can only be used by the Ender Dragon.


## minecraft:behavior.dragonlanding


Allows the Dragon to stop flying and transition into perching mode. Can only be used by the Ender Dragon.


## minecraft:behavior.dragonscanning


Allows the dragon to look around for a player to attack while in perch mode. Can only be used by the Ender Dragon.


## minecraft:behavior.dragonstrafeplayer


Allows this entity to fly around looking for a player to shoot fireballs at. Can only be used by the Ender Dragon.


| Name | Type | Default Value | Description |
| --- | --- | --- | --- |
| active_speed | Decimal | 0.6 | The speed this entity moves when this behavior has started or while it's active. |
| fireball_range | Decimal | 64 | Maximum distance of this entity's fireball attack while strafing. |
| flight_speed | Decimal | 0.6 | The speed this entity moves while this behavior is not active. |
| switch_direction_probability | Decimal | 0.125 | Percent chance to to switch this entity's strafe direction between clockwise and counterclockwise. Switch direction chance occurs each time a new target is chosen (1.0 = 100%). |
| target_in_range_and_in_view_time | Decimal | 0.25 | Time (in seconds) the target must be in fireball range, and in view [ie, no solid terrain in-between the target and this entity], before a fireball can be shot. |
| target_zone | Range [a, b] | [10, 150] | Minimum and maximum distance, from the target, this entity can use this behavior. |
| turn_speed | Decimal | 0.7 | The speed at which this entity turns while using this behavior. |
| view_angle | Decimal | 10 | The target must be within "view_angle" degrees of the dragon's current rotation before a fireball can be shot. |


## minecraft:behavior.dragontakeoff


Allows the dragon to leave perch mode and go back to flying around. Can only be used by the Ender Dragon.


## minecraft:behavior.drink_milk (See JSON Schema since 1.21.120)


Allows the mob to drink milk based on specified environment conditions.


| Name | Type | Default Value | Description |
| --- | --- | --- | --- |
| cooldown_seconds | Decimal | 5.00 | Time (in seconds) that the goal is on cooldown before it can be used again. |
| filters | Minecraft Filter |  | Conditions that need to be met for the behavior to start. |


## minecraft:behavior.drink_potion


Allows the mob to drink potions based on specified environment conditions.


| Name | Type | Default Value | Description |
| --- | --- | --- | --- |
| potions | List |  | A list of potions that this entity can drink. Each potion entry has the following parameters: |
| speed_modifier | Decimal | 0.0 | The movement speed modifier to apply to the entity while it is drinking a potion. A value of 0 represents no change in speed. |


> **potions** 子参数


| Name | Type | Default Value | Description |
| --- | --- | --- | --- |
| chance | Decimal | 1.0 | The percent chance (from 0.0 to 1.0) of this potion being selected when searching for a potion to use. |
| filters | Minecraft Filter |  | The filters to use when determining if this potion can be selected. |
| id | Integer | -1 | The registry ID of the potion to use |


## minecraft:behavior.drop_item_for


Allows the entity to move toward a target, and drop an item near the target. This goal requires a "minecraft:navigation" to execute.


| Name | Type | Default Value | Description |
| --- | --- | --- | --- |
| cooldown | Decimal | 0.2 | Total time that the goal is on cooldown before it can be used again. |
| drop_item_chance | Decimal | 1.0 | The percent chance the entity will drop an item when using this goal. |
| entity_types | Minecraft Filter |  | The list of conditions another entity must meet to be a valid target to drop an item for. |
| goal_radius | Decimal | 0.50 | Distance in blocks within the entity considers it has reached it's target position. |
| loot_table | String |  | The loot table that contains the possible loot the entity can drop with this goal. |
| max_head_look_at_height | Decimal | 10.0 | The maximum height the entities head will look at when dropping the item. The entity will always be looking at its target. |
| minimum_teleport_distance | Decimal | 2.0 | If the target position is farther away than this distance on any tick, the entity will teleport to the target position. |
| offering_distance | Decimal | 1.0 | The preferred distance the entity tries to be from the target it is dropping an item for. |
| on_drop_attempt | Trigger |  | The event to trigger when the entity attempts to drop an item. |
| search_count | Integer | 0 | The number of blocks each tick that the entity will check within its search range and height for a valid block to move to. A value of 0 will have the mob check every block within range in one tick. |
| search_height | Integer | 1 | The Height in blocks the entity will search within to find a valid target position. |
| search_range | Integer | 0 | The distance in blocks the entity will search within to find a valid target position. |
| seconds_before_pickup | Decimal | 0.0 | The numbers of seconds that will pass before the dropped entity can be picked up from the ground. |
| speed_multiplier | Decimal | 1.00 | Movement speed multiplier of the entity when using this Goal. |
| target_range | Vector [a, b, c] | [1, 1, 1] | The range in blocks within which the entity searches to find a target to drop an item for. |
| teleport_offset | Vector [a, b, c] | [0, 1, 0] | When the entity teleports, offset the teleport position by this many blocks in the X, Y, and Z coordinate. |
| time_of_day_range | Range [a, b] | [0, 1] | The valid times of day that this goal can be used. For reference: noon is 0.0, sunset is 0.25, midnight is 0.5, and sunrise is 0.75, and back to noon for 1.0. |


## minecraft:behavior.eat_block


Allows the entity to consume a block, replace the eaten block with another block, and trigger an event as a result.


| Name | Type | Default Value | Description |
| --- | --- | --- | --- |
| eat_and_replace_block_pairs | Array | N/A | A collection of pairs of blocks; the first ("eat_block")is the block the entity should eat, the second ("replace_block") is the block that should replace the eaten block. |
| on_eat | Trigger | N/A | The event to trigger when the block eating animation has completed. |
| success_chance | Molang | 0.02 | A molang expression defining the success chance the entity has to consume a block. |
| time_until_eat | Decimal | 1.8 | The amount of time (in seconds) it takes for the block to be eaten upon a successful eat attempt. |


## minecraft:behavior.eat_carried_item


If the mob is carrying a food item, the mob will eat it and the effects will be applied to the mob.


| Name | Type | Default Value | Description |
| --- | --- | --- | --- |
| delay_before_eating | Decimal |  | Time in seconds the mob should wait before eating the item. |


## minecraft:behavior.eat_mob (See JSON Schema since 1.21.100)


Allows the entity to eat a specified Mob.


| Name | Type | Default Value | Description |
| --- | --- | --- | --- |
| eat_animation_time | Decimal | 1.000000 | Sets the time in seconds the eat animation should play for. |
| eat_mob_sound | String |  | Sets the sound that should play when eating a mob. |
| loot_table | String |  | The loot table for loot to be dropped when eating a mob. |
| pull_in_force | Decimal | 1.000000 | Sets the force which the mob-to-be-eaten is pulled towards the eating mob. |
| reach_mob_distance | Decimal | 1.000000 | Sets the desired distance to be reached before eating the mob. |
| run_speed | Decimal | 1.000000 | Sets the entity's speed when running toward the target. |


## minecraft:behavior.emerge


Allows this entity to emerge from the ground


| Name | Type | Default Value | Description |
| --- | --- | --- | --- |
| cooldown_time | Decimal | 0.50 | Time in seconds the mob has to wait before using the goal again |
| duration | Decimal | 5.00 | Goal duration in seconds |
| on_done | Trigger |  | Trigger to be executed when the goal execution is about to end |


## minecraft:behavior.enderman_leave_block


Allows the enderman to drop a block they are carrying. Can only be used by Endermen.


## minecraft:behavior.enderman_take_block


Allows the enderman to take a block and carry it around. Can only be used by Endermen.


## minecraft:behavior.equip_item


The entity puts on the desired equipment.


## minecraft:behavior.explore_outskirts


Allows the entity to first travel to a random point on the outskirts of the village, and then explore random points within a small distance. This goal requires "minecraft:dweller" and "minecraft:navigation" to execute.


| Name | Type | Default Value | Description |
| --- | --- | --- | --- |
| dist_from_boundary | Vector [a, b, c] | [5, 0, 5] | The distance from the boundary the villager must be within in to explore the outskirts. |
| explore_dist | Decimal | 5.00 | Total distance in blocks the the entity will explore beyond the village bounds when choosing its travel point. |
| max_travel_time | Decimal | 60.0 | This is the maximum amount of time an entity will attempt to reach it's travel point on the outskirts of the village before the goal exits. |
| max_wait_time | Decimal | 0.0 | The wait time in seconds between choosing new explore points will be chosen on a random interval between this value and the minimum wait time. This value is also the total amount of time the entity will explore random points before the goal stops. |
| min_dist_from_target | Decimal | 2.2 | The entity must be within this distance for it to consider it has successfully reached its target. |
| min_perimeter | Decimal | 1.0 | The minimum perimeter of the village required to run this goal. |
| min_wait_time | Decimal | 3.0 | The wait time in seconds between choosing new explore points will be chosen on a random interval between this value and the maximum wait time. |
| next_xz | Integer | 5 | A new explore point will randomly be chosen within this XZ distance of the current target position when navigation has finished and the wait timer has elapsed. |
| next_y | Integer | 3 | A new explore point will randomly be chosen within this Y distance of the current target position when navigation has finished and the wait timer has elapsed. |
| speed_multiplier | Decimal | 1.0 | The multiplier for speed while using this goal. 1.0 maintains the speed. |
| timer_ratio | Decimal | 2.0 | Each new explore point will be chosen on a random interval between the minimum and the maximum wait time, divided by this value. This does not apply to the first explore point chosen when the goal runs. |


## minecraft:behavior.fertilize_farm_block


Allows the mob to search within an area for a growable crop block. If found, the mob will use any available fertilizer in their inventory on the crop. This goal will not execute if the mob does not have a fertilizer item in its inventory.


| Name | Type | Default Value | Description |
| --- | --- | --- | --- |
| goal_radius | Decimal | 1.50 | Distance in blocks within the mob considers it has reached it's target position. |
| max_fertilizer_usage | Integer | 1 | The maximum number of times the mob will use fertilzer on the target block. |
| search_cooldown_max_seconds | Decimal | 8.0 | The maximum amount of time in seconds that the goal can take before searching again. The time is chosen between 0 and this number. |
| search_count | Integer | 9 | The number of randomly selected blocks each tick that the mob will check within its search range and height for a valid block to move to. A value of 0 will have the mob check every block within range in one tick. |
| search_height | Integer | 1 | The Height in blocks the mob will search within to find a valid target position. |
| search_range | Integer | 1 | The distance in blocks the mob will search within to find a valid target position. |
| speed_multiplier | Decimal | 0.50 | Movement speed multiplier of the mob when using this Goal. |


## minecraft:behavior.find_cover


Allows the mob to seek shade.


| Name | Type | Default Value | Description |
| --- | --- | --- | --- |
| cooldown_time | Decimal | 0.0 | Time in seconds the mob has to wait before using the goal again |
| speed_multiplier | Decimal | 1.0 | Movement speed multiplier of the mob when using this AI Goal |


## minecraft:behavior.find_mount


Allows the mob to look around for another mob to ride atop it.


| Name | Type | Default Value | Description |
| --- | --- | --- | --- |
| avoid_water | Boolean | false | If true, the mob will not go into water blocks when going towards a mount |
| mount_distance | Decimal | -1.0 | This is the distance the mob needs to be, in blocks, from the desired mount to mount it. If the value is below 0, the mob will use its default attack distance |
| start_delay | Integer | 0 | Time the mob will wait before starting to move towards the mount |
| target_needed | Boolean | false | If true, the mob will only look for a mount if it has a target |
| within_radius | Decimal | 0.0 | Distance in blocks within which the mob will look for a mount |


## minecraft:behavior.find_underwater_treasure


Allows the mob to move towards the nearest underwater ruin or shipwreck.


| Name | Type | Default Value | Description |
| --- | --- | --- | --- |
| search_range | Integer | 0 | The range that the mob will search for a treasure chest within a ruin or shipwreck to move towards. |
| speed_multiplier | Decimal | 1.0 | Movement speed multiplier of the mob when using this AI Goal |
| stop_distance | Decimal | 2 | The distance the mob will move before stopping. |


## minecraft:behavior.fire_at_target


Allows an entity to attack by firing a shot with a delay. Anchor and offset parameters of this component overrides the anchor and offset from projectile component.


| Name | Type | Default Value | Description |
| --- | --- | --- | --- |
| attack_cooldown | Decimal | 0.500000 | The cooldown time in seconds before this goal can be used again. |
| attack_range | Range [a, b] | [2.000000, 16.000000] | Target needs to be within this range for the attack to happen. |
| filters | Minecraft Filter |  | Conditions that need to be met for the behavior to start. |
| max_head_rotation_x | Decimal | 30.000000 | Maximum head rotation (in degrees), on the X-axis, that this entity can apply while trying to look at the target. |
| max_head_rotation_y | Decimal | 30.000000 | Maximum head rotation (in degrees), on the Y-axis, that this entity can apply while trying to look at the target. |
| owner_anchor | Integer | 2 | Entity anchor for the projectile spawn location. |
| owner_offset | Vector [a, b, c] | [0.000, 0.000, 0.000] | Offset vector from the owner_anchor. |
| post_shoot_delay | Decimal | 0.200000 | Time in seconds between firing the projectile and ending the goal. |
| pre_shoot_delay | Decimal | 0.750000 | Time in seconds before firing the projectile. |
| projectile_def | String |  | Actor definition to use as projectile for the ranged attack. The actor must be a projectile. This field is required for the goal to be usable. |
| ranged_fov | Decimal | 90.000000 | Field of view (in degrees) when using sensing to detect a target for attack. |
| target_anchor | Integer | 2 | Entity anchor for projectile target. |
| target_offset | Vector [a, b, c] | [0.000, 0.000, 0.000] | Offset vector from the target_anchor. |


## minecraft:behavior.flee_sun


Allows the mob to run away from direct sunlight and seek shade.


| Name | Type | Default Value | Description |
| --- | --- | --- | --- |
| speed_multiplier | Decimal | 1.0 | Movement speed multiplier of the mob when using this AI Goal |


## minecraft:behavior.float


Allows the mob to stay afloat while swimming. Passengers will be kicked out the moment the mob's head goes underwater, which may not happen for tall mobs.


| Name | Type | Default Value | Description |
| --- | --- | --- | --- |
| sink_with_passengers | Boolean | false | If true, the mob will keep sinking as long as it has passengers. |


## minecraft:behavior.float_tempt


Allows a mob to be tempted by a player holding a specific item. Uses point-to-point movement. Designed for mobs that are floating (e.g. use the "minecraft:navigation.float" component).


| Name | Type | Default Value | Description |
| --- | --- | --- | --- |
| can_get_scared | Boolean | false | If true, the mob can stop being tempted if the player moves too fast while close to this mob. |
| can_tempt_vertically | Boolean | false | If true, vertical distance to the player will be considered when tempting. |
| can_tempt_while_ridden | Boolean | false | If true, the mob can be tempted even if it has a passenger (i.e. if being ridden). |
| items | Array | [] | List of items that can tempt the mob. |
| sound_interval | Range [a, b] | [0.0, 0.0] | Range of random ticks to wait between tempt sounds. |
| speed_multiplier | Decimal | 1.0 | Movement speed multiplier of the mob when using this AI Goal |
| stop_distance | Decimal | 1.5 | The distance at which the mob will stop following the player. |
| tempt_sound | String |  | Sound to play while the mob is being tempted. |
| within_radius | Decimal | 0.0 | Distance in blocks this mob can get tempted by a player holding an item they like. |


## minecraft:behavior.float_wander


Allows the mob to float around like the Ghast.


| Name | Type | Default Value | Description |
| --- | --- | --- | --- |
| additional_collision_buffer | Boolean | false | If true, the mob will have an additional buffer zone around it to avoid collisions with blocks when picking a position to wander to. |
| allow_navigating_through_liquids | Boolean | false | If true allows the mob to navigate through liquids on its way to the target position. |
| float_duration | Range [a, b] | [0.0, 0.0] | Range of time in seconds the mob will float around before landing and choosing to do something else |
| float_wander_has_move_control | Boolean | true | If true, the MoveControl flag will be added to the behavior which means that it can no longer be active at the same time as other behaviors with MoveControl. |
| must_reach | Boolean | false | If true, the point has to be reachable to be a valid target |
| navigate_around_surface | Boolean | false | If true, will prioritize finding random positions in the vicinity of surfaces, i.e. blocks that are not Air or Liquid. |
| random_reselect | Boolean | false | If true, the mob will randomly pick a new point while moving to the previously selected one |
| surface_xz_dist | Integer | 0 | The horizontal distance in blocks that the goal will check for a surface from a candidate position. Only valid when `navigate_around_surface` is true. |
| surface_y_dist | Integer | 0 | The vertical distance in blocks that the goal will check for a surface from a candidate position. Only valid when `navigate_around_surface` is true. |
| use_home_position_restriction | Boolean | true | If true, the mob will respect home position restrictions when choosing new target positions. If false, it will choose target position without considering home restrictions |
| xz_dist | Integer | 10 | Distance in blocks on ground that the mob will look for a new spot to move to. Must be at least 1 |
| y_dist | Integer | 7 | Distance in blocks that the mob will look up or down for a new spot to move to. Must be at least 1 |
| y_offset | Decimal | 0.0 | Height in blocks to add to the selected target position |


## minecraft:behavior.follow_caravan


Allows the mob to follow mobs that are in a caravan.


| Name | Type | Default Value | Description |
| --- | --- | --- | --- |
| entity_count | Integer | 1 | Number of entities that can be in the caravan |
| entity_types | JSON Object |  | List of entity types that this mob can follow in a caravan |
| speed_multiplier | Decimal | 1.0 | Movement speed multiplier of the mob when using this AI Goal |


> **entity_types** 子参数


| Name | Type | Default Value | Description |
| --- | --- | --- | --- |
| cooldown | Decimal | 0.0 | The amount of time in seconds that the mob has to wait before selecting a target of the same type again |
| filters | Minecraft Filter |  | Conditions that make this entry in the list valid |
| max_dist | Decimal | 16 | Maximum distance this mob can be away to be a valid choice |
| must_see | Boolean | false | If true, the mob has to be visible to be a valid choice |
| must_see_forget_duration | Decimal | 3.0 | Determines the amount of time in seconds that this mob will look for a target before forgetting about it and looking for a new one when the target isn't visible any more |
| reevaluate_description | Boolean | false | If true, the mob will stop being targeted if it stops meeting any conditions. |
| sprint_speed_multiplier | Decimal | 1.0 | Multiplier for the running speed. A value of 1.0 means the speed is unchanged |
| walk_speed_multiplier | Decimal | 1.0 | Multiplier for the walking speed. A value of 1.0 means the speed is unchanged |


## minecraft:behavior.follow_mob


Allows the mob to follow other mobs.


| Name | Type | Default Value | Description |
| --- | --- | --- | --- |
| filters | Minecraft Filter |  | If non-empty, provides criteria for filtering which nearby Mobs can be followed. If empty default criteria will be used, which will exclude Players, Squid variants, Fish variants, Tadpoles, Dolphins, and mobs of the same type as the owner of the Goal. |
| preferred_actor_type | String |  | The type of actor to prefer following. If left unspecified, a random actor among those in range will be chosen. |
| search_range | Integer | 0 | The distance in blocks it will look for a mob to follow |
| speed_multiplier | Decimal | 1.0 | Movement speed multiplier of the mob when using this AI Goal |
| stop_distance | Decimal | 2 | The distance in blocks this mob stops from the mob it is following |
| use_home_position_restriction | Boolean | true | If true, the mob will respect the 'minecraft:home' component's 'restriction_radius' field when choosing a target to follow. If false, it will choose target position without considering home restrictions. |


## minecraft:behavior.follow_owner


Allows a mob to follow the player that owns it.


| Name | Type | Default Value | Description |
| --- | --- | --- | --- |
| can_teleport | Boolean | true | Defines if the mob will teleport to its owner when too far away. |
| ignore_vibration | Boolean | true | Defines if the mob should disregard following its owner after detecting a recent vibration. |
| max_distance | Decimal | 60.0 | The maximum distance the mob can be from its owner to start following it. Applicable only when "can_teleport" is set to false. |
| post_teleport_distance | Boolean | "stop_distance" + 1 | Defines how far (in blocks) the entity will be from its owner after teleporting. If not specified, it defaults to "stop_distance" + 1, allowing the entity to seamlessly resume navigation. |
| speed_multiplier | Decimal | 1.0 | Movement speed multiplier of the mob when using this AI Goal |
| start_distance | Decimal | 10.0 | The minimum distance the mob must be from its owner to start following it. |
| stop_distance | Decimal | 2 | The distance at which the mob will stop following its owner. |


## minecraft:behavior.follow_parent


Allows the mob to follow their parent around.


| Name | Type | Default Value | Description |
| --- | --- | --- | --- |
| speed_multiplier | Decimal | 1.0 | Movement speed multiplier of the mob when using this AI Goal |


## minecraft:behavior.follow_target_captain


Allows mob to move towards its current target captain.


| Name | Type | Default Value | Description |
| --- | --- | --- | --- |
| follow_distance | Decimal | 0.0 | Defines the distance in blocks the mob will stay from its target while following. |
| within_radius | Decimal | 0.0 | Defines the maximum distance in blocks a mob can get from its target captain before giving up trying to follow it. |


## minecraft:behavior.go_and_give_items_to_noteblock


The entity will attempt to toss the items from its inventory to a nearby recently played noteblock.


| Name | Type | Default Value | Description |
| --- | --- | --- | --- |
| listen_time | Integer | 30 | Sets the time an entity should continue delivering items to a noteblock after hearing it. |
| on_item_throw | Array |  | Event(s) to run when this mob throws items. |
| reach_block_distance | Decimal | 3.000000 | Sets the desired distance to be reached before throwing the items towards the block. |
| run_speed | Decimal | 1.000000 | Sets the entity's speed when running toward the block. |
| throw_force | Decimal | 0.200000 | Sets the throw force. |
| throw_sound | String |  | Sound to play when this mob throws an item. |
| vertical_throw_mul | Decimal | 1.500000 | Sets the vertical throw multiplier that is applied on top of the throw force in the vertical direction. |


## minecraft:behavior.go_and_give_items_to_owner


The entity will attempt to toss the items from its inventory to its owner.


| Name | Type | Default Value | Description |
| --- | --- | --- | --- |
| on_item_throw | Array |  | Event(s) to run when this mob throws items. |
| reach_mob_distance | Decimal | 3.000000 | Sets the desired distance to be reached before giving items to owner. |
| run_speed | Decimal | 1.000000 | Sets the entity's speed when running toward the owner. |
| throw_force | Decimal | 0.200000 | Sets the throw force. |
| throw_sound | String | item_thrown | Sound to play when this mob throws an item. |
| vertical_throw_mul | Decimal | 1.500000 | Sets the vertical throw multiplier that is applied on top of the throw force in the vertical direction. |


## minecraft:behavior.go_home


Allows the mob to move back to the position they were spawned.


| Name | Type | Default Value | Description |
| --- | --- | --- | --- |
| calculate_new_path_radius | Decimal | 2.00 | Distance in blocks that the mob is considered close enough to the end of the current path. A new path will then be calculated to continue toward home. |
| goal_radius | Decimal | 0.50 | Distance in blocks within the mob considers it has reached the goal. This is the "wiggle room" to stop the AI from bouncing back and forth trying to reach a specific spot. |
| interval | Integer | 120 | A random value to determine when to randomly move somewhere. This has a 1/interval chance to choose this goal. |
| on_failed | Array |  | Event(s) to run when this goal fails. |
| on_home | Array |  | Event(s) to run when this mob gets home. |
| speed_multiplier | Decimal | 1.0 | Movement speed multiplier of the mob when using this AI Goal. |


## minecraft:behavior.guardian_attack


Allows this entity to use a laser beam attack. Can only be used by Guardians and Elder Guardians.


| Name | Type | Default Value | Description |
| --- | --- | --- | --- |
| elder_extra_magic_damage | Integer | 2 | Amount of additional damage dealt from an elder guardian's magic attack. |
| hard_mode_extra_magic_damage | Integer | 2 | In hard difficulty, amount of additional damage dealt from a guardian's magic attack. |
| magic_damage | Integer | 1 | Amount of damage dealt from a guardian's magic attack. Magic attack damage is added to the guardian's base attack damage. |
| min_distance | Decimal | 3 | Guardian attack behavior stops if the target is closer than this distance (doesn't apply to elders). |
| sound_delay_time | Decimal | 0.5 | Time (in seconds) to wait after starting an attack before playing the guardian attack sound. |
| x_max_rotation | Decimal | 90 | Maximum rotation (in degrees), on the X-axis, this entity can rotate while trying to look at the target. |
| y_max_head_rotation | Decimal | 90 | Maximum rotation (in degrees), on the Y-axis, this entity can rotate its head while trying to look at the target. |


## minecraft:behavior.harvest_farm_block


Allows the entity to search within an area for farmland with air above it. If found, the entity will replace the air block by planting a seed item from its inventory on the farmland block. This goal requires "minecraft:inventory" and "minecraft:navigation" to execute. This goal will not execute if the entity does not have an item in its inventory.


| Name | Type | Default Value | Description |
| --- | --- | --- | --- |
| goal_radius | Decimal | 1.50 | Distance in blocks within the entity considers it has reached it's target position. |
| max_seconds_before_search | Decimal | 1.0 | The maximum amount of time in seconds that the goal can take before searching for the first harvest block. The time is chosen between 0 and this number. |
| search_cooldown_max_seconds | Decimal | 8.0 | The maximum amount of time in seconds that the goal can take before searching again, after failing to find a a harvest block already. The time is chosen between 0 and this number. |
| search_count | Integer | 0 | The number of randomly selected blocks each tick that the entity will check within its search range and height for a valid block to move to. A value of 0 will have the mob check every block within range in one tick. |
| search_height | Integer | 1 | The Height in blocks the entity will search within to find a valid target position. |
| search_range | Integer | 16 | The distance in blocks the entity will search within to find a valid target position. |
| seconds_until_new_task | Decimal | 0.5 | The amount of time in seconds that the goal will cooldown after a successful reap/sow, before it can start again. |
| speed_multiplier | Decimal | 0.50 | Movement speed multiplier of the entity when using this Goal. |


## minecraft:behavior.hide


Allows a mob with the hide component to attempt to move to - and hide at - an owned or nearby POI.


| Name | Type | Default Value | Description |
| --- | --- | --- | --- |
| duration | Decimal | 1.0 | Amount of time in seconds that the mob reacts. |
| poi_type | String |  | Defines what POI type to hide at. |
| speed_multiplier | Decimal | 1.0 | Movement speed multiplier of the mob when using this AI Goal |
| timeout_cooldown | Decimal | 8.0 | The cooldown time in seconds before the goal can be reused after a internal failure or timeout condition. |


## minecraft:behavior.hold_ground


The mob freezes and looks at the mob they are targeting.


| Name | Type | Default Value | Description |
| --- | --- | --- | --- |
| broadcast | Boolean | false | Whether to broadcast out the mob's target to other mobs of the same type. |
| broadcast_range | Decimal | 0.0f | Range in blocks for how far to broadcast. |
| min_radius | Decimal | 10.0f | Minimum distance the target must be for the mob to run this goal. |
| within_radius_event | String |  | Event to run when target is within the radius. This event is broadcasted if broadcast is true. |


## minecraft:behavior.hurt_by_target


Allows the mob to target another mob that hurts them.


| Name | Type | Default Value | Description |
| --- | --- | --- | --- |
| alert_same_type | Boolean | false | If true, nearby mobs of the same type will be alerted about the damage |
| entity_types | JSON Object |  | List of entity types that this mob can target when hurt by them |
| hurt_owner | Boolean | false | If true, the mob will hurt its owner and other mobs with the same owner as itself |


> **entity_types** 子参数


| Name | Type | Default Value | Description |
| --- | --- | --- | --- |
| cooldown | Decimal | 0.0 | The amount of time in seconds that the mob has to wait before selecting a target of the same type again |
| filters | Minecraft Filter |  | Conditions that make this entry in the list valid |
| max_dist | Decimal | 16 | Maximum distance this mob can be away to be a valid choice |
| must_see | Boolean | false | If true, the mob has to be visible to be a valid choice |
| must_see_forget_duration | Decimal | 3.0 | Determines the amount of time in seconds that this mob will look for a target before forgetting about it and looking for a new one when the target isn't visible any more |
| reevaluate_description | Boolean | false | If true, the mob will stop being targeted if it stops meeting any conditions. |
| sprint_speed_multiplier | Decimal | 1.0 | Multiplier for the running speed. A value of 1.0 means the speed is unchanged |
| walk_speed_multiplier | Decimal | 1.0 | Multiplier for the walking speed. A value of 1.0 means the speed is unchanged |


## minecraft:behavior.inspect_bookshelf


Allows the mob to inspect bookshelves.


| Name | Type | Default Value | Description |
| --- | --- | --- | --- |
| goal_radius | Decimal | 0.5 | Distance in blocks within the mob considers it has reached the goal. This is the "wiggle room" to stop the AI from bouncing back and forth trying to reach a specific spot |
| search_count | Integer | 10 | The number of blocks each tick that the mob will check within its search range and height for a valid block to move to. A value of 0 will have the mob check every block within range in one tick |
| search_height | Integer | 1 | The height that the mob will search for bookshelves |
| search_range | Integer | 0 | Distance in blocks the mob will look for books to inspect |
| speed_multiplier | Decimal | 1.0 | Movement speed multiplier of the mob when using this AI Goal |


## minecraft:behavior.investigate_suspicious_location


Allows this entity to move towards a "suspicious" position based on data gathered in minecraft:suspect_tracking


| Name | Type | Default Value | Description |
| --- | --- | --- | --- |
| goal_radius | Decimal | 1.5 | Distance in blocks within the entity considers it has reached it's target position. |
| speed_multiplier | Decimal | 1 | Movement speed multiplier |


## minecraft:behavior.jump_around_target


Allows an entity to jump around a target.


| Name | Type | Default Value | Description |
| --- | --- | --- | --- |
| check_collision | Boolean | false | Enables collision checks when calculating the jump. Setting check_collision to true may affect performance and should be used with care. |
| entity_bounding_box_scale | Decimal | 0.700000 | Scaling temporarily applied to the entity's AABB bounds when jumping. A smaller bounding box reduces the risk of collisions during the jump. When check_collision is true it also increases the chance of being able to jump when close to obstacles. |
| filters | Minecraft Filter |  | Conditions that need to be met for the behavior to start. |
| jump_angles | Array | [ 40.0, 55.0, 60.0, 75.0, 80.0 ] | The jump angles in float degrees that are allowed when performing the jump. The order in which the angles are chosen is randomized. |
| jump_cooldown_duration | Decimal | 0.500000 | The time in seconds to spend in cooldown before this goal can be used again. |
| jump_cooldown_when_hurt_duration | Decimal | 0.100000 | The time in seconds to spend in cooldown after being hurt before this goal can be used again. |
| landing_distance_from_target | Range [a, b] | [4.000000, 8.000000] | The range deciding how close to and how far away from the target the landing position can be when jumping. |
| landing_position_spread_degrees | Integer | 90 | This angle (in degrees) is used for controlling the spread when picking a landing position behind the target. A zero spread angle means the landing position will be straight behind the target with no variance. A 90 degree spread angle means the landing position can be up to 45 degrees to the left and to the right of the position straight behind the target's view direction. |
| last_hurt_duration | Decimal | 2.000000 | If the entity was hurt within these last seconds, the jump_cooldown_when_hurt_duration will be used instead of jump_cooldown_duration. |
| line_of_sight_obstruction_height_ignore | Integer | 4 | If the entity's line of sight towards its target is obstructed by an obstacle with a height below this number, the obstacle will be ignored, and the goal will try to find a valid landing position. |
| max_jump_velocity | Decimal | 1.400000 | Maximum velocity a jump can be performed at. |
| prepare_jump_duration | Decimal | 0.500000 | The time in seconds to spend preparing for the jump. |
| required_vertical_space | Integer | 4 | The number of blocks above the entity's head that has to be air for this goal to be usable. |
| snap_to_surface_block_range | Integer | 10 | The number of blocks above and below from the jump target position that will be checked to find a surface to land on. |
| valid_distance_to_target | Range [a, b] | [4.000000, 20.000000] | Target needs to be within this range for the jump to happen. |


## minecraft:behavior.jump_to_block


Allows an entity to jump to another random block.


| Name | Type | Default Value | Description |
| --- | --- | --- | --- |
| cooldown_range | Range [a, b] | [10, 20] | Minimum and maximum cooldown time-range (positive, in seconds) between each attempted jump. |
| forbidden_blocks | Array |  | Blocks that the mob can't jump to. |
| max_velocity | Decimal | 1.500000 | The maximum velocity with which the mob can jump. |
| minimum_distance | Integer | 2 | The minimum distance (in blocks) from the mob to a block, in order to consider jumping to it. |
| minimum_path_length | Integer | 5 | The minimum length (in blocks) of the mobs path to a block, in order to consider jumping to it. |
| preferred_blocks | Array |  | Blocks that the mob prefers jumping to. |
| preferred_blocks_chance | Decimal | 1.000000 | Chance (between 0.0 and 1.0) that the mob will jump to a preferred block, if in range. Only matters if preferred blocks are defined. |
| scale_factor | Decimal | 0.700000 | The scalefactor of the bounding box of the mob while it is jumping. |
| search_height | Integer | 10 | The height (in blocks, in range [2, 15]) of the search box, centered around the mob. |
| search_width | Integer | 8 | The width (in blocks, in range [2, 15]) of the search box, centered around the mob. |


## minecraft:behavior.knockback_roar


Allows the mob to perform a damaging knockback that affects all nearby entities.


| Name | Type | Default Value | Description |
| --- | --- | --- | --- |
| attack_time | Decimal | 0.5 | The delay after which the knockback occurs (in seconds). |
| cooldown_time | Decimal | 0.10 | Time (in seconds) the mob has to wait before using the goal again. |
| damage_filters | Minecraft Filter |  | The list of conditions another entity must meet to be a valid target to apply damage to. |
| duration | Decimal | 1.0 | The max duration of the roar (in seconds). |
| knockback_damage | Integer | 6 | The damage dealt by the knockback roar. |
| knockback_filters | Minecraft Filter |  | The list of conditions another entity must meet to be a valid target to apply knockback to. |
| knockback_height_cap | Decimal | 0.40 | The maximum height for vertical knockback. |
| knockback_horizontal_strength | Integer | 4 | The strength of the horizontal knockback. |
| knockback_range | Integer | 4 | The radius (in blocks) of the knockback effect. |
| knockback_vertical_strength | Integer | 4 | The strength of the vertical knockback. |
| on_roar_end | Trigger |  | Event that is triggered when the roar ends. |


## minecraft:behavior.lay_down


Allows mobs to lay down at times


| Name | Type | Default Value | Description |
| --- | --- | --- | --- |
| interval | Integer | 120 | A random value to determine at what intervals something can occur. This has a 1/interval chance to choose this goal |
| random_stop_interval | Integer | 120 | a random value in which the goal can use to pull out of the behavior. This is a 1/interval chance to play the sound |


## minecraft:behavior.lay_egg


Allows the mob to lay an egg block on certain types of blocks if the mob is pregnant.


| Name | Type | Default Value | Description |
| --- | --- | --- | --- |
| allow_laying_from_below | Boolean | false | Allows the mob to lay its eggs from below the target if it can't get there. This is useful if the target block is water with air above, since mobs may not be able to get to the air block above water. |
| egg_type | String | minecraft:turtle_egg | Block type for the egg to lay. If this is a turtle egg, the number of eggs in the block is randomly set. |
| goal_radius | Decimal | 0.5 | Distance in blocks within the mob considers it has reached the goal. This is the "wiggle room" to stop the AI from bouncing back and forth trying to reach a specific spot |
| lay_egg_sound | String | lay_egg | Name of the sound event played when laying the egg. Defaults to lay_egg, which is used for Turtles. |
| lay_seconds | Decimal | 10.0f | Duration of the laying egg process in seconds. |
| on_lay | Trigger |  | Event to run when this mob lays the egg. |
| search_height | Integer | 1 | Height in blocks the mob will look for a target block to move towards |
| search_range | Integer | 0 | The distance in blocks it will look for a target block to move towards |
| speed_multiplier | Decimal | 1.0 | Movement speed multiplier of the mob when using this AI Goal |
| target_blocks | Array | [ minecraft:sand ] | Blocks that the mob can lay its eggs on top of. |
| target_materials_above_block | Array | [ Air ] | Types of materials that can exist above the target block. Valid types are Air, Water, and Lava. |
| use_default_animation | Boolean | true | Specifies if the default lay-egg animation should be played when the egg is placed or not. |


## minecraft:behavior.leap_at_target


Allows monsters to jump at and attack their target. Can only be used by hostile mobs.


| Name | Type | Default Value | Description |
| --- | --- | --- | --- |
| must_be_on_ground | Boolean | true | If true, the mob will only jump at its target if its on the ground. Setting it to false will allow it to jump even if its already in the air |
| set_persistent | Boolean | false | Allows the actor to be set to persist upon targeting a player |
| yd | Decimal | 0.0 | The height in blocks the mob jumps when leaping at its target |


## minecraft:behavior.look_at_entity


Allows the mob to look at nearby entities.


| Name | Type | Default Value | Description |
| --- | --- | --- | --- |
| angle_of_view_horizontal | Integer | 360 | The angle in degrees that the mob can see in the Y-axis (up-down). |
| angle_of_view_vertical | Integer | 360 | The angle in degrees that the mob can see in the X-axis (left-right). |
| filters | Minecraft Filter |  | Filter to determine the conditions for this mob to look at the entity |
| look_distance | Decimal | 8.0 | The distance in blocks from which the entity will look at the nearest entity. |
| look_time | Range [a, b] | [2, 4] | Time range to look at the nearest entity. |
| probability | Decimal | 0.02 | The probability of looking at the target. A value of 1.00 is 100%. |


## minecraft:behavior.look_at_player


Allows the mob to look at the player when the player is nearby.


| Name | Type | Default Value | Description |
| --- | --- | --- | --- |
| angle_of_view_horizontal | Integer | 360 | The angle in degrees that the mob can see in the Y-axis (up-down). |
| angle_of_view_vertical | Integer | 360 | The angle in degrees that the mob can see in the X-axis (left-right). |
| look_distance | Decimal | 8.0 | The distance in blocks from which the entity will look at the nearest player. |
| look_time | Range [a, b] | [2, 4] | Time range to look at the nearest player. |
| probability | Decimal | 0.02 | The probability of looking at the target. A value of 1.00 is 100%. |


## minecraft:behavior.look_at_target


Allows the mob to look at the entity they are targetting.


| Name | Type | Default Value | Description |
| --- | --- | --- | --- |
| angle_of_view_horizontal | Integer | 360 | The angle in degrees that the mob can see in the Y-axis (up-down). |
| angle_of_view_vertical | Integer | 360 | The angle in degrees that the mob can see in the X-axis (left-right). |
| look_distance | Decimal | 8.0 | The distance in blocks from which the entity will look at this mob's current target. |
| look_time | Range [a, b] | [2, 4] | Time range to look at this mob's current target. |
| probability | Decimal | 0.02 | The probability of looking at the target. A value of 1.00 is 100%. |


## minecraft:behavior.look_at_trading_player


Allows the mob to look at the player they are trading with.


| Name | Type | Default Value | Description |
| --- | --- | --- | --- |
| angle_of_view_horizontal | Integer | 360 | The angle in degrees that the mob can see in the Y-axis (up-down). |
| angle_of_view_vertical | Integer | 360 | The angle in degrees that the mob can see in the X-axis (left-right). |
| look_distance | Decimal | 8.0 | The distance in blocks from which the entity will look at the player this mob is trading with. |
| look_time | Range [a, b] | [2, 4] | Time range to look at the player this mob is trading with. |
| probability | Decimal | 0.02 | The probability of looking at the target. A value of 1.00 is 100%. |


## minecraft:behavior.make_love


Allows the villager to look for a mate to spawn other villagers with. Can only be used by Villagers.


## minecraft:behavior.melee_attack


Allows an entity to deal damage through a melee attack.


| Name | Type | Default Value | Description |
| --- | --- | --- | --- |
| attack_once | Boolean | false | Allows the entity to use this attack behavior, only once EVER. |
| attack_types | String | N/A | Defines the entity types this entity will attack. |
| can_spread_on_fire | Boolean | false | If the entity is on fire, this allows the entity's target to catch on fire after being hit. |
| cooldown_time | Decimal | 1 | Cooldown time (in seconds) between attacks. |
| inner_boundary_time_increase | Decimal | 0.25 | Time (in seconds) to add to attack path recalculation when the target is beyond the "path_inner_boundary". |
| max_path_time | Decimal | 0.55 | Maximum base time (in seconds) to recalculate new attack path to target (before increases applied). |
| melee_fov | Decimal | 90 | Field of view (in degrees) when using the sensing component to detect an attack target. |
| min_path_time | Decimal | 0.2 | Minimum base time (in seconds) to recalculate new attack path to target (before increases applied). |
| on_attack | Trigger | N/A | Defines the event to trigger when this entity successfully attacks. |
| on_kill | Trigger | N/A | Defines the event to trigger when this entity kills the target. |
| outer_boundary_time_increase | Decimal | 0.5 | Time (in seconds) to add to attack path recalculation when the target is beyond the "path_outer_boundary". |
| path_fail_time_increase | Decimal | 0.75 | Time (in seconds) to add to attack path recalculation when this entity cannot move along the current path. |
| path_inner_boundary | Decimal | 16 | Distance at which to increase attack path recalculation by "inner_boundary_tick_increase". |
| path_outer_boundary | Decimal | 32 | Distance at which to increase attack path recalculation by "outer_boundary_tick_increase". |
| random_stop_interval | Integer | 0 | This entity will have a 1 in N chance to stop it's current attack, where N = "random_stop_interval". |
| reach_multiplier | Decimal | 2 | Used with the base size of the entity to determine minimum target-distance before trying to deal attack damage. |
| require_complete_path | Boolean | false | Toggles (on/off) the need to have a full path from the entity to the target when using this melee attack behavior. |
| speed_multiplier | Decimal | 1 | This multiplier modifies the attacking entity's speed when moving toward the target. |
| track_target | Boolean | false | Allows the entity to track the attack target, even if the entity has no sensing. |
| x_max_rotation | Decimal | 30 | Maximum rotation (in degrees), on the X-axis, this entity can rotate while trying to look at the target. |
| y_max_head_rotation | Decimal | 30 | Maximum rotation (in degrees), on the Y-axis, this entity can rotate its head while trying to look at the target. |


## minecraft:behavior.melee_box_attack


Allows an entity to deal damage through a melee attack with reach calculations based on bounding boxes.


| Name | Type | Default Value | Description |
| --- | --- | --- | --- |
| attack_once | Boolean | false | Allows the entity to use this attack behavior, only once EVER. |
| attack_types | String | N/A | Defines the entity types this entity will attack. |
| can_spread_on_fire | Boolean | false | If the entity is on fire, this allows the entity's target to catch on fire after being hit. |
| cooldown_time | Decimal | 1 | Cooldown time (in seconds) between attacks. |
| horizontal_reach | Decimal | 0.8 | The attack reach of the mob will be a box with the size of the mobs bounds increased by this value in all horizontal directions. |
| inner_boundary_time_increase | Decimal | 0.25 | Time (in seconds) to add to attack path recalculation when the target is beyond the "path_inner_boundary". |
| max_path_time | Decimal | 0.55 | Maximum base time (in seconds) to recalculate new attack path to target (before increases applied). |
| melee_fov | Decimal | 90 | Field of view (in degrees) when using the sensing component to detect an attack target. |
| min_path_time | Decimal | 0.2 | Minimum base time (in seconds) to recalculate new attack path to target (before increases applied). |
| on_attack | Trigger | N/A | Defines the event to trigger when this entity successfully attacks. |
| on_kill | Trigger | N/A | Defines the event to trigger when this entity kills the target. |
| outer_boundary_time_increase | Decimal | 0.5 | Time (in seconds) to add to attack path recalculation when the target is beyond the "path_outer_boundary". |
| path_fail_time_increase | Decimal | 0.75 | Time (in seconds) to add to attack path recalculation when this entity cannot move along the current path. |
| path_inner_boundary | Decimal | 16 | Distance at which to increase attack path recalculation by "inner_boundary_tick_increase". |
| path_outer_boundary | Decimal | 32 | Distance at which to increase attack path recalculation by "outer_boundary_tick_increase". |
| random_stop_interval | Integer | 0 | This entity will have a 1 in N chance to stop it's current attack, where N = "random_stop_interval". |
| require_complete_path | Boolean | false | Toggles (on/off) the need to have a full path from the entity to the target when using this melee attack behavior. |
| speed_multiplier | Decimal | 1 | This multiplier modifies the attacking entity's speed when moving toward the target. |
| track_target | Boolean | false | Allows the entity to track the attack target, even if the entity has no sensing. |
| x_max_rotation | Decimal | 30 | Maximum rotation (in degrees), on the X-axis, this entity can rotate while trying to look at the target. |
| y_max_head_rotation | Decimal | 30 | Maximum rotation (in degrees), on the Y-axis, this entity can rotate its head while trying to look at the target. |


## minecraft:behavior.mingle


Allows an entity to go to the village bell and mingle with other entities


| Name | Type | Default Value | Description |
| --- | --- | --- | --- |
| cooldown_time | Decimal | 0.0 | Time in seconds the mob has to wait before using the goal again |
| duration | Decimal | 1.0 | Amount of time in seconds that the entity will chat with another entity |
| mingle_distance | Decimal | 2.0f | The distance from its partner that this entity will mingle. If the entity type is not the same as the entity, this value needs to be identical on both entities. |
| mingle_partner_type | List | empty | The entity type that this entity is allowed to mingle with |
| speed_multiplier | Decimal | 1.0 | Movement speed multiplier of the mob when using this AI Goal |


## minecraft:behavior.mount_pathing


Allows the mob to move around on its own while mounted seeking a target to attack.


| Name | Type | Default Value | Description |
| --- | --- | --- | --- |
| speed_multiplier | Decimal | 1.0 | Movement speed multiplier of the mob when using this AI Goal |
| target_dist | Decimal | 0.0 | The distance at which this mob wants to be away from its target |
| track_target | Boolean | false | If true, this mob will chase after the target as long as it's a valid target |


## minecraft:behavior.move_around_target


Allows an entity to move around a target. If the entity is too close (i.e. closer than destination range min and height difference limit) it will try to move away from its target. If the entity is too far away from its target it will try to move closer to a random position within the destination range. A randomized amount of those positions will be behind the target, and the spread can be tweaked with 'destination_pos_spread_degrees'.


| Name | Type | Default Value | Description |
| --- | --- | --- | --- |
| destination_pos_spread_degrees | Decimal | 90.000000 | This angle (in degrees) is used for controlling the spread when picking a destination position behind the target. A zero spread angle means the destination position will be straight behind the target with no variance. A 90 degree spread angle means the destination position can be up to 45 degrees to the left and to the right of the position straight behind the target's view direction.. |
| destination_position_range | Range [a, b] | [4.000000, 8.000000] | The range of distances from the target entity within which the goal should look for a position to move the owner entity to. |
| filters | Minecraft Filter |  | Conditions that need to be met for the behavior to start. |
| height_difference_limit | Decimal | 10.000000 | Distance in height (in blocks) between the owner entity and the target has to be less than this value when owner checks if it is too close and should move away from the target. This value needs to be bigger than zero for the move away logic to trigger. |
| horizontal_search_distance | Integer | 5 | Horizontal search distance (in blocks) when searching for a position to move away from target. |
| movement_speed | Decimal | 0.600000 | The speed with which the entity should move to its target position. |
| vertical_search_distance | Integer | 5 | Vertical search distance (in blocks) when searching for a position to move away from target. |


## minecraft:behavior.move_indoors


Allows this entity to move indoors.


| Name | Type | Default Value | Description |
| --- | --- | --- | --- |
| speed_multiplier | Decimal | 0.800000 | The movement speed modifier to apply to the entity while it is moving indoors. |
| timeout_cooldown | Decimal | 8.000000 | The cooldown time in seconds before the goal can be reused after pathfinding fails |


## minecraft:behavior.move_outdoors


Allows this entity to move outdoors.


| Name | Type | Default Value | Description |
| --- | --- | --- | --- |
| goal_radius | Decimal | 0.500000 | The radius away from the target block to count as reaching the goal. |
| search_count | Integer | 10 | The amount of times to try finding a random outdoors position before failing. |
| search_height | Integer | 5 | The y range to search for an outdoors position for. |
| search_range | Integer | 15 | The x and z range to search for an outdoors position for. |
| speed_multiplier | Decimal | 0.500000 | The movement speed modifier to apply to the entity while it is moving outdoors. |
| timeout_cooldown | Decimal | 8.000000 | The cooldown time in seconds before the goal can be reused after pathfinding fails |


## minecraft:behavior.move_through_village


Can only be used by Villagers. Allows the villagers to create paths around the village.


| Name | Type | Default Value | Description |
| --- | --- | --- | --- |
| only_at_night | Boolean | false | If true, the mob will only move through the village during night time |
| speed_multiplier | Decimal | 1.0 | Movement speed multiplier of the mob when using this AI Goal |


## minecraft:behavior.move_to_block


Allows mob to move towards a block.


| Name | Type | Default Value | Description |
| --- | --- | --- | --- |
| goal_radius | Decimal | 0.5 | Distance in blocks within the mob considers it has reached the goal. This is the "wiggle room" to stop the AI from bouncing back and forth trying to reach a specific spot |
| on_reach | Trigger |  | Event to run on block reached. |
| on_stay_completed | Trigger |  | Event to run on completing a stay of stay_duration at the block. |
| search_height | Integer | 1 | The height in blocks that the mob will look for the block. |
| search_range | Integer | 0 | The distance in blocks that the mob will look for the block. |
| speed_multiplier | Decimal | 1.0 | Movement speed multiplier of the mob when using this AI Goal |
| start_chance | Decimal | 1.0 | Chance to start the behavior (applied after each random tick_interval). |
| stay_duration | Decimal | 0.0 | Number of ticks needed to complete a stay at the block. |
| target_blocks | List |  | Block types to move to. |
| target_offset | Vector [a, b, c] | [0, 0, 0] | Offset to add to the selected target position. |
| target_selection_method | String | nearest | Kind of block to find fitting the specification. Valid values are "random" and "nearest". |
| tick_interval | Integer | 20 | Average interval in ticks to try to run this behavior. |


## minecraft:behavior.move_to_land


Allows the mob to move back onto land when in water.


| Name | Type | Default Value | Description |
| --- | --- | --- | --- |
| goal_radius | Decimal | 0.5 | Distance in blocks within the mob considers it has reached the goal. This is the "wiggle room" to stop the AI from bouncing back and forth trying to reach a specific spot |
| search_count | Integer | 10 | The number of blocks each tick that the mob will check within its search range and height for a valid block to move to. A value of 0 will have the mob check every block within range in one tick |
| search_height | Integer | 1 | Height in blocks the mob will look for land to move towards |
| search_range | Integer | 0 | The distance in blocks it will look for land to move towards |
| speed_multiplier | Decimal | 1.0 | Movement speed multiplier of the mob when using this AI Goal |


## minecraft:behavior.move_to_lava


Allows the mob to move back into lava when on land.


| Name | Type | Default Value | Description |
| --- | --- | --- | --- |
| goal_radius | Decimal | 0.5 | Distance in blocks within the mob considers it has reached the goal. This is the "wiggle room" to stop the AI from bouncing back and forth trying to reach a specific spot |
| search_count | Integer | 10 | The number of blocks each tick that the mob will check within its search range and height for a valid block to move to. A value of 0 will have the mob check every block within range in one tick |
| search_height | Integer | 1 | Height in blocks the mob will look for lava to move towards |
| search_range | Integer | 0 | The distance in blocks it will look for lava to move towards |
| speed_multiplier | Decimal | 1.0 | Movement speed multiplier of the mob when using this AI Goal |


## minecraft:behavior.move_to_liquid


Allows the mob to move into a liquid when on land.


| Name | Type | Default Value | Description |
| --- | --- | --- | --- |
| goal_radius | Decimal | 0.5 | Distance in blocks within the mob considers it has reached the goal. This is the "wiggle room" to stop the AI from bouncing back and forth trying to reach a specific spot |
| material_type | String | Any | The material type of the liquid block to find. Valid values are "Any", "Water", and "Lava". |
| search_count | Integer | 10 | The number of blocks each tick that the mob will check within its search range and height for a valid block to move to. A value of 0 will have the mob check every block within range in one tick |
| search_height | Integer | 1 | Height in blocks the mob will look for the liquid block to move towards |
| search_range | Integer | 0 | The distance in blocks it will look for the liquid block to move towards |
| speed_multiplier | Decimal | 1.0 | Movement speed multiplier of the mob when using this AI Goal |


## minecraft:behavior.move_to_poi


Allows the mob to move to a POI if able to


| Name | Type | Default Value | Description |
| --- | --- | --- | --- |
| poi_type | String |  | Tells the goal what POI type it should be looking for |
| speed_multiplier | Decimal | 1.0 | Movement speed multiplier of the mob when using this AI Goal |


## minecraft:behavior.move_to_random_block


Allows mob to move towards a random block.


| Name | Type | Default Value | Description |
| --- | --- | --- | --- |
| block_distance | Decimal | 16.0 | Defines the distance from the mob, in blocks, that the block to move to will be chosen. |
| within_radius | Decimal | 0.0 | Defines the distance in blocks the mob has to be from the block for the movement to be finished. |


## minecraft:behavior.move_to_village


Allows the mob to move into a random location within a village.


| Name | Type | Default Value | Description |
| --- | --- | --- | --- |
| cooldown_time | Decimal | 0.0 | Time in seconds the mob has to wait before using the goal again |
| goal_radius | Decimal | 0.5 | Distance in blocks within the mob considers it has reached the goal. This is the "wiggle room" to stop the AI from bouncing back and forth trying to reach a specific spot |
| search_range | Integer | 0 | The distance in blocks to search for villages. If <= 0, find the closest village regardless of distance. |
| speed_multiplier | Decimal | 1.0 | Movement speed multiplier of the mob when using this AI Goal |


## minecraft:behavior.move_to_water


Allows the mob to move back into water when on land.


| Name | Type | Default Value | Description |
| --- | --- | --- | --- |
| goal_radius | Decimal | 0.5 | Distance in blocks within the mob considers it has reached the goal. This is the "wiggle room" to stop the AI from bouncing back and forth trying to reach a specific spot |
| search_count | Integer | 10 | The number of blocks each tick that the mob will check within its search range and height for a valid block to move to. A value of 0 will have the mob check every block within range in one tick |
| search_height | Integer | 1 | Height in blocks the mob will look for water to move towards |
| search_range | Integer | 0 | The distance in blocks it will look for water to move towards |
| speed_multiplier | Decimal | 1.0 | Movement speed multiplier of the mob when using this AI Goal |


## minecraft:behavior.move_towards_dwelling_restriction


Allows entities with the "minecraft:dweller" component to move toward their Village area that the entity should be restricted to.


| Name | Type | Default Value | Description |
| --- | --- | --- | --- |
| speed_multiplier | Decimal | 1.0 | This multiplier modifies the entity's speed when moving towards its restriction. |


## minecraft:behavior.move_towards_home_restriction


Allows entities with a "minecraft:home" component to move towards their home position.


If "restriction_radius" is set, entities will be able to run this behavior only if outside of it.


| Name | Type | Default Value | Description |
| --- | --- | --- | --- |
| speed_multiplier | Decimal | 1.0 | This multiplier modifies the entity's speed when moving towards its restriction. |


## minecraft:behavior.move_towards_target


Allows mob to move towards its current target.


| Name | Type | Default Value | Description |
| --- | --- | --- | --- |
| within_radius | Decimal | 0.0 | Defines the radius in blocks that the mob tries to be from the target. A value of 0 means it tries to occupy the same block as the target |


## minecraft:behavior.nap


Allows mobs to occassionally stop and take a nap under certain conditions.


| Name | Type | Default Value | Description |
| --- | --- | --- | --- |
| cooldown_max | Decimal | 0.0 | Maximum time in seconds the mob has to wait before using the goal again |
| cooldown_min | Decimal | 0.0 | Minimum time in seconds the mob has to wait before using the goal again |
| mob_detect_dist | Decimal | 6.0 | The block distance in x and z that will be checked for mobs that this mob detects |
| mob_detect_height | Decimal | 6.0 | The block distance in y that will be checked for mobs that this mob detects |


## minecraft:behavior.nearest_attackable_target


Allows an entity to attack the closest target within a given subset of specific target types.


| Name | Type | Default Value | Description |
| --- | --- | --- | --- |
| attack_interval\|attack_interval_min | Integer | 0 | Time range (in seconds) between searching for an attack target, range is in (0, "attack_interval"]. Only used if "attack_interval" is greater than 0, otherwise "scan_interval" is used. |
| attack_owner | Boolean | false | If true, this entity can attack its owner. |
| entity_types | Minecraft Filter |  | Filters which types of targets are valid for this entity. |
| must_reach | Boolean | false | If true, this entity requires a path to the target. |
| must_see | Boolean | false | Determines if target-validity requires this entity to be in range only, or both in range and in sight. |
| must_see_forget_duration | Decimal | 3.0 | Time (in seconds) the target must not be seen by this entity to become invalid. Used only if "must_see" is true. |
| persist_time | Decimal | 0.0 | Time (in seconds) this entity can continue attacking the target after the target is no longer valid. |
| reselect_targets | Boolean | false | Allows the attacking entity to update the nearest target, otherwise a target is only reselected after each "scan_interval" or "attack_interval". |
| scan_interval | Integer | 10 | If "attack_interval" is 0 or isn't declared, then between attacks: scanning for a new target occurs every amount of ticks equal to "scan_interval", minimum value is 1. Values under 10 can affect performance. |
| set_persistent | Boolean | false | Allows the actor to be set to persist upon targeting a player |
| target_invisible_multiplier | Decimal | 0.70 | Multiplied with the target's armor coverage percentage to modify "max_dist" when detecting an invisible target. |
| target_search_height | Decimal | -1.00 | Maximum vertical target-search distance, if it's greater than the target type's "max_dist". A negative value defaults to "entity_types" greatest "max_dist". |
| target_sneak_visibility_multiplier | Decimal | 0.80 | Multiplied with the target type's "max_dist" when trying to detect a sneaking target. |
| within_radius | Decimal | 0.0 | Maximum distance this entity can be from the target when following it, otherwise the target becomes invalid. This value is only used if the entity doesn't declare "minecraft:follow_range". |


## minecraft:behavior.nearest_prioritized_attackable_target


Allows the mob to check for and pursue the nearest valid target.


| Name | Type | Default Value | Description |
| --- | --- | --- | --- |
| attack_interval | Integer | 0 | Time in seconds before selecting a target |
| cooldown | Decimal | 0.0 | The amount of time in seconds that the mob has to wait before selecting a target of the same type again |
| entity_types | JSON Object |  | List of entity types that this mob considers valid targets |
| must_reach | Boolean | false | If true, only entities that this mob can path to can be selected as targets |
| must_see | Boolean | false | If true, only entities in this mob's viewing range can be selected as targets |
| must_see_forget_duration | Decimal | 3.0 | Determines the amount of time in seconds that this mob will look for a target before forgetting about it and looking for a new one when the target isn't visible any more |
| persist_time | Decimal | 0.0f | Time in seconds for a valid target to stay targeted when it becomes and invalid target. |
| priority | Integer | 0 | Specifies the priority in which filtered enemy types should be attacked. Lower number means higher priority. |
| reselect_targets | Boolean | false | If true, the target will change to the current closest entity whenever a different entity is closer |
| scan_interval | Integer | 10 | How many ticks to wait between scanning for a target. |
| set_persistent | Boolean | false | Allows the actor to be set to persist upon targeting a player |
| target_search_height | Decimal | -1.0f | Height in blocks to search for a target mob. -1.0f means the height does not matter. |
| within_radius | Decimal | 0.0 | Distance in blocks that the target can be within to launch an attack |


> **entity_types** 子参数


| Name | Type | Default Value | Description |
| --- | --- | --- | --- |
| cooldown | Decimal | 0.0 | The amount of time in seconds that the mob has to wait before selecting a target of the same type again |
| filters | Minecraft Filter |  | Conditions that make this entry in the list valid |
| max_dist | Decimal | 16 | Maximum distance this mob can be away to be a valid choice |
| must_see | Boolean | false | If true, the mob has to be visible to be a valid choice |
| must_see_forget_duration | Decimal | 3.0 | Determines the amount of time in seconds that this mob will look for a target before forgetting about it and looking for a new one when the target isn't visible any more |
| reevaluate_description | Boolean | false | If true, the mob will stop being targeted if it stops meeting any conditions. |
| sprint_speed_multiplier | Decimal | 1.0 | Multiplier for the running speed. A value of 1.0 means the speed is unchanged |
| walk_speed_multiplier | Decimal | 1.0 | Multiplier for the walking speed. A value of 1.0 means the speed is unchanged |


## minecraft:behavior.ocelot_sit_on_block


Allows to mob to be able to sit in place like the ocelot.


| Name | Type | Default Value | Description |
| --- | --- | --- | --- |
| speed_multiplier | Decimal | 1.0 | Movement speed multiplier of the mob when using this AI Goal |


## minecraft:behavior.ocelotattack


Allows an entity to attack by sneaking and pouncing.


| Name | Type | Default Value | Description |
| --- | --- | --- | --- |
| cooldown_time | Decimal | 1 | Time (in seconds) between attacks. |
| max_distance | Decimal | 15 | Max distance from the target, this entity will use this attack behavior. |
| max_sneak_range | Decimal | 15 | Max distance from the target, this entity starts sneaking. |
| max_sprint_range | Decimal | 4 | Max distance from the target, this entity starts sprinting (sprinting takes priority over sneaking). |
| reach_multiplier | Decimal | 2 | Used with the base size of the entity to determine minimum target-distance before trying to deal attack damage. |
| sneak_speed_multiplier | Decimal | 0.6 | Modifies the attacking entity's movement speed while sneaking. |
| sprint_speed_multiplier | Decimal | 1.33 | Modifies the attacking entity's movement speed while sprinting. |
| walk_speed_multiplier | Decimal | 0.8 | Modifies the attacking entity's movement speed when not sneaking or sprinting, but still within attack range. |
| x_max_rotation | Decimal | 30 | Maximum rotation (in degrees), on the X-axis, this entity can rotate while trying to look at the target. |
| y_max_head_rotation | Decimal | 30 | Maximum rotation (in degrees), on the Y-axis, this entity can rotate its head while trying to look at the target. |


## minecraft:behavior.offer_flower


Allows the mob to offer a flower to another mob with the minecraft:take_flower behavior.


| Name | Type | Default Value | Description |
| --- | --- | --- | --- |
| chance_to_start | Decimal | 0.00 | Percent chance that the mob will start this goal from 0.0 to 1.0 (where 1.0 = 100%). |
| filters | Minecraft Filter |  | Conditions that need to be met for the behavior to start. |
| max_head_rotation_y | Decimal | 30 | Maximum rotation (in degrees), on the Y-axis, this entity can rotate its head while trying to look at the target. |
| max_offer_flower_duration | Decimal | 20.00 | The max amount of time (in seconds) that the mob will offer the flower for before exiting the Goal. |
| max_rotation_x | Decimal | 30 | Maximum rotation (in degrees), on the X-axis, this entity can rotate while trying to look at the target. |
| search_area | Vector [a, b, c] | [6, 2, 6] | The dimensions of the AABB used to search for a potential mob to offer flower to. |


## minecraft:behavior.open_door


Allows the mob to open doors. Requires the mob to be able to path through doors, otherwise the mob won't even want to try opening them.


| Name | Type | Default Value | Description |
| --- | --- | --- | --- |
| close_door_after | Boolean | true | If true, the mob will close the door after opening it and going through it |


## minecraft:behavior.owner_hurt_by_target


Allows the mob to target another mob that hurts their owner.


| Name | Type | Default Value | Description |
| --- | --- | --- | --- |
| entity_types | JSON Object |  | List of entity types that this mob can target if they hurt their owner |


> **entity_types** 子参数


| Name | Type | Default Value | Description |
| --- | --- | --- | --- |
| cooldown | Decimal | 0.0 | The amount of time in seconds that the mob has to wait before selecting a target of the same type again |
| filters | Minecraft Filter |  | Conditions that make this entry in the list valid |
| max_dist | Decimal | 16 | Maximum distance this mob can be away to be a valid choice |
| must_see | Boolean | false | If true, the mob has to be visible to be a valid choice |
| must_see_forget_duration | Decimal | 3.0 | Determines the amount of time in seconds that this mob will look for a target before forgetting about it and looking for a new one when the target isn't visible any more |
| reevaluate_description | Boolean | false | If true, the mob will stop being targeted if it stops meeting any conditions. |
| sprint_speed_multiplier | Decimal | 1.0 | Multiplier for the running speed. A value of 1.0 means the speed is unchanged |
| walk_speed_multiplier | Decimal | 1.0 | Multiplier for the walking speed. A value of 1.0 means the speed is unchanged |


## minecraft:behavior.owner_hurt_target


Allows the mob to target a mob that is hurt by their owner.


| Name | Type | Default Value | Description |
| --- | --- | --- | --- |
| entity_types | JSON Object |  | List of entity types that this entity can target if the potential target is hurt by this mob's owner |


> **entity_types** 子参数


| Name | Type | Default Value | Description |
| --- | --- | --- | --- |
| cooldown | Decimal | 0.0 | The amount of time in seconds that the mob has to wait before selecting a target of the same type again |
| filters | Minecraft Filter |  | Conditions that make this entry in the list valid |
| max_dist | Decimal | 16 | Maximum distance this mob can be away to be a valid choice |
| must_see | Boolean | false | If true, the mob has to be visible to be a valid choice |
| must_see_forget_duration | Decimal | 3.0 | Determines the amount of time in seconds that this mob will look for a target before forgetting about it and looking for a new one when the target isn't visible any more |
| reevaluate_description | Boolean | false | If true, the mob will stop being targeted if it stops meeting any conditions. |
| sprint_speed_multiplier | Decimal | 1.0 | Multiplier for the running speed. A value of 1.0 means the speed is unchanged |
| walk_speed_multiplier | Decimal | 1.0 | Multiplier for the walking speed. A value of 1.0 means the speed is unchanged |


## minecraft:behavior.panic


Allows the mob to enter the panic state, which makes it run around and away from the damage source that made it enter this state.


| Name | Type | Default Value | Description |
| --- | --- | --- | --- |
| damage_sources | List | [campfire, fire, fire_tick, freezing, lava, lightning, magma, soul_campfire, temperature, entity_attack, entity_explosion, fireworks, magic, projectile, ram_attack, sonic_boom, wither, mace_smash] | The list of Entity Damage Sources that will cause this mob to panic |
| force | Boolean | false | If true, this mob will not stop panicking until it can't move anymore or the goal is removed from it |
| ignore_mob_damage | Boolean | false | If true, the mob will not panic in response to damage from other mobs. This overrides the damage types in "damage_sources" |
| prefer_water | Boolean | false | If true, the mob will prefer water over land |
| speed_multiplier | Decimal | 1.0 | Movement speed multiplier of the mob when using this AI Goal |


## minecraft:behavior.pet_sleep_with_owner


Allows the pet mob to move onto a bed with its owner while sleeping.


| Name | Type | Default Value | Description |
| --- | --- | --- | --- |
| goal_radius | Decimal | 0.5 | Distance in blocks within the mob considers it has reached the goal. This is the "wiggle room" to stop the AI from bouncing back and forth trying to reach a specific spot |
| search_height | Integer | 1 | Height in blocks from the owner the pet can be to sleep with owner. |
| search_range | Integer | 0 | The distance in blocks from the owner the pet can be to sleep with owner. |
| speed_multiplier | Decimal | 1.0 | Movement speed multiplier of the mob when using this AI Goal |


## minecraft:behavior.pickup_items


Allows the mob to pick up items on the ground.


| Name | Type | Default Value | Description |
| --- | --- | --- | --- |
| can_pickup_any_item | Boolean | false | If true, the mob can pickup any item |
| can_pickup_to_hand_or_equipment | Boolean | true | If true, the mob can pickup items to its hand or armor slots |
| excluded_items | List |  | List of items this mob will not pick up |
| goal_radius | Decimal | 0.5 | Distance in blocks within the mob considers it has reached the goal. This is the "wiggle room" to stop the AI from bouncing back and forth trying to reach a specific spot |
| max_dist | Decimal | 0.0 | Maximum distance this mob will look for items to pick up |
| pickup_based_on_chance | Boolean | false | If true, depending on the difficulty, there is a random chance that the mob may not be able to pickup items |
| speed_multiplier | Decimal | 1.0 | Movement speed multiplier of the mob when using this AI Goal |
| track_target | Boolean | false | If true, this mob will chase after the target as long as it's a valid target |


## minecraft:behavior.place_block


| Name | Type | Default Value | Description |
| --- | --- | --- | --- |
| affected_by_griefing_rule | Boolean |  | If true, whether the goal is affected by the mob griefing game rule. |
| can_place | Minecraft Filter |  | Filters for if the entity should try to place its block. Self and Target are set. |
| chance | Decimal |  | Chance each tick for the entity to try and place a block. |
| on_place | Trigger |  | Trigger ran if the entity does place its block. Self, Target, and Block are set. |
| placeable_carried_blocks | Array |  | Block descriptors for which blocks are valid to be placed from the entity's carried item, if empty all blocks are valid. |
| randomly_placeable_blocks | Array |  | Weighted block descriptors for which blocks should be randomly placed, if empty the entity will try to place its carried block from placeable_carried_blocks. |
| xz_range | Range [a, b] |  | XZ range from which the entity will try and place blocks in. |
| y_range | Range [a, b] |  | Y range from which the entity will try and place blocks in. |


## minecraft:behavior.play


Allows the mob to play with other mobs by chasing each other and moving around randomly.


| Name | Type | Default Value | Description |
| --- | --- | --- | --- |
| chance_to_start | Decimal | 0.00 | Percent chance that the mob will start this goal, from 0 to 1. |
| follow_distance | Integer | 2 | The distance (in blocks) that the mob tries to be in range of the friend it's following. |
| friend_search_area | Vector [a, b, c] | [6, 3, 6] | The dimensions of the AABB used to search for a potential friend to play with. |
| friend_types | Minecraft Filter |  | The entity type(s) to consider when searching for a potential friend to play with. |
| max_play_duration_seconds | Decimal | 50.00 | The max amount of seconds that the mob will play for before exiting the Goal. |
| random_pos_search_height | Integer | 3 | The height (in blocks) that the mob will search within to find a random position position to move to. Must be at least 1. |
| random_pos_search_range | Integer | 16 | The distance (in blocks) on ground that the mob will search within to find a random position to move to. Must be at least 1. |
| speed_multiplier | Decimal | 1.00 | Movement speed multiplier of the mob when using this AI Goal. |


## minecraft:behavior.play_dead


Allows this entity to pretend to be dead to avoid being targeted by attackers.


| Name | Type | Default Value | Description |
| --- | --- | --- | --- |
| apply_regeneration | Boolean | false | Whether the mob will receive the regeneration effect while playing dead. |
| damage_sources | Array | all | The list of Entity Damage Sources that will cause this mob to play dead. |
| duration | Decimal | 1.000000 | The amount of time the mob will remain playing dead (in seconds). |
| filters | Minecraft Filter |  | The list of other triggers that are required for the mob to activate play dead |
| force_below_health | Integer | 0 | The amount of health at which damage will cause the mob to play dead. |
| random_damage_range | Range [a, b] | [0, 0] | The range of damage that may cause the goal to start depending on randomness. Damage taken below the min will never cause the goal to start. Damage taken above the max will always cause the goal to start. |
| random_start_chance | Decimal | 1.000000 | The likelihood of this goal starting upon taking damage. |


## minecraft:behavior.player_ride_tamed


Allows the mob to be ridden by the player after being tamed.


## minecraft:behavior.raid_garden


Allows the mob to eat/raid crops out of farms until they are full.


| Name | Type | Default Value | Description |
| --- | --- | --- | --- |
| blocks | List |  | Blocks that the mob is looking for to eat/raid |
| eat_delay | Integer | 2 | Time in seconds between each time it eats/raids |
| full_delay | Integer | 100 | Amount of time in seconds before this mob wants to eat/raid again after eating its maximum |
| goal_radius | Decimal | 0.5 | Distance in blocks within the mob considers it has reached the goal. This is the "wiggle room" to stop the AI from bouncing back and forth trying to reach a specific spot |
| initial_eat_delay | Integer | 0 | Time in seconds before starting to eat/raid once it arrives at it |
| max_to_eat | Integer | 6 | Maximum number of crops this entity wants to eat/raid. If set to zero or less then it doesn't have a maximum |
| search_range | Integer | 0 | Distance in blocks the mob will look for crops to eat |
| speed_multiplier | Decimal | 1.0 | Movement speed multiplier of the mob when using this AI Goal |


## minecraft:behavior.ram_attack


Allows this entity to damage a target by using a running attack.


| Name | Type | Default Value | Description |
| --- | --- | --- | --- |
| baby_knockback_modifier | Decimal | 0.333333 | The modifier to knockback that babies have. |
| cooldown_range | Range [a, b] | [10, 20] | Minimum and maximum cooldown time-range (positive, in seconds) between each attempted ram attack. |
| knockback_force | Decimal | 5.000000 | The force of the knockback of the ram attack. |
| knockback_height | Decimal | 0.100000 | The height of the knockback of the ram attack. |
| min_ram_distance | Integer | 4 | The minimum distance at which the mob can start a ram attack. |
| on_start | Array |  | The event to trigger when attacking |
| pre_ram_sound | String |  | The sound to play when an entity is about to perform a ram attack. |
| ram_distance | Integer | 7 | The distance at which the mob start to run with ram speed. |
| ram_impact_sound | String |  | The sound to play when an entity is impacting on a ram attack. |
| ram_speed | Decimal | 2.000000 | Sets the entity's speed when charging toward the target. |
| run_speed | Decimal | 1.000000 | Sets the entity's speed when running toward the target. |


## minecraft:behavior.random_breach


Allows the mob to randomly break surface of the water.


| Name | Type | Default Value | Description |
| --- | --- | --- | --- |
| cooldown_time | Decimal | 0.0 | Time in seconds the mob has to wait before using the goal again |
| interval | Integer | 120 | A random value to determine when to randomly move somewhere. This has a 1/interval chance to choose this goal |
| speed_multiplier | Decimal | 1.0 | Movement speed multiplier of the mob when using this AI Goal |
| xz_dist | Integer | 10 | Distance in blocks on ground that the mob will look for a new spot to move to. Must be at least 1 |
| y_dist | Integer | 7 | Distance in blocks that the mob will look up or down for a new spot to move to. Must be at least 1 |


## minecraft:behavior.random_fly


Allows a mob to randomly fly around.


| Name | Type | Default Value | Description |
| --- | --- | --- | --- |
| can_land_on_trees | Boolean | true | If true, the mob can stop flying and land on a tree instead of the ground |
| speed_multiplier | Decimal | 1.0 | Movement speed multiplier of the mob when using this AI Goal |
| xz_dist | Integer | 10 | Distance in blocks on ground that the mob will look for a new spot to move to. Must be at least 1 |
| y_dist | Integer | 7 | Distance in blocks that the mob will look up or down for a new spot to move to. Must be at least 1 |


## minecraft:behavior.random_hover


Allows the mob to hover around randomly, close to the surface


| Name | Type | Default Value | Description |
| --- | --- | --- | --- |
| hover_height | Range [a, b] |  | The height above the surface which the mob will try to maintain |
| interval | Integer | 120 | A random value to determine when to randomly move somewhere. This has a 1/interval chance to choose this goal |
| speed_multiplier | Decimal | 1.0 | Movement speed multiplier of the mob when using this AI Goal |
| xz_dist | Integer | 10 | Distance in blocks on ground that the mob will look for a new spot to move to. Must be at least 1 |
| y_dist | Integer | 7 | Distance in blocks that the mob will look up or down for a new spot to move to. Must be at least 1 |
| y_offset | Decimal | 0.0 | Height in blocks to add to the selected target position |


## minecraft:behavior.random_look_around


Allows the mob to randomly look around.


| Name | Type | Default Value | Description |
| --- | --- | --- | --- |
| look_time | Range [a, b] | [2, 4] | The range of time in seconds the mob will stay looking in a random direction before looking elsewhere |
| max_angle_of_view_horizontal | Integer | 30 | The rightmost angle a mob can look at on the horizontal plane with respect to its initial facing direction. |
| min_angle_of_view_horizontal | Integer | -30 | The leftmost angle a mob can look at on the horizontal plane with respect to its initial facing direction. |


## minecraft:behavior.random_look_around_and_sit


Allows the mob to randomly sit and look around for a duration. Note: Must have a sitting animation set up to use this.


| Name | Type | Default Value | Description |
| --- | --- | --- | --- |
| continue_if_leashed | Boolean | false | If the goal should continue to be used as long as the mob is leashed. |
| continue_sitting_on_reload | Boolean | false | The mob will stay sitting on reload. |
| max_angle_of_view_horizontal | Decimal | 30.0 | The rightmost angle a mob can look at on the horizontal plane with respect to its initial facing direction. |
| max_look_count | Integer | 2 | The max amount of unique looks a mob will have while looking around. |
| max_look_time | Integer | 40 | The max amount of time (in ticks) a mob will stay looking at a direction while looking around. |
| min_angle_of_view_horizontal | Decimal | -30.0 | The leftmost angle a mob can look at on the horizontal plane with respect to its initial facing direction. |
| min_look_count | Integer | 1 | The min amount of unique looks a mob will have while looking around. |
| min_look_time | Integer | 20 | The min amount of time (in ticks) a mob will stay looking at a direction while looking around. |
| probability | Decimal | 0.02 | The probability of randomly looking around/sitting. |
| random_look_around_cooldown | Integer | 0 | The cooldown in seconds before the goal can be used again. |


## minecraft:behavior.random_search_and_dig


Allows this entity to locate a random target block that it can path find to. Once found, the entity will move towards it and dig up an item. [Default target block types: Dirt, Grass, Podzol, DirtWithRoots, MossBlock, Mud, MuddyMangroveRoots].


| Name | Type | Default Value | Description |
| --- | --- | --- | --- |
| cooldown_range | Range [a, b] | [0.000000, 0.000000] | Goal cooldown range in seconds. |
| digging_duration_range | Range [a, b] | [8.000000, 10.000000] | Digging duration in seconds. |
| find_valid_position_retries | Integer | 5 | Amount of retries to find a valid target position within search range. |
| goal_radius | Decimal | 1.500000 | Distance in blocks within the entity to considers it has reached it's target position. |
| item_table | String |  | File path relative to the resource pack root for items to spawn list (loot table format). |
| on_digging_start | Trigger |  | Event to run when the goal ends searching has begins digging. |
| on_fail_during_digging | Trigger |  | Event to run when the goal failed while in digging state. |
| on_fail_during_searching | Trigger |  | Event to run when the goal failed while in searching state. |
| on_item_found | Trigger |  | Event to run when the goal find a item. |
| on_searching_start | Trigger |  | Event to run when the goal starts searching. |
| on_success | Trigger |  | Event to run when searching and digging has ended. |
| search_range_xz | Integer | 10 | Width and length of the volume around the entity used to find a valid target position |
| search_range_y | Integer | 7 | Height of the volume around the entity used to find a valid target position |
| spawn_item_after_seconds | Decimal | 0.000000 | Digging duration before spawning item in seconds. |
| spawn_item_pos_offset | Decimal | 0.000000 | Distance to offset the item's spawn location in the direction the mob is facing. |
| speed_multiplier | Decimal | 1.000000 | Searching movement speed multiplier. |
| target_blocks | Array |  | List of target block types the goal will look to dig on. Overrides the default list. |
| target_dig_position_offset | Decimal | 2.250000 | Dig target position offset from the feet position of the mob in their facing direction. |


## minecraft:behavior.random_sitting


Allows the mob to randomly sit for a duration.


| Name | Type | Default Value | Description |
| --- | --- | --- | --- |
| cooldown_time | Decimal | 0.0 | Time in seconds the mob has to wait before using the goal again |
| min_sit_time | Decimal | 10 | The minimum amount of time in seconds before the mob can stand back up |
| start_chance | Decimal | 0.1 | This is the chance that the mob will start this goal, from 0 to 1 |
| stop_chance | Decimal | 0.3 | This is the chance that the mob will stop this goal, from 0 to 1 |


## minecraft:behavior.random_stroll


Allows a mob to randomly stroll around.


| Name | Type | Default Value | Description |
| --- | --- | --- | --- |
| interval | Integer | 120 | A random value to determine when to randomly move somewhere. This has a 1/interval chance to choose this goal |
| speed_multiplier | Decimal | 1.0 | Movement speed multiplier of the mob when using this AI Goal |
| xz_dist | Integer | 10 | Distance in blocks on ground that the mob will look for a new spot to move to. Must be at least 1 |
| y_dist | Integer | 7 | Distance in blocks that the mob will look up or down for a new spot to move to. Must be at least 1 |


## minecraft:behavior.random_swim


Allows an entity to randomly move through water


| Name | Type | Default Value | Description |
| --- | --- | --- | --- |
| avoid_surface | Boolean | true | If true, the mob will avoid surface water blocks by swimming below them |
| interval | Integer | 120 | A random value to determine when to randomly move somewhere. This has a 1/interval chance to choose this goal |
| speed_multiplier | Decimal | 1.0 | Movement speed multiplier of the mob when using this AI Goal |
| xz_dist | Integer | 10 | Distance in blocks on ground that the mob will look for a new spot to move to. Must be at least 1 |
| y_dist | Integer | 7 | Distance in blocks that the mob will look up or down for a new spot to move to. Must be at least 1 |


## minecraft:behavior.ranged_attack


Allows an entity to attack by using ranged shots. "charge_shoot_trigger" must be greater than 0 to enable charged up burst-shot attacks. Requires minecraft:shooter to define projectile behaviour.


| Name | Type | Default Value | Description |
| --- | --- | --- | --- |
| attack_interval | Decimal | 0 | Alternative to "attack_interval_min" & "attack_interval_max". Consistent reload-time (in seconds), when not using a charged shot. Does not scale with target-distance. |
| attack_interval_max | Decimal | 0 | Maximum bound for reload-time range (in seconds), when not using a charged shot. Reload-time range scales with target-distance. |
| attack_interval_min | Decimal | 0 | Minimum bound for reload-time range (in seconds), when not using a charged shot. Reload-time range scales with target-distance. |
| attack_radius | Decimal | 0 | Minimum distance to target before this entity will attempt to shoot. |
| attack_radius_min | Decimal | 0 | Minimum distance the target can be for this mob to fire. If the target is closer, this mob will move first before firing |
| burst_interval | Decimal | 0 | Time (in seconds) between each individual shot when firing a burst of shots from a charged up attack. |
| burst_shots | Integer | 1 | Number of shots fired every time the attacking entity uses a charged up attack. |
| charge_charged_trigger | Decimal | 0 | Time (in seconds, then add "charge_shoot_trigger"), before a charged up attack is done charging. Charge-time decays while target is not in sight. |
| charge_shoot_trigger | Decimal | 0 | Amount of time (in seconds, then doubled) a charged shot must be charging before reloading burst shots. Charge-time decays while target is not in sight. |
| ranged_fov | Decimal | 90 | Field of view (in degrees) when using sensing to detect a target for attack. |
| set_persistent | Boolean | false | Allows the actor to be set to persist upon targeting a player |
| speed_multiplier | Decimal | 1 | During attack behavior, this multiplier modifies the entity's speed when moving toward the target. |
| swing | Boolean | false | If a swing animation (using variable.attack_time) exists, this causes the actor to swing their arm(s) upon firing the ranged attack. |
| target_in_sight_time | Decimal | 1 | Minimum amount of time (in seconds) the attacking entity needs to see the target before moving toward it. |
| x_max_rotation | Decimal | 30 | Maximum rotation (in degrees), on the X-axis, this entity can rotate while trying to look at the target. |
| y_max_head_rotation | Decimal | 30 | Maximum rotation (in degrees), on the Y-axis, this entity can rotate its head while trying to look at the target. |


## minecraft:behavior.receive_love


Allows the villager to stop so another villager can breed with it. Can only be used by a Villager.


## minecraft:behavior.restrict_open_door


Allows the mob to stay indoors during night time.


## minecraft:behavior.restrict_sun


Allows the mob to automatically start avoiding the sun when its a clear day out.


## minecraft:behavior.rise_to_liquid_level


Allows the mob to stay at a certain level when in liquid.


| Name | Type | Default Value | Description |
| --- | --- | --- | --- |
| liquid_y_offset | Decimal | 0.0 | Target distance down from the liquid surface. i.e. Positive values move the target Y down. |
| rise_delta | Decimal | 0.0 | Movement up in Y per tick when below the liquid surface. |
| sink_delta | Decimal | 0.0 | Movement down in Y per tick when above the liquid surface. |


## minecraft:behavior.roar (See JSON Schema since 1.21.110)


Allows this entity to roar at another entity based on data in minecraft:anger_level. Once the anger threshold specified in minecraft:anger_level has been reached, this entity will roar for the specified amount of time, look at the other entity, apply anger boost towards it, and finally target it.


| Name | Type | Default Value | Description |
| --- | --- | --- | --- |
| duration | Decimal | 0.0 | The amount of time to roar for. |


## minecraft:behavior.roll


This allows the mob to roll forward.


| Name | Type | Default Value | Description |
| --- | --- | --- | --- |
| probability | Decimal | [1.0] | The probability that the mob will use the goal. |


## minecraft:behavior.run_around_like_crazy


Allows the mob to run around aimlessly.


| Name | Type | Default Value | Description |
| --- | --- | --- | --- |
| speed_multiplier | Decimal | 1.0 | Movement speed multiplier of the mob when using this AI Goal |


## minecraft:behavior.scared


Allows the a mob to become scared when the weather outside is thundering


| Name | Type | Default Value | Description |
| --- | --- | --- | --- |
| sound_interval | Integer | 0 | The interval in which a sound will play when active in a 1/delay chance to kick off |


## minecraft:behavior.send_event


Allows the mob to send an event to another mob.


| Name | Type | Default Value | Description |
| --- | --- | --- | --- |
| cast_duration | Decimal | Total delay of the steps | Time in seconds for the entire event sending process |
| look_at_target | Boolean | true | If true, the mob will face the entity it sends an event to |
| sequence | List |  | List of events to send |


> **sequence** 子参数


| Name | Type | Default Value | Description |
| --- | --- | --- | --- |
| base_delay | Decimal | 0.0 | Amount of time in seconds before starting this step |
| event | String |  | The event to send to the entity |
| sound_event | String |  | The sound event to play when this step happens |


## minecraft:behavior.share_items


Allows the mob to give items it has to others.


| Name | Type | Default Value | Description |
| --- | --- | --- | --- |
| entity_types | JSON Object |  | List of entities this mob will share items with |
| goal_radius | Decimal | 0.5 | Distance in blocks within the mob considers it has reached the goal. This is the "wiggle room" to stop the AI from bouncing back and forth trying to reach a specific spot |
| max_dist | Decimal | 0.0 | Maximum distance in blocks this mob will look for entities to share items with |
| speed_multiplier | Decimal | 1.0 | Movement speed multiplier of the mob when using this AI Goal |


> **entity_types** 子参数


| Name | Type | Default Value | Description |
| --- | --- | --- | --- |
| cooldown | Decimal | 0.0 | The amount of time in seconds that the mob has to wait before selecting a target of the same type again |
| filters | Minecraft Filter |  | Conditions that make this entry in the list valid |
| max_dist | Decimal | 16 | Maximum distance this mob can be away to be a valid choice |
| must_see | Boolean | false | If true, the mob has to be visible to be a valid choice |
| must_see_forget_duration | Decimal | 3.0 | Determines the amount of time in seconds that this mob will look for a target before forgetting about it and looking for a new one when the target isn't visible any more |
| reevaluate_description | Boolean | false | If true, the mob will stop being targeted if it stops meeting any conditions. |
| sprint_speed_multiplier | Decimal | 1.0 | Multiplier for the running speed. A value of 1.0 means the speed is unchanged |
| walk_speed_multiplier | Decimal | 1.0 | Multiplier for the walking speed. A value of 1.0 means the speed is unchanged |


## minecraft:behavior.silverfish_merge_with_stone


Allows the mob to go into stone blocks like Silverfish do. Currently it can only be used by Silverfish.


## minecraft:behavior.silverfish_wake_up_friends


Allows the mob to alert mobs in nearby blocks to come out. Currently it can only be used by Silverfish.


## minecraft:behavior.skeleton_horse_trap


Allows Equine mobs to be Horse Traps and be triggered like them, spawning a lightning bolt and a bunch of horses when a player is nearby. Can only be used by Horses, Mules, Donkeys and Skeleton Horses.


| Name | Type | Default Value | Description |
| --- | --- | --- | --- |
| duration | Decimal | 1.0 | Amount of time in seconds the trap exists. After this amount of time is elapsed, the trap is removed from the world if it hasn't been activated |
| within_radius | Decimal | 0.0 | Distance in blocks that the player has to be within to trigger the horse trap |


## minecraft:behavior.sleep


Allows mobs that own a bed to in a village to move to and sleep in it.


| Name | Type | Default Value | Description |
| --- | --- | --- | --- |
| can_sleep_while_riding | Boolean | false | If true, the mob will be able to use the sleep goal if riding something |
| cooldown_time | Decimal | 0.0 | Time in seconds the mob has to wait before using the goal again |
| sleep_collider_height | Decimal | 1.0 | The height of the mob's collider while sleeping |
| sleep_collider_width | Decimal | 1.0 | The width of the mob's collider while sleeping |
| sleep_y_offset | Decimal | 1.0 | The y offset of the mob's collider while sleeping |
| speed_multiplier | Decimal | 1.0 | Movement speed multiplier of the mob when using this AI Goal |
| timeout_cooldown | Decimal | 8.0 | The cooldown time in seconds before the goal can be reused after a internal failure or timeout condition |


## minecraft:behavior.slime_attack (See JSON Schema since 1.21.110)


Causes the entity to grow tired every once in a while, while attacking.


| Name | Type | Default Value | Description |
| --- | --- | --- | --- |
| set_persistent | Boolean | false | Allows the actor to be set to persist upon targeting a player |
| speed_multiplier | Decimal | 1 | During attack behavior, this multiplier modifies the entity's speed when moving toward the target. |
| x_max_rotation | Decimal | 10 | Maximum rotation (in degrees), on the X-axis, this entity can rotate while trying to look at the target. |
| y_max_rotation | Decimal | 10 | Maximum rotation (in degrees), on the Y-axis, this entity can rotate while trying to look at the target. |


## minecraft:behavior.slime_float (See JSON Schema since 1.21.110)


Allow slimes to float in water / lava. Can only be used by Slime and Magma Cubes.


| Name | Type | Default Value | Description |
| --- | --- | --- | --- |
| jump_chance_percentage | Decimal | 0.8 | Percent chance a slime or magma cube has to jump while in water / lava. |
| speed_multiplier | Decimal | 1.2 | Determines the multiplier the entity's speed is modified by when moving through water / lava. |


## minecraft:behavior.slime_keep_on_jumping (See JSON Schema since 1.21.110)


Allows the entity to continuously jump around like a slime.


| Name | Type | Default Value | Description |
| --- | --- | --- | --- |
| speed_multiplier | Decimal | 1 | Determines the multiplier this entity's speed is modified by when jumping around. |


## minecraft:behavior.slime_random_direction (See JSON Schema since 1.21.110)


Allows the entity to move in random directions like a slime.


| Name | Type | Default Value | Description |
| --- | --- | --- | --- |
| add_random_time_range | Integer | 3 | Additional time (in whole seconds), chosen randomly in the range of [0, "add_random_time_range"], to add to "min_change_direction_time". |
| min_change_direction_time | Decimal | 2 | Constant minimum time (in seconds) to wait before choosing a new direction. |
| turn_range | Integer | 360 | Maximum rotation angle range (in degrees) when randomly choosing a new direction. |


## minecraft:behavior.snacking


Allows the mob to take a load off and snack on food that it found nearby.


| Name | Type | Default Value | Description |
| --- | --- | --- | --- |
| items | List |  | Items that we are interested in snacking on |
| snacking_cooldown | Decimal | 7.5 | The cooldown time in seconds before the mob is able to snack again |
| snacking_cooldown_min | Decimal | 0.5f | The minimum time in seconds before the mob is able to snack again |
| snacking_stop_chance | Decimal | 0.0017 | This is the chance that the mob will stop snacking, from 0 to 1 |


## minecraft:behavior.sneeze


Allows the mob to stop and sneeze possibly startling nearby mobs and dropping an item.


| Name | Type | Default Value | Description |
| --- | --- | --- | --- |
| cooldown_time | Decimal | 0.0 | Time in seconds the mob has to wait before using the goal again |
| drop_item_chance | Decimal | 1.0 | The probability that the mob will drop an item when it sneezes. |
| entity_types | JSON Object |  | List of entity types this mob will startle (cause to jump) when it sneezes. |
| loot_table | String |  | Loot table to select dropped items from. |
| prepare_sound | String |  | Sound to play when the sneeze is about to happen. |
| prepare_time | Decimal | 1.0 | The time in seconds that the mob takes to prepare to sneeze (while the prepare_sound is playing). |
| probability | Decimal | 0.02 | The probability of sneezing. A value of 1.00 is 100% |
| sound | String |  | Sound to play when the sneeze occurs. |
| within_radius | Decimal | 0.0 | Distance in blocks that mobs will be startled. |


> **entity_types** 子参数


| Name | Type | Default Value | Description |
| --- | --- | --- | --- |
| cooldown | Decimal | 0.0 | The amount of time in seconds that the mob has to wait before selecting a target of the same type again |
| filters | Minecraft Filter |  | Conditions that make this entry in the list valid |
| max_dist | Decimal | 16 | Maximum distance this mob can be away to be a valid choice |
| must_see | Boolean | false | If true, the mob has to be visible to be a valid choice |
| must_see_forget_duration | Decimal | 3.0 | Determines the amount of time in seconds that this mob will look for a target before forgetting about it and looking for a new one when the target isn't visible any more |
| reevaluate_description | Boolean | false | If true, the mob will stop being targeted if it stops meeting any conditions. |
| sprint_speed_multiplier | Decimal | 1.0 | Multiplier for the running speed. A value of 1.0 means the speed is unchanged |
| walk_speed_multiplier | Decimal | 1.0 | Multiplier for the walking speed. A value of 1.0 means the speed is unchanged |


## minecraft:behavior.sniff (See JSON Schema since 1.21.110)


Allows this entity to detect the nearest player within "sniffing_radius" and update its "minecraft:suspect_tracking" component state


| Name | Type | Default Value | Description |
| --- | --- | --- | --- |
| cooldown_range | Range [a, b] | [3, 10] | Cooldown range between sniffs in seconds |
| duration | Decimal | 1.0 | Sniffing duration in seconds |
| sniffing_radius | Decimal | 5.0 | Mob detection radius |
| suspicion_radius_horizontal | Decimal | 3.0 | Mob suspicion horizontal radius. When a player is within this radius horizontally, the anger level towards that player is increased |
| suspicion_radius_vertical | Decimal | 3.0 | Mob suspicion vertical radius. When a player is within this radius vertically, the anger level towards that player is increased |


## minecraft:behavior.sonic_boom (See JSON Schema since 1.21.110)


Allows this entity to perform a 'sonic boom' ranged attack


| Name | Type | Default Value | Description |
| --- | --- | --- | --- |
| attack_cooldown | Decimal | 5.00 | Cooldown in seconds required after using this attack until the entity can use sonic boom again. |
| attack_damage | Decimal | 30.00 | Attack damage of the sonic boom. |
| attack_range_horizontal | Decimal | 15.00 | Horizontal range (in blocks) at which the sonic boom can damage the target. |
| attack_range_vertical | Decimal | 20.00 | Vertical range (in blocks) at which the sonic boom can damage the target. |
| attack_sound | String |  | Sound event for the attack. |
| charge_sound | String |  | Sound event for the charge up. |
| duration | Decimal | 0.00 | Goal duration in seconds |
| duration_until_attack_sound | Decimal | 1.70 | Duration in seconds until the attack sound is played. |
| knockback_height_cap | Decimal | 0.00 | Height cap of the attack knockback's vertical delta. |
| knockback_horizontal_strength | Decimal | 0.00 | Horizontal strength of the attack's knockback applied to the attack target. |
| knockback_vertical_strength | Decimal | 0.00 | Vertical strength of the attack's knockback applied to the attack target. |
| speed_multiplier | Decimal | 1.00 | This multiplier modifies the attacking entity's speed when moving toward the target. |


## minecraft:behavior.squid_dive


Allows the squid to dive down in water. Can only be used by the Squid.


## minecraft:behavior.squid_flee


Allows the squid to swim away. Can only be used by the Squid.


## minecraft:behavior.squid_idle


Allows the squid to swim in place idly. Can only be used by the Squid.


## minecraft:behavior.squid_move_away_from_ground


Allows the squid to move away from ground blocks and back to water. Can only be used by the Squid.


## minecraft:behavior.squid_out_of_water


Allows the squid to stick to the ground when outside water. Can only be used by the Squid.


## minecraft:behavior.stalk_and_pounce_on_target


Allows a mob to stalk a target, then once within range pounce onto a target, on success the target will be attacked dealing damage defined by the attack component. On failure, the mob will risk getting stuck


| Name | Type | Default Value | Description |
| --- | --- | --- | --- |
| interest_time | Decimal | 2.0 | The amount of time the mob will be interested before pouncing. This happens when the mob is within range of pouncing |
| leap_distance | Decimal | 0.8 | The distance in blocks the mob jumps in the direction of its target |
| leap_height | Decimal | 0.9 | The height in blocks the mob jumps when leaping at its target |
| max_stalk_dist | Decimal | 10.0 | The maximum distance away a target can be before the mob gives up on stalking |
| pounce_max_dist | Decimal | 5.0 | The maximum distance away from the target in blocks to begin pouncing at the target |
| set_persistent | Boolean | false | Allows the actor to be set to persist upon targeting a player |
| stalk_speed | Decimal | 1.2 | The movement speed in which you stalk your target |
| strike_dist | Decimal | 2.0 | The max distance away from the target when landing from the pounce that will still result in damaging the target |
| stuck_time | Decimal | 2.0 | The amount of time the mob will be stuck if they fail and land on a block they can be stuck on |


## minecraft:behavior.stay_near_noteblock (See JSON Schema since 1.21.110)


The entity will attempt to toss the items from its inventory to a nearby recently played noteblock.


| Name | Type | Default Value | Description |
| --- | --- | --- | --- |
| listen_time | Integer | 30 | Sets the time an entity should stay near a noteblock after hearing it. |
| speed | Decimal | 1.000000 | Sets the entity's speed when moving toward the block. |
| start_distance | Decimal | 10.000000 | Sets the distance the entity needs to be away from the block to attempt to start the goal. |
| stop_distance | Decimal | 2.000000 | Sets the distance from the block the entity will attempt to reach. |


## minecraft:behavior.stay_while_sitting


Allows the mob to stay put while it is in a sitting state instead of doing something else.


## minecraft:behavior.stomp_attack


Allows an entity to attack using stomp AoE damage behavior.


| Name | Type | Default Value | Description |
| --- | --- | --- | --- |
| attack_once | Boolean | false | Allows the entity to use this attack behavior, only once EVER. |
| attack_types | String | N/A | Defines the entity types this entity will attack. |
| can_spread_on_fire | Boolean | false | If the entity is on fire, this allows the entity's target to catch on fire after being hit. |
| cooldown_time | Decimal | 1 | Cooldown time (in seconds) between attacks. |
| inner_boundary_time_increase | Decimal | 0.25 | Time (in seconds) to add to attack path recalculation when the target is beyond the "path_inner_boundary". |
| max_path_time | Decimal | 0.55 | Maximum base time (in seconds) to recalculate new attack path to target (before increases applied). |
| min_path_time | Decimal | 0.2 | Minimum base time (in seconds) to recalculate new attack path to target (before increases applied). |
| no_damage_range_multiplier | Decimal | 2 | Multiplied with the final AoE damage range to determine a no damage range. The stomp attack will go on cooldown if target is in this no damage range. |
| on_attack | Trigger | N/A | Defines the event to trigger when this entity successfully attacks. |
| on_kill | Trigger | N/A | Defines the event to trigger when this entity kills the target. |
| outer_boundary_time_increase | Decimal | 0.5 | Time (in seconds) to add to attack path recalculation when the target is beyond the "path_outer_boundary". |
| path_fail_time_increase | Decimal | 0.75 | Time (in seconds) to add to attack path recalculation when this entity cannot move along the current path. |
| path_inner_boundary | Decimal | 16 | Distance at which to increase attack path recalculation by "inner_boundary_tick_increase". |
| path_outer_boundary | Decimal | 32 | Distance at which to increase attack path recalculation by "outer_boundary_tick_increase". |
| random_stop_interval | Integer | 0 | This entity will have a 1 in N chance to stop it's current attack, where N = "random_stop_interval". |
| reach_multiplier | Decimal | 2 | Used with the base size of the entity to determine minimum target-distance before trying to deal attack damage. |
| require_complete_path | Boolean | false | Toggles (on/off) the need to have a full path from the entity to the target when using this melee attack behavior. |
| speed_multiplier | Decimal | 1 | This multiplier modifies the attacking entity's speed when moving toward the target. |
| stomp_range_multiplier | Decimal | 2 | Multiplied with the base size of the entity to determine stomp AoE damage range. |
| track_target | Boolean | false | Allows the entity to track the attack target, even if the entity has no sensing. |
| x_max_rotation | Decimal | 30 | Maximum rotation (in degrees), on the X-axis, this entity can rotate while trying to look at the target. |
| y_max_head_rotation | Decimal | 30 | Maximum rotation (in degrees), on the Y-axis, this entity can rotate its head while trying to look at the target. |


## minecraft:behavior.stomp_turtle_egg


Allows this mob to stomp turtle eggs


| Name | Type | Default Value | Description |
| --- | --- | --- | --- |
| goal_radius | Decimal | 0.5 | Distance in blocks within the mob considers it has reached the goal. This is the "wiggle room" to stop the AI from bouncing back and forth trying to reach a specific spot |
| interval | Integer | 120 | A random value to determine when to randomly move somewhere. This has a 1/interval chance to choose this goal |
| search_height | Integer | 1 | Height in blocks the mob will look for turtle eggs to move towards |
| search_range | Integer | 0 | The distance in blocks it will look for turtle eggs to move towards |
| speed_multiplier | Decimal | 1.0 | Movement speed multiplier of the mob when using this AI Goal |


## minecraft:behavior.stroll_towards_village


Allows the mob to move into a random location within a village within the search range.


| Name | Type | Default Value | Description |
| --- | --- | --- | --- |
| cooldown_time | Decimal | 0.0 | Time in seconds the mob has to wait before using the goal again |
| goal_radius | Decimal | 0.5 | Distance in blocks within the mob considers it has reached the goal. This is the "wiggle room" to stop the AI from bouncing back and forth trying to reach a specific spot |
| search_range | Integer | 0 | The distance in blocks to search for points inside villages. If <= 0, find the closest village regardless of distance. |
| speed_multiplier | Decimal | 1.0 | Movement speed multiplier of the mob when using this AI Goal |
| start_chance | Decimal | 0.1 | This is the chance that the mob will start this goal, from 0 to 1 |


## minecraft:behavior.summon_entity


Allows the mob to attack the player by summoning other entities.


| Name | Type | Default Value | Description |
| --- | --- | --- | --- |
| summon_choices | List |  | List of spells for the mob to use to summon entities. Each spell has the following parameters: |


> **summon_choices** 子参数


| Name | Type | Default Value | Description |
| --- | --- | --- | --- |
| cast_duration | Decimal | Total delay of the steps | Time in seconds the spell casting will take |
| cooldown_time | Decimal | 0.0 | Time in seconds the mob has to wait before using the spell again |
| do_casting | Boolean | true | If true, the mob will do the casting animations and render spell particles |
| filters | Minecraft Filter |  |  |
| max_activation_range | Decimal | 32.0 | Upper bound of the activation distance in blocks for this spell, must not be negative. |
| min_activation_range | Decimal | 1.0 | Lower bound of the activation distance in blocks for this spell, must not be negative. |
| particle_color | Integer | 0 | The color of the particles for this spell |
| sequence | List |  | List of steps for the spell. Each step has the following parameters: |
| start_sound_event | String |  | The sound event to play when using this spell |
| weight | Decimal | 0.0 | The weight of this spell. Controls how likely the mob is to choose this spell when casting one |


> **sequence** 子参数


| Name | Type | Default Value | Description |
| --- | --- | --- | --- |
| base_delay | Decimal | 0.0 | Amount of time in seconds to wait before this step starts |
| delay_per_summon | Decimal | 0.0 | Amount of time in seconds before each entity is summoned in this step |
| entity_lifespan | Decimal | -1.0 | Amount of time in seconds that the spawned entity will be alive for. A value of -1.0 means it will remain alive for as long as it can |
| entity_type | String |  | The entity type of the entities we will spawn in this step |
| num_entities_spawned | Integer | 1 | Number of entities that will be spawned in this step |
| shape | String | line | The base shape of this step. Valid values are circle and line |
| size | Decimal | 1.0 | The base size of the entity |
| sound_event | String |  | The sound event to play for this step |
| summon_cap | Integer | 0 | Maximum number of summoned entities at any given time |
| summon_cap_radius | Decimal | 0.0 |  |
| summon_event | String |  | Event to invoke on each summoned entity on spawn |
| target | String | self | The target of the spell. This is where the spell will start (line will start here, circle will be centered here) |


## minecraft:behavior.swell


Allows the creeper to swell up when a player is nearby. It can only be used by Creepers.


| Name | Type | Default Value | Description |
| --- | --- | --- | --- |
| start_distance | Decimal | 10.0 | This mob starts swelling when a target is at least this many blocks away |
| stop_distance | Decimal | 2 | This mob stops swelling when a target has moved away at least this many blocks |


## minecraft:behavior.swim_idle (See JSON Schema since 1.21.110)


Allows the entity go idle, if swimming. Entity must be in water.


| Name | Type | Default Value | Description |
| --- | --- | --- | --- |
| idle_time | Decimal | 5 | Amount of time (in seconds) to stay idle. |
| success_rate | Decimal | 0.1 | Percent chance this entity will go idle, 1.0 = 100%. |


## minecraft:behavior.swim_up_for_breath (See JSON Schema since 1.21.110)


Allows the mob to try to move to air once it is close to running out of its total breathable supply. Requires "minecraft:breathable".


| Name | Type | Default Value | Description |
| --- | --- | --- | --- |
| material_type | String | water | The material the mob is traveling in. An air block will only be considered valid to move to with a block of this material below it. Options are: "water", "lava", or "any". |
| search_height | Integer | 16 | The height (in blocks) above the mob's current position that it will search for a valid air block to move to. If a valid block cannot be found, the mob will move to the position this many blocks above it. |
| search_radius | Integer | 4 | The radius (in blocks) around the mob's current position that it will search for a valid air block to move to. |
| speed_mod | Decimal | 1.40 | Movement speed multiplier of the mob when using this Goal. |


## minecraft:behavior.swim_wander (See JSON Schema since 1.21.110)


Allows the entity to wander around while swimming, when not path-finding.


| Name | Type | Default Value | Description |
| --- | --- | --- | --- |
| interval | Decimal | 0.00833 | Percent chance to start wandering, when not path-finding. 1 = 100% |
| look_ahead | Decimal | 5 | Distance to look ahead for obstacle avoidance, while wandering. |
| speed_multiplier | Decimal | 1 | This multiplier modifies the entity's speed when wandering. |
| wander_time | Decimal | 5 | Amount of time (in seconds) to wander after wandering behavior was successfully started. |


## minecraft:behavior.swim_with_entity


Allows the entity follow another entity. Both entities must be swimming [ie, in water].


| Name | Type | Default Value | Description |
| --- | --- | --- | --- |
| catch_up_multiplier | Decimal | 2.5 | The multiplier this entity's speed is modified by when matching another entity's direction. |
| catch_up_threshold | Decimal | 12 | Distance, from the entity being followed, at which this entity will speed up to reach that entity. |
| chance_to_stop | Decimal | 0.0333 | Percent chance to stop following the current entity, if they're riding another entity or they're not swimming. 1.0 = 100% |
| entity_types | Minecraft Filter |  | Filters which types of entities are valid to follow. |
| match_direction_threshold | Decimal | 2 | Distance, from the entity being followed, at which this entity will try to match that entity's direction |
| search_range | Decimal | 20 | Radius around this entity to search for another entity to follow. |
| speed_multiplier | Decimal | 1.5 | The multiplier this entity's speed is modified by when trying to catch up to the entity being followed. |
| state_check_interval | Decimal | 0.5 | Time (in seconds) between checks to determine if this entity should catch up to the entity being followed or match the direction of the entity being followed. |
| stop_distance | Decimal | 5 | Distance, from the entity being followed, at which this entity will stop following that entity. |
| success_rate | Decimal | 0.1 | Percent chance to start following another entity, if not already doing so. 1.0 = 100% |


## minecraft:behavior.swoop_attack (See JSON Schema since 1.21.110)


Allows an entity to attack using swoop attack behavior; Ideal for use with flying mobs. The behavior ends if the entity has a horizontal collision or gets hit.


| Name | Type | Default Value | Description |
| --- | --- | --- | --- |
| damage_reach | Decimal | 0.2 | Added to the base size of the entity, to determine the target's maximum allowable distance, when trying to deal attack damage. |
| delay_range | Range [a, b] | [10, 20] | Minimum and maximum cooldown time-range (in seconds) between each attempted swoop attack. |
| speed_multiplier | Decimal | 1 | During swoop attack behavior, this determines the multiplier the entity's speed is modified by when moving toward the target. |


## minecraft:behavior.take_block


| Name | Type | Default Value | Description |
| --- | --- | --- | --- |
| affected_by_griefing_rule | Boolean |  | If true, whether the goal is affected by the mob griefing game rule. |
| blocks | Array |  | Block descriptors for which blocks are valid to be taken by the entity, if empty all blocks are valid. |
| can_take | Minecraft Filter |  | Filters for if the entity should try to take a block. Self and Target are set. |
| chance | Decimal |  | Chance each tick for the entity to try and take a block. |
| on_take | Trigger |  | Trigger ran if the entity does take a block. Self, Target, and Block are set. |
| requires_line_of_sight | Boolean |  | If true, whether the entity needs line of sight to the block they are trying to take. |
| xz_range | Range [a, b] |  | XZ range from which the entity will try and take blocks from. |
| y_range | Range [a, b] |  | Y range from which the entity will try and take blocks from. |


## minecraft:behavior.take_flower


Allows the mob to accept flowers from another mob with the minecraft:offer_flower behavior.


| Name | Type | Default Value | Description |
| --- | --- | --- | --- |
| filters | Minecraft Filter |  | Conditions that need to be met for the behavior to start. |
| max_head_rotation_y | Decimal | 30 | Maximum rotation (in degrees), on the Y-axis, this entity can rotate its head while trying to look at the target. |
| max_rotation_x | Decimal | 30 | Maximum rotation (in degrees), on the X-axis, this entity can rotate while trying to look at the target. |
| max_wait_time | Decimal | 20.00 | The maximum amount of time (in seconds) for the mob to randomly wait for before taking the flower. |
| min_distance_to_target | Decimal | 2.00 | Minimum distance (in blocks) for the entity to be considered having reached its target. |
| min_wait_time | Decimal | 4.00 | The minimum amount of time (in seconds) for the mob to randomly wait for before taking the flower. |
| on_take_flower | Trigger |  | Event triggered when the entity takes a flower from another entity. |
| search_area | Vector [a, b, c] | [6, 2, 6] | The dimensions of the AABB used to search for a potential mob to take a flower from. |
| speed_multiplier | Decimal | 0.50 | Movement speed multiplier of the mob when using this AI Goal. |


## minecraft:behavior.teleport_to_owner


Allows an entity to teleport to its owner.


| Name | Type | Default Value | Description |
| --- | --- | --- | --- |
| cooldown | Decimal | 1.00 | The time in seconds that must pass for the entity to be able to try to teleport again. |
| filters | Minecraft Filter |  | Conditions to be satisfied for the entity to teleport to its owner. |


## minecraft:behavior.tempt


Allows a mob to be tempted by a player holding a specific item. Uses pathfinding for movement.


| Name | Type | Default Value | Description |
| --- | --- | --- | --- |
| can_get_scared | Boolean | false | If true, the mob can stop being tempted if the player moves too fast while close to this mob. |
| can_tempt_vertically | Boolean | false | If true, vertical distance to the player will be considered when tempting. |
| can_tempt_while_ridden | Boolean | false | If true, the mob can be tempted even if it has a passenger (i.e. if being ridden). |
| items | Array | [] | List of items that can tempt the mob. |
| sound_interval | Range [a, b] | [0.0, 0.0] | Range of random ticks to wait between tempt sounds. |
| speed_multiplier | Decimal | 1.0 | Movement speed multiplier of the mob when using this AI Goal |
| stop_distance | Decimal | 1.5 | The distance at which the mob will stop following the player. |
| tempt_sound | String |  | Sound to play while the mob is being tempted. |
| within_radius | Decimal | 0.0 | Distance in blocks this mob can get tempted by a player holding an item they like. |


## minecraft:behavior.timer_flag_1


Fires an event when this behavior starts, then waits for a duration before stopping. When stopping due to that timeout or due to being interrupted by another behavior, fires another event. query.timer_flag_1 will return 1.0 on both the client and server when this behavior is running, and 0.0 otherwise.


| Name | Type | Default Value | Description |
| --- | --- | --- | --- |
| cooldown_range | Range [a, b] | [10.000000, 10.000000] | Goal cooldown range in seconds. If specified, the cooldown will have to elapse even before the goal can be selected for the first time. |
| duration_range | Range [a, b] | [2.000000, 2.000000] | Goal duration range in seconds. |
| on_end | Trigger |  | Event(s) to run when the goal end. |
| on_start | Trigger |  | Event(s) to run when the goal starts. |


## minecraft:behavior.timer_flag_2


Fires an event when this behavior starts, then waits for a duration before stopping. When stopping due to that timeout or due to being interrupted by another behavior, fires another event. query.timer_flag_2 will return 1.0 on both the client and server when this behavior is running, and 0.0 otherwise.


| Name | Type | Default Value | Description |
| --- | --- | --- | --- |
| cooldown_range | Range [a, b] | [10.000000, 10.000000] | Goal cooldown range in seconds. If specified, the cooldown will have to elapse even before the goal can be selected for the first time. |
| duration_range | Range [a, b] | [2.000000, 2.000000] | Goal duration range in seconds. |
| on_end | Trigger |  | Event(s) to run when the goal end. |
| on_start | Trigger |  | Event(s) to run when the goal starts. |


## minecraft:behavior.timer_flag_3


Fires an event when this behavior starts, then waits for a duration before stopping. When stopping due to that timeout or due to being interrupted by another behavior, fires another event. query.timer_flag_3 will return 1.0 on both the client and server when this behavior is running, and 0.0 otherwise.


| Name | Type | Default Value | Description |
| --- | --- | --- | --- |
| cooldown_range | Range [a, b] | [10.000000, 10.000000] | Goal cooldown range in seconds. If specified, the cooldown will have to elapse even before the goal can be selected for the first time. |
| duration_range | Range [a, b] | [2.000000, 2.000000] | Goal duration range in seconds. |
| on_end | Trigger |  | Event(s) to run when the goal end. |
| on_start | Trigger |  | Event(s) to run when the goal starts. |


## minecraft:behavior.trade_interest


Allows the mob to look at a player that is holding a tradable item.


| Name | Type | Default Value | Description |
| --- | --- | --- | --- |
| carried_item_switch_time | Decimal | 2.0 | The max time in seconds that the trader will hold an item before attempting to switch for a different item that takes the same trade |
| cooldown | Decimal | 2.0 | The time in seconds before the trader can use this goal again |
| interest_time | Decimal | 45.0 | The max time in seconds that the trader will be interested with showing its trade items |
| remove_item_time | Decimal | 1.0 | The max time in seconds that the trader will wait when you no longer have items to trade |
| within_radius | Decimal | 0.0 | Distance in blocks this mob can be interested by a player holding an item they like |


## minecraft:behavior.trade_with_player


Allows the player to trade with this mob. When the goal starts, it will stop the mob's navigation.


| Name | Type | Default Value | Description |
| --- | --- | --- | --- |
| filters | Minecraft Filter |  | Conditions that need to be met for the behavior to start. |
| max_distance_from_player | Decimal | 8.00 | The max distance that the mob can be from the player before exiting the goal. |


## minecraft:behavior.vex_copy_owner_target


Allows the mob to target the same entity its owner is targeting.


| Name | Type | Default Value | Description |
| --- | --- | --- | --- |
| entity_types | JSON Object |  | List of entities this mob can copy the owner from |


> **entity_types** 子参数


| Name | Type | Default Value | Description |
| --- | --- | --- | --- |
| cooldown | Decimal | 0.0 | The amount of time in seconds that the mob has to wait before selecting a target of the same type again |
| filters | Minecraft Filter |  | Conditions that make this entry in the list valid |
| max_dist | Decimal | 16 | Maximum distance this mob can be away to be a valid choice |
| must_see | Boolean | false | If true, the mob has to be visible to be a valid choice |
| must_see_forget_duration | Decimal | 3.0 | Determines the amount of time in seconds that this mob will look for a target before forgetting about it and looking for a new one when the target isn't visible any more |
| reevaluate_description | Boolean | false | If true, the mob will stop being targeted if it stops meeting any conditions. |
| sprint_speed_multiplier | Decimal | 1.0 | Multiplier for the running speed. A value of 1.0 means the speed is unchanged |
| walk_speed_multiplier | Decimal | 1.0 | Multiplier for the walking speed. A value of 1.0 means the speed is unchanged |


## minecraft:behavior.vex_random_move


Allows the mob to move around randomly like the Vex.


## minecraft:behavior.wither_random_attack_pos_goal


Allows the wither to launch random attacks. Can only be used by the Wither Boss.


## minecraft:behavior.wither_target_highest_damage


Allows the wither to focus its attacks on whichever mob has dealt the most damage to it.


| Name | Type | Default Value | Description |
| --- | --- | --- | --- |
| entity_types | JSON Object |  | List of entity types the wither takes into account to find who dealt the most damage to it |


> **entity_types** 子参数


| Name | Type | Default Value | Description |
| --- | --- | --- | --- |
| cooldown | Decimal | 0.0 | The amount of time in seconds that the mob has to wait before selecting a target of the same type again |
| filters | Minecraft Filter |  | Conditions that make this entry in the list valid |
| max_dist | Decimal | 16 | Maximum distance this mob can be away to be a valid choice |
| must_see | Boolean | false | If true, the mob has to be visible to be a valid choice |
| must_see_forget_duration | Decimal | 3.0 | Determines the amount of time in seconds that this mob will look for a target before forgetting about it and looking for a new one when the target isn't visible any more |
| reevaluate_description | Boolean | false | If true, the mob will stop being targeted if it stops meeting any conditions. |
| sprint_speed_multiplier | Decimal | 1.0 | Multiplier for the running speed. A value of 1.0 means the speed is unchanged |
| walk_speed_multiplier | Decimal | 1.0 | Multiplier for the walking speed. A value of 1.0 means the speed is unchanged |


## minecraft:behavior.work


Allows the NPC to use the POI


| Name | Type | Default Value | Description |
| --- | --- | --- | --- |
| active_time | Integer | 0 | The amount of ticks the NPC will stay in their the work location |
| can_work_in_rain | Boolean | false | If true, this entity can work when their jobsite POI is being rained on. |
| goal_cooldown | Integer | 0 | The amount of ticks the goal will be on cooldown before it can be used again |
| on_arrival | Trigger |  | Event to run when the mob reaches their jobsite. |
| sound_delay_max | Integer | 0 | The max interval in which a sound will play. |
| sound_delay_min | Integer | 0 | The min interval in which a sound will play. |
| speed_multiplier | Decimal | 0.50 | Movement speed multiplier of the mob when using this AI Goal |
| work_in_rain_tolerance | Integer | -1 | If "can_work_in_rain" is false, this is the maximum number of ticks left in the goal where rain will not interrupt the goal |


## minecraft:behavior.work_composter


Allows the NPC to use the composter POI to convert excess seeds into bone meal.


| Name | Type | Default Value | Description |
| --- | --- | --- | --- |
| active_time | Integer | 0 | The amount of ticks the NPC will stay in their the work location |
| block_interaction_max | Integer | 1 | The maximum number of times the mob will interact with the composter. |
| can_empty_composter | Boolean | true | Determines whether the mob can empty a full composter. |
| can_fill_composter | Boolean | true | Determines whether the mob can add items to a composter given that it is not full. |
| can_work_in_rain | Boolean | false | If true, this entity can work when their jobsite POI is being rained on. |
| goal_cooldown | Integer | 0 | The amount of ticks the goal will be on cooldown before it can be used again |
| items_per_use_max | Integer | 20 | The maximum number of items which can be added to the composter per block interaction. |
| min_item_count | Integer | 10 | Limits the amount of each compostable item the mob can use. Any amount held over this number will be composted if possible |
| on_arrival | Trigger |  | Event to run when the mob reaches their jobsite. |
| speed_multiplier | Decimal | 0.50 | Movement speed multiplier of the mob when using this AI Goal |
| use_block_max | Integer | 200 | The maximum interval in which the mob will interact with the composter. |
| use_block_min | Integer | 100 | The minimum interval in which the mob will interact with the composter. |
| work_in_rain_tolerance | Integer | -1 | If "can_work_in_rain" is false, this is the maximum number of ticks left in the goal where rain will not interrupt the goal |


# Attributes


## minecraft:attack


Defines an entity's melee attack and any additional effects on it.


| Name | Type | Default Value | Description |
| --- | --- | --- | --- |
| damage | Range [a, b] |  | Range of the random amount of damage the melee attack deals. A negative value can heal the entity instead of hurting it |
| effect_duration | Decimal | 0.0 | Duration in seconds of the status ailment applied to the damaged entity. Can also be "infinite" |
| effect_name | String |  | Identifier of the status ailment to apply to an entity attacked by this entity's melee attack |


## minecraft:spell_effects


Defines what mob effects to add and remove to the entity when adding this component.


| Name | Type | Default Value | Description |
| --- | --- | --- | --- |
| add_effects | List |  | List of effects to add to this entity after adding this component |
| remove_effects | String |  | List of identifiers of effects to be removed from this entity after adding this component |


> **add_effects** 子参数


| Name | Type | Default Value | Description |
| --- | --- | --- | --- |
| effect | String |  | Effect to add to this entity. Includes 'duration' in seconds, 'amplifier' level, 'ambient' if it is to be considered an ambient effect, and 'visible' if the effect should be visible |


## minecraft:strength


Defines the entity's strength to carry items.


| Name | Type | Default Value | Description |
| --- | --- | --- | --- |
| max | Integer | 5 | The maximum strength of this entity |
| value | Integer | 1 | The initial value of the strength |


# Built-in Events


| Name | Description |
| --- | --- |
| minecraft:entity_born | Event called on an entity that is spawned through two entities breeding. |
| minecraft:entity_spawned | Event called on an entity that is placed in the level. |
| minecraft:entity_transformed | Event called on an entity that transforms into another entity. |
| minecraft:on_prime | Event called on an entity whose fuse is lit and is ready to explode. |


# Components


## minecraft:addrider


Adds a rider to the entity. Requires minecraft:rideable.


| Name | Type | Default Value | Description |
| --- | --- | --- | --- |
| entity_type | String |  | The entity type that will be riding this entity. |
| spawn_event | String |  | The spawn event that will be used when the riding entity is created. |


## minecraft:admire_item


Causes the mob to ignore attackable targets for a given duration.


| Name | Type | Default Value | Description |
| --- | --- | --- | --- |
| cooldown_after_being_attacked | Integer | 0 | Duration, in seconds, for which mob won't admire items if it was hurt |
| duration | Integer | 10 | Duration, in seconds, that the mob is pacified. |


## minecraft:ageable


Adds a timer for the entity to grow up. It can be accelerated by giving the entity the items it likes as defined by feedItems.


| Name | Type | Default Value | Description |
| --- | --- | --- | --- |
| drop_items | List |  | List of items that the entity drops when it grows up. |
| duration | Decimal | 1200.0 | Amount of time before the entity grows up, -1 for always a baby. |
| feed_items | List |  | List of items that can be fed to the entity. Includes 'item' for the item name and 'growth' to define how much time it grows up by. |
| grow_up | String |  | Event to run when this entity grows up. |
| interact_filters | Minecraft Filter |  | List of conditions to meet so that the entity can be fed. |
| transform_to_item | Item Description Properties |  | The feed item used will transform to this item upon successful interaction. Format: itemName:auxValue |


## minecraft:anger_level


Allows this entity to track anger towards a set of nuisances


| Name | Type | Default Value | Description |
| --- | --- | --- | --- |
| anger_decrement_interval | Decimal | 1.00 | Anger level will decay over time. Defines how often anger towards all nuisances will be decreased by one |
| angry_boost | Positive Integer | 20 | Anger boost applied to angry threshold when mob gets angry |
| angry_threshold | Positive Integer | 80 | Threshold that define when the mob is considered angry at a nuisance |
| default_annoyingness | String | 0 | The default amount of annoyingness for any given nuisance. Specifies how much to raise anger level on each provocation |
| max_anger | Positive Integer | 100 | The maximum anger level that can be reached. Applies to any nuisance |
| nuisance_filter | Minecraft Filter |  | Filter that is applied to determine if a mob can be a nuisance |
| on_increase_sounds | Array |  | Sounds to play when the entity is getting provoked. Evaluated in order. First matching condition winscondition A Molang expression describing under which conditions to play this sound, given that the entity was provoked sound The sound to play |
| remove_targets_below_angry_threshold | Boolean | true | Defines if the mob should remove target if it falls below 'angry' threshold |


## minecraft:angry


Defines the entity's 'angry' state using a timer.


| Name | Type | Default Value | Description |
| --- | --- | --- | --- |
| angry_sound | String |  | The sound event to play when the mob is angry |
| broadcast_anger | Boolean | false | If true, other entities of the same entity definition within the broadcastRange will also become angry |
| broadcast_anger_on_attack | Boolean | false | If true, other entities of the same entity definition within the broadcastRange will also become angry whenever this mob attacks |
| broadcast_anger_on_being_attacked | Boolean | false | If true, other entities of the same entity definition within the broadcastRange will also become angry whenever this mob is attacked |
| broadcast_anger_when_dying | Boolean | true | If false, when this mob is killed it does not spread its anger to other entities of the same entity definition within the broadcastRange |
| broadcast_filters | Minecraft Filter |  | Conditions that make this entry in the list valid |
| broadcast_range | Integer | 20 | Distance in blocks within which other entities of the same entity definition will become angry |
| broadcast_targets | List |  | A list of entity families to broadcast anger to |
| calm_event | String |  | Event to run after the number of seconds specified in duration expires (when the entity stops being 'angry') |
| duration | Integer | 25 | The amount of time in seconds that the entity will be angry |
| duration_delta | Integer | 0 | Variance in seconds added to the duration [-delta, delta] |
| filters | Minecraft Filter |  | Filter out mob types that it should not attack while angry (other Piglins) |
| sound_interval | Range [a, b] | 0 | The range of time in seconds to randomly wait before playing the sound again |


## minecraft:annotation.break_door


Allows the actor to break doors assuming that that flags set up for the component to use in navigation


| Name | Type | Default Value | Description |
| --- | --- | --- | --- |
| break_time | Decimal | 12.0 | The time in seconds required to break through doors. |
| min_difficulty | String | hard | The minimum difficulty that the world must be on for this entity to break doors. |


## minecraft:annotation.open_door


Allows the actor to open doors assuming that that flags set up for the component to use in navigation


## minecraft:area_attack


A component that does damage to entities that get within range.


| Name | Type | Default Value | Description |
| --- | --- | --- | --- |
| cause | String |  | The type of damage that is applied to entities that enter the damage range. |
| damage_cooldown | Decimal | 0 | Attack cooldown (in seconds) for how often this entity can attack a target. |
| damage_per_tick | Integer | 2 | How much damage per tick is applied to entities that enter the damage range. |
| damage_range | Decimal | 0.2 | How close a hostile entity must be to have the damage applied. |
| entity_filter | Minecraft Filter |  | The set of entities that are valid to apply the damage to when within range. |
| play_attack_sound | Boolean | true | If the entity should play their attack sound when attacking a target. |


## minecraft:attack_cooldown


Adds a cooldown to a mob. The intention of this cooldown is to be used to prevent the mob from attempting to aquire new attack targets.


| Name | Type | Default Value | Description |
| --- | --- | --- | --- |
| attack_cooldown_complete_event | Trigger |  | Event to be run when the cooldown is complete. |
| attack_cooldown_time | Range [a, b] | [0.00, 1.00] | Amount of time in seconds for the cooldown. Can be specified as a number or a pair of numbers (min and max). |


## minecraft:barter


Enables the component to drop an item as a barter exchange.


| Name | Type | Default Value | Description |
| --- | --- | --- | --- |
| barter_table | String |  | Loot table that's used to drop a random item. |
| cooldown_after_being_attacked | Integer | 0 | Duration, in seconds, for which mob won't barter items if it was hurt |


## minecraft:block_climber


Allows the player to detect and maneuver on the scaffolding block.


## minecraft:block_sensor


Fires off a specified event when a block in the block list is broken within the sensor range.


| Name | Type | Default Value | Description |
| --- | --- | --- | --- |
| on_break | List |  | List of blocks to watch for being broken to fire off a specified event. If a block is in multiple lists, multiple events will fire. |
| sensor_radius | Decimal | 16.00 | The maximum radial distance in which a specified block can be detected. The biggest radius is 32.0. |
| sources | List |  | List of sources that break the block to listen for. If none are specified, all block breaks will be detected. |


## minecraft:boostable


Defines the conditions and behavior of a rideable entity's boost.


| Name | Type | Default Value | Description |
| --- | --- | --- | --- |
| boost_items | List |  | List of items that can be used to boost while riding this entity. Each item has the following properties: |
| duration | Decimal | 3.00 | Time in seconds for the boost. |
| speed_multiplier | Decimal | 1.35 | Factor by which the entity's normal speed increases. E.g. 2.0 means go twice as fast. Requires "format_version" of 1.20 or more, otherwise the value 1.35 will be used. |


> **boost_items** 子参数


| Name | Type | Default Value | Description |
| --- | --- | --- | --- |
| damage | Integer | 1 | This is the damage that the item will take each time it is used. |
| item | String |  | Name of the item that can be used to boost. |
| replace_item | String |  | The item used to boost will become this item once it is used up. |


## minecraft:boss


The current state of the boss for updating the boss HUD.


| Name | Type | Default Value | Description |
| --- | --- | --- | --- |
| hud_range | Integer | 55 | The max distance from the boss at which the boss's health bar is present on the players screen. |
| name | String |  | The name that will be displayed above the boss's health bar. |
| should_darken_sky | Boolean | false | Whether the sky should darken in the presence of the boss. |


## minecraft:break_blocks


Specifies the blocks that this entity can break as it moves around.


| Name | Type | Default Value | Description |
| --- | --- | --- | --- |
| breakable_blocks | List |  | A list of the blocks that can be broken as this entity moves around |


## minecraft:breathable


Defines what blocks this entity can breathe in and gives them the ability to suffocate.


| Name | Type | Default Value | Description |
| --- | --- | --- | --- |
| breathe_blocks | List |  | List of blocks this entity can breathe in, in addition to the other "breathes" parameters. |
| breathes_air | Boolean | true | If true, this entity can breathe in air. |
| breathes_lava | Boolean | true | If true, this entity can breathe in lava. |
| breathes_solids | Boolean | false | If true, this entity can breathe in solid blocks. |
| breathes_water | Boolean | false | If true, this entity can breathe in water. |
| generates_bubbles | Boolean | true | If true, this entity will have visible bubbles while in water. |
| inhale_time | Decimal | 0 | Time in seconds to recover breath to maximum. |
| non_breathe_blocks | List |  | List of blocks this entity can't breathe in, in addition to the other "breathes" parameters. |
| suffocate_time | Integer | -20 | Time in seconds between suffocation damage. |
| total_supply | Integer | 15 | Time in seconds the entity can hold its breath. |


## minecraft:breedable


Defines the way an entity can get into the 'love' state.


| Name | Type | Default Value | Description |
| --- | --- | --- | --- |
| allow_sitting | Boolean | false | If true, entities can breed while sitting |
| blend_attributes | Boolean | true | If true, the entities will blend their attributes in the offspring after they breed. |
| breed_cooldown | Decimal | 60 | Time in seconds before the Entity can breed again. |
| breed_items | List |  | The list of items that can be used to get the entity into the 'love' state |
| breeds_with | List |  | The list of entity definitions that this entity can breed with. |
| causes_pregnancy | Boolean | false | If true, the entity will become pregnant instead of spawning a baby. |
| deny_parents_variant | JSON Object |  | Determines how likely the baby of parents with the same variant will deny that variant and take a random variant within the given range instead. |
| environment_requirements | List |  | The list of nearby block requirements to get the entity into the 'love' state.blocks The block types required nearby for the entity to breed. count The number of the required block types nearby for the entity to breed. radius How many blocks radius from the mob's center to search in for the required blocks. Bounded between 0 and 16. |
| extra_baby_chance | Decimal | 0 | Chance that up to 16 babies will spawn between 0.0 and 1.0, where 1.0 is 100%. |
| inherit_tamed | Boolean | true | If true, the babies will be automatically tamed if its parents are |
| love_filters | Minecraft Filter |  | The filters to run when attempting to fall in love. |
| mutation_factor | JSON Object |  | Determines how likely the babies are to NOT inherit one of their parent's variances. Values are between 0.0 and 1.0, with a higher number meaning more likely to mutate. |
| mutation_strategy | String | none | Strategy used for mutating variants and extra variants for offspring. Current valid alternatives are 'random' and 'none'. |
| parent_centric_attribute_blending | List |  | [EXPERIMENTAL] List of attributes that should benefit from parent centric attribute blending. For example, horses blend their health, movement, and jump_strength in their offspring. |
| property_inheritance | List |  | List of Entity Properties that should be inherited from the parent entities and potentially mutated. |
| random_extra_variant_mutation_interval | Range [a, b] | 0 | Range used to determine random extra variant. |
| random_variant_mutation_interval | Range [a, b] | 0 | Range used to determine random variant. |
| require_full_health | Boolean | false | If true, the entity needs to be at full health before it can breed. |
| require_tame | Boolean | true | If true, the entities need to be tamed first before they can breed. |
| transform_to_item | String |  | The breed item used will transform to this item upon successful interaction. Format: itemName:auxValue |


> **breeds_with** 子参数


| Name | Type | Default Value | Description |
| --- | --- | --- | --- |
| baby_type | String |  | The entity definition of this entity's babies. |
| breed_event | String |  | Event to run when this entity breeds. |
| mate_type | String |  | The entity definition of this entity's mate. |


> **deny_parents_variant** 子参数


| Name | Type | Default Value | Description |
| --- | --- | --- | --- |
| chance | Decimal | 0 | The percentage chance of denying the parents' variant. |
| max_variant | Integer | 0 | The inclusive maximum of the variant range. |
| min_variant | Integer | 0 | The inclusive minimum of the variant range. |


> **mutation_factor** 子参数


| Name | Type | Default Value | Description |
| --- | --- | --- | --- |
| color | Decimal | 0 | The percentage chance of the offspring getting its color as if spawned rather than inheriting color from its parents. |
| extra_variant | Decimal | 0 | The percentage chance of a mutation on the entity's extra variant type. |
| variant | Decimal | 0 | The percentage chance of a mutation on the entity's variant type. |


## minecraft:bribeable


Defines the way an entity can get into the 'bribed' state.


| Name | Type | Default Value | Description |
| --- | --- | --- | --- |
| bribe_cooldown | Decimal | 2.0 | Time in seconds before the Entity can be bribed again. |
| bribe_items | List |  | The list of items that can be used to bribe the entity. |


## minecraft:buoyant


Enables an entity to float on the specified liquid blocks.


| Name | Type | Default Value | Description |
| --- | --- | --- | --- |
| apply_gravity | Boolean | true | Applies gravity each tick. Causes more of a wave simulation, but will cause more gravity to be applied outside liquids. |
| base_buoyancy | Decimal | 1.0 | Base buoyancy used to calculate how much will a mob float. |
| big_wave_probability | Decimal | 0.03 | Probability for a big wave hitting the entity. Only used if `simulate_waves` is true. |
| big_wave_speed | Decimal | 10.0 | Multiplier for the speed to make a big wave. Triggered depending on 'big_wave_probability'. |
| drag_down_on_buoyancy_removed | Decimal | 0.0 | How much an actor will be dragged down when the Buoyancy Component is removed. |
| liquid_blocks | List |  | List of blocks this entity can float on. Must be a liquid block. |
| simulate_waves | Boolean | true | Should the movement simulate waves going through. |


## minecraft:burns_in_daylight


Specifies if a mob burns in daylight.


## minecraft:can_join_raid


Determines that this entity can join an existing raid.


## minecraft:celebrate_hunt


Specifies hunt celebration behaviour.


| Name | Type | Default Value | Description |
| --- | --- | --- | --- |
| broadcast | Boolean | true | If true, celebration will be broadcasted to other entities in the radius. |
| celeberation_targets | Minecraft Filter |  | The list of conditions that target of hunt must satisfy to initiate celebration. |
| celebrate_sound | String |  | The sound event to play when the mob is celebrating |
| duration | Integer | 4 | Duration, in seconds, of celebration |
| radius | Decimal | 16 | If broadcast is enabled, specifies the radius in which it will notify other entities for celebration. |
| sound_interval | Range [a, b] | 0 | The range of time in seconds to randomly wait before playing the sound again |


## minecraft:collision_box


Sets the width and height of the Entity's collision box.


| Name | Type | Default Value | Description |
| --- | --- | --- | --- |
| height | Decimal | 1.0 | Height of the collision box in blocks. A negative value will be assumed to be 0. Min value is -100000000.000000 Max value is 100000000.000000 |
| width | Decimal | 1.0 | Width of the collision box in blocks. A negative value will be assumed to be 0. Min value is -100000000.000000 Max value is 100000000.000000 |


## minecraft:combat_regeneration


Gives Regeneration I and removes Mining Fatigue from the mob that kills the Actor's attack target.


| Name | Type | Default Value | Description |
| --- | --- | --- | --- |
| apply_to_family | Boolean | false | Determines if the mob will grant mobs of the same type combat buffs if they kill the target. |
| apply_to_self | Boolean | false | Determines if the mob will grant itself the combat buffs if it kills the target. |
| regeneration_duration | Integer | 5 | The duration in seconds of Regeneration I added to the mob. Can also be set to "infinite" |


## minecraft:conditional_bandwidth_optimization


Defines the Conditional Spatial Update Bandwidth Optimizations of this entity.


| Name | Type | Default Value | Description |
| --- | --- | --- | --- |
| conditional_values | List |  | The object containing the conditional bandwidth optimization values.conditional_values Conditions that must be met for these optimization values to be used. max_dropped_ticks In relation to the optimization value, determines the maximum ticks spatial update packets can be not sent. max_optimized_distance The maximum distance considered during bandwidth optimizations. Any value below the max is interpolated to find optimization, and any value greater than or equal to this max results in max optimization. use_motion_prediction_hints When set to true, smaller motion packets will be sent during drop packet intervals, resulting in the same amount of packets being sent as without optimizations but with much less data being sent. This should be used when actors are travelling very quickly or teleporting to prevent visual oddities. |
| default_values | JSON Object |  | The object containing the default bandwidth optimization values.max_dropped_ticks In relation to the optimization value, determines the maximum ticks spatial update packets can be not sent. max_optimized_distance The maximum distance considered during bandwidth optimizations. Any value below the max is interpolated to find optimization, and any value greater than or equal to this max results in max optimization. use_motion_prediction_hints When set to true, smaller motion packets will be sent during drop packet intervals, resulting in the same amount of packets being sent as without optimizations but with much less data being sent. This should be used when actors are travelling very quickly or teleporting to prevent visual oddities. |


## minecraft:custom_hit_test


List of hitboxes for melee and ranged hits against the entity.


| Name | Type | Default Value | Description |
| --- | --- | --- | --- |
| hitboxes | Array |  | Comma seperated list of hitboxes. |


## minecraft:damage_over_time


Applies defined amount of damage to the entity at specified intervals.


| Name | Type | Default Value | Description |
| --- | --- | --- | --- |
| damage_per_hurt | Integer | 1 | Amount of damage caused each hurt. |
| time_between_hurt | Decimal | 0.0 | Time in seconds between damage. |


## minecraft:damage_sensor


"Defines what events to call when this entity is damaged by specific entities or items.


| Name | Type | Default Value | Description |
| --- | --- | --- | --- |
| triggers | List |  | List of triggers with the events to call when taking specific kinds of damage." |


> **triggers** 子参数


| Name | Type | Default Value | Description |
| --- | --- | --- | --- |
| cause | String | none | Type of damage that triggers the events. |
| damage_modifier | Decimal | 0.00 | A modifier that adds/removes to the base damage received from the specified damage cause. It does not reduce damage to less than 0. |
| damage_multiplier | Decimal | 1.00 | A multiplier that modifies the base damage received from the specified damage cause. If "deals_damage" is true the multiplier can only reduce the damage the entity will take to a minimum of 1. |
| deals_damage | Boolean | yes | Defines how received damage affects the entity: \n- "yes", received damage is applied to the entity. \n- "no", received damage is not applied to the entity. \n- "no_but_side_effects_apply", received damage is not applied to the entity, but the side effects of the attack are. This means that the attacker's weapon loses durability, enchantment side effects are applied, and so on. |
| on_damage | JSON Object |  | Defines which entities the trigger applies to, and which, if any, event to emit when damaged. |
| on_damage_sound_event | String |  | Defines what sound to play, if any, when the "on_damage" filters are met. |


## minecraft:dash_action


Ability for a rideable entity to dash.


| Name | Type | Default Value | Description |
| --- | --- | --- | --- |
| cooldown_time | Decimal | 1.00 | The dash cooldown in seconds. Default value is 1.000000. |
| direction | String | entity | Should the momentum be applied in the direction of the 'entity' or 'passenger'. When 'entity' is used the momentum is applied horizontally according to the direction the entity is looking, using only the entity's yaw. When 'passenger' is used the momentum will be applied in the direction the controlling passenger is looking, using the passenger's pitch and yaw. |
| horizontal_momentum | Decimal | 1.00 | Horizontal momentum of the dash. |
| vertical_momentum | Decimal | 1.00 | Vertical momentum of the dash. |


## minecraft:despawn


Despawns the Actor when the despawn rules or optional filters evaluate to true.


| Name | Type | Default Value | Description |
| --- | --- | --- | --- |
| despawn_from_chance | Boolean | true | Determines if "min_range_random_chance" is used in the standard despawn rules |
| despawn_from_distance | JSON Object |  | Specifies if the "min_distance" and "max_distance" are used in the standard despawn rules. |
| despawn_from_inactivity | Boolean | true | Determines if the "min_range_inactivity_timer" is used in the standard despawn rules. |
| despawn_from_simulation_edge | Boolean | true | Determines if the mob is instantly despawned at the edge of simulation distance in the standard despawn rules. |
| filters | Minecraft Filter |  | The list of conditions that must be satisfied before the Actor is despawned. If a filter is defined then standard despawn rules are ignored. |
| min_range_inactivity_timer | Integer | 30 | The amount of time in seconds that the mob must be inactive. |
| min_range_random_chance | Integer | 800 | A random chance between 1 and the given value. |
| remove_child_entities | Boolean | false | If true, all entities linked to this entity in a child relationship (eg. leashed) will also be despawned. |


> **despawn_from_distance** 子参数


| Name | Type | Default Value | Description |
| --- | --- | --- | --- |
| max_distance | Integer | 128 | maximum distance for standard despawn rules to instantly despawn the mob. |
| min_distance | Integer | 32 | minimum distance for standard despawn rules to try to despawn the mob. |


## minecraft:dimension_bound


Prevents the entity from changing dimension through portals.


## minecraft:drying_out_timer


Adds a timer for drying out that will count down and fire 'dried_out_event' or will stop as soon as the entity will get under rain or water and fire 'stopped_drying_out_event'


| Name | Type | Default Value | Description |
| --- | --- | --- | --- |
| dried_out_event | String |  | Event to fire when the drying out time runs out. |
| recover_after_dried_out_event | String |  | Event to fire when entity was already dried out but received increase in water supply. |
| stopped_drying_out_event | String |  | Event to fire when entity stopped drying out, for example got into water or under rain. |
| total_time | Decimal | 0.0 | Amount of time in seconds to dry out fully. |
| water_bottle_refill_time | Decimal | 0.0 | Optional amount of additional time in seconds given by using splash water bottle on entity. |


## minecraft:economy_trade_table


Defines this entity's ability to trade with players.


| Name | Type | Default Value | Description |
| --- | --- | --- | --- |
| convert_trades_economy | Boolean | false | Determines when the mob transforms, if the trades should be converted when the new mob has a economy_trade_table. When the trades are converted, the mob will generate a new trade list with their new trade table, but then it will try to convert any of the same trades over to have the same enchantments and user data. For example, if the original has a Emerald to Enchanted Iron Sword (Sharpness 1), and the new trade also has an Emerald for Enchanted Iron Sword, then the enchantment will be Sharpness 1. |
| cured_discount | Range [a, b] | [-25, -20] | How much should the discount be modified by when the player has cured the Zombie Villager. Can be specified as a pair of numbers (When use_legacy_price_formula is true this is the low-tier trade discount and high-tier trade discount, otherwise it is the minor positive gossip and major positive gossip.) |
| display_name | String |  | Name to be displayed while trading with this entity |
| hero_demand_discount | Integer | -4 | Used in legacy prices to determine how much should Demand be modified by when the player has the Hero of the Village mob effect |
| max_cured_discount | Range [a, b] | [-25, -20] | The max the discount can be modified by when the player has cured the Zombie Villager. Can be specified as a pair of numbers (When use_legacy_price_formula is true this is the low-tier trade discount and high-tier trade discount, otherwise it is the minor positive gossip and major positive gossip.) |
| max_nearby_cured_discount | Integer | -200 | The max the discount can be modified by when the player has cured a nearby Zombie Villager. Only used when use_legacy_price_formula is true, otherwise max_cured_discount (low) is used. |
| nearby_cured_discount | Integer | -20 | How much should the discount be modified by when the player has cured a nearby Zombie Villager |
| new_screen | Boolean | false | Used to determine if trading with entity opens the new trade screen |
| persist_trades | Boolean | false | Determines if the trades should persist when the mob transforms. This makes it so that the next time the mob is transformed to something with a trade_table or economy_trade_table, then it keeps their trades. |
| show_trade_screen | Boolean | true | Show an in game trade screen when interacting with the mob. |
| table | String |  | File path relative to the resource pack root for this entity's trades |
| use_legacy_price_formula | Boolean | false | Determines whether the legacy formula is used to determines the trade prices. |


## minecraft:entity_armor_equipment_slot_mapping


It defines to which armor slot an item equipped to 'minecraft:equippable''s second slot should be equipped to. It is automatically applied to all entities for worlds with a version greater than or equal to 1.21.10. For older worlds, 'slot.armor.torso' will be used. It is strongly advised not to explicitly use this component, as no backwards compatibility for it will be provided.


| Name | Type | Default Value | Description |
| --- | --- | --- | --- |
| armor_slot | String |  | The armor slot an item equipped to 'minecraft:equippable''s second slot should be equipped to. It defaults to 'slot.armor.torso' for entities with a format version prior to 1.21.10, and to 'slot.armor.body' otherwise. |


## minecraft:entity_sensor


A component that owns multiple subsensors, each one firing an event when a set of conditions are met by other entities within the defined range.


| Name | Type | Default Value | Description |
| --- | --- | --- | --- |
| find_players_only | Boolean | false | Limits the search to Players only for all subsensors. |
| relative_range | Boolean | true | If true the subsensors' range is additive on top of the entity's size. |
| subsensors | List |  | The list of subsensors which sense for entities and emit events when all their conditions are met. |


> **subsensors** 子参数


| Name | Type | Default Value | Description |
| --- | --- | --- | --- |
| cooldown | Decimal | -1 | How many seconds should elapse before the subsensor can once again sense for entities. The cooldown is applied on top of the base 1 tick (0.05 seconds) delay. Negative values will result in no cooldown being used. |
| event | String |  | Event to fire when the conditions are met. |
| event_filters | Minecraft Filter |  | The set of conditions that must be satisfied to fire the event. |
| maximum_count | Integer | -1 | The maximum number of entities that must pass the filter conditions for the event to send. |
| minimum_count | Integer | 1 | The minimum number of entities that must pass the filter conditions for the event to send. |
| range | Vector [a, b] | [10, 10] | The maximum horizontal and vertical distance another entity can be from this and have the filters checked against it. |
| require_all | Boolean | false | If true requires all nearby entities to pass the filter conditions for the events to send. |
| y_offset | Decimal | 0 | Vertical offset applied to the entity's position when computing the distance from other entities. |


## minecraft:environment_sensor


Creates a trigger based on environment conditions.


| Name | Type | Default Value | Description |
| --- | --- | --- | --- |
| triggers | List |  | The list of triggers that fire when the environment conditions match the given filter criteria. |


## minecraft:equip_item


The entity puts on the desired equipment.


| Name | Type | Default Value | Description |
| --- | --- | --- | --- |
| excluded_items | List |  | List of items that the entity should not equip. |


## minecraft:equippable


Defines an entity's behavior for having items equipped to it.


| Name | Type | Default Value | Description |
| --- | --- | --- | --- |
| slots | List |  | List of slots and the item that can be equipped. |


> **slots** 子参数


| Name | Type | Default Value | Description |
| --- | --- | --- | --- |
| accepted_items | List |  | The list of items that can go in this slot. |
| interact_text | String |  | Text to be displayed when the entity can be equipped with this item when playing with Touch-screen controls. |
| item | String |  | Identifier of the item that can be equipped for this slot. |
| on_equip | String |  | Event to trigger when this entity is equipped with this item. |
| on_unequip | String |  | Event to trigger when this item is removed from this entity. |
| slot | Integer | 0 | The slot number of this slot. |


## minecraft:exhaustion_values


Defines how much exhaustion each player action should take.


| Name | Type | Default Value | Description |
| --- | --- | --- | --- |
| attack | Decimal | 0.1 | Amount of exhaustion applied when attacking. |
| damage | Decimal | 0.1 | Amount of exhaustion applied when taking damage. |
| heal | Decimal | 6 | Amount of exhaustion applied when healed through food regeneration. |
| jump | Decimal | 0.05 | Amount of exhaustion applied when jumping. |
| mine | Decimal | 0.005 | Amount of exhaustion applied when mining. |
| sprint | Decimal | 0.01 | Amount of exhaustion applied when sprinting. |
| sprint_jump | Decimal | 0.2 | Amount of exhaustion applied when sprint jumping. |
| swim | Decimal | 0.01 | Amount of exhaustion applied when swimming. |
| walk | Decimal | 0 | Amount of exhaustion applied when walking. |


## minecraft:experience_reward


.


| Name | Type | Default Value | Description |
| --- | --- | --- | --- |
| on_bred | Molang | 0 | A Molang expression defining the amount of experience rewarded when this entity is successfully bred. An array of expressions adds each expression's result together for a final total. |
| on_death | Molang | 0 | A Molang expression defining the amount of experience rewarded when this entity dies. An array of expressions adds each expression's result together for a final total. |


## minecraft:explode


Defines how the entity explodes.


| Name | Type | Default Value | Description |
| --- | --- | --- | --- |
| allow_underwater | Boolean | false | If true, the explosion will affect blocks and entities under water. |
| breaks_blocks | Boolean | true | If true, the explosion will destroy blocks in the explosion radius. |
| causes_fire | Boolean | false | If true, blocks in the explosion radius will be set on fire. |
| damage_scaling | Decimal | 1.000000 | A scale factor applied to the explosion's damage to entities. A value of 0 prevents the explosion from dealing any damage. Negative values cause the explosion to heal entities instead. |
| destroy_affected_by_griefing | Boolean | false | If true, whether the explosion breaks blocks is affected by the mob griefing game rule. |
| fire_affected_by_griefing | Boolean | false | If true, whether the explosion causes fire is affected by the mob griefing game rule. |
| fuse_length | Range [a, b] | [0.0, 0.0] | The range for the random amount of time the fuse will be lit before exploding, a negative value means the explosion will be immediate. |
| fuse_lit | Boolean | false | If true, the fuse is already lit when this component is added to the entity. |
| knockback_scaling | Decimal | 1.000000 | A scale factor applied to the knockback force caused by the explosion. |
| max_resistance | Decimal | 3.40282e+38 | A blocks explosion resistance will be capped at this value when an explosion occurs. |
| negates_fall_damage | Boolean | false | Defines whether the explosion should apply fall damage negation to Players above the point of collision. |
| particle_effect | String | explosion | The name of the particle effect to use. The accepted strings are 'explosion', 'wind_burst', or 'breeze_wind_burst'. |
| power | Decimal | 3 | The radius of the explosion in blocks and the amount of damage the explosion deals. |
| sound_effect | String | explode | The name of the sound effect played when the explosion triggers. |
| toggles_blocks | Boolean | false | If true, the explosion will toggle blocks in the explosion radius. |


## minecraft:flocking


Allows entities to flock in groups in water or not.


| Name | Type | Default Value | Description |
| --- | --- | --- | --- |
| block_distance | Decimal | 0 | The amount of blocks away the entity will look at to push away from. |
| block_weight | Decimal | 0 | The weight of the push back away from blocks. |
| breach_influence | Decimal | 0 | The amount of push back given to a flocker that breaches out of the water. |
| cohesion_threshold | Decimal | 1 | The threshold in which to start applying cohesion. |
| cohesion_weight | Decimal | 1 | The weight applied for the cohesion steering of the flock. |
| goal_weight | Decimal | 0 | The weight on which to apply on the goal output. |
| high_flock_limit | Integer | 0 | Determines the high bound amount of entities that can be allowed in the flock. |
| in_water | Boolean | false | Tells the Flocking Component if the entity exists in water. |
| influence_radius | Decimal | 0 | The area around the entity that allows others to be added to the flock. |
| innner_cohesion_threshold | Decimal | 0 | The distance in which the flocker will stop applying cohesion. |
| loner_chance | Decimal | 0 | The percentage chance between 0-1 that a fish will spawn and not want to join flocks. Invalid values will be capped at the end points. |
| low_flock_limit | Integer | 0 | Determines the low bound amount of entities that can be allowed in the flock. |
| match_variants | Boolean | false | Tells the flockers that they can only match similar entities that also match the variant, mark variants, and color data of the other potential flockers. |
| max_height | Decimal | 0 | The max height allowable in the air or water. |
| min_height | Decimal | 0 | The min height allowable in the air or water. |
| separation_threshold | Decimal | 2 | The distance that is determined to be to close to another flocking and to start applying separation. |
| separation_weight | Decimal | 1 | The weight applied to the separation of the flock. |
| use_center_of_mass | Boolean | false | Tells the flockers that they will follow flocks based on the center of mass. |


## minecraft:free_camera_controlled


When configured as a rideable entity, the entity will be controlled using WASD controls and mouse to move in three dimensions.


| Name | Type | Default Value | Description |
| --- | --- | --- | --- |
| backwards_movement_modifier | Decimal | 0.5 | Modifies speed going backwards. |
| strafe_speed_modifier | Decimal | 0.4 | Modifies the strafe speed. |


## minecraft:game_event_movement_tracking


Allows an entity to emit `entityMove`, `swim` and `flap` game events, depending on the block the entity is moving through. It is added by default to every mob. Add it again to override its behavior.


| Name | Type | Default Value | Description |
| --- | --- | --- | --- |
| emit_flap | Boolean | false | If true, the `flap` game event will be emitted when the entity moves through air. |
| emit_move | Boolean | true | If true, the `entityMove` game event will be emitted when the entity moves on ground or through a solid. |
| emit_swim | Boolean | true | If true, the `swim` game event will be emitted when the entity moves through a liquid. |


## minecraft:genetics


Defines the way a mob's genes and alleles are passed on to its offspring, and how those traits manifest in the child. Compatible parent genes are crossed together, the alleles are handed down from the parents to the child, and any matching genetic variants fire off JSON events to modify the child and express the traits.


| Name | Type | Default Value | Description |
| --- | --- | --- | --- |
| genes | List |  | The list of genes that this entity has and will cross with a partner during breeding. |
| mutation_rate | Decimal | 0.03125 | Chance that an allele will be replaced with a random one instead of the parent's allele during birth. |


> **genes** 子参数


| Name | Type | Default Value | Description |
| --- | --- | --- | --- |
| allele_range | Integer |  | The range of positive integer allele values for this gene. Spawned mobs will have a random number in this range assigned to them. |
| genetic_variants | List |  | The list of genetic variants for this gene. These check for particular allele combinations and fire events when all of them are satisfied. |
| mutation_rate | Decimal | -1 | If this value is non-negative, overrides the chance for this gene that an allele will be replaced with a random one instead of the parent's allele during birth. Non-negative values greater than `1` will be the same as the value `1`. |
| name | String |  | The name of the gene. |


> **allele_range** 子参数


| Name | Type | Default Value | Description |
| --- | --- | --- | --- |
| range_max | Integer | 0 | Upper bound of the allele values for this gene. |
| range_min | Integer | 0 | Lower bound of the allele values for this gene. |


> **genetic_variants** 子参数


| Name | Type | Default Value | Description |
| --- | --- | --- | --- |
| birth_event | String |  | Event to run when this mob is created and matches the allele conditions. |
| both_allele | Integer | -1 | If this value is non-negative, compare both the mob's main and hidden alleles with this value for a match with both. Can also be a range of integers. |
| either_allele | Integer | -1 | If this value is non-negative, compare both the mob's main and hidden alleles with this value for a match with either. Can also be a range of integers. |
| hidden_allele | Integer | -1 | If this value is non-negative, compare the mob's hidden allele with this value for a match. Can also be a range of integers. |
| main_allele | Integer | -1 | If this value is non-negative, compare the mob's main allele with this value for a match. Can also be a range of integers. |


## minecraft:giveable


Defines sets of items that can be used to trigger events when used on this entity. The item will also be taken and placed in the entity's inventory.


| Name | Type | Default Value | Description |
| --- | --- | --- | --- |
| cooldown | Decimal | 0.0 | An optional cool down in seconds to prevent spamming interactions. |
| items | List |  | The list of items that can be given to the entity to place in their inventory. |
| on_give | String |  | Event to fire when the correct item is given. |


## minecraft:group_size


Keeps track of entity group size in the given radius.


| Name | Type | Default Value | Description |
| --- | --- | --- | --- |
| filters | Minecraft Filter |  | The list of conditions that must be satisfied for other entities to be counted towards group size. |
| radius | Decimal | 16 | Radius from center of entity. |


## minecraft:grows_crop


Could increase crop growth when entity walks over crop


| Name | Type | Default Value | Description |
| --- | --- | --- | --- |
| chance | Decimal | 0 | Value between 0-1. Chance of success per tick. |
| charges | Integer | 10 | Number of charges |


## minecraft:healable


Defines the interactions with this entity for healing it.


| Name | Type | Default Value | Description |
| --- | --- | --- | --- |
| filters | Minecraft Filter |  | The filter group that defines the conditions for using this item to heal the entity. |
| force_use | Boolean | false | Determines if item can be used regardless of entity being at full health. |
| items | Array |  | The array of items that can be used to heal this entity. |


> **items** 子参数


| Name | Type | Default Value | Description |
| --- | --- | --- | --- |
| heal_amount | Integer | 1 | The amount of health this entity gains when fed this item. |
| item | String |  | Item identifier that can be used to heal this entity. |


## minecraft:heartbeat


Defines the entity's heartbeat.


| Name | Type | Default Value | Description |
| --- | --- | --- | --- |
| interval | Molang | 1.00 | A Molang expression defining the inter-beat interval in seconds. A value of zero or less means no heartbeat. |
| sound_event | String | heartbeat | Level sound event to be played as the heartbeat sound. |


## minecraft:home


Saves a home position for when the the entity is spawned.


| Name | Type | Default Value | Description |
| --- | --- | --- | --- |
| home_block_list | List |  | Optional list of blocks that can be considered a valid home. If no such block longer exists at that position, the home restriction is removed. Example syntax: minecraft:sand. Not supported: minecraft:sand:1. |
| restriction_radius | Integer | 0 | Optional radius that the entity will be restricted to in relation to its home. |
| restriction_type | String | none | Defines how the the entity will be restricted to its home position. The possible values are: \n- "none", which poses no restriction. \n- "random_movement", which restricts randomized movement to be around the home position. \n- "all_movement", which restricts any kind of movement to be around the home position. However, entities that somehow got too far away from their home will always be able to move closer to it, if prompted to do so. |


## minecraft:hurt_on_condition


Defines a set of conditions under which an entity should take damage.


| Name | Type | Default Value | Description |
| --- | --- | --- | --- |
| damage_conditions | Array |  | List of damage conditions that when met can cause damage to the entity.cause The kind of damage that is caused to the entity. Various armors and spells use this to determine if the entity is immune. Valid damage causes. An invalid value will result in the default cause of "none" being selected. override contact entity_attack projectile suffocation fall fire fire_tick lava drowning block_explosion entity_explosion void SelfDestruct magic wither starve anvil thorns falling_block piston magma fireworks charging temperature all none damage_per_tick The amount of damage done each tick that the conditions are met. filters The set of conditions that must be satisfied before the entity takes the defined damage. |


## minecraft:inside_block_notifier


Verifies whether the entity is inside any of the listed blocks.


| Name | Type | Default Value | Description |
| --- | --- | --- | --- |
| block_list | List |  | List of blocks, with certain block states, that we are monitoring to see if the entity is inside. |


## minecraft:insomnia


Adds a timer since last rested to see if phantoms should spawn.


| Name | Type | Default Value | Description |
| --- | --- | --- | --- |
| days_until_insomnia | Decimal | 3.0 | Number of days the mob has to stay up until the insomnia effect begins. |


## minecraft:instant_despawn


Despawns the Actor immediately.


| Name | Type | Default Value | Description |
| --- | --- | --- | --- |
| remove_child_entities | Boolean | false | If true, all entities linked to this entity in a child relationship (eg. leashed) will also be despawned. |


## minecraft:interact


Defines interactions with this entity.


| Name | Type | Default Value | Description |
| --- | --- | --- | --- |
| add_items | JSON Object |  | Loot table with items to add to the player's inventory upon successful interaction. |
| cooldown | Decimal | 0 | Time in seconds before this entity can be interacted with again. |
| cooldown_after_being_attacked | Decimal | 0 | Time in seconds before this entity can be interacted with after being attacked. |
| drop_item_slot | String |  | The entity's slot to remove and drop the item from, if any, upon successful interaction. Inventory slots are denoted by positive numbers. Equipment slots are denoted by 'slot.weapon.mainhand', 'slot.weapon.offhand', 'slot.armor.head', 'slot.armor.chest', 'slot.armor.legs', 'slot.armor.feet' and 'slot.armor.body'. |
| drop_item_y_offset | Decimal | 0 | Will offset the item drop position this amount in the y direction. Requires "drop_item_slot" to be specified. |
| equip_item_slot | String |  | The entity's slot to equip the item to, if any, upon successful interaction. Inventory slots are denoted by positive numbers. Equipment slots are denoted by 'slot.weapon.mainhand', 'slot.weapon.offhand', 'slot.armor.head', 'slot.armor.chest', 'slot.armor.legs', 'slot.armor.feet' and 'slot.armor.body'. |
| health_amount | Integer | 0 | The amount of health this entity will recover or lose when interacting with this item. Negative values will harm the entity. |
| hurt_item | Integer | 0 | The amount of damage the item will take when used to interact with this entity. A value of 0 means the item won't lose durability. |
| interact_text | String |  | Text to show when the player is able to interact in this way with this entity when playing with touch-screen controls. |
| on_interact | String |  | Event to fire when the interaction occurs. |
| particle_on_start | JSON Object |  | Particle effect that will be triggered at the start of the interaction.particle_offset_towards_interactor Whether or not the particle will appear closer to who performed the interaction. particle_type The type of particle that will be spawned. particle_y_offset Will offset the particle this amount in the y direction. |
| play_sounds | String |  | List of sounds to play when the interaction occurs. |
| repair_entity_item | JSON Object |  | Allows to repair one of the entity's items. |
| spawn_entities | String |  | List of entities to spawn when the interaction occurs. |
| spawn_items | JSON Object |  | Loot table with items to drop on the ground upon successful interaction. |
| swing | Boolean | false | If true, the player will do the 'swing' animation when interacting with this entity. |
| transform_to_item | String |  | The item used will transform to this item upon successful interaction. Format: itemName:auxValue |
| use_item | Boolean | false | If true, the interaction will use an item. |
| vibration | String | entity_interact | Vibration to emit when the interaction occurs. Admitted values are none (no vibration emitted), shear, entity_die, entity_act, entity_interact. |


> **add_items** 子参数


| Name | Type | Default Value | Description |
| --- | --- | --- | --- |
| table | String |  | File path, relative to the Behavior Pack's path, to the loot table file. |


> **repair_entity_item** 子参数


| Name | Type | Default Value | Description |
| --- | --- | --- | --- |
| amount | Integer |  | How much of the item durability should be restored upon interaction. |
| slot | Integer |  | The entity's slot containing the item to be repaired. Inventory slots are denoted by positive numbers. Equipment slots are denoted by 'slot.weapon.mainhand', 'slot.weapon.offhand', 'slot.armor.head', 'slot.armor.chest', 'slot.armor.legs', 'slot.armor.feet' and 'slot.armor.body'. |


> **spawn_items** 子参数


| Name | Type | Default Value | Description |
| --- | --- | --- | --- |
| table | String |  | File path, relative to the Behavior Pack's path, to the loot table file. |
| y_offset | Decimal | 1 | Will offset the items spawn position this amount in the y direction. |


## minecraft:inventory


Defines this entity's inventory properties.


| Name | Type | Default Value | Description |
| --- | --- | --- | --- |
| additional_slots_per_strength | Integer | 0 | Number of slots that this entity can gain per extra strength |
| can_be_siphoned_from | Boolean | false | If true, the contents of this inventory can be removed by a hopper |
| container_type | String | none | Type of container this entity has. Can be horse, minecart_chest, chest_boat, minecart_hopper, inventory, container or hopper |
| inventory_size | Integer | 5 | Number of slots the container has |
| private | Boolean | false | If true, the entity will not drop its inventory on death |
| restrict_to_owner | Boolean | false | If true, the entity's inventory can only be accessed by its owner or itself |


## minecraft:item_hopper


Determines that this entity is an item hopper.


## minecraft:jump.dynamic


Defines a dynamic type jump control that will change jump properties based on the speed modifier of the mob.


## minecraft:jump.static


Gives the entity the ability to jump.


| Name | Type | Default Value | Description |
| --- | --- | --- | --- |
| jump_power | Decimal | 0.42 | The initial vertical velocity for the jump |


## minecraft:leashable


Allows this entity to be leashed and defines the conditions and events for this entity when is leashed.


| Name | Type | Default Value | Description |
| --- | --- | --- | --- |
| can_be_cut | Boolean | true | If true, players can cut both incoming and outgoing leashes by using shears on the entity. |
| can_be_stolen | Boolean | true | If true, players can leash this entity even if it is already leashed to another entity. |
| on_leash | String |  | Event to call when this entity is leashed. |
| on_unleash | String |  | Event to call when this entity is unleashed. |
| on_unleash_interact_only | Boolean | false | When set to true, "on_unleash" does not trigger when the entity gets unleashed for reasons other than the player directly interacting with it. |
| presets | List |  | Defines how this entity behaves when leashed to another entity. The first preset which "filter" conditions are met will be applied; if none match, a default configuration is used instead. |


> **presets** 子参数


| Name | Type | Default Value | Description |
| --- | --- | --- | --- |
| filter | Minecraft Filter |  | Conditions that must be met for this preset to be applied. |
| hard_distance | Decimal | 7 | Distance (in blocks) over which the entity starts being pulled towards the leash holder with a spring-like force. Entities can enter and stay in vehicles if the leash is stretched under this distance, but will dismount once it exceeds it. |
| max_distance | Decimal | 12 | Distance in blocks at which the leash breaks. |
| rotation_adjustment | Decimal | 0 | Adjusts the rotation at which the entity reaches equilibrium, when "spring_type" is set to "dampened" or "quad_dampened". |
| soft_distance | Decimal | 4 | Distance (in blocks) over which the entity starts pathfinding toward the leash holder, if able. |
| spring_type | Enumerator | dampened | Defines the type of spring-like force that pulls the entity towards its leash holder:- "bouncy": Simulates a highly elastic spring that never reaches an equilibrium if the leashed entity is suspended mid-air.- "dampened": Simulates a dampened spring attached to the front of the leashed entity's collision. It reaches an equilibrium if the entity is suspended mid-air and aligns with the movement direction.- "quad_dampened": Simulates four dampened springs connected to the center of each side of the entities' collisions. It reaches an equilibrium if the entity is suspended mid-air and gradually aligns with the leash holder over time. |


## minecraft:leashable_to


Allows players to leash entities to this entity, retrieve entities already leashed to it, or free them using shears. For the last interaction to work, the leashed entities must have "can_be_cut" set to true in their "minecraft:leashable" component.


| Name | Type | Default Value | Description |
| --- | --- | --- | --- |
| can_retrieve_from | Boolean | false | Allows players to retrieve entities that are leashed to this entity. |


## minecraft:looked_at


Defines the behavior when another entity looks at the owner entity.


| Name | Type | Default Value | Description |
| --- | --- | --- | --- |
| field_of_view | Decimal | 26 | Defines, in degrees, the width of the field of view for entities looking at the owner entity. If "scale_fov_by_distance" is set to true, this value corresponds to the field of view at a distance of one block between the entities. |
| filters | Minecraft Filter |  | Defines which entities are considered when searching for entities looking at the owner entity. |
| find_players_only | Boolean | false | Limits the search to only the nearest Player that meets the specified "filters" rather than all nearby entities. |
| line_of_sight_obstruction_type | String | collision | Defines the type of block shape used to check for line of sight obstructions. Valid values: "outline", "collision", "collision_for_camera". |
| look_at_locations | List |  | A list of locations on the owner entity towards which line of sight checks are performed. At least one location must be unobstructed for the entity to be considered as looked at. |
| looked_at_cooldown | Range [a, b] | [0, 0] | Specifies the range for the random number of seconds that must pass before the owner entity can check again for entities looking at it, after detecting an entity looking at it. |
| looked_at_event | String |  | Defines the event to trigger when an entity is detected looking at the owner entity. |
| min_looked_at_duration | Decimal | 0 | Defines the minimum, continuous time the owner entity has to be looked at before being considered as such. Defaults to 0 if not explicitly specified. |
| not_looked_at_event | String |  | Defines the event to trigger when no entity is found looking at the owner entity. |
| scale_fov_by_distance | Boolean | true | When true, the field of view narrows as the distance between the owner entity and the entity looking at it increases. This ensures that the width of the view cone remains somewhat constant towards the owner entity position, regardless of distance. |
| search_radius | Decimal | 10 | Maximum distance the owner entity will search for entities looking at it. |
| set_target | Boolean | once_and_stop_scanning | Defines if and how the owner entity will set entities that are looking at it as its combat targets. Valid values: \n- "never", looking entities are never set as targets, but events are emitted. \n- "once_and_stop_scanning", the first detected looking entity is set as target. Scanning and event emission is suspended if and until the owner entity has a target. \n- "once_and_keep_scanning", the first detected looking entity is set as target. Scanning and event emission continues.s |


## minecraft:managed_wandering_trader


This component is used to implement part of the Wandering Trader behavior


## minecraft:mob_effect


A component that applies a mob effect to entities that get within range.


| Name | Type | Default Value | Description |
| --- | --- | --- | --- |
| cooldown_time | Integer | 0 | Time in seconds to wait between each application of the effect. |
| effect_range | Decimal | 0.2 | How close a hostile entity must be to have the mob effect applied. |
| effect_time | Integer | 10 | How long the applied mob effect lasts in seconds. Can also be set to "infinite" |
| entity_filter | Minecraft Filter |  | The set of entities that are valid to apply the mob effect to. |
| mob_effect | String |  | The mob effect that is applied to entities that enter this entities effect range. |


## minecraft:mob_effect_immunity


Entities with this component will have an immunity to the provided mob effects.


| Name | Type | Default Value | Description |
| --- | --- | --- | --- |
| mob_effects | Array |  | List of names of effects the entity is immune to. |


## minecraft:movement.amphibious


This move control allows the mob to swim in water and walk on land.


| Name | Type | Default Value | Description |
| --- | --- | --- | --- |
| max_turn | Decimal | 30.0 | The maximum number in degrees the mob can turn per tick. |


## minecraft:movement.basic


This component accents the movement of an entity.


| Name | Type | Default Value | Description |
| --- | --- | --- | --- |
| max_turn | Decimal | 30.0 | The maximum number in degrees the mob can turn per tick. |


## minecraft:movement.fly


This move control causes the mob to fly.


| Name | Type | Default Value | Description |
| --- | --- | --- | --- |
| max_turn | Decimal | 30.0 | The maximum number in degrees the mob can turn per tick. |


## minecraft:movement.generic


This move control allows a mob to fly, swim, climb, etc.


| Name | Type | Default Value | Description |
| --- | --- | --- | --- |
| max_turn | Decimal | 30.0 | The maximum number in degrees the mob can turn per tick. |


## minecraft:movement.hover


This move control causes the mob to hover.


| Name | Type | Default Value | Description |
| --- | --- | --- | --- |
| max_turn | Decimal | 30.0 | The maximum number in degrees the mob can turn per tick. |


## minecraft:movement.jump


Move control that causes the mob to jump as it moves with a specified delay between jumps.


| Name | Type | Default Value | Description |
| --- | --- | --- | --- |
| jump_delay | Range [a, b] | [0.0, 0.0] | Delay after landing when using the slime move control. |
| max_turn | Decimal | 30.0 | The maximum number in degrees the mob can turn per tick. |


## minecraft:movement.skip


This move control causes the mob to hop as it moves.


| Name | Type | Default Value | Description |
| --- | --- | --- | --- |
| max_turn | Decimal | 30.0 | The maximum number in degrees the mob can turn per tick. |


## minecraft:movement.sway


This move control causes the mob to sway side to side giving the impression it is swimming.


| Name | Type | Default Value | Description |
| --- | --- | --- | --- |
| max_turn | Decimal | 30.0 | The maximum number in degrees the mob can turn per tick. |
| sway_amplitude | Decimal | 0.05 | Strength of the sway movement. |
| sway_frequency | Decimal | 0.5 | Multiplier for the frequency of the sway movement. |


## minecraft:nameable


Allows this entity to be named (e.g. using a name tag).


| Name | Type | Default Value | Description |
| --- | --- | --- | --- |
| allow_name_tag_renaming | Boolean | true | If true, this entity can be renamed with name tags |
| always_show | Boolean | false | If true, the name will always be shown |
| default_trigger | String |  | Trigger to run when the entity gets named |
| name_actions | JSON Object |  | Describes the special names for this entity and the events to call when the entity acquires those names |


> **name_actions** 子参数


| Name | Type | Default Value | Description |
| --- | --- | --- | --- |
| name_filter | String |  | List of special names that will cause the events defined in 'on_named' to fire |
| on_named | String |  | Event to be called when this entity acquires the name specified in 'name_filter' |


## minecraft:navigation.climb


Allows this entity to generate paths that include vertical walls like the vanilla Spiders do.


| Name | Type | Default Value | Description |
| --- | --- | --- | --- |
| avoid_damage_blocks | Boolean | false | Tells the pathfinder to avoid blocks that cause damage when finding a path |
| avoid_portals | Boolean | false | Tells the pathfinder to avoid portals (like nether portals) when finding a path |
| avoid_sun | Boolean | false | Whether or not the pathfinder should avoid tiles that are exposed to the sun when creating paths |
| avoid_water | Boolean | false | Tells the pathfinder to avoid water when creating a path |
| blocks_to_avoid | List |  | Tells the pathfinder which blocks to avoid when creating a path |
| can_breach | Boolean | false | Tells the pathfinder whether or not it can jump out of water (like a dolphin) |
| can_break_doors | Boolean | false | Tells the pathfinder that it can path through a closed door and break it |
| can_jump | Boolean | true | Tells the pathfinder whether or not it can jump up blocks |
| can_open_doors | Boolean | false | Tells the pathfinder that it can path through a closed door assuming the AI will open the door |
| can_open_iron_doors | Boolean | false | Tells the pathfinder that it can path through a closed iron door assuming the AI will open the door |
| can_pass_doors | Boolean | true | Whether a path can be created through a door |
| can_path_from_air | Boolean | false | Tells the pathfinder that it can start pathing when in the air |
| can_path_over_lava | Boolean | false | Tells the pathfinder whether or not it can travel on the surface of the lava |
| can_path_over_water | Boolean | false | Tells the pathfinder whether or not it can travel on the surface of the water |
| can_sink | Boolean | true | Tells the pathfinder whether or not it will be pulled down by gravity while in water |
| can_swim | Boolean | false | Tells the pathfinder whether or not it can path anywhere through water and plays swimming animation along that path |
| can_walk | Boolean | true | Tells the pathfinder whether or not it can walk on the ground outside water |
| can_walk_in_lava | Boolean | false | Tells the pathfinder whether or not it can travel in lava like walking on ground |
| is_amphibious | Boolean | false | Tells the pathfinder whether or not it can walk on the ground underwater |


## minecraft:navigation.float


Allows this entity to generate paths by flying around the air like the regular Ghast.


| Name | Type | Default Value | Description |
| --- | --- | --- | --- |
| avoid_damage_blocks | Boolean | false | Tells the pathfinder to avoid blocks that cause damage when finding a path |
| avoid_portals | Boolean | false | Tells the pathfinder to avoid portals (like nether portals) when finding a path |
| avoid_sun | Boolean | false | Whether or not the pathfinder should avoid tiles that are exposed to the sun when creating paths |
| avoid_water | Boolean | false | Tells the pathfinder to avoid water when creating a path |
| blocks_to_avoid | List |  | Tells the pathfinder which blocks to avoid when creating a path |
| can_breach | Boolean | false | Tells the pathfinder whether or not it can jump out of water (like a dolphin) |
| can_break_doors | Boolean | false | Tells the pathfinder that it can path through a closed door and break it |
| can_jump | Boolean | true | Tells the pathfinder whether or not it can jump up blocks |
| can_open_doors | Boolean | false | Tells the pathfinder that it can path through a closed door assuming the AI will open the door |
| can_open_iron_doors | Boolean | false | Tells the pathfinder that it can path through a closed iron door assuming the AI will open the door |
| can_pass_doors | Boolean | true | Whether a path can be created through a door |
| can_path_from_air | Boolean | false | Tells the pathfinder that it can start pathing when in the air |
| can_path_over_lava | Boolean | false | Tells the pathfinder whether or not it can travel on the surface of the lava |
| can_path_over_water | Boolean | false | Tells the pathfinder whether or not it can travel on the surface of the water |
| can_sink | Boolean | true | Tells the pathfinder whether or not it will be pulled down by gravity while in water |
| can_swim | Boolean | false | Tells the pathfinder whether or not it can path anywhere through water and plays swimming animation along that path |
| can_walk | Boolean | true | Tells the pathfinder whether or not it can walk on the ground outside water |
| can_walk_in_lava | Boolean | false | Tells the pathfinder whether or not it can travel in lava like walking on ground |
| is_amphibious | Boolean | false | Tells the pathfinder whether or not it can walk on the ground underwater |


## minecraft:navigation.fly


Allows this entity to generate paths in the air like the vanilla Parrots do.


| Name | Type | Default Value | Description |
| --- | --- | --- | --- |
| avoid_damage_blocks | Boolean | false | Tells the pathfinder to avoid blocks that cause damage when finding a path |
| avoid_portals | Boolean | false | Tells the pathfinder to avoid portals (like nether portals) when finding a path |
| avoid_sun | Boolean | false | Whether or not the pathfinder should avoid tiles that are exposed to the sun when creating paths |
| avoid_water | Boolean | false | Tells the pathfinder to avoid water when creating a path |
| blocks_to_avoid | List |  | Tells the pathfinder which blocks to avoid when creating a path |
| can_breach | Boolean | false | Tells the pathfinder whether or not it can jump out of water (like a dolphin) |
| can_break_doors | Boolean | false | Tells the pathfinder that it can path through a closed door and break it |
| can_jump | Boolean | true | Tells the pathfinder whether or not it can jump up blocks |
| can_open_doors | Boolean | false | Tells the pathfinder that it can path through a closed door assuming the AI will open the door |
| can_open_iron_doors | Boolean | false | Tells the pathfinder that it can path through a closed iron door assuming the AI will open the door |
| can_pass_doors | Boolean | true | Whether a path can be created through a door |
| can_path_from_air | Boolean | false | Tells the pathfinder that it can start pathing when in the air |
| can_path_over_lava | Boolean | false | Tells the pathfinder whether or not it can travel on the surface of the lava |
| can_path_over_water | Boolean | false | Tells the pathfinder whether or not it can travel on the surface of the water |
| can_sink | Boolean | true | Tells the pathfinder whether or not it will be pulled down by gravity while in water |
| can_swim | Boolean | false | Tells the pathfinder whether or not it can path anywhere through water and plays swimming animation along that path |
| can_walk | Boolean | true | Tells the pathfinder whether or not it can walk on the ground outside water |
| can_walk_in_lava | Boolean | false | Tells the pathfinder whether or not it can travel in lava like walking on ground |
| is_amphibious | Boolean | false | Tells the pathfinder whether or not it can walk on the ground underwater |


## minecraft:navigation.generic


Allows this entity to generate paths by walking, swimming, flying and/or climbing around and jumping up and down a block.


| Name | Type | Default Value | Description |
| --- | --- | --- | --- |
| avoid_damage_blocks | Boolean | false | Tells the pathfinder to avoid blocks that cause damage when finding a path |
| avoid_portals | Boolean | false | Tells the pathfinder to avoid portals (like nether portals) when finding a path |
| avoid_sun | Boolean | false | Whether or not the pathfinder should avoid tiles that are exposed to the sun when creating paths |
| avoid_water | Boolean | false | Tells the pathfinder to avoid water when creating a path |
| blocks_to_avoid | List |  | Tells the pathfinder which blocks to avoid when creating a path |
| can_breach | Boolean | false | Tells the pathfinder whether or not it can jump out of water (like a dolphin) |
| can_break_doors | Boolean | false | Tells the pathfinder that it can path through a closed door and break it |
| can_jump | Boolean | true | Tells the pathfinder whether or not it can jump up blocks |
| can_open_doors | Boolean | false | Tells the pathfinder that it can path through a closed door assuming the AI will open the door |
| can_open_iron_doors | Boolean | false | Tells the pathfinder that it can path through a closed iron door assuming the AI will open the door |
| can_pass_doors | Boolean | true | Whether a path can be created through a door |
| can_path_from_air | Boolean | false | Tells the pathfinder that it can start pathing when in the air |
| can_path_over_lava | Boolean | false | Tells the pathfinder whether or not it can travel on the surface of the lava |
| can_path_over_water | Boolean | false | Tells the pathfinder whether or not it can travel on the surface of the water |
| can_sink | Boolean | true | Tells the pathfinder whether or not it will be pulled down by gravity while in water |
| can_swim | Boolean | false | Tells the pathfinder whether or not it can path anywhere through water and plays swimming animation along that path |
| can_walk | Boolean | true | Tells the pathfinder whether or not it can walk on the ground outside water |
| can_walk_in_lava | Boolean | false | Tells the pathfinder whether or not it can travel in lava like walking on ground |
| is_amphibious | Boolean | false | Tells the pathfinder whether or not it can walk on the ground underwater |


## minecraft:navigation.hover


Allows this entity to generate paths in the air like the vanilla Bees do. Keeps them from falling out of the skies and doing predictive movement.


| Name | Type | Default Value | Description |
| --- | --- | --- | --- |
| avoid_damage_blocks | Boolean | false | Tells the pathfinder to avoid blocks that cause damage when finding a path |
| avoid_portals | Boolean | false | Tells the pathfinder to avoid portals (like nether portals) when finding a path |
| avoid_sun | Boolean | false | Whether or not the pathfinder should avoid tiles that are exposed to the sun when creating paths |
| avoid_water | Boolean | false | Tells the pathfinder to avoid water when creating a path |
| blocks_to_avoid | List |  | Tells the pathfinder which blocks to avoid when creating a path |
| can_breach | Boolean | false | Tells the pathfinder whether or not it can jump out of water (like a dolphin) |
| can_break_doors | Boolean | false | Tells the pathfinder that it can path through a closed door and break it |
| can_jump | Boolean | true | Tells the pathfinder whether or not it can jump up blocks |
| can_open_doors | Boolean | false | Tells the pathfinder that it can path through a closed door assuming the AI will open the door |
| can_open_iron_doors | Boolean | false | Tells the pathfinder that it can path through a closed iron door assuming the AI will open the door |
| can_pass_doors | Boolean | true | Whether a path can be created through a door |
| can_path_from_air | Boolean | false | Tells the pathfinder that it can start pathing when in the air |
| can_path_over_lava | Boolean | false | Tells the pathfinder whether or not it can travel on the surface of the lava |
| can_path_over_water | Boolean | false | Tells the pathfinder whether or not it can travel on the surface of the water |
| can_sink | Boolean | true | Tells the pathfinder whether or not it will be pulled down by gravity while in water |
| can_swim | Boolean | false | Tells the pathfinder whether or not it can path anywhere through water and plays swimming animation along that path |
| can_walk | Boolean | true | Tells the pathfinder whether or not it can walk on the ground outside water |
| can_walk_in_lava | Boolean | false | Tells the pathfinder whether or not it can travel in lava like walking on ground |
| is_amphibious | Boolean | false | Tells the pathfinder whether or not it can walk on the ground underwater |


## minecraft:navigation.swim


Allows this entity to generate paths that include water.


| Name | Type | Default Value | Description |
| --- | --- | --- | --- |
| avoid_damage_blocks | Boolean | false | Tells the pathfinder to avoid blocks that cause damage when finding a path |
| avoid_portals | Boolean | false | Tells the pathfinder to avoid portals (like nether portals) when finding a path |
| avoid_sun | Boolean | false | Whether or not the pathfinder should avoid tiles that are exposed to the sun when creating paths |
| avoid_water | Boolean | false | Tells the pathfinder to avoid water when creating a path |
| blocks_to_avoid | List |  | Tells the pathfinder which blocks to avoid when creating a path |
| can_breach | Boolean | false | Tells the pathfinder whether or not it can jump out of water (like a dolphin) |
| can_break_doors | Boolean | false | Tells the pathfinder that it can path through a closed door and break it |
| can_jump | Boolean | true | Tells the pathfinder whether or not it can jump up blocks |
| can_open_doors | Boolean | false | Tells the pathfinder that it can path through a closed door assuming the AI will open the door |
| can_open_iron_doors | Boolean | false | Tells the pathfinder that it can path through a closed iron door assuming the AI will open the door |
| can_pass_doors | Boolean | true | Whether a path can be created through a door |
| can_path_from_air | Boolean | false | Tells the pathfinder that it can start pathing when in the air |
| can_path_over_lava | Boolean | false | Tells the pathfinder whether or not it can travel on the surface of the lava |
| can_path_over_water | Boolean | false | Tells the pathfinder whether or not it can travel on the surface of the water |
| can_sink | Boolean | true | Tells the pathfinder whether or not it will be pulled down by gravity while in water |
| can_swim | Boolean | false | Tells the pathfinder whether or not it can path anywhere through water and plays swimming animation along that path |
| can_walk | Boolean | true | Tells the pathfinder whether or not it can walk on the ground outside water |
| can_walk_in_lava | Boolean | false | Tells the pathfinder whether or not it can travel in lava like walking on ground |
| is_amphibious | Boolean | false | Tells the pathfinder whether or not it can walk on the ground underwater |


## minecraft:navigation.walk


Allows this entity to generate paths by walking around and jumping up and down a block like regular mobs.


| Name | Type | Default Value | Description |
| --- | --- | --- | --- |
| avoid_damage_blocks | Boolean | false | Tells the pathfinder to avoid blocks that cause damage when finding a path |
| avoid_portals | Boolean | false | Tells the pathfinder to avoid portals (like nether portals) when finding a path |
| avoid_sun | Boolean | false | Whether or not the pathfinder should avoid tiles that are exposed to the sun when creating paths |
| avoid_water | Boolean | false | Tells the pathfinder to avoid water when creating a path |
| blocks_to_avoid | List |  | Tells the pathfinder which blocks to avoid when creating a path |
| can_breach | Boolean | false | Tells the pathfinder whether or not it can jump out of water (like a dolphin) |
| can_break_doors | Boolean | false | Tells the pathfinder that it can path through a closed door and break it |
| can_jump | Boolean | true | Tells the pathfinder whether or not it can jump up blocks |
| can_open_doors | Boolean | false | Tells the pathfinder that it can path through a closed door assuming the AI will open the door |
| can_open_iron_doors | Boolean | false | Tells the pathfinder that it can path through a closed iron door assuming the AI will open the door |
| can_pass_doors | Boolean | true | Whether a path can be created through a door |
| can_path_from_air | Boolean | false | Tells the pathfinder that it can start pathing when in the air |
| can_path_over_lava | Boolean | false | Tells the pathfinder whether or not it can travel on the surface of the lava |
| can_path_over_water | Boolean | false | Tells the pathfinder whether or not it can travel on the surface of the water |
| can_sink | Boolean | true | Tells the pathfinder whether or not it will be pulled down by gravity while in water |
| can_swim | Boolean | false | Tells the pathfinder whether or not it can path anywhere through water and plays swimming animation along that path |
| can_walk | Boolean | true | Tells the pathfinder whether or not it can walk on the ground outside water |
| can_walk_in_lava | Boolean | false | Tells the pathfinder whether or not it can travel in lava like walking on ground |
| is_amphibious | Boolean | false | Tells the pathfinder whether or not it can walk on the ground underwater |


## minecraft:out_of_control


Defines the entity's 'out of control' state.


## minecraft:peek


Defines the entity's 'peek' behavior, defining the events that should be called during it.


| Name | Type | Default Value | Description |
| --- | --- | --- | --- |
| on_close | String |  | Event to call when the entity is done peeking. |
| on_open | String |  | Event to call when the entity starts peeking. |
| on_target_open | String |  | Event to call when the entity's target entity starts peeking. |


## minecraft:persistent


Defines whether an entity should be persistent in the game world.


## minecraft:physics


Defines physics properties of an actor, including if it is affected by gravity or if it collides with objects.


| Name | Type | Default Value | Description |
| --- | --- | --- | --- |
| has_collision | Boolean | true | Whether or not the object collides with things. |
| has_gravity | Boolean | true | Whether or not the entity is affected by gravity. |
| push_towards_closest_space | Boolean | false | Whether or not the entity should be pushed towards the nearest open area when stuck inside a block. |


## minecraft:preferred_path


Specifies costing information for mobs that prefer to walk on preferred paths.


| Name | Type | Default Value | Description |
| --- | --- | --- | --- |
| default_block_cost | Decimal | 0 | Cost for non-preferred blocks |
| jump_cost | Integer | 0 | Added cost for jumping up a node |
| max_fall_blocks | Integer | 3 | Distance mob can fall without taking damage |
| preferred_path_blocks | List |  | A list of blocks with their associated cost |


## minecraft:projectile


Allows the entity to be a thrown entity.


| Name | Type | Default Value | Description |
| --- | --- | --- | --- |
| angle_offset | Decimal | 0 | Determines the angle at which the projectile is thrown |
| catch_fire | Boolean | false | If true, the entity hit will be set on fire |
| crit_particle_on_hurt | Boolean | false | If true, the projectile will produce additional particles when a critical hit happens |
| destroy_on_hurt | Boolean | false | If true, this entity will be destroyed when hit |
| filter | String |  | Entity Definitions defined here can't be hurt by the projectile |
| fire_affected_by_griefing | Boolean | false | If true, whether the projectile causes fire is affected by the mob griefing game rule |
| gravity | Decimal | 0.05 | The gravity applied to this entity when thrown. The higher the value, the faster the entity falls |
| hit_nearest_passenger | Boolean | false | If true, when hitting a vehicle, and there's at least one passenger in the vehicle, the damage will be dealt to the passenger closest to the projectile impact point. If there are no passengers, this setting does nothing. |
| hit_sound | String |  | The sound that plays when the projectile hits something |
| homing | Boolean | false | If true, the projectile homes in to the nearest entity |
| ignored_entities | Array |  | [EXPERIMENTAL] An array of strings defining the types of entities that this entity does not collide with. |
| inertia | Decimal | 0.99 | The fraction of the projectile's speed maintained every frame while traveling in air |
| is_dangerous | Boolean | false | If true, the projectile will be treated as dangerous to the players |
| knockback | Boolean | true | If true, the projectile will knock back the entity it hits |
| lightning | Boolean | false | If true, the entity hit will be struck by lightning |
| liquid_inertia | Decimal | 0.6 | The fraction of the projectile's speed maintained every frame while traveling in water |
| multiple_targets | Boolean | true | If true, the projectile can hit multiple entities per flight |
| offset | Vector [a, b, c] | [0, 0, 0] | The offset from the entity's anchor where the projectile will spawn |
| on_fire_time | Decimal | 5 | Time in seconds that the entity hit will be on fire for |
| particle | String | iconcrack | Particle to use upon collision |
| potion_effect | Integer | -1 | Defines the effect the arrow will apply to the entity it hits |
| power | Decimal | 1.3 | Determines the velocity of the projectile |
| reflect_immunity | Decimal | 0 | During the specified time, in seconds, the projectile cannot be reflected by hitting it |
| reflect_on_hurt | Boolean | false | If true, this entity will be reflected back when hit |
| semi_random_diff_damage | Boolean | false | If true, damage will be randomized based on damage and speed |
| shoot_sound | String |  | The sound that plays when the projectile is shot |
| shoot_target | Boolean | true | If true, the projectile will be shot towards the target of the entity firing it |
| should_bounce | Boolean | false | If true, the projectile will bounce upon hit |
| splash_potion | Boolean | false | If true, the projectile will be treated like a splash potion |
| splash_range | Decimal | 4 | Radius in blocks of the 'splash' effect |
| uncertainty_base | Decimal | 0 | The base accuracy. Accuracy is determined by the formula uncertaintyBase - difficultyLevel * uncertaintyMultiplier |
| uncertainty_multiplier | Decimal | 0 | Determines how much difficulty affects accuracy. Accuracy is determined by the formula uncertaintyBase - difficultyLevel * uncertaintyMultiplier |


## minecraft:pushable


Defines what can push an entity between other entities and pistons.


| Name | Type | Default Value | Description |
| --- | --- | --- | --- |
| is_pushable | Boolean | true | Whether the entity can be pushed by other entities. |
| is_pushable_by_piston | Boolean | true | Whether the entity can be pushed by pistons safely. |


## minecraft:raid_trigger


Attempts to trigger a raid at the entity's location.


| Name | Type | Default Value | Description |
| --- | --- | --- | --- |
| triggered_event | String |  | Event to run when a raid is triggered on the village. |


## minecraft:rail_movement


Defines the entity's movement on the rails. An entity with this component is only allowed to move on the rail.


| Name | Type | Default Value | Description |
| --- | --- | --- | --- |
| max_speed | Decimal | 0.4 | Maximum speed that this entity will move at when on the rail. |


## minecraft:rail_sensor


| Name | Type | Default Value | Description |
| --- | --- | --- | --- |
| check_block_types | Boolean | false | If true, on tick this entity will trigger its on_deactivate behavior |
| eject_on_activate | Boolean | true | If true, this entity will eject all of its riders when it passes over an activated rail |
| eject_on_deactivate | Boolean | false | If true, this entity will eject all of its riders when it passes over a deactivated rail |
| on_activate | String |  | Event to call when the rail is activated |
| on_deactivate | String |  | Event to call when the rail is deactivated |
| tick_command_block_on_activate | Boolean | true | If true, command blocks will start ticking when passing over an activated rail |
| tick_command_block_on_deactivate | Boolean | false | If false, command blocks will stop ticking when passing over a deactivated rail |


## minecraft:ravager_blocked


Defines the ravager's response to their melee attack being blocked.


| Name | Type | Default Value | Description |
| --- | --- | --- | --- |
| knockback_strength | Decimal | 3.0 | The strength with which blocking entities should be knocked back |
| reaction_choices | List | [ ] | A list of weighted responses to the melee attack being blocked |


## minecraft:reflect_projectiles


[EXPERIMENTAL] Allows an entity to reflect projectiles.


| Name | Type | Default Value | Description |
| --- | --- | --- | --- |
| azimuth_angle | Molang | 0 | [EXPERIMENTAL] A Molang expression defining the angle in degrees to add to the projectile's y axis rotation. |
| elevation_angle | Molang | 0 | [EXPERIMENTAL] A Molang expression defining the angle in degrees to add to the projectile's x axis rotation. |
| reflected_projectiles | Array |  | [EXPERIMENTAL] An array of strings defining the types of projectiles that are reflected when they hit the entity. |
| reflection_scale | Molang | 1 | [EXPERIMENTAL] A Molang expression defining the velocity scaling of the reflected projectile. Values below 1 decrease the projectile's velocity, and values above 1 increase it. |
| reflection_sound | String | reflect | [EXPERIMENTAL] A string defining the name of the sound event to be played when a projectile is reflected. "reflect" unless specified. |


## minecraft:rideable


Determines whether this entity can be ridden. Allows specifying the different seat positions and amount.


| Name | Type | Default Value | Description |
| --- | --- | --- | --- |
| controlling_seat | Integer | 0 | The seat that designates the driver of the entity. Entities with the "minecraft:behavior.controlled_by_player" goal ignore this field and give control to any player in any seat. |
| crouching_skip_interact | Boolean | true | If true, this entity can't be interacted with if the entity interacting with it is crouching. |
| dismount_mode | Enumerator | default | Defines where riders are placed when dismounting this entity:- "default", riders are placed on a valid ground position around the entity, or at the center of the entity's collision box if none is found.- "on_top_center", riders are placed at the center of the top of the entity's collision box. |
| family_types | List |  | List of entities that can ride this entity. |
| interact_text | String |  | The text to display when the player can interact with the entity when playing with touch-screen controls. |
| on_rider_enter_event | String |  | Event to execute on the owner entity when an entity starts riding it. |
| on_rider_exit_event | String |  | Event to execute on the owner entity when an entity stops riding it. |
| passenger_max_width | Decimal | 0.00 | The max width a mob can have to be a rider. A value of 0 ignores this parameter. |
| priority | Integer | N/A | This field may exist in old data but isn't used by "minecraft:rideable". |
| pull_in_entities | Boolean | false | If true, this entity will pull entities matching the specified "family_types" into any available seats. Entities that are leashed will only be pulled in if their distance to their leash holder is less than the "hard_distance" defined in their own "minecraft:leashable" component. |
| rider_can_interact | Boolean | false | If true, this entity will be picked when looked at by the rider. |
| seat_count | Integer | 1 | The number of entities that can ride this entity at the same time. |
| seats | List |  | The list of positions and number of riders for each position for entities riding this entity. |


> **seats** 子参数


| Name | Type | Default Value | Description |
| --- | --- | --- | --- |
| lock_rider_rotation | Decimal | 181 | Angle in degrees that a rider is allowed to rotate while riding this entity. Omit this property for no limit. |
| max_rider_count | Integer | 0 | Defines the maximum number of riders that can be riding this entity for this seat to be valid. |
| min_rider_count | Integer | 0 | Defines the minimum number of riders that need to be riding this entity before this seat can be used. |
| position | Vector [a, b, c] | [0, 0, 0] | Position of this seat relative to this entity's position. |
| rotate_rider_by | Molang | 0 | Offset to rotate riders by. |


## minecraft:scale_by_age


Defines the entity's size interpolation based on the entity's age.


| Name | Type | Default Value | Description |
| --- | --- | --- | --- |
| end_scale | Decimal | 1 | Ending scale of the entity when it's fully grown. |
| start_scale | Decimal | 1 | Initial scale of the newborn entity. |


## minecraft:scheduler


Fires off scheduled mob events at time of day events.


| Name | Type | Default Value | Description |
| --- | --- | --- | --- |
| scheduled_events | List |  | The list of triggers that fire when the conditions match the given filter criteria. If any filter criteria overlap the first defined event will be picked. |


## minecraft:shareables


Defines a list of items the mob wants to share. Each item must have the following parameters:


| Name | Type | Default Value | Description |
| --- | --- | --- | --- |
| all_items | Boolean | false | A bucket for all other items in the game. Note this category is always least priority items. |
| all_items_max_amount | Integer | -1 | Maximum number of this item the mob will hold. |
| all_items_surplus_amount | Integer | -1 | Number of this item considered extra that the entity wants to share. |
| all_items_want_amount | Integer | -1 | Number of this item this entity wants to share. |
| items | List |  | List of items or item tags that the entity wants to share. Items in-game will match to this list from top to bottom, meaning that the first occurance of a match, be it name or tag, will shadow those coming after.admire Mob will admire the item after picking up by looking at it. For this to happen the mob needs to have an Admire component and an Admire goal. barter Mob will barter for the item after picking it up. For this to work the mob needs to have a Barter component and a Barter goal. consume_item Determines whether the mob will consume the item or not. craft_into Defines the item this entity wants to craft with the item defined by "item". Should be an item name. item The name of the item. Aux value can be specified, for instance 'minecraft:skull:1'. Alternatively, a tag can be specified to match all items with that tag. max_amount Maximum number of this item the mob will hold. pickup_limit Maximum number items the mob will pick up during a single goal tick. pickup_only Determines whether the mob can only pickup the item and not drop it. priority Prioritizes which items the entity prefers. 0 is the highest priority. stored_in_inventory Determines whether the mob will try to put the item in its inventory if it has the inventory component and if it can't be equipped. surplus_amount Number of this item considered extra that the entity wants to share. want_amount Number of this item this entity wants to share. |
| singular_pickup | Boolean | false | Boolean value that controls if the mob is able to pick up more of the same item if it is already holding that item |


## minecraft:shooter


Defines the entity's ranged attack behavior. The "minecraft:behavior.ranged_attack" goal uses this component to determine which projectiles to shoot.


| Name | Type | Default Value | Description |
| --- | --- | --- | --- |
| aux_val | Integer | -1 | ID of the Potion effect for the default projectile to be applied on hit. |
| def | String |  | Actor definition to use as the default projectile for the ranged attack. The actor definition must have the projectile component to be able to be shot as a projectile. |
| magic | Boolean | false | Sets whether the projectiles being used are flagged as magic. If set, the ranged attack goal will not be used at the same time as other magic goals, such as minecraft:behavior.drink_potion |
| power | Decimal | 0.00 | Velocity in which the projectiles will be shot at. A power of 0 will be overwritten by the default projectile throw power. |
| projectiles | List |  | List of projectiles that can be used by the shooter. Projectiles are evaluated in the order of the list; After a projectile is chosen, the rest of the list is ignored. |
| sound | String |  | Sound that is played when the shooter shoots a projectile. |


## minecraft:sittable


Defines the entity's 'sit' state.


| Name | Type | Default Value | Description |
| --- | --- | --- | --- |
| sit_event | String |  | Event to run when the entity enters the 'sit' state |
| stand_event | String |  | Event to run when the entity exits the 'sit' state |


## minecraft:spawn_entity


Adds a timer after which this entity will spawn another entity or item (similar to vanilla's chicken's egg-laying behavior).


| Name | Type | Default Value | Description |
| --- | --- | --- | --- |
| filters | Minecraft Filter |  | If present, the specified entity will only spawn if the filter evaluates to true. |
| max_wait_time | Integer | 600 | Maximum amount of time to randomly wait in seconds before another entity is spawned. |
| min_wait_time | Integer | 300 | Minimum amount of time to randomly wait in seconds before another entity is spawned. |
| num_to_spawn | Integer | 1 | The number of entities of this type to spawn each time that this triggers. |
| should_leash | Boolean | false | If true, this the spawned entity will be leashed to the parent. |
| single_use | Boolean | false | If true, this component will only ever spawn the specified entity once. |
| spawn_entity | String |  | Identifier of the entity to spawn, leave empty to spawn the item defined by "spawn_item" instead. |
| spawn_event | String | minecraft:entity_born | Event to call on the spawned entity when it spawns. |
| spawn_item | String | egg | Item identifier of the item to spawn. |
| spawn_item_event | Trigger |  | Event to call on this entity when the item is spawned. |
| spawn_method | String | born | Method to use to spawn the entity. |
| spawn_sound | String | plop | Identifier of the sound effect to play when the entity is spawned. |


## minecraft:suspect_tracking


Allows this entity to remember suspicious locations


## minecraft:tameable


Defines the rules for a mob to be tamed by the player.


| Name | Type | Default Value | Description |
| --- | --- | --- | --- |
| probability | Decimal | 1 | The chance of taming the entity with each item use between 0.0 and 1.0, where 1.0 is 100% |
| tame_event | String |  | Event to run when this entity becomes tamed |
| tame_items | List |  | The list of items that can be used to tame this entity |


## minecraft:tamemount


Allows the Entity to be tamed by mounting it.


| Name | Type | Default Value | Description |
| --- | --- | --- | --- |
| attempt_temper_mod | Integer | 5 | The amount the entity's temper will increase when mounted. |
| autoRejectItems | JSON Object |  | The list of items that, if carried while interacting with the entity, will anger it. |
| feed_items | JSON Object |  | The list of items that can be used to increase the entity's temper and speed up the taming process. |
| feed_text | String |  | The text that shows in the feeding interact button. |
| max_temper | Integer | 100 | The maximum value for the entity's random starting temper. |
| min_temper | Integer | 0 | The minimum value for the entity's random starting temper. |
| ride_text | String |  | The text that shows in the riding interact button. |
| tame_event | String |  | Event that triggers when the entity becomes tamed. |


> **autoRejectItems** 子参数


| Name | Type | Default Value | Description |
| --- | --- | --- | --- |
| item | String |  | Name of the item this entity dislikes and will cause it to get angry if used while untamed. |


> **feed_items** 子参数


| Name | Type | Default Value | Description |
| --- | --- | --- | --- |
| item | String |  | Name of the item this entity likes and can be used to increase this entity's temper. |
| temper_mod | Integer | 0 | The amount of temper this entity gains when fed this item. |


## minecraft:target_nearby_sensor


Defines the entity's range within which it can see or sense other entities to target them.


| Name | Type | Default Value | Description |
| --- | --- | --- | --- |
| inside_range | Decimal | 1 | Maximum distance in blocks that another entity will be considered in the 'inside' range |
| must_see | Boolean | false | Whether the other entity needs to be visible to trigger 'inside' events |
| on_inside_range | String |  | Event to call when an entity gets in the inside range. Can specify 'event' for the name of the event and 'target' for the target of the event |
| on_outside_range | String |  | Event to call when an entity gets in the outside range. Can specify 'event' for the name of the event and 'target' for the target of the event |
| on_vision_lost_inside_range | String |  | Event to call when an entity exits visual range. Can specify 'event' for the name of the event and 'target' for the target of the event |
| outside_range | Decimal | 5 | Maximum distance in blocks that another entity will be considered in the 'outside' range |


## minecraft:teleport


Defines an entity's teleporting behavior.


| Name | Type | Default Value | Description |
| --- | --- | --- | --- |
| dark_teleport_chance | Decimal | 0.01 | Modifies the chance that the entity will teleport if the entity is in darkness |
| light_teleport_chance | Decimal | 0.01 | Modifies the chance that the entity will teleport if the entity is in daylight |
| max_random_teleport_time | Decimal | 20 | Maximum amount of time in seconds between random teleports |
| min_random_teleport_time | Decimal | 0 | Minimum amount of time in seconds between random teleports |
| random_teleport_cube | Vector [a, b, c] | [32, 16, 32] | Entity will teleport to a random position within the area defined by this cube |
| random_teleports | Boolean | true | If true, the entity will teleport randomly |
| target_distance | Decimal | 16 | Maximum distance the entity will teleport when chasing a target |
| target_teleport_chance | Decimal | 1 | The chance that the entity will teleport between 0.0 and 1.0. 1.0 means 100% |


## minecraft:timer


Adds a timer after which an event will fire.


| Name | Type | Default Value | Description |
| --- | --- | --- | --- |
| looping | Boolean | true | If true, the timer will restart every time after it fires. |
| randomInterval | Boolean | true | If true, the amount of time on the timer will be random between the min and max values specified in time. |
| random_time_choices | List | [ ] | This is a list of objects, representing one value in seconds that can be picked before firing the event and an optional weight. Incompatible with time. |
| time | Range [a, b] | [0.0, 0.0] | Amount of time in seconds for the timer. Can be specified as a number or a pair of numbers (min and max). Incompatible with random_time_choices. |
| time_down_event | String |  | Event to fire when the time on the timer runs out. |


## minecraft:trade_table


Defines this entity's ability to trade with players.


| Name | Type | Default Value | Description |
| --- | --- | --- | --- |
| convert_trades_economy | Boolean | false | Determines when the mob transforms, if the trades should be converted when the new mob has a economy_trade_table. When the trades are converted, the mob will generate a new trade list with their new trade table, but then it will try to convert any of the same trades over to have the same enchantments and user data. For example, if the original has a Emerald to Enchanted Iron Sword (Sharpness 1), and the new trade also has an Emerald for Enchanted Iron Sword, then the enchantment will be Sharpness 1. |
| display_name | String |  | Name to be displayed while trading with this entity. |
| new_screen | Boolean | false | Used to determine if trading with entity opens the new trade screen. |
| persist_trades | Boolean | false | Determines if the trades should persist when the mob transforms. This makes it so that the next time the mob is transformed to something with a trade_table or economy_trade_table, then it keeps their trades. |
| table | String |  | File path relative to the behavior pack root for this entity's trades. |


## minecraft:trail


Causes an entity to leave a trail of blocks as it moves about the world.


| Name | Type | Default Value | Description |
| --- | --- | --- | --- |
| block_type | String | air | The type of block you wish to be spawned by the entity as it move about the world. Solid blocks may not be spawned at an offset of (0,0,0). |
| spawn_filter | Minecraft Filter |  | One or more conditions that must be met in order to cause the chosen block type to spawn. |
| spawn_offset | Vector [a, b, c] | [0, 0, 0] | The distance from the entities current position to spawn the block. Capped at up to 16 blocks away. The X value is left/right(-/+), the Z value is backward/forward(-/+), the Y value is below/above(-/+). |


## minecraft:transformation


Defines an entity's transformation from the current definition into another


| Name | Type | Default Value | Description |
| --- | --- | --- | --- |
| add | JSON Object |  | List of components to add to the entity after the transformation |
| begin_transform_sound | String |  | Sound to play when the transformation starts |
| delay | JSON Object |  | Defines the properties of the delay for the transformation |
| drop_equipment | Boolean | false | Cause the entity to drop all equipment upon transformation |
| drop_inventory | Boolean | false | Cause the entity to drop all items in inventory upon transformation |
| into | String |  | Entity Definition that this entity will transform into |
| keep_level | Boolean | false | If this entity has trades and has leveled up, it should maintain that level after transformation. |
| keep_owner | Boolean | false | If this entity is owned by another entity, it should remain owned after transformation. |
| preserve_equipment | Boolean | false | Cause the entity to keep equipment after going through transformation |
| transformation_sound | String |  | Sound to play when the entity is done transforming |


> **add** 子参数


| Name | Type | Default Value | Description |
| --- | --- | --- | --- |
| component_groups | List |  | Names of component groups to add |


> **delay** 子参数


| Name | Type | Default Value | Description |
| --- | --- | --- | --- |
| block_assist_chance | Decimal | 0 | Chance that the entity will look for nearby blocks that can speed up the transformation. Value must be between 0.0 and 1.0 |
| block_chance | Decimal | 0 | Chance that, once a block is found, will help speed up the transformation |
| block_max | Integer | 0 | Maximum number of blocks the entity will look for to aid in the transformation. If not defined or set to 0, it will be set to the block radius |
| block_radius | Integer | 0 | Distance in Blocks that the entity will search for blocks that can help the transformation |
| block_types | List |  | List of blocks that can help the transformation of this entity |
| range_max | Decimal | 0 | Time in seconds to be added to value to have the maximum random time range value until the entity transforms (if non-zero then the time in seconds before the entity transforms will be random between value+range_min and value+range_max) |
| range_min | Decimal | 0 | Time in seconds to be added to value to have the minimum random time range value until the entity transforms (if non-zero then the time in seconds before the entity transforms will be random between value+range_min and value+range_max) |
| value | Decimal | 0 | Time in seconds before the entity transforms |


## minecraft:transient


An entity with this component will NEVER persist, and forever disappear when unloaded.


## minecraft:trusting


Defines the rules for a mob to trust players.


| Name | Type | Default Value | Description |
| --- | --- | --- | --- |
| probability | Decimal | 1.00 | The chance of the entity trusting with each item use between 0.0 and 1.0, where 1.0 is 100%. |
| trust_event | String |  | Event to run when this entity becomes trusting. |
| trust_items | List |  | The list of items that can be used to get the entity to trust players. |


## minecraft:variable_max_auto_step


Entities with this component will have a maximum auto step height that is different depending on whether they are on a block that prevents jumping. Incompatible with "runtime_identifier": "minecraft:horse".


| Name | Type | Default Value | Description |
| --- | --- | --- | --- |
| base_value | Decimal | 0.5625 | The maximum auto step height when on any other block. |
| controlled_value | Decimal | 0.5625 | The maximum auto step height when on any other block and controlled by the player. |
| jump_prevented_value | Decimal | 0.5625 | The maximum auto step height when on a block that prevents jumping. |


## minecraft:vertical_movement_action


When configured as a rideable entity, the entity will move upwards or downwards when the player uses the jump action.


| Name | Type | Default Value | Description |
| --- | --- | --- | --- |
| vertical_velocity | Decimal | 0.5 | Vertical velocity to apply when jump action is issued. |


## minecraft:vibration_damper


Vibrations emitted by this entity will be ignored.


## minecraft:water_movement


| Name | Type | Default Value | Description |
| --- | --- | --- | --- |
| drag_factor | Decimal | 0.8 | Drag factor to determine movement speed when in water. |


# Entity Definition Properties


These properties are part of the Entity Definition as a whole and go before the Component or Component Groups. Make sure to place them before any Components, Component Groups or Events sections or they will be ignored.


## format_version


Specifies the version of the game this entity was made in. If the version is lower than the current version, any changes made to the entity in the vanilla version will be applied to it.


## Code Example


Example


```json
{
  "format_version": "1.8.0",
  "minecraft:entity": {
    "component_groups": {
      ...
    }
    ...
  }
}
```


# Entity Description Properties


This section defines properties required to identify and construct the entity


## Entity Description Properties


### identifier


Sets the name for this entity's description.


| Type | Default Value | Description |
| --- | --- | --- |
| String |  | The identifier for this entity |


### is_spawnable


Sets whether or not this entity has a spawn egg in the creative ui.


| Type | Default Value | Description |
| --- | --- | --- |
| Boolean | false | Set to determine if this entity has a spawn egg. |


### is_summonable


Sets whether or not we can summon this entity using commands such as /summon.


| Type | Default Value | Description |
| --- | --- | --- |
| Boolean | false | Flag to mark this entity as being summonable or not. |


### runtime_identifier


Sets the name for the Vanilla Minecraft identifier this entity will use to build itself from.


| Type | Default Value | Description |
| --- | --- | --- |
| String |  | The identifier for the class to construct this entity with |


### spawn_category


Sets the Spawn Category type of this entity. This entity will spawn with the rest of this Spawn Category type.


| Type | Default Value | Description |
| --- | --- | --- |
| String |  | The Spawn Category type of this entity, used to spawn the entity. |


## animations


Sets the mapping of internal animation references to actual animations. This is a JSON Object of name/animation pairs


| Name | Type | Default Value | Description |
| --- | --- | --- | --- |
| animation | String | none | Actual animation to use |
| name | String | none | Internal (to the entity) name of this animation entry |


## scripts


Sets the mapping of internal animation controller references to actual animation controller. This is a JSON Array of name/animation-controller pairs


| Name | Type | Default Value | Description |
| --- | --- | --- | --- |
| animation controller | String | none | Actual animation to use |
| name | String | none | Internal (to the entity) name of this animation entry |


# ID Lists


## AI Goals


| JSON Name | ID |
| --- | --- |
| minecraft:behavior.admire_item | 1024844406 |
| minecraft:behavior.avoid_block (See JSON Schema since 1.21.120) | 11673217032596464851 |
| minecraft:behavior.avoid_mob_type (See JSON Schema since 1.21.120) | 12619167115468661473 |
| minecraft:behavior.barter | 536670686 |
| minecraft:behavior.beg | 1346418048 |
| minecraft:behavior.break_door | -1798237626 |
| minecraft:behavior.breed | 51166360 |
| minecraft:behavior.celebrate | 3133100024050825239 |
| minecraft:behavior.celebrate_survive | 9921563176146616642 |
| minecraft:behavior.charge_attack (See JSON Schema since 1.21.100) | 17135737530272528235 |
| minecraft:behavior.charge_held_item | 9534738104831396526 |
| minecraft:behavior.circle_around_anchor (See JSON Schema since 1.21.100) | 10683034454299639524 |
| minecraft:behavior.controlled_by_player (See JSON Schema since 1.21.100) | 6293745006137368026 |
| minecraft:behavior.croak | 13695866816529218596 |
| minecraft:behavior.defend_trusted_target | 2070046390 |
| minecraft:behavior.defend_village_target | 5237560637228864639 |
| minecraft:behavior.delayed_attack | 16686103324935382281 |
| minecraft:behavior.dig (See JSON Schema since 1.21.120) | 1960392148256567422 |
| minecraft:behavior.door_interact | -286762735 |
| minecraft:behavior.dragonchargeplayer | 3000393824704693780 |
| minecraft:behavior.dragondeath | -1541162059 |
| minecraft:behavior.dragonflaming (See JSON Schema since 1.21.100) | 347128835287996895 |
| minecraft:behavior.dragonholdingpattern | 1066617362 |
| minecraft:behavior.dragonlanding | -152352492 |
| minecraft:behavior.dragonscanning | -1003941066 |
| minecraft:behavior.dragonstrafeplayer | 18364378208758607551 |
| minecraft:behavior.dragontakeoff | -1912535317 |
| minecraft:behavior.drink_milk (See JSON Schema since 1.21.120) | 14323617142891735146 |
| minecraft:behavior.drink_potion | 277029334 |
| minecraft:behavior.drop_item_for | 6288016308568137423 |
| minecraft:behavior.eat_block | 13717849383294281770 |
| minecraft:behavior.eat_carried_item | -184757575 |
| minecraft:behavior.eat_mob (See JSON Schema since 1.21.100) | 17437542916452098121 |
| minecraft:behavior.emerge | 12499259831953635711 |
| minecraft:behavior.enderman_leave_block | -717580550 |
| minecraft:behavior.enderman_take_block | -537294220 |
| minecraft:behavior.equip_item | 13771509599928214056 |
| minecraft:behavior.explore_outskirts | 1573426610843290210 |
| minecraft:behavior.fertilize_farm_block | 9206289212958280677 |
| minecraft:behavior.find_cover | -1231227755 |
| minecraft:behavior.find_mount | 731306871 |
| minecraft:behavior.find_underwater_treasure | -1602192311 |
| minecraft:behavior.fire_at_target | 15552728018745447502 |
| minecraft:behavior.flee_sun | -617664229 |
| minecraft:behavior.float | 1758503000 |
| minecraft:behavior.float_tempt | 1075595177 |
| minecraft:behavior.float_wander | -1122048364 |
| minecraft:behavior.follow_caravan | 9936402 |
| minecraft:behavior.follow_mob | -1524701626 |
| minecraft:behavior.follow_owner | 1213259599 |
| minecraft:behavior.follow_parent | 2127040136 |
| minecraft:behavior.follow_target_captain | -135480526 |
| minecraft:behavior.go_and_give_items_to_noteblock | 4654150191236239757 |
| minecraft:behavior.go_and_give_items_to_owner | 12205890323878762703 |
| minecraft:behavior.go_home | 16188843613883396158 |
| minecraft:behavior.guardian_attack | 10035257602683297598 |
| minecraft:behavior.harvest_farm_block | 10346419677401805698 |
| minecraft:behavior.hide | -1101331718 |
| minecraft:behavior.hold_ground | 1804286487 |
| minecraft:behavior.hurt_by_target | -1710404297 |
| minecraft:behavior.inspect_bookshelf | 40191182 |
| minecraft:behavior.investigate_suspicious_location | 17828766620487298865 |
| minecraft:behavior.jump_around_target | 11795366379540415834 |
| minecraft:behavior.jump_to_block | 9920654059703051752 |
| minecraft:behavior.knockback_roar | 10556128811593022910 |
| minecraft:behavior.lay_down | -2074934675 |
| minecraft:behavior.lay_egg | -1207740530 |
| minecraft:behavior.leap_at_target | 256926956 |
| minecraft:behavior.look_at_entity | -1735428573 |
| minecraft:behavior.look_at_player | -1892159379 |
| minecraft:behavior.look_at_target | -885375871 |
| minecraft:behavior.look_at_trading_player | -2094605693 |
| minecraft:behavior.make_love | -1551283431 |
| minecraft:behavior.melee_attack | 11509785650956819759 |
| minecraft:behavior.melee_box_attack | 16943510645914782909 |
| minecraft:behavior.mingle | 385775952 |
| minecraft:behavior.mount_pathing | 1112137677 |
| minecraft:behavior.move_around_target | 12172580866581071579 |
| minecraft:behavior.move_indoors | 15759011189030877060 |
| minecraft:behavior.move_outdoors | 13363481534821336549 |
| minecraft:behavior.move_through_village | 779377630 |
| minecraft:behavior.move_to_block | -342788833 |
| minecraft:behavior.move_to_land | -943551837 |
| minecraft:behavior.move_to_lava | -945036492 |
| minecraft:behavior.move_to_liquid | 635836098 |
| minecraft:behavior.move_to_poi | -1614682114 |
| minecraft:behavior.move_to_random_block | 395022171 |
| minecraft:behavior.move_to_village | -805189394 |
| minecraft:behavior.move_to_water | 1886139445 |
| minecraft:behavior.move_towards_dwelling_restriction | 4996579375027897212 |
| minecraft:behavior.move_towards_home_restriction | 6619321617752047371 |
| minecraft:behavior.move_towards_target | -166537884 |
| minecraft:behavior.nap | 992184933 |
| minecraft:behavior.nearest_attackable_target | 5824270165841165755 |
| minecraft:behavior.nearest_prioritized_attackable_target | 335414683 |
| minecraft:behavior.ocelot_sit_on_block | -1327999383 |
| minecraft:behavior.ocelotattack | 6119969836310458450 |
| minecraft:behavior.offer_flower | 14523794672066366898 |
| minecraft:behavior.open_door | -906200433 |
| minecraft:behavior.owner_hurt_by_target | 530667419 |
| minecraft:behavior.owner_hurt_target | 995655261 |
| minecraft:behavior.panic | -169886247 |
| minecraft:behavior.pet_sleep_with_owner | 1379392240 |
| minecraft:behavior.pickup_items | 644287189 |
| minecraft:behavior.place_block | 8805775981579988731 |
| minecraft:behavior.play | 9028661884418335188 |
| minecraft:behavior.play_dead | 15611122012431414539 |
| minecraft:behavior.player_ride_tamed | 603869698 |
| minecraft:behavior.raid_garden | -1145687602 |
| minecraft:behavior.ram_attack | 16987281751541937759 |
| minecraft:behavior.random_breach | 928528911 |
| minecraft:behavior.random_fly | -446385505 |
| minecraft:behavior.random_hover | -1797323808 |
| minecraft:behavior.random_look_around | 1177762851 |
| minecraft:behavior.random_look_around_and_sit | -1368070166 |
| minecraft:behavior.random_search_and_dig | 9274891347700307903 |
| minecraft:behavior.random_sitting | 1722716662 |
| minecraft:behavior.random_stroll | -1643504294 |
| minecraft:behavior.random_swim | -1465281278 |
| minecraft:behavior.ranged_attack | 17427060681688556526 |
| minecraft:behavior.receive_love | 1156694272 |
| minecraft:behavior.restrict_open_door | 715583988 |
| minecraft:behavior.restrict_sun | -382716017 |
| minecraft:behavior.rise_to_liquid_level | -950950337 |
| minecraft:behavior.roar (See JSON Schema since 1.21.110) | 955363010492591411 |
| minecraft:behavior.roll | 817471653 |
| minecraft:behavior.run_around_like_crazy | -1390363669 |
| minecraft:behavior.scared | -1643945926 |
| minecraft:behavior.send_event | -823429137 |
| minecraft:behavior.share_items | 2045906118 |
| minecraft:behavior.silverfish_merge_with_stone | 321908121 |
| minecraft:behavior.silverfish_wake_up_friends | 484391748 |
| minecraft:behavior.skeleton_horse_trap | 530952387 |
| minecraft:behavior.sleep | -1811763 |
| minecraft:behavior.slime_attack (See JSON Schema since 1.21.110) | 10551302716455748080 |
| minecraft:behavior.slime_float (See JSON Schema since 1.21.110) | 12028999845868286476 |
| minecraft:behavior.slime_keep_on_jumping (See JSON Schema since 1.21.110) | 7774985779375551490 |
| minecraft:behavior.slime_random_direction (See JSON Schema since 1.21.110) | 12283841380565759883 |
| minecraft:behavior.snacking | 875141064 |
| minecraft:behavior.sneeze | 1528584076 |
| minecraft:behavior.sniff (See JSON Schema since 1.21.110) | 5511693697264850991 |
| minecraft:behavior.sonic_boom (See JSON Schema since 1.21.110) | 17472690819733335393 |
| minecraft:behavior.squid_dive | 1626998843 |
| minecraft:behavior.squid_flee | 1304043261 |
| minecraft:behavior.squid_idle | 1741234679 |
| minecraft:behavior.squid_move_away_from_ground | 892387674 |
| minecraft:behavior.squid_out_of_water | -1347598607 |
| minecraft:behavior.stalk_and_pounce_on_target | -1248479202 |
| minecraft:behavior.stay_near_noteblock (See JSON Schema since 1.21.110) | 353733443286183369 |
| minecraft:behavior.stay_while_sitting | 1346980478 |
| minecraft:behavior.stomp_attack | 2494714009520253728 |
| minecraft:behavior.stomp_turtle_egg | 851839416 |
| minecraft:behavior.stroll_towards_village | -1563831906 |
| minecraft:behavior.summon_entity | -2005850647 |
| minecraft:behavior.swell | -1473132493 |
| minecraft:behavior.swim_idle (See JSON Schema since 1.21.110) | 454794286552255810 |
| minecraft:behavior.swim_up_for_breath (See JSON Schema since 1.21.110) | 7965902647256278910 |
| minecraft:behavior.swim_wander (See JSON Schema since 1.21.110) | 13632521305782430415 |
| minecraft:behavior.swim_with_entity | 10204691344094313319 |
| minecraft:behavior.swoop_attack (See JSON Schema since 1.21.110) | 6088369849583684812 |
| minecraft:behavior.take_block | 6937905075348270719 |
| minecraft:behavior.take_flower | 16066043784359778497 |
| minecraft:behavior.teleport_to_owner | 1980014114121577323 |
| minecraft:behavior.tempt | 422904556 |
| minecraft:behavior.timer_flag_1 | 9107193206350859182 |
| minecraft:behavior.timer_flag_2 | 9107192106839230971 |
| minecraft:behavior.timer_flag_3 | 9107191007327602760 |
| minecraft:behavior.trade_interest | -574851053 |
| minecraft:behavior.trade_with_player | 13082946109325010651 |
| minecraft:behavior.vex_copy_owner_target | -643459543 |
| minecraft:behavior.vex_random_move | -1117822165 |
| minecraft:behavior.wither_random_attack_pos_goal | -528895885 |
| minecraft:behavior.wither_target_highest_damage | -1065562072 |
| minecraft:behavior.work | 11642413332579264431 |
| minecraft:behavior.work_composter | 8770872254111982548 |


## Attributes


| JSON Name | ID |
| --- | --- |
| minecraft:attack | 1651346034 |
| minecraft:spell_effects | 523264365 |
| minecraft:strength | 648564399 |


## Components


| JSON Name | ID |
| --- | --- |
|  | -2078137563 |
| minecraft:addrider | -1014528905 |
| minecraft:admire_item | -1838165406 |
| minecraft:ageable | 1092934985 |
| minecraft:anger_level | 500775548 |
| minecraft:angry | 725411499 |
| minecraft:annotation.break_door | 2116838963 |
| minecraft:annotation.open_door | 1004733768 |
| minecraft:area_attack | 1169114880 |
| minecraft:attack_cooldown | -1439360398 |
| minecraft:barter | -1660828566 |
| minecraft:block_climber | -505801518 |
| minecraft:block_sensor | 1944169706 |
| minecraft:boostable | -8316315 |
| minecraft:boss | -1309638025 |
| minecraft:break_blocks | -944056304 |
| minecraft:breathable | -707187232 |
| minecraft:breedable | -1616430100 |
| minecraft:bribeable | -2125526908 |
| minecraft:buoyant | -467213736 |
| minecraft:burns_in_daylight | 1256874605 |
| minecraft:can_join_raid | -1847990870 |
| minecraft:celebrate_hunt | -789611239 |
| minecraft:collision_box | -1277663508 |
| minecraft:combat_regeneration | -315365016 |
| minecraft:conditional_bandwidth_optimization | 1333085468 |
| minecraft:custom_hit_test | 2035968892 |
| minecraft:damage_over_time | -656420526 |
| minecraft:damage_sensor | -82616534 |
| minecraft:dash_action | -1446083519 |
| minecraft:despawn | 1674909940 |
| minecraft:dimension_bound | -1843592159 |
| minecraft:drying_out_timer | 2113510784 |
| minecraft:economy_trade_table | 1705466896 |
| minecraft:entity_armor_equipment_slot_mapping | 388041038 |
| minecraft:entity_sensor | -406052184 |
| minecraft:environment_sensor | 687748970 |
| minecraft:equip_item | -1774220620 |
| minecraft:equippable | -1164142226 |
| minecraft:exhaustion_values | 1522147793 |
| minecraft:experience_reward | -1992597900 |
| minecraft:explode | -1683058581 |
| minecraft:flocking | 1967597361 |
| minecraft:free_camera_controlled | -1826251563 |
| minecraft:game_event_movement_tracking | 763815151 |
| minecraft:genetics | -82484670 |
| minecraft:giveable | 1739199795 |
| minecraft:group_size | -1997375941 |
| minecraft:grows_crop | 43636353 |
| minecraft:healable | -54247424 |
| minecraft:heartbeat | 2102942906 |
| minecraft:home | 610829097 |
| minecraft:hurt_on_condition | 163715083 |
| minecraft:inside_block_notifier | -377593253 |
| minecraft:insomnia | -300455606 |
| minecraft:instant_despawn | -634288138 |
| minecraft:interact | -1996861528 |
| minecraft:inventory | 1941951218 |
| minecraft:item_hopper | -447601772 |
| minecraft:jump.dynamic | 945664737 |
| minecraft:jump.static | 1064987526 |
| minecraft:leashable | 893445039 |
| minecraft:leashable_to | 1636924761 |
| minecraft:looked_at | -1189178052 |
| minecraft:managed_wandering_trader | 846609640 |
| minecraft:mob_effect | 490430596 |
| minecraft:mob_effect_immunity | 1589547531 |
| minecraft:movement.amphibious | -500784850 |
| minecraft:movement.basic | 3354475 |
| minecraft:movement.fly | -1487316136 |
| minecraft:movement.generic | -1363369868 |
| minecraft:movement.hover | 1743805051 |
| minecraft:movement.jump | 1256268727 |
| minecraft:movement.skip | -1828832272 |
| minecraft:movement.sway | 2126249079 |
| minecraft:nameable | -595398763 |
| minecraft:navigation.climb | -1075360945 |
| minecraft:navigation.float | -842998036 |
| minecraft:navigation.fly | -1997823129 |
| minecraft:navigation.generic | 1768002583 |
| minecraft:navigation.hover | -769247640 |
| minecraft:navigation.swim | 1908076634 |
| minecraft:navigation.walk | 1909033775 |
| minecraft:npc | 1140005505 |
| minecraft:out_of_control | -639930856 |
| minecraft:peek | -863721039 |
| minecraft:persistent | 1435858387 |
| minecraft:physics | 1021552959 |
| minecraft:preferred_path | 1431188951 |
| minecraft:projectile | 1922456869 |
| minecraft:pushable | -1784639134 |
| minecraft:raid_trigger | 2093209329 |
| minecraft:rail_movement | -78235634 |
| minecraft:rail_sensor | 1273161273 |
| minecraft:ravager_blocked | -1417579541 |
| minecraft:reflect_projectiles | -2073986952 |
| minecraft:rideable | 1656173828 |
| minecraft:scale_by_age | 64881972 |
| minecraft:scheduler | -684492525 |
| minecraft:shareables | 1301057082 |
| minecraft:shooter | 184150118 |
| minecraft:sittable | -1376274106 |
| minecraft:spawn_entity | 1991964333 |
| minecraft:suspect_tracking | 690744457 |
| minecraft:tameable | 932870003 |
| minecraft:tamemount | 1615660288 |
| minecraft:target_nearby_sensor | 1309819882 |
| minecraft:teleport | -1900891127 |
| minecraft:timer | 1862095863 |
| minecraft:trade_table | 50480315 |
| minecraft:trail | 1632590434 |
| minecraft:transformation | -457894577 |
| minecraft:transient | -413020544 |
| minecraft:trusting | 936499892 |
| minecraft:variable_max_auto_step | -368684982 |
| minecraft:vertical_movement_action | 2076572763 |
| minecraft:vibration_damper | 1836172792 |
| minecraft:vibration_listener | 1844567337 |
| minecraft:water_movement | -128701925 |


## Entity Description Properties


| JSON Name | ID |
| --- | --- |
| animations | 1232311658 |
| scripts | -76244517 |


## Properties


| JSON Name | ID |
| --- | --- |
| minecraft:ambient_sound_interval | -1314051310 |
| minecraft:body_rotation_always_follows_head | 1766999981 |
| minecraft:body_rotation_blocked | -31742338 |
| minecraft:can_climb | -550459594 |
| minecraft:can_fly | 985724318 |
| minecraft:can_power_jump | -763124853 |
| minecraft:cannot_be_attacked | 1363084217 |
| minecraft:color | 1069623177 |
| minecraft:color2 | 1429635777 |
| minecraft:default_look_angle | -815556357 |
| minecraft:equipment | 714661022 |
| minecraft:fire_immune | -1071767182 |
| minecraft:floats_in_liquid | 179778474 |
| minecraft:flying_speed | -570508595 |
| minecraft:friction_modifier | -714364556 |
| minecraft:ground_offset | -1816881803 |
| minecraft:ignore_cannot_be_attacked | 2139692418 |
| minecraft:input_ground_controlled | -646415617 |
| minecraft:is_baby | 1853172763 |
| minecraft:is_charged | 125340337 |
| minecraft:is_chested | -817599379 |
| minecraft:is_collidable | 1703955772 |
| minecraft:is_dyeable | 1381448355 |
| minecraft:is_hidden_when_invisible | -1553744 |
| minecraft:is_ignited | 905710223 |
| minecraft:is_illager_captain | 921912694 |
| minecraft:is_pregnant | -1253700046 |
| minecraft:is_saddled | 1037901616 |
| minecraft:is_shaking | 523951884 |
| minecraft:is_sheared | 1154836813 |
| minecraft:is_stackable | 1184456737 |
| minecraft:is_stunned | 1818977510 |
| minecraft:is_tamed | 1646421078 |
| minecraft:item_controllable | 1868311069 |
| minecraft:loot | 1775814032 |
| minecraft:mark_variant | 663621689 |
| minecraft:movement_sound_distance_offset | 124679519 |
| minecraft:push_through | -138030768 |
| minecraft:renders_when_invisible | -1395968564 |
| minecraft:rotation_axis_aligned | -882714065 |
| minecraft:scale | 77854436 |
| minecraft:skin_id | 1838729593 |
| minecraft:sound_volume | 937951776 |
| minecraft:type_family | -255011285 |
| minecraft:variant | -1789583977 |
| minecraft:walk_animation_speed | 972393614 |
| minecraft:wants_jockey | 1832515141 |


## Triggers


| JSON Name | ID |
| --- | --- |
| minecraft:on_death | -49357854 |
| minecraft:on_friendly_anger | -1009986313 |
| minecraft:on_hurt | -1028603471 |
| minecraft:on_hurt_by_player | -599009831 |
| minecraft:on_ignite | -1911489054 |
| minecraft:on_start_landing | -555648368 |
| minecraft:on_start_takeoff | 426083399 |
| minecraft:on_target_acquired | 1063277906 |
| minecraft:on_target_escape | -1373130027 |
| minecraft:on_wake_with_owner | -1543219003 |


# Properties


## minecraft:ambient_sound_interval


Sets the entity's delay between playing its ambient sound.


| Name | Type | Default Value | Description |
| --- | --- | --- | --- |
| event_name | String | ambient | Level sound event to be played as the ambient sound. |
| event_names | Array |  | List of dynamic level sound events, with conditions for choosing between them. Evaluated in order, first one wins. If none evaluate to true, 'event_name' will take precedence.condition The condition that must be satisfied to select the given ambient sound event_name Level sound event to be played as the ambient sound |
| range | Decimal | 16.000000 | Maximum time in seconds to randomly add to the ambient sound delay time. |
| value | Decimal | 8.000000 | Minimum time in seconds before the entity plays its ambient sound again. |


## minecraft:body_rotation_always_follows_head


Causes the entity's body to always be automatically rotated to align with the entity's head.


Does not override the "minecraft:body_rotation_blocked" component.


## minecraft:body_rotation_blocked


When set, the entity will no longer visually rotate their body to match their facing direction.


## minecraft:can_climb


Allows this entity to climb up ladders.


## minecraft:can_fly


Marks the entity as being able to fly, the pathfinder won't be restricted to paths where a solid block is required underneath it.


## minecraft:can_power_jump


Allows the entity to power jump like the Horse does in Vanilla.


## minecraft:cannot_be_attacked


When set, blocks entities from attacking the owner entity unless they have the "minecraft:ignore_cannot_be_attacked" component.


## minecraft:color


Defines the entity's color. Only works on vanilla entities that have predefined color values (sheep, llama, shulker).


| Name | Type | Default Value | Description |
| --- | --- | --- | --- |
| value | Integer | 0 | The Palette Color value of the entity. |


## minecraft:color2


Defines the entity's second texture color. Only works on vanilla entities that have a second predefined color values (tropical fish).


| Name | Type | Default Value | Description |
| --- | --- | --- | --- |
| value | Integer | 0 | The second Palette Color value of the entity. |


## minecraft:default_look_angle


Sets this entity's default head rotation angle.


| Name | Type | Default Value | Description |
| --- | --- | --- | --- |
| value | Decimal | 0.0f | Angle in degrees. |


## minecraft:equipment


Sets the Equipment table to use for this Entity.


| Name | Type | Default Value | Description |
| --- | --- | --- | --- |
| slot_drop_chance | List |  | A list of slots with the chance to drop an equipped item from that slot. |
| table | String |  | The file path to the equipment table, relative to the behavior pack's root. |


## minecraft:fire_immune


Sets that this entity doesn't take damage from fire.


## minecraft:floats_in_liquid


Sets that this entity can float in liquid blocks.


## minecraft:flying_speed


Speed in Blocks that this entity flies at.


| Name | Type | Default Value | Description |
| --- | --- | --- | --- |
| value | Decimal | 0.02 | Flying speed in blocks per tick. |


## minecraft:friction_modifier


Defines how much friction affects this entity.


| Name | Type | Default Value | Description |
| --- | --- | --- | --- |
| value | Decimal | 1.0 | The higher the number, the more the friction affects this entity. A value of 1.0 means regular friction, while 2.0 means twice as much. |


## minecraft:ground_offset


Sets the offset from the ground that the entity is actually at.


| Name | Type | Default Value | Description |
| --- | --- | --- | --- |
| value | Decimal | 0.0 | The value of the entity's offset from the terrain, in blocks. |


## minecraft:ignore_cannot_be_attacked


When set, blocks entities from attacking the owner entity unless they have the "minecraft:ignore_cannot_be_attacked" component.


| Name | Type | Default Value | Description |
| --- | --- | --- | --- |
| filters | Minecraft Filter |  | Defines which entities are exceptions and are allowed to be attacked by the owner entity, potentially attacked entity is subject "other". If this is not specified then all attacks by the owner are allowed. |


## minecraft:input_ground_controlled


When configured as a rideable entity, the entity will be controlled using WASD controls. Beginning with 1.19.50 the default auto step height for rideable entities is half a block. Consider adding the "minecraft:variable_max_auto_step" component to increase it.


## minecraft:is_baby


Sets that this entity is a baby.


## minecraft:is_charged


Sets that this entity is charged.


## minecraft:is_chested


Sets that this entity is currently carrying a chest.


## minecraft:is_collidable


Allows other mobs to have vertical and horizontal collisions with this mob. For a collision to occur, both mobs must have a "minecraft:collision_box" component. This component can only be used on mobs and enables collisions exclusively between mobs.


Please note that this type of collision is unreliable for moving collidable mobs. It is recommended to use this component only in scenarios where the collidable mob remains stationary.


Collidable behavior is closely related to stackable behavior. While the "minecraft:is_collidable" component governs how other mobs interact with the component's owner, the "minecraft:is_stackable" component describes how an entity interacts with others of its own kind.


## minecraft:is_dyeable


Allows dyes to be used on this entity to change its color.


| Name | Type | Default Value | Description |
| --- | --- | --- | --- |
| interact_text | String |  | The text that will display when interacting with this entity with a dye when playing with Touch-screen controls. |


## minecraft:is_hidden_when_invisible


Sets that this entity can hide from hostile mobs while invisible.


## minecraft:is_ignited


Sets that this entity is currently on fire.


## minecraft:is_illager_captain


Sets that this entity is an Illager Captain.


## minecraft:is_pregnant


Sets that this entity is currently pregnant.


## minecraft:is_saddled


Sets that this entity is currently saddled.


## minecraft:is_shaking


Sets that this entity is currently shaking.


## minecraft:is_sheared


Sets that this entity is currently sheared.


## minecraft:is_stackable


Allows instances of this entity to have vertical and horizontal collisions with each other. For a collision to occur, both instances must have a "minecraft:collision_box" component.


Stackable behavior is closely related to collidable behavior. While the "minecraft:is_stackable" component describes how an entity interacts with others of its own kind, the "minecraft:is_collidable" component governs how other mobs interact with the component's owner.


## minecraft:is_stunned


Sets that this entity is currently stunned.


## minecraft:is_tamed


Sets that this entity is currently tamed.


## minecraft:item_controllable


Defines what items can be used to control this entity while ridden.


| Name | Type | Default Value | Description |
| --- | --- | --- | --- |
| control_items | List |  | List of items that can be used to control this entity. |


## minecraft:loot


Sets the loot table for what items this entity drops upon death.


| Name | Type | Default Value | Description |
| --- | --- | --- | --- |
| table | String |  | The path to the loot table, relative to the Behavior Pack's root. |


## minecraft:mark_variant


Additional variant value. Can be used to further differentiate variants.


| Name | Type | Default Value | Description |
| --- | --- | --- | --- |
| value | Integer | 0 | The ID of the variant. By convention, 0 is the ID of the base entity. |


## minecraft:movement_sound_distance_offset


Sets the offset used to determine the next step distance for playing a movement sound.


| Name | Type | Default Value | Description |
| --- | --- | --- | --- |
| value | Decimal | 1.0 | The higher the number, the less often the movement sound will be played. |


## minecraft:push_through


Sets the distance through which the entity can push through.


| Name | Type | Default Value | Description |
| --- | --- | --- | --- |
| value | Decimal | 0.0 | The value of the entity's push-through, in blocks. |


## minecraft:renders_when_invisible


When set, the entity will render even when invisible. Appropriate rendering behavior can then be specified in the corresponding "minecraft:client_entity".


## minecraft:rotation_axis_aligned


Causes the entity to automatically rotate to align with the nearest cardinal direction based on its current facing direction.


Combining this with the "minecraft:body_rotation_blocked" component will cause the entity's body to align with the nearest cardinal direction and remain fixed in that orientation, regardless of changes in its facing direction.


## minecraft:scale


Sets the entity's visual size.


| Name | Type | Default Value | Description |
| --- | --- | --- | --- |
| value | Decimal | 1.0 | The value of the scale. 1.0 means the entity will appear at the scale they are defined in their model. Higher numbers make the entity bigger. |


## minecraft:skin_id


Skin ID value. Can be used to differentiate skins, such as base skins for villagers.


| Name | Type | Default Value | Description |
| --- | --- | --- | --- |
| value | Integer | 0 | The ID of the skin. By convention, 0 is the ID of the base skin. |


## minecraft:sound_volume


Sets the entity's base volume for sound effects.


| Name | Type | Default Value | Description |
| --- | --- | --- | --- |
| value | Decimal | 1.0 | The value of the volume the entity uses for sound effects. |


## minecraft:type_family


Defines the families this entity belongs to.


| Name | Type | Default Value | Description |
| --- | --- | --- | --- |
| family | List |  | List of family names. |


## minecraft:variant


Used to differentiate the component group of a variant of an entity from others. (e.g. ocelot, villager)


| Name | Type | Default Value | Description |
| --- | --- | --- | --- |
| value | Integer | 0 | The ID of the variant. By convention, 0 is the ID of the base entity. |


## minecraft:walk_animation_speed


Sets the speed multiplier for this entity's walk animation speed.


| Name | Type | Default Value | Description |
| --- | --- | --- | --- |
| value | Decimal | 1.0 | The higher the number, the faster the animation for walking plays. A value of 1.0 means normal speed, while 2.0 means twice as fast. |


## minecraft:wants_jockey


Sets that this entity wants to become a jockey.


# Triggers


## minecraft:on_death


Adds a trigger to call on this entity's death.


| Name | Type | Default Value | Description |
| --- | --- | --- | --- |
| event | String |  | The event to run when the conditions for this trigger are met. |
| filters | Minecraft Filter |  | The list of conditions for this trigger to execute. |
| target | String | self | The target of the event. |


## minecraft:on_friendly_anger


Adds a trigger that will run when a nearby entity of the same type as this entity becomes Angry.


| Name | Type | Default Value | Description |
| --- | --- | --- | --- |
| event | String |  | The event to run when the conditions for this trigger are met. |
| filters | Minecraft Filter |  | The list of conditions for this trigger to execute. |
| target | String | self | The target of the event. |


## minecraft:on_hurt


Adds a trigger to call when this entity takes damage.


| Name | Type | Default Value | Description |
| --- | --- | --- | --- |
| event | String |  | The event to run when the conditions for this trigger are met. |
| filters | Minecraft Filter |  | The list of conditions for this trigger to execute. |
| target | String | self | The target of the event. |


## minecraft:on_hurt_by_player


Adds a trigger to call when this entity is attacked by the player.


| Name | Type | Default Value | Description |
| --- | --- | --- | --- |
| event | String |  | The event to run when the conditions for this trigger are met. |
| filters | Minecraft Filter |  | The list of conditions for this trigger to execute. |
| target | String | self | The target of the event. |


## minecraft:on_ignite


Adds a trigger to call when this entity is set on fire.


| Name | Type | Default Value | Description |
| --- | --- | --- | --- |
| event | String |  | The event to run when the conditions for this trigger are met. |
| filters | Minecraft Filter |  | The list of conditions for this trigger to execute. |
| target | String | self | The target of the event. |


## minecraft:on_start_landing


Only usable by the Ender Dragon. Adds a trigger to call when this entity lands.


| Name | Type | Default Value | Description |
| --- | --- | --- | --- |
| event | String |  | The event to run when the conditions for this trigger are met. |
| filters | Minecraft Filter |  | The list of conditions for this trigger to execute. |
| target | String | self | The target of the event. |


## minecraft:on_start_takeoff


Only usable by the Ender Dragon. Adds a trigger to call when this entity starts flying.


| Name | Type | Default Value | Description |
| --- | --- | --- | --- |
| event | String |  | The event to run when the conditions for this trigger are met. |
| filters | Minecraft Filter |  | The list of conditions for this trigger to execute. |
| target | String | self | The target of the event. |


## minecraft:on_target_acquired


Adds a trigger to call when this entity finds a target.


| Name | Type | Default Value | Description |
| --- | --- | --- | --- |
| event | String |  | The event to run when the conditions for this trigger are met. |
| filters | Minecraft Filter |  | The list of conditions for this trigger to execute. |
| target | String | self | The target of the event. |


## minecraft:on_target_escape


Adds a trigger to call when this entity loses the target it currently has.


| Name | Type | Default Value | Description |
| --- | --- | --- | --- |
| event | String |  | The event to run when the conditions for this trigger are met. |
| filters | Minecraft Filter |  | The list of conditions for this trigger to execute. |
| target | String | self | The target of the event. |


## minecraft:on_wake_with_owner


Adds a trigger to call when this pet's owner awakes after sleeping with the pet.


| Name | Type | Default Value | Description |
| --- | --- | --- | --- |
| event | String |  | The event to run when the conditions for this trigger are met. |
| filters | Minecraft Filter |  | The list of conditions for this trigger to execute. |
| target | String | self | The target of the event. |
