# BIOMES DOCUMENTATION Version: 1.21.120.4


## Index


# Overview


Minecraft biomes can have different terrain characteristics. By writing custom biome data you could:


1) Change the general shape of the terrain.


2) Change the ratio of frequency of biome types.


3) Change the blocks that make up the biome, both at the surface and down below.


4) Change the distribution of decorative features like trees, grass, and flowers.


5) Change the mobs that spawn.


6) Change the climate.


7) ...and more!


# JSON Format


All biomes should specify the version that they target via the "format_version" field. Biomes should also specify a "description" sub-object with a namespaced "identifier" field that uniquely labels the biome. The remainder of the biome data is divided up into independent JSON sub-objects, or components.


In general, a component defines what game behaviors a biome participates in, and the component fields define how it participates.


There are basically two categories of components:


1) Namespaced components, such as those with a 'name:' prefix, that map to specific behaviors in-game. They may have member fields that parameterize that behavior. Only names that have a valid mapping are supported.


2) 'tags' which are defined under the "minecraft:tags" component. Tags consist of alphanumeric characters, along with '.' and '_'. A tag is attached to the biome so that either code or data may check for its existence.


Here is a sample biome schema with additional details and the full list of namespaced components.


```json
{
  "format_version": "1.21.110",
  "minecraft:biome": {
    "description": {
      "identifier": "minecraft:plains"
    },
    "minecraft:climate": {
      "downfall": 0.4,
      "snow_accumulation": [ 0.0, 0.125 ],
      "temperature": 0.8
    },
    "minecraft:overworld_height": {
      "noise_type": "lowlands"
    },
    "minecraft:surface_builder": {
    "builder": {
        "type": "minecraft:overworld",
    "sea_floor_depth": 7,
    "sea_floor_material": "minecraft:gravel",
    "foundation_material": "minecraft:stone",
    "mid_material": "minecraft:dirt",
    "top_material": "minecraft:grass_block"
    }
    },
    "minecraft:overworld_generation_rules": {
      "hills_transformation": [
        [ "minecraft:forest_hills", 1 ],
        [ "minecraft:forest", 2 ]
      ],
      "mutate_transformation": "minecraft:sunflower_plains",
      "generate_for_climates": [
        [ "medium", 3 ],
        [ "warm", 1 ],
        [ "cold", 1 ]
      ]
    },

    "minecraft:tags": {
      "tags": [
        "animal",
        "monster",
        "overworld",
        "plains"
      ]
    }
  }
}
```


# Adding Biomes


Biomes are read from JSON files in the biomes subfolders of behavior packs. Loading enforces one biome per file. The biome's identifier acts as its name and must be namespaced. Adding a file with a new name to the biome data location will make it available for the game to use, while existing biomes can be overridden using another biome with the same identifier. Note that if you add a new biome, you'll need to write component data that allows it to participate in world generation (as shown in the full schema below), or else it won't show up in your worlds!


# Schema


# Biome Components


Any components that this Biome uses


