#ifdef __APPLE__
#include <GLUT/glut.h>
#else
#include <GL/glut.h>
#endif
#include <cmath>
#include <cstdio>
#include <cstdlib>

const float PI_VALUE = 3.14159265f;
const int WINDOW_WIDTH = 1280;
const int WINDOW_HEIGHT = 720;
const int TIMER_INTERVAL_MS = 16;
const int RAIN_DROP_COUNT = 500;

const float homeGarageDoorBottomY = 300.0f;
const float homeGarageDoorHeight = 110.0f;

int currentScene = 1;
int FIRST_SCENE_INDEX = 1;
int LAST_SCENE_INDEX = 9;
bool isRainEnabled = false;
bool isPaused = false;
float animationSpeed = 1.0f;

float rainDropX[RAIN_DROP_COUNT];
float rainDropY[RAIN_DROP_COUNT];

float sunHorizontalOffset = 0.0f;
float cloudOffsetX_layerA = 0.0f;
float cloudOffsetX_layerB = 0.0f;

float wheelRotationAngle = 0.0f;
float garageDoorOpenAmount = 0.0f;

// ============================================
// SCENE 1 VARIABLES
// ============================================
float scene1_carPosX = 400.0f;
float scene1_carPosY = 50.0f;
int carState_scene1 = 0;
bool scene1HasCarExitedScreen = false;

// ============================================
// SCENE 2 VARIABLES
// ============================================
float scene2_redCarPosX = -100.0f;
float scene2_redCarPosY = 55.0f;
int carState_scene2 = 0;
float scene2_aiCarsX[8];
float scene2_aiCarsY[8];
float scene2_aiCarSpeeds[8];
int scene2_aiCarLanes[8];
float scene2_aiCarColors[8][3];

// ============================================
// SCENE 3 VARIABLES
// ============================================
float scene3_carPosX = -100.0f;
float scene3_carPosY = 50.0f;
int carState_scene3 = 0;
float scene3_rampProgress = 0.0f;

// ============================================
// SCENE 4 VARIABLES
// ============================================
float scene4_workerPosX = 1100.0f;
float scene4_workerPosY = 80.0f;
int carState_scene4 = 0;
bool scene4_isWorkerSeated = false;
float scene4_fanAngle1 = 0.0f;
float scene4_fanAngle2 = 60.0f;
float scene4_fanAngle3 = 120.0f;
float scene4_bgWorker1X = 550.0f;
float scene4_bgWorker2X = 900.0f;
int scene4_bgWorker1Dir = 1;
int scene4_bgWorker2Dir = -1;
int scene4_ambientTimer = 0;

// ============================================
// SCENE 5 VARIABLES
// ============================================
float scene5_workerPosX = 300.0f;
float scene5_workerPosY = 80.0f;
int carState_scene5 = 0;
bool scene5_coffeePoured = false;
float scene5_coffeeLevel = 0.0f;
int scene5_pauseTimer = 0;

// ============================================
// SCENE 6 VARIABLES
// ============================================
float scene6_presenterPosX = 1100.0f;
float scene6_presenterPosY = 80.0f;
int carState_scene6 = 0;
int scene6_currentSlide = 0;
int scene6_slideTimer = 0;

// ============================================
// SCENE 7 VARIABLES
// ============================================
float scene7_carPosX = 700.0f;
float scene7_carPosY = -40.0f;
int carState_scene7 = 0;
float scene7_rampProgress = 1.0f;

// ============================================
// SCENE 8 VARIABLES
// ============================================
float scene8_redCarPosX = -100.0f;
float scene8_redCarPosY = 55.0f;
int carState_scene8 = 0;
float scene8_aiCarsX[5];
float scene8_aiCarsY[5];
float scene8_aiCarSpeeds[5];
int scene8_aiCarLanes[5];
float scene8_aiCarColors[5][3];

// ============================================
// SCENE 9 VARIABLES
// ============================================
float scene9_carPosX = 500.0f;
float scene9_carPosY = 50.0f;
int carState_scene9 = 1;
bool scene9ParkingCompleted = false;
bool isHouseLightOn = true;

int sceneFrameCounter = 0;
int scene9ParkedFrameCounter = 0;

// ============================================
// BASIC SHAPES
// ============================================
void rect(float x, float y, float w, float h) {
    glBegin(GL_QUADS);
    glVertex2f(x, y);
    glVertex2f(x + w, y);
    glVertex2f(x + w, y + h);
    glVertex2f(x, y + h);
    glEnd();
}

void circle(float cx, float cy, float r, int seg) {
    if (seg < 12) seg = 12;
    glBegin(GL_POLYGON);
    for (int i = 0; i < seg; i++) {
        float a = 2.0f * PI_VALUE * i / seg;
        glVertex2f(cx + cos(a) * r, cy + sin(a) * r);
    }
    glEnd();
}

void ellipse(float cx, float cy, float rx, float ry, int seg) {
    if (seg < 12) seg = 12;
    glBegin(GL_POLYGON);
    for (int i = 0; i < seg; i++) {
        float a = 2.0f * PI_VALUE * i / seg;
        glVertex2f(cx + cos(a) * rx, cy + sin(a) * ry);
    }
    glEnd();
}

// ============================================
// ENVIRONMENT ELEMENTS
// ============================================
void gradSky(float br, float bg, float bb, float tr, float tg, float tb) {
    glBegin(GL_QUADS);
    glColor3f(br, bg, bb);
    glVertex2f(0, 0);
    glColor3f(br, bg, bb);
    glVertex2f(1280, 0);
    glColor3f(tr, tg, tb);
    glVertex2f(1280, 720);
    glColor3f(tr, tg, tb);
    glVertex2f(0, 720);
    glEnd();
}

void cloud(float cx, float cy) {
    if (isRainEnabled) glColor3f(0.50f, 0.54f, 0.58f);
    else glColor3f(0.98f, 0.98f, 0.99f);
    circle(cx, cy, 28, 28);
    circle(cx - 30, cy - 4, 22, 24);
    circle(cx + 30, cy - 2, 24, 24);
    circle(cx + 2, cy + 18, 20, 24);
}

void sun(float cx, float cy) {
    glColor3f(1.0f, 0.92f, 0.32f);
    circle(cx, cy, 36, 36);
    glColor3f(1, 0.84f, 0.22f);
    glLineWidth(2);
    glBegin(GL_LINES);
    glVertex2f(cx - 54, cy);
    glVertex2f(cx - 38, cy);
    glVertex2f(cx + 38, cy);
    glVertex2f(cx + 54, cy);
    glVertex2f(cx, cy - 54);
    glVertex2f(cx, cy - 38);
    glVertex2f(cx, cy + 38);
    glVertex2f(cx, cy + 54);
    glEnd();
    glLineWidth(1);
}

void moon(float cx, float cy) {
    glColor3f(0.90f, 0.92f, 0.96f);
    circle(cx, cy, 30, 34);
    glColor3f(0.08f, 0.10f, 0.18f);
    circle(cx + 10, cy + 4, 26, 34);
}

void stars() {
    glPointSize(2.8f);
    glBegin(GL_POINTS);
    for (int i = 0; i < 100; i++) { glVertex2f(rand() % 1280, rand() % 720); }
    glEnd();
}

void initRain() { for (int i = 0; i < RAIN_DROP_COUNT; i++) { rainDropX[i] = rand() % WINDOW_WIDTH; rainDropY[i] = rand() % WINDOW_HEIGHT; } }

void drawRain() {
    glColor3f(0.70f, 0.74f, 0.80f);
    glLineWidth(1.4f);
    glBegin(GL_LINES);
    for (int i = 0; i < RAIN_DROP_COUNT; i++) { glVertex2f(rainDropX[i], rainDropY[i]); glVertex2f(rainDropX[i] + 3.0f, rainDropY[i] -14.0f); }
    glEnd();
    glLineWidth(1.0f);
}

void tree(float bx, float by) {
    glColor3f(0.36f, 0.22f, 0.12f);
    rect(bx - 8.0f, by, 16.0f, 50.0f);
    glColor3f(0.12f, 0.38f, 0.14f);
    glBegin(GL_TRIANGLES);
    glVertex2f(bx - 40.0f, by + 30.0f);
    glVertex2f(bx + 40.0f, by + 30.0f);
    glVertex2f(bx, by + 90.0f);
    glEnd();
    glColor3f(0.16f, 0.46f, 0.18f);
    glBegin(GL_TRIANGLES);
    glVertex2f(bx - 32.0f, by + 65.0f);
    glVertex2f(bx + 32.0f, by + 65.0f);
    glVertex2f(bx, by + 120.0f);
    glEnd();
    glColor3f(0.24f, 0.64f, 0.26f);
    glBegin(GL_TRIANGLES);
    glVertex2f(bx - 24.0f, by + 95.0f);
    glVertex2f(bx + 24.0f, by + 95.0f);
    glVertex2f(bx, by + 145.0f);
    glEnd();
}

void road() {
    glColor3f(0.23f, 0.23f, 0.25f);
    rect(0, 0, 1280, 150);
    glColor3f(0.75f, 0.75f, 0.78f);
    rect(0, 0, 1280, 5);
    rect(0, 145, 1280, 5);
    glColor3f(0.95f, 0.95f, 0.72f);
    for (int i = 1; i < 3; i++) {
        float dy = 50.0f * i;
        for (float dx = 20; dx < 1280; dx += 76) rect(dx, dy - 2.0f, 48, 4);
    }
}

void roadNight() {
    glColor3f(0.10f, 0.10f, 0.12f);
    rect(0, 0, 1280, 150);
    glColor3f(0.50f, 0.50f, 0.20f);
    rect(0, 0, 1280, 5);
    rect(0, 145, 1280, 5);
    glColor3f(0.75f, 0.72f, 0.40f);
    for (int i = 1; i < 3; i++) { float dy = 50.0f * i; for (float dx = 20; dx < 1280; dx += 76) rect(dx, dy - 2.0f, 48, 4); }
}

// ============================================
// VEHICLE
// ============================================
void wheel(float centerX, float centerY, float angle) {
    glPushMatrix();
    glTranslatef(centerX, centerY, 0.0f);
    glRotatef(angle, 0.0f, 0.0f, 1.0f);
    glColor3f(0.05f, 0.05f, 0.06f);
    circle(0, 0, 16, 36);
    glColor3f(0.15f, 0.15f, 0.17f);
    circle(0, 0, 12, 30);
    glColor3f(0.74f, 0.74f, 0.78f);
    circle(0, 0, 7, 28);
    glColor3f(0.95f, 0.95f, 0.96f);
    glLineWidth(2);
    glBegin(GL_LINES);
    glVertex2f( -8.0f, 0.0f);
    glVertex2f(8.0f, 0.0f);
    glVertex2f(0.0f, -8.0f);
    glVertex2f(0.0f, 8.0f);
    glVertex2f( -5.5f, -5.5f);
    glVertex2f(5.5f, 5.5f);
    glVertex2f( -5.5f, 5.5f);
    glVertex2f(5.5f, -5.5f);
    glEnd();
    glLineWidth(1);
    glPopMatrix();
}

