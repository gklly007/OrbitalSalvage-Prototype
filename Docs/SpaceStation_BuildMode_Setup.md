# Space Station Build Mode - Setup Guide

## Overview
This document covers the complete setup process for the grid-based building system in the Space Station management game variant.

---

## Phase B: Grid Building System - Complete Setup

### 1. Blueprint Class Creation

All Blueprint classes are located in: `Content/Variant_SpaceStation/Blueprints/`

#### Created Blueprints:
- **BP_SpaceStationGameMode** - Parent: ASpaceStationGameMode (C++)
- **BP_SpaceStationPawn** - Parent: ASpaceStationPawn (C++)
- **BP_SpaceStationPlayerController** - Parent: ASpaceStationPlayerController (C++)
- **BP_StationGrid** - Parent: AStationGrid (C++)
- **BP_StationModule_Corridor** - Parent: AStationModule (C++)

#### Blueprint Creation Issues Fixed:
**Problem:** Blueprints showing "missing or NULL parent class" errors
**Solution:**
1. Close Unreal Editor completely
2. Right-click on `TestGame4.uproject` → "Generate Visual Studio project files"
3. Wait for generation to complete
4. Open `TestGame4.uproject` (triggers automatic recompile)
5. All C++ parent classes now load correctly

---

### 2. Test Level Setup (LVL_SpaceStation)

#### Floor Plane (Required for Raycasting)
The build system uses cursor raycasting to position the preview module. Without a floor, raycasts fail.

**Setup:**
1. Window → Details (if not visible)
2. Add a Cube from Place Actors panel
3. Select cube, set Transform in Details panel:
   - **Location:** X=0, Y=0, Z=0
   - **Scale:** X=100, Y=100, Z=0.1 (makes it a flat floor)
4. Under Collision section:
   - **Collision Enabled:** Query and Physics (or Query Only)
   - Ensure Visibility trace channel is enabled

#### Player Start (Required for Spawning)
**Setup:**
1. Place Actors panel → Search "Player Start"
2. Drag into viewport
3. Set Location: X=0, Y=0, Z=200 (above the floor)

#### World Settings Configuration
1. Settings (toolbar) → World Settings
2. Set Game Mode values:
   - **GameMode Override:** BP_SpaceStationGameMode
   - **Default Pawn Class:** BP_SpaceStationPawn
   - **Player Controller Class:** BP_SpaceStationPlayerController

---

### 3. Blueprint Configuration

#### BP_StationGrid Setup

**CRITICAL FIX - Grid Color:**
The grid debug lines were invisible because the color alpha channel was set to 0.

**Steps to Fix:**
1. Open BP_StationGrid (double-click in Content Browser)
2. Click "Class Defaults" button in toolbar (looks like a document/list icon)
3. In Details panel, find Debug → Grid Color
4. Set values:
   - **R = 0**
   - **G = 255** (green)
   - **B = 0**
   - **A = 255** (CRITICAL: This is alpha/opacity - must be 255 to be visible!)
5. Compile and Save

**Other Settings:**
- Tile Size: 200 (default, correct)
- Grid Origin: 0, 0, 0 (default, correct)
- Show Grid: TRUE (checked)
- Debug Grid Size: 20 (default, correct)

#### BP_StationModule_Corridor Setup

The preview module needs a mesh to be visible.

**Steps:**
1. Open BP_StationModule_Corridor
2. Click on "MeshComponent" in Components panel (left side)
3. In Details panel (right), find "Static Mesh"
4. Set to: **Cube** (or any basic cube mesh)
5. Compile and Save

---

### 4. Level Blueprint Setup

The Level Blueprint automatically enters build mode when the level starts (for testing purposes).

**Node Chain:**
```
Event BeginPlay → Get Player Controller → Cast To SpaceStationPlayerController → Enter Build Mode
```

**Detailed Steps:**
1. Open Level Blueprint (Blueprints → Open Level Blueprint)

2. **Event BeginPlay** (should already exist)

3. **Get Player Controller:**
   - Drag from BeginPlay execution pin (white), search "get player controller"
   - Select the variant with "Player Index" parameter (NOT Target, Device Id, or User Id variants)
   - Set Player Index = 0

4. **Cast To SpaceStationPlayerController:**
   - Drag from Get Player Controller execution pin, search "cast to spacestation"
   - Connect Return Value (blue) from Get Player Controller to Object (blue) on Cast node

5. **Enter Build Mode:**
   - Drag from Cast execution pin, search "enter build mode"
   - Ensure it shows "Target is Space Station Player Controller"
   - Set **Module Class = BP_StationModule_Corridor**
   - Connect Target (blue) from Cast's "As Space Station Player Controller" output

6. Compile and Save

**Common Blueprint Issues:**
- **Red pin won't connect to green pin:** You're trying to connect incompatible types (execution vs data pins). White pins are execution, colored pins are data.
- **"Enter Build Mode" not appearing in search:** Make sure you're dragging from the blue "As Space Station Player Controller" pin, and uncheck "Context Sensitive" in the search menu.
- **ERROR on Cast nodes:** The blue data pin isn't connected. Connect Return Value from Get Player Controller to Object on Cast node.

