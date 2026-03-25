# Entities

## Format

An entity entry has the following format:

```nohighlight
{NAME}

{HP};{MANA};{MANA_REGEN};{ARMOR};

{SKILL_NAME}:{SKILL_VALUE},
END:

{ACTION_NAME},
END,

{DELIM}
```

***

Name        | Value   | Description
:-----------|:-------:|:-----------
NAME        | String  | The name of the entity.
HP          | Integer | The entity's base HP.
MANA        | Integer | The entity's base Mana.
MANA_REGEN  | Integer | The entity's base Mana Regeneration.
ARMOR       | Integer | The entity's base Armor.
SKILL_NAME  | String  | The name of the skill.
SKILL_VALUE | Integer | The level in the skill.
ACTION_NAME | String  | The name of the action.
DELIM       | String  | CONTINUE if there is another entry in the file, FINAL to finish the parsing.

***

There can be any number of `{SKILL_NAME}:{SKILL_VALUE},` before the `END:` for the skills.

There can be any number of `{ACTION_NAME},` before the `END,` for the actions.

The file should always end with a `FINAL` delim.

## Example

### entities.data

```nohighlight
Goblin

32;0;0;6;

END:

END,

CONTINUE

Revenant

80;60;8;10;

END:

END,

FINAL
```