void drawCar(float posX, float posY, float colorR, float colorG, float colorB, float wheelAngle, bool isHeadlightOn, bool isTaillightOn, bool isFacingRight) {
    glPushMatrix();
    glTranslatef(posX, posY, 0.0f);
    if (!isFacingRight) glScalef( -1.0f, 1.0f, 1.0f);
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    glColor4f(0.0f, 0.0f, 0.0f, 0.24f);
    ellipse(0, -10, 74, 11, 36);
    glColor3f(colorR, colorG, colorB);
    rect( -71, 0, 142, 32);
    glBegin(GL_POLYGON);
    glVertex2f( -38, 32);
    glVertex2f(42, 32);
    glVertex2f(20, 58);
    glVertex2f( -20, 58);
    glEnd();
    glColor3f(0.64f, 0.84f, 0.96f);
    glBegin(GL_POLYGON);
    glVertex2f( -30, 35);
    glVertex2f(34, 35);
    glVertex2f(16, 54);
    glVertex2f( -14, 54);
    glEnd();
    glColor3f(0.18f, 0.18f, 0.20f);
    rect(64, 8, 10, 14);
    rect( -74, 8, 10, 14);
    wheel( -42, -2, wheelAngle);
    wheel(42, -2, wheelAngle);
    if (isHeadlightOn) {
        glColor4f(1.0f, 0.96f, 0.42f, 0.30f);
        glBegin(GL_TRIANGLES);
        glVertex2f(70, 20);
        glVertex2f(186, 58);
        glVertex2f(186, -10);
        glEnd();
        glColor3f(1.0f, 1.0f, 0.72f);
        rect(66, 14, 8, 8);
    }
    if (isTaillightOn) {
        glColor3f(0.95f, 0.08f, 0.08f);
        rect(-74, 16, 8, 8);
        rect(-74, 6, 8, 8);
    }
    glDisable(GL_BLEND);
    glPopMatrix();
}

void drawCarScaled(float posX, float posY, float colorR, float colorG, float colorB, float wheelAngle, bool isHeadlightOn, bool isTaillightOn, bool isFacingRight, float scale) {
    glPushMatrix();
    glTranslatef(posX, posY, 0.0f);
    glScalef(scale, scale, 1.0f);
    drawCar(0, 0, colorR, colorG, colorB, wheelAngle, isHeadlightOn, isTaillightOn, isFacingRight);
    glPopMatrix();
}

void drawHeadlightCone(float offsetX, float offsetY, bool isOn) {
    if (!isOn) return;
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    glColor4f(1.0f, 0.94f, 0.46f, 0.26f);
    glBegin(GL_TRIANGLES);
    glVertex2f(offsetX + 70.0f, offsetY + 20.0f);
    glVertex2f(offsetX + 252.0f, offsetY + 58.0f);
    glVertex2f(offsetX + 252.0f, offsetY - 14.0f);
    glEnd();
    glDisable(GL_BLEND);
}

// ============================================
// ARCHITECTURE - HOME
// ============================================
void homeGround(bool night) {
    float rR = night ? 0.10f : 0.20f, rG = night ? 0.10f : 0.20f, rB = night ? 0.12f : 0.22f;
    float gR = night ? 0.12f : 0.26f, gG = night ? 0.24f : 0.58f, gB = night ? 0.14f : 0.30f;
    float pR = night ? 0.30f : 0.52f, pG = night ? 0.31f : 0.52f, pB = night ? 0.34f : 0.56f;
    glColor3f(rR, rG, rB);
    rect(0, 0, 1280, 110);
    glColor3f(0.66f, 0.66f, 0.70f);
    rect(0, 107, 1280, 3);
    glColor3f(gR, gG, gB);
    rect(0, 110, 1280, 190);
    glColor3f(pR, pG, pB);
    glBegin(GL_POLYGON);
    glVertex2f(216, 110);
    glVertex2f(376, 110);
    glVertex2f(386, 300);
    glVertex2f(206, 300);
    glEnd();
}

void houseWindow(float lx, float by, float w, float h, bool morning, bool lit) {
    glColor3f(0.20f, 0.18f, 0.16f);
    rect(lx, by, w, h);
    if (morning) glColor3f(0.60f, 0.74f, 0.84f);
    else if (lit) glColor3f(0.98f, 0.88f, 0.54f);
    else glColor3f(0.12f, 0.14f, 0.18f);
    rect(lx + 3, by + 3, w - 6, h - 6);
    glColor3f(0.18f, 0.16f, 0.14f);
    rect(lx + w / 2 - 1, by + 3, 2, h - 6);
    rect(lx + 3, by + h / 2 - 1, w - 6, 2);
}

void house(bool isLit) {
    float hX = 464, hY = 300, gX = 200, gY = 300;
    bool morning = (currentScene == 1);
    glColor3f(0.12f, 0.13f, 0.15f);
    rect(hX, hY, 300, 126);
    glColor3f(0.18f, 0.20f, 0.22f);
    rect(hX, hY + 126, 300, 144);
    glColor3f(0.14f, 0.15f, 0.17f);
    rect(gX, gY, 240, 170);
    glColor3f(0.04f, 0.04f, 0.05f);
    glBegin(GL_TRIANGLES);
    glVertex2f(hX - 24, hY + 278);
    glVertex2f(hX + 324, hY + 278);
    glVertex2f(hX + 150, hY + 370);
    glEnd();
    glBegin(GL_TRIANGLES);
    glVertex2f(gX - 24, gY + 176);
    glVertex2f(gX + 264, gY + 176);
    glVertex2f(gX + 120, gY + 230);
    glEnd();
    glColor3f(0.40f, 0.18f, 0.10f);
    rect(hX - 6, hY + 122, 312, 6);
    rect(gX - 6, gY + 168, 252, 6);
    glColor3f(0.35f, 0.16f, 0.08f);
    rect(hX + 94, hY, 56, 120);
    glColor3f(0.75f, 0.75f, 0.80f);
    rect(hX + 140, hY + 50, 4, 32);
    houseWindow(hX + 16, hY + 30, 28, 82, morning, isLit);
    houseWindow(hX + 50, hY + 30, 28, 82, morning, isLit);
    houseWindow(hX + 170, hY + 58, 110, 54, morning, false);
    houseWindow(hX + 16, hY + 150, 80, 80, morning, isLit);
    houseWindow(hX + 104, hY + 150, 80, 80, morning, false);
    houseWindow(hX + 192, hY + 150, 80, 80, morning, isLit);
    houseWindow(gX + 16, gY + 110, 60, 24, morning, isLit);
    houseWindow(gX + 164, gY + 110, 60, 24, morning, isLit);
    glColor3f(0.02f, 0.02f, 0.02f);
    rect(206, homeGarageDoorBottomY, 180, homeGarageDoorHeight);
}

void drawGarageDoor() {
    float cH = homeGarageDoorHeight - garageDoorOpenAmount;
    if (cH <= 0.5f) return;
    float dY = homeGarageDoorBottomY + garageDoorOpenAmount;
    glColor3f(0.06f, 0.07f, 0.08f);
    rect(206, dY, 180, cH);
    glColor3f(0.28f, 0.30f, 0.35f);
    for (int i = 1; i < 8; i++) { float pY = dY + (cH / 8.0f) * i; if (pY < homeGarageDoorBottomY + homeGarageDoorHeight) rect(212, pY, 168, 3); }
}

// ============================================
// TRAFFIC ENTITIES (Scenes 2 & 8)
// ============================================
void trafficLight(float x, float y, bool isRed) {
    glColor3f(0.30f, 0.30f, 0.32f);
    rect(x - 5, y, 10, 40);
    rect(x - 12, y, 6, 100);
    rect(x + 6, y, 6, 100);
    glColor3f(0.18f, 0.18f, 0.20f);
    rect(x - 15, y + 96, 30, 72);
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    if (isRed) {
        glColor4f(0.95f, 0.10f, 0.10f, 0.9f);
        circle(x, y + 150, 9, 22);
        glColor4f(0.95f, 0.15f, 0.10f, 0.3f);
        circle(x, y + 150, 18, 18);
        glColor3f(0.28f, 0.28f, 0.22f);
        circle(x, y + 133, 7, 16);
        glColor3f(0.20f, 0.55f, 0.22f);
        circle(x, y + 118, 7, 16);
    }
    else {
        glColor3f(0.45f, 0.18f, 0.18f);
        circle(x, y + 150, 9, 22);
        glColor3f(0.28f, 0.28f, 0.22f);
        circle(x, y + 133, 7, 16);
        glColor4f(0.10f, 0.90f, 0.20f, 0.9f);
        circle(x, y + 118, 7, 16);
        glColor4f(0.15f, 0.85f, 0.25f, 0.25f);
        circle(x, y + 118, 16, 18);
    }
    glDisable(GL_BLEND);
}

void drawBuilding(float bx, float by, float bw, float bh, bool night, int wCols, int wRows) {
    float wR = night ? 0.18f : 0.50f, wG = night ? 0.20f : 0.52f, wB = night ? 0.26f : 0.58f;
    int hash = (int)(bx * 7 + by * 13) % 1000;
    glColor3f(wR * 0.85f, wG * 0.85f, wB * 0.85f);
    rect(bx, by, bw, bh);
    glColor3f(wR, wG, wB);
    rect(bx + 2, by + 2, bw - 4, bh - 4);
    glColor3f(wR * 0.75f, wG * 0.75f, wB * 0.75f);
    rect(bx, by + bh - 8, bw, 8);
    rect(bx, by, bw, 5);
    glColor3f(0.30f, 0.28f, 0.25f);
    rect(bx, by - 3, bw + 2, 6);
    float wSX = bw / (wCols + 1), wSY = bh / (wRows + 2);
    for (int r = 0; r < wRows; r++) {
        for (int c = 0; c < wCols; c++) {
            float wx = bx + (c + 1) * wSX - 10, wy = by + (r + 1) * wSY;
            bool lit = night && ((hash + c * 3 + r * 7) % 3 != 0);
            glColor3f(0.25f, 0.26f, 0.30f);
            rect(wx, wy, 20, 24);
            if (night && lit) glColor3f(0.95f, 0.85f, 0.45f);
            else if (night) glColor3f(0.08f, 0.10f, 0.16f);
            else glColor3f(0.55f, 0.75f, 0.88f);
            rect(wx + 2, wy + 2, 16, 20);
            glColor3f(0.25f, 0.26f, 0.30f);
            rect(wx + 9, wy + 2, 2, 20);
            rect(wx + 2, wy + 11, 16, 2);
        }
    }
}

