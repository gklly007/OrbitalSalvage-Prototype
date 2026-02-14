# Orbital Salvage Game - Fast Test Candidate Plan

Date: 2026-02-14
Project: TestGame4 (UE C++ variants architecture)
Assumption: "UR5" means Unreal Engine 5 (UE5).

## 1) Goal

Build a fast playable test candidate inspired by:
- Delta V: Rings of Saturn (inertial ship handling, mining/salvage pressure, subsystem management).
- Starcom: Unknown Space (exploration sectors, light story missions, ship upgrade progression).

The test candidate should prove the core loop in 10-15 minutes of play.

## 2) Non-Negotiable Pillars

1. Inertial top-down ship handling with meaningful momentum.
2. Resource pressure (fuel, power, heat, hull, cargo space).
3. Salvage/mining to profit loop.
4. Sector travel with at least one narrative encounter.
5. Tangible ship upgrade choice that changes moment-to-moment play.

## 3) Scope for Fast Vertical Slice

## In Scope
- 1 player ship with thruster-based movement and rotation.
- 2 sectors:
  - Sector A: asteroid belt + wrecks + station.
  - Sector B: hostile zone + mission objective.
- 3 interactive resource types: ore, salvage parts, fuel.
- 1 station screen for sell/buy/repair/refit.
- 3 modules to equip: Reactor, Cargo Pod, Mining Laser.
- 2 enemy/obstacle archetypes: drone and turret wreck field.
- 1 short mission chain (3 steps): "Recover black box -> jump -> extract under pressure."
- Minimal UI: velocity, heading, power, heat, hull, cargo.

## Out of Scope (for this test candidate)
- Full campaign, factions, procedural galaxy, crew management depth, multiplayer.

## 4) 10-Minute Core Loop

1. Launch from station with starter loadout.
2. Fly using inertial controls to an asteroid cluster.
3. Mine/salvage while balancing heat and power.
4. Avoid or disengage from one hostile encounter.
5. Return to station, sell cargo, buy one upgrade.
6. Jump to second sector and complete one mission objective.

If this is fun without polished art, the concept is validated.

## 5) Fit to Existing Repo Architecture

Your project already uses variant separation in `Source/TestGame4`:
- `Variant_TwinStick`
- `Variant_Strategy`
- `Variant_SpaceStation`

Recommended new variant:
- `Source/TestGame4/Variant_OrbitalSalvage`

Reuse from current code:
- Enhanced Input setup patterns from `Variant_Strategy` and `Variant_SpaceStation`.
- Selection/command and HUD patterns from strategy/space-station controllers.
- Resource/system simulation approach from `Variant_SpaceStation/Components`.

## 6) Proposed C++ Class Set (Minimum)

Gameplay:
- `AOrbitalShipPawn` (physics-driven ship movement, thrusters, damping model).
- `AOrbitalPlayerController` (flight input, target selection, interact, dock).
- `AOrbitalGameMode` (economy state, mission state, sector transitions).
- `AOrbitalSectorManager` (spawns asteroids/wrecks/enemies for active sector).

Systems:
- `UShipSystemsComponent` (power budget, heat, fuel, hull, cargo mass effect).
- `USalvageComponent` (mining/salvage extraction rates and timers).
- `UMissionComponent` (objective progression and rewards).

World actors:
- `AAsteroidResourceNode` (ore amount, hardness, damage response).
- `AWreckSalvageNode` (loot table + danger event chance).
- `AJumpGateActor` (sector transfer trigger).
- `AStationDockActor` (dock/undock + market UI trigger).

Data:
- `UDataAsset` classes for ship modules, resources, and mission definitions.

UI:
- `AOrbitalHUD` and `UUserWidget` pages for flight HUD and station market.

## 7) Build Sequence (Fast Path)

## Phase 0 - Setup (0.5 day)
- Create variant folders/classes.
- Register default map and GameMode for new variant test map.
- Create input mapping context and actions.

Done when:
- Player ship spawns and can move in empty test map.

## Phase 1 - Flight Model + Camera (1 day)
- Implement thrust forward/back, strafe, yaw torque.
- Add capped velocity and optional assist damping toggle.
- Top-down camera with zoom and edge-safe bounds.

Done when:
- Ship momentum feels controllable and readable.

## Phase 2 - Resource Nodes + Extraction (1 day)
- Spawn asteroid and wreck nodes.
- Mining laser with line trace and heat generation.
- Cargo fill and mass effect on ship acceleration.

Done when:
- Player can collect and store resources, with overheating failure pressure.

## Phase 3 - Station Economy + Refit (1 day)
- Dock interaction.
- Sell ore/salvage, buy fuel/repair, equip one module slot.
- Module effects update runtime systems component.

Done when:
- One complete profit cycle works end-to-end.

## Phase 4 - Sector Transition + Mission (1 day)
- Jump gate to second sector.
- Mission tracker with 3-state objective chain.
- Basic hostile encounter during objective completion.

Done when:
- Player completes full loop across two sectors.

## Phase 5 - UX and Tuning (1 day)
- HUD read clarity, warning states, mission prompts.
- Balance extraction, travel, and combat pressure.
- Fix obvious pacing and dead-time issues.

Done when:
- Fresh tester can complete the loop without explanation.

## 8) Acceptance Checklist

- Ship inertia is clearly felt and manageable.
- Mining/salvage and cargo pressure are meaningful.
- Player can dock, sell, repair, and upgrade.
- Sector jump works and mission progresses reliably.
- At least one build choice changes strategy (example: bigger cargo vs cooler reactor).
- No critical blockers in a 15-minute play session.

## 9) Risks and Mitigations

1. Physics feels too floaty/unreadable.
   - Mitigation: expose damping/assist curves in data assets and tune with live values.
2. Scope creep from story systems.
   - Mitigation: lock mission chain to 3 states for this test.
3. UI overload.
   - Mitigation: keep only 6 core metrics on HUD (vel, power, heat, hull, fuel, cargo).
4. Performance drops in asteroid field.
   - Mitigation: start with simple instancing and profile before adding effects.

## 10) Immediate Task Backlog (First 48 Hours)

1. Create `Variant_OrbitalSalvage` class skeletons and compile.
2. Implement `AOrbitalShipPawn` with AddForce/AddTorque driven input.
3. Add one asteroid resource actor and one mining interaction.
4. Add temporary dock actor with sell-all action.
5. Wire a simple two-sector transition via jump gate trigger.

