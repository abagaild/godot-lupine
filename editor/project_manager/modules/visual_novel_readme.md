# Visual Novel Template - Lupine Engine

## Overview

The Lupine Engine Visual Novel Template provides a complete, writer-forward framework for creating visual novels with rich storytelling features. This template includes a custom script format designed to be intuitive for writers while providing powerful branching narrative capabilities.

## Features

- **Writer-Forward Script Format**: Easy-to-learn syntax for dialogue, choices, and scene control
- **Character Portrait System**: Dynamic character display with emotion support
- **Background Management**: Scene backgrounds with transition effects
- **Audio System**: Music and sound effects with crossfading
- **Save/Load System**: Multiple save slots with auto-save functionality
- **Choice System**: Branching narratives with conditional choices
- **Asset Discovery**: Automatic detection of portraits, backgrounds, and audio files

## Script Format Guide

### Basic Structure

Visual novel scripts use a node-based format where each dialogue segment is a numbered node:

```
NodeID
Speaker
Dialogue text
[Next Node or Choice]
```

### Example Script

```
FN : Conversation

[[background school_courtyard]]
[[setLeft Char1_neutral]]
[[setRight Char2_neutral]]

1_1
Char2
Hey Char1! How are you doing?
[Good!|2_1]
[Bad!|2_2]

2_1
Char2_happy
That's great!
[[var mood = good]]
[3_1]

2_2
Char2_sad
Oh, I'm sorry
[[var mood = bad]]
[3_1]

3_1 if mood = good
Char2_happy
I'm doing good as well!
[end]

3_1 if mood = bad
Char2_sad
At least I'm doing better...
[end]
```

### Commands

Commands are enclosed in double brackets `[[command]]`:

- `[[background name]]` - Set scene background
- `[[setLeft/Center/Right character_emotion]]` - Position character portraits
- `[[playMusic track]]` - Play background music with looping
- `[[stopMusic]]` - Stop current music (optional: `force` for immediate stop)
- `[[crossMusic track]]` - Crossfade to new music
- `[[playSound effect]]` - Play sound effect
- `[[showKeyImage image]]` - Display key artwork/CG
- `[[clearKeyImage]]` - Hide key artwork
- `[[transitionEffect effect]]` - Play screen transition effect
- `[[characterEffect effect,position]]` - Play character animation
- `[[var name = value]]` - Set story variable
- `[[signal name]]` - Emit custom signal for game logic

### Character Names

Character names are automatically formatted:
- `DamienWayne` becomes "Damien Wayne"
- `DamienWayne_happy` uses the happy portrait and displays "Damien Wayne"

### Choices and Branching

Choices use the format `[Choice Text|Target Node]`:

```
1_1
Character
What would you like to do?
[Go left|left_path]
[Go right|right_path]
[Stay here|stay_here]
```

### Conditional Nodes

Nodes can have conditions based on story variables:

```
3_1 if mood = good
Character_happy
Everything is wonderful!
[end]

3_1 if mood = bad
Character_sad
Things could be better...
[end]
```

Supported operators: `=`, `>`, `<`, `>=`, `<=`, `and`, `or`

### Functions

Group related scenes using functions:

```
FN : SchoolScene

# Nodes for school scene here...

FN : HomeScene

# Nodes for home scene here...
```

## Asset Organization

### Folder Structure

```
assets/
├── backgrounds/        # Scene backgrounds (.png, .jpg, .webp)
├── portraits/         # Character portraits (.png, .jpg, .webp)
│   ├── character1_neutral.png
│   ├── character1_happy.png
│   └── character1_sad.png
├── music/            # Background music (.mp3, .wav, .ogg)
└── soundEffects/     # Sound effects (.wav, .mp3, .ogg)
```

### Asset Discovery

The engine automatically finds assets by name:
- `[[background farmhouse]]` finds `assets/backgrounds/farmhouse.png`
- `[[playMusic peaceful]]` finds `assets/music/peaceful.mp3`
- Character portraits use fallback: `Char1_happy` → `Char1_neutral` → `Char1`

## Controls

### Player Controls
- **Space/Enter**: Continue dialogue
- **Mouse Click**: Continue dialogue or skip text animation
- **Number Keys (1-9)**: Quick choice selection
- **Escape**: Toggle main menu
- **Ctrl+S**: Quick save
- **Ctrl+L**: Quick load
- **F5**: Open save/load menu

### Menu Navigation
- **New Game**: Start from the beginning
- **Load Game**: Access save slots
- **Settings**: Game configuration (if implemented)
- **Quit**: Exit game

## Getting Started

1. **Create Your Script**: Add `.vn` files to `data/dialogue/`
2. **Add Assets**: Place images and audio in appropriate `assets/` folders
3. **Test Your Story**: Run the game and use the main menu to start
4. **Iterate**: Use the save/load system to test different story branches

## Advanced Features

### Custom Signals

Use `[[signal name]]` to trigger custom game logic:

```gdscript
# In your game script
func _ready():
    VNScriptParser.connect("signal_emitted", _on_vn_signal)

func _on_vn_signal(signal_name: String, args: Array):
    match signal_name:
        "playerWon":
            # Handle victory
        "playerLost":
            # Handle defeat
```

### Variable System

Track story state with variables:

```
[[var relationship_points = 0]]
[[var has_key = true]]
[[var current_chapter = 1]]
```

Use in conditions:

```
door_scene if has_key = true
Narrator
You unlock the door with your key.
[enter_room]

door_scene if has_key = false
Narrator
The door is locked. You need a key.
[look_around]
```

## Tips for Writers

1. **Plan Your Structure**: Sketch your story flow before writing
2. **Use Meaningful Node IDs**: `chapter1_intro` is better than `1_1`
3. **Test Frequently**: Use the save/load system to test different paths
4. **Organize Assets**: Keep consistent naming for easy asset discovery
5. **Use Variables**: Track important story state for complex narratives

## Technical Notes

- Scripts are parsed at runtime for easy iteration
- All systems are modular and can be extended
- Save files include complete game state for reliable loading
- Asset caching improves performance on subsequent loads
- The engine supports both linear and complex branching narratives

For more advanced customization, see the generated script files in your project's `globals/` and `scripts/` directories.