void highwayBackground(bool night) {
    float gR = night ? 0.10f : 0.26f, gG = night ? 0.20f : 0.56f, gB = night ? 0.12f : 0.28f;
    glColor3f(gR, gG, gB);
    rect(0, 150, 1280, 18);
    glColor3f(0.40f, 0.35f, 0.25f);
    rect(0, 148, 1280, 5);
    glColor3f(0.82f, 0.80f, 0.74f);
    rect(0, 165, 1280, 8);
    drawBuilding(30, 165, 90, 340, night, 2, 7);
    drawBuilding(150, 165, 70, 285, night, 2, 6);
    drawBuilding(250, 165, 110, 365, night, 3, 8);
    drawBuilding(400, 165, 80, 305, night, 2, 7);
    drawBuilding(510, 165, 100, 355, night, 3, 8);
    drawBuilding(650, 165, 75, 275, night, 2, 6);
    drawBuilding(760, 165, 95, 345, night, 3, 7);
    drawBuilding(890, 165, 85, 295, night, 2, 6);
    drawBuilding(1010, 165, 105, 365, night, 3, 8);
    drawBuilding(1150, 165, 80, 315, night, 2, 7);
    if (night) {
        for (float lx = 60; lx < 1280; lx += 160) {
            glColor3f(0.55f, 0.55f, 0.50f);
            rect(lx, 170, 5, 100);
            glColor3f(0.72f, 0.72f, 0.68f);
            rect(lx + 1, 170, 3, 60);
            ellipse(lx + 2.5f, 230, 20, 8, 16);
            glEnable(GL_BLEND);
            glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
            glColor4f(0.95f, 0.90f, 0.50f, 0.10f);
            circle(lx + 2, 205, 30, 20);
            glDisable(GL_BLEND);
        }
    }
}

// ============================================
// OFFICE EXTERIOR ENTITIES (Scenes 3 & 7)
// ============================================
void officeBuildingExterior(bool night) {
    float wR = night ? 0.22f : 0.52f, wG = night ? 0.24f : 0.55f, wB = night ? 0.30f : 0.62f;
    float aR = night ? 0.16f : 0.40f, aG = night ? 0.18f : 0.42f, aB = night ? 0.22f : 0.48f;
    float winR, winG, winB;
    if (night) { winR = 0.98f; winG = 0.88f; winB = 0.54f; } else { winR = 0.55f; winG = 0.76f; winB = 0.88f; }
    glColor3f(wR, wG, wB);
    rect(380, 150, 520, 430);
    glColor3f(aR, aG, aB);
    rect(380, 560, 520, 20);
    rect(380, 150, 520, 10);
    for (int i = 0; i < 4; i++) { float px = 510 + i * 120; rect(px, 160, 8, 400); }
    for (int row = 0; row < 8; row++) {
        for (int col = 0; col < 5; col++) {
            float wx = 400.0f + col * 96.0f, wy = 180.0f + row * 46.0f;
            bool lit = night && ((row + col) % 3 != 0);
            glColor3f(0.25f, 0.26f, 0.30f);
            rect(wx, wy, 72, 34);
            if (night && lit) glColor3f(0.95f, 0.85f, 0.50f);
            else if (night) glColor3f(0.08f, 0.10f, 0.16f);
            else glColor3f(winR, winG, winB);
            rect(wx + 3, wy + 3, 66, 28);
            glColor3f(0.25f, 0.26f, 0.30f);
            rect(wx + 35, wy + 3, 2, 28);
            rect(wx + 3, wy + 15, 66, 2);
        }
    }
    glColor3f(0.20f, 0.22f, 0.28f);
    rect(570, 150, 140, 90);
    glColor3f(0.42f, 0.58f, 0.70f);
    rect(578, 158, 58, 74);
    rect(644, 158, 58, 74);
    glColor3f(0.60f, 0.58f, 0.55f);
    rect(380, 555, 520, 6);
}

void undergroundEntrance() {
    glColor3f(0.03f, 0.04f, 0.06f);
    rect(600, 0, 200, 50);
    glColor3f(0.14f, 0.14f, 0.16f);
    rect(605, 5, 190, 40);
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    glColor4f(0.95f, 0.85f, 0.30f, 0.35f);
    rect(605, 42, 190, 6);
    glDisable(GL_BLEND);
    glColor3f(0.90f, 0.78f, 0.12f);
    for (float sx = 610; sx < 790; sx += 24) rect(sx, 44, 14, 4);
    glColor3f(0.15f, 0.45f, 0.85f);
    rect(660, 45, 80, 18);
    glColor3f(0.95f, 0.95f, 0.95f);
    rect(670, 48, 25, 12);
    rect(705, 48, 25, 12);
}

// ============================================
// OFFICE INTERIOR ENTITIES (Scene 4)
// ============================================
void wallClock(float x, float y) {
    glColor3f(0.15f, 0.15f, 0.18f);
    circle(x, y, 24, 32);
    glColor3f(0.98f, 0.98f, 0.96f);
    circle(x, y, 21, 32);
    glColor3f(0.12f, 0.12f, 0.14f);
    circle(x, y, 20, 32);
    glColor3f(0.98f, 0.98f, 0.96f);
    circle(x, y, 18, 32);
    for (int i = 0; i < 12; i++) {
        float a = i * 30.0f * PI_VALUE / 180.0f;
        glColor3f(0.15f, 0.15f, 0.18f);
        rect(x + cos(a) * 15 - 1, y + sin(a) * 15 - 1, 2, 2);
    }
    glColor3f(0.08f, 0.08f, 0.10f);
    glLineWidth(2);
    glBegin(GL_LINES);
    glVertex2f(x, y);
    glVertex2f(x + 8, y + 8);
    glEnd();
    glLineWidth(1);
    glColor3f(0.08f, 0.08f, 0.10f);
    glLineWidth(1.5f);
    glBegin(GL_LINES);
    glVertex2f(x, y);
    glVertex2f(x - 12, y);
    glEnd();
    glLineWidth(1);
    glColor3f(0.85f, 0.08f, 0.08f);
    circle(x, y, 2, 10);
}

void pictureFrame(float x, float y, float w, float h) {
    glColor3f(0.10f, 0.06f, 0.04f);
    rect(x, y, w, h);
    glColor3f(0.35f, 0.22f, 0.12f);
    rect(x + 3, y + 3, w - 6, h - 6);
    glColor3f(0.82f, 0.84f, 0.88f);
    rect(x + 6, y + 6, w - 12, h - 12);
    glColor3f(0.45f, 0.55f, 0.65f);
    rect(x + 10, y + 10, w - 20, h - 20);
    glColor3f(0.55f, 0.65f, 0.78f);
    rect(x + 10, y + h - 16, w - 20, 4);
    for (int i = 0; i < 3; i++) {
        glColor3f(0.75f, 0.78f, 0.82f);
        rect(x + 14 + i * (w - 28) / 2.5f, y + h / 2 - 6, 12, 10);
        glColor3f(0.50f, 0.65f, 0.60f);
        rect(x + 16 + i * (w - 28) / 2.5f, y + h / 2 - 4, 8, 6);
    }
    glColor3f(0.25f, 0.38f, 0.55f);
    ellipse(x + w / 2, y + h / 2, w * 0.15f, h * 0.15f, 20);
    glColor3f(1.0f, 0.90f, 0.20f);
    ellipse(x + w / 2 + 2, y + h * 0.8f, w * 0.05f, h * 0.04f, 12);
}

void noticeBoard(float x, float y, float w, float h) {
    glColor3f(0.40f, 0.42f, 0.46f);
    rect(x, y, w, h);
    glColor3f(0.62f, 0.48f, 0.30f);
    rect(x + 4, y + 4, w - 8, h - 8);
    glColor3f(0.95f, 0.95f, 0.92f);
    rect(x + 10, y + 20, w * 0.35f, h * 0.45f);
    glColor3f(0.98f, 0.98f, 0.60f);
    rect(x + w * 0.50f, y + 15, w * 0.32f, h * 0.35f);
    glColor3f(0.98f, 0.98f, 0.95f);
    rect(x + 15, y + h * 0.70f, w * 0.55f, h * 0.22f);
    glColor3f(0.85f, 0.85f, 0.80f);
    rect(x + w * 0.60f, y + h * 0.55f, w * 0.30f, h * 0.38f);
    glColor3f(0.60f, 0.65f, 0.70f);
    circle(x + 20, y + h - 12, 3, 8);
    circle(x + 30, y + h - 12, 3, 8);
    circle(x + 40, y + h - 12, 3, 8);
}

void officePlant(float x, float y) {
    glColor3f(0.55f, 0.28f, 0.12f);
    rect(x - 8, y, 16, 22);
    glColor3f(0.50f, 0.25f, 0.10f);
    rect(x - 10, y + 18, 20, 4);
    glColor3f(0.12f, 0.40f, 0.16f);
    glBegin(GL_TRIANGLES);
    glVertex2f(x - 18, y + 22);
    glVertex2f(x + 18, y + 22);
    glVertex2f(x, y + 50);
    glEnd();
    glColor3f(0.15f, 0.48f, 0.20f);
    glBegin(GL_TRIANGLES);
    glVertex2f(x - 14, y + 34);
    glVertex2f(x + 14, y + 34);
    glVertex2f(x, y + 58);
    glEnd();
    glColor3f(0.20f, 0.55f, 0.24f);
    glBegin(GL_TRIANGLES);
    glVertex2f(x - 10, y + 44);
    glVertex2f(x + 10, y + 44);
    glVertex2f(x, y + 64);
    glEnd();
    glColor3f(0.18f, 0.50f, 0.22f);
    ellipse(x, y + 24, 16, 8, 16);
}

void keyboard(float x, float y) {
    glColor3f(0.15f, 0.15f, 0.18f);
    rect(x, y, 34, 12);
    glColor3f(0.22f, 0.22f, 0.25f);
    rect(x + 1, y + 1, 32, 10);
    for (int r = 0; r < 4; r++) { for (int c = 0; c < 5; c++) { glColor3f(0.30f, 0.30f, 0.33f); rect(x + 2 + c * 6, y + 2 + r * 2.5f, 5, 2); } }
    glColor3f(0.25f, 0.25f, 0.28f);
    rect(x + 1, y + 10, 32, 2);
}

void mouseDesk(float x, float y) {
    glColor3f(0.22f, 0.22f, 0.25f);
    ellipse(x, y, 8, 5, 16);
    glColor3f(0.28f, 0.28f, 0.32f);
    ellipse(x, y + 1, 6, 3, 12);
    glColor3f(0.35f, 0.35f, 0.38f);
    rect(x - 2, y, 4, 1);
}

