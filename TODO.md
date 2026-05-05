You are an expert C++ / OpenGL / GLUT project-merging AI agent.



IMPORTANT FILE LOCATION NOTE:

Both files are named `main.cpp`, but they are located in different folders.



At the start of this task, I will provide the exact folder paths for both files.



Use the paths I provide as the source of truth.



Do NOT confuse the two `main.cpp` files.



============================================================

FILE PATHS — I WILL PROVIDE THESE

============================================================



SOURCE FILE:

This is the file containing the upgraded Scene 1 and Scene 9 code.



SOURCE_MAIN_CPP_PATH = "/home/s4d/Documents/GraphicProjectCompleted/ComputerGraphicsProject_TheHomeModule/main.cpp"



MASTER FILE:

This is the main project file that must receive the merge.



MASTER_MAIN_CPP_PATH = "/home/s4d/Documents/GraphicProjectCompleted/Life-of-A-Office-Worker-In-A-Modern-Urban-Area/main.cpp"



============================================================

TASK

============================================================



Carefully merge ONLY the Scene 1 and Scene 9 upgrades from:



SOURCE_MAIN_CPP_PATH



into:



MASTER_MAIN_CPP_PATH



Your final output must be the fully merged code for:



MASTER_MAIN_CPP_PATH



============================================================

CRITICAL RULES — FOLLOW EXACTLY

============================================================



1. Only work with Scene 1 and Scene 9.

2. Do NOT touch, rewrite, remove, simplify, rename, or reorganize any Scene 2, 3, 4, 5, 6, 7, or 8 code.

3. Do NOT remove any existing code from the master `main.cpp`.

4. Do NOT overwrite the whole project blindly.

5. Do NOT replace the master `main.cpp` with the source `main.cpp`.

6. Do NOT delete any existing global variables, helper functions, shapes, traffic logic, office logic, highway logic, rain logic, keyboard controls, scene switching, or animation logic.

7. Any helper function used by Scenes 2–8 must remain backward compatible.

8. If a helper function signature changes, use default arguments so old calls still compile.

9. The project must still support Scene 1 through Scene 9 after merging.

10. Before final output, verify that Scene 2–8 logic remains untouched.



============================================================

MAIN GOAL

============================================================



Use the master `main.cpp` as the base file.



Use the source `main.cpp` only as the source for Scene 1 and Scene 9 upgrades.



Merge the improved visuals, animations, environment effects, and Scene 1 / Scene 9 behavior from the source file into the master file.



Do not copy unrelated missing code from the source file if it would overwrite, remove, or damage code already present in the master file.



============================================================

STEP 1 — ADD ONLY REQUIRED GLOBAL VARIABLES

============================================================



In the master `main.cpp`, add any missing global variables from the source `main.cpp` that are required for Scene 1 and Scene 9 upgrades.



Add these only if missing:

float grassSwayTimer = 0.0f;



float cloudOffsetX_layerC = 0.0f;

float cloudDriftSpeedA = 0.0f;

float cloudDriftSpeedB = 0.0f;

float cloudDriftSpeedC = 0.0f;



float sunGlowPulse = 0.0f;

float sunGlowDirection = 1.0f;



float scene1_carAngle = 0.0f;

float scene9_carAngle = 0.0f;



float windowFlickerTimer = 0.0f;

float windowFlickerAmount[10];





Do not remove or rename any existing variables.



Keep all existing Scene 2–8 global variables exactly as they are.



============================================================

STEP 2 — MERGE SAFE ENVIRONMENT HELPERS



Add these new functions from the source main.cpp if they are missing in the master main.cpp:



void drawFireflies();

void drawGrassField(bool night);



Use the full function bodies from the source main.cpp.



Replace these existing helper functions in the master main.cpp with the upgraded versions from the source main.cpp:



void sun(float cx, float cy);

void moon(float cx, float cy);

void stars();

void tree(float bx, float by);



Replace cloud() with the upgraded version from the source main.cpp, but make the function backward compatible by using a default argument:



void cloud(float cx, float cy, float alpha = 1.0f)



This is mandatory because Scenes 2–8 may still call:



cloud(x, y);



Those calls must continue to compile without editing Scenes 2–8.



Do NOT update cloud calls in Scenes 2–8.



============================================================

STEP 3 — HANDLE ROAD FUNCTION SAFELY



Do NOT risk breaking highway or road behavior in Scenes 2, 3, 7, or 8.



Check how road() is used in the master main.cpp.



If the new road() from the source main.cpp behaves exactly the same and does not affect Scenes 2–8, replacing it is acceptable.



However, if there is any risk that replacing road() changes Scenes 2–8, then use this safer method:



Keep the original road() function in the master main.cpp unchanged.

Add the Scene 1 / Scene 9 road version from the source main.cpp as a new function:

void homeRoad()

Update only scene1() and scene9() to call:

