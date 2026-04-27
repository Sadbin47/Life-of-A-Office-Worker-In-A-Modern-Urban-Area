/*
 * Life-of-A-Office-Worker-In-A-Modern-Urban-Area
 *
 * 8th semester university project
 * - Single file (main.cpp)
 * - Pure 2D (gluOrtho2D)
 * - GLUT fixed pipeline (no shaders, no textures, no 3D)
 */

#ifdef __APPLE__
#include <GLUT/glut.h>
#else
#include <GL/glut.h>
#endif

#include <cmath>
#include <cstdio>
#include <cstdlib>

// ==========================================================
// ====== GLOBAL WINDOW, TIMING, AND SCENE CONSTANTS ========
// ==========================================================

const int WINDOW_WIDTH  = 1280; // Required project width
const int WINDOW_HEIGHT = 720;  // Required project height

const float ORTHO_LEFT_X   = 0.0f;             // Left border of 2D world
const float ORTHO_RIGHT_X  = 1280.0f;          // Right border of 2D world
const float ORTHO_BOTTOM_Y = 0.0f;             // Bottom border of 2D world
const float ORTHO_TOP_Y    = 720.0f;           // Top border of 2D world
const float TARGET_ASPECT  = 16.0f / 9.0f;     // Required aspect ratio

const int TIMER_INTERVAL_MS = 16;              // ~60 FPS timer interval
const int FIRST_SCENE_INDEX = 1;               // Scene index starts from 1
const int LAST_SCENE_INDEX  = 9;               // Total 9 scenes

const float PI_VALUE = 3.14159265358979323846f; // Constant PI for circle math

// Each scene lasts approximately 5-8 seconds at ~60 FPS.
const int sceneDurationFrameCount[10] = {
    0,   // Index 0 is unused
    420, // Scene 1  ~7.0 sec
    390, // Scene 2  ~6.5 sec
    360, // Scene 3  ~6.0 sec
    420, // Scene 4  ~7.0 sec
    390, // Scene 5  ~6.5 sec
    360, // Scene 6  ~6.0 sec
    360, // Scene 7  ~6.0 sec
    420, // Scene 8  ~7.0 sec
    430  // Scene 9  ~7.1 sec
};

const int FADE_START_BEFORE_END_FRAMES = 55; // Start fade before scene end
const float FADE_ALPHA_STEP = 0.03f;         // Alpha increase/decrease per frame

// ==========================================================
// ====== GLOBAL ENGINE STATE (SCENE MACHINE + FADE) ========
// ==========================================================

int currentScene = 1;         // Scene state machine variable (1 to 9)
int sceneFrameCounter = 0;    // Counts frames spent in current scene
int totalFrameCounter = 0;    // Counts total frames across all scenes

bool isSceneTransitionActive = false; // True while fade transition is running
bool isFadeOutPhase = false;          // True = alpha rising, False = alpha falling
float fadeOverlayAlpha = 0.0f;        // Current fade quad alpha (0.0 to 1.0)

bool showTextOverlay = false; // Panel already shows scene info and frame count

// ==========================================================
// ====== GLOBAL ENVIRONMENT ANIMATION VARIABLES ============
// ==========================================================

float cloudOffsetX_layerA = 0.0f;    // Cloud layer A horizontal movement
float cloudOffsetX_layerB = 0.0f;    // Cloud layer B horizontal movement
float sunHorizontalOffset = 0.0f;    // Small horizontal drift for sun animation
float starTwinkleCounter = 0.0f;     // Shared phase for star twinkling

// Star positions for night scenes (8 and 9).
const int STAR_COUNT = 34;
const float starPositionX[STAR_COUNT] = {
     40.0f,  90.0f, 145.0f, 210.0f, 275.0f, 330.0f, 390.0f, 455.0f, 520.0f,
    585.0f, 650.0f, 715.0f, 770.0f, 830.0f, 885.0f, 950.0f,1010.0f,1070.0f,
   1130.0f,1190.0f,1245.0f,  65.0f, 180.0f, 305.0f, 420.0f, 560.0f, 690.0f,
    815.0f, 940.0f,1065.0f,1185.0f, 250.0f, 745.0f, 980.0f
};
const float starPositionY[STAR_COUNT] = {
    640.0f, 680.0f, 620.0f, 700.0f, 655.0f, 610.0f, 685.0f, 640.0f, 705.0f,
    625.0f, 690.0f, 650.0f, 710.0f, 635.0f, 675.0f, 618.0f, 698.0f, 645.0f,
    708.0f, 630.0f, 695.0f, 605.0f, 665.0f, 618.0f, 700.0f, 646.0f, 710.0f,
    622.0f, 690.0f, 640.0f, 705.0f, 680.0f, 635.0f, 670.0f
};

// ==========================================================
// ====== ROAD / CITY DRAW CONFIG (SET PER SCENE) ===========
// ==========================================================

float roadBottomY_current = 120.0f; // Current road lower edge Y
float roadHeight_current = 180.0f;  // Current road total height
int roadLaneCount_current = 3;      // Current lane count
bool roadNightMode_current = false; // Color mode for road

bool buildingWindowNightMode = false; // If true, windows are warm/yellow lights

// ==========================================================
// ====== HOME / HOUSE SHARED STATE (SCENE 1 and 9) =========
// ==========================================================

float garageDoorOpenRatio_house = 0.0f; // 0 = closed, 1 = fully open
bool houseNightMode = false;            // Day/night color switch

// ==========================================================
// ====== CHARACTER SHARED POSE STATE =======================
// ==========================================================

float characterLeftArmAngleDegrees = 0.0f;  // Rotation for left arm segment
float characterRightArmAngleDegrees = 0.0f; // Rotation for right arm segment
float characterLegSwingAngleDegrees = 0.0f; // Opposite swing for left/right legs

bool characterFacingRight = true;    // True = face right, False = face left
bool characterCarryBriefcase = false; // Draw briefcase near right hand
bool characterSittingPose = false;    // Sit pose used in office desk scene
bool characterPointingPose = false;   // Right arm points toward board

// ==========================================================
// ====== SCENE-SPECIFIC ANIMATION VARIABLES =================
// ==========================================================

// Scene 1: Rainy morning departure (Developer 1 module).
float scene1_carPosX = 392.0f;          // Car starts inside garage (X)
float scene1_carPosY = 318.0f;          // Car starts inside garage (Y)
float wheelRotationAngle = 0.0f;        // Shared wheel angle for Scene 1 and Scene 9

// Smaller increments make animation smoother and easier to observe.
float animationSpeed = 1.0f;

// Pause stops updates but keeps rendering the last frame.
bool isPaused = false;

// This allows interactive control of environment.
bool isRainEnabled = false;  // Start in sunny mode; press R to enable rain

// Scene 1 car state machine:
// 0 = inside garage, 1 = moving forward, 2 = moving right
int carState_scene1 = 0;
bool scene1HasCarExitedScreen = false;  // True only after x > 1280

// Rain uses fixed-size arrays for viva-friendly explanation.
const int RAIN_DROP_COUNT = 130;
float rainDropX[RAIN_DROP_COUNT];
float rainDropY[RAIN_DROP_COUNT];

const float scene1GarageStartX = 392.0f;    // Initial car X inside garage
const float scene1GarageStartY = 318.0f;    // Initial car Y inside garage
const float scene1RoadTravelY = 72.0f;      // Car Y while driving on road
const float scene1ExitCheckX = 1280.0f;     // Scene can change only after x > 1280

// Scene 2: Morning traffic.
float trafficCarPositionX_scene2_A = -180.0f;
float trafficCarPositionX_scene2_B = 1350.0f;
float trafficCarPositionX_scene2_C = -420.0f;

float trafficCarWheelAngle_scene2_A = 0.0f;
float trafficCarWheelAngle_scene2_B = 0.0f;
float trafficCarWheelAngle_scene2_C = 0.0f;

float parallaxOffset_scene2 = 0.0f; // Buildings move slower than cars

// Scene 3: Office arrival.
float carPositionX_scene3 = -220.0f;
float carPositionY_scene3 = 286.0f;
float wheelRotationAngle_scene3 = 0.0f;
float barrierRotationAngle_scene3 = 0.0f; // 0 = closed horizontal, ~85 = open

// Scene 4: Main office.
float typingArmAngle_scene4 = 0.0f;
float fanRotationAngle_scene4 = 0.0f;
float wallClockHandAngle_scene4 = 90.0f;
float workerPositionX_scene4_A = 800.0f;
float workerPositionX_scene4_B = 980.0f;
float workerDirection_scene4_A = 1.0f;
float workerDirection_scene4_B = -1.0f;

// Scene 5: Coffee break.
float workerWalkPositionX_scene5 = 250.0f;
float workerWalkLegSwing_scene5 = 0.0f;

struct SteamParticle {
    float particleX;      // Particle center X
    float particleY;      // Particle center Y
    float verticalSpeed;  // Upward speed per frame
    float alphaValue;     // Current transparency
};

const int STEAM_PARTICLE_COUNT = 16;
SteamParticle coffeeSteamParticles[STEAM_PARTICLE_COUNT];

// Scene 6: Presentation.
float presentationBarGrowRatio_scene6 = 0.0f; // 0 -> 1 grows bars gradually
float pointerOscillationAngle_scene6 = 0.0f;  // Small pointer motion

// Scene 7: Leaving office.
float carPositionX_scene7 = 520.0f;
float carPositionY_scene7 = 170.0f;
float wheelRotationAngle_scene7 = 0.0f;
float barrierRotationAngle_scene7 = 0.0f;

// Scene 8: Evening traffic.
float trafficCarPositionX_scene8_A = -260.0f;
float trafficCarPositionX_scene8_B = 1380.0f;
float trafficCarPositionX_scene8_C = -560.0f;

float trafficCarWheelAngle_scene8_A = 0.0f;
float trafficCarWheelAngle_scene8_B = 0.0f;
float trafficCarWheelAngle_scene8_C = 0.0f;

float parallaxOffset_scene8 = 0.0f;

// Scene 9: Night return (Developer 1 module).
float scene9_carPosX = -220.0f;         // Car enters from left side
float scene9_carPosY = 72.0f;           // Car road lane Y at scene start

// Scene 9 car state machine:
// 0 = move right, 1 = move up into garage, 2 = parked
int carState_scene9 = 0;
int scene9ParkedFrameCounter = 0;       // Small hold time before transition
bool scene9ParkingCompleted = false;    // True when car fully stops in garage

bool isHouseLightOn = false;            // Lights turn on when car fully parks

const float scene9RoadTravelY = 72.0f;      // Car road lane Y in Scene 9
const float scene9DrivewayEntryX = 340.0f;  // X where car starts moving up driveway
const float scene9GarageParkingX = 392.0f;  // Final parked X inside garage
const float scene9GarageParkingY = 318.0f;  // Final parked Y inside garage

// Shared home layout constants used by Scene 1 and Scene 9.
const float homeRoadBottomY = 0.0f;             // Bottom of dark road strip
const float homeRoadHeight = 110.0f;            // Road thickness
const float homeGrassBottomY = 110.0f;          // Grass starts immediately above road
const float homeGrassHeight = 190.0f;           // Grass vertical size

const float homeDrivewayRoadLeftX = 250.0f;     // Driveway lower-left near road
const float homeDrivewayRoadRightX = 430.0f;    // Driveway lower-right near road
const float homeDrivewayRoadY = 110.0f;         // Driveway lower edge touches grass-top/road-top
const float homeDrivewayGarageLeftX = 300.0f;   // Driveway upper-left near garage door
const float homeDrivewayGarageRightX = 500.0f;  // Driveway upper-right near garage door
const float homeDrivewayGarageY = 300.0f;       // Driveway upper edge at garage floor

const float homeHouseLeftX = 140.0f;            // Main house body left edge
const float homeHouseBottomY = 300.0f;          // Main house body bottom edge
const float homeHouseWidth = 420.0f;            // Main house body width
const float homeHouseHeight = 250.0f;           // Main house body height

const float homeGarageLeftX = 280.0f;           // Garage box left edge
const float homeGarageBottomY = 300.0f;         // Garage box bottom edge
const float homeGarageWidth = 240.0f;           // Garage width
const float homeGarageHeight = 170.0f;          // Garage height

const float homeGarageDoorLeftX = 312.0f;       // Garage opening left edge
const float homeGarageDoorBottomY = 300.0f;     // Garage opening bottom edge
const float homeGarageDoorWidth = 176.0f;       // Garage opening width
const float homeGarageDoorHeight = 126.0f;      // Garage opening height

// ==========================================================
// ====== FORWARD DECLARATIONS ===============================
// ==========================================================

float clampFloat(float value, float minValue, float maxValue);
float wrapOffsetToRange(float value, float minValue, float maxValue);

void drawRectangle(float x, float y, float width, float height);
void drawFilledCircle(float centerX, float centerY, float radius, int segmentCount);
void drawFilledEllipse(float centerX, float centerY, float radiusX, float radiusY, int segmentCount);
void drawVerticalSkyGradient(float bottomRed, float bottomGreen, float bottomBlue,
                             float topRed, float topGreen, float topBlue);

void drawWheel(float wheelCenterX, float wheelCenterY, float wheelRotationAngle);
void drawCar(float carPositionX, float carPositionY, float bodyRed, float bodyGreen, float bodyBlue,
             float wheelRotationAngle = 0.0f,
             bool headlightsEnabled = false,
             bool taillightsEnabled = false,
             bool faceRight = true);

void initRain();
void drawRain();
void drawHouseWindow(float windowLeftX, float windowBottomY, float windowWidth, float windowHeight,
                     bool morningMode, bool windowLit);
void drawHeadlightCone(float carPositionX, float carPositionY, bool coneEnabled);
void drawHouse(bool isLit);
void drawTree(float treeBaseX, float treeBaseY);
void drawBuilding(float buildingPositionX, float buildingBottomY, float buildingWidth, float buildingHeight);
void drawRoad();
void drawCharacter(float characterPositionX, float characterPositionY);
void drawFan(float fanCenterX, float fanCenterY);
void drawCloud(float cloudCenterX, float cloudCenterY);
void drawStars();
void drawTextIfNeeded();
void drawControlHints();

void drawBitmapText(float textPositionX, float textPositionY, const char* textString);
void drawSun(float sunCenterX, float sunCenterY);
void drawMoon(float moonCenterX, float moonCenterY);
void drawMorningHomeSky();
void drawNightHomeSky();
void drawHomeGroundAndDriveway(bool nightMode);
void drawParallaxCity(float baseParallaxOffset, bool nightMode);
void drawStreetLight(float poleBaseX, float poleBaseY, bool glowEnabled);
void drawOfficeComplex(bool eveningLightingEnabled);
void drawOfficeRampDown();
void drawOfficeRampUp();
void drawParkingBarrier(float pivotX, float pivotY, float rotationAngleDegrees);
void drawWallClock(float centerX, float centerY, float handAngleDegrees);
void drawOfficeDeskSetup(float deskLeftX, float deskBottomY);
void drawCoffeeMachine(float machineLeftX, float machineBottomY);
void drawSteamParticles();
void drawPresentationBoard(float boardLeftX, float boardBottomY, float boardWidth, float boardHeight);