void coffeeMug(float x, float y) {
    glColor3f(0.95f, 0.93f, 0.88f);
    glBegin(GL_POLYGON);
    glVertex2f(x, y);
    glVertex2f(x + 12, y);
    glVertex2f(x + 11, y + 14);
    glVertex2f(x + 1, y + 14);
    glEnd();
    glColor3f(0.35f, 0.22f, 0.10f);
    rect(x + 2, y + 2, 8, 10);
    glColor3f(0.92f, 0.90f, 0.85f);
    rect(x + 12, y + 2, 4, 6);
    circle(x + 16, y + 5, 3, 8);
    glColor3f(0.95f, 0.90f, 0.80f);
    rect(x + 1, y + 12, 10, 2);
}

void officeRoom() {
    glColor3f(0.88f, 0.85f, 0.80f);
    rect(0, 80, 1280, 560);
    for (float px = 0; px < 1280; px += 120) {
        glColor3f(0.54f, 0.40f, 0.28f);
        rect(px, 0, 60, 80);
        glColor3f(0.60f, 0.46f, 0.32f);
        rect(px + 2, 2, 56, 76);
    }
    glColor3f(0.50f, 0.36f, 0.24f);
    rect(0, 78, 1280, 4);
    for (float fx = 0; fx < 1280; fx += 120) {
        glColor3f(0.48f, 0.34f, 0.22f);
        rect(fx, 642, 60, 78);
        glColor3f(0.42f, 0.30f, 0.18f);
        rect(fx + 4, 644, 52, 74);
    }
    glColor3f(0.44f, 0.30f, 0.18f);
    rect(0, 640, 1280, 4);
    glColor3f(0.68f, 0.58f, 0.44f);
    rect(0, 638, 1280, 3);
    for (float lx = 200; lx < 1280; lx += 350) {
        glColor3f(0.90f, 0.90f, 0.88f);
        rect(lx, 72, 70, 10);
        glColor3f(0.82f, 0.82f, 0.80f);
        rect(lx + 32, 0, 6, 72);
    }
    glColor3f(0.30f, 0.32f, 0.38f);
    rect(1180, 80, 30, 260);
    rect(1240, 80, 30, 260);
    glColor3f(0.45f, 0.55f, 0.68f);
    rect(1184, 84, 22, 252);
    glColor3f(0.70f, 0.78f, 0.85f);
    rect(1190, 130, 10, 30);
    glColor3f(0.10f, 0.10f, 0.12f);
    rect(1180, 336, 86, 4);
    rect(1180, 80, 4, 260);
    rect(1262, 80, 4, 260);
    glColor3f(0.60f, 0.75f, 0.88f);
    rect(150, 360, 220, 200);
    glColor3f(0.45f, 0.52f, 0.58f);
    rect(150, 360, 220, 4);
    rect(150, 360, 4, 200);
    rect(366, 360, 4, 200);
    rect(150, 556, 220, 4);
    rect(256, 360, 4, 200);
    glColor3f(0.35f, 0.50f, 0.68f);
    rect(153, 363, 214, 194);
    glColor3f(0.55f, 0.72f, 0.88f);
    rect(156, 366, 208, 55);
    for (int wy = 426; wy < 554; wy += 28) { glColor3f(0.45f, 0.62f, 0.80f); rect(156, wy, 208, 2); }
    glColor3f(0.60f, 0.75f, 0.88f);
    rect(580, 580, 3, 58);
    rect(690, 560, 3, 78);
}

void officeDesk(float x, float y) {
    glColor3f(0.55f, 0.40f, 0.25f);
    rect(x, y + 28, 120, 8);
    glColor3f(0.48f, 0.34f, 0.20f);
    rect(x, y + 36, 120, 3);
    glColor3f(0.42f, 0.30f, 0.18f);
    rect(x + 4, y, 6, 28);
    rect(x + 110, y, 6, 28);
    glColor3f(0.50f, 0.36f, 0.22f);
    rect(x + 50, y + 6, 65, 22);
    glColor3f(0.45f, 0.32f, 0.18f);
    rect(x + 52, y + 8, 28, 8);
    rect(x + 82, y + 8, 28, 8);
    glColor3f(0.38f, 0.26f, 0.14f);
    rect(x + 54, y + 10, 4, 4);
    rect(x + 84, y + 10, 4, 4);
    glColor3f(0.42f, 0.30f, 0.18f);
    rect(x + 10, y - 2, 20, 5);
}

void officeChair(float x, float y) {
    glColor3f(0.18f, 0.18f, 0.24f);
    rect(x - 14, y + 10, 28, 20);
    glColor3f(0.15f, 0.15f, 0.20f);
    rect(x - 12, y + 12, 24, 16);
    glColor3f(0.22f, 0.22f, 0.28f);
    rect(x - 2, y, 4, 28);
    glColor3f(0.14f, 0.14f, 0.18f);
    rect(x - 18, y + 30, 36, 5);
    glColor3f(0.10f, 0.10f, 0.14f);
    rect(x - 20, y + 35, 8, 4);
    rect(x + 12, y + 35, 8, 4);
    glColor3f(0.12f, 0.12f, 0.16f);
    rect(x - 20, y - 4, 6, 6);
    rect(x + 14, y - 4, 6, 6);
}

void computerMonitor(float x, float y, bool on) {
    glColor3f(0.18f, 0.18f, 0.20f);
    rect(x, y, 60, 42);
    if (on) { glColor3f(0.22f, 0.42f, 0.72f); rect(x + 3, y + 3, 54, 36);
        glColor3f(0.40f, 0.62f, 0.88f);
        rect(x + 5, y + 5, 50, 14);
        for (int ly = 22; ly < 37; ly += 5) { glColor3f(0.30f, 0.52f, 0.78f); rect(x + 5, y + ly, 50, 2); }
        glColor3f(0.60f, 0.75f, 0.90f);
        rect(x + 10, y + 7, 15, 10);
        glColor3f(0.25f, 0.50f, 0.30f);
        rect(x + 28, y + 7, 8, 10);
        glColor3f(0.85f, 0.35f, 0.30f);
        rect(x + 39, y + 7, 12, 10);
    } else { glColor3f(0.10f, 0.10f, 0.12f);
    rect(x + 3, y + 3, 54, 36);
    }
    glColor3f(0.20f, 0.20f, 0.22f);
    rect(x + 24, y - 10, 12, 10);
    rect(x + 18, y - 12, 24, 5);
    glColor3f(0.15f, 0.50f, 0.20f);
    circle(x + 55, y + 39, 2, 6);
}

void ceilingFan(float x, float y, float angle) {
    glColor3f(0.80f, 0.78f, 0.76f);
    circle(x, y, 8, 16);
    for (int i = 0; i < 4; i++) {
        float a = angle * PI_VALUE / 180.0f + i * 90.0f * PI_VALUE / 180.0f;
        float bx = x + cos(a) * 50, by = y + sin(a) * 50;
        glColor3f(0.62f, 0.62f, 0.66f);
        glBegin(GL_TRIANGLES);
        glVertex2f(x + cos(a) * 8, y + sin(a) * 8);
        glVertex2f(bx - sin(a) * 14, by + cos(a) * 14);
        glVertex2f(bx + sin(a) * 14, by - cos(a) * 14);
        glEnd();
    }
    glColor3f(0.55f, 0.55f, 0.58f);
    rect(x - 2, y + 6, 4, 28);
    glColor3f(0.72f, 0.70f, 0.68f);
    circle(x, y + 8, 5, 12);
}

void workerCharacter(float x, float y, bool sitting) {
    if (sitting) {
        glColor3f(0.14f, 0.22f, 0.48f);
        rect(x - 10, y + 18, 20, 22);
        glColor3f(0.10f, 0.18f, 0.40f);
        rect(x - 8, y + 20, 16, 18);
        glColor3f(0.88f, 0.22f, 0.18f);
        rect(x - 2, y + 22, 4, 8);
        glColor3f(0.95f, 0.78f, 0.60f);
        circle(x, y + 44, 8, 20);
        glColor3f(0.22f, 0.12f, 0.06f);
        ellipse(x, y + 47, 9, 6, 14);
        glColor3f(0.16f, 0.08f, 0.04f);
        rect(x - 4, y + 46, 8, 2);
        glColor3f(0.10f, 0.08f, 0.06f);
        circle(x - 3, y + 43, 1.5f, 8);
        circle(x + 3, y + 43, 1.5f, 8);
        glColor3f(0.95f, 0.78f, 0.60f);
        rect(x - 16, y + 26, 8, 3);
        rect(x + 8, y + 26, 8, 3);
        glColor3f(0.15f, 0.15f, 0.18f);
        rect(x - 6, y + 6, 5, 14);
        rect(x + 1, y + 6, 5, 14);
        glColor3f(0.20f, 0.12f, 0.06f);
        rect(x - 6, y + 4, 6, 4);
        rect(x + 1, y + 4, 6, 4);
    } else {
        glColor3f(0.14f, 0.22f, 0.48f);
        rect(x - 10, y + 30, 20, 30);
        glColor3f(0.10f, 0.18f, 0.40f);
        rect(x - 8, y + 32, 16, 26);
        glColor3f(0.88f, 0.22f, 0.18f);
        rect(x - 2, y + 34, 4, 10);
        glColor3f(0.15f, 0.15f, 0.18f);
        rect(x - 2, y + 40, 4, 6);
        glColor3f(0.95f, 0.78f, 0.60f);
        circle(x, y + 70, 9, 20);
        glColor3f(0.22f, 0.12f, 0.06f);
        ellipse(x, y + 74, 10, 7, 14);
        glColor3f(0.16f, 0.08f, 0.04f);
        rect(x - 4, y + 73, 8, 2);
        glColor3f(0.10f, 0.08f, 0.06f);
        circle(x - 3, y + 69, 1.5f, 8);
        circle(x + 3, y + 69, 1.5f, 8);
        glColor3f(0.95f, 0.78f, 0.60f);
        rect(x - 18, y + 40, 10, 4);
        rect(x + 8, y + 40, 10, 4);
        glColor3f(0.15f, 0.15f, 0.18f);
        rect(x - 6, y, 5, 30);
        rect(x + 1, y, 5, 30);
        glColor3f(0.20f, 0.12f, 0.06f);
        rect(x - 6, y - 2, 6, 4);
        rect(x + 1, y - 2, 6, 4);
    }
}

void backgroundWorker(float x, float y) {
    glColor3f(0.45f, 0.45f, 0.52f);
    rect(x - 7, y + 12, 14, 22);
    glColor3f(0.40f, 0.38f, 0.46f);
    rect(x - 5, y + 14, 10, 18);
    glColor3f(0.05f, 0.05f, 0.08f);
    rect(x - 2, y + 16, 4, 6);
    glColor3f(0.82f, 0.74f, 0.62f);
    circle(x, y + 38, 7, 16);
    glColor3f(0.24f, 0.16f, 0.08f);
    ellipse(x, y + 41, 8, 5, 12);
    glColor3f(0.35f, 0.35f, 0.40f);
    rect(x - 6, y, 5, 14);
    rect(x + 1, y, 5, 14);
    glColor3f(0.20f, 0.14f, 0.08f);
    rect(x - 6, y - 2, 6, 4);
    rect(x + 1, y - 2, 6, 4);
    glColor3f(0.82f, 0.74f, 0.62f);
    rect(x - 12, y + 20, 5, 3);
    rect(x + 7, y + 20, 5, 3);
}

