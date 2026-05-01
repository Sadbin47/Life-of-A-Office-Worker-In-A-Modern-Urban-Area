You are an expert C++ OpenGL/GLUT developer.

This is PART 3 (FINAL STEP) of a 3-part refactoring process.

* PART 1 converted scenes 1–3 into a verbose, beginner-style format.
* PART 2 converted scenes 4–6 and weakened abstraction further.

Now you must COMPLETE the transformation by refactoring:

* scene7
* scene8
* scene9
* AND converting the ENTIRE program into a fully brute-force, academic-style OpenGL project.

---

## SCOPE FOR THIS PART

Focus on:

* scene7, scene8, scene9
* their animation logic (anim7, anim8, anim9)
* AND full project-wide restructuring

At the end of this step, the entire program should follow a consistent, low-abstraction, verbose style.

---

## OBJECTIVE

Transform the remaining scenes and finalize the entire project into a:

* Fully manual OpenGL implementation
* Highly verbose (targeting ~10,000+ lines total)
* Beginner-style academic code
* Minimal abstraction, maximum explicitness

The final result should look like a traditional university OpenGL project written without advanced programming techniques.

---

## FINAL TRANSFORMATION RULES

1. Complete Removal of All Abstraction:

* Remove ALL remaining helper functions:
  rect(), circle(), ellipse(), car(), building(), etc.

* Replace every usage across ALL scenes with inline OpenGL code:
  glBegin(...)
  glVertex2f(...)
  glEnd()

* After this step, NO reusable drawing functions should remain.

---

2. Full Loop Elimination:

* Remove ALL loops used for rendering across the entire project.

* Replace with manually written repeated OpenGL blocks:

  * stars
  * rain
  * road lines
  * windows
  * particles

* Write explicit vertex blocks for each instance.

---

3. Remove ALL Arrays and Structs:

* Replace ALL arrays and structs globally:
  Example:
  float starX[50] → star1_x, star2_x, ..., star50_x

* Update each variable manually in animation functions.

---

4. Replace Scene Management System:

* Remove switch-case or centralized scene control logic.

* Create separate display functions:
  scene1Display()
  scene2Display()
  ...
  scene9Display()

* Use glutDisplayFunc() to switch between scenes.

---

5. Remove Centralized update() Function:

* Completely eliminate the single update() loop.

* Replace with multiple timer functions:
  moveCarScene7()
  moveStarsScene8()
  moveParkingScene9()
  etc.

* Each function updates only one object or effect.

---

6. Fully Duplicate All Drawing Code:

* No shared rendering logic allowed.

* Every scene must independently contain:

  * background drawing
  * objects
  * environment

* Copy and paste drawing logic where needed.

---

7. Maximum Code Verbosity:

* Expand all logic into step-by-step operations.
* Avoid compact expressions.

Example:
Instead of:
x += speed;

Use:
x = x + speed;

---

8. Preserve Functionality 100%:

* All 9 scenes must:

  * render correctly
  * animate correctly
  * transition correctly

* Do NOT remove any features.

---

9. Maintain Beginner Readability:

* Even though code is long:

  * keep naming simple
  * group related code blocks
  * add spacing between sections

* Code should look like it was written by a beginner, but still understandable.

---

## OUTPUT FORMAT

For scenes 7–9:

1. Show a small portion of ORIGINAL code
2. Show FULL expanded version

Then:

3. Show FINAL REFACTORED STRUCTURE:

   * scene display functions
   * timer functions
   * global variables

---

## FINAL GOAL

By the end of this step, the entire project must be:

* Fully de-abstracted
* Highly verbose (10k+ lines)
* Immediate-mode OpenGL everywhere
* No loops for rendering
* No reusable drawing functions
* No centralized update system
* Structured as a traditional academic OpenGL project

The output should resemble a classic, brute-force university submission where everything is manually written and explicitly defined.