void drawScene1MorningHomeDeparture();
void drawScene2MorningTraffic();
void drawScene3OfficeArrival();
void drawScene4MainOffice();
void drawScene5CoffeeBreak();
void drawScene6Presentation();
void drawScene7LeavingOffice();
void drawScene8EveningTraffic();
void drawScene9ReturnHome();

void initializeSteamParticles();
void resetVariablesForScene(int sceneIndex);
void moveToNextScene();

void updateScene1Animation();
void updateScene2Animation();
void updateScene3Animation();
void updateScene4Animation();
void updateScene5Animation();
void updateScene6Animation();
void updateScene7Animation();
void updateScene8Animation();
void updateScene9Animation();

void reshape(int newWidth, int newHeight);
void display();
void update(int value);
void keyboard(unsigned char key, int x, int y);

// ==========================================================
// ====== UTILITY FUNCTIONS ==================================
// ==========================================================

float clampFloat(float value, float minValue, float maxValue) {
    if (value < minValue) {
        return minValue;
    }
    if (value > maxValue) {
        return maxValue;
    }
    return value;
}

float wrapOffsetToRange(float value, float minValue, float maxValue) {
    const float span = maxValue - minValue;
    if (span <= 0.0f) {
        return minValue;
    }

    while (value < minValue) {
        value += span;
    }
    while (value > maxValue) {
        value -= span;
    }
    return value;
}

// ==========================================================
// ====== BASIC 2D DRAWING PRIMITIVES =======================
// ==========================================================

void drawRectangle(float x, float y, float width, float height) {
    glBegin(GL_QUADS);
    glVertex2f(x,         y);          // Bottom-left corner
    glVertex2f(x + width, y);          // Bottom-right corner
    glVertex2f(x + width, y + height); // Top-right corner
    glVertex2f(x,         y + height); // Top-left corner
    glEnd();
}

void drawFilledCircle(float centerX, float centerY, float radius, int segmentCount) {
    if (segmentCount < 12) {
        segmentCount = 12;
    }

    glBegin(GL_POLYGON);
    for (int segmentIndex = 0; segmentIndex < segmentCount; ++segmentIndex) {
        const float angleRadians = (2.0f * PI_VALUE * static_cast<float>(segmentIndex)) / static_cast<float>(segmentCount);
        const float vertexX = centerX + std::cos(angleRadians) * radius;
        const float vertexY = centerY + std::sin(angleRadians) * radius;
        glVertex2f(vertexX, vertexY);
    }
    glEnd();
}

void drawFilledEllipse(float centerX, float centerY, float radiusX, float radiusY, int segmentCount) {
    if (segmentCount < 12) {
        segmentCount = 12;
    }

    glBegin(GL_POLYGON);
    for (int segmentIndex = 0; segmentIndex < segmentCount; ++segmentIndex) {
        const float angleRadians = (2.0f * PI_VALUE * static_cast<float>(segmentIndex)) / static_cast<float>(segmentCount);
        const float vertexX = centerX + std::cos(angleRadians) * radiusX;
        const float vertexY = centerY + std::sin(angleRadians) * radiusY;
        glVertex2f(vertexX, vertexY);
    }
    glEnd();
}

void drawVerticalSkyGradient(float bottomRed, float bottomGreen, float bottomBlue,
                             float topRed, float topGreen, float topBlue) {
    const float skyLeftX = ORTHO_LEFT_X;    // Gradient starts at left border
    const float skyRightX = ORTHO_RIGHT_X;  // Gradient ends at right border
    const float skyBottomY = ORTHO_BOTTOM_Y; // Gradient bottom at world bottom
    const float skyTopY = ORTHO_TOP_Y;      // Gradient top at world top

    glBegin(GL_QUADS);
    glColor3f(bottomRed, bottomGreen, bottomBlue);
    glVertex2f(skyLeftX,  skyBottomY); // Bottom-left gradient point
    glVertex2f(skyRightX, skyBottomY); // Bottom-right gradient point

    glColor3f(topRed, topGreen, topBlue);
    glVertex2f(skyRightX, skyTopY);    // Top-right gradient point
    glVertex2f(skyLeftX,  skyTopY);    // Top-left gradient point
    glEnd();
}

// ==========================================================
// ====== REQUIRED REUSABLE DRAW HELPERS ====================
// ==========================================================

void drawWheel(float wheelCenterX, float wheelCenterY, float wheelRotationAngle) {
    const float tireOuterRadius = 16.0f; // Outer tire radius
    const float tireInnerRadius = 13.0f; // Inner tire ring radius
    const float rimRadius = 8.0f;        // Metallic rim radius

    glPushMatrix();
    glTranslatef(wheelCenterX, wheelCenterY, 0.0f);
    glRotatef(wheelRotationAngle, 0.0f, 0.0f, 1.0f);

    // Tire outer rubber.
    glColor3f(0.06f, 0.06f, 0.07f);
    drawFilledCircle(0.0f, 0.0f, tireOuterRadius, 36);

    // Tire inner ring.
    glColor3f(0.14f, 0.14f, 0.16f);
    drawFilledCircle(0.0f, 0.0f, tireInnerRadius, 34);

    // Rim.
    glColor3f(0.68f, 0.68f, 0.72f);
    drawFilledCircle(0.0f, 0.0f, rimRadius, 30);

    // Spokes.
    glColor3f(0.92f, 0.92f, 0.94f);
    glLineWidth(2.0f);
    glBegin(GL_LINES);
    glVertex2f(-rimRadius, 0.0f); glVertex2f(rimRadius, 0.0f);       // Horizontal spoke
    glVertex2f(0.0f, -rimRadius); glVertex2f(0.0f, rimRadius);       // Vertical spoke
    glVertex2f(-5.5f, -5.5f);     glVertex2f(5.5f, 5.5f);            // Diagonal spoke 1
    glVertex2f(-5.5f, 5.5f);      glVertex2f(5.5f, -5.5f);           // Diagonal spoke 2
    glEnd();
    glLineWidth(1.0f);

    // Hub cap center.
    glColor3f(0.82f, 0.82f, 0.86f);
    drawFilledCircle(0.0f, 0.0f, 2.8f, 16);

    glPopMatrix();
}

void drawCar(float carPositionX, float carPositionY, float bodyRed, float bodyGreen, float bodyBlue,
             float wheelRotationAngle,
             bool headlightsEnabled,
             bool taillightsEnabled,
             bool faceRight) {
    const float carBodyWidth = 142.0f;      // Main body width
    const float carBodyHeight = 32.0f;      // Main body height
    const float wheelOffsetX = 42.0f;       // Horizontal distance from center to each wheel
    const float wheelCenterY = -2.0f;       // Wheel center Y relative to car origin

    glPushMatrix();
    glTranslatef(carPositionX, carPositionY, 0.0f);

    if (!faceRight) {
        glScalef(-1.0f, 1.0f, 1.0f); // Mirror car to face left
    }

    // Soft shadow under car for depth.
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    glColor4f(0.0f, 0.0f, 0.0f, 0.24f);
    drawFilledEllipse(0.0f, -10.0f, 74.0f, 11.0f, 36);

    // Car lower body.
    glColor3f(bodyRed, bodyGreen, bodyBlue);
    drawRectangle(-carBodyWidth * 0.5f, 0.0f, carBodyWidth, carBodyHeight);

    // Car roof section.
    glBegin(GL_POLYGON);
    glVertex2f(-38.0f, 32.0f); // Rear roof lower point
    glVertex2f( 42.0f, 32.0f); // Front roof lower point
    glVertex2f( 20.0f, 58.0f); // Front roof upper point
    glVertex2f(-20.0f, 58.0f); // Rear roof upper point
    glEnd();

    // Front and rear bumper strips.
    glColor3f(0.18f, 0.18f, 0.20f);
    drawRectangle( 64.0f, 8.0f, 10.0f, 14.0f); // Front bumper block
    drawRectangle(-74.0f, 8.0f, 10.0f, 14.0f); // Rear bumper block

    // Window glass panel.
    glColor3f(0.64f, 0.84f, 0.96f);
    glBegin(GL_POLYGON);
    glVertex2f(-30.0f, 35.0f); // Rear window bottom-left
    glVertex2f( 34.0f, 35.0f); // Front window bottom-right
    glVertex2f( 16.0f, 54.0f); // Front window top-right
    glVertex2f(-14.0f, 54.0f); // Rear window top-left
    glEnd();

    // Door split line and side molding.
    glColor3f(0.20f, 0.20f, 0.20f);
    glLineWidth(2.0f);
    glBegin(GL_LINES);
    glVertex2f(0.0f, 2.0f);  glVertex2f(0.0f, 55.0f);  // Middle door separation
    glVertex2f(-58.0f, 14.0f); glVertex2f(58.0f, 14.0f); // Side molding line
    glEnd();
    glLineWidth(1.0f);

    // Wheels.
    drawWheel(-wheelOffsetX, wheelCenterY, wheelRotationAngle);
    drawWheel( wheelOffsetX, wheelCenterY, wheelRotationAngle);

    // Headlights: transparent yellow triangles for fake light beam.
    if (headlightsEnabled) {
        glColor4f(1.0f, 0.96f, 0.42f, 0.30f);
        glBegin(GL_TRIANGLES);
        glVertex2f(70.0f, 20.0f);   // Triangle origin at front bumper
        glVertex2f(186.0f, 58.0f);  // Upper light spread point
        glVertex2f(186.0f, -10.0f); // Lower light spread point
        glEnd();

        glColor3f(1.0f, 1.0f, 0.72f);
        drawRectangle(66.0f, 14.0f, 8.0f, 8.0f); // Headlight block
    }

    // Taillights: red blocks plus soft glow.
    if (taillightsEnabled) {
        glColor3f(0.95f, 0.08f, 0.08f);
        drawRectangle(-74.0f, 16.0f, 8.0f, 8.0f); // Upper taillight
        drawRectangle(-74.0f, 6.0f, 8.0f, 8.0f);  // Lower taillight

        glColor4f(0.95f, 0.12f, 0.12f, 0.24f);
        drawFilledEllipse(-74.0f, 15.0f, 14.0f, 10.0f, 24); // Red glow halo
    }

    glDisable(GL_BLEND);
    glPopMatrix();
}

void drawCloud(float cloudCenterX, float cloudCenterY) {
    const float centerPuffRadius = 28.0f;      // Main middle cloud puff
    const float leftPuffRadius = 22.0f;        // Left cloud puff
    const float rightPuffRadius = 24.0f;       // Right cloud puff
    const float topPuffRadius = 20.0f;         // Top cloud puff

    glColor3f(0.98f, 0.98f, 0.99f);
    drawFilledCircle(cloudCenterX,          cloudCenterY,          centerPuffRadius, 28);
    drawFilledCircle(cloudCenterX - 30.0f,  cloudCenterY - 4.0f,   leftPuffRadius,   24);
    drawFilledCircle(cloudCenterX + 30.0f,  cloudCenterY - 2.0f,   rightPuffRadius,  24);
    drawFilledCircle(cloudCenterX + 2.0f,   cloudCenterY + 18.0f,  topPuffRadius,    24);
}

void drawTree(float treeBaseX, float treeBaseY) {
    const float trunkWidth = 18.0f;   // Rectangle trunk width
    const float trunkHeight = 78.0f;  // Rectangle trunk height

    // Trunk: simple brown rectangle for viva-friendly explanation.
    glColor3f(0.42f, 0.26f, 0.14f);
    drawRectangle(treeBaseX - trunkWidth * 0.5f, treeBaseY, trunkWidth, trunkHeight);

    // Leaves use overlapping polygons to avoid a perfect circular tree shape.
    glColor3f(0.16f, 0.46f, 0.18f);
    glBegin(GL_POLYGON);
    glVertex2f(treeBaseX - 44.0f, treeBaseY + 78.0f);  // Lower-left leaf corner
    glVertex2f(treeBaseX - 26.0f, treeBaseY + 112.0f); // Upper-left leaf corner
    glVertex2f(treeBaseX + 10.0f, treeBaseY + 124.0f); // Upper-middle leaf corner
    glVertex2f(treeBaseX + 46.0f, treeBaseY + 98.0f);  // Upper-right leaf corner
    glVertex2f(treeBaseX + 26.0f, treeBaseY + 68.0f);  // Lower-right leaf corner
    glVertex2f(treeBaseX - 16.0f, treeBaseY + 62.0f);  // Lower-middle leaf corner
    glEnd();

    glColor3f(0.20f, 0.56f, 0.22f);
    glBegin(GL_POLYGON);
    glVertex2f(treeBaseX - 30.0f, treeBaseY + 96.0f);  // Left overlap leaf point
    glVertex2f(treeBaseX - 8.0f, treeBaseY + 136.0f);  // Top-left overlap leaf point
    glVertex2f(treeBaseX + 22.0f, treeBaseY + 140.0f); // Top-right overlap leaf point
    glVertex2f(treeBaseX + 40.0f, treeBaseY + 108.0f); // Right overlap leaf point
    glVertex2f(treeBaseX + 14.0f, treeBaseY + 86.0f);  // Bottom-right overlap leaf point
    glVertex2f(treeBaseX - 18.0f, treeBaseY + 84.0f);  // Bottom-left overlap leaf point
    glEnd();

    glColor3f(0.12f, 0.38f, 0.14f);
    glBegin(GL_POLYGON);
    glVertex2f(treeBaseX - 14.0f, treeBaseY + 120.0f); // Top canopy left point
    glVertex2f(treeBaseX + 4.0f, treeBaseY + 152.0f);  // Top canopy peak point
    glVertex2f(treeBaseX + 24.0f, treeBaseY + 126.0f); // Top canopy right point
    glVertex2f(treeBaseX + 6.0f, treeBaseY + 108.0f);  // Top canopy lower point
    glEnd();
}

void drawStars() {
    glPointSize(2.8f);
    glBegin(GL_POINTS);

    for (int starIndex = 0; starIndex < STAR_COUNT; ++starIndex) {
        // Brightness oscillates over time to create twinkling effect.
        const float phaseShift = static_cast<float>(starIndex) * 0.71f;
        const float wave = 0.5f + 0.5f * std::sin(starTwinkleCounter + phaseShift);
        const float brightness = 0.35f + 0.65f * wave;

        glColor3f(brightness, brightness, brightness * 0.92f);
        glVertex2f(starPositionX[starIndex], starPositionY[starIndex]);
    }

    glEnd();
    glPointSize(1.0f);
}