// ============================================
// BREAKROOM ENTITIES (Scene 5)
// ============================================
void breakRoom() {
    glColor3f(0.88f, 0.85f, 0.80f);
    rect(0, 80, 1280, 560);
    for (float px = 0; px < 1280; px += 120) {
        glColor3f(0.54f, 0.40f, 0.28f);
        rect(px, 0, 60, 80);
        glColor3f(0.60f, 0.46f, 0.32f);
        rect(px + 2, 2, 56, 76);
    }
    glColor3f(0.50f, 0.36f, 0.24f);
    rect(0, 78, 1280, 4);
    for (float fx = 0; fx < 1280; fx += 120) {
        glColor3f(0.48f, 0.34f, 0.22f);
        rect(fx, 642, 60, 78);
        glColor3f(0.42f, 0.30f, 0.18f);
        rect(fx + 4, 644, 52, 74);
    }
    glColor3f(0.44f, 0.30f, 0.18f);
    rect(0, 640, 1280, 4);
    glColor3f(0.66f, 0.58f, 0.44f);
    rect(0, 638, 1280, 3);
    glColor3f(0.60f, 0.75f, 0.88f);
    rect(100, 340, 200, 180);
    glColor3f(0.45f, 0.52f, 0.58f);
    rect(100, 340, 200, 4);
    rect(100, 340, 4, 180);
    rect(296, 340, 4, 180);
    rect(100, 516, 200, 4);
    rect(196, 340, 4, 180);
    glColor3f(0.60f, 0.75f, 0.88f);
    rect(750, 380, 200, 160);
    glColor3f(0.45f, 0.52f, 0.58f);
    rect(750, 380, 200, 4);
    rect(750, 380, 4, 160);
    rect(946, 380, 4, 160);
    rect(750, 536, 200, 4);
    rect(846, 380, 4, 160);
    glColor3f(0.42f, 0.38f, 0.34f);
    rect(750, 80, 500, 5);
    glColor3f(0.58f, 0.42f, 0.28f);
    rect(750, 85, 500, 280);
    glColor3f(0.65f, 0.60f, 0.54f);
    rect(760, 85, 430, 6);
    glColor3f(0.90f, 0.86f, 0.80f);
    rect(760, 95, 430, 265);
    glColor3f(0.58f, 0.48f, 0.38f);
    rect(780, 130, 90, 80);
    rect(880, 130, 90, 80);
    rect(980, 130, 90, 80);
    rect(1080, 130, 90, 80);
    glColor3f(0.52f, 0.42f, 0.32f);
    rect(783, 133, 84, 74);
    rect(883, 133, 84, 74);
    rect(983, 133, 84, 74);
    rect(1083, 133, 84, 74);
}

void coffeeMachine(float x, float y) {
    glColor3f(0.25f, 0.25f, 0.28f);
    rect(x, y, 42, 72);
    glColor3f(0.20f, 0.20f, 0.22f);
    rect(x + 3, y + 50, 36, 20);
    glColor3f(0.14f, 0.14f, 0.16f);
    rect(x + 10, y + 36, 22, 15);
    glColor3f(0.85f, 0.15f, 0.12f);
    circle(x + 14, y + 26, 5, 12);
    glColor3f(0.15f, 0.70f, 0.20f);
    circle(x + 30, y + 26, 5, 12);
    glColor3f(0.92f, 0.85f, 0.15f);
    circle(x + 22, y + 16, 4, 10);
    glColor3f(0.68f, 0.68f, 0.72f);
    rect(x + 16, y + 50, 10, 6);
    glColor3f(0.60f, 0.60f, 0.64f);
    rect(x + 2, y + 70, 38, 4);
    glColor3f(0.22f, 0.22f, 0.25f);
    rect(x + 6, y - 8, 30, 10);
}

void coffeeCup(float x, float y, float fillLevel) {
    glColor3f(0.95f, 0.93f, 0.90f);
    glBegin(GL_POLYGON);
    glVertex2f(x, y);
    glVertex2f(x + 24, y);
    glVertex2f(x + 22, y + 30);
    glVertex2f(x + 2, y + 30);
    glEnd();
    if (fillLevel > 0.01f) {
        float fH = 28.0f * fillLevel;
        float fT = y + 30 - fH;
        glColor3f(0.35f, 0.22f, 0.10f);
        glBegin(GL_POLYGON);
        glVertex2f(x + 2, fT);
        glVertex2f(x + 22, fT);
        glVertex2f(x + 21, y + 28);
        glVertex2f(x + 3, y + 28);
        glEnd();
    }
    glColor3f(0.90f, 0.88f, 0.85f);
    glBegin(GL_POLYGON);
    glVertex2f(x + 22, y + 5);
    glVertex2f(x + 30, y + 3);
    glVertex2f(x + 32, y + 13);
    glVertex2f(x + 26, y + 17);
    glEnd();
}

// ============================================
// PRESENTATION ENTITIES (Scene 6)
// ============================================
void calendarSheet(float x, float y) {
    glColor3f(0.95f, 0.95f, 0.92f);
    rect(x, y, 55, 65);
    glColor3f(0.85f, 0.82f, 0.78f);
    rect(x + 1, y + 1, 53, 63);
    glColor3f(0.25f, 0.30f, 0.55f);
    rect(x + 2, y + 2, 51, 14);
    glColor3f(0.95f, 0.95f, 0.95f);
    rect(x + 3, y + 3, 49, 12);
    glColor3f(0.55f, 0.55f, 0.58f);
    for (int r = 0; r < 5; r++) {
        for (int c = 0; c < 7; c++) {
            rect(x + 4 + c * 7, y + 20 + r * 8, 6, 6);
            glColor3f(0.75f, 0.75f, 0.78f);
            rect(x + 5 + c * 7, y + 21 + r * 8, 4, 4);
            glColor3f(0.55f, 0.55f, 0.58f);
        }
    }
    glColor3f(0.85f, 0.18f, 0.18f);
    rect(x + 4, y + 20, 6, 6);
    glColor3f(0.25f, 0.30f, 0.55f);
    rect(x + 6, y + 60, 4, 2);
    glColor3f(0.18f, 0.18f, 0.20f);
    circle(x + 38, y + 13, 2, 6);
    circle(x + 44, y + 13, 2, 6);
}

void presentationRoom() {
    glColor3f(0.90f, 0.88f, 0.84f);
    rect(0, 80, 1280, 560);
    for (float px = 0; px < 1280; px += 120) {
        glColor3f(0.48f, 0.36f, 0.24f);
        rect(px, 0, 60, 80);
        glColor3f(0.54f, 0.40f, 0.28f);
        rect(px + 2, 2, 56, 76);
    }
    glColor3f(0.44f, 0.36f, 0.25f);
    rect(0, 78, 1280, 4);
    for (float fx = 0; fx < 1280; fx += 120) {
        glColor3f(0.44f, 0.34f, 0.22f);
        rect(fx, 642, 60, 78);
        glColor3f(0.38f, 0.28f, 0.16f);
        rect(fx + 4, 644, 52, 74);
    }
    glColor3f(0.40f, 0.32f, 0.20f);
    rect(0, 640, 1280, 4);
    glColor3f(0.66f, 0.58f, 0.44f);
    rect(0, 638, 1280, 3);
    glColor3f(0.30f, 0.32f, 0.38f);
    rect(50, 80, 18, 280);
    rect(1212, 80, 18, 280);
    glColor3f(0.45f, 0.50f, 0.58f);
    rect(54, 84, 10, 272);
    rect(1216, 84, 10, 272);
    glColor3f(0.55f, 0.60f, 0.68f);
    rect(46, 78, 26, 4);
    rect(1208, 78, 26, 4);
    glColor3f(0.18f, 0.20f, 0.25f);
    rect(47, 350, 1186, 5);
    for (float pb = 50; pb < 1230; pb += 60) { glColor3f(0.35f, 0.38f, 0.45f); rect(pb, 350, 0.5f, 140); }
}

void presentationBoard(float x, float y) {
    glColor3f(0.70f, 0.72f, 0.75f);
    rect(x - 6, y - 6, 412, 296);
    glColor3f(0.18f, 0.18f, 0.20f);
    rect(x, y, 400, 10);
    rect(x, y + 274, 400, 10);
    glColor3f(0.92f, 0.95f, 0.98f);
    rect(x + 5, y + 10, 390, 264);
    glColor3f(0.82f, 0.84f, 0.88f);
    rect(x + 5, y + 10, 390, 3);
    rect(x + 5, y + 271, 390, 3);
    glColor3f(0.85f, 0.10f, 0.10f);
    rect(x + 10, y + 3, 380, 5);
    glColor3f(0.18f, 0.18f, 0.20f);
    rect(x + 155, y + 284, 90, 60);
    rect(x + 190, y + 344, 20, 40);
    glColor3f(0.25f, 0.45f, 0.68f);
    rect(x + 160, y + 289, 80, 50);
    rect(x + 195, y + 349, 10, 30);
}

