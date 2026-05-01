## Progress Summary

- Refactored `scene1` in `main.cpp` to verbose style.
- Refactored `scene2` in `main.cpp` to verbose style, inlining sky gradient, sun, clouds, fence posts, road dashes, shrubs, and traffic car drawing logic.
- Ensured helper functions remain untouched and are still used by scenes 4-9.

## Current Code State

- `main.cpp`: `scene1` and `scene2` now use verbose, inlined OpenGL drawing primitives.
- `main.cpp`: `scene3` still uses helper functions and loops for drawing.
- No changes made outside `main.cpp`.

## Pending Next Steps (Continue in `main.cpp` Only)

1.  **Scene3 refactor (priority):**
    - Inline sky gradient, sun, cloud, parallax city: replace `gradSky`, `sun`, `cloud`, `parallaxCity` calls in scene3 with explicit `glBegin`/`glVertex2f` blocks.
    - Inline `officeComplex` and `officeRampDown` drawing blocks directly in scene3.
    - Replace `road()` call with explicit road drawing for scene3.
    - Expand loops in scene3 (fence blocks, shrubs) into manual repeated blocks.
    - Inline `parkingBarrier` and `car` draws in scene3.

2.  **Keep constraints:**
    - Do not change scenes 4-9 or shared helpers; only expand scene3 internals.
    - Preserve visuals and animation behavior.


    You are an expert C++ OpenGL/GLUT developer.

This is PART 2 of a 3-part refactoring process. In PART 1, scenes 1–3 were already converted into a verbose, beginner-style format with reduced abstraction.

---

## SCOPE FOR THIS PART ONLY

Focus ONLY on:

* scene4
* scene5
* scene6
* their related animation logic (anim4, anim5, anim6 or equivalent)

You may now also begin partial restructuring of shared systems if needed, but DO NOT fully rewrite the entire program yet.

---

## OBJECTIVE

Transform scenes 4–6 into a verbose, manual, beginner-style academic format while maintaining consistency with the already refactored scenes 1–3.

The result should:

* Increase line count significantly
* Reduce abstraction further
* Look like manually written OpenGL code
* Stay fully functional and visually identical

---

## TRANSFORMATION RULES (SCENES 4–6)

1. Full Inline Rendering (No Helpers Here):

* Remove ALL helper function usage inside scenes 4–6:
  rect(), circle(), ellipse(), car(), building(), character(), etc.

* Replace them with full inline OpenGL code:
  glBegin(...)
  glVertex2f(...)
  glEnd()

* Unlike Part 1, DO NOT rely on any remaining helper functions here.

---

2. Aggressive Loop Expansion:

* Replace ALL loops used for rendering in scenes 4–6:
  (chairs, tables, people, windows, lights, particles, etc.)
* Manually write repeated OpenGL drawing blocks.
* Keep repetition structured (group similar shapes together).

---

3. Remove Arrays and Structs (Scenes 4–6):

* Replace arrays and structs used in these scenes:
  Example:
  SteamParticle → steam1_x, steam1_y, steam2_x...

* Update each variable manually in animation functions.

---

4. Expand Animation System (Deeper Than Part 1):

* Break anim4, anim5, anim6 into smaller functions:
  moveCharacter1()
  moveSteam1()
  animateMeetingPointer()
  etc.

* Avoid central logic inside a single animation function.

---

5. Start Weakening Centralized Systems:

* Begin reducing reliance on the global update() structure:

  * Move scene-specific logic closer to each scene
* DO NOT completely remove update() yet (final removal in Part 3)

---

6. Duplicate All Scene Content:

* Inline EVERYTHING:

  * backgrounds
  * objects
  * props

* Do NOT reuse any shared drawing functions.

---

7. Increase Verbosity Further:

* Break all calculations into multiple steps
* Avoid compact expressions
* Write movement logic step-by-step

Example:
Instead of:
x += speed;

Use:
x = x + speed;

---

8. Maintain Visual Accuracy:

* Scenes must look identical to original:

  * office desks
  * cafeteria setup
  * meeting room visuals
* Do NOT remove any elements

---

9. Maintain Beginner Readability:

* Use simple variable names
* Group code logically (e.g., "draw chair", "draw table")
* Add spacing between blocks for clarity

---

## OUTPUT FORMAT

For each scene (4 → 6):

1. Show a small portion of the ORIGINAL modular code
2. Show the FULL EXPANDED version
3. Clearly demonstrate:

   * helper functions → inline OpenGL blocks
   * loops → repeated manual drawing
   * arrays/structs → individual variables

Work scene-by-scene.

---

## IMPORTANT NOTES

* Scenes 1–3 are already refactored → match their style
* Scenes 7–9 remain untouched for now
* Do NOT fully remove scene switching system yet
* Do NOT attempt full project rewrite in this step

---

## GOAL FOR PART 2

Convert scenes 4–6 into a heavily expanded, low-abstraction, academic-style format, while preparing the codebase for final full transformation in Part 3.

This step should make the project look significantly larger and more manual, but still controlled and readable.