void drawRoad() {
    const float roadLeftX = 0.0f;                 // Road starts at screen left border
    const float roadWidth = 1280.0f;              // Road covers full screen width
    const float edgeLineThickness = 5.0f;         // Thickness for road edge lines

    if (roadNightMode_current) {
        glColor3f(0.10f, 0.10f, 0.12f); // Dark asphalt at night
    } else {
        glColor3f(0.23f, 0.23f, 0.25f); // Gray asphalt in daytime
    }
    drawRectangle(roadLeftX, roadBottomY_current, roadWidth, roadHeight_current);

    // Road boundary lines.
    glColor3f(0.75f, 0.75f, 0.78f);
    drawRectangle(roadLeftX, roadBottomY_current, roadWidth, edgeLineThickness);
    drawRectangle(roadLeftX, roadBottomY_current + roadHeight_current - edgeLineThickness, roadWidth, edgeLineThickness);

    // Lane markers.
    const float laneHeight = roadHeight_current / static_cast<float>(roadLaneCount_current);
    const float dashWidth = 48.0f;      // Width of each lane marker dash
    const float dashHeight = 4.0f;      // Thickness of dash
    const float dashGap = 28.0f;        // Gap between dashes

    if (roadNightMode_current) {
        glColor3f(0.92f, 0.92f, 0.64f); // Slight warm yellow at night
    } else {
        glColor3f(0.95f, 0.95f, 0.72f); // Bright yellow in daytime
    }

    for (int laneIndex = 1; laneIndex < roadLaneCount_current; ++laneIndex) {
        const float laneMarkerY = roadBottomY_current + laneHeight * static_cast<float>(laneIndex);

        for (float dashX = 20.0f; dashX < 1280.0f; dashX += (dashWidth + dashGap)) {
            drawRectangle(dashX, laneMarkerY - dashHeight * 0.5f, dashWidth, dashHeight);
        }
    }
}

void drawBuilding(float buildingPositionX, float buildingBottomY, float buildingWidth, float buildingHeight) {
    // Building body color is varied slightly by position to avoid repetition.
    const float colorSeed = std::fabs(std::sin(buildingPositionX * 0.007f));
    const float baseRed = 0.22f + 0.16f * colorSeed;
    const float baseGreen = 0.24f + 0.14f * colorSeed;
    const float baseBlue = 0.28f + 0.12f * colorSeed;

    glColor3f(baseRed, baseGreen, baseBlue);
    drawRectangle(buildingPositionX, buildingBottomY, buildingWidth, buildingHeight);

    // Roof strip.
    glColor3f(baseRed * 0.8f, baseGreen * 0.8f, baseBlue * 0.8f);
    drawRectangle(buildingPositionX, buildingBottomY + buildingHeight, buildingWidth, 10.0f);

    // Window grid setup.
    const float windowMarginX = 11.0f; // Left/right inner margin for windows
    const float windowMarginY = 12.0f; // Bottom/top inner margin for windows
    const float windowWidth = 12.0f;   // Width of each small window
    const float windowHeight = 15.0f;  // Height of each small window
    const float windowGapX = 10.0f;    // Horizontal spacing between windows
    const float windowGapY = 10.0f;    // Vertical spacing between windows

    const int windowColumnCount = static_cast<int>((buildingWidth - 2.0f * windowMarginX) / (windowWidth + windowGapX));
    const int windowRowCount = static_cast<int>((buildingHeight - 2.0f * windowMarginY) / (windowHeight + windowGapY));

    for (int rowIndex = 0; rowIndex < windowRowCount; ++rowIndex) {
        for (int columnIndex = 0; columnIndex < windowColumnCount; ++columnIndex) {
            const float windowX = buildingPositionX + windowMarginX + columnIndex * (windowWidth + windowGapX);
            const float windowY = buildingBottomY + windowMarginY + rowIndex * (windowHeight + windowGapY);

            if (buildingWindowNightMode) {
                const float flicker = 0.45f + 0.55f * (0.5f + 0.5f * std::sin(starTwinkleCounter + rowIndex + columnIndex * 0.5f));
                glColor3f(0.55f + 0.45f * flicker, 0.50f + 0.35f * flicker, 0.22f + 0.18f * flicker);
            } else {
                glColor3f(0.62f, 0.78f, 0.90f);
            }

            drawRectangle(windowX, windowY, windowWidth, windowHeight);
        }
    }
}

void drawFan(float fanCenterX, float fanCenterY) {
    const float rodWidth = 6.0f;       // Ceiling rod width
    const float rodHeight = 38.0f;     // Ceiling rod height
    const float bladeLength = 62.0f;   // Each blade extends this far
    const float bladeHalfWidth = 10.0f; // Half thickness of blade

    // Rod from ceiling.
    glColor3f(0.34f, 0.34f, 0.36f);
    drawRectangle(fanCenterX - rodWidth * 0.5f, fanCenterY - rodHeight, rodWidth, rodHeight);

    glPushMatrix();
    glTranslatef(fanCenterX, fanCenterY - rodHeight, 0.0f); // Fan hub pivot point

    glColor3f(0.25f, 0.25f, 0.27f);
    drawFilledCircle(0.0f, 0.0f, 9.0f, 24); // Hub circle

    // We rotate the whole blade set every frame to simulate spinning fan.
    glRotatef(fanRotationAngle_scene4, 0.0f, 0.0f, 1.0f);

    glColor3f(0.62f, 0.62f, 0.64f);
    for (int bladeIndex = 0; bladeIndex < 3; ++bladeIndex) {
        glRotatef(120.0f, 0.0f, 0.0f, 1.0f);

        glBegin(GL_TRIANGLES);
        glVertex2f(0.0f, 0.0f);                      // Blade root at hub center
        glVertex2f(bladeLength, -bladeHalfWidth);    // Blade tip lower point
        glVertex2f(bladeLength, bladeHalfWidth);     // Blade tip upper point
        glEnd();
    }

    glPopMatrix();
}

void drawCharacter(float characterPositionX, float characterPositionY) {
    // Character dimensions (simple stylized human for explainability).
    const float footToHipHeight = 30.0f;      // Leg length from foot to hip
    const float torsoHeight = 42.0f;          // Torso height
    const float torsoWidth = 28.0f;           // Torso width
    const float headRadius = 11.0f;           // Head circle radius

    glPushMatrix();
    glTranslatef(characterPositionX, characterPositionY, 0.0f);

    if (!characterFacingRight) {
        glScalef(-1.0f, 1.0f, 1.0f); // Mirror model so it can face left
    }

    // Legs.
    glColor3f(0.14f, 0.14f, 0.20f);

    if (characterSittingPose) {
        // Sitting pose: upper legs are forward and lower legs are downward.
        drawRectangle(-8.0f, 14.0f, 6.0f, 16.0f); // Left upper leg
        drawRectangle( 2.0f, 14.0f, 6.0f, 16.0f); // Right upper leg
        drawRectangle(-8.0f,  0.0f, 6.0f, 16.0f); // Left lower leg
        drawRectangle( 2.0f,  0.0f, 6.0f, 16.0f); // Right lower leg
    } else {
        // Standing/walking pose with opposite swing on two legs.
        glPushMatrix();
        glTranslatef(-7.0f, footToHipHeight, 0.0f);                // Left leg pivot at hip
        glRotatef(characterLegSwingAngleDegrees, 0.0f, 0.0f, 1.0f);
        drawRectangle(-3.0f, -footToHipHeight, 6.0f, footToHipHeight);
        glPopMatrix();

        glPushMatrix();
        glTranslatef(7.0f, footToHipHeight, 0.0f);                 // Right leg pivot at hip
        glRotatef(-characterLegSwingAngleDegrees, 0.0f, 0.0f, 1.0f);
        drawRectangle(-3.0f, -footToHipHeight, 6.0f, footToHipHeight);
        glPopMatrix();
    }

    // Shoes.
    glColor3f(0.08f, 0.08f, 0.09f);
    drawRectangle(-10.0f, -2.0f, 9.0f, 4.0f); // Left shoe
    drawRectangle(  1.0f, -2.0f, 9.0f, 4.0f); // Right shoe

    // Torso.
    glColor3f(0.14f, 0.16f, 0.28f);
    drawRectangle(-torsoWidth * 0.5f, footToHipHeight, torsoWidth, torsoHeight);

    // Shirt collar / tie for formal office look.
    glColor3f(0.95f, 0.95f, 0.95f);
    drawRectangle(-6.0f, footToHipHeight + torsoHeight - 4.0f, 12.0f, 4.0f);
    glColor3f(0.72f, 0.10f, 0.12f);
    drawRectangle(-2.0f, footToHipHeight + 8.0f, 4.0f, 22.0f);

    // Arms.
    glColor3f(0.14f, 0.16f, 0.28f);

    glPushMatrix();
    glTranslatef(-torsoWidth * 0.5f, footToHipHeight + torsoHeight - 6.0f, 0.0f); // Left shoulder pivot
    glRotatef(characterLeftArmAngleDegrees, 0.0f, 0.0f, 1.0f);
    drawRectangle(-4.0f, -24.0f, 8.0f, 24.0f);
    glPopMatrix();

    float rightArmDrawAngle = characterRightArmAngleDegrees;
    if (characterPointingPose) {
        rightArmDrawAngle = -58.0f + pointerOscillationAngle_scene6; // Raise arm toward board
    }

    glPushMatrix();
    glTranslatef(torsoWidth * 0.5f, footToHipHeight + torsoHeight - 6.0f, 0.0f); // Right shoulder pivot
    glRotatef(rightArmDrawAngle, 0.0f, 0.0f, 1.0f);
    drawRectangle(-4.0f, -24.0f, 8.0f, 24.0f);
    glPopMatrix();

    // Head and face.
    glColor3f(0.88f, 0.72f, 0.58f);
    drawFilledCircle(0.0f, footToHipHeight + torsoHeight + headRadius + 3.0f, headRadius, 28);

    glColor3f(0.08f, 0.08f, 0.09f);
    drawRectangle(-9.0f, footToHipHeight + torsoHeight + 16.0f, 18.0f, 4.0f); // Hair strip

    // Optional briefcase in right hand.
    if (characterCarryBriefcase) {
        glColor3f(0.28f, 0.18f, 0.10f);
        drawRectangle(15.0f, 12.0f, 18.0f, 22.0f);
        glColor3f(0.45f, 0.34f, 0.20f);
        drawRectangle(20.0f, 34.0f, 8.0f, 4.0f);
    }

    glPopMatrix();
}

// ==========================================================
// ====== COMMON ENVIRONMENT HELPERS ========================
// ==========================================================

void drawBitmapText(float textPositionX, float textPositionY, const char* textString) {
    glRasterPos2f(textPositionX, textPositionY);
    for (const char* characterPointer = textString; *characterPointer != '\0'; ++characterPointer) {
        glutBitmapCharacter(GLUT_BITMAP_HELVETICA_18, *characterPointer);
    }
}

void drawTextIfNeeded() {
    if (!showTextOverlay) {
        return;
    }

    const char* sceneTitleText[10] = {
        "",
        "Scene 1: Morning Home Departure",
        "Scene 2: Morning Traffic",
        "Scene 3: Office Arrival",
        "Scene 4: Main Office Work",
        "Scene 5: Coffee Break",
        "Scene 6: Presentation",
        "Scene 7: Leaving Office",
        "Scene 8: Evening Traffic",
        "Scene 9: Return Home"
    };

    char frameInfoText[128];
    std::snprintf(frameInfoText, sizeof(frameInfoText), "Frame: %d", sceneFrameCounter);

    // Text shadow for readability on bright and dark backgrounds.
    glColor3f(0.0f, 0.0f, 0.0f);
    drawBitmapText(23.0f, 692.0f, sceneTitleText[currentScene]);
    drawBitmapText(23.0f, 664.0f, frameInfoText);

    glColor3f(1.0f, 1.0f, 1.0f);
    drawBitmapText(20.0f, 695.0f, sceneTitleText[currentScene]);
    drawBitmapText(20.0f, 667.0f, frameInfoText);
}

void drawControlHints() {
    // Top-left info and keyboard hint panel.
    const float panelLeftX = 14.0f;
    const float panelTopY = 706.0f;
    const float panelWidth = 430.0f;
    const float panelHeight = 196.0f;
    const float panelBottomY = panelTopY - panelHeight;

    const char* sceneNames[10] = {
        "",
        "Scene 1: Rainy Morning Departure",
        "Scene 2: Morning Traffic",
        "Scene 3: Office Arrival",
        "Scene 4: Main Office Work",
        "Scene 5: Coffee Break",
        "Scene 6: Presentation",
        "Scene 7: Leaving Office",
        "Scene 8: Evening Traffic",
        "Scene 9: Night Return"
    };

    char frameCountText[80];
    std::snprintf(frameCountText, sizeof(frameCountText), "Frame: %d", totalFrameCounter);

    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    glColor4f(0.0f, 0.0f, 0.0f, 0.38f);
    drawRectangle(panelLeftX, panelBottomY, panelWidth, panelHeight);

    glDisable(GL_BLEND);

    glColor3f(1.0f, 1.0f, 1.0f);
    drawBitmapText(panelLeftX + 10.0f, panelTopY - 20.0f, "Scene info:");
    drawBitmapText(panelLeftX + 10.0f, panelTopY - 44.0f, sceneNames[currentScene]);

    drawBitmapText(panelLeftX + 10.0f, panelTopY - 74.0f, "Keybinds:");
    drawBitmapText(panelLeftX + 10.0f, panelTopY - 98.0f,  "SPACE : Pause / Resume");
    drawBitmapText(panelLeftX + 10.0f, panelTopY - 120.0f, "R     : Rain ON");
    drawBitmapText(panelLeftX + 10.0f, panelTopY - 142.0f, "V     : Rain OFF");
    drawBitmapText(panelLeftX + 10.0f, panelTopY - 164.0f, "N     : Restart from Scene 1");

    drawBitmapText(panelLeftX + 10.0f, panelTopY - 186.0f, frameCountText);
}

void drawSun(float sunCenterX, float sunCenterY) {
    glColor3f(1.0f, 0.92f, 0.32f);
    drawFilledCircle(sunCenterX, sunCenterY, 36.0f, 36);

    glColor3f(1.0f, 0.84f, 0.22f);
    glLineWidth(2.0f);
    glBegin(GL_LINES);
    glVertex2f(sunCenterX - 54.0f, sunCenterY); glVertex2f(sunCenterX - 38.0f, sunCenterY); // Left ray
    glVertex2f(sunCenterX + 38.0f, sunCenterY); glVertex2f(sunCenterX + 54.0f, sunCenterY); // Right ray
    glVertex2f(sunCenterX, sunCenterY - 54.0f); glVertex2f(sunCenterX, sunCenterY - 38.0f); // Bottom ray
    glVertex2f(sunCenterX, sunCenterY + 38.0f); glVertex2f(sunCenterX, sunCenterY + 54.0f); // Top ray
    glEnd();
    glLineWidth(1.0f);
}

