# Space Station Management Game - Implementation Plan

## Context

Creating a 2.5D top-down space station management game in UE5.7 inspired by space-sim mechanics. The game will feature:
- **Isometric camera** (45° view angle) for 2.5D perspective
- **Grid-based tile system** for station construction (like RimWorld/Barotrauma)
- **Balanced gameplay** with station building, crew simulation, and survival systems

The existing project has TwinStick and Strategy variants. We'll create a new "Variant_SpaceStation" following the established architectural pattern where each variant is completely independent with its own GameMode, PlayerController, and Pawn/Character classes.

## Architecture Overview

### Directory Structure
```
Source/TestGame4/Variant_SpaceStation/
├── Gameplay/           (Core game classes)
├── AI/                 (Crew AI)
├── Components/         (Reusable components)
└── UI/                 (HUD and widgets)

Content/Variant_SpaceStation/
├── Blueprints/         (BP implementations)
├── Input/Actions/      (Enhanced Input)
├── UI/                 (UMG widgets)
├── Materials/          (Module visuals)
└── LVL_SpaceStation.umap
```

### Core C++ Classes

1. **ASpaceStationPawn** - Floating camera platform with isometric view
   - SpringArm component at rotation (-35°, 45°, 0°) for isometric angle
   - Perspective camera (not orthographic) with 90° FOV
   - FloatingPawnMovement for camera panning
   - Zoom via SpringArm TargetArmLength (1500-4000 range)
   - Pattern: [StrategyPawn.h](d:\UE Projects\TestGame4\Source\TestGame4\Variant_Strategy\StrategyPawn.h)

2. **ASpaceStationGameMode** - Station management and resource tracking
   - Spawns StationGrid actor
   - Manages resource pools (Power, Oxygen, Food)
   - Maintains registries: `TArray<AStationModule*>` and `TArray<ACrewMember*>`
   - Pattern: [TwinStickGameMode.h](d:\UE Projects\TestGame4\Source\TestGame4\Variant_TwinStick\TwinStickGameMode.h)

3. **ASpaceStationPlayerController** - Input handling for building and crew commands
   - Build mode: Select module type, preview placement, rotate, finalize
   - Crew mode: Select crew, issue movement commands
   - Enhanced Input with separate contexts for building vs gameplay
   - Pattern: [StrategyPlayerController.h](d:\UE Projects\TestGame4\Source\TestGame4\Variant_Strategy\StrategyPlayerController.h)

4. **AStationGrid** - Grid-based building system manager
   - `TMap<FIntPoint, AStationModule*> GridMap` for sparse storage
   - TileSize = 200cm (2m tiles)
   - Functions: WorldToGrid(), GridToWorld(), IsValidPlacement(), PlaceModule()
   - Validates placement (unoccupied, connected to station, affordable)
   - Spawned by GameMode at BeginPlay

5. **AStationModule** - Base class for all placeable rooms/corridors
   - Grid properties: Size (FIntPoint), Position, Rotation (90° increments)
   - System properties: PowerGeneration/Consumption, OxygenGeneration/Consumption
   - Connectivity: ConnectionPoints (relative offsets), ConnectedModules array
   - Visual states: Preview mode (ghost), Valid/Invalid placement (green/red)
   - Blueprint events: BP_ModulePlaced(), BP_PowerStateChanged(), BP_ConnectionUpdated()
   - Pattern: Hybrid of [TwinStickProjectile](d:\UE Projects\TestGame4\Source\TestGame4\Variant_TwinStick\Gameplay\TwinStickProjectile.h) (visual feedback) and [StrategyUnit](d:\UE Projects\TestGame4\Source\TestGame4\Variant_Strategy\StrategyUnit.h) (selection)

