# UE5 Research Brief for Orbital Salvage Test Candidate

Date: 2026-02-14
Project: TestGame4
Purpose: Gather practical UE5 implementation guidance and map it to this game's fast prototype needs.

## 1) Engine Version Reality Check

- Epic documentation currently exposes UE 5.7 docs navigation, and UE 5.6 release notes are available.
- This project file `TestGame4.uproject` uses an `EngineAssociation` GUID, so exact local engine version is not human-readable in the file.
- Action: verify exact installed version in Editor before coding final feature set.

## 2) Core UE5 Systems to Use

## Gameplay Framework
- Source: Gameplay Framework in Unreal Engine.
- Why it matters: clean separation of `GameMode`, `PlayerController`, `Pawn`, and `HUD` fits this repo's variant architecture.
- Project mapping:
  - `AOrbitalGameMode` = sector/economy/missions.
  - `AOrbitalPlayerController` = input orchestration and high-level commands.
  - `AOrbitalShipPawn` = movement/physics and ship state.

## Enhanced Input
- Source: Enhanced Input in Unreal Engine.
- Why it matters: contextual input mapping and action-based design let you support flight mode, dock menu mode, and map mode cleanly.
- Project mapping:
  - Use one base mapping context for flight.
  - Add/remap a station context while docked.
  - Follow existing controller patterns in:
    - `Source/TestGame4/Variant_Strategy/StrategyPlayerController.cpp`
    - `Source/TestGame4/Variant_SpaceStation/Gameplay/SpaceStationPlayerController.cpp`

## Physics Thrusters + Force/Torque
- Source: Physics Components in Unreal Engine, and Blueprint API `Add Force`.
- Why it matters: a physics-authored ship gives the Delta V style motion feel quickly.
- Key notes from docs:
  - Physics Thruster Components apply force along local -X.
  - `AddForce` is designed for per-frame use and supports mass-aware force application.
- Project mapping:
  - Start with `UPrimitiveComponent` simulation + `AddForce`/`AddTorqueInRadians`.
  - Add flight assist damping as optional stabilization.

## StateTree for Lightweight Agent Logic
- Source: StateTree in Unreal Engine.
- Why it matters: clean finite-state logic for hostile drones, patrols, and mission encounter agents.
- Project mapping:
  - Use simple states: Patrol, Investigate, Attack, BreakOff, Disabled.
  - Keep behavior logic data-driven for rapid tuning.

## World Partition and Data Layers
- Source: World Partition in Unreal Engine.
- Why it matters: stream larger space sectors while editing collaboratively.
- Project mapping:
  - For the vertical slice, start with two contained sectors.
  - Use Data Layers to toggle encounter sets/objective variants per mission stage.

## Save/Load
- Source: Saving and Loading Your Game.
- Why it matters: station->sector loop benefits from quick save persistence.
- Project mapping:
  - Implement minimal `USaveGame` object:
    - credits, ship hull, fuel, installed module IDs, mission step.
  - Save on dock; load on game start.

## Asset Manager and Primary Data Assets
- Source: Asset Management in Unreal Engine.
- Why it matters: module stats, loot tables, and mission definitions should not be hardcoded.
- Project mapping:
  - Use Data Assets for:
    - ship module definitions
    - resource item definitions
    - mission definitions
  - Register as primary asset types only if scale grows beyond prototype.

## Procedural Content Generation (PCG)
- Source: PCG Development Guides.
- Why it matters: quick asteroid/wreck layout generation without hand-placing everything.
- Project mapping:
  - Use simple PCG graph for asteroid belts.
  - Spawn mission-critical props outside PCG to guarantee objective reliability.

## UI with Widget Blueprints
- Source: Widget Blueprints in Unreal Engine.
- Why it matters: fastest route for HUD and dock market UI.
- Project mapping:
  - Flight HUD widget: velocity, heading, power, heat, hull, fuel, cargo.
  - Dock widget: market actions and module install buttons.

## Profiling and Optimization
- Source: Introduction to Performance Profiling and Configuration.
- Why it matters: physics + many asteroids can tank frame time quickly.
- Project mapping:
  - Baseline with `stat unit`, `stat fps`, and Unreal Insights capture.
  - Profile before adding expensive VFX.

## 3) Reference Game Signals to Preserve

## Delta V: Rings of Saturn signals
- Hard sci-fi tone and "newtonian physics based mining sim" framing.
- Focus on mining/salvage mechanics, ship handling, and subsystem pressure.

## Starcom: Unknown Space signals
- Top-down action + open-world exploration + mission progression.
- Discovery and encounter pacing layered over core ship gameplay.

## 4) Recommended Technical Defaults for This Prototype

- Tick rate assumptions: 60 FPS target on dev machine.
- Input: mouse + keyboard first.
- Physics:
  - Cap linear speed to keep encounter readability.
  - Expose assist and damping in editable variables.
- Mission system:
  - Hard-coded 3-step mission first, then data-asset migration.
- UI update cadence:
  - Poll heavy values on short timer, not every Tick where avoidable.

## 5) Research Sources (Primary)

- Delta V: Rings of Saturn Steam page:
  - https://store.steampowered.com/app/846030/Delta_V_Rings_of_Saturn/
- Starcom: Unknown Space Steam page:
  - https://store.steampowered.com/app/1750770/Starcom_Unknown_Space/
- UE Gameplay Framework:
  - https://dev.epicgames.com/documentation/en-us/unreal-engine/gameplay-framework-in-unreal-engine
- UE Enhanced Input:
  - https://dev.epicgames.com/documentation/en-us/unreal-engine/enhanced-input-in-unreal-engine
- UE Physics Components:
  - https://dev.epicgames.com/documentation/en-us/unreal-engine/physics-components-in-unreal-engine
- UE Blueprint API Add Force:
  - https://dev.epicgames.com/documentation/en-us/unreal-engine/BlueprintAPI/Physics/AddForce
- UE StateTree:
  - https://dev.epicgames.com/documentation/en-us/unreal-engine/state-tree-in-unreal-engine
- UE World Partition:
  - https://dev.epicgames.com/documentation/en-us/unreal-engine/world-partition-in-unreal-engine
- UE Saving and Loading:
  - https://dev.epicgames.com/documentation/en-us/unreal-engine/saving-and-loading-your-game
- UE Asset Management:
  - https://dev.epicgames.com/documentation/en-us/unreal-engine/asset-management-in-unreal-engine
- UE PCG Development Guides:
  - https://dev.epicgames.com/documentation/en-us/unreal-engine/pcg-development-guides
- UE Widget Blueprints:
  - https://dev.epicgames.com/documentation/en-us/unreal-engine/widget-blueprints-in-umg-for-unreal-engine
- UE Performance Profiling Intro:
  - https://dev.epicgames.com/documentation/en-us/unreal-engine/introduction-to-performance-profiling-and-configuration-in-unreal-engine
- UE 5.6 Release Notes:
  - https://dev.epicgames.com/documentation/en-us/unreal-engine/unreal-engine-5-6-release-notes