void drawMoon(float moonCenterX, float moonCenterY) {
    glColor3f(0.90f, 0.92f, 0.96f);
    drawFilledCircle(moonCenterX, moonCenterY, 30.0f, 34);

    // Crescent effect by drawing a dark overlapping circle.
    glColor3f(0.08f, 0.10f, 0.18f);
    drawFilledCircle(moonCenterX + 10.0f, moonCenterY + 4.0f, 26.0f, 34);
}

void drawMorningHomeSky() {
    // Light blue morning sky gradient.
    drawVerticalSkyGradient(
        0.68f, 0.88f, 0.99f, // Bottom sky tint
        0.88f, 0.96f, 1.00f  // Top sky tint
    );

    // Sun at upper-right region.
    drawSun(1020.0f + sunHorizontalOffset * 0.20f, 620.0f);

    // Morning clouds.
    drawCloud(210.0f + cloudOffsetX_layerA * 0.70f, 620.0f);
    drawCloud(520.0f + cloudOffsetX_layerB * 0.60f, 650.0f);
    drawCloud(870.0f + cloudOffsetX_layerA * 0.50f, 606.0f);
}

void drawNightHomeSky() {
    // Dark navy night sky gradient.
    drawVerticalSkyGradient(
        0.04f, 0.06f, 0.14f, // Bottom night tint
        0.10f, 0.14f, 0.24f  // Top night tint
    );

    // Moon and stars.
    drawMoon(1030.0f, 620.0f);
    drawStars();
}

void drawHomeGroundAndDriveway(bool nightMode) {
    // Bottom road (dark gray, full width).
    if (nightMode) {
        glColor3f(0.10f, 0.10f, 0.12f);
    } else {
        glColor3f(0.20f, 0.20f, 0.22f);
    }
    drawRectangle(0.0f, homeRoadBottomY, 1280.0f, homeRoadHeight);

    // Road top edge line.
    glColor3f(0.66f, 0.66f, 0.70f);
    drawRectangle(0.0f, homeRoadBottomY + homeRoadHeight - 3.0f, 1280.0f, 3.0f);

    // Grass area above road.
    if (nightMode) {
        glColor3f(0.12f, 0.24f, 0.14f);
    } else {
        glColor3f(0.26f, 0.58f, 0.30f);
    }
    drawRectangle(0.0f, homeGrassBottomY, 1280.0f, homeGrassHeight);

    // Driveway polygon connecting garage to road (no gap with road).
    if (nightMode) {
        glColor3f(0.30f, 0.31f, 0.34f);
    } else {
        glColor3f(0.52f, 0.52f, 0.56f);
    }
    glBegin(GL_POLYGON);
    glVertex2f(homeDrivewayRoadLeftX,  homeDrivewayRoadY);  // Lower-left driveway corner near road
    glVertex2f(homeDrivewayRoadRightX, homeDrivewayRoadY);  // Lower-right driveway corner near road
    glVertex2f(homeDrivewayGarageRightX, homeDrivewayGarageY); // Upper-right driveway corner near garage
    glVertex2f(homeDrivewayGarageLeftX,  homeDrivewayGarageY); // Upper-left driveway corner near garage
    glEnd();

    // Driveway side highlight lines.
    if (nightMode) {
        glColor3f(0.52f, 0.54f, 0.58f);
    } else {
        glColor3f(0.72f, 0.72f, 0.74f);
    }
    glLineWidth(2.0f);
    glBegin(GL_LINES);
    glVertex2f(homeDrivewayRoadLeftX, homeDrivewayRoadY);
    glVertex2f(homeDrivewayGarageLeftX, homeDrivewayGarageY);
    glVertex2f(homeDrivewayRoadRightX, homeDrivewayRoadY);
    glVertex2f(homeDrivewayGarageRightX, homeDrivewayGarageY);
    glEnd();
    glLineWidth(1.0f);
}

void drawParallaxCity(float baseParallaxOffset, bool nightMode) {
    buildingWindowNightMode = nightMode;

    // Parallax explanation:
    // Far buildings use smaller offset multiplier, so they move slower.
    // Near buildings use larger offset multiplier, so they move faster.
    const float farLayerOffset = baseParallaxOffset * 0.35f;
    const float nearLayerOffset = baseParallaxOffset * 0.70f;

    const float repeatingBlockWidth = 220.0f; // Horizontal repetition spacing for buildings

    // Far layer (taller, more distant skyline).
    for (int column = -1; column < 8; ++column) {
        const float buildingX = -140.0f + column * repeatingBlockWidth + farLayerOffset;
        drawBuilding(buildingX, 290.0f, 160.0f, 250.0f);
    }

    // Near layer (slightly shorter, appears closer to road).
    for (int column = -1; column < 9; ++column) {
        const float buildingX = -60.0f + column * repeatingBlockWidth + nearLayerOffset;
        drawBuilding(buildingX, 250.0f, 140.0f, 190.0f);
    }
}

void drawStreetLight(float poleBaseX, float poleBaseY, bool glowEnabled) {
    const float poleHeight = 150.0f; // Pole vertical size
    const float poleWidth = 8.0f;    // Pole width

    glColor3f(0.22f, 0.22f, 0.25f);
    drawRectangle(poleBaseX - poleWidth * 0.5f, poleBaseY, poleWidth, poleHeight);

    // Lamp head.
    glColor3f(0.55f, 0.55f, 0.58f);
    drawRectangle(poleBaseX - 14.0f, poleBaseY + poleHeight - 8.0f, 28.0f, 8.0f);

    // Glow effect uses transparent circles.
    if (glowEnabled) {
        glEnable(GL_BLEND);
        glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

        const float pulsation = 0.5f + 0.5f * std::sin(starTwinkleCounter * 1.4f + poleBaseX * 0.03f);
        const float glowAlpha = 0.18f + 0.16f * pulsation;

        glColor4f(1.0f, 0.92f, 0.45f, glowAlpha);
        drawFilledCircle(poleBaseX, poleBaseY + poleHeight - 10.0f, 30.0f, 30);

        glColor4f(1.0f, 0.96f, 0.68f, glowAlpha + 0.12f);
        drawFilledCircle(poleBaseX, poleBaseY + poleHeight - 10.0f, 16.0f, 28);

        glDisable(GL_BLEND);
    }
}

void drawWallClock(float centerX, float centerY, float handAngleDegrees) {
    // Clock body.
    glColor3f(0.94f, 0.94f, 0.94f);
    drawFilledCircle(centerX, centerY, 26.0f, 32);

    // Clock border ring.
    glColor3f(0.25f, 0.25f, 0.28f);
    drawFilledCircle(centerX, centerY, 22.0f, 30);

    // White dial area.
    glColor3f(0.99f, 0.99f, 0.99f);
    drawFilledCircle(centerX, centerY, 20.0f, 28);

    // Minute hand rotates over time.
    const float angleRadians = handAngleDegrees * PI_VALUE / 180.0f;
    const float handEndX = centerX + std::cos(angleRadians) * 14.0f;
    const float handEndY = centerY + std::sin(angleRadians) * 14.0f;

    glColor3f(0.10f, 0.10f, 0.10f);
    glLineWidth(2.0f);
    glBegin(GL_LINES);
    glVertex2f(centerX, centerY);  glVertex2f(handEndX, handEndY); // Minute hand
    glVertex2f(centerX, centerY);  glVertex2f(centerX + 0.0f, centerY + 10.0f); // Hour hand fixed simple
    glEnd();
    glLineWidth(1.0f);
}

void drawOfficeDeskSetup(float deskLeftX, float deskBottomY) {
    // Desk top and body.
    glColor3f(0.45f, 0.30f, 0.16f);
    drawRectangle(deskLeftX, deskBottomY + 50.0f, 260.0f, 16.0f); // Table top board
    drawRectangle(deskLeftX + 20.0f, deskBottomY, 22.0f, 50.0f);  // Left leg
    drawRectangle(deskLeftX + 218.0f, deskBottomY, 22.0f, 50.0f); // Right leg

    // Computer monitor.
    glColor3f(0.14f, 0.14f, 0.16f);
    drawRectangle(deskLeftX + 148.0f, deskBottomY + 70.0f, 82.0f, 56.0f); // Monitor frame
    glColor3f(0.28f, 0.40f, 0.48f);
    drawRectangle(deskLeftX + 154.0f, deskBottomY + 76.0f, 70.0f, 44.0f); // Monitor screen

    glColor3f(0.22f, 0.22f, 0.24f);
    drawRectangle(deskLeftX + 182.0f, deskBottomY + 58.0f, 14.0f, 12.0f); // Monitor stand

    // Keyboard.
    glColor3f(0.18f, 0.18f, 0.20f);
    drawRectangle(deskLeftX + 126.0f, deskBottomY + 52.0f, 64.0f, 6.0f);

    // Chair.
    glColor3f(0.18f, 0.20f, 0.32f);
    drawRectangle(deskLeftX + 48.0f, deskBottomY + 20.0f, 36.0f, 30.0f); // Chair seat
    drawRectangle(deskLeftX + 46.0f, deskBottomY + 50.0f, 40.0f, 40.0f); // Chair back
}

void drawCoffeeMachine(float machineLeftX, float machineBottomY) {
    // Machine body.
    glColor3f(0.22f, 0.22f, 0.24f);
    drawRectangle(machineLeftX, machineBottomY, 90.0f, 120.0f);

    // Display panel.
    glColor3f(0.30f, 0.54f, 0.68f);
    drawRectangle(machineLeftX + 20.0f, machineBottomY + 80.0f, 50.0f, 24.0f);

    // Nozzle.
    glColor3f(0.70f, 0.70f, 0.72f);
    drawRectangle(machineLeftX + 42.0f, machineBottomY + 56.0f, 8.0f, 16.0f);

    // Cup on tray.
    glColor3f(0.88f, 0.88f, 0.88f);
    drawRectangle(machineLeftX + 35.0f, machineBottomY + 22.0f, 22.0f, 20.0f);
    glColor3f(0.28f, 0.20f, 0.12f);
    drawRectangle(machineLeftX + 38.0f, machineBottomY + 30.0f, 16.0f, 10.0f);
}

void drawSteamParticles() {
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    for (int particleIndex = 0; particleIndex < STEAM_PARTICLE_COUNT; ++particleIndex) {
        const SteamParticle& particle = coffeeSteamParticles[particleIndex];
        glColor4f(0.95f, 0.95f, 0.95f, clampFloat(particle.alphaValue, 0.0f, 1.0f));
        drawFilledCircle(particle.particleX, particle.particleY, 5.0f, 20);
    }

    glDisable(GL_BLEND);
}

void drawPresentationBoard(float boardLeftX, float boardBottomY, float boardWidth, float boardHeight) {
    // White board panel.
    glColor3f(0.95f, 0.95f, 0.95f);
    drawRectangle(boardLeftX, boardBottomY, boardWidth, boardHeight);

    // Border.
    glColor3f(0.30f, 0.30f, 0.32f);
    drawRectangle(boardLeftX, boardBottomY + boardHeight, boardWidth, 8.0f);

    // Animated bar chart values.
    const float barBaseY = boardBottomY + 28.0f; // Shared baseline for chart bars
    const float barWidth = 42.0f;                // Width of each bar
    const float barGap = 24.0f;                  // Horizontal gap between bars

    const float targetBarHeightA = 90.0f;  // Final bar A height
    const float targetBarHeightB = 130.0f; // Final bar B height
    const float targetBarHeightC = 170.0f; // Final bar C height
    const float targetBarHeightD = 210.0f; // Final bar D height

    const float animatedHeightA = targetBarHeightA * presentationBarGrowRatio_scene6;
    const float animatedHeightB = targetBarHeightB * presentationBarGrowRatio_scene6;
    const float animatedHeightC = targetBarHeightC * presentationBarGrowRatio_scene6;
    const float animatedHeightD = targetBarHeightD * presentationBarGrowRatio_scene6;

    glColor3f(0.24f, 0.60f, 0.84f);
    drawRectangle(boardLeftX + 40.0f, barBaseY, barWidth, animatedHeightA);

    glColor3f(0.28f, 0.72f, 0.46f);
    drawRectangle(boardLeftX + 40.0f + (barWidth + barGap), barBaseY, barWidth, animatedHeightB);

    glColor3f(0.94f, 0.66f, 0.20f);
    drawRectangle(boardLeftX + 40.0f + 2.0f * (barWidth + barGap), barBaseY, barWidth, animatedHeightC);

    glColor3f(0.88f, 0.32f, 0.28f);
    drawRectangle(boardLeftX + 40.0f + 3.0f * (barWidth + barGap), barBaseY, barWidth, animatedHeightD);

    // Baseline.
    glColor3f(0.35f, 0.35f, 0.35f);
    drawRectangle(boardLeftX + 30.0f, barBaseY - 4.0f, boardWidth - 60.0f, 4.0f);
}

// ==========================================================
// ====== DEVELOPER 1: HOME MODULE (SCENE 1 & 9) ============
// ==========================================================

void initRain() {
    for (int rainIndex = 0; rainIndex < RAIN_DROP_COUNT; ++rainIndex) {
        rainDropX[rainIndex] = static_cast<float>(std::rand() % WINDOW_WIDTH);
        rainDropY[rainIndex] = static_cast<float>(std::rand() % WINDOW_HEIGHT);
    }
}

void drawRain() {
    // Rain is simulated by repeatedly moving line segments downward.
    glColor3f(0.70f, 0.74f, 0.80f);
    glLineWidth(1.4f);
    glBegin(GL_LINES);
    for (int rainIndex = 0; rainIndex < RAIN_DROP_COUNT; ++rainIndex) {
        glVertex2f(rainDropX[rainIndex], rainDropY[rainIndex]);
        glVertex2f(rainDropX[rainIndex] + 3.0f, rainDropY[rainIndex] - 14.0f);
    }
    glEnd();
    glLineWidth(1.0f);
}

