# CLIENT BIOMES DOCUMENTATION Version: 1.21.120.4


## Index


# Overview


Minecraft client_biome files define client-side settings for biomes in resource packs.


This is the new preferred location for per-biome settings that used to be in biomes_client.json.


(As of base game version 1.21.40, biomes_client.json is no longer loaded from built-in Vanilla resource packs. That file will still be loaded for other content regardless of version, and worlds using older base game versions will also still use it.)


These files are not part of the 'Custom Biomes' experiment and do not cause biome definitions to exist on their own, but they can be used to customize client-side settings of custom biomes.


# JSON Format


Here is a sample client_biome file.


```json
{
  "format_version": "1.21.40",
  "minecraft:client_biome": {
    "description": {
      "identifier": "the_end"
    },
    "components": {
      "minecraft:sky_color": {
        "sky_color": "#000000"
      },
      "minecraft:fog_appearance": {
        "fog_identifier": "minecraft:fog_the_end"
      },
      "minecraft:water_appearance": {
        "surface_color": "#62529e"
      }
    }
  }
}
```


# Schema


# Client Biome Components


Any components that this Client Biome uses


| Name | Type | Required? | Description |
| --- | --- | --- | --- |
| minecraft:ambient_sounds | Object | Optional | Set the ambient sounds for the biome. These sounds must be in the 'individual_named_sounds' in a 'sounds.json' file. |
| minecraft:atmosphere_identifier | Object | Optional | Set the identifier used for atmospherics in Vibrant Visuals mode. Identifiers must resolve to identifiers in valid Atmospheric Scattering JSON schemas under the "atmospherics" directory. Biomes without this component will have default atmosphere settings. |
| minecraft:biome_music | Object | Optional | Affect how music plays within the biome |
| minecraft:color_grading_identifier | Object | Optional | Set the identifier used for color grading in Vibrant Visuals mode. Identifiers must resolve to identifiers in valid Color Grading JSON schemas under the "color_grading" directory. Biomes without this component will have default color_grading settings. |
| minecraft:dry_foliage_color | Object | Optional | Set the dry foliage color used during rendering. Biomes without this component will have default dry foliage color behavior. |
| minecraft:fog_appearance | Object | Optional | Set the fog settings used during rendering. Biomes without this component will have default fog settings. |
| minecraft:foliage_appearance | Object | Optional | Set the foliage color or color map used during rendering. Biomes without this component will have default foliage appearance. |
| minecraft:grass_appearance | Object | Optional | Set the grass color or color map used during rendering. Biomes without this component will have default grass appearance. |
| minecraft:lighting_identifier | Object | Optional | Set the identifier used for lighting in Vibrant Visuals mode. Identifiers must resolve to identifiers in valid Lighting JSON schemas under the "lighting" directory. Biomes without this component will have default lighting settings. |
| minecraft:precipitation | Object | Optional | Describes the visuals for a biome's precipitation. Biomes without this component will have default values. At most one precipitation type can be set for a biome. |
| minecraft:sky_color | Object | Optional | Set the sky color used during rendering. Biomes without this component will have default sky color behavior. |
| minecraft:water_appearance | Object | Optional | Set the water surface color used during rendering. Biomes without this component will have default water surface color behavior. |
| minecraft:water_identifier | Object | Optional | Set the identifier used for rendering water in Vibrant Visuals mode. Identifiers must resolve to identifiers in valid Water JSON schemas under the "water" directory. Biomes without this component will have default water settings. |


# Client Biome Definition


Contains a description and components to define a Client Biome.


| Name | Type | Required? | Description |
| --- | --- | --- | --- |
| components | Object of type Client Biome Components | Required | Components for this Client Biome. |
| description | Object of type Client Biome Description | Required | Non-component settings, including the Client Biome name. |


# Client Biome Description


Contains non-component settings for a Client Biome.


| Name | Type | Required? | Description |
| --- | --- | --- | --- |
| identifier | String | Required | The name of the Client Biome, used by other features like the '/locate biome' command. Must match the name of a Biome defined by the game or a behavior pack. |


# Client Biome JSON File


Contains a format version and a Client Biome definition


| Name | Type | Required? | Description |
| --- | --- | --- | --- |
| format_version | String | Required | Version of the JSON schema used by this file |
| minecraft:client_biome | Object of type Client Biome Definition | Required | A single Client Biome definition, containing rendering or sound settings related to a Biome defined by the game or a behavior pack |


# Foliage Color Map


Object specifying a color map for foliage instead of a specific color.


| Name | Type | Required? | Description |
| --- | --- | --- | --- |
| color_map | "foliage", "birch", "evergreen", "mangrove_swamp_foliage", "swamp_foliage", "dry_foliage" | Required | Color map from textures/colormap to determine color of foliage. |


# Grass Color Map


Object specifying a color map for grass instead of a specific color.


| Name | Type | Required? | Description |
| --- | --- | --- | --- |
| color_map | "grass", "swamp_grass" | Required | Color map from textures/colormap to determine color of grass. |


# Sound Addition


| Name | Type | Required? | Description |
| --- | --- | --- | --- |
| asset | Object of type Reference | Required | Name of the sound asset to play |
| chance | Float | Required | Probability of the sound playing each interval, between 0.0 and 1.0 |


# minecraft:ambient_sounds


Set the ambient sounds for the biome. These sounds must be in the 'individual_named_sounds' in a 'sounds.json' file.