---

## Testing the Build Mode

### Expected Behavior
When you press Play (Alt+P):

1. **Green grid lines** appear on the floor (200cm apart)
2. **Semi-transparent cube** (preview module) appears and follows mouse cursor
3. Preview **snaps to grid** as you move the mouse
4. Preview turns **green** when placement is valid
5. Preview turns **red** when placement is invalid (overlapping, etc.)

### Camera Controls
- Isometric view angle (-35° pitch, 45° yaw)
- Zoom range: 1500-4000 units
- Perspective projection

### Debug Checklist
If something isn't working, check these in the Outliner (Window → Outliner) during Play:

**Should be present:**
- ✅ BP_SpaceStationGameMode0
- ✅ BP_SpaceStationPawn0
- ✅ BP_SpaceStationPlayerController0
- ✅ BP_StationGrid0
- ✅ BP_StationModule_Corridor (preview - only appears if build mode activated)
- ✅ Plane (floor)
- ✅ PlayerStart

**If preview module is missing:**
- Check Level Blueprint's Enter Build Mode node has Module Class set
- Check BP_StationModule_Corridor has a mesh assigned

**If grid lines are invisible:**
- Select BP_StationGrid0 in Outliner during Play
- Check Grid Color alpha channel (A) is set to 255 (not 0)
- Verify Show Grid is checked
- Fix permanently in BP_StationGrid's Class Defaults

---

## Files Reference

### C++ Classes (Phase A & B)
- `Source/TestGame4/Variant_SpaceStation/Gameplay/SpaceStationGameMode.h/.cpp`
- `Source/TestGame4/Variant_SpaceStation/Gameplay/SpaceStationPawn.h/.cpp`
- `Source/TestGame4/Variant_SpaceStation/Gameplay/SpaceStationPlayerController.h/.cpp`
- `Source/TestGame4/Variant_SpaceStation/Gameplay/StationGrid.h/.cpp`
- `Source/TestGame4/Variant_SpaceStation/Gameplay/StationModule.h/.cpp`

### Blueprint Classes (Phase B)
- `Content/Variant_SpaceStation/Blueprints/BP_SpaceStationGameMode.uasset`
- `Content/Variant_SpaceStation/Blueprints/BP_SpaceStationPawn.uasset`
- `Content/Variant_SpaceStation/Blueprints/BP_SpaceStationPlayerController.uasset`
- `Content/Variant_SpaceStation/Blueprints/BP_StationGrid.uasset`
- `Content/Variant_SpaceStation/Blueprints/BP_StationModule_Corridor.uasset`

### Levels
- `Content/Variant_SpaceStation/Maps/LVL_SpaceStation.umap`

---

## Next Steps (Phase C - Pending)

1. **Create Enhanced Input Actions and Mapping Context**
   - Build mode toggle action
   - Rotate module action
   - Place module action (left-click)
   - Cancel build mode action (right-click or ESC)
   - Camera zoom/pan actions

2. **Add Module Types**
   - Corridor (already created for testing)
   - Power Generator
   - Life Support
   - Living Quarters
   - Storage

3. **Implement Module Functionality**
   - Power generation/consumption
   - Oxygen generation/consumption
   - Crew assignment
   - Storage capacity

4. **Polish Build System**
   - Better visual feedback for connectivity
   - Module rotation preview
   - Cost/resource requirements
   - Build mode UI

---

## Troubleshooting Guide

### Issue: "Blueprints have missing or NULL parent class"
**Cause:** C++ classes not compiled or loaded
**Fix:**
1. Close Unreal Editor
2. Right-click TestGame4.uproject → Generate Visual Studio project files
3. Open TestGame4.uproject (will auto-compile)

### Issue: Grid lines not visible
**Cause:** Grid Color alpha channel set to 0 (transparent)
**Fix:**
1. Open BP_StationGrid
2. Click Class Defaults
3. Debug → Grid Color → Set A = 255
4. Compile and Save

### Issue: Preview module not appearing
**Cause 1:** Module Class not set in Level Blueprint
**Fix:** Open Level Blueprint, set Enter Build Mode's Module Class to BP_StationModule_Corridor

**Cause 2:** Module has no mesh
**Fix:** Open BP_StationModule_Corridor, set MeshComponent's Static Mesh to Cube

### Issue: Player doesn't spawn
**Cause:** No Player Start in level
**Fix:** Add Player Start from Place Actors panel, position at Z=200

### Issue: Raycast fails / Preview doesn't follow cursor
**Cause:** No floor plane for raycast to hit
**Fix:** Add scaled cube (X=100, Y=100, Z=0.1) as floor at origin with collision enabled

---

## Phase B Status: ✅ COMPLETE

**Completed Tasks:**
- ✅ C++ grid building system (Phase B)
- ✅ Blueprint class creation
- ✅ Test level setup (floor, player start, world settings)
- ✅ Grid visualization (debug lines)
- ✅ Build mode preview system
- ✅ Grid snapping and validation
- ✅ Fixed grid color visibility issue
- ✅ Fixed Blueprint parent class issues
- ✅ Level Blueprint auto-enter build mode for testing

**Next Phase:** Enhanced Input system and additional module types
