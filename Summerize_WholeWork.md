# Project Overview: Life of an Office Worker (Modern Urban Area)

## Project Goal
Refactor an existing, modular, single-file OpenGL/GLUT C++ project (~1700 lines) into a highly verbose, beginner-style academic project (~10,000+ lines).

## Core Requirements (Part 1 of 3)
1. **Target:** Scenes 1–3 only.
2. **Method:** 
    - De-abstraction: Replace helper function calls (e.g., `rect`, `circle`, `car`) with inline `glBegin`/`glVertex2f` blocks.
    - Loop Expansion: Replace rendering loops with manual, repeated vertex blocks.
    - Verbosity: Break complex logic into smaller, explicit steps.
3. **Constraints:**
    - Scenes 4–9 must remain modular and untouched.
    - Global update systems and overall project functionality must be preserved.
    - Visual output must be identical.

---

## Progress Report (Phase 1)

### Completed Tasks
- **Scene 1:** Fully refactored to verbose, manual OpenGL style. Includes inlined drawing for house, garage, trees, and car.
- **Scene 2:** Fully refactored to verbose, manual OpenGL style.
    - Inlined sky gradient (`gradSky`), sun, and clouds.
    - Inlined city background (`parallaxCity`), including all building types.
    - Expanded road drawing (`road`) and road dashes into manual blocks.
    - Inlined traffic drawing (`car`, `bus`, `deliveryVan`) and expanded their rendering logic.
    - Replaced all rendering loops with manual, repeated vertex/primitive blocks.
- **Animation Logic:** Scene 1–3 animation functions were already modularized and have been preserved/adapted to support the new verbose drawing structure.

### Current Code State
- `main.cpp`: Scene 1 and Scene 2 are now expanded/verbose. 
- `main.cpp`: Scene 3 remains in its original, modular state (using helper functions).
- `main.cpp`: Scenes 4–9 remain original and untouched, utilizing shared helper functions (`rect`, `ellipse`, `car`, etc.).

---

## Pending Tasks (Next Steps)
1. **Scene 3 Refactor:**
    - Inline all helper calls (`gradSky`, `sun`, `cloud`, `parallaxCity`, `officeComplex`, `officeRampDown`, `parkingBarrier`, `car`).
    - Expand all loops used for rendering (fence blocks, shrubs) into manual, repeated OpenGL blocks.
    - Maintain visual consistency and functionality.
2. **Final Verification:** Ensure Scene 3 works correctly within the existing scene manager.