6. **ACrewMember** - Simulated crew character with needs
   - Character-based (inherits ACharacter) with movement
   - Components: UCrewNeedsComponent (oxygen, food, sleep, health)
   - Selection state and visual feedback
   - AI commands: MoveToLocation(), MoveToModule(), InteractWithModule()
   - Current/Target module tracking
   - Pattern: [StrategyUnit](d:\UE Projects\TestGame4\Source\TestGame4\Variant_Strategy\StrategyUnit.h) (selection) + [TwinStickNPC](d:\UE Projects\TestGame4\Source\TestGame4\Variant_TwinStick\AI\TwinStickNPC.h) (AI)

7. **UStationSystemsComponent** - Station-wide system simulation
   - Attached to GameMode
   - Tracks: CurrentPower, PowerCapacity, PowerDemand (same for Oxygen)
   - Functions: TickSystems(), RecalculateResources(), PropagateAtmosphere()
   - Flood-fill algorithm for atmosphere from life support modules
   - Power/atmosphere failure states with Blueprint events

8. **UCrewNeedsComponent** - Individual crew survival needs
   - Attached to CrewMember
   - Need values (0-100): Oxygen, Food, Sleep, Health
   - Depletion rates (per second) configurable
   - Replenishment functions called during module interaction
   - Critical state checks (<20%) trigger AI behavior changes

9. **ACrewAIController** - StateTree-based AI behavior
   - Uses StateTree plugin (already enabled)
   - Need-based priority: Oxygen > Food > Sleep > Idle
   - FindNearestModuleOfType() to locate target
   - OnMoveCompleted() triggers module interaction
   - Pattern: [TwinStickAIController.h](d:\UE Projects\TestGame4\Source\TestGame4\Variant_TwinStick\AI\TwinStickAIController.h)

10. **ASpaceStationHUD** - UI overlay and visual feedback
    - Manages UI widgets (main HUD, build menu, crew panel)
    - Drawing: Module preview (green/red), selection boxes, connections
    - Pattern: [StrategyHUD.h](d:\UE Projects\TestGame4\Source\TestGame4\Variant_Strategy\UI\StrategyHUD.h)

## Implementation Sequence

### Phase A: Foundation (Week 1)
1. Create directory structure (source + content)
2. Update [TestGame4.Build.cs](d:\UE Projects\TestGame4\Source\TestGame4\TestGame4.Build.cs) with Variant_SpaceStation include paths
3. Create C++ class headers and minimal implementations:
   - SpaceStationPawn, SpaceStationGameMode, SpaceStationPlayerController
   - StationGrid, StationModule
   - Compile to verify structure

### Phase B: Grid Building System (Week 1-2)
1. **Implement Grid Core**
   - Complete WorldToGrid() / GridToWorld() conversions
   - Implement IsValidPlacement() with connectivity checks
   - Add debug grid visualization (DrawDebugLine)

2. **Implement Module Base**
   - Add StaticMeshComponent for visual representation
   - SetPreviewMode() for semi-transparent ghost
   - SetValidPlacement() for green/red material tinting
   - Grid and system properties (size, rotation, power, oxygen)

3. **Implement Building Controls**
   - EnterBuildMode() spawns preview module
   - UpdateBuildPreview() (called in Tick) - raycast cursor to grid
   - PlaceModule() validates and finalizes placement
   - RotatePreview() cycles through 90° rotations
   - ExitBuildMode() destroys preview

4. **Create Blueprint Modules**
   - BP_StationModule_Corridor (1x1, connects all sides)
   - BP_StationModule_PowerGenerator (2x2, generates power)
   - BP_StationModule_LifeSupport (2x2, generates oxygen)
   - BP_StationModule_Quarters (2x3, crew sleep area)
   - Use placeholder cube meshes initially
   - Create preview materials (M_ModulePreview_Valid/Invalid)

5. **Test Grid Placement**
   - Create LVL_SpaceStation.umap
   - Set GameMode in World Settings
   - Test: Enter build mode, place modules, validate placement rules

### Phase C: Module Connectivity & Systems (Week 2-3)
1. **Implement Connectivity**
   - Add ConnectionPoints (relative FIntPoint offsets) to modules
   - UpdateConnections() checks adjacent tiles for connected modules
   - Called when module placed or neighbors change
   - Visual feedback via BP_ConnectionUpdated event