void drawHouseWindow(float windowLeftX, float windowBottomY, float windowWidth, float windowHeight,
                     bool morningMode, bool windowLit) {
    const float frameThickness = 3.0f; // Thickness of dark outer frame
    const float splitThickness = 2.0f; // Thickness of middle split bars

    // Outer frame.
    glColor3f(0.20f, 0.18f, 0.16f);
    drawRectangle(windowLeftX, windowBottomY, windowWidth, windowHeight);

    // Glass fill color depends on Scene 1 / Scene 9 lighting state.
    if (morningMode) {
        glColor3f(0.60f, 0.74f, 0.84f); // Scene 1: muted light-blue glass
    } else if (windowLit) {
        glColor3f(0.98f, 0.88f, 0.54f); // Scene 9 ON: warm yellow interior light
    } else {
        glColor3f(0.12f, 0.14f, 0.18f); // Scene 9 OFF: dark window
    }
    drawRectangle(windowLeftX + frameThickness,
                  windowBottomY + frameThickness,
                  windowWidth - 2.0f * frameThickness,
                  windowHeight - 2.0f * frameThickness);

    // Window split bars (cross frame).
    glColor3f(0.18f, 0.16f, 0.14f);
    drawRectangle(windowLeftX + windowWidth * 0.5f - splitThickness * 0.5f,
                  windowBottomY + frameThickness,
                  splitThickness,
                  windowHeight - 2.0f * frameThickness);
    drawRectangle(windowLeftX + frameThickness,
                  windowBottomY + windowHeight * 0.5f - splitThickness * 0.5f,
                  windowWidth - 2.0f * frameThickness,
                  splitThickness);
}

void drawHeadlightCone(float carPositionX, float carPositionY, bool coneEnabled) {
    if (!coneEnabled) {
        return;
    }

    const float coneStartX = carPositionX + 70.0f;   // Front bumper X where light starts
    const float coneStartY = carPositionY + 20.0f;   // Front bumper center Y
    const float coneReachX = carPositionX + 252.0f;  // Light reach distance on road

    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    // This triangle simulates light spreading forward.
    glColor4f(1.0f, 0.94f, 0.46f, 0.26f);
    glBegin(GL_TRIANGLES);
    glVertex2f(coneStartX, coneStartY);                  // Light origin at front bumper
    glVertex2f(coneReachX, carPositionY + 58.0f);        // Upper spread point
    glVertex2f(coneReachX, carPositionY - 14.0f);        // Lower spread point
    glEnd();

    // Inner brighter core for stronger center beam.
    glColor4f(1.0f, 0.98f, 0.70f, 0.34f);
    glBegin(GL_TRIANGLES);
    glVertex2f(coneStartX + 4.0f, coneStartY);           // Inner light origin
    glVertex2f(coneReachX - 36.0f, carPositionY + 42.0f); // Inner upper spread point
    glVertex2f(coneReachX - 36.0f, carPositionY + 0.0f);  // Inner lower spread point
    glEnd();

    glDisable(GL_BLEND);
}

void drawHouse(bool isLit) {
    const float houseBaseX = homeHouseLeftX + 14.0f;          // Left edge of main 2-story section
    const float houseBaseY = homeHouseBottomY;                // Ground contact Y for main section
    const float mainSectionWidth = homeHouseWidth - 120.0f;   // Main 2-story section width
    const float mainSectionHeight = homeHouseHeight + 20.0f;  // Main 2-story section height

    const float garageStartX = homeGarageLeftX;         // Garage wing left edge
    const float garageBaseY = homeGarageBottomY;        // Garage wing bottom edge
    const float garageWingWidth = homeGarageWidth;      // Garage wing width
    const float garageWingHeight = homeGarageHeight;    // Garage wing one-story height

    const float firstFloorHeight = 126.0f; // Main section first-floor height
    const float roofOverhang = 16.0f;      // Roof extends beyond wall for realism

    const float mainRoofBaseY = houseBaseY + mainSectionHeight + 8.0f; // Main roof base Y
    const float mainRoofPeakX = houseBaseX + mainSectionWidth * 0.58f; // Slightly shifted peak for asymmetry
    const float mainRoofPeakY = mainRoofBaseY + 122.0f;                // Main roof top peak Y

    const float garageRoofBaseY = garageBaseY + garageWingHeight + 6.0f; // Garage roof base Y
    const float garageRoofPeakX = garageStartX + garageWingWidth * 0.47f; // Garage roof peak X
    const float garageRoofPeakY = garageRoofBaseY + 68.0f;                // Garage roof peak Y

    const bool isMorningScene = (currentScene == 1); // Scene 1 uses blue glass, Scene 9 uses OFF/ON logic

    // Color variation gives illusion of different materials.
    glColor3f(0.85f, 0.79f, 0.69f);
    drawRectangle(houseBaseX, houseBaseY, mainSectionWidth, firstFloorHeight);

    glColor3f(0.80f, 0.74f, 0.65f);
    drawRectangle(houseBaseX, houseBaseY + firstFloorHeight, mainSectionWidth, mainSectionHeight - firstFloorHeight);

    // Floor separator strip makes two-story section visually clear.
    glColor3f(0.72f, 0.66f, 0.58f);
    drawRectangle(houseBaseX, houseBaseY + firstFloorHeight - 4.0f, mainSectionWidth, 8.0f);

    // One-story garage wing attached to the right side.
    glColor3f(0.77f, 0.72f, 0.64f);
    drawRectangle(garageStartX, garageBaseY, garageWingWidth, garageWingHeight);

    // Main roof with two gray shades.
    // Different colors simulate roof material.
    glColor3f(0.24f, 0.25f, 0.28f);
    glBegin(GL_TRIANGLES);
    glVertex2f(houseBaseX - roofOverhang, mainRoofBaseY);                // Main roof left overhang point
    glVertex2f(houseBaseX + mainSectionWidth + roofOverhang, mainRoofBaseY); // Main roof right overhang point
    glVertex2f(mainRoofPeakX, mainRoofPeakY);                            // Main roof peak point
    glEnd();

    glColor3f(0.34f, 0.35f, 0.38f);
    glBegin(GL_TRIANGLES);
    glVertex2f(mainRoofPeakX, mainRoofPeakY);                            // Shingle shade top point
    glVertex2f(houseBaseX + mainSectionWidth + roofOverhang, mainRoofBaseY); // Shingle shade right point
    glVertex2f(houseBaseX + mainSectionWidth * 0.62f, mainRoofBaseY);   // Shingle shade bottom point
    glEnd();

    // Garage roof with matching material style.
    // Different colors simulate roof material.
    glColor3f(0.22f, 0.23f, 0.26f);
    glBegin(GL_TRIANGLES);
    glVertex2f(garageStartX - roofOverhang, garageRoofBaseY);                // Garage roof left overhang point
    glVertex2f(garageStartX + garageWingWidth + roofOverhang, garageRoofBaseY); // Garage roof right overhang point
    glVertex2f(garageRoofPeakX, garageRoofPeakY);                              // Garage roof peak point
    glEnd();

    glColor3f(0.31f, 0.32f, 0.35f);
    glBegin(GL_TRIANGLES);
    glVertex2f(garageRoofPeakX, garageRoofPeakY);                              // Garage shade top point
    glVertex2f(garageStartX + garageWingWidth + roofOverhang, garageRoofBaseY); // Garage shade right point
    glVertex2f(garageStartX + garageWingWidth * 0.56f, garageRoofBaseY);       // Garage shade bottom point
    glEnd();

    // Chimney on main roof.
    glColor3f(0.56f, 0.36f, 0.32f);
    drawRectangle(houseBaseX + mainSectionWidth - 84.0f, mainRoofBaseY + 34.0f, 24.0f, 68.0f);

    // Front entrance with porch canopy.
    // This creates a realistic entrance instead of flat wall.
    const float doorWidth = 56.0f;                    // Main front door width
    const float doorHeight = 120.0f;                  // Main front door height
    const float doorLeftX = houseBaseX + 94.0f;       // Front door left edge
    const float doorBottomY = houseBaseY;             // Front door bottom edge
    const float porchRoofBaseY = doorBottomY + doorHeight + 12.0f; // Porch roof base Y

    glColor3f(0.62f, 0.60f, 0.56f);
    drawRectangle(doorLeftX - 18.0f, houseBaseY - 10.0f, doorWidth + 36.0f, 10.0f); // Upper step
    drawRectangle(doorLeftX - 26.0f, houseBaseY - 18.0f, doorWidth + 52.0f, 8.0f);  // Lower step

    glColor3f(0.68f, 0.62f, 0.54f);
    drawRectangle(doorLeftX - 20.0f, doorBottomY + doorHeight, doorWidth + 40.0f, 12.0f); // Porch canopy base

    glColor3f(0.28f, 0.29f, 0.32f);
    glBegin(GL_TRIANGLES);
    glVertex2f(doorLeftX - 28.0f, porchRoofBaseY);          // Porch roof left point
    glVertex2f(doorLeftX + doorWidth + 28.0f, porchRoofBaseY); // Porch roof right point
    glVertex2f(doorLeftX + doorWidth * 0.5f, porchRoofBaseY + 34.0f); // Porch roof peak point
    glEnd();

    // Door pillars under porch canopy.
    glColor3f(0.74f, 0.69f, 0.60f);
    drawRectangle(doorLeftX - 18.0f, doorBottomY, 8.0f, doorHeight);                 // Left porch pillar
    drawRectangle(doorLeftX + doorWidth + 10.0f, doorBottomY, 8.0f, doorHeight);     // Right porch pillar

    // Main front door.
    glColor3f(0.48f, 0.31f, 0.17f);
    drawRectangle(doorLeftX, doorBottomY, doorWidth, doorHeight);

    glColor3f(0.34f, 0.22f, 0.12f);
    drawRectangle(doorLeftX + 6.0f, doorBottomY + 12.0f, doorWidth - 12.0f, 42.0f);  // Lower door panel
    drawRectangle(doorLeftX + 6.0f, doorBottomY + 66.0f, doorWidth - 12.0f, 42.0f);  // Upper door panel

    glColor3f(0.92f, 0.76f, 0.26f);
    drawFilledCircle(doorLeftX + doorWidth - 10.0f, doorBottomY + 58.0f, 3.2f, 18);  // Door handle

    // Window layout for main house and garage.
    const float windowWidth = 56.0f;   // Main window width
    const float windowHeight = 54.0f;  // Main window height

    // Scene 9 warm welcome effect: selected windows turn ON after parking.
    const bool litUpperMiddle = isLit;
    const bool litUpperRight = isLit;
    const bool litLowerRight = isLit;

    drawHouseWindow(houseBaseX + 18.0f,  houseBaseY + 58.0f, windowWidth, windowHeight, isMorningScene, false);        // Lower-left window
    drawHouseWindow(houseBaseX + 168.0f, houseBaseY + 58.0f, windowWidth, windowHeight, isMorningScene, litLowerRight); // Lower-right window

    drawHouseWindow(houseBaseX + 18.0f,  houseBaseY + 166.0f, windowWidth, windowHeight, isMorningScene, false);         // Upper-left window
    drawHouseWindow(houseBaseX + 122.0f, houseBaseY + 166.0f, windowWidth, windowHeight, isMorningScene, litUpperMiddle); // Upper-middle window
    drawHouseWindow(houseBaseX + 226.0f, houseBaseY + 166.0f, windowWidth, windowHeight, isMorningScene, litUpperRight);  // Upper-right window

    // Garage side window.
    drawHouseWindow(garageStartX + 12.0f, garageBaseY + 98.0f, 42.0f, 38.0f, isMorningScene, isLit); // Small garage window

    // Garage door opening and panel details.
    glColor3f(0.08f, 0.09f, 0.11f);
    drawRectangle(homeGarageDoorLeftX, homeGarageDoorBottomY, homeGarageDoorWidth, homeGarageDoorHeight); // Dark interior opening

    const int garagePanelCount = 6; // Number of horizontal garage door panel lines
    const float panelStepY = homeGarageDoorHeight / static_cast<float>(garagePanelCount);

    glColor3f(0.46f, 0.48f, 0.52f);
    for (int panelIndex = 1; panelIndex < garagePanelCount; ++panelIndex) {
        const float panelLineY = homeGarageDoorBottomY + panelStepY * static_cast<float>(panelIndex);
        drawRectangle(homeGarageDoorLeftX + 8.0f, panelLineY, homeGarageDoorWidth - 16.0f, 2.0f); // Garage panel separator line
    }

    // Garage door side rails and driveway threshold.
    glColor3f(0.54f, 0.56f, 0.60f);
    drawRectangle(homeGarageDoorLeftX, homeGarageDoorBottomY, 4.0f, homeGarageDoorHeight); // Left rail
    drawRectangle(homeGarageDoorLeftX + homeGarageDoorWidth - 4.0f,
                  homeGarageDoorBottomY,
                  4.0f,
                  homeGarageDoorHeight); // Right rail

    drawRectangle(homeDrivewayGarageLeftX,
                  homeDrivewayGarageY,
                  homeDrivewayGarageRightX - homeDrivewayGarageLeftX,
                  6.0f); // Driveway top threshold aligned with garage
}

// ==========================================================
// ====== DEVELOPER 2: TRAFFIC MODULE (SCENE 2 & 8) =========
// ==========================================================

// -----------------------------------------
// SCENE 2: MORNING TRAFFIC
// -----------------------------------------
void drawScene2MorningTraffic() {
    if (!isRainEnabled) {
        // Morning to afternoon bright blue sky.
        drawVerticalSkyGradient(
            0.44f, 0.74f, 0.95f,
            0.68f, 0.89f, 0.99f
        );

        drawSun(980.0f + sunHorizontalOffset * 0.35f, 610.0f);

        // Clouds move slowly from left to right.
        drawCloud(220.0f + cloudOffsetX_layerA, 610.0f);
        drawCloud(560.0f + cloudOffsetX_layerB, 640.0f);
        drawCloud(920.0f + cloudOffsetX_layerA * 0.8f, 600.0f);
    } else {
        // Rain mode sky for Scene 2 (no sun while raining).
        drawVerticalSkyGradient(
            0.26f, 0.28f, 0.32f,
            0.36f, 0.38f, 0.42f
        );

        // Dark clouds while raining.
        glColor3f(0.28f, 0.30f, 0.34f);
        drawFilledEllipse(240.0f + cloudOffsetX_layerA * 0.32f, 620.0f, 170.0f, 45.0f, 36);
        drawFilledEllipse(640.0f + cloudOffsetX_layerB * 0.30f, 646.0f, 190.0f, 50.0f, 36);
        drawFilledEllipse(1000.0f + cloudOffsetX_layerA * 0.26f, 612.0f, 180.0f, 46.0f, 36);
    }

    // Parallax city: far buildings move slower than traffic cars.
    drawParallaxCity(parallaxOffset_scene2, false);

    // Configure road for this scene.
    roadBottomY_current = 120.0f; // Road starts above grass strip
    roadHeight_current = 220.0f;  // Tall multi-lane traffic road
    roadLaneCount_current = 4;    // Four lanes for busy traffic
    roadNightMode_current = false;
    drawRoad();

    // Foreground grass strip.
    glColor3f(0.24f, 0.52f, 0.26f);
    drawRectangle(0.0f, 0.0f, 1280.0f, 120.0f);

    // Cars at different speeds and lane directions.
    drawCar(trafficCarPositionX_scene2_A, 168.0f, 0.88f, 0.22f, 0.18f,
            trafficCarWheelAngle_scene2_A, false, false, true);

    drawCar(trafficCarPositionX_scene2_B, 230.0f, 0.20f, 0.50f, 0.86f,
            trafficCarWheelAngle_scene2_B, false, false, false);

    drawCar(trafficCarPositionX_scene2_C, 292.0f, 0.18f, 0.72f, 0.38f,
            trafficCarWheelAngle_scene2_C, false, false, true);
}