| Name | Type | Required? | Description |
| --- | --- | --- | --- |
| minecraft:climate | Object | Optional | Describes temperature, humidity, precipitation, and similar. Biomes without this component will have default values. |
| minecraft:creature_spawn_probability | Object | Optional | Probability that creatures will spawn within the biome when a chunk is generated. |
| minecraft:humidity | Object | Optional | Forces a biome to ether always be humid or never humid. Humidity effects the spread chance, and spread rate of fire in the biome |
| minecraft:map_tints | Object | Optional | Sets the color grass and foliage will be tinted by in this biome on the map. |
| minecraft:mountain_parameters | Object | Optional | Noise parameters used to drive mountain terrain generation in Overworld. |
| minecraft:multinoise_generation_rules | Object | Optional | Controls how this biome is instantiated (and then potentially modified) during world generation of the nether. |
| minecraft:overworld_generation_rules | Object | Optional | Controls how this biome is instantiated (and then potentially modified) during world generation of the overworld. |
| minecraft:overworld_height | Object | Optional | Noise parameters used to drive terrain height in the Overworld. |
| minecraft:partially_frozen | Object | Optional | Component will impact the temperature in a frozen biome, causing some areas to not be frozen. Ex: patchy ice, patchy snow |
| minecraft:replace_biomes | Object | Optional | Replaces a specified portion of one or more Minecraft biomes. |
| minecraft:surface_builder | Object | Optional | Controls the materials used for terrain generation. |
| minecraft:surface_material_adjustments | Object | Optional | Specify fine-detail changes to blocks used in terrain generation (based on a noise function). |
| minecraft:tags | Object | Optional | Attach arbitrary string tags to this biome.Most biome tags are referenced by JSON settings, but some meanings of tags are directly implemented in the game's code. These tags are listed here:birch: Biome uses wildflowers (mutually exclusive with other flower biome tags). Does nothing if biome is tagged "hills".cold: Villagers will be dressed for snowy weather.deep: Pre-Caves and Cliffs, prevents an ocean from having islands or connected rivers and makes the biome less likely to have hills.desert: Allows partially-buried ruined portals to be placed in the biome. Sand blocks will play ambient sounds when the player is nearby.extreme_hills: Ruined portals can be placed higher than normal. Biomes tagged "forest" or "forest_generation" will use normal Overworld flowers instead of forest flowers.flower_forest: Biome uses forest flowers (mutually exclusive with other flower biome tags).forest: Biome uses forest flowers (mutually exclusive with other flower biome tags). Does nothing if biome is tagged tagged "taiga" or "extreme_hills".forest_generation: Equivalent to "forest".frozen: Villagers will be dressed for snowy weather. Prevents the biome from containing lava springs if it is also tagged "ocean".ice: Around ruined portals, lava is always replaced by Netherrack and Netherrack cannot be replaced by magma.ice_plains: Prevents the biome from containing lava springs if it is also tagged "mutated".jungle: Ruined portals will be very mossy.hills: Biomes tagged "meadow" or "birch" will use normal Overworld flowers instead of wildflowers.meadow: Biome uses wildflowers (mutually exclusive with other flower biome tags). Does nothing if biome is tagged "hills".mesa: Sand blocks will play ambient sounds when the player is nearby.mountain: Ruined portals can be placed higher than normal.mutated: Pre-Caves and Cliffs, prevents switching to the specified "mutate_transformation" as the biome is already considered mutated. Prevents the biome from containing lava springs if it is also tagged "ice_plains".no_legacy_worldgen: Prevents biome from using legacy world generation behavior unless the biome is being placed in the Overworld.ocean: Prevents the biome from containing lava springs if it is also tagged "frozen". Allows ruined portals to be found underwater. Pre-Caves and Cliffs, determines if shorelines and rivers should be placed at the edges of the biome and identifies the biome as a shallow ocean for placing islands, unless the "deep" tag is present.pale_garden: Biome uses closed-eye blossoms (mutually exclusive with other flower biome tags).plains: Biome uses plains flowers (mutually exclusive with other flower biome tags).rare: Pre-Caves and Cliffs, this tag flags the biome as a special biome. Oceans cannot be special.swamp: Allows ruined portals to be found underwater. Biome uses swamp flowers (mutually exclusive with other flower biome tags).taiga: Biomes tagged "forest" or "forest_generation" will use normal Overworld flowers instead of forest flowers. |


# Biome Definition


Contains a description and components to define a Biome.


| Name | Type | Required? | Description |
| --- | --- | --- | --- |
| components | Object of type Biome Components | Required | Components for this Biome. |
| description | Object of type Biome Description | Required | Non-component settings, including the Biome name. |


# Biome Description


Contains non-component settings for a Biome.