homeRoad();



Do not update road calls outside Scene 1 and Scene 9.



Do not edit any road-related logic in Scenes 2–8.



============================================================

STEP 4 — REPLACE ONLY SCENE 1 AND SCENE 9 FUNCTIONS



In the master main.cpp, replace only these two scene-rendering functions with the versions from the source main.cpp:



void scene1()

void scene9()



Do not modify or replace:



void scene2()

void scene3()

void scene4()

void scene5()

void scene6()

void scene7()

void scene8()



Scene 2–8 functions must remain exactly as they already are in the master main.cpp.



============================================================

STEP 5 — REPLACE ONLY SCENE 1 AND SCENE 9 ANIMATION FUNCTIONS



In the master main.cpp, replace only these animation functions with the versions from the source main.cpp:



void anim1()

void anim9()



Do not modify or replace:



void anim2()

void anim3()

void anim4()

void anim5()

void anim6()

void anim7()

void anim8()



Scene 2–8 animation functions must remain exactly as they already are in the master main.cpp.



============================================================

STEP 6 — CAREFULLY INJECT INTO init()



Do NOT overwrite the whole init() function.



Inside the existing init() function in the master main.cpp, add only the missing initialization logic needed by the new Scene 1 / Scene 9 effects.



Add initialization for:



cloudDriftSpeedA

cloudDriftSpeedB

cloudDriftSpeedC

windowFlickerAmount[10]



Use the same initialization logic from the source main.cpp.



Keep every existing line in init() intact.



Do not remove or modify existing initialization code for rain, traffic, office, highway, or any other scene.



============================================================

STEP 7 — CAREFULLY INJECT INTO resetScene(int idx)



Do NOT overwrite the whole resetScene(int idx) function.



Only update the blocks for:



if (idx == 1)

if (idx == 9)



Add the new reset logic from the source main.cpp, including required Scene 1 and Scene 9 variables such as:



scene1_carAngle

scene9_carAngle

sunGlowPulse

sunGlowDirection

grassSwayTimer

cloudOffsetX_layerA

cloudOffsetX_layerB

cloudOffsetX_layerC

windowFlickerTimer



Leave all other reset blocks untouched, especially:



if (idx == 2)

if (idx == 3)

if (idx == 4)

if (idx == 5)

if (idx == 6)

if (idx == 7)

if (idx == 8)



Do not remove any existing reset logic.



Do not change reset behavior for Scenes 2–8.



============================================================

STEP 8 — CAREFULLY INJECT INTO update(int val)



Do NOT overwrite the whole update(int val) function.



Inside the existing:



if (!isPaused)



block, inject only the new timer and environmental animation updates required by Scene 1 and Scene 9.



Add or update logic for:



sunGlowPulse

sunGlowDirection

grassSwayTimer

cloudOffsetX_layerA

cloudOffsetX_layerB

cloudOffsetX_layerC

windowFlickerTimer

windowFlickerAmount



Use the same logic from the source main.cpp.



Do not remove or modify existing update logic for:



rain

scene2

scene3

scene4

scene5

scene6

scene7

scene8

scene switching

glutPostRedisplay()

glutTimerFunc()



Do not overwrite the whole update loop.



Only inject the required Scene 1 / Scene 9 animation support.



============================================================

STEP 9 — COMPILE SAFETY CHECK



After merging, check for compile errors related to:



cloud()

scene1_carAngle

scene9_carAngle

cloudOffsetX_layerC

cloudDriftSpeedA

cloudDriftSpeedB

cloudDriftSpeedC

windowFlickerAmount

drawGrassField()

drawFireflies()

sunGlowPulse

sunGlowDirection

grassSwayTimer



Fix only issues directly related to the Scene 1 and Scene 9 merge.



Do not refactor unrelated code.



Do not rename unrelated functions.



Do not remove unrelated code to fix compile errors.



============================================================

STEP 10 — FINAL VALIDATION



Before giving the final code, verify all of the following:



The master main.cpp still contains all scenes from 1 to 9.

Scene 1 uses upgraded morning visuals and animation from the source main.cpp.

Scene 9 uses upgraded night visuals, lighting, stars, fireflies, and parking animation from the source main.cpp.

Scenes 2–8 are still present.

Scenes 2–8 were not touched.

No unrelated code was deleted.

No existing global variables were removed.

No existing helper functions unrelated to Scene 1 / Scene 9 were removed.

cloud(float cx, float cy) style calls still compile because alpha has a default value.

init(), resetScene(int idx), and update(int val) were edited only by injection, not full replacement.

The final output is a complete, compilable master main.cpp.

============================================================

FINAL OUTPUT FORMAT



Return only the completed merged C++ code.



Do not explain.

Do not summarize.

Do not add markdown.

Do not add text before or after the code.

Do not mention what you changed.



Output must be the full final contents of the master main.cpp