// -----------------------------------------
// SCENE 8: EVENING TRAFFIC
// -----------------------------------------
void drawScene8EveningTraffic() {
    // Dark evening sky.
    drawVerticalSkyGradient(
        0.06f, 0.08f, 0.16f, // Bottom night blue
        0.12f, 0.16f, 0.28f  // Top night blue
    );

    drawMoon(1060.0f, 618.0f);
    drawStars();

    // Night skyline with lit windows.
    drawParallaxCity(parallaxOffset_scene8, true);

    // Night road configuration.
    roadBottomY_current = 110.0f;
    roadHeight_current = 230.0f;
    roadLaneCount_current = 4;
    roadNightMode_current = true;
    drawRoad();

    // Grass foreground in night tone.
    glColor3f(0.10f, 0.20f, 0.12f);
    drawRectangle(0.0f, 0.0f, 1280.0f, 110.0f);

    // Street lights on both sides with glow.
    for (float poleX = 70.0f; poleX <= 1230.0f; poleX += 170.0f) {
        drawStreetLight(poleX, 340.0f, true);
    }

    // Night traffic with headlights and taillights.
    drawCar(trafficCarPositionX_scene8_A, 160.0f, 0.80f, 0.26f, 0.20f,
            trafficCarWheelAngle_scene8_A, true, true, true);

    drawCar(trafficCarPositionX_scene8_B, 230.0f, 0.20f, 0.52f, 0.84f,
            trafficCarWheelAngle_scene8_B, true, true, false);

    drawCar(trafficCarPositionX_scene8_C, 300.0f, 0.30f, 0.72f, 0.44f,
            trafficCarWheelAngle_scene8_C, true, true, true);
}

// ==========================================================
// ====== DEVELOPER 3: OFFICE EXTERIOR MODULE (3 & 7) =======
// ==========================================================

void drawOfficeComplex(bool eveningLightingEnabled) {
    const float officeLeftX = 690.0f;    // Office building left edge
    const float officeBottomY = 190.0f;  // Office building ground Y
    const float officeWidth = 470.0f;    // Office width
    const float officeHeight = 360.0f;   // Office height

    if (eveningLightingEnabled) {
        glColor3f(0.22f, 0.24f, 0.32f);
    } else {
        glColor3f(0.56f, 0.62f, 0.72f);
    }
    drawRectangle(officeLeftX, officeBottomY, officeWidth, officeHeight);

    // Glass front section.
    if (eveningLightingEnabled) {
        glColor3f(0.20f, 0.28f, 0.38f);
    } else {
        glColor3f(0.44f, 0.66f, 0.86f);
    }
    drawRectangle(officeLeftX + 36.0f, officeBottomY + 56.0f, officeWidth - 72.0f, officeHeight - 120.0f);

    // Window grid lines.
    glColor3f(0.30f, 0.34f, 0.40f);
    glLineWidth(1.0f);
    glBegin(GL_LINES);
    for (float verticalX = officeLeftX + 74.0f; verticalX <= officeLeftX + officeWidth - 74.0f; verticalX += 42.0f) {
        glVertex2f(verticalX, officeBottomY + 56.0f);
        glVertex2f(verticalX, officeBottomY + officeHeight - 64.0f);
    }
    for (float horizontalY = officeBottomY + 90.0f; horizontalY <= officeBottomY + officeHeight - 70.0f; horizontalY += 36.0f) {
        glVertex2f(officeLeftX + 36.0f, horizontalY);
        glVertex2f(officeLeftX + officeWidth - 36.0f, horizontalY);
    }
    glEnd();

    // Office entrance.
    glColor3f(0.18f, 0.20f, 0.24f);
    drawRectangle(officeLeftX + 192.0f, officeBottomY, 86.0f, 132.0f);

    if (eveningLightingEnabled) {
        glColor3f(0.98f, 0.90f, 0.56f);
    } else {
        glColor3f(0.82f, 0.90f, 0.96f);
    }
    drawRectangle(officeLeftX + 204.0f, officeBottomY + 14.0f, 28.0f, 106.0f);
    drawRectangle(officeLeftX + 238.0f, officeBottomY + 14.0f, 28.0f, 106.0f);

    // Company sign board.
    glColor3f(0.14f, 0.16f, 0.26f);
    drawRectangle(officeLeftX + 136.0f, officeBottomY + officeHeight + 12.0f, 198.0f, 34.0f);

    glColor3f(1.0f, 1.0f, 1.0f);
    drawBitmapText(officeLeftX + 150.0f, officeBottomY + officeHeight + 34.0f, "URBAN TECH OFFICE");

    // Parking lot surface.
    if (eveningLightingEnabled) {
        glColor3f(0.16f, 0.16f, 0.18f);
    } else {
        glColor3f(0.34f, 0.34f, 0.37f);
    }
    drawRectangle(420.0f, 120.0f, 860.0f, 70.0f);
}

void drawOfficeRampDown() {
    // Downward ramp used during office arrival.
    glColor3f(0.40f, 0.40f, 0.43f);
    glBegin(GL_POLYGON);
    glVertex2f(480.0f, 290.0f); // Ramp top-left on main road
    glVertex2f(760.0f, 290.0f); // Ramp top-right on main road
    glVertex2f(860.0f, 170.0f); // Ramp bottom-right in parking area
    glVertex2f(580.0f, 170.0f); // Ramp bottom-left in parking area
    glEnd();
}

void drawOfficeRampUp() {
    // Upward ramp used during leaving office scene.
    glColor3f(0.40f, 0.40f, 0.43f);
    glBegin(GL_POLYGON);
    glVertex2f(580.0f, 170.0f); // Ramp bottom-left near parking lot
    glVertex2f(860.0f, 170.0f); // Ramp bottom-right near parking lot
    glVertex2f(760.0f, 290.0f); // Ramp top-right near road
    glVertex2f(480.0f, 290.0f); // Ramp top-left near road
    glEnd();
}

void drawParkingBarrier(float pivotX, float pivotY, float rotationAngleDegrees) {
    // Barrier stand.
    glColor3f(0.30f, 0.30f, 0.34f);
    drawRectangle(pivotX - 14.0f, pivotY - 58.0f, 28.0f, 58.0f);

    glPushMatrix();
    glTranslatef(pivotX, pivotY, 0.0f);               // Pivot where barrier rotates
    glRotatef(rotationAngleDegrees, 0.0f, 0.0f, 1.0f);

    // Barrier arm extends left when closed.
    glColor3f(0.90f, 0.90f, 0.92f);
    drawRectangle(-170.0f, -6.0f, 170.0f, 12.0f);

    // Red safety strips.
    glColor3f(0.88f, 0.14f, 0.14f);
    drawRectangle(-154.0f, -6.0f, 18.0f, 12.0f);
    drawRectangle(-116.0f, -6.0f, 18.0f, 12.0f);
    drawRectangle( -78.0f, -6.0f, 18.0f, 12.0f);
    drawRectangle( -40.0f, -6.0f, 18.0f, 12.0f);

    glPopMatrix();
}

// -----------------------------------------
// SCENE 3: OFFICE ARRIVAL
// -----------------------------------------
void drawScene3OfficeArrival() {
    if (!isRainEnabled) {
        // Afternoon sky.
        drawVerticalSkyGradient(
            0.48f, 0.72f, 0.92f,
            0.70f, 0.86f, 0.98f
        );

        drawSun(1090.0f + sunHorizontalOffset * 0.22f, 610.0f);

        // Clouds are visible in early daytime scenes (1 to 3).
        drawCloud(230.0f + cloudOffsetX_layerA * 0.7f, 598.0f);
        drawCloud(640.0f + cloudOffsetX_layerB * 0.7f, 632.0f);
    } else {
        // Rain mode sky for Scene 3 (no sun while raining).
        drawVerticalSkyGradient(
            0.24f, 0.27f, 0.31f,
            0.34f, 0.37f, 0.41f
        );

        // Dark clouds while raining.
        glColor3f(0.26f, 0.29f, 0.33f);
        drawFilledEllipse(250.0f + cloudOffsetX_layerA * 0.28f, 612.0f, 180.0f, 46.0f, 36);
        drawFilledEllipse(700.0f + cloudOffsetX_layerB * 0.30f, 640.0f, 200.0f, 50.0f, 36);
        drawFilledEllipse(1040.0f + cloudOffsetX_layerA * 0.24f, 608.0f, 170.0f, 44.0f, 36);
    }

    // Distant city behind office.
    drawParallaxCity(parallaxOffset_scene2 * 0.4f, false);

    // Main road before ramp.
    roadBottomY_current = 250.0f;
    roadHeight_current = 90.0f;
    roadLaneCount_current = 2;
    roadNightMode_current = false;
    drawRoad();

    drawOfficeRampDown();
    drawOfficeComplex(false);

    // Barrier opens as car approaches.
    drawParkingBarrier(860.0f, 170.0f, barrierRotationAngle_scene3);

    // Arrival car moving down the ramp.
    drawCar(carPositionX_scene3, carPositionY_scene3, 0.86f, 0.24f, 0.20f,
            wheelRotationAngle_scene3, false, false, true);
}

// -----------------------------------------
// SCENE 7: LEAVING OFFICE
// -----------------------------------------
void drawScene7LeavingOffice() {
    // Evening sky with warm tone.
    drawVerticalSkyGradient(
        0.34f, 0.24f, 0.32f, // Bottom evening color
        0.92f, 0.60f, 0.34f  // Top sunset color
    );

    drawSun(180.0f + sunHorizontalOffset * 0.25f, 580.0f);

    drawOfficeComplex(true);
    drawOfficeRampUp();

    // Main road after ramp.
    roadBottomY_current = 250.0f;
    roadHeight_current = 90.0f;
    roadLaneCount_current = 2;
    roadNightMode_current = false;
    drawRoad();

    // Barrier opens for exit.
    drawParkingBarrier(860.0f, 170.0f, barrierRotationAngle_scene7);

    // Leaving car with headlights on due evening light.
    drawCar(carPositionX_scene7, carPositionY_scene7, 0.86f, 0.24f, 0.20f,
            wheelRotationAngle_scene7, true, false, true);
}

// ==========================================================
// ====== DEVELOPER 4: OFFICE INTERIOR MODULE (4,5,6) =======
// ==========================================================

// -----------------------------------------
// SCENE 4: MAIN OFFICE
// -----------------------------------------
void drawScene4MainOffice() {
    // Indoor office background.
    drawVerticalSkyGradient(
        0.78f, 0.80f, 0.86f, // Floor-side tint
        0.94f, 0.96f, 0.99f  // Ceiling-side tint
    );

    // Back wall and floor.
    glColor3f(0.92f, 0.94f, 0.97f);
    drawRectangle(0.0f, 180.0f, 1280.0f, 540.0f);
    glColor3f(0.68f, 0.66f, 0.62f);
    drawRectangle(0.0f, 0.0f, 1280.0f, 180.0f);

    // Office window strip.
    glColor3f(0.60f, 0.78f, 0.90f);
    drawRectangle(70.0f, 420.0f, 440.0f, 210.0f);

    glColor3f(0.36f, 0.42f, 0.48f);
    drawRectangle(282.0f, 420.0f, 6.0f, 210.0f); // Window vertical frame
    drawRectangle(70.0f, 520.0f, 440.0f, 6.0f);  // Window horizontal frame

    // Ceiling fan.
    drawFan(740.0f, 700.0f);

    // Wall clock with moving hand.
    drawWallClock(1140.0f, 610.0f, wallClockHandAngle_scene4);

    // Main desk and chair.
    drawOfficeDeskSetup(380.0f, 160.0f);

    // Main office worker sitting and typing.
    characterFacingRight = true;
    characterSittingPose = true;
    characterCarryBriefcase = false;
    characterPointingPose = false;

    characterLeftArmAngleDegrees = -32.0f - typingArmAngle_scene4 * 0.4f;
    characterRightArmAngleDegrees = -20.0f + typingArmAngle_scene4;
    characterLegSwingAngleDegrees = 0.0f;

    drawCharacter(438.0f, 170.0f);

    // Two background workers moving left-right.
    characterSittingPose = false;
    characterCarryBriefcase = false;
    characterPointingPose = false;

    characterLeftArmAngleDegrees = -8.0f;
    characterRightArmAngleDegrees = 12.0f;
    characterLegSwingAngleDegrees = 8.0f;
    drawCharacter(workerPositionX_scene4_A, 190.0f);

    characterFacingRight = false;
    characterLeftArmAngleDegrees = 10.0f;
    characterRightArmAngleDegrees = -12.0f;
    characterLegSwingAngleDegrees = -8.0f;
    drawCharacter(workerPositionX_scene4_B, 190.0f);

    // Reset pose defaults.
    characterFacingRight = true;
    characterSittingPose = false;
}

// -----------------------------------------
// SCENE 5: COFFEE BREAK
// -----------------------------------------
void drawScene5CoffeeBreak() {
    // Indoor break area background.
    drawVerticalSkyGradient(
        0.80f, 0.82f, 0.88f,
        0.95f, 0.96f, 0.98f
    );

    glColor3f(0.90f, 0.92f, 0.95f);
    drawRectangle(0.0f, 170.0f, 1280.0f, 550.0f); // Wall

    glColor3f(0.70f, 0.68f, 0.64f);
    drawRectangle(0.0f, 0.0f, 1280.0f, 170.0f); // Floor

    // Pantry counter.
    glColor3f(0.58f, 0.44f, 0.28f);
    drawRectangle(760.0f, 170.0f, 420.0f, 100.0f);

    // Coffee machine and steam.
    drawCoffeeMachine(950.0f, 220.0f);
    drawSteamParticles();

    // Worker walking toward coffee machine.
    characterFacingRight = true;
    characterSittingPose = false;
    characterCarryBriefcase = false;
    characterPointingPose = false;

    characterLegSwingAngleDegrees = workerWalkLegSwing_scene5;
    characterLeftArmAngleDegrees = -workerWalkLegSwing_scene5 * 0.8f;
    characterRightArmAngleDegrees = workerWalkLegSwing_scene5 * 0.8f;

    drawCharacter(workerWalkPositionX_scene5, 172.0f);
}