| Name | Type | Required? | Description |
| --- | --- | --- | --- |
| identifier | Object of type Identifier | Required | The name of the Biome, used by other features like the '/locate biome' command. Identifiers should only be lowercase. |


# Biome JSON File


Contains a format version and a biome definition


| Name | Type | Required? | Description |
| --- | --- | --- | --- |
| format_version | String | Required | Version of the JSON schema used by this file |
| minecraft:biome | Object of type Biome Definition | Required | A single biome definition |


# Biome Replacement


Represents the replacement information used to determine the placement of the overriding biome.


| Name | Type | Required? | Description |
| --- | --- | --- | --- |
| amount | Float | Required | Noise value used to determine whether or not the replacement is attempted, similar to a percentage. Must be in the range (0.0, 1.0]. |
| dimension | String | Required | Dimension in which this replacement can happen. Must be 'minecraft:overworld'. |
| noise_frequency_scale | Float | Required | Scaling value used to alter the frequency of replacement attempts. A lower frequency will mean a bigger contiguous biome area that occurs less often. A higher frequency will mean smaller contiguous biome areas that occur more often. Must be in the range (0.0, 100.0]. |
| targets | Array of Object of type Reference | Required | Biomes that are going to be replaced by the overriding biome. Target biomes must not contain namespaces. |


# Block Specifier


Specifies a particular block. Can be a string block name or a JSON object


| Name | Type | Required? | Description |
| --- | --- | --- | --- |
| name | Object of type Reference | Required | Name of the block |
| states | Object | Optional | Contains members named after each state, with boolean, integer, or string values. |


# Molang Expression


A JSON field that specifies a Molang expression. Can be an integer, float, boolean, or string.


# custom_map_tint_grass_noise


Makes grass use the noise based colors for tinting in this biome on the map.


| Name | Type | Required? | Description |
| --- | --- | --- | --- |
| type | "tint", "noise" | Required | Controls the type of grass tint to use. |


# custom_map_tint_grass_tint


Sets the color grass will be tinted by in this biome on the map.


| Name | Type | Required? | Description |
| --- | --- | --- | --- |
| tint | Object of type Color255RGB | Required | Tint color used in this biome on the map. |
| type | "tint", "noise" | Required | Controls the type of grass tint to use. |


# minecraft:capped


Generates surface on blocks with non-solid blocks above or below.


| Name | Type | Required? | Description |
| --- | --- | --- | --- |
| beach_material | Object of type Block Specifier | Optional | Material used to decorate surface near sea level. |
| ceiling_materials | Array of Object of type Block Specifier | Required | Materials used for the surface ceiling. |
| floor_materials | Array of Object of type Block Specifier | Required | Materials used for the surface floor. |
| foundation_material | Object of type Block Specifier | Required | Material used to replace solid blocks that are not surface blocks. |
| sea_material | Object of type Block Specifier | Required | Material used to replace air blocks below sea level. |
| type | "minecraft:overworld", "minecraft:frozen_ocean", "minecraft:mesa", "minecraft:swamp", "minecraft:capped", "minecraft:the_end" | Required | Controls the type of surface builder to use |


# minecraft:climate


Describes temperature, humidity, precipitation, and similar. Biomes without this component will have default values.


| Name | Type | Required? | Description |
| --- | --- | --- | --- |
| downfall | Float | Optional | Amount that precipitation affects colors and block changes. Setting to 0 will stop rain from falling in the biome. |
| snow_accumulation | Array of 2 Floats | Optional | Minimum and maximum snow level, each multiple of 0.125 is another snow layer |
| temperature | Float | Optional | Temperature affects a variety of visual and behavioral things, including snow and ice placement, sponge drying, and sky color |


# minecraft:creature_spawn_probability


Probability that creatures will spawn within the biome when a chunk is generated.


| Name | Type | Required? | Description |
| --- | --- | --- | --- |
| probability | Float | Optional | Probabiltity between [0.0, 0.75] of creatures spawning within the biome on chunk generation. |


