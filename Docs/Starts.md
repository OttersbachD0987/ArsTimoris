# Starts

## Format

A start entry has the following format:

```nohighlight
{NAME}
{DESCRIPTION}

{HP};{MANA};{MANA_REGEN};{ARMOR};

{SKILL_NAME}:{SKILL_VALUE},
END:

{ACTION_NAME},
END,

{CLASS_NAME}:{CLASS_LEVEL},
END:

{PERK_NAME},
END,

{ITEM_ID}x{ITEM_COUNT};{ITEM_METADATA},
END,

{TURNS};{START_ROOM_ID};{XP};{GOLD};

{DELIM}
```

***

Name          | Value   | Description
:-------------|:-------:|:-----------
NAME          | String  | The name of the start.
DESCRIPTION   | String  | The description of the start.
HP            | Integer | The player's starting HP.
MANA          | Integer | The player's starting Mana.
MANA_REGEN    | Integer | The player's starting Mana Regeneration.
ARMOR         | Integer | The player's starting Armor.
SKILL_NAME    | String  | The name of the skill.
SKILL_VALUE   | Integer | The level in the skill.
ACTION_NAME   | String  | The name of the action.
CLASS_NAME    | String  | The name of the class.
CLASS_LEVEL   | Integer | The level in the class.
PERK_NAME     | String  | The name of the perk.
ITEM_ID       | Integer | The id of the item.
ITEM_COUNT    | Integer | The size of the item stack.
ITEM_METADATA | String  | The metadata of the item.
TURNS         | Integer | The player's starting Turns.
START_ROOM_ID | Integer | The id of the room the player starts in.
XP            | Integer | The player's starting XP.
GOLD          | Integer | The player's starting Gold.
DELIM         | String  | CONTINUE if there is another entry in the file, FINAL to finish the parsing.

***

There can be any number of `{SKILL_NAME}:{SKILL_VALUE},` before the `END:` for the skills.

There can be any number of `{ACTION_NAME},` before the `END,` for the actions.

There can be any number of `{CLASS_NAME}:{CLASS_LEVEL},` before the `END:` for the classes.

There can be any number of `{PERK_NAME},` before the `END,` for the perks.

There can be any number of `{ITEM_ID}x{ITEM_COUNT};{ITEM_METADATA},`before the `END,` for items.

The file should always end with a `FINAL` delim.

## Example

### starts.data

```nohighlight
Default
A simple start to the game, good for beginners.

100;100;10;10;

Martial Combat:2,
Unarmed Combat:1,
Brawling:1,
END:

Punch,
END,

Fighter:1,
END:

INSIGHT,
END,

1x4;,
6x1;,
END,

1;0;0;0;

CONTINUE

Normal
Similar to Default, but with less extra help.

100;100;10;10;

Martial Combat:2,
END:

Punch,
END,

Fighter:1,
END:

END,

1x2;,
7x1;,
END,

1;0;0;0;

CONTINUE

Trivial
A trivial start to the game, good for people bad at the game.

150;150;10;12;

Martial Combat:3,
Unarmed Combat:3,
Brawling:3,
END:

Punch,
END,

Fighter:1,
END:

INSIGHT,
HORDE_SLAYER,
END,

1x16;,
2x16;,
3x5;,
4x5;,
5x1(ToHit:2),
10x1;,
END,

1;0;0;0;

CONTINUE

Wretch
This is hard mode, no mana, no perks, no items, no classes.

100;0;0;10;

END:

Punch,
END,

END:

END,

END,

1;0;0;0;

CONTINUE

Occultist
They say that the Ars Puppetarii Mortui hold secrets not meant for mortals, you think they're just stupid.

65;175;15;10;

Knowledge of Death:2,
Necromancy:1,
END:

Punch,
END,

Necromancer:1,
END:

END,

8x1;,
9x1;,
END,

1;0;0;0;

FINAL

```