2. **Implement StationSystemsComponent**
   - Add component to GameMode
   - RecalculateResources() sums all module power/oxygen contributions
   - Called when modules placed/removed
   - IsPowerAvailable() checks if generation meets demand

3. **Implement Power System**
   - SetPoweredState() updates module's bIsPowered flag
   - Visual feedback via BP_PowerStateChanged (lights on/off)
   - All modules update when RecalculateResources runs

4. **Implement Atmosphere System**
   - PropagateAtmosphere() uses flood-fill from powered life support modules
   - Sets bHasAtmosphere flag on connected modules
   - Crew can only replenish oxygen in modules with atmosphere

5. **Test Systems**
   - Place power generator → modules power up
   - Place life support → atmosphere propagates through connections
   - Remove modules → systems recalculate correctly

### Phase D: Crew System (Week 3-4)
1. **Implement CrewMember Class**
   - Character with selection properties
   - SetSelected() for visual feedback (selection ring)
   - MoveToLocation() and MoveToModule() commands
   - Current/target module tracking

2. **Implement CrewAIController**
   - AAIController with StateTree instance
   - OnPossess() initializes StateTree
   - OnMoveCompleted() handles arrival at target
   - FindNearestModuleOfType() helper function

3. **Implement CrewNeedsComponent**
   - Need values: Oxygen, Food, Sleep, Health (0-100)
   - TickNeeds() depletes values over time
   - ReplenishOxygen/Food/Sleep() called during module interaction
   - Critical state checks (<20%) trigger warnings

4. **Create Crew Blueprint**
   - BP_CrewMember with Mannequin mesh
   - Configure depletion rates
   - Implement selection visuals (BP_Selected/Deselected)
   - Implement need critical indicators (BP_NeedCritical)

5. **Implement Crew Selection**
   - Click detection (raycast to crew)
   - SelectCrew() manages selection array
   - CommandCrewMove() for right-click commands
   - Test: Spawn, select, command movement

### Phase E: Crew AI & Needs (Week 4-5)
1. **Create StateTree Asset**
   - ST_CrewBehavior in Blueprints/Crew/
   - Root selector: EvaluateNeeds
   - States: CriticalOxygen → SeekOxygen → UseLifeSupport
   - States: CriticalFood, CriticalSleep, Idle
   - Transitions based on need thresholds

2. **Implement Module Interaction**
   - InteractWithModule() checks module type
   - Life support → replenish oxygen (if powered + atmosphere)
   - Quarters → replenish sleep
   - Continuous replenishment while in interaction zone

3. **Integrate StateTree**
   - StateTree checks CrewNeedsComponent for current needs
   - Uses FindNearestModuleOfType() to select destination
   - Commands movement via MoveToModule()
   - Test: Let oxygen deplete, watch crew seek life support

4. **Test AI Loop**
   - Multiple crew members navigate independently
   - Needs deplete → crew seeks appropriate modules → needs replenish
   - Handle edge cases (module unreachable, all modules unpowered)

### Phase F: UI & Polish (Week 5-6)
1. **Implement HUD Class**
   - Create ASpaceStationHUD with widget management
   - DrawModulePreview() for green/red grid overlay
   - DrawConnectionLines() for module connectivity visualization

2. **Create Main HUD Widget**
   - WBP_SpaceStationHUD
   - Resource bar at top (power, oxygen, food)
   - Crew panel at bottom left (selected crew info, needs bars)
   - System warnings at center top
   - Update via timer (every 0.5s)

3. **Create Build Menu**
   - WBP_BuildMenu with module selection grid
   - Module icons, names, costs
   - Click to EnterBuildMode()
   - Toggle with B key
   - Grey out unaffordable modules

4. **Polish Visuals**
   - Replace cube placeholders with proper meshes
   - Materials with emissive for powered state
   - Particle effects for life support
   - Lighting that responds to power state
   - Connection visualization (pipes, doorways)

