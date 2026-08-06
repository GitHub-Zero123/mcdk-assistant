# BLOCKS DOCUMENTATION Version: 1.21.120.4


## Index


# Blocks


# Block Components


| Name | Type | Default Value | Description |
| --- | --- | --- | --- |
| minecraft:breathability | String |  | Determines whether the block is breathable by defining if the block is treated as a `solid` or as `air`. The default is `solid` if this component is omitted. |
| minecraft:collision_box | Boolean / JSON Object | true | Defines the area of the block that collides with entities. If set to true, default values are used. If set to false, the block's collision with entities is disabled. If this component is omitted, default values are used. |
| minecraft:crafting_table | JSON Object |  | Makes your block into a custom crafting table which enables the crafting table UI and the ability to craft recipes. This component supports only "recipe_shaped" and "recipe_shapeless" typed recipes and not others like "recipe_furnace" or "recipe_brewing_mix". If there are two recipes for one item, the recipe book will pick the first that was parsed. If two input recipes are the same, crafting may assert and the resulting item may vary.crafting_tags Defines the tags recipes should define to be crafted on this table. Limited to 64 tags. Each tag is limited to 64 characters. table_name Specifies the language file key that maps to what text will be displayed in the UI of this table. If the string given can not be resolved as a loc string, the raw string given will be displayed. If this field is omitted, the name displayed will default to the name specified in the "display_name" component. If this block has no "display_name" component, the name displayed will default to the name of the block. |
| minecraft:custom_components | Array |  | Sets an ordered list of custom component names which are bound in script to be executed upon a block event. |
| minecraft:destructible_by_explosion | Boolean / JSON Object | true | Describes the destructible by explosion properties for this block. If set to true, the block will have the default explosion resistance. If set to false, this block is indestructible by explosion. If the component is omitted, the block will have the default explosion resistance. |
| minecraft:destructible_by_mining | Boolean / JSON Object | true | Describes the destructible by mining properties for this block. If set to true, the block will take the default number of seconds to destroy. If set to false, this block is indestructible by mining. If the component is omitted, the block will take the default number of seconds to destroy. |
| minecraft:destruction_particles | JSON Object |  | Sets the particles that will be used when block is destroyed. This component can be omitted.particle_count Optional, number of particles to spawn of destruction. Default is 100, maximum is 255 inclusively texture The texture name used for the particle. tint_method Optional, tint multiplied to the color. Tint method logic varies, but often refers to the "rain" and "temperature" of the biome the block is placed in to compute the tint. Supported tint methods are "none", "default_foliage", "birch_foliage", "evergreen_foliage", "dry_foliage", "grass" and "water" |
| minecraft:display_name | Localization String |  | Specifies the language file key that maps to what text will be displayed when you hover over the block in your inventory and hotbar. If the string given can not be resolved as a loc string, the raw string given will be displayed. If this component is omitted, the name of the block will be used as the display name. |
| minecraft:flammable | Boolean / JSON Object |  | Describes the flammable properties for this block. If set to true, default values are used. If set to false, or if this component is omitted, the block will not be able to catch on fire naturally from neighbors, but it can still be directly ignited. |
| minecraft:friction | Decimal | 0.4 | Describes the friction for this block in a range of (0.0-0.9). Friction affects an entity's movement speed when it travels on the block. Greater value results in more friction. |
| minecraft:geometry | String / JSON Object |  | The description identifier of the geometry to use to render this block. This identifier must either match an existing geometry identifier in any of the loaded resource packs or be one of the currently supported Vanilla identifiers: "minecraft:geometry.full_block" or "minecraft:geometry.cross".As an object The component can be defined as an object to gain control over more specific behaviors. |
| minecraft:item_visual | JSON Object |  | The description identifier of the geometry and material used to render the item of this block.Experimental toggles required: Upcoming Creator Features (in format versions before 1.21.50)geometry [Required] The "minecraft:geometry" component that will be used for the item. material_instances [Required] The "minecraft:material_instances" component that will be used for the item. |
| minecraft:light_dampening | Integer | 15 | The amount that light will be dampened when it passes through the block, in a range (0-15). Higher value means the light will be dampened more. |
| minecraft:light_emission | Integer | 0 | The amount of light this block will emit in a range (0-15). Higher value means more light will be emitted. |
| minecraft:liquid_detection | JSON Object |  | The definitions for how a block behaves when detecting liquid. Only one rule definition is allowed per liquid type - if multiple are specified, the first will be used and the rest will be ignored.Experimental toggles required: Upcoming Creator Features (in format versions before 1.21.60) |
| minecraft:loot | Path String |  | The path to the loot table, relative to the behavior pack. Path string is limited to 256 characters. |
| minecraft:map_color | JSON Object |  | Sets the color of the block when rendered to a map. If this component is omitted, the block will not show up on the map.color The color is represented as a hex value in the format "#RRGGBB". May also be expressed as an array of [R, G, B] from 0 to 255. tint_method Optional, tint multiplied to the color. Tint method logic varies, but often refers to the "rain" and "temperature" of the biome the block is placed in to compute the tint. Supported tint methods are "none", "default_foliage", "birch_foliage", "evergreen_foliage", "dry_foliage", "grass" and "water" |
| minecraft:material_instances | JSON Object |  | The material instances for a block. Maps face or material_instance names in a geometry file to an actual material instance. You can assign a material instance object to any of these faces: "up", "down", "north", "south", "east", "west", or "*". You can also give an instance the name of your choosing such as "my_instance", and then assign it to a face by doing "north":"my_instance".Material Instance A material instance definition to map to a material instance in a geometry file. The material instance "*" will be used for any materials that don't have a match. |
| minecraft:movable | JSON Object |  | The description identifier of the movable componentExperimental toggles required: Upcoming Creator Features (in format versions before 1.21.100)movement_type [Required] How the block reacts to being pushed by another block like a piston. Must be one of the following options:"push_pull" - The default value for this field. The block will be pushed and pulled by a piston."push" - The block will only be pulled by a piston and will ignore a sticky piston."popped" - The block is destroyed when moved by a piston."immovable" - The block is unaffected by a piston. sticky [Optional] How the block should handle adjacent blocks around it when being pushed by another block like a piston. Must be one of the following options:"same" - Adjacent blocks to this block will be moved when moved. This excludes other blocks with the "same" property. This will only work with the movement_type: "push_pull"."none" - The default and will not move adjacent blocks. |
| minecraft:placement_filter | JSON Object |  | Sets rules for under what conditions the block can be placed/surviveconditions List of conditions where the block can be placed/survive. Limited to 64 conditions. |
| minecraft:precipitation_interactions | JSON Object |  | Component that determines how the block will interact with rain and snowprecipitation_behavior What behavior should the block have. Three possible values: obstruct_rain, obstruct_rain_accumulate_snow and none |
| minecraft:queued_ticking | JSON Object |  | Triggers the specified event, either once, or at a regular interval equal to a number of ticks randomly chosen from the interval_range provided. |
| minecraft:random_ticking | JSON Object |  | Triggers the specified event randomly based on the random tick speed gamerule. The random tick speed determines how often blocks are updated. Some other examples of game mechanics that use random ticking are crop growth and fire spreading. |
| minecraft:redstone_conductivity | JSON Object |  | The basic redstone properties of a block; if the component is not provided the default values are used. Requires 1.21.30 format version and above.Experimental toggles required: Upcoming Creator Features (in format versions before 1.21.30) |
| minecraft:replaceable | JSON Object |  | A block with this component can be replaced when another block is placed in the same block position.Experimental toggles required: Upcoming Creator Features (in format versions before 1.21.60) |
| minecraft:selection_box | Boolean / JSON Object |  | Defines the area of the block that is selected by the player's cursor. If set to true, default values are used. If set to false, this block is not selectable by the player's cursor. If this component is omitted, default values are used. |
| minecraft:transformation | JSON Object |  | The block's translation, rotation and scale with respect to the center of its world positionrotation The block's rotation in increments of 90 degrees rotation_pivot The point to apply rotation around scale The block's scale scale_pivot The point to apply scale around translation The block's translation |
| minecraft:unit_cube | JSON Object |  | Specifies that a unit cube is to be used with tessellation. |


