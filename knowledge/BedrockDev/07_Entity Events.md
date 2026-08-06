# ENTITY EVENTS DOCUMENTATION Version: 1.21.120.4


## Index


# This describes the structure of the Events section.


# Overview


Entity events can be structured by a combination of 'sequence' and 'randomize' nodes.


'sequence' nodes are array nodes and will execute all entries in order from first element to last.


'randomize' nodes are array nodes that will pick one entry to execute, based on a weight.


'filters' can also be added within 'sequence' and 'randomize' nodes to restrict execution.


Within 'randomize' and 'sequence' nodes, you can specify a few operations.


'trigger', 'filters', 'add', and 'remove'.


You can read about 'filters' in the 'Filters' section of the documentation.


'trigger' can be used to fire additional entity events when an event is hit.


'add' can be used to add component groups to your entity.


'remove' can be used to remove component groups from your entity.


When an event is received, the effects of that event are determined immediately, but those changes


are not applied to the entity until the entity ticks on the server side of the game. This means


filters in later entries in a 'sequence' array won't see changes from earlier in that array.


It also means that when one entity sends an event to another entity, it could take effect on the same


game tick or on the next tick, depending on whether the target entity has already been updated.


# Versioned Changes


A 'format_version' of '1.19.20' or higher is required to properly evaluate filters specified on an entity event definition


at the root level of the event, that is any filter that is not underneath a 'sequence' or 'randomize' node.


Content with a lower version will use the old behavior, which was to ignore root level filters.


# Randomize Node


The 'randomize' node is an array node that will pick one entry to execute, based on a weight.


If no weight is specified, a node will have a weight of 1.0.


If you add a weight of 4.0 in one node, and 8.0 in another, then those nodes will have a 33.33% (4 / (4 + 8)) and 66.66% (8 / (4 + 8)) chance of executing, respectively.


Example:


```json
"randomize": [
      {
        "weight": <float>
        // actions like 'add' or 'remove'
      }
    ]
```


# Sequence Node


Example:


```json
"sequence": [
      {
        // I will execute first! c:
      },
      {
        // I will execute last! :c
      }
    ]
```


# Trigger


Triggers additional entity events when hit. For example, you could use a randomize node in minecraft:entity_spawned to choose either an adult or baby event for adding component groups.


Example:


```json
"sample:spawn_adult": {
      // add adult component groups
    },
    "sample:spawn_baby": {
      // add baby component groups
    },
    "minecraft:entity_spawned": {
      "randomize": [
        {
          "weight": 50.0,
          "trigger": "sample:spawn_adult"
        },
        {
          "weight": 50.0,
          "trigger": "sample:spawn_baby"
        }
      ]
    }
```


# Add Component Group


Adds component groups to the current entity. These groups must be defined in the 'component_groups' section of the file. As entities can only have one component of each type active, any components in a group that is being added will replace previously added components. Additionally, adding a component group that is already active will cause those components to be re-initialized. For some types of components like minecraft:is_baby, re-initializing an already active component has no effect, but for other component types the associated logic will start over. For example, an already-added minecraft:timer that is added again will start its timing logic over.


Example:


```json
"sequence": [
      {
        "add": { "component_groups": [ "one" ] }
      },
      {
        "add": { "component_groups": [ "two", "five", "etc.." ] }
      }
    ]
```


# Remove Component Group


Removes component groups from the current entity. This can be any group you have defined in the 'component_groups' section of the file.


Example:


```json
"sequence": [
      {
        "remove": { "component_groups": [ "one" ] }
      },
      {
        "remove": { "component_groups": [ "two", "five", "etc.." ] }
      }
    ]
```


# Set Entity Property


Sets the value of an entity property. The property must be defined in the 'properties' section of the file.


Example:


```json
"set_property": {
      "minecraft:has_nectar": false
    }
```


# Queue Command


Queues a command to be run on the entity. The command will run within the next tick unless the entity has been removed.


Example:


```json
"queue_command": {
      "command": "say I have died!"
    }
```