5. **Polish Camera & Controls**
   - Fine-tune isometric angle (may need -45° instead of -35°)
   - Smooth zoom interpolation (FInterpTo)
   - Camera bounds to prevent panning too far
   - Edge scrolling (optional)

### Phase G: Integration & Balancing (Week 6)
1. **Resource Balancing**
   - Tune module costs, power/oxygen generation rates
   - Tune crew needs depletion rates
   - Test: Can sustain 5 crew with reasonable station size?

2. **Gameplay Loop Testing**
   - Start with command center + basic resources
   - Build power → life support → quarters
   - Spawn crew and verify survival loop
   - Expand station organically
   - Test failure states (power loss, oxygen depletion)

3. **Edge Case Handling**
   - Module destroyed → disconnect and recalculate
   - Crew pathfinding failure → fallback behavior
   - All power lost → emergency visuals
   - Invalid placement feedback

4. **Performance Optimization**
   - Throttle UI updates (0.5s timer, not tick)
   - Profile tick costs (Unreal Insights)
   - Check for performance issues with many modules/crew

5. **Final Polish**
   - Sound effects (placement, power up, alarms)
   - Ambient music
   - Tutorial hints in UI
   - Pause menu

## Enhanced Input Configuration

### Input Actions to Create
```
Content/Variant_SpaceStation/Input/Actions/
- IA_CameraPan          (Vector2D, WASD)
- IA_CameraZoom         (Axis1D, Mouse Wheel)
- IA_ResetCamera        (Digital, Home)
- IA_Select             (Digital, Left Mouse)
- IA_PlaceModule        (Digital, Left Mouse in build mode)
- IA_CancelPlacement    (Digital, Right Mouse / ESC)
- IA_RotatePreview      (Digital, R)
- IA_CommandCrew        (Digital, Right Mouse)
- IA_OpenBuildMenu      (Digital, B)
```

### Input Mapping Context
**IMC_SpaceStation** bindings:
- WASD → IA_CameraPan
- Mouse Wheel → IA_CameraZoom
- Left Mouse → IA_Select
- R → IA_RotatePreview
- ESC → IA_CancelPlacement
- B → IA_OpenBuildMenu

## Key Design Decisions