> **minecraft:collision_box** 子参数


| Name | Type | Default Value | Description |
| --- | --- | --- | --- |
| origin | Vector [a, b, c] | [-8.0, 0.0, -8.0] | Minimal position of the bounds of the collision box. "origin" is specified as [x, y, z] and must be in the range (-8, 0, -8) to (8, 16, 8), inclusive. |
| size | Vector [a, b, c] | [16.0, 16.0, 16.0] | Size of each side of the collision box. Size is specified as [x, y, z]. "origin" + "size" must be in the range (-8, 0, -8) to (8, 16, 8), inclusive. |


> **minecraft:destructible_by_explosion** 子参数


| Name | Type | Default Value | Description |
| --- | --- | --- | --- |
| explosion_resistance | Decimal | 0 | Sets the explosion resistance for the block. Greater values result in greater resistance to explosions. The scale will be different for different explosion power levels. A negative value or 0 means it will easily explode; larger numbers increase level of resistance. |


> **minecraft:destructible_by_mining** 子参数


| Name | Type | Default Value | Description |
| --- | --- | --- | --- |
| item_specific_speeds | Array |  | Optional array of objects to describe item specific block destroy speeds, each object contains an 'item' ItemDescriptor and a 'destroy_speed' float. This array currently requires UpcomingFeatures experiment to be enabled.destroy_speed Required.A destroy speed applied while using the defined 'item'. item Required.An ItemDescriptor filtering for the item used while mining. |
| seconds_to_destroy | Decimal | 0.0 | Sets the number of seconds it takes to destroy the block with base equipment. Greater numbers result in greater mining times. |