void slideContent(int slideNumber) {
    float bx = 145.0f, by = 250.0f;
    if (slideNumber == 1) {
        glColor3f(0.20f, 0.30f, 0.60f);
        rect(bx, by + 200, 350, 30);
        glColor3f(0.95f, 0.95f, 0.98f);
        rect(bx + 8, by + 205, 334, 20);
        glColor3f(0.15f, 0.15f, 0.20f);
        rect(bx + 30, by + 150, 290, 8);
        rect(bx + 30, by + 125, 250, 8);
        rect(bx + 30, by + 100, 270, 8);
        glColor3f(0.40f, 0.40f, 0.45f);
        rect(bx + 30, by + 60, 200, 6);
        rect(bx + 30, by + 45, 180, 6);
        rect(bx + 30, by + 30, 220, 6);
    } else if (slideNumber == 2) {
        glColor3f(0.20f, 0.50f, 0.30f);
        rect(bx, by + 200, 350, 30);
        glColor3f(0.95f, 0.95f, 0.98f);
        rect(bx + 8, by + 205, 334, 20);
        glColor3f(0.15f, 0.15f, 0.20f);
        rect(bx + 30, by + 150, 290, 8);
        glColor3f(0.20f, 0.55f, 0.75f);
        rect(bx + 40, by + 90, 40, 60);
        glColor3f(0.25f, 0.70f, 0.35f);
        rect(bx + 100, by + 110, 40, 40);
        glColor3f(0.85f, 0.55f, 0.20f);
        rect(bx + 160, by + 70, 40, 80);
        glColor3f(0.70f, 0.25f, 0.25f);
        rect(bx + 220, by + 100, 40, 50);
        glColor3f(0.40f, 0.40f, 0.45f);
        rect(bx + 30, by + 30, 290, 6);
    } else if (slideNumber == 3) {
        glColor3f(0.60f, 0.35f, 0.15f);
        rect(bx, by + 200, 350, 30);
        glColor3f(0.95f, 0.95f, 0.98f);
        rect(bx + 8, by + 205, 334, 20);
        glColor3f(0.15f, 0.15f, 0.20f);
        rect(bx + 30, by + 150, 290, 8);
        glColor3f(0.20f, 0.55f, 0.75f);
        rect(bx + 40, by + 100, 80, 40);
        glColor3f(0.85f, 0.65f, 0.20f);
        glBegin(GL_TRIANGLES);
        glVertex2f(bx + 160, by + 60);
        glVertex2f(bx + 200, by + 140);
        glVertex2f(bx + 240, by + 60);
        glEnd();
        glColor3f(0.25f, 0.70f, 0.35f);
        rect(bx + 260, by + 80, 80, 60);
        glColor3f(0.40f, 0.40f, 0.45f);
        rect(bx + 30, by + 30, 290, 6);
    } else if (slideNumber == 4) {
        glColor3f(0.80f, 0.25f, 0.20f);
        rect(bx, by + 200, 350, 30);
        glColor3f(0.95f, 0.95f, 0.98f);
        rect(bx + 8, by + 205, 334, 20);
        glColor3f(0.15f, 0.15f, 0.20f);
        rect(bx + 80, by + 120, 190, 8);
        glColor3f(0.40f, 0.40f, 0.45f);
        rect(bx + 100, by + 80, 150, 6);
        rect(bx + 120, by + 60, 110, 6);
        glColor3f(0.95f, 0.90f, 0.40f);
        circle(bx + 175, by + 30, 20, 20);
    }
}

void fruitBowl(float x, float y) {
    glColor3f(0.92f, 0.88f, 0.20f);
    circle(x, y, 14, 16);
    glColor3f(0.90f, 0.85f, 0.15f);
    circle(x + 6, y + 4, 10, 16);
    glColor3f(0.88f, 0.60f, 0.20f);
    circle(x + 20, y - 2, 8, 12);
    glColor3f(0.95f, 0.30f, 0.22f);
    circle(x + 32, y + 2, 9, 12);
    glColor3f(0.30f, 0.70f, 0.25f);
    circle(x + 44, y - 4, 7, 12);
    glColor3f(0.15f, 0.15f, 0.18f);
    rect(x - 5, y - 15, 8, 4);
    rect(x + 15, y - 12, 8, 4);
}

void snackDispenser(float x, float y) {
    glColor3f(0.70f, 0.72f, 0.78f);
    rect(x, y, 20, 40);
    glColor3f(0.25f, 0.28f, 0.35f);
    rect(x + 2, y + 3, 16, 35);
    glColor3f(0.30f, 0.35f, 0.42f);
    rect(x + 4, y + 15, 12, 8);
}

void waterCooler(float x, float y) {
    glColor3f(0.78f, 0.78f, 0.82f);
    rect(x, y, 45, 70);
    glColor3f(0.65f, 0.72f, 0.80f);
    rect(x + 4, y + 4, 37, 10);
    rect(x + 4, y + 56, 37, 10);
    glColor3f(0.85f, 0.85f, 0.88f);
    rect(x + 8, y + 16, 29, 38);
}

void audienceMember(float x, float y, int variant) {
    float sR, sG, sB;
    if (variant % 3 == 0) { sR = 0.30f; sG = 0.35f; sB = 0.55f; }
    else if (variant % 3 == 1) { sR = 0.48f; sG = 0.24f; sB = 0.24f; }
    else { sR = 0.25f; sG = 0.40f; sB = 0.30f; }
    glColor3f(sR, sG, sB);
    rect(x - 7, y + 6, 14, 22);
    glColor3f(0.82f, 0.74f, 0.65f);
    circle(x, y + 34, 6, 14);
    glColor3f(0.22f, 0.14f, 0.08f);
    ellipse(x, y + 37, 7, 4, 12);
    glColor3f(0.40f, 0.40f, 0.46f);
    rect(x - 5, y, 5, 10);
    rect(x, y, 5, 10);
    glColor3f(0.25f, 0.20f, 0.12f);
    rect(x - 5, y - 2, 5, 3);
    rect(x, y - 2, 5, 3);
}

// ============================================
// SCENES
// ============================================

void scene1() {
    if (!isRainEnabled) { gradSky(0.68f, 0.88f, 0.99f, 0.88f, 0.96f, 1.00f); sun(1020.0f, 620.0f); cloud(210.0f, 620.0f); cloud(520.0f, 650.0f); }
    else { gradSky(0.24f, 0.26f, 0.29f, 0.36f, 0.38f, 0.41f); cloud(220.0f, 632.0f); cloud(620.0f, 654.0f); }
    homeGround(false);
    tree(88, 120);
    tree(840, 132);
    tree(948, 124);
    house(false);
    drawCar(scene1_carPosX, scene1_carPosY, 0.84f, 0.20f, 0.18f, wheelRotationAngle, false, false, true);
    drawGarageDoor();
}

void scene2() {
    if (!isRainEnabled) {
        gradSky(0.68f, 0.88f, 0.99f, 0.88f, 0.96f, 1.00f);
        sun(1100, 620);
        cloud(180.0f, 600.0f);
        cloud(500.0f, 640.0f);
        cloud(850.0f, 610.0f);
    }
    else { gradSky(0.24f, 0.26f, 0.29f, 0.36f, 0.38f, 0.41f); cloud(180, 632); cloud(500, 654); }
    highwayBackground(false);
    road();
    trafficLight(100, 150, true);
    trafficLight(1200, 150, false);
    for (int i = 0; i < 8; i++) {
        drawCar(
            scene2_aiCarsX[i], scene2_aiCarsY[i],
            scene2_aiCarColors[i][0], scene2_aiCarColors[i][1], scene2_aiCarColors[i][2],
            wheelRotationAngle, false, false, true
        );
    }
    drawCar(scene2_redCarPosX, scene2_redCarPosY, 0.84f, 0.20f, 0.18f, wheelRotationAngle, false, false, true);
}

void scene3() {
    gradSky(0.68f, 0.88f, 0.99f, 0.88f, 0.96f, 1.00f);
    sun(900, 600);
    cloud(300, 620);
    tree(80, 155);
    tree(1100, 160);
    officeBuildingExterior(false);
    road();
    undergroundEntrance();
    float s = 1.0f - 0.3f * scene3_rampProgress;
    if (s > 0.1f) drawCarScaled(scene3_carPosX, scene3_carPosY, 0.84f, 0.20f, 0.18f, wheelRotationAngle, false, false, true, s);
}

void scene4() {
    officeRoom();
    wallClock(60, 590);
    calendarSheet(50, 485);
    pictureFrame(190, 485, 55, 65);
    noticeBoard(620, 485, 70, 65);
    officePlant(90, 80);
    officePlant(1050, 80);
    officeDesk(220, 80);
    officeChair(275, 80);
    computerMonitor(240, 126, true);
    keyboard(305, 125);
    mouseDesk(365, 130);
    coffeeMug(330, 120);
    officeDesk(550, 80);
    computerMonitor(570, 126, true);
    keyboard(635, 125);
    mouseDesk(695, 130);
    backgroundWorker(scene4_bgWorker1X, 80);
    officeDesk(900, 80);
    computerMonitor(920, 126, true);
    keyboard(985, 125);
    mouseDesk(1045, 130);
    backgroundWorker(scene4_bgWorker2X, 80);
    ceilingFan(300, 620, scene4_fanAngle1);
    ceilingFan(640, 620, scene4_fanAngle2);
    ceilingFan(980, 620, scene4_fanAngle3);
    workerCharacter(scene4_workerPosX, scene4_workerPosY, scene4_isWorkerSeated);
}

void scene5() {
    breakRoom();
    wallClock(60, 560);
    calendarSheet(50, 460);
    officePlant(500, 80);
    officeDesk(230, 80);
    officeChair(285, 80);
    computerMonitor(250, 126, true);
    coffeeMug(310, 120);
    coffeeMachine(870, 130);
    coffeeCup(890, 85, scene5_coffeeLevel);
    fruitBowl(1020, 110);
    snackDispenser(1060, 125);
    waterCooler(1130, 90);
    workerCharacter(scene5_workerPosX, scene5_workerPosY, false);
}

void scene6() {
    presentationRoom();
    wallClock(60, 600);
    pictureFrame(1120, 540, 65, 65);
    calendarSheet(1130, 440);
    presentationBoard(100, 260);
    slideContent(scene6_currentSlide);
    for (int i = 0; i < 7; i++) audienceMember(600 + i * 70, 180, i);
    for (int i = 0; i < 6; i++) audienceMember(630 + i * 65, 120, i + 2);
    workerCharacter(scene6_presenterPosX, scene6_presenterPosY, false);
}

void scene7() {
    gradSky(0.85f, 0.55f, 0.30f, 0.42f, 0.30f, 0.55f);
    sun(100, 200);
    cloud(400, 550);
    tree(80, 155);
    tree(1100, 160);
    officeBuildingExterior(true);
    road();
    undergroundEntrance();
    float s = 1.0f - 0.3f * scene7_rampProgress;
    if (s > 0.1f) drawCarScaled(scene7_carPosX, scene7_carPosY, 0.84f, 0.20f, 0.18f, wheelRotationAngle, true, true, true, s);
}

void scene8() {
    gradSky(0.05f, 0.08f, 0.18f, 0.11f, 0.16f, 0.28f);
    stars();
    moon(1050, 630);
    highwayBackground(true);
    roadNight();
    trafficLight(100, 150, false);
    trafficLight(1200, 150, true);
    for (int i = 0; i < 5; i++) {
        drawCar(
            scene8_aiCarsX[i], scene8_aiCarsY[i],
            scene8_aiCarColors[i][0], scene8_aiCarColors[i][1], scene8_aiCarColors[i][2],
            wheelRotationAngle, true, true, true
        );
    }
    drawCar(scene8_redCarPosX, scene8_redCarPosY, 0.84f, 0.20f, 0.18f, wheelRotationAngle, true, true, true);
    if (isRainEnabled) drawRain();
}

void scene9() {
    gradSky(0.05f, 0.08f, 0.18f, 0.11f, 0.16f, 0.28f);
    stars();
    moon(1030, 620);
    homeGround(true);
    tree(88, 120);
    tree(840, 132);
    tree(948, 124);
    house(isHouseLightOn);
    bool headOn = (carState_scene9 != 2);
    drawCar(scene9_carPosX, scene9_carPosY, 0.84f, 0.20f, 0.18f, wheelRotationAngle, headOn, true, true);
    drawHeadlightCone(scene9_carPosX, scene9_carPosY, headOn);
    drawGarageDoor();
}

