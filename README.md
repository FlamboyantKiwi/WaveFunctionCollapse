# Wave Function Collapse (WFC) Generator for Unreal Engine 5

[![GitHub Repo](https://img.shields.io/badge/GitHub-Repository-blue.svg)](https://github.com/FlamboyantKiwi/WaveFunctionCollapse)

A data-driven, step-by-step Wave Function Collapse (WFC) algorithm built in Unreal Engine 5 C++ and controlled via UMG Blueprints. 

This project is a ground-up C++ rebuild of a former university assignment. The primary goal is to transition from basic, hardcoded logic into a clean, highly customizable, and data-driven architecture. It currently focuses purely on algorithmic accuracy and UI visualization, laying a perfect mathematical foundation before introducing physical 3D meshes into the world.

---

## 💻 Technical Details
* **Engine:** Unreal Engine 5
* **Core Language:** C++
* **UI/UX:** UMG (Unreal Motion Graphics) & Blueprints

---

### 🎥 See it in Action
![WFC Generation Demo](<img width="930" height="712" alt="WFCExample1" src="https://github.com/user-attachments/assets/065f1539-3f0f-48ee-a7da-57da74666406" />)

*Demonstrating dynamic grid resizing via the UI and real-time visualization of the WFC algorithm solving constraints step-by-step.*

---

## 🚀 Architecture & Features

This project recently underwent a massive architectural overhaul to separate the algorithmic "Brain" (C++) from the visual "Controller" (Blueprint UI).

* **UI-Driven Lifecycle & Manual Stepping:** The `BP_WFCGen` level actor has been deprecated. The algorithm is now initialized, scaled, and stepped entirely through the `WBP_WFCScreen` loaded at level start. Users can watch the UI dynamically resize and step through the generation one cell at a time, making constraint debugging incredibly transparent.
* **Data-Driven Rules (`UTileDataAsset`):** Rules and weights are defined via C++ Data Assets, allowing for quick iteration. We recently replaced basic arrays with a highly flexible `FTileRuleWrapper`, supporting Conditional checks (Must have / Must avoid) and Directional rules (Orthogonal, Diagonal, etc.).
* **Bidirectional Constraint Solving:** Automatically resolves asymmetric relationships. If Tile A is set to avoid Tile B, the system implicitly ensures Tile B also avoids Tile A, preventing logic deadlocks regardless of collapse order.
* **Optimized Math Helpers:** Introduced `EWfcDirection`, `EWfcRuleCondition`, and a lightweight `FWfcMath` utility class to handle spatial indexing, completely eliminating out-of-bounds crashes.
* **Enhanced Debugging:** Integrated the VisualStudioTools plugin for superior C++ debugging support.

---

## 🛠️ How It Works

1. **The Brain (C++):** `AWfcGenerator` handles all the heavy lifting. It calculates entropy, propagates neighbor constraints, and detects contradictions using strict const-correctness.
2. **The Rules (Data Assets):** `UTileDataAsset` acts as the recipe card for each tile, holding its UI color, 3D Mesh, spawn weights, and an array of adjacency rules.
3. **The Screen (UMG):** `WBP_WFCScreen` manages the local state. Users define the grid size (X/Y) and the UI handles the timers to step through the C++ math, perfectly mapping the algorithmic index to the visual grid.

---

## 🗺️ Development Roadmap

This repository is actively in development. Upcoming features include:

* **[NEXT] In-Game Tooling & Runtime Customization:** Developing a runtime UI suite that allows users to create, edit, and define WFC Data Assets directly during gameplay without needing to open the Unreal Editor.
* **Advanced Playback & Backtracking:** Adding robust controls to pause generation mid-solve, alongside a state-saving "Undo" (backtracking) feature to reverse previous collapses if the algorithm hits a contradiction.
* **Pre-seeded Generation:** The ability to lock pre-existing data into specific grid positions before the algorithm runs, allowing designers to dictate fixed start/end points or borders.
* **3D Actor Spawning:** Once the 2D UI grid successfully solves without deadlocks, translating that final state into a physical 3D world by spawning the corresponding static meshes/actors.
