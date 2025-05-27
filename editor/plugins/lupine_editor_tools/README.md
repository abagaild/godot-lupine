# Lupine Editor Tools

RPG Maker-style visual editor tools for the Lupine Engine, providing intuitive drag-and-drop interfaces for creating narrative-focused games.

## Overview

The Lupine Editor Tools extend Godot's editor with specialized visual tools designed for rapid game development. These tools generate clean, maintainable code while providing a user-friendly interface for non-programmers.

## Tools Included

### 1. World Builder
**Purpose**: Visual scene composition with drag-and-drop entity placement

**Features**:
- Entity palette with categorized game objects (Characters, Interactive, Environment, Logic)
- Visual scene canvas with grid snapping and gizmos
- Property panel for configuring selected entities
- Real-time preview of entity placement
- Automatic code generation for placed entities

**Usage**:
1. Open the Lupine Tools panel from the toolbar
2. Select the "World Builder" tab
3. Choose entities from the palette on the left
4. Click on the scene canvas to place entities
5. Select placed entities to edit their properties

### 2. Entity Editor
**Purpose**: Configure characters, NPCs, enemies, and items

**Features**:
- Tabbed interface for different entity types
- Visual property editing with appropriate controls
- Real-time updates to entity configurations
- Integration with existing Lupine templates

### 3. Dialogue Editor
**Purpose**: Node-based visual dialogue and conversation design

**Features**:
- Visual dialogue tree editor with drag-and-drop nodes
- Character integration and portrait management
- Branching conversation paths
- Export to Lupine dialogue format

## Architecture

### Plugin Structure
```
lupine_editor_tools/
├── lupine_editor_plugin.h/cpp     # Main plugin and tool manager
├── lupine_world_builder.h/cpp     # World Builder implementation
├── lupine_entity_editor.h/cpp     # Entity Editor implementation
├── lupine_dialogue_editor.h/cpp   # Dialogue Editor implementation
└── SCsub                          # Build configuration
```

### Integration Points
- **Template System**: Tools automatically configure based on selected Lupine template
- **Module System**: Tools generate appropriate module configurations
- **Asset Pipeline**: Seamless integration with Godot's import system
- **Code Generation**: All visual edits produce readable, maintainable code

## Entity Library

The World Builder includes a comprehensive entity library with pre-configured game objects:

### Characters
- **Basic NPC**: Non-player character with dialogue capabilities
- **Basic Enemy**: Enemy with AI and combat capabilities

### Interactive Objects
- **Treasure Chest**: Container for items and gold
- **Door**: Transition between scenes with lock/key support

### Environment
- **Tree**: Decorative outdoor environment object
- **Rock**: Decorative rock formation

### Logic
- **Trigger Zone**: Invisible area that triggers events
- **Spawn Point**: Player or entity spawn location

## Code Generation

All visual editor changes generate clean, readable code that follows Godot best practices:

- Entity placement creates scene instances with proper positioning
- Property changes update node metadata and custom properties
- Generated code is version control friendly
- Full compatibility with manual code editing

## Getting Started

1. **Enable the Plugin**: The Lupine Editor Tools are automatically enabled when building the Lupine Engine
2. **Open Tools**: Click the "Lupine Tools" button in the editor toolbar
3. **Create a Scene**: Open or create a scene to work with
4. **Start Building**: Use the World Builder to place entities and configure your game world

## Best Practices

- **Use Grid Snapping**: Enable grid snapping for consistent entity placement
- **Organize by Category**: Use the entity palette categories to find objects quickly
- **Preview Changes**: Use the scene canvas to preview entity placement before finalizing
- **Save Frequently**: The tools integrate with Godot's save system for automatic backups

## Extending the Tools

The Lupine Editor Tools are designed to be extensible:

- **Custom Entities**: Add new entity types by extending the entity library
- **New Tools**: Create additional tools by inheriting from `LupineEditorTool`
- **Custom Properties**: Add specialized property editors for unique game mechanics
- **Integration**: Connect tools with existing Lupine modules and templates

## Troubleshooting

### Common Issues

**Tool Panel Not Visible**:
- Ensure the Lupine Tools button is pressed in the toolbar
- Check that a scene is open and selected

**Entities Not Placing**:
- Verify the entity scene files exist in the expected paths
- Check the console for error messages about missing resources

**Properties Not Updating**:
- Ensure the target node is properly selected
- Check that property names match expected metadata keys

### Debug Information

The tools provide debug output in the Godot console:
- Entity placement confirmations
- Property change notifications
- Error messages for missing resources

## Future Enhancements

Planned improvements for the Lupine Editor Tools:

- **Advanced Canvas**: Zoom, pan, and multi-selection support
- **Custom Gizmos**: Specialized visual indicators for different entity types
- **Undo/Redo**: Full undo/redo support for all tool operations
- **Templates**: Save and load entity placement templates
- **Collaboration**: Multi-user editing support for team development

---

For more information about the Lupine Engine and its templates, see the main project documentation.