# minecraft:frozen_ocean


Similar to overworld_surface. Adds icebergs.


| Name | Type | Required? | Description |
| --- | --- | --- | --- |
| foundation_material | Object of type Block Specifier | Required | Controls the block type used deep underground in this biome |
| mid_material | Object of type Block Specifier | Required | Controls the block type used in a layer below the surface of this biome |
| sea_floor_depth | Integer | Required | Controls how deep below the world water level the floor should occur |
| sea_floor_material | Object of type Block Specifier | Required | Controls the block type used as a floor for bodies of water in this biome |
| sea_material | Object of type Block Specifier | Required | Controls the block type used for the bodies of water in this biome |
| top_material | Object of type Block Specifier | Required | Controls the block type used for the surface of this biome |
| type | "minecraft:overworld", "minecraft:frozen_ocean", "minecraft:mesa", "minecraft:swamp", "minecraft:capped", "minecraft:the_end" | Required | Controls the type of surface builder to use |


# minecraft:humidity


Forces a biome to ether always be humid or never humid. Humidity effects the spread chance, and spread rate of fire in the biome


| Name | Type | Required? | Description |
| --- | --- | --- | --- |
| is_humid | Boolean | Required |  |


# minecraft:map_tints


Sets the color grass and foliage will be tinted by in this biome on the map.


| Name | Type | Required? | Description |
| --- | --- | --- | --- |
| foliage | Object of type Color255RGB | Optional | Sets the color foliage will be tinted by in this biome on the map. |
| grass | Object of type custom_map_tint_grass_tint or Object of type custom_map_tint_grass_noise | Required | Controls whether the grass will use a custom tint color or a noise based tint color. |


# minecraft:mesa


Similar to overworld_surface. Adds colored strata and optional pillars.


| Name | Type | Required? | Description |
| --- | --- | --- | --- |
| bryce_pillars | Boolean | Required | Whether the mesa generates with pillars |
| clay_material | Object of type Block Specifier | Required | Base clay block to use |
| foundation_material | Object of type Block Specifier | Required | Controls the block type used deep underground in this biome |
| hard_clay_material | Object of type Block Specifier | Required | Hardened clay block to use |
| has_forest | Boolean | Required | Places coarse dirt and grass at high altitudes |
| mid_material | Object of type Block Specifier | Required | Controls the block type used in a layer below the surface of this biome |
| sea_floor_depth | Integer | Required | Controls how deep below the world water level the floor should occur |
| sea_floor_material | Object of type Block Specifier | Required | Controls the block type used as a floor for bodies of water in this biome |
| sea_material | Object of type Block Specifier | Required | Controls the block type used for the bodies of water in this biome |
| top_material | Object of type Block Specifier | Required | Controls the block type used for the surface of this biome |
| type | "minecraft:overworld", "minecraft:frozen_ocean", "minecraft:mesa", "minecraft:swamp", "minecraft:capped", "minecraft:the_end" | Required | Controls the type of surface builder to use |


# minecraft:mountain_parameters


Noise parameters used to drive mountain terrain generation in Overworld.


| Name | Type | Required? | Description |
| --- | --- | --- | --- |
| steep_material_adjustment | Object of type minecraft:mountain_parameters - steep_material_adjustment settings | Optional | Defines surface material for steep slopes |
| top_slide | Object of type minecraft:mountain_parameters - top_slide settings | Optional | Controls the density tapering that happens at the top of the world to prevent terrain from reaching too high |


# minecraft:mountain_parameters - steep_material_adjustment settings


Defines surface material for steep slopes


| Name | Type | Required? | Description |
| --- | --- | --- | --- |
| east_slopes | Boolean | Optional | Enable for east-facing slopes |
| material | Object of type Block Specifier | Optional | Block type use as steep material |
| north_slopes | Boolean | Optional | Enable for north-facing slopes |
| south_slopes | Boolean | Optional | Enable for south-facing slopes |
| west_slopes | Boolean | Optional | Enable for west-facing slopes |


# minecraft:mountain_parameters - top_slide settings


Controls the density tapering that happens at the top of the world to prevent terrain from reaching too high.


| Name | Type | Required? | Description |
| --- | --- | --- | --- |
| enabled | Boolean | Required | If false, top slide will be disabled. If true, other parameters will be taken into account. |


# minecraft:multinoise_generation_rules


Controls how this biome is instantiated (and then potentially modified) during world generation of the nether.


| Name | Type | Required? | Description |
| --- | --- | --- | --- |
| target_altitude | Float | Optional | Altitude with which this biome should be generated, relative to other biomes. |
| target_humidity | Float | Optional | Humidity with which this biome should be generated, relative to other biomes. |
| target_temperature | Float | Optional | Temperature with which this biome should be generated, relative to other biomes. |
| target_weirdness | Float | Optional | Weirdness with which this biome should be generated, relative to other biomes. |
| weight | Float | Optional | Weight with which this biome should be generated, relative to other biomes. |


# minecraft:overworld


Controls the blocks used for the default Minecraft Overworld terrain generation.


| Name | Type | Required? | Description |
| --- | --- | --- | --- |
| foundation_material | Object of type Block Specifier | Required | Controls the block type used deep underground in this biome |
| mid_material | Object of type Block Specifier | Required | Controls the block type used in a layer below the surface of this biome |
| sea_floor_depth | Integer | Required | Controls how deep below the world water level the floor should occur |
| sea_floor_material | Object of type Block Specifier | Required | Controls the block type used as a floor for bodies of water in this biome |
| sea_material | Object of type Block Specifier | Required | Controls the block type used for the bodies of water in this biome |
| top_material | Object of type Block Specifier | Required | Controls the block type used for the surface of this biome |
| type | "minecraft:overworld", "minecraft:frozen_ocean", "minecraft:mesa", "minecraft:swamp", "minecraft:capped", "minecraft:the_end" | Required | Controls the type of surface builder to use |


# minecraft:overworld_generation_rules


Controls how this biome is instantiated (and then potentially modified) during world generation of the overworld.


| Name | Type | Required? | Description |
| --- | --- | --- | --- |
| generate_for_climates | Array of Object of type minecraft:overworld_generation_rules - Weighted climate categories settings | Optional | Controls the world generation climate categories that this biome can spawn for. A single biome can be associated with multiple categories with different weightings. |
| hills_transformation | Object of type minecraft:overworld_generation_rules - Weighted biome names settings | Optional | What biome to switch to when converting to a hilly biome |
| mutate_transformation | Object of type minecraft:overworld_generation_rules - Weighted biome names settings | Optional | What biome to switch to when converting to a mutated biome |
| river_transformation | Object of type minecraft:overworld_generation_rules - Weighted biome names settings | Optional | What biome to switch to when converting to a river biome (if not the Vanilla 'river' biome) |
| shore_transformation | Object of type minecraft:overworld_generation_rules - Weighted biome names settings | Optional | What biome to switch to when adjacent to an ocean biome |


# minecraft:overworld_generation_rules - Weighted biome names settings


Can be just the string name of a Biome, or an array of any size. If an array, each entry can be a Biome name string, or an array of size 2, where the first entry is a Biome name and the second entry is a positive integer representing how that Biome is weighted against other entries. If no weight is provided, a weight of 1 is used.


# minecraft:overworld_generation_rules - Weighted climate categories settings


An array of any size containing arrays of exactly two elements. For each contained array, the first element is a climate category string ('medium', 'warm', 'lukewarm', 'cold', or 'frozen'). The second element is a positive integer for how much that entry is weighted relative to other entries.


# minecraft:overworld_height


Noise parameters used to drive terrain height in the Overworld.


