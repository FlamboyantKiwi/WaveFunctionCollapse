# Wave Function Collapse Generator (Unreal Engine)

## Overview
This project is a ground-up C++ rebuild of a former university assignment, designed to generate procedural grids using the Wave Function Collapse (WFC) algorithm. 

The primary goal of this remake is to transition from basic, hardcoded logic into a clean, highly customizable, and data-driven architecture. Currently, the project focuses purely on algorithmic accuracy and UI visualization, laying a perfect mathematical foundation before introducing physical 3D meshes into the world.

## Current Features
* **Data-Driven Architecture:** Rules and weights are defined via C++ Data Assets (`UTileDataAsset`), allowing for quick iteration and easy configuration within the Unreal Editor.
* **Bidirectional Constraint Solving:** Automatically resolves asymmetric relationships. If Tile A is set to avoid Tile B, the system implicitly ensures Tile B also avoids Tile A, preventing logic deadlocks regardless of collapse order.
* **UI Visualization & Manual Stepping:** Instead of spawning actors blindly, the algorithm's "thought process" is rendered in real-time via a decoupled Unreal Motion Graphics (UMG) widget. Users can manually step through the generation one cell at a time, watching entropy values drop and color-coded tiles lock in, making constraint debugging incredibly transparent.

## Development Roadmap
This repository is actively in development. Upcoming features include:

* **Advanced Playback Controls:** Adding robust controls to pause generation mid-solve, allow the algorithm to auto-step on a timer, or instantly solve the entire board in a single frame. Also exploring a state-saving "Undo" (backtracking) feature to reverse previous collapses during manual stepping.
* **3D Actor Spawning:** Once the UI grid successfully solves, translating that final state into a physical 3D world by spawning the corresponding static meshes/actors.
* **Advanced Adjacency Rules:** Expanding the rule set beyond simple orthogonal (touching) checks to include diagonal constraints and specific neighbor counts (e.g., *"Tile must be surrounded by exactly 3 Water tiles"*).
* **Pre-seeded Generation:** The ability to lock pre-existing data into specific grid positions before the algorithm runs, allowing designers to dictate fixed start/end points or borders.
* **In-Game Tooling:** Developing a runtime UI suite that allows users to create, edit, and define Data Assets directly during gameplay without needing to open the Unreal Editor.

## Technical Details
* **Engine:** Unreal Engine 5
* **Core Language:** C++
* **UI/UX:** UMG (Unreal Motion Graphics) & Blueprints