// -----------------------------------------
// SCENE 6: PRESENTATION
// -----------------------------------------
void drawScene6Presentation() {
    // Presentation room background.
    drawVerticalSkyGradient(
        0.74f, 0.78f, 0.84f,
        0.92f, 0.94f, 0.97f
    );

    glColor3f(0.90f, 0.93f, 0.96f);
    drawRectangle(0.0f, 150.0f, 1280.0f, 570.0f); // Wall

    glColor3f(0.62f, 0.60f, 0.56f);
    drawRectangle(0.0f, 0.0f, 1280.0f, 150.0f); // Floor

    // Presentation board with animated bar chart.
    drawPresentationBoard(650.0f, 220.0f, 500.0f, 360.0f);

    // Presenter character near board.
    characterFacingRight = true;
    characterSittingPose = false;
    characterCarryBriefcase = false;
    characterPointingPose = true;

    characterLegSwingAngleDegrees = 0.0f;
    characterLeftArmAngleDegrees = 5.0f;
    characterRightArmAngleDegrees = -48.0f;

    drawCharacter(540.0f, 150.0f);

    // Pointer line from hand to board.
    glColor3f(0.20f, 0.16f, 0.10f);
    glLineWidth(2.0f);
    glBegin(GL_LINES);
    glVertex2f(560.0f, 250.0f); // Pointer handle near character hand
    glVertex2f(760.0f, 370.0f); // Pointer tip on board chart
    glEnd();
    glLineWidth(1.0f);

    // Small audience silhouettes.
    characterPointingPose = false;
    characterFacingRight = true;
    characterLeftArmAngleDegrees = 0.0f;
    characterRightArmAngleDegrees = 0.0f;
    characterLegSwingAngleDegrees = 0.0f;

    drawCharacter(220.0f, 150.0f);
    drawCharacter(300.0f, 150.0f);
    drawCharacter(380.0f, 150.0f);
}

// ==========================================================
// ====== SCENE 1 and SCENE 9 RENDER FUNCTIONS ==============
// ==========================================================

// -----------------------------------------
// SCENE 1: MORNING HOME DEPARTURE
// -----------------------------------------
void drawScene1MorningHomeDeparture() {
    // Weather toggle:
    // - Rain OFF: old morning look with sun and clouds.
    // - Rain ON : dark sky with wet ground.
    if (!isRainEnabled) {
        drawMorningHomeSky();
        drawHomeGroundAndDriveway(false);

        // Keep subtle wet patches so morning departure still feels rainy and muted.
        glColor3f(0.72f, 0.74f, 0.78f);
        drawFilledEllipse(170.0f, 70.0f, 54.0f, 13.0f, 30);
        drawFilledEllipse(502.0f, 73.0f, 66.0f, 15.0f, 30);
        drawFilledEllipse(905.0f, 69.0f, 60.0f, 12.0f, 30);
    } else {
        // Overcast rainy sky.
        drawVerticalSkyGradient(
            0.24f, 0.26f, 0.29f,
            0.36f, 0.38f, 0.41f
        );

        // Dark cloud masses.
        glColor3f(0.28f, 0.30f, 0.32f);
        drawFilledEllipse(220.0f + cloudOffsetX_layerA * 0.30f, 632.0f, 170.0f, 46.0f, 40);
        drawFilledEllipse(620.0f + cloudOffsetX_layerB * 0.30f, 654.0f, 190.0f, 52.0f, 40);
        drawFilledEllipse(980.0f + cloudOffsetX_layerA * 0.24f, 620.0f, 180.0f, 48.0f, 40);

        // Bottom dark gray road.
        glColor3f(0.18f, 0.18f, 0.20f);
        drawRectangle(0.0f, homeRoadBottomY, 1280.0f, homeRoadHeight);

        // Road top edge line for clean separation.
        glColor3f(0.56f, 0.58f, 0.62f);
        drawRectangle(0.0f, homeRoadBottomY + homeRoadHeight - 3.0f, 1280.0f, 3.0f);

        // Muted green grass above road.
        glColor3f(0.30f, 0.42f, 0.34f);
        drawRectangle(0.0f, homeGrassBottomY, 1280.0f, homeGrassHeight);

        // Driveway polygon from garage to road.
        glColor3f(0.48f, 0.48f, 0.50f);
        glBegin(GL_POLYGON);
        glVertex2f(homeDrivewayRoadLeftX, homeDrivewayRoadY);
        glVertex2f(homeDrivewayRoadRightX, homeDrivewayRoadY);
        glVertex2f(homeDrivewayGarageRightX, homeDrivewayGarageY);
        glVertex2f(homeDrivewayGarageLeftX, homeDrivewayGarageY);
        glEnd();

        // Wet road patches (simple puddles).
        glColor3f(0.60f, 0.62f, 0.66f);
        drawFilledEllipse(180.0f, 72.0f, 58.0f, 14.0f, 32);
        drawFilledEllipse(540.0f, 74.0f, 70.0f, 15.0f, 32);
        drawFilledEllipse(930.0f, 70.0f, 62.0f, 13.0f, 32);
    }

    // Trees around the upgraded house.
    drawTree(88.0f, 120.0f);
    drawTree(640.0f, 132.0f);
    drawTree(748.0f, 124.0f);

    // House with dim rainy windows.
    drawHouse(false);

    // Departure car.
    drawCar(scene1_carPosX, scene1_carPosY, 0.84f, 0.20f, 0.18f,
            wheelRotationAngle, false, false, true);

    // Rain is rendered globally from display() so all scenes share the same weather.
}

// -----------------------------------------
// SCENE 9: RETURN HOME
// -----------------------------------------
void drawScene9ReturnHome() {
    // Dark blue night sky.
    drawVerticalSkyGradient(
        0.05f, 0.08f, 0.18f,
        0.11f, 0.16f, 0.28f
    );

    // Stars use GL_POINTS.
    drawStars();

    // Simple moon.
    drawMoon(1030.0f, 620.0f);

    // Reuse shared night ground so road-grass-driveway join is clean.
    drawHomeGroundAndDriveway(true);

    // Trees around the upgraded house.
    drawTree(88.0f, 120.0f);
    drawTree(640.0f, 132.0f);
    drawTree(748.0f, 124.0f);

    // House windows are dark first; turn bright when car fully parks.
    drawHouse(isHouseLightOn);

    // Headlights stay on while car is moving.
    const bool headlightsOn = (carState_scene9 != 2);

    // Return car with headlights and taillights.
    drawCar(scene9_carPosX, scene9_carPosY, 0.84f, 0.20f, 0.18f,
            wheelRotationAngle, headlightsOn, true, true);

    // Dedicated moving light cone in front of the car.
    drawHeadlightCone(scene9_carPosX, scene9_carPosY, headlightsOn);
}

// ==========================================================
// ====== SCENE UPDATE LOGIC (PER SCENE) ====================
// ==========================================================

void initializeSteamParticles() {
    const float steamNozzleCenterX = 992.0f; // Nozzle center X at coffee machine
    const float steamStartY = 292.0f;        // Steam starts just above cup

    for (int index = 0; index < STEAM_PARTICLE_COUNT; ++index) {
        const int repeatingColumn = index % 5;

        coffeeSteamParticles[index].particleX = steamNozzleCenterX + (repeatingColumn - 2) * 6.0f;
        coffeeSteamParticles[index].particleY = steamStartY + static_cast<float>(index) * 6.0f;
        coffeeSteamParticles[index].verticalSpeed = 0.55f + static_cast<float>(index % 4) * 0.16f;
        coffeeSteamParticles[index].alphaValue = 0.40f + static_cast<float>(index % 6) * 0.08f;
    }
}

void updateScene1Animation() {
    // Scene 1 state 0: car starts inside garage and waits one frame.
    if (carState_scene1 == 0) {
        carState_scene1 = 1;
    }

    // Scene 1 state 1: car moves forward out of garage (down the driveway).
    if (carState_scene1 == 1) {
        const float forwardStepY = 1.6f * animationSpeed;
        scene1_carPosY -= forwardStepY;
        wheelRotationAngle -= forwardStepY * 2.0f;

        if (scene1_carPosY <= scene1RoadTravelY) {
            scene1_carPosY = scene1RoadTravelY;
            carState_scene1 = 2;
        }
    }

    // Scene 1 state 2: car moves horizontally right.
    if (carState_scene1 == 2) {
        const float rightStepX = 2.0f * animationSpeed;
        scene1_carPosX += rightStepX;
        wheelRotationAngle -= rightStepX * 2.0f;

        // Scene can transition only after car fully passes right boundary.
        if (scene1_carPosX > scene1ExitCheckX) {
            scene1HasCarExitedScreen = true;
        }
    }
}

void updateScene2Animation() {
    const float speedCarA = 3.2f; // Car A rightward speed
    const float speedCarB = 2.4f; // Car B leftward speed
    const float speedCarC = 4.1f; // Car C rightward speed

    trafficCarPositionX_scene2_A += speedCarA;
    trafficCarPositionX_scene2_B -= speedCarB;
    trafficCarPositionX_scene2_C += speedCarC;

    if (trafficCarPositionX_scene2_A > 1440.0f) {
        trafficCarPositionX_scene2_A = -220.0f;
    }
    if (trafficCarPositionX_scene2_B < -220.0f) {
        trafficCarPositionX_scene2_B = 1440.0f;
    }
    if (trafficCarPositionX_scene2_C > 1500.0f) {
        trafficCarPositionX_scene2_C = -360.0f;
    }

    trafficCarWheelAngle_scene2_A -= speedCarA * 4.0f;
    trafficCarWheelAngle_scene2_B -= speedCarB * 4.0f;
    trafficCarWheelAngle_scene2_C -= speedCarC * 4.0f;

    // Parallax offset update: far city motion slower than car speed.
    parallaxOffset_scene2 -= 0.8f;
    if (parallaxOffset_scene2 < -220.0f) {
        parallaxOffset_scene2 += 220.0f;
    }
}

void updateScene3Animation() {
    const float carSpeedPerFrame = 2.8f;
    const float rampStartX = 520.0f; // Car starts descending after this X
    const float rampSlope = 0.38f;   // Y drop per X unit on downward ramp

    if (carPositionX_scene3 < 930.0f) {
        carPositionX_scene3 += carSpeedPerFrame;
        wheelRotationAngle_scene3 -= carSpeedPerFrame * 4.1f;
    }

    if (carPositionX_scene3 < rampStartX) {
        carPositionY_scene3 = 286.0f; // Before ramp: stay on upper road level
    } else {
        const float descentAmount = (carPositionX_scene3 - rampStartX) * rampSlope;
        carPositionY_scene3 = 286.0f - descentAmount;
        carPositionY_scene3 = clampFloat(carPositionY_scene3, 170.0f, 286.0f);
    }

    // Open barrier when car gets close.
    if (carPositionX_scene3 > 700.0f) {
        barrierRotationAngle_scene3 += 2.4f;
        barrierRotationAngle_scene3 = clampFloat(barrierRotationAngle_scene3, 0.0f, 85.0f);
    }
}

void updateScene4Animation() {
    // Typing animation by oscillating arm angle.
    typingArmAngle_scene4 = std::sin(static_cast<float>(sceneFrameCounter) * 0.32f) * 16.0f;

    // Rotating fan.
    fanRotationAngle_scene4 += 14.0f;
    if (fanRotationAngle_scene4 > 360.0f) {
        fanRotationAngle_scene4 -= 360.0f;
    }

    // Clock minute hand rotates continuously.
    wallClockHandAngle_scene4 -= 1.6f;
    if (wallClockHandAngle_scene4 < -360.0f) {
        wallClockHandAngle_scene4 += 360.0f;
    }

    // Workers pacing left-right.
    workerPositionX_scene4_A += workerDirection_scene4_A * 1.2f;
    if (workerPositionX_scene4_A > 940.0f || workerPositionX_scene4_A < 760.0f) {
        workerDirection_scene4_A *= -1.0f;
    }

    workerPositionX_scene4_B += workerDirection_scene4_B * 1.0f;
    if (workerPositionX_scene4_B > 1080.0f || workerPositionX_scene4_B < 860.0f) {
        workerDirection_scene4_B *= -1.0f;
    }
}

void updateScene5Animation() {
    // Worker walks to coffee machine then slows near it.
    if (workerWalkPositionX_scene5 < 830.0f) {
        workerWalkPositionX_scene5 += 1.6f;
        workerWalkLegSwing_scene5 = std::sin(static_cast<float>(sceneFrameCounter) * 0.32f) * 18.0f;
    } else {
        workerWalkPositionX_scene5 = 830.0f;
        workerWalkLegSwing_scene5 = 2.0f * std::sin(static_cast<float>(sceneFrameCounter) * 0.12f);
    }

    // Steam particles: move up and fade out, then reset.
    const float steamNozzleCenterX = 992.0f;
    const float steamResetY = 292.0f;

    for (int particleIndex = 0; particleIndex < STEAM_PARTICLE_COUNT; ++particleIndex) {
        SteamParticle& particle = coffeeSteamParticles[particleIndex];

        particle.particleY += particle.verticalSpeed;
        particle.particleX += std::sin(starTwinkleCounter * 0.5f + particleIndex) * 0.18f;
        particle.alphaValue -= 0.006f;

        if (particle.particleY > 430.0f || particle.alphaValue <= 0.02f) {
            const int repeatingColumn = particleIndex % 5;
            particle.particleX = steamNozzleCenterX + (repeatingColumn - 2) * 6.0f;
            particle.particleY = steamResetY + static_cast<float>(particleIndex % 6) * 4.0f;
            particle.alphaValue = 0.82f;
        }
    }
}

void updateScene6Animation() {
    // Grow bar chart gradually.
    presentationBarGrowRatio_scene6 += 0.007f;
    presentationBarGrowRatio_scene6 = clampFloat(presentationBarGrowRatio_scene6, 0.0f, 1.0f);

    // Small pointer oscillation keeps presenter dynamic.
    pointerOscillationAngle_scene6 = std::sin(static_cast<float>(sceneFrameCounter) * 0.15f) * 6.0f;
}

void updateScene7Animation() {
    const float carSpeedPerFrame = 2.9f;
    const float rampStartX = 580.0f; // Start moving up ramp after this X
    const float rampSlope = 0.34f;   // Y rise per X unit on upward ramp

    if (carPositionX_scene7 < 1320.0f) {
        carPositionX_scene7 += carSpeedPerFrame;
        wheelRotationAngle_scene7 -= carSpeedPerFrame * 4.1f;
    }

    if (carPositionX_scene7 < rampStartX) {
        carPositionY_scene7 = 170.0f; // Parking level before ramp
    } else {
        const float riseAmount = (carPositionX_scene7 - rampStartX) * rampSlope;
        carPositionY_scene7 = 170.0f + riseAmount;
        carPositionY_scene7 = clampFloat(carPositionY_scene7, 170.0f, 286.0f);
    }

    // Open barrier while car exits.
    if (carPositionX_scene7 > 700.0f) {
        barrierRotationAngle_scene7 += 2.6f;
        barrierRotationAngle_scene7 = clampFloat(barrierRotationAngle_scene7, 0.0f, 85.0f);
    }
}