| Name | Type | Required? | Description |
| --- | --- | --- | --- |
| noise_params | Array of 2 Floats | Optional | First value is depth - more negative means deeper underwater, while more positive means higher. Second value is scale, which affects how much noise changes as it moves from the surface. |
| noise_type | "default", "default_mutated", "river", "ocean", "deep_ocean", "lowlands", "taiga", "mountains", "highlands", "extreme", "less_extreme", "beach", "stone_beach", "mushroom", "swamp" | Optional | Specifies a preset based on a built-in setting rather than manually using noise_params |


# minecraft:partially_frozen


Component will impact the temperature in a frozen biome, causing some areas to not be frozen. Ex: patchy ice, patchy snow


# minecraft:replace_biomes


Replaces a specified portion of one or more Minecraft biomes.


| Name | Type | Required? | Description |
| --- | --- | --- | --- |
| replacements | Array of Object of type Biome Replacement | Required | List of biome replacement configurations. Retroactively adding a new replacement to the front of this list will cause the world generation to change. Please add any new replacements to the end of the list. |


# minecraft:surface_builder


Controls the materials used for terrain generation.


| Name | Type | Required? | Description |
| --- | --- | --- | --- |
| builder | Object of type minecraft:overworld or Object of type minecraft:frozen_ocean or Object of type minecraft:mesa or Object of type minecraft:swamp or Object of type minecraft:capped or Object of type minecraft:the_end | Required | Controls the block types used for terrain generation. |


# minecraft:surface_material_adjustments


Specify fine-detail changes to blocks used in terrain generation (based on a noise function).


| Name | Type | Required? | Description |
| --- | --- | --- | --- |
| adjustments | Array of Object of type minecraft:surface_material_adjustments - surface adjustment settings | Optional | All adjustments that match the column's noise values will be applied in the order listed. |


# minecraft:surface_material_adjustments - surface adjustment materials settings


The specific blocks used for this surface adjustment


| Name | Type | Required? | Description |
| --- | --- | --- | --- |
| foundation_material | Object of type Block Specifier | Optional | Controls the block type used deep underground in this biome when this adjustment is active. |
| mid_material | Object of type Block Specifier | Optional | Controls the block type used in a layer below the surface of this biome when this adjustment is active. |
| sea_floor_material | Object of type Block Specifier | Optional | Controls the block type used as a floor for bodies of water in this biome when this adjustment is active. |
| sea_material | Object of type Block Specifier | Optional | Controls the block type used in the bodies of water in this biome when this adjustment is active. |
| top_material | Object of type Block Specifier | Optional | Controls the block type used for the surface of this biome when this adjustment is active. |


# minecraft:surface_material_adjustments - surface adjustment settings


An adjustment to generated terrain, replacing blocks based on the specified settings.


| Name | Type | Required? | Description |
| --- | --- | --- | --- |
| height_range | Array of 2 Molang expressions | Optional | Defines a range of noise values [min, max] for which this adjustment should be applied. |
| materials | Object of type minecraft:surface_material_adjustments - surface adjustment materials settings | Required |  |
| noise_frequency_scale | Float | Optional | The scale to multiply by the position when accessing the noise value for the material adjustments. |
| noise_range | Array of 2 Floats | Optional | Defines a range of noise values [min, max] for which this adjustment should be applied. |


# minecraft:swamp


Used to add decoration to the surface of swamp biomes such as water lilies.