> **minecraft:flammable** 子参数


| Name | Type | Default Value | Description |
| --- | --- | --- | --- |
| catch_chance_modifier | Integer | 5 | A modifier affecting the chance that this block will catch flame when next to a fire. Values are greater than or equal to 0, with a higher number meaning more likely to catch on fire. For a "catch_chance_modifier" greater than 0, the fire will continue to burn until the block is destroyed (or it will burn forever if the "destroy_chance_modifier" is 0). If the "catch_chance_modifier" is 0, and the block is directly ignited, the fire will eventually burn out without destroying the block (or it will have a chance to be destroyed if "destroy_chance_modifier" is greater than 0). The default value of 5 is the same as that of Planks. |
| destroy_chance_modifier | Integer | 20 | A modifier affecting the chance that this block will be destroyed by flames when on fire. Values are greater than or equal to 0, with a higher number meaning more likely to be destroyed by fire. For a "destroy_chance_modifier" of 0, the block will never be destroyed by fire, and the fire will burn forever if the "catch_chance_modifier" is greater than 0. The default value of 20 is the same as that of Planks. |


> **minecraft:geometry** 子参数


| Name | Type | Default Value | Description |
| --- | --- | --- | --- |
| bone_visibility | JSON Object |  | A JSON object that contains a list of key/value pairs that map from bone name in the specified geometry file (key) to a boolean that tells whether the bone should be visible or not (value). |
| culling | String |  | Which block_culling .json file to use when rendering this block. |
| culling_layer | String | minecraft:culling_layer.undefined | A string that allows culling rule to group multiple blocks together when comparing them. When using the minecraft namespace, the only allowed culling layer identifiers are : "minecraft:culling_layer.undefined" or "minecraft:culling_layer.leaves". When using no namespaces or a custom one, the names must start and end with an alpha-numeric character. |
| identifier | String |  | The description identifier of the geometry to use to render this block. |
| uv_lock | Boolean / String Array |  | A Boolean locking UV orientation of all bones in the geometry, or an array of strings locking UV orientation of specific bones in the geometry. For performance reasons it is recommended to use the Boolean. Note that for cubes using Box UVs, rather than Per-face UVs, 'uv_lock' is only supported if the cube faces are square. |