1. **Fixed Grid System**: 200cm tiles for simplicity, cleaner pathfinding, easier connectivity
2. **Simplified Atmosphere**: Binary system (has atmosphere or doesn't) via flood-fill from life support
3. **Centralized Power**: Station-wide power pool, not per-module distribution
4. **StateTree for AI**: Modern UE5 approach, good for needs-based priority selection
5. **Blueprint-Heavy Content**: C++ for architecture, Blueprints for modules, UI, and iteration

## Critical Files to Reference

| File | Purpose |
|------|---------|
| [StrategyPawn.h:24-32](d:\UE Projects\TestGame4\Source\TestGame4\Variant_Strategy\StrategyPawn.h) | Camera setup pattern |
| [StrategyPlayerController.h:26-90](d:\UE Projects\TestGame4\Source\TestGame4\Variant_Strategy\StrategyPlayerController.h) | Input handling and mode switching |
| [TwinStickGameMode.h](d:\UE Projects\TestGame4\Source\TestGame4\Variant_TwinStick\TwinStickGameMode.h) | GameMode state management |
| [StrategyUnit.h](d:\UE Projects\TestGame4\Source\TestGame4\Variant_Strategy\StrategyUnit.h) | Selectable entity pattern |
| [TwinStickAIController.h](d:\UE Projects\TestGame4\Source\TestGame4\Variant_TwinStick\AI\TwinStickAIController.h) | StateTree AI pattern |
| [TestGame4.Build.cs](d:\UE Projects\TestGame4\Source\TestGame4\TestGame4.Build.cs) | Add Variant_SpaceStation paths |

## Testing Checklist

**Grid System:**
- [ ] Preview appears under cursor
- [ ] Preview snaps to grid correctly
- [ ] Preview shows valid/invalid (green/red)
- [ ] Rotation works (R key)
- [ ] Placement validates connectivity
- [ ] Can't place on occupied tiles

**Module Systems:**
- [ ] Modules register with GameMode
- [ ] Power generation/consumption calculates correctly
- [ ] Atmosphere propagates through connections
- [ ] Powered modules show visual difference
- [ ] UI displays correct resource values

**Crew:**
- [ ] Selection works (click to select)
- [ ] Movement works (right-click command)
- [ ] Needs deplete over time
- [ ] AI seeks modules when needs critical
- [ ] Needs replenish in correct modules
- [ ] Multiple crew work independently

**Integration:**
- [ ] Complete station can be built
- [ ] Crew can be sustained long-term
- [ ] Resource economy is balanced
- [ ] Performance is acceptable (60+ FPS)

## Verification

1. **Build Initial Station**: Place power generator, life support, quarters, corridors
2. **Spawn Crew**: Add 3-5 crew members
3. **Observe AI Loop**: Watch crew needs deplete and crew automatically seek modules
4. **Test Building**: Add more modules, verify connectivity and systems update
5. **Test Failure**: Remove power generator, verify systems fail gracefully
6. **Check Performance**: Profile with Unreal Insights, verify no severe bottlenecks

## Phase C: Enhanced Input System - Implementation Plan

### Context
Phase B is complete - the grid building system works (preview cube follows cursor, snaps to grid, green grid lines visible). But there are NO input bindings yet, so the player can't:
- Move the camera (WASD)
- Zoom in/out (Mouse Wheel)
- Place modules (Left Click)
- Rotate preview (R key)
- Cancel build mode (Right Click / Escape)

The C++ code already has all handler functions implemented. We just need to create the Input Action and Input Mapping Context assets in Unreal Editor, then assign them in BP_SpaceStationPlayerController.

### What Already Exists (No C++ Changes Needed)
- All 9 input action UPROPERTY declarations in `SpaceStationPlayerController.h` (lines 38-68)
- All handler functions implemented in `SpaceStationPlayerController.cpp` (lines 23-336)
- `SetupInputComponent()` already binds all actions (lines 23-75)
- `EnhancedInput` module dependency in `TestGame4.Build.cs`
- Reference assets exist in `Content/Variant_Strategy/Input/` to use as pattern

### Assets to Create (All in Unreal Editor)

**Folder:** `Content/Variant_SpaceStation/Input/Actions/`

| # | Asset Name | Type | Value Type | Notes |
|---|-----------|------|-----------|-------|
| 1 | IA_SS_CameraPan | Input Action | Axis2D (Vector2D) | WASD movement |
| 2 | IA_SS_CameraZoom | Input Action | Axis1D (Float) | Mouse wheel scroll |
| 3 | IA_SS_ResetCamera | Input Action | Digital (Bool) | Home key |
| 4 | IA_SS_Select | Input Action | Digital (Bool) | Left mouse click |
| 5 | IA_SS_PlaceModule | Input Action | Digital (Bool) | Left mouse click (same as select) |
| 6 | IA_SS_CancelPlacement | Input Action | Digital (Bool) | Right mouse / Escape |
| 7 | IA_SS_RotatePreview | Input Action | Digital (Bool) | R key |
| 8 | IA_SS_CommandCrew | Input Action | Digital (Bool) | Right mouse click |
| 9 | IA_SS_OpenBuildMenu | Input Action | Digital (Bool) | B key |

**Folder:** `Content/Variant_SpaceStation/Input/`

| # | Asset Name | Type |
|---|-----------|------|
| 1 | IMC_SpaceStation | Input Mapping Context |

### Input Mapping Context Bindings (IMC_SpaceStation)

| Action | Key Binding | Modifiers | Notes |
|--------|------------|-----------|-------|
| IA_SS_CameraPan | W/A/S/D | Swizzle Input Axis (for WASD→Vector2D) | W=+Y, S=-Y, A=-X, D=+X |
| IA_SS_CameraZoom | Mouse Wheel Axis | None | Scroll up = zoom in, down = zoom out |
| IA_SS_ResetCamera | Home | None | Reset camera position and zoom |
| IA_SS_Select | Left Mouse Button | None | Select in normal mode |
| IA_SS_PlaceModule | Left Mouse Button | None | Place in build mode |
| IA_SS_CancelPlacement | Right Mouse Button | None | Cancel build mode |
| IA_SS_CancelPlacement | Escape | None | Also cancel with Escape |
| IA_SS_RotatePreview | R | None | Rotate preview 90° |
| IA_SS_CommandCrew | Right Mouse Button | None | Command crew in normal mode |
| IA_SS_OpenBuildMenu | B | None | Toggle build menu |

### Blueprint Assignment (BP_SpaceStationPlayerController)

Open BP_SpaceStationPlayerController → Class Defaults → Input section:
- Space Station Mapping Context → IMC_SpaceStation
- Camera Pan Action → IA_SS_CameraPan
- Camera Zoom Action → IA_SS_CameraZoom
- Reset Camera Action → IA_SS_ResetCamera
- Select Action → IA_SS_Select
- Place Module Action → IA_SS_PlaceModule
- Cancel Placement Action → IA_SS_CancelPlacement
- Rotate Preview Action → IA_SS_RotatePreview
- Command Crew Action → IA_SS_CommandCrew
- Open Build Menu Action → IA_SS_OpenBuildMenu

### Step-by-Step Instructions

**Step 1: Create Input Actions**
For each of the 9 actions above:
1. Right-click in Content Browser → Input → Input Action
2. Name it (e.g., IA_SS_CameraPan)
3. Open it and set Value Type:
   - CameraPan: Axis2D
   - CameraZoom: Axis1D
   - All others: Digital (Bool)
4. Save

**Step 2: Create Input Mapping Context**
1. Right-click in Content Browser → Input → Input Mapping Context
2. Name it IMC_SpaceStation
3. Open it and add mappings (see table above)
4. For CameraPan (WASD), add 4 separate key bindings:
   - W → IA_SS_CameraPan with Swizzle modifier (YXZ) and Negate modifier on none
   - S → IA_SS_CameraPan with Swizzle modifier (YXZ) and Negate modifier
   - A → IA_SS_CameraPan with Negate modifier
   - D → IA_SS_CameraPan (no modifiers)
5. Save

**Step 3: Assign in Blueprint**
1. Open BP_SpaceStationPlayerController
2. Click Class Defaults
3. Assign all 10 properties (1 mapping context + 9 actions)
4. Compile and Save

**Step 4: Test**
1. Press Play
2. Verify: WASD moves camera, Mouse Wheel zooms, R rotates preview, Left Click places module, Right Click cancels

### Verification
- [ ] WASD pans camera in all 4 directions
- [ ] Mouse wheel zooms in/out (1500-4000 range)
- [ ] Home key resets camera to origin and default zoom
- [ ] Left click places module on grid (in build mode)
- [ ] R rotates preview 90° each press
- [ ] Right click / Escape exits build mode
- [ ] Preview cube disappears after exiting build mode
- [ ] Can re-enter build mode (will need build menu later)

### Critical Files
- `SpaceStationPlayerController.h` (lines 38-68) - UPROPERTY declarations
- `SpaceStationPlayerController.cpp` (lines 23-75) - SetupInputComponent bindings
- `SpaceStationPawn.h/.cpp` - Movement and zoom functions
- `BP_SpaceStationPlayerController` - Blueprint to assign assets

## Future Enhancements (Post-Prototype)

- Resource gathering/mining (asteroids)
- Research/tech tree (unlock modules)
- Station events (failures, meteor strikes)
- Mission objectives (survival goals)
- Crew roles/skills (engineer, scientist)
- Module damage/repair
- Multiplayer co-op
- Save/load system