// ============================================
// FORWARD DECLARATIONS
// ============================================
void nextScene();
void resetScene(int idx);

// ============================================
// ANIMATION
// ============================================

void anim1() {
    if (carState_scene1 == 0) carState_scene1 = 1;
    else if (carState_scene1 == 1) {
        garageDoorOpenAmount += 1.5f * animationSpeed;
        if (garageDoorOpenAmount >= homeGarageDoorHeight - 5.0f) {
            garageDoorOpenAmount = homeGarageDoorHeight - 5.0f;
            carState_scene1 = 2;
        }
    }
    else if (carState_scene1 == 2) {
        float sp = 2.0f * animationSpeed;
        scene1_carPosY -= sp;
        wheelRotationAngle -= sp * 2.0f;
        if (scene1_carPosY <= 50.0f) {
            scene1_carPosY = 50.0f;
            carState_scene1 = 3;
        }
    }
    else if (carState_scene1 == 3) {
        float sp = 3.0f * animationSpeed;
        scene1_carPosX += sp;
        wheelRotationAngle -= sp * 2.0f;
        if (garageDoorOpenAmount > 0) {
            garageDoorOpenAmount -= 1.0f * animationSpeed;
            if (garageDoorOpenAmount < 0) garageDoorOpenAmount = 0;
        }
        if (scene1_carPosX > 1350) {
            scene1HasCarExitedScreen = true;
            nextScene();
        }
    }
}

void anim2() {
    if (carState_scene2 == 0) {
        scene2_redCarPosX = -100;
        scene2_redCarPosY = 55;
        float lY[3] = {18, 55, 92};
        float sp[3][8] = {
            {0.8f, 1.0f, 0.6f, 1.2f, 0.9f, 2.0f, 1.5f, 0.7f},
            {1.5f, 2.0f, 1.8f, 2.5f, 1.2f, 1.0f, 2.2f, 1.6f},
            {0.5f, 1.5f, 0.3f, 1.0f, 0.8f, 2.0f, 1.3f, 0.6f}
        };
        scene2_aiCarColors[0][0] = 0.18f;
        scene2_aiCarColors[0][1] = 0.25f;
        scene2_aiCarColors[0][2] = 0.72f;
        scene2_aiCarColors[1][0] = 0.92f;
        scene2_aiCarColors[1][1] = 0.92f;
        scene2_aiCarColors[1][2] = 0.94f;
        scene2_aiCarColors[2][0] = 0.15f;
        scene2_aiCarColors[2][1] = 0.55f;
        scene2_aiCarColors[2][2] = 0.20f;
        scene2_aiCarColors[3][0] = 0.72f;
        scene2_aiCarColors[3][1] = 0.72f;
        scene2_aiCarColors[3][2] = 0.76f;
        scene2_aiCarColors[4][0] = 0.60f;
        scene2_aiCarColors[4][1] = 0.15f;
        scene2_aiCarColors[4][2] = 0.12f;
        scene2_aiCarColors[5][0] = 0.18f;
        scene2_aiCarColors[5][1] = 0.25f;
        scene2_aiCarColors[5][2] = 0.72f;
        scene2_aiCarColors[6][0] = 0.15f;
        scene2_aiCarColors[6][1] = 0.55f;
        scene2_aiCarColors[6][2] = 0.20f;
        scene2_aiCarColors[7][0] = 0.72f;
        scene2_aiCarColors[7][1] = 0.72f;
        scene2_aiCarColors[7][2] = 0.76f;
        int ln[8] = {0, 2, 1, 0, 2, 1, 0, 2};
        float p[8] = {200, 450, 700, 950, 1200, 350, 650, 900};
        for (int i = 0; i < 8; i++) {
            scene2_aiCarLanes[i] = ln[i];
            scene2_aiCarsY[i] = lY[ln[i]];
            scene2_aiCarsX[i] = p[i];
            scene2_aiCarSpeeds[i] = sp[ln[i]][i];
        }
        carState_scene2 = 1;
    } else if (carState_scene2 == 1) {
        float sp = 3.0f * animationSpeed;
        scene2_redCarPosX += sp;
        wheelRotationAngle -= sp * 2.0f;
        for (int i = 0; i < 8; i++) {
            scene2_aiCarsX[i] += scene2_aiCarSpeeds[i] * animationSpeed;
            if (scene2_aiCarsX[i] > 1380) scene2_aiCarsX[i] = -150;
        }
        if (scene2_redCarPosX > 350) carState_scene2 = 2;
    } else if (carState_scene2 == 2) {
        float sp = 4.5f * animationSpeed;
        scene2_redCarPosX += sp;
        wheelRotationAngle -= sp * 2.0f;
        for (int i = 0; i < 8; i++) {
            scene2_aiCarsX[i] += scene2_aiCarSpeeds[i] * animationSpeed;
            if (scene2_aiCarsX[i] > 1380) scene2_aiCarsX[i] = -150;
        }
        if (scene2_redCarPosX > 1380) {
            carState_scene2 = 3;
            nextScene();
        }
    }
}

void anim3() {
    float sX = 640, sY = 50, eX = 700, eY = -40;
    if (carState_scene3 == 0) {
        scene3_carPosX = -100;
        scene3_carPosY = 50;
        scene3_rampProgress = 0;
        carState_scene3 = 1;
    }
    else if (carState_scene3 == 1) {
        float sp = 3.5f * animationSpeed;
        scene3_carPosX += sp;
        wheelRotationAngle -= sp * 2.0f;
        if (scene3_carPosX >= sX) {
            scene3_carPosX = sX;
            carState_scene3 = 2;
        }
    }
    else if (carState_scene3 == 2) {
        scene3_rampProgress += 0.012f * animationSpeed;
        scene3_carPosX = sX + (eX - sX) * scene3_rampProgress;
        scene3_carPosY = sY + (eY - sY) * scene3_rampProgress;
        wheelRotationAngle -= 2.0f * animationSpeed;
        if (scene3_rampProgress >= 1.0f) {
            carState_scene3 = 3;
            nextScene();
        }
    }
}

void anim4() {
    if (carState_scene4 == 0) {
        scene4_workerPosX = 1100;
        scene4_workerPosY = 80;
        scene4_isWorkerSeated = false;
        scene4_ambientTimer = 0;
        scene4_fanAngle1 = 0;
        scene4_fanAngle2 = 60;
        scene4_fanAngle3 = 120;
        scene4_bgWorker1X = 550;
        scene4_bgWorker2X = 900;
        carState_scene4 = 1;
    }
    else if (carState_scene4 == 1) {
        scene4_workerPosX -= 2.5f * animationSpeed;
        if (scene4_workerPosX <= 280) {
            scene4_workerPosX = 280;
            carState_scene4 = 2;
        }
    }
    else if (carState_scene4 == 2) { scene4_isWorkerSeated = true; carState_scene4 = 3; }
    else if (carState_scene4 == 3) {
        scene4_fanAngle1 += 2.0f * animationSpeed;
        scene4_fanAngle2 += 2.0f * animationSpeed;
        scene4_fanAngle3 += 2.0f * animationSpeed;
        scene4_bgWorker1X += 0.5f * scene4_bgWorker1Dir * animationSpeed;
        if (scene4_bgWorker1X > 600) scene4_bgWorker1Dir = -1;
        if (scene4_bgWorker1X < 500) scene4_bgWorker1Dir = 1;
        scene4_bgWorker2X += 0.4f * scene4_bgWorker2Dir * animationSpeed;
        if (scene4_bgWorker2X > 950) scene4_bgWorker2Dir = -1;
        if (scene4_bgWorker2X < 850) scene4_bgWorker2Dir = 1;
        scene4_ambientTimer++;
        if (scene4_ambientTimer >= 300) { carState_scene4 = 4; nextScene(); }
    }
}

void anim5() {
    if (carState_scene5 == 0) {
        scene5_workerPosX = 280;
        scene5_workerPosY = 80;
        scene5_coffeeLevel = 0;
        scene5_coffeePoured = false;
        scene5_pauseTimer = 0;
        carState_scene5 = 1;
    }
    else if (carState_scene5 == 1) {
        scene5_workerPosX += 2.0f * animationSpeed;
        if (scene5_workerPosX >= 840) {
            scene5_workerPosX = 840;
            carState_scene5 = 2;
        }
    }
    else if (carState_scene5 == 2) {
        scene5_pauseTimer++;
        if (scene5_pauseTimer >= 20) {
            scene5_pauseTimer = 0;
            carState_scene5 = 3;
        }
    }
    else if (carState_scene5 == 3) {
        scene5_coffeeLevel += 0.012f * animationSpeed;
        if (scene5_coffeeLevel >= 1.0f) {
            scene5_coffeeLevel = 1.0f;
            scene5_coffeePoured = true;
            carState_scene5 = 4;
        }
    }
    else if (carState_scene5 == 4) {
        scene5_pauseTimer++;
        if (scene5_pauseTimer >= 60) {
            scene5_pauseTimer = 0;
            carState_scene5 = 5;
        }
    }
    else if (carState_scene5 == 5) {
        scene5_workerPosX += 2.5f * animationSpeed;
        if (scene5_workerPosX > 1350) {
            carState_scene5 = 6;
            nextScene();
        }
    }
}

void anim6() {
    if (carState_scene6 == 0) {
        scene6_presenterPosX = 1100;
        scene6_presenterPosY = 80;
        scene6_currentSlide = 0;
        scene6_slideTimer = 0;
        carState_scene6 = 1;
    }
    else if (carState_scene6 == 1) {
        scene6_presenterPosX -= 2.0f * animationSpeed;
        if (scene6_presenterPosX <= 340) {
            scene6_presenterPosX = 340;
            carState_scene6 = 2;
        }
    }
    else if (carState_scene6 == 2) {
        scene6_currentSlide = 1;
        scene6_slideTimer++;
        if (scene6_slideTimer >= 60) {
            scene6_slideTimer = 0;
            carState_scene6 = 3;
        }
    }
    else if (carState_scene6 == 3) {
        scene6_currentSlide = 2;
        scene6_slideTimer++;
        if (scene6_slideTimer >= 90) {
            scene6_slideTimer = 0;
            carState_scene6 = 4;
        }
    }
    else if (carState_scene6 == 4) {
        scene6_currentSlide = 3;
        scene6_slideTimer++;
        if (scene6_slideTimer >= 90) {
            scene6_slideTimer = 0;
            carState_scene6 = 5;
        }
    }
    else if (carState_scene6 == 5) {
        scene6_currentSlide = 4;
        scene6_slideTimer++;
        if (scene6_slideTimer >= 60) {
            scene6_slideTimer = 0;
            carState_scene6 = 6;
        }
    }
    else if (carState_scene6 == 6) {
        scene6_slideTimer++;
        if (scene6_slideTimer >= 40) {
            scene6_slideTimer = 0;
            carState_scene6 = 7;
        }
    }
    else if (carState_scene6 == 7) {
        scene6_presenterPosX += 3.0f * animationSpeed;
        if (scene6_presenterPosX > 1350) {
            carState_scene6 = 8;
            nextScene();
        }
    }
}