void updateScene8Animation() {
    const float speedCarA = 3.6f;
    const float speedCarB = 2.7f;
    const float speedCarC = 4.3f;

    trafficCarPositionX_scene8_A += speedCarA;
    trafficCarPositionX_scene8_B -= speedCarB;
    trafficCarPositionX_scene8_C += speedCarC;

    if (trafficCarPositionX_scene8_A > 1460.0f) {
        trafficCarPositionX_scene8_A = -260.0f;
    }
    if (trafficCarPositionX_scene8_B < -260.0f) {
        trafficCarPositionX_scene8_B = 1460.0f;
    }
    if (trafficCarPositionX_scene8_C > 1540.0f) {
        trafficCarPositionX_scene8_C = -520.0f;
    }

    trafficCarWheelAngle_scene8_A -= speedCarA * 4.0f;
    trafficCarWheelAngle_scene8_B -= speedCarB * 4.0f;
    trafficCarWheelAngle_scene8_C -= speedCarC * 4.0f;

    parallaxOffset_scene8 -= 0.65f;
    if (parallaxOffset_scene8 < -220.0f) {
        parallaxOffset_scene8 += 220.0f;
    }
}

void updateScene9Animation() {
    // Scene 9 state 0: car enters from left and moves right.
    if (carState_scene9 == 0) {
        const float rightStepX = 1.8f * animationSpeed;
        scene9_carPosX += rightStepX;
        scene9_carPosY = scene9RoadTravelY;
        wheelRotationAngle -= rightStepX * 2.0f;

        if (scene9_carPosX >= scene9DrivewayEntryX) {
            scene9_carPosX = scene9DrivewayEntryX;
            carState_scene9 = 1;
        }
    }

    // Scene 9 state 1: car moves up driveway into garage.
    if (carState_scene9 == 1) {
        const float upStepY = 1.4f * animationSpeed;
        const float alignStepX = 0.7f * animationSpeed;

        if (scene9_carPosX < scene9GarageParkingX) {
            scene9_carPosX += alignStepX;
        }
        if (scene9_carPosY < scene9GarageParkingY) {
            scene9_carPosY += upStepY;
        }

        wheelRotationAngle -= (upStepY + alignStepX) * 1.8f;

        if (scene9_carPosX >= scene9GarageParkingX) {
            scene9_carPosX = scene9GarageParkingX;
        }
        if (scene9_carPosY >= scene9GarageParkingY) {
            scene9_carPosY = scene9GarageParkingY;
        }

        if (scene9_carPosX >= scene9GarageParkingX && scene9_carPosY >= scene9GarageParkingY) {
            carState_scene9 = 2;
            scene9ParkingCompleted = true;
            // This simulates someone turning lights on when the person arrives home.
            isHouseLightOn = true;
        }
    }

    // Scene 9 state 2: parked hold.
    if (carState_scene9 == 2) {
        scene9ParkedFrameCounter += 1;
    }
}

// ==========================================================
// ====== SCENE RESET AND TRANSITION ========================
// ==========================================================

void resetVariablesForScene(int sceneIndex) {
    // Reset variables each time scene changes, to keep behavior deterministic.
    if (sceneIndex == 1) {
        scene1_carPosX = scene1GarageStartX;
        scene1_carPosY = scene1GarageStartY;
        wheelRotationAngle = 0.0f;
        carState_scene1 = 0;
        scene1HasCarExitedScreen = false;
        initRain();
    }

    if (sceneIndex == 2) {
        trafficCarPositionX_scene2_A = -180.0f;
        trafficCarPositionX_scene2_B = 1350.0f;
        trafficCarPositionX_scene2_C = -420.0f;

        trafficCarWheelAngle_scene2_A = 0.0f;
        trafficCarWheelAngle_scene2_B = 0.0f;
        trafficCarWheelAngle_scene2_C = 0.0f;

        parallaxOffset_scene2 = 0.0f;
    }

    if (sceneIndex == 3) {
        carPositionX_scene3 = -220.0f;
        carPositionY_scene3 = 286.0f;
        wheelRotationAngle_scene3 = 0.0f;
        barrierRotationAngle_scene3 = 0.0f;
    }

    if (sceneIndex == 4) {
        typingArmAngle_scene4 = 0.0f;
        fanRotationAngle_scene4 = 0.0f;
        wallClockHandAngle_scene4 = 90.0f;

        workerPositionX_scene4_A = 800.0f;
        workerPositionX_scene4_B = 980.0f;
        workerDirection_scene4_A = 1.0f;
        workerDirection_scene4_B = -1.0f;
    }

    if (sceneIndex == 5) {
        workerWalkPositionX_scene5 = 250.0f;
        workerWalkLegSwing_scene5 = 0.0f;
        initializeSteamParticles();
    }

    if (sceneIndex == 6) {
        presentationBarGrowRatio_scene6 = 0.0f;
        pointerOscillationAngle_scene6 = 0.0f;
    }

    if (sceneIndex == 7) {
        carPositionX_scene7 = 520.0f;
        carPositionY_scene7 = 170.0f;
        wheelRotationAngle_scene7 = 0.0f;
        barrierRotationAngle_scene7 = 0.0f;
    }

    if (sceneIndex == 8) {
        trafficCarPositionX_scene8_A = -260.0f;
        trafficCarPositionX_scene8_B = 1380.0f;
        trafficCarPositionX_scene8_C = -560.0f;

        trafficCarWheelAngle_scene8_A = 0.0f;
        trafficCarWheelAngle_scene8_B = 0.0f;
        trafficCarWheelAngle_scene8_C = 0.0f;

        parallaxOffset_scene8 = 0.0f;
    }

    if (sceneIndex == 9) {
        scene9_carPosX = -220.0f;
        scene9_carPosY = scene9RoadTravelY;
        wheelRotationAngle = 0.0f;
        carState_scene9 = 0;
        scene9ParkedFrameCounter = 0;
        scene9ParkingCompleted = false;
        isHouseLightOn = false;
    }
}

void moveToNextScene() {
    currentScene += 1;
    if (currentScene > LAST_SCENE_INDEX) {
        currentScene = FIRST_SCENE_INDEX; // Loop back to scene 1 after scene 9
    }

    sceneFrameCounter = 0;
    resetVariablesForScene(currentScene);
}

// ==========================================================
// ====== GLUT CALLBACKS ====================================
// ==========================================================

void reshape(int newWidth, int newHeight) {
    if (newHeight <= 0) {
        newHeight = 1;
    }

    const float currentAspect = static_cast<float>(newWidth) / static_cast<float>(newHeight);

    int viewportX = 0;
    int viewportY = 0;
    int viewportWidth = newWidth;
    int viewportHeight = newHeight;

    // Keep 16:9 using letterbox/pillarbox viewport.
    if (currentAspect > TARGET_ASPECT) {
        viewportWidth = static_cast<int>(static_cast<float>(newHeight) * TARGET_ASPECT);
        viewportX = (newWidth - viewportWidth) / 2;
    } else if (currentAspect < TARGET_ASPECT) {
        viewportHeight = static_cast<int>(static_cast<float>(newWidth) / TARGET_ASPECT);
        viewportY = (newHeight - viewportHeight) / 2;
    }

    glViewport(viewportX, viewportY, viewportWidth, viewportHeight);

    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    gluOrtho2D(ORTHO_LEFT_X, ORTHO_RIGHT_X, ORTHO_BOTTOM_Y, ORTHO_TOP_Y);

    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
}

void display() {
    // Time-of-day clear color system (required for realism and viva explanation).
    if (currentScene == 1 || currentScene == 2) {
        glClearColor(0.62f, 0.84f, 0.97f, 1.0f); // Morning light blue
    } else if (currentScene >= 3 && currentScene <= 6) {
        glClearColor(0.52f, 0.74f, 0.94f, 1.0f); // Afternoon bright blue
    } else if (currentScene == 7) {
        glClearColor(0.60f, 0.40f, 0.30f, 1.0f); // Evening orange-purple tone
    } else {
        glClearColor(0.05f, 0.07f, 0.15f, 1.0f); // Night dark blue
    }

    glClear(GL_COLOR_BUFFER_BIT);

    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();

    // Scene state machine.
    switch (currentScene) {
        case 1: drawScene1MorningHomeDeparture(); break;
        case 2: drawScene2MorningTraffic(); break;
        case 3: drawScene3OfficeArrival(); break;
        case 4: drawScene4MainOffice(); break;
        case 5: drawScene5CoffeeBreak(); break;
        case 6: drawScene6Presentation(); break;
        case 7: drawScene7LeavingOffice(); break;
        case 8: drawScene8EveningTraffic(); break;
        case 9: drawScene9ReturnHome(); break;
        default: drawScene1MorningHomeDeparture(); break;
    }

    // Global rain overlay:
    // If rain is enabled, rain appears only in outdoor scenes.
    const bool isOutdoorScene = (currentScene == 1 || currentScene == 2 || currentScene == 3 ||
                                 currentScene == 7 || currentScene == 8 || currentScene == 9);
    if (isRainEnabled && isOutdoorScene) {
        drawRain();
    }

    // Always show key hints on left side.
    drawControlHints();

    drawTextIfNeeded();

    // Smooth fade transition overlay.
    if (fadeOverlayAlpha > 0.001f) {
        // Alpha increases to create fade-out, then decreases for fade-in.
        glEnable(GL_BLEND);
        glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

        glColor4f(0.0f, 0.0f, 0.0f, fadeOverlayAlpha);
        drawRectangle(0.0f, 0.0f, 1280.0f, 720.0f);

        glDisable(GL_BLEND);
    }

    glutSwapBuffers();
}

void update(int value) {
    totalFrameCounter += 1;

    // Pause stops updates but keeps rendering the last frame.
    if (isPaused) {
        glutPostRedisplay();
        glutTimerFunc(TIMER_INTERVAL_MS, update, 0);
        (void)value;
        return;
    }

    // Shared environment updates.
    cloudOffsetX_layerA += 0.32f;
    cloudOffsetX_layerB += 0.22f;
    sunHorizontalOffset += 0.18f;
    starTwinkleCounter += 0.06f;

    cloudOffsetX_layerA = wrapOffsetToRange(cloudOffsetX_layerA, -260.0f, 1320.0f);
    cloudOffsetX_layerB = wrapOffsetToRange(cloudOffsetX_layerB, -360.0f, 1320.0f);

    if (sunHorizontalOffset > 220.0f) {
        sunHorizontalOffset = -220.0f;
    }

    // Global rain update so rain animation works in all scenes.
    if (isRainEnabled) {
        const float rainFallStep = 1.1f * animationSpeed;
        for (int rainIndex = 0; rainIndex < RAIN_DROP_COUNT; ++rainIndex) {
            rainDropY[rainIndex] -= rainFallStep;
            if (rainDropY[rainIndex] < -20.0f) {
                rainDropY[rainIndex] = ORTHO_TOP_Y + static_cast<float>(std::rand() % 120);
                rainDropX[rainIndex] = static_cast<float>(std::rand() % WINDOW_WIDTH);
            }
        }
    }

    // Update active scene animation.
    switch (currentScene) {
        case 1: updateScene1Animation(); break;
        case 2: updateScene2Animation(); break;
        case 3: updateScene3Animation(); break;
        case 4: updateScene4Animation(); break;
        case 5: updateScene5Animation(); break;
        case 6: updateScene6Animation(); break;
        case 7: updateScene7Animation(); break;
        case 8: updateScene8Animation(); break;
        case 9: updateScene9Animation(); break;
        default: break;
    }

    // Advance local frame counter while not transitioning.
    if (!isSceneTransitionActive) {
        sceneFrameCounter += 1;
    }

    // Trigger transition near scene end.
    if (!isSceneTransitionActive) {
        const int fadeTriggerFrame = sceneDurationFrameCount[currentScene] - FADE_START_BEFORE_END_FRAMES;
        bool sceneSpecificTransitionReady = true;

        // Scene 1 rule: transition only after car fully exits right side.
        if (currentScene == 1) {
            sceneSpecificTransitionReady = scene1HasCarExitedScreen;
        }

        // Scene 9 rule: transition only after car is parked in garage.
        if (currentScene == 9) {
            sceneSpecificTransitionReady = scene9ParkingCompleted && (scene9ParkedFrameCounter > 45);
        }

        if (sceneFrameCounter >= fadeTriggerFrame && sceneSpecificTransitionReady) {
            isSceneTransitionActive = true;
            isFadeOutPhase = true;
        }
    }

    // Fade engine.
    if (isSceneTransitionActive) {
        if (isFadeOutPhase) {
            fadeOverlayAlpha += FADE_ALPHA_STEP;
            if (fadeOverlayAlpha >= 1.0f) {
                fadeOverlayAlpha = 1.0f;
                moveToNextScene();
                isFadeOutPhase = false;
            }
        } else {
            fadeOverlayAlpha -= FADE_ALPHA_STEP;
            if (fadeOverlayAlpha <= 0.0f) {
                fadeOverlayAlpha = 0.0f;
                isSceneTransitionActive = false;
            }
        }
    }

    glutPostRedisplay();
    glutTimerFunc(TIMER_INTERVAL_MS, update, 0);

    (void)value; // Unused GLUT timer argument
}

void keyboard(unsigned char key, int x, int y) {
    // Press SPACE to pause/resume animation.
    if (key == ' ') {
        isPaused = !isPaused;
    }

    // Press R to enable rain.
    if (key == 'r' || key == 'R') {
        isRainEnabled = true;
    }

    // Press V to disable rain.
    if (key == 'v' || key == 'V') {
        isRainEnabled = false;
    }

    // Keep text overlay toggle with T.
    if (key == 't' || key == 'T') {
        showTextOverlay = !showTextOverlay;
    }

    // Keep manual restart with N.
    if (key == 'n' || key == 'N') {
        currentScene = 1;
        sceneFrameCounter = 0;
        totalFrameCounter = 0;
        fadeOverlayAlpha = 0.0f;
        isSceneTransitionActive = false;
        isFadeOutPhase = false;
        resetVariablesForScene(1);
    }

    if (key == 27) {
        std::exit(0);
    }

    (void)x;
    (void)y;
}

// ==========================================================
// ====== PROGRAM ENTRY POINT ================================
// ==========================================================

int main(int argc, char** argv) {
    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGBA);
    glutInitWindowSize(WINDOW_WIDTH, WINDOW_HEIGHT);
    glutInitWindowPosition(80, 40);
    glutCreateWindow("Life-of-A-Office-Worker-In-A-Modern-Urban-Area");

    glDisable(GL_DEPTH_TEST); // Strictly 2D project

    resetVariablesForScene(1);

    glutDisplayFunc(display);
    glutReshapeFunc(reshape);
    glutKeyboardFunc(keyboard);
    glutTimerFunc(TIMER_INTERVAL_MS, update, 0);

    glutMainLoop();
    return 0;
}