| Name | Type | Required? | Description |
| --- | --- | --- | --- |
| addition | Object of type Sound Addition | Optional | Named sound that occasionally plays at the listener position |
| loop | Object of type Reference | Optional | Named sound that loops while the listener position is inside the biome |
| mood | Object of type Reference | Optional | Named sound that rarely plays at a nearby air block position when the light level is low. Biomes without an ambient mood sound will use the 'ambient.cave' sound. |
| underwater_addition | Object of type Sound Addition | Optional | Named sound that occasionally plays at the listener position when underwater |
| underwater_loop | Object of type Reference | Optional | Named sound that loops while the listener position is inside the biome and underwater |
| underwater_mood | Object of type Reference | Optional | Named sound that rarely plays at a nearby water block position when the light level is low. Biomes without an underwater ambient mood sound will use the 'ambient.underwater.loop' sound. |


# minecraft:atmosphere_identifier


Set the identifier used for atmospherics in Vibrant Visuals mode. Identifiers must resolve to identifiers in valid Atmospheric Scattering JSON schemas under the "atmospherics" directory. Biomes without this component will have default atmosphere settings.


| Name | Type | Required? | Description |
| --- | --- | --- | --- |
| atmosphere_identifier | Object of type Reference | Required | Identifier of atmosphere definition to use |


# minecraft:biome_music


Affect how music plays within the biome


| Name | Type | Required? | Description |
| --- | --- | --- | --- |
| music_definition | Object of type Reference | Optional | Music to be played when inside this biome. If left off or not found the default music will be determined by the dimension. Empty string will result in no music. |
| underwater_music | Boolean | Optional | Enables underwater music |
| volume_multiplier | Float | Optional | Multiplier temporarily and gradually applied to music volume when within this biome. Must be a value between 0 and 1, inclusive. |


# minecraft:color_grading_identifier


Set the identifier used for color grading in Vibrant Visuals mode. Identifiers must resolve to identifiers in valid Color Grading JSON schemas under the "color_grading" directory. Biomes without this component will have default color_grading settings.


| Name | Type | Required? | Description |
| --- | --- | --- | --- |
| color_grading_identifier | Object of type Reference | Required | Identifier of color_grading definition to use |


# minecraft:dry_foliage_color


Set the dry foliage color used during rendering. Biomes without this component will have default dry foliage color behavior.


| Name | Type | Required? | Description |
| --- | --- | --- | --- |
| color | Object of type Color255RGB | Required | RGB color of dry foliage |


# minecraft:fog_appearance


Set the fog settings used during rendering. Biomes without this component will have default fog settings.


| Name | Type | Required? | Description |
| --- | --- | --- | --- |
| fog_identifier | Object of type Reference | Required | Identifier of fog definition to use |


# minecraft:foliage_appearance


Set the foliage color or color map used during rendering. Biomes without this component will have default foliage appearance.


| Name | Type | Required? | Description |
| --- | --- | --- | --- |
| color | Object of type Color255RGB or Object of type Foliage Color Map | Optional | RGB color of foliage, or a Foliage Color Map object. |


# minecraft:grass_appearance


Set the grass color or color map used during rendering. Biomes without this component will have default grass appearance.


| Name | Type | Required? | Description |
| --- | --- | --- | --- |
| color | Object of type Color255RGB or Object of type Grass Color Map | Optional | RGB color of grass. |
| grass_is_shaded | Boolean | Optional | Adds a shading effect to the grass as if there was a roof. |


# minecraft:lighting_identifier


Set the identifier used for lighting in Vibrant Visuals mode. Identifiers must resolve to identifiers in valid Lighting JSON schemas under the "lighting" directory. Biomes without this component will have default lighting settings.


| Name | Type | Required? | Description |
| --- | --- | --- | --- |
| lighting_identifier | Object of type Reference | Required | Identifier of lighting definition to use |


# minecraft:precipitation


Describes the visuals for a biome's precipitation. Biomes without this component will have default values. At most one precipitation type can be set for a biome.


| Name | Type | Required? | Description |
| --- | --- | --- | --- |
| ash | Float | Optional | Density of ash precipitation visuals |
| blue_spores | Float | Optional | Density of blue spore precipitation visuals |
| red_spores | Float | Optional | Density of blue spore precipitation visuals |
| white_ash | Float | Optional | Density of white ash precipitation visuals |


# minecraft:sky_color


Set the sky color used during rendering. Biomes without this component will have default sky color behavior.


| Name | Type | Required? | Description |
| --- | --- | --- | --- |
| sky_color | Object of type Color255RGB | Required | RGB color of the sky |


# minecraft:water_appearance


Set the water surface color used during rendering. Biomes without this component will have default water surface color behavior.


| Name | Type | Required? | Description |
| --- | --- | --- | --- |
| surface_color | Object of type Color255RGB | Optional | RGB color of the water surface |
| surface_opacity | Float | Optional | Opacity of the water surface (must be between 0 for invisible and 1 for opaque, inclusive) |


# minecraft:water_identifier


Set the identifier used for rendering water in Vibrant Visuals mode. Identifiers must resolve to identifiers in valid Water JSON schemas under the "water" directory. Biomes without this component will have default water settings.


| Name | Type | Required? | Description |
| --- | --- | --- | --- |
| water_identifier | Object of type Reference | Required | Identifier of water definition to use |