> **minecraft:liquid_detection** 子参数


| Name | Type | Default Value | Description |
| --- | --- | --- | --- |
| can_contain_liquid | Boolean | false | Whether this block can contain the liquid. For example, if the liquid type is `water`, this means the block can be waterlogged. |
| liquid_type | String | water | The type of liquid this detection rule is for. Currently, `water` is the only supported liquid type. If this field is omitted, `water` will be the liquid type by default. |
| on_liquid_touches | String | blocking | How the block reacts to flowing water. Must be one of the following options:"blocking" - The default value for this field. The block stops the liquid from flowing."broken" - The block is destroyed completely."popped" - The block is destroyed and its item is spawned."no_reaction" - The block is unaffected; visually, the liquid will flow through the block. |
| stops_liquid_flowing_from_direction | Array |  | When a block contains a liquid, controls the directions in which the liquid can't flow out from the block. Also controls the directions in which a block can stop liquid flowing into it if `no_reaction` is set for the `on_liquid_touches` field. Can be a list of the following directions: "up", "down", "north", "south", "east", "west". The default is an empty list; this means that liquid can flow out of all directions by default. |


> **minecraft:material_instances** 子参数


| Name | Type | Default Value | Description |
| --- | --- | --- | --- |
| alpha_masked_tint | Boolean | false | Should the tint be multiplied by the alpha channel of texture (similar to grass_block's sides)? Requires 'format_version' >= 1.21.120, a "tint_method" other than "none" and a "render_method" set to "opaque" |
| ambient_occlusion | Decimal | true | Should this material have ambient occlusion applied when lighting? If true, shadows will be created around and underneath the block. Optionally can be constructed with float to control exponent applied to ao value after lighting. |
| emissive | Boolean | false | Should this material be rendered as emissive? Requires 'format_version' >= 1.21.120. |
| face_dimming | Boolean | true | Should this material be dimmed by the direction it's facing? |
| isotropic | Boolean | false | Should the faces that this material is applied to randomize their UVs? |
| render_method | String | opaque | The render method to use. Must be one of these options and must be the same for each:"opaque" - Used for a regular block texture without an alpha layer. Does not allow for transparency or translucency."double_sided" - Used for completely disabling backface culling."blend" - Used for a block like stained glass. Allows for transparency and translucency (slightly transparent textures)."alpha_test" - Used for a block like the vanilla (unstained) glass. Does not allow for translucency, only fully opaque or fully transparent textures. Also disables backface culling. |
| texture | String |  | Texture name for the material. |
| tint_method | Boolean | none | Tint multiplied to the color. Tint method logic varies, but often refers to the "rain" and "temperature" of the biome the block is placed in to compute the tint. Supported tint methods are "none", "default_foliage", "birch_foliage", "evergreen_foliage", "dry_foliage", "grass" and "water" |


> **minecraft:placement_filter** 子参数


| Name | Type | Default Value | Description |
| --- | --- | --- | --- |
| allowed_faces | Array |  | List of any of the following strings describing which face(s) this block can be placed on: "up", "down", "north", "south", "east", "west", "side", "all". Limited to 6 faces. |
| block_filter | Array |  | List of blocks that this block can be placed against in the "allowed_faces" direction. Limited to 64 blocks. Each block in this list can either be specified as a String (block name) or as a BlockDescriptor. A BlockDescriptor is an object that allows you to reference a block (or multiple blocks) based on its tags, or based on its name and states. The fields of a BlockDescriptor are described below. |


> **block_filter** 子参数


| Name | Type | Default Value | Description |
| --- | --- | --- | --- |
| name | String |  | The name of a block. |
| states | Array |  | The list of Vanilla block states and their values that the block can have, expressed in key/value pairs. |
| tags | String | 1 | A condition using Molang queries that results to true/false that can be used to query for blocks with certain tags. |


> **minecraft:queued_ticking** 子参数


| Name | Type | Default Value | Description |
| --- | --- | --- | --- |
| interval_range | Array |  | A range of values, specified in ticks, that will be used to decide the interval between times this event triggers. Each interval will be chosen randomly from the range, so the times between this event triggering will differ given an interval_range of two different values. If the values in the interval_range are the same, the event will always be triggered after that number of ticks. |
| looping | Boolean | true | Does the event loop? If false, the event will only be triggered once, after a delay equal to a number of ticks randomly chosen from the interval_range. If true, the event will loop, and each interval between events will be equal to a number of ticks randomly chosen from the interval_range. |
| on_tick | Trigger |  | The event that will be triggered once or on an interval. |


> **on_tick** 子参数


| Name | Type | Default Value | Description |
| --- | --- | --- | --- |
| condition | String | 1 | A condition using Molang queries that results to true/false. If true on the scheduled tick, the event will be triggered. If false on the scheduled tick, the event will not be triggered. |
| event | String | set_block_state | The event that will be triggered. |
| target | String | self | The target of the event executed by the block. |


> **minecraft:random_ticking** 子参数


| Name | Type | Default Value | Description |
| --- | --- | --- | --- |
| on_tick | Trigger |  | The event that will be triggered on random ticks. |


> **on_tick** 子参数


| Name | Type | Default Value | Description |
| --- | --- | --- | --- |
| condition | String | 1 | A condition using Molang queries that results to true/false. If true on the random tick, the event will be triggered. If false on the random tick, the event will not be triggered. |
| event | String | set_block_state | The event that will be triggered. |
| target | String | self | The target of the event executed by the block. |


> **minecraft:redstone_conductivity** 子参数


| Name | Type | Default Value | Description |
| --- | --- | --- | --- |
| allows_wire_to_step_down | Boolean | true | Specifies if redstone wire can stair-step downward on the block. |
| redstone_conductor | Boolean | false | Specifies if the block can be powered by redstone. |


> **minecraft:selection_box** 子参数


| Name | Type | Default Value | Description |
| --- | --- | --- | --- |
| origin | Vector [a, b, c] | [-8.0, 0.0, -8.0] | Minimal position of the bounds of the selection box. "origin" is specified as [x, y, z] and must be in the range (-8, 0, -8) to (8, 16, 8), inclusive. |
| size | Vector [a, b, c] | [16.0, 16.0, 16.0] | Size of each side of the selection box. Size is specified as [x, y, z]. "origin" + "size" must be in the range (-8, 0, -8) to (8, 16, 8), inclusive. |


# Block Definition Properties


These properties are part of the Block Definition. This helps the system determine how to parse and initialize this block.


| Name | Type | Default Value | Description |
| --- | --- | --- | --- |
| format_version | String |  | Specifies the version of the game this entity was made in. If the version is lower than the current version, any changes made to the entity in the vanilla version will be applied to it. |


## Code Example


Example


```json
{
  "format_version": "1.21.120",
  "minecraft:block": {
    "description": {
      "identifier": "design:lavenderstone"
    },
    "components": {
      "minecraft:loot": "loot_tables/chests/simple_dungeon.json",
      "minecraft:destroy_time": 4.0,
      "minecraft:friction": 0.6,
      "minecraft:map_color": "#00ff00",
      "minecraft:flammable": {
        "flame_odds": 50,
        "burn_odds": 0
    },
      "minecraft:light_emission": 1
    }
  }
}
```


# Block Description Properties


| Name | Type | Default Value | Description |
| --- | --- | --- | --- |
| identifier | String |  | The identifier for this block. The name must include a namespace and must not use the Minecraft namespace unless overriding a Vanilla block. |


# Block Event Responses


Event responses for block trigger components.


| Name | Type | Default Value | Description |
| --- | --- | --- | --- |
| add_mob_effect | JSON Object |  | Apply mob effect to target. |
| damage | JSON Object |  | Deals damage to the target. |
| decrement_stack | JSON Object |  | Decrement item stack. |
| die | JSON Object |  | Kill target. If target is self and this is run from a block then destroy the block. |
| play_effect | JSON Object |  | Spawns a particle effect relative to target position. |
| play_sound | JSON Object |  | Play a sound relative to target position. |
| remove_mob_effect | JSON Object |  | Removes mob effect from target. |
| run_command | JSON Object |  | Triggers a slash command or a list of slash commands. |
| set_block | JSON Object |  | Sets this block to another block type. |
| set_block_at_pos | JSON Object |  | Sets a block relative to this block to another block type. |
| set_block_state | JSON Object |  | Sets a block state on this block. |
| spawn_loot | JSON Object |  | Spawn loot from block. |
| swing | JSON Object |  | Event causes the actor to swing. |
| teleport | JSON Object |  | Teleport target randomly around destination point. |
| transform_item | JSON Object |  | Transforms item into another item. |


> **add_mob_effect** 子参数


| Name | Type | Default Value | Description |
| --- | --- | --- | --- |
| amplifier | Integer | 0 | The amplifier for the mob effect. |
| duration | Decimal | 0.0 | The duration of the mob effect in seconds. Can also be set to "infinite" |
| effect | String |  | The mob effect to apply. |
| target | Minecraft Filter | self | The target context to execute against. |


> **damage** 子参数


| Name | Type | Default Value | Description |
| --- | --- | --- | --- |
| amount | Integer | 0 | The amount of damage to deal. |
| mob_amount | Integer | 0 | The amount of damage to deal if held by a mob. |
| target | Minecraft Filter | self | The target context to execute against. |
| type | String |  | The type of damage to deal. |


> **die** 子参数


| Name | Type | Default Value | Description |
| --- | --- | --- | --- |
| target | Minecraft Filter | self | The target context to execute against. |


> **play_effect** 子参数


| Name | Type | Default Value | Description |
| --- | --- | --- | --- |
| data | Integer | 0 | Particle data value. |
| effect | String |  | The name of the particle effect to create. |
| target | Minecraft Filter | self | The target context to execute against. |


> **play_sound** 子参数


| Name | Type | Default Value | Description |
| --- | --- | --- | --- |
| sound | String |  | The name of the sound to play. |
| target | Minecraft Filter | self | The target context to execute against. |


> **remove_mob_effect** 子参数


| Name | Type | Default Value | Description |
| --- | --- | --- | --- |
| effect | String |  | The mob effect to remove. Use 'all' to remove all mob effects from target. |
| target | Minecraft Filter | self | The target context to execute against. |


> **run_command** 子参数


| Name | Type | Default Value | Description |
| --- | --- | --- | --- |
| command | String |  | Slash command to run. |
| command array | Array |  | List of slash commands to run. |
| target | Minecraft Filter | self | The target context to execute against. |


> **set_block** 子参数


| Name | Type | Default Value | Description |
| --- | --- | --- | --- |
| block_type | String |  | The type of block to set. |


> **set_block_at_pos** 子参数


| Name | Type | Default Value | Description |
| --- | --- | --- | --- |
| block_offset | Vector [a, b, c] | [0.0, 0.0, 0.0] | The offset from the block's center. |
| block_type | String |  | The type of block to set. |


> **set_block_state** 子参数


| Name | Type | Default Value | Description |
| --- | --- | --- | --- |
| state | Molang |  | Block state to set on the block. |


> **spawn_loot** 子参数


| Name | Type | Default Value | Description |
| --- | --- | --- | --- |
| table | String |  | File path, relative to the Behavior Pack's path, to the loot table file. |


> **teleport** 子参数


| Name | Type | Default Value | Description |
| --- | --- | --- | --- |
| avoid_water | Boolean | true | Determines if the teleport avoids putting the target in water. |
| destination | Vector [a, b, c] | [0.0, 0.0, 0.0] | Origin destination of the teleport. |
| land_on_block | Boolean | true | Determines if the teleport places the target on a block. |
| max_range | Vector [a, b, c] | [8.0, 8.0, 8.0] | Max range the target can teleport relative to the origin destination. |
| target | Minecraft Filter | self | The target context to execute against. |


> **transform_item** 子参数


| Name | Type | Default Value | Description |
| --- | --- | --- | --- |
| transform | String |  | Name of the item it should transform into |


## Code Example


Event Response Example


```json
"minecraft:block": {
      "description": {
        "identifier": "test:on_interact_change_state_block",
        "properties": {
          "custom:direction": [0, 1, 2, 3]
        }
      },
      "components": {
        "minecraft:on_interact": {
          "event": "test_event"
        }
      },
      "events": {
        "test_event": {
          "set_block_state": {
            "custom:direction": "1"
          }
        }
      }
    }
```


# Block Trigger Components


| Name | Type | Default Value | Description |
| --- | --- | --- | --- |
| minecraft:on_fall_on | JSON Object |  | Describes event for this block. |
| minecraft:on_interact | JSON Object |  | Describes event for this block. |
| minecraft:on_placed | JSON Object |  | Describes event for this block. |
| minecraft:on_player_destroyed | JSON Object |  | Describes event for this block. |
| minecraft:on_player_placing | JSON Object |  | Describes event for this block. |
| minecraft:on_step_off | JSON Object |  | Describes event for this block. |
| minecraft:on_step_on | JSON Object |  | Describes event for this block. |


> **minecraft:on_fall_on** 子参数


| Name | Type | Default Value | Description |
| --- | --- | --- | --- |
| condition | String |  | The condition of event to be executed on the block. |
| event | String |  | The event executed on the block. |
| min_fall_distance | Decimal | 0.0 | The minimum distance in blocks that an actor needs to fall to trigger this event. |
| target | String | self | The target of event executed on the block. |


> **minecraft:on_interact** 子参数


| Name | Type | Default Value | Description |
| --- | --- | --- | --- |
| condition | String |  | The condition of event to be executed on the block. |
| event | String |  | The event executed on the block. |
| target | String | self | The target of event executed on the block. |


> **minecraft:on_placed** 子参数


| Name | Type | Default Value | Description |
| --- | --- | --- | --- |
| condition | String |  | The condition of event to be executed on the block. |
| event | String |  | The event executed on the block. |
| target | String | self | The target of event executed on the block. |


> **minecraft:on_player_destroyed** 子参数


| Name | Type | Default Value | Description |
| --- | --- | --- | --- |
| condition | String |  | The condition of event to be executed on the block. |
| event | String |  | The event executed on the block. |
| target | String | self | The target of event executed on the block. |


> **minecraft:on_player_placing** 子参数


| Name | Type | Default Value | Description |
| --- | --- | --- | --- |
| condition | String |  | The condition of event to be executed on the block. |
| event | String |  | The event executed on the block. |
| target | String | self | The target of event executed on the block. |


> **minecraft:on_step_off** 子参数


| Name | Type | Default Value | Description |
| --- | --- | --- | --- |
| condition | String |  | The condition of event to be executed on the block. |
| event | String |  | The event executed on the block. |
| target | String | self | The target of event executed on the block. |


> **minecraft:on_step_on** 子参数


| Name | Type | Default Value | Description |
| --- | --- | --- | --- |
| condition | String |  | The condition of event to be executed on the block. |
| event | String |  | The event executed on the block. |
| target | String | self | The target of event executed on the block. |
