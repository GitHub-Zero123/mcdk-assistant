# RECIPES DOCUMENTATION Version: 1.21.120.4


## Index


# Recipes


Recipes are setup in Json files under the behavior_packs /'name of pack'/ recipes directory.


Recipe JSON files have different structures dependent on their type.


# Furnace Recipe


Represents a furnace recipe for a furnace.'Input' items will burn and transform into items specified in 'output'.


## Parameters


| Name | Type | Description |
| --- | --- | --- |
| input | item names | Items used as input for the furnace recipe. |
| output | item names | Items used as output for the furnace recipe. |


## Furnace Recipe Example:


```json
{
"format_version": "1.12",
"minecraft:recipe_furnace": {
"description": {
"identifier": "minecraft:furnace_beef"
},
"tags": ["furnace", "smoker", "campfire", "soul_campfire"],
"input": {
"item": "minecraft:beef",
"data": 0,
"count": 4
},
"output ": "minecraft:cooked_beef"
}
}
```


# Potion Brewing Container Recipe


Represents a Potion Brewing Container Recipe.


## Parameters


| Name | Type | Description |
| --- | --- | --- |
| input | potion | input potion used in the brewing container recipe. |
| output | potion | output potion from the brewing container recipe. |
| reagent | item | item used in the brewing container recipe with the input potion. |
| tags | array of strings | Item used in a Brewing Container Recipe. |


## Potion Brewing Container Recipe Example:


```json
{
"format_version": "1.12",
"minecraft:recipe_brewing_container": {
"description": {
  "identifier": "minecraft:brew_potion_sulphur"
  },
 
 "tags": [ "brewing_stand" ],
 
  "input": "minecraft:potion",
  "reagent": "minecraft:gunpowder",
  "output": "minecraft:splash_potion"
  }
}
```


# Potion Brewing Mix


Represents a Potion Brewing Mix.


## Parameters


| Name | Type | Description |
| --- | --- | --- |
| input | potion | input potion used on the brewing stand. |
| output | potion | output potion from mixing the input potion with the reagent on the brewing stand. |
| reagent | item | item used to mix with the input potion. |
| tags | array of strings | Item used to make a brewing mix. |


## Potion Brewing Mix Example:


```json
{
"format_version": "1.12",
"minecraft:recipe_brewing_mix": {
"description": {
  "identifier": "minecraft:brew_awkward_blaze_powder"
  },
 
 "tags": [ "brewing_stand" ],
 
  "input": "minecraft:potion_type:awkward",
  "reagent": "minecraft:blaze_powder",
  "output": "minecraft:potion_type:strength"
  }
}
```


# Shaped Recipe


Represents a shaped crafting recipe for a crafting table.


The key used in the pattern may be any single character except the 'space' character, which is reserved for empty slots in a recipe.


## Parameters


| Name | Type | Description |
| --- | --- | --- |
| assume_symmetry | boolean | determines if the recipe should assume symmetry or not. |
| key | array of key and item pairs | patten key character mapped to item names. |
| pattern | array of strings | characters that represent a pattern to be defined by keys. |
| priority | integer | Item used as output for the furnace recipe. |
| result | array of item names | when input items match the pattern then these items are the result. |
| tags | array of strings | Item used as input for the furnace recipe. |


## Shaped Recipe Example:


```json
{
"format_version": "1.12",
"minecraft:recipe_shaped": {
"description": {
  "identifier": "minecraft:acacia_boat"
  },
"tags": [ "crafting_table" ],
"pattern": [
        "#P#",
        "###"
        ],
  "key": {
    "P": {
      "item": "minecraft:wooden_shovel"
    },
    "#": {
      "item": "minecraft:planks",
      "data": 4
      }
    },
"result": {
    "item": "minecraft:boat",
    "data": 4
    }
  }
}
```


# Shapeless Recipe


Represents a shapeless crafting recipe.


## Parameters


| Name | Type | Description |
| --- | --- | --- |
| ingredients | array of item names | items used as input (without a shape) for the recipe. |
| priority | integer | Item used as output for the furnace recipe. |
| result | array of item names | these items are the result. |
| tags | array of strings | Item used as input for the furnace recipe. |


## Shapeless Recipe Example:


```json
{
"format_version": "1.12",
"minecraft:recipe_shapeless": {
"description": {
  "identifier": "minecraft:firecharge_coal_sulphur"
  },
 "priority": 0,
 "ingredients": {
      "item": "minecraft:fireball",
      "data": 0,
      "count": 4
 },
"result": {
      "item": "minecraft:blaze_powder",
      "data": 4
      }
  }
}
```


# Smithing Transform Recipe


Represents a Smithing Transform Recipe for the Smithing Table.


This recipe transforms an item into another one, while retaining its properties.


## Parameters


| Name | Type | Description |
| --- | --- | --- |
| addition | item name | The material needed to perform the transform operation. In case of stackable items, only 1 item is consumed. The only accepted item is "minecraft:netherite_ingot". Items must have the "minecraft:transform_materials" tag to be accepted into the respective UI slot. |
| base | item name | The item to transform. Its properties will be copied to "result". The only accepted items are armor and tools. Items must have the "minecraft:transformable_items" tag to be accepted into the respective UI slot. |
| result | item name | The item to transform to. |
| tags | array of strings | The crafting stations the recipe is compatible with. The only accepted value is "smithing_table". |
| template | item name | The template needed to perform the transform operation. In case of stackable items, only 1 item is consumed. Items must have the "minecraft:transform_templates" tag to be accepted into the respective UI slot. |


## Smithing Transform Recipe Example:


```json
{
 "format_version": "1.12",
 "minecraft:recipe_smithing_transform": {
  "description": {
    "identifier": "minecraft:smithing_netherite_boots"
   },
  
  "tags": [ "smithing_table" ],
  
   "template": "minecraft:netherite_upgrade_smithing_template",
   "base": "minecraft:diamond_boots",
   "addition": "minecraft:netherite_ingot",
   "result": "minecraft:netherite_boots"
 }
}
```


# Smithing Trim Recipe


Represents a Smithing Trim Recipe for the Smithing Table.


This recipe applies a colored trim pattern to an item, while preserving its other properties.


## Parameters


| Name | Type | Description |
| --- | --- | --- |
| addition | item name or item tag | The material needed to perform the trim operation. It defines the color in which the pattern will be applied to the item. In case of stackable items, only 1 item is consumed. Items must have the "minecraft:trim_materials" tag to be accepted into the respective UI slot. |
| base | item name or item tag | The item to trim. Its properties will be preserved. The only accepted items are armors. Items must have the "minecraft:trimmable_armors" tag to be accepted into the respective UI slot. |
| tags | array of strings | The crafting stations the recipe is compatible with. The only accepted value is "smithing_table". |
| template | item name or item tag | The template needed to perform the trim operation. It defines the pattern which will be applied to the item. In case of stackable items, only 1 item is consumed. Items must have the "minecraft:trim_templates" tag to be accepted into the respective UI slot. |


## Smithing Trim Recipe Example:


```json
{
 "format_version": "1.12",
 "minecraft:recipe_smithing_trim": {
  "description": {
    "identifier": "minecraft:smithing_diamond_boots_jungle_quartz_trim"
   },
  
  "tags": [ "smithing_table" ],
  
   "template": "minecraft:jungle_temple_smithing_template",
   "base": "minecraft:diamond_boots",
   "addition": "minecraft:quartz",
 }
}
```