void anim7() {
    float sX = 640, sY = 50, eX = 700, eY = -40;
    if (carState_scene7 == 0) {
        scene7_carPosX = eX;
        scene7_carPosY = eY;
        scene7_rampProgress = 1.0f;
        carState_scene7 = 1;
    }
    else if (carState_scene7 == 1) {
        scene7_rampProgress -= 0.012f * animationSpeed;
        scene7_carPosX = sX + (eX - sX) * scene7_rampProgress;
        scene7_carPosY = sY + (eY - sY) * scene7_rampProgress;
        wheelRotationAngle -= 2.0f * animationSpeed;
        if (scene7_rampProgress <= 0) {
            scene7_rampProgress = 0;
            scene7_carPosX = sX;
            scene7_carPosY = sY;
            carState_scene7 = 2;
        }
    }
    else if (carState_scene7 == 2) {
        float sp = 3.5f * animationSpeed;
        scene7_carPosX += sp;
        wheelRotationAngle -= sp * 2.0f;
        if (scene7_carPosX > 1380) {
            carState_scene7 = 3;
            nextScene();
        }
    }
}

void anim8() {
    if (carState_scene8 == 0) {
        scene8_redCarPosX = -100;
        scene8_redCarPosY = 55;
        float lY[3] = {18, 55, 92};
        scene8_aiCarColors[0][0] = 0.15f;
        scene8_aiCarColors[0][1] = 0.20f;
        scene8_aiCarColors[0][2] = 0.62f;
        scene8_aiCarColors[1][0] = 0.70f;
        scene8_aiCarColors[1][1] = 0.70f;
        scene8_aiCarColors[1][2] = 0.74f;
        scene8_aiCarColors[2][0] = 0.62f;
        scene8_aiCarColors[2][1] = 0.62f;
        scene8_aiCarColors[2][2] = 0.65f;
        scene8_aiCarColors[3][0] = 0.92f;
        scene8_aiCarColors[3][1] = 0.92f;
        scene8_aiCarColors[3][2] = 0.94f;
        scene8_aiCarColors[4][0] = 0.15f;
        scene8_aiCarColors[4][1] = 0.50f;
        scene8_aiCarColors[4][2] = 0.20f;
        int ln[5] = {0, 1, 2, 0, 1};
        float p[5] = {300, 700, 400, 1000, 550};
        float sp[5] = {1.0f, 1.8f, 1.4f, 0.7f, 2.0f};
        for (int i = 0; i < 5; i++) { scene8_aiCarLanes[i] = ln[i]; scene8_aiCarsY[i] = lY[ln[i]]; scene8_aiCarsX[i] = p[i]; scene8_aiCarSpeeds[i] = sp[i]; }
        carState_scene8 = 1;
    } else if (carState_scene8 == 1) {
        float sp = 3.5f * animationSpeed;
        scene8_redCarPosX += sp;
        wheelRotationAngle -= sp * 2.0f;
        for (int i = 0; i < 5; i++) {
            scene8_aiCarsX[i] += scene8_aiCarSpeeds[i] * animationSpeed;
            if (scene8_aiCarsX[i] > 1380) scene8_aiCarsX[i] = -150;
        }
        if (scene8_redCarPosX > 1380) {
            carState_scene8 = 2;
            nextScene();
        }
    }
}

void anim9() {
    if (carState_scene9 == 0) {
        float sp = 3.0f * animationSpeed;
        scene9_carPosX += sp;
        wheelRotationAngle -= sp * 2.0f;
        if (scene9_carPosX >= 296) {
            scene9_carPosX = 296;
            carState_scene9 = 1;
        }
    }
    else if (carState_scene9 == 1) {
        float up = 2.0f * animationSpeed;
        scene9_carPosY += up;
        wheelRotationAngle -= up * 1.8f;
        if (scene9_carPosY >= 330) {
            scene9_carPosY = 330;
            carState_scene9 = 2;
            scene9ParkingCompleted = true;
            isHouseLightOn = true;
        }
    }
    else if (carState_scene9 == 2) {
        scene9ParkedFrameCounter++;
        if (garageDoorOpenAmount > 0) {
            garageDoorOpenAmount -= 1.0f * animationSpeed;
            if (garageDoorOpenAmount <= 0) {
                garageDoorOpenAmount = 0;
                isPaused = true;
                currentScene = 1;
                resetScene(1);
            }
        }
    }
}

// ============================================
// PIPELINE
// ============================================
void init() {
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    glOrtho(0, WINDOW_WIDTH, 0, WINDOW_HEIGHT, -1, 1);
    glMatrixMode(GL_MODELVIEW);
    initRain();
}

void resetScene(int idx) {
    if (idx == 1) {
        scene1_carPosX = 296;
        scene1_carPosY = 330;
        wheelRotationAngle = 0;
        garageDoorOpenAmount = 0;
        carState_scene1 = 0;
        scene1HasCarExitedScreen = false;
    }
    else if (idx == 2) {
        scene2_redCarPosX = -100;
        scene2_redCarPosY = 55;
        wheelRotationAngle = 0;
        carState_scene2 = 0;
    }
    else if (idx == 3) {
        scene3_carPosX = -100;
        scene3_carPosY = 50;
        scene3_rampProgress = 0;
        wheelRotationAngle = 0;
        carState_scene3 = 0;
    }
    else if (idx == 4) {
        scene4_workerPosX = 1100;
        scene4_workerPosY = 80;
        scene4_isWorkerSeated = false;
        scene4_fanAngle1 = 0;
        scene4_fanAngle2 = 60;
        scene4_fanAngle3 = 120;
        scene4_bgWorker1X = 550;
        scene4_bgWorker2X = 900;
        scene4_bgWorker1Dir = 1;
        scene4_bgWorker2Dir = -1;
        scene4_ambientTimer = 0;
        carState_scene4 = 0;
    }
    else if (idx == 5) {
        scene5_workerPosX = 280;
        scene5_workerPosY = 80;
        scene5_coffeeLevel = 0;
        scene5_coffeePoured = false;
        scene5_pauseTimer = 0;
        carState_scene5 = 0;
    }
    else if (idx == 6) {
        scene6_presenterPosX = 1100;
        scene6_presenterPosY = 80;
        scene6_currentSlide = 0;
        scene6_slideTimer = 0;
        carState_scene6 = 0;
    }
    else if (idx == 7) {
        scene7_carPosX = 700;
        scene7_carPosY = -40;
        scene7_rampProgress = 1.0f;
        wheelRotationAngle = 0;
        carState_scene7 = 0;
    }
    else if (idx == 8) {
        scene8_redCarPosX = -100;
        scene8_redCarPosY = 55;
        wheelRotationAngle = 0;
        carState_scene8 = 0;
    }
    else if (idx == 9) {
        scene9_carPosX = -100;
        scene9_carPosY = 50;
        wheelRotationAngle = 0;
        carState_scene9 = 0;
        scene9ParkingCompleted = false;
        isHouseLightOn = false;
        garageDoorOpenAmount = homeGarageDoorHeight - 5.0f;
    }
}

void nextScene() {
    currentScene++;
    if (currentScene > LAST_SCENE_INDEX) currentScene = FIRST_SCENE_INDEX;
    sceneFrameCounter = 0;
    resetScene(currentScene);
}

void display() {
    if (currentScene <= 3) glClearColor(0.62f, 0.84f, 0.97f, 1.0f);
    else if (currentScene <= 5) glClearColor(0.86f, 0.84f, 0.80f, 1.0f);
    else if (currentScene == 6) glClearColor(0.84f, 0.82f, 0.78f, 1.0f);
    else if (currentScene == 7) glClearColor(0.50f, 0.38f, 0.26f, 1.0f);
    else glClearColor(0.05f, 0.07f, 0.15f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT);
    glLoadIdentity();
    if (currentScene == 1) scene1();
    else if (currentScene == 2) scene2();
    else if (currentScene == 3) scene3();
    else if (currentScene == 4) scene4();
    else if (currentScene == 5) scene5();
    else if (currentScene == 6) scene6();
    else if (currentScene == 7) scene7();
    else if (currentScene == 8) scene8();
    else if (currentScene == 9) scene9();
    if (isRainEnabled && currentScene != 4 && currentScene != 5 && currentScene != 6) drawRain();
    glutSwapBuffers();
}

void update(int val) {
    if (!isPaused) {
        if (currentScene == 1) anim1();
        else if (currentScene == 2) anim2();
        else if (currentScene == 3) anim3();
        else if (currentScene == 4) anim4();
        else if (currentScene == 5) anim5();
        else if (currentScene == 6) anim6();
        else if (currentScene == 7) anim7();
        else if (currentScene == 8) anim8();
        else if (currentScene == 9) anim9();
        if (isRainEnabled) {
            for (int i = 0; i < RAIN_DROP_COUNT; i++) {
                rainDropY[i] -= 12.0f;
                rainDropX[i] -= 2.0f;
                if (rainDropY[i] < 0) {
                    rainDropY[i] = WINDOW_HEIGHT;
                    rainDropX[i] = rand() % WINDOW_WIDTH;
                }
            }
        }
    }
    glutPostRedisplay();
    glutTimerFunc(TIMER_INTERVAL_MS, update, 0);
}

void keyboard(unsigned char key, int x, int y) {
    if (key == 27) exit(0);
    if (key == 'r' || key == 'R') isRainEnabled = true;
    if (key == 'v' || key == 'V') isRainEnabled = false;
    if (key == ' ') isPaused = !isPaused;
    if (key == 'n' || key == 'N') {
        currentScene = 1;
        for (int i = 1; i <= 9; i++) resetScene(i);
        isPaused = false;
    }
}

void specialKeys(int key, int x, int y) {
    if (key == GLUT_KEY_LEFT) {
        currentScene--;
        if (currentScene < FIRST_SCENE_INDEX) currentScene = LAST_SCENE_INDEX;
        resetScene(currentScene);
    }
    if (key == GLUT_KEY_RIGHT) nextScene();
}

int main(int argc, char ** argv) {
    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGBA);
    glutInitWindowSize(WINDOW_WIDTH, WINDOW_HEIGHT);
    glutCreateWindow("Life of an Office Worker - OpenGL Animation");
    init();
    resetScene(currentScene);
    glutDisplayFunc(display);
    glutTimerFunc(TIMER_INTERVAL_MS, update, 0);
    glutKeyboardFunc(keyboard);
    glutSpecialFunc(specialKeys);
    glutMainLoop();
    return 0;
}