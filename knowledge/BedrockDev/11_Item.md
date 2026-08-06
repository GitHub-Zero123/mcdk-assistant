# ITEM DOCUMENTATION Version: 1.21.120.4


## Index


# Items


Items are defined in a JSON file inside a behavior pack.


All attributes, including item names, must be defined using item components.


# Item Definition Properties


Properties are part of the Item Definition. They help the system determine how to parse and initialize the item.


| Name | Type | Default Value | Description |
| --- | --- | --- | --- |
| format_version |  |  | Specifies the version of the game this entity was made in. If the version is lower than the current version, any changes made to the entity in the vanilla version will be applied to it. |
| minecraft:item |  |  | Item definition includes the "description" and "components" sections. |


## Code Example


Example


```json
{
  "format_version": "1.20.20",
  "minecraft:item": {
    "description": {
      "identifier": "minecraft:blaze_rod"
    },
    "components": {
      "minecraft:fuel": {
        "duration": 120.0
      },
      "minecraft:max_stack_size": 64,
      "minecraft:icon": {
        "texture": "blaze_rod"
      },
      "minecraft:hand_equipped": true,
      "minecraft:display_name": {
        "value": "Blaze Rod"
      }
    }
  }
}
```