| Name | Type | Required? | Description |
| --- | --- | --- | --- |
| foundation_material | Object of type Block Specifier | Required | Controls the block type used deep underground in this biome. |
| max_puddle_depth_below_sea_level | Integer | Required | Controls the depth at which surface level blocks can be replaced with water for puddles. The number represents the number of blocks (0, 127) below sea level that we will go down to look for a surface block. |
| mid_material | Object of type Block Specifier | Required | Controls the block type used in a layer below the surface of this biome. |
| sea_floor_depth | Integer | Required | Controls how deep below the world water level the floor should occur. |
| sea_floor_material | Object of type Block Specifier | Required | Controls the block type used as a floor for bodies of water in this biome. |
| sea_material | Object of type Block Specifier | Required | Controls the block type used for the bodies of water in this biome. |
| top_material | Object of type Block Specifier | Required | Controls the block type used for the surface of this biome. |
| type | "minecraft:overworld", "minecraft:frozen_ocean", "minecraft:mesa", "minecraft:swamp", "minecraft:capped", "minecraft:the_end" | Required | Controls the type of surface builder to use |


# minecraft:tags


Attach arbitrary string tags to this biome.


Most biome tags are referenced by JSON settings, but some meanings of tags are directly implemented in the game's code. These tags are listed here:


birch: Biome uses wildflowers (mutually exclusive with other flower biome tags). Does nothing if biome is tagged "hills".


cold: Villagers will be dressed for snowy weather.


deep: Pre-Caves and Cliffs, prevents an ocean from having islands or connected rivers and makes the biome less likely to have hills.


desert: Allows partially-buried ruined portals to be placed in the biome. Sand blocks will play ambient sounds when the player is nearby.


extreme_hills: Ruined portals can be placed higher than normal. Biomes tagged "forest" or "forest_generation" will use normal Overworld flowers instead of forest flowers.


flower_forest: Biome uses forest flowers (mutually exclusive with other flower biome tags).


forest: Biome uses forest flowers (mutually exclusive with other flower biome tags). Does nothing if biome is tagged tagged "taiga" or "extreme_hills".


forest_generation: Equivalent to "forest".


frozen: Villagers will be dressed for snowy weather. Prevents the biome from containing lava springs if it is also tagged "ocean".


ice: Around ruined portals, lava is always replaced by Netherrack and Netherrack cannot be replaced by magma.


ice_plains: Prevents the biome from containing lava springs if it is also tagged "mutated".


jungle: Ruined portals will be very mossy.


hills: Biomes tagged "meadow" or "birch" will use normal Overworld flowers instead of wildflowers.


meadow: Biome uses wildflowers (mutually exclusive with other flower biome tags). Does nothing if biome is tagged "hills".


mesa: Sand blocks will play ambient sounds when the player is nearby.


mountain: Ruined portals can be placed higher than normal.


mutated: Pre-Caves and Cliffs, prevents switching to the specified "mutate_transformation" as the biome is already considered mutated. Prevents the biome from containing lava springs if it is also tagged "ice_plains".


no_legacy_worldgen: Prevents biome from using legacy world generation behavior unless the biome is being placed in the Overworld.


ocean: Prevents the biome from containing lava springs if it is also tagged "frozen". Allows ruined portals to be found underwater. Pre-Caves and Cliffs, determines if shorelines and rivers should be placed at the edges of the biome and identifies the biome as a shallow ocean for placing islands, unless the "deep" tag is present.


pale_garden: Biome uses closed-eye blossoms (mutually exclusive with other flower biome tags).


plains: Biome uses plains flowers (mutually exclusive with other flower biome tags).


rare: Pre-Caves and Cliffs, this tag flags the biome as a special biome. Oceans cannot be special.


swamp: Allows ruined portals to be found underwater. Biome uses swamp flowers (mutually exclusive with other flower biome tags).


taiga: Biomes tagged "forest" or "forest_generation" will use normal Overworld flowers instead of forest flowers.


| Name | Type | Required? | Description |
| --- | --- | --- | --- |
| tags | Array of String | Required | Array of string tags used by other systems such as entity spawning |


# minecraft:the_end


| Name | Type | Required? | Description |
| --- | --- | --- | --- |
| type | "minecraft:overworld", "minecraft:frozen_ocean", "minecraft:mesa", "minecraft:swamp", "minecraft:capped", "minecraft:the_end" | Required | Use default Minecraft End terrain generation. |
