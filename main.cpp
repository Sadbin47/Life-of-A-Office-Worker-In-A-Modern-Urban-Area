#ifdef __APPLE__
#include <GLUT/glut.h>
#else
#include <GL/glut.h>
#endif

#include <cmath>
#include <cstdio>
#include <cstdlib>

const int WINDOW_WIDTH = 1280;
const int WINDOW_HEIGHT = 720;
const float TARGET_ASPECT = 16.0f / 9.0f;
const float PI_VALUE = 3.14159265358979323846f;
const int TIMER_INTERVAL_MS = 16;
const int FIRST_SCENE_INDEX = 1;
const int LAST_SCENE_INDEX = 9;
const int sceneDurationFrameCount[10] = {0,420,390,360,420,390,360,360,420,430};
const int FADE_START_BEFORE_END_FRAMES = 55;
const float FADE_ALPHA_STEP = 0.03f;

int currentScene = 1;
int sceneFrameCounter = 0;
int totalFrameCounter = 0;
bool isPaused = false;
bool isRainEnabled = false;
bool showTextOverlay = false;
bool isSceneTransitionActive = false;
bool isFadeOutPhase = false;
float fadeOverlayAlpha = 0.0f;

float cloudOffsetX_layerA = 0.0f;
float cloudOffsetX_layerB = 0.0f;
float sunHorizontalOffset = 0.0f;
float starTwinkleCounter = 0.0f;

const int STAR_COUNT = 34;
const float starPositionX[STAR_COUNT] = {
     40,90,145,210,275,330,390,455,520,585,650,715,770,830,885,
    950,1010,1070,1130,1190,1245,65,180,305,420,560,690,815,940,1065,
    1185,250,745,980
};
const float starPositionY[STAR_COUNT] = {
    640,680,620,700,655,610,685,640,705,625,690,650,710,635,675,
    618,698,645,708,630,695,605,665,618,700,646,710,622,690,640,
    705,680,635,670
};

float roadBottomY_current = 120.0f;
float roadHeight_current = 180.0f;
int roadLaneCount_current = 3;
bool roadNightMode_current = false;
bool buildingWindowNightMode = false;

float characterLeftArmAngleDegrees = 0.0f;
float characterRightArmAngleDegrees = 0.0f;
float characterLegSwingAngleDegrees = 0.0f;
bool characterFacingRight = true;
bool characterCarryBriefcase = false;
bool characterSittingPose = false;
bool characterPointingPose = false;

float scene1_carPosX = 392.0f;
float scene1_carPosY = 318.0f;
float wheelRotationAngle = 0.0f;
float animationSpeed = 1.0f;
int carState_scene1 = 0;
bool scene1HasCarExitedScreen = false;

const int RAIN_DROP_COUNT = 130;
float rainDropX[RAIN_DROP_COUNT], rainDropY[RAIN_DROP_COUNT];
const float scene1GarageStartX = 392.0f;
const float scene1GarageStartY = 318.0f;
const float scene1RoadTravelY = 72.0f;
const float scene1ExitCheckX = 1280.0f;

float trafficCarPositionX_scene2_A = -180.0f;
float trafficCarPositionX_scene2_B = 1350.0f;
float trafficCarPositionX_scene2_C = -420.0f;
float trafficCarWheelAngle_scene2_A = 0.0f;
float trafficCarWheelAngle_scene2_B = 0.0f;
float trafficCarWheelAngle_scene2_C = 0.0f;
float parallaxOffset_scene2 = 0.0f;
float roadDashOffset_scene2 = 0.0f;

float carPositionX_scene3 = -220.0f;
float carPositionY_scene3 = 286.0f;
float wheelRotationAngle_scene3 = 0.0f;
float barrierRotationAngle_scene3 = 0.0f;

float typingArmAngle_scene4 = 0.0f;
float wallClockHandAngle_scene4 = 90.0f;
float workerWalkPositionX_scene5 = 250.0f;
float workerWalkLegSwing_scene5 = 0.0f;
float presentationBarGrowRatio_scene6 = 0.0f;
float pointerOscillationAngle_scene6 = 0.0f;

float carPositionX_scene7 = 520.0f;
float carPositionY_scene7 = 170.0f;
float wheelRotationAngle_scene7 = 0.0f;
float barrierRotationAngle_scene7 = 0.0f;

float trafficCarPositionX_scene8_A = -260.0f;
float trafficCarPositionX_scene8_B = 1380.0f;
float trafficCarPositionX_scene8_C = -560.0f;
float trafficCarWheelAngle_scene8_A = 0.0f;
float trafficCarWheelAngle_scene8_B = 0.0f;
float trafficCarWheelAngle_scene8_C = 0.0f;
float parallaxOffset_scene8 = 0.0f;

float scene9_carPosX = -220.0f;
float scene9_carPosY = 72.0f;
int carState_scene9 = 0;
int scene9ParkedFrameCounter = 0;
bool scene9ParkingCompleted = false;
bool isHouseLightOn = false;

const float homeRoadBottomY = 0.0f;
const float homeRoadHeight = 110.0f;
const float homeGrassBottomY = 110.0f;
const float homeGrassHeight = 190.0f;
const float homeHouseLeftX = 140.0f;
const float homeHouseBottomY = 300.0f;
const float homeGarageLeftX = 280.0f;
const float homeGarageBottomY = 300.0f;
const float homeGarageDoorLeftX = 312.0f;
const float homeGarageDoorBottomY = 300.0f;
const float homeGarageDoorWidth = 176.0f;
const float homeGarageDoorHeight = 126.0f;

struct SteamParticle { float x, y, speed, alpha; };
const int STEAM_PARTICLE_COUNT = 20;
SteamParticle coffeeSteamParticles[STEAM_PARTICLE_COUNT];

float clamp(float v, float mn, float mx) { return v < mn ? mn : (v > mx ? mx : v); }

float wrap(float v, float mn, float mx) {
    float s = mx - mn;
    if (s <= 0) return mn;
    while (v < mn) v += s;
    while (v > mx) v -= s;
    return v;
}

void rect(float x, float y, float w, float h) {
    glBegin(GL_QUADS);
    glVertex2f(x,y); glVertex2f(x+w,y); glVertex2f(x+w,y+h); glVertex2f(x,y+h);
    glEnd();
}

void circle(float cx, float cy, float r, int seg) {
    if (seg < 12) seg = 12;
    glBegin(GL_POLYGON);
    for (int i = 0; i < seg; i++) {
        float a = 2.0f * PI_VALUE * i / seg;
        glVertex2f(cx + cos(a)*r, cy + sin(a)*r);
    }
    glEnd();
}

void ellipse(float cx, float cy, float rx, float ry, int seg) {
    if (seg < 12) seg = 12;
    glBegin(GL_POLYGON);
    for (int i = 0; i < seg; i++) {
        float a = 2.0f * PI_VALUE * i / seg;
        glVertex2f(cx + cos(a)*rx, cy + sin(a)*ry);
    }
    glEnd();
}

void gradSky(float br, float bg, float bb, float tr, float tg, float tb) {
    glBegin(GL_QUADS);
    glColor3f(br,bg,bb); glVertex2f(0,0);
    glColor3f(br,bg,bb); glVertex2f(1280,0);
    glColor3f(tr,tg,tb); glVertex2f(1280,720);
    glColor3f(tr,tg,tb); glVertex2f(0,720);
    glEnd();
}

void bmpText(float x, float y, const char* s) {
    glRasterPos2f(x,y);
    for (const char* c = s; *c; ++c) glutBitmapCharacter(GLUT_BITMAP_HELVETICA_18, *c);
}

void wheel(float cx, float cy, float ang) {
    glPushMatrix();
    glTranslatef(cx,cy,0);
    glRotatef(ang,0,0,1);
    glColor3f(0.05f,0.05f,0.06f); circle(0,0,16,36);
    glColor3f(0.15f,0.15f,0.17f); circle(0,0,12,30);
    glColor3f(0.74f,0.74f,0.78f); circle(0,0,7,28);
    glColor3f(0.95f,0.95f,0.96f);
    glLineWidth(2);
    glBegin(GL_LINES);
    glVertex2f(-8,0); glVertex2f(8,0);
    glVertex2f(0,-8); glVertex2f(0,8);
    glVertex2f(-5.5f,-5.5f); glVertex2f(5.5f,5.5f);
    glVertex2f(-5.5f,5.5f); glVertex2f(5.5f,-5.5f);
    glEnd();
    glLineWidth(1);
    glPopMatrix();
}

void car(float x, float y, float r, float g, float b, float wa, bool hl, bool tl, bool fr) {
    glPushMatrix();
    glTranslatef(x,y,0);
    if (!fr) glScalef(-1,1,1);
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    glColor4f(0,0,0,0.24f); ellipse(0,-10,74,11,36);
    glColor3f(r,g,b); rect(-71,0,142,32);
    glBegin(GL_POLYGON);
    glVertex2f(-38,32); glVertex2f(42,32); glVertex2f(20,58); glVertex2f(-20,58);
    glEnd();
    glColor3f(0.64f,0.84f,0.96f);
    glBegin(GL_POLYGON);
    glVertex2f(-30,35); glVertex2f(34,35); glVertex2f(16,54); glVertex2f(-14,54);
    glEnd();
    glColor3f(0.18f,0.18f,0.20f);
    rect(64,8,10,14); rect(-74,8,10,14);
    wheel(-42,-2,wa); wheel(42,-2,wa);
    if (hl) {
        glColor4f(1,0.96f,0.42f,0.30f);
        glBegin(GL_TRIANGLES);
        glVertex2f(70,20); glVertex2f(186,58); glVertex2f(186,-10);
        glEnd();
        glColor3f(1,1,0.72f); rect(66,14,8,8);
    }
    if (tl) {
        glColor3f(0.95f,0.08f,0.08f); rect(-74,16,8,8); rect(-74,6,8,8);
    }
    glDisable(GL_BLEND);
    glPopMatrix();
}

void suv(float x, float y, float r, float g, float b, float wa, bool fr) {
    glPushMatrix();
    glTranslatef(x,y,0);
    if (!fr) glScalef(-1,1,1);
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    glColor4f(0,0,0,0.24f); ellipse(0,-12,86,12,36);
    glColor3f(r,g,b); rect(-82,0,164,42);
    glBegin(GL_POLYGON);
    glVertex2f(-48,42); glVertex2f(50,42); glVertex2f(35,70); glVertex2f(-30,72);
    glEnd();
    glColor3f(0.56f,0.78f,0.90f);
    rect(-38,46,32,18); rect(2,46,36,18);
    glColor3f(0.12f,0.13f,0.15f);
    rect(72,12,10,14); rect(-82,12,10,14);
    wheel(-52,-2,wa); wheel(52,-2,wa);
    glDisable(GL_BLEND);
    glPopMatrix();
}

void bus(float x, float y, float r, float g, float b, float wa, bool fr) {
    glPushMatrix();
    glTranslatef(x,y,0);
    if (!fr) glScalef(-1,1,1);
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    glColor4f(0,0,0,0.22f); ellipse(0,-12,112,13,36);
    glColor3f(r,g,b); rect(-110,0,220,64);
    glColor3f(0.78f,0.90f,0.96f);
    for (int i=0; i<5; i++) rect(-92 + i*34, 34, 26, 20);
    glColor3f(0.20f,0.20f,0.22f); rect(78,8,22,46);
    glColor3f(0.95f,0.72f,0.18f); rect(-110,18,220,7);
    wheel(-70,-2,wa); wheel(72,-2,wa);
    glDisable(GL_BLEND);
    glPopMatrix();
}

void deliveryVan(float x, float y, float r, float g, float b, float wa, bool fr) {
    glPushMatrix();
    glTranslatef(x,y,0);
    if (!fr) glScalef(-1,1,1);
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    glColor4f(0,0,0,0.23f); ellipse(0,-11,92,12,36);
    glColor3f(r,g,b); rect(-88,0,176,48);
    glBegin(GL_POLYGON);
    glVertex2f(20,48); glVertex2f(68,48); glVertex2f(86,28); glVertex2f(86,0); glVertex2f(20,0);
    glEnd();
    glColor3f(0.72f,0.86f,0.94f); rect(42,29,30,15);
    glColor3f(0.92f,0.92f,0.94f); rect(-72,18,66,18);
    glColor3f(0.16f,0.16f,0.18f); rect(80,10,8,16);
    wheel(-52,-2,wa); wheel(54,-2,wa);
    glDisable(GL_BLEND);
    glPopMatrix();
}

void cloud(float cx, float cy) {
    glColor3f(0.98f,0.98f,0.99f);
    circle(cx,cy,28,28);
    circle(cx-30,cy-4,22,24);
    circle(cx+30,cy-2,24,24);
    circle(cx+2,cy+18,20,24);
}

void sun(float cx, float cy) {
    glColor3f(1.0f,0.92f,0.32f); circle(cx,cy,36,36);
    glColor3f(1,0.84f,0.22f);
    glLineWidth(2);
    glBegin(GL_LINES);
    glVertex2f(cx-54,cy); glVertex2f(cx-38,cy);
    glVertex2f(cx+38,cy); glVertex2f(cx+54,cy);
    glVertex2f(cx,cy-54); glVertex2f(cx,cy-38);
    glVertex2f(cx,cy+38); glVertex2f(cx,cy+54);
    glEnd();
    glLineWidth(1);
}

void moon(float cx, float cy) {
    glColor3f(0.90f,0.92f,0.96f); circle(cx,cy,30,34);
    glColor3f(0.08f,0.10f,0.18f); circle(cx+10,cy+4,26,34);
}

void stars() {
    glPointSize(2.8f);
    glBegin(GL_POINTS);
    for (int i=0; i<STAR_COUNT; i++) {
        float w = 0.5f + 0.5f * sin(starTwinkleCounter + i*0.71f);
        float b = 0.35f + 0.65f * w;
        glColor3f(b,b,b*0.92f);
        glVertex2f(starPositionX[i], starPositionY[i]);
    }
    glEnd();
    glPointSize(1);
}

void road() {
    glColor3f(roadNightMode_current ? 0.10f : 0.23f, roadNightMode_current ? 0.10f : 0.23f, roadNightMode_current ? 0.12f : 0.25f);
    rect(0, roadBottomY_current, 1280, roadHeight_current);
    glColor3f(0.75f,0.75f,0.78f);
    rect(0, roadBottomY_current, 1280, 5);
    rect(0, roadBottomY_current+roadHeight_current-5, 1280, 5);
    float lh = roadHeight_current / roadLaneCount_current;
    glColor3f(roadNightMode_current ? 0.92f : 0.95f, roadNightMode_current ? 0.92f : 0.95f, roadNightMode_current ? 0.64f : 0.72f);
    for (int i=1; i<roadLaneCount_current; i++) {
        float y = roadBottomY_current + lh*i;
        for (float dx=20; dx<1280; dx+=76) rect(dx, y-2, 48, 4);
    }
}

void building(float px, float py, float w, float h) {
    float seed = fabs(sin(px*0.007f));
    float r = 0.22f + 0.16f*seed;
    float g = 0.24f + 0.14f*seed;
    float b = 0.28f + 0.12f*seed;
    glColor3f(r,g,b); rect(px, py, w, h);
    glColor3f(r*0.8f,g*0.8f,b*0.8f); rect(px, py+h, w, 10);
    int cols = (int)((w-22)/22);
    int rows = (int)((h-24)/25);
    for (int row=0; row<rows; row++) {
        for (int col=0; col<cols; col++) {
            float wx = px + 11 + col*22;
            float wy = py + 12 + row*25;
            if (buildingWindowNightMode) {
                float flick = 0.45f + 0.55f * (0.5f+0.5f*sin(starTwinkleCounter+row+col*0.5f));
                glColor3f(0.55f+0.45f*flick, 0.50f+0.35f*flick, 0.22f+0.18f*flick);
            } else {
                glColor3f(0.62f,0.78f,0.90f);
            }
            rect(wx, wy, 12, 15);
        }
    }
}

void glassTower(float x, float y, float w, float h, float slant, bool night) {
    glColor3f(night ? 0.09f : 0.35f, night ? 0.13f : 0.46f, night ? 0.20f : 0.54f);
    glBegin(GL_POLYGON);
    glVertex2f(x,y);
    glVertex2f(x+w,y);
    glVertex2f(x+w+slant,y+h);
    glVertex2f(x+slant*0.25f,y+h);
    glEnd();
    glColor3f(night ? 0.12f : 0.50f, night ? 0.22f : 0.74f, night ? 0.30f : 0.86f);
    glBegin(GL_POLYGON);
    glVertex2f(x+8,y+12);
    glVertex2f(x+w-10,y+12);
    glVertex2f(x+w+slant-14,y+h-16);
    glVertex2f(x+slant*0.25f+12,y+h-16);
    glEnd();
    glColor3f(night ? 0.05f : 0.16f, night ? 0.09f : 0.32f, night ? 0.15f : 0.40f);
    glLineWidth(1);
    glBegin(GL_LINES);
    for (float gx=x+28; gx<x+w; gx+=34) {
        glVertex2f(gx,y+12);
        glVertex2f(gx+slant*0.75f,y+h-16);
    }
    for (float gy=y+42; gy<y+h-20; gy+=36) {
        glVertex2f(x+8,gy);
        glVertex2f(x+w+slant-14,gy);
    }
    glEnd();
}

void apartmentBlock(float x, float y, float w, float h, bool night) {
    glColor3f(night ? 0.12f : 0.50f, night ? 0.13f : 0.52f, night ? 0.16f : 0.55f);
    rect(x,y,w,h);
    glColor3f(night ? 0.09f : 0.40f, night ? 0.10f : 0.42f, night ? 0.13f : 0.46f);
    rect(x,y+h-12,w,12);
    for (float by=y+24; by<y+h-28; by+=42) {
        glColor3f(night ? 0.20f : 0.78f, night ? 0.22f : 0.82f, night ? 0.28f : 0.84f);
        rect(x+12,by,w-24,4);
        for (float wx=x+18; wx<x+w-24; wx+=28) {
            if (night) glColor3f(0.95f,0.76f,0.34f);
            else glColor3f(0.62f,0.78f,0.88f);
            rect(wx,by+10,14,18);
        }
    }
}

void steppedOfficeBlock(float x, float y, float w, float h, bool night) {
    glColor3f(night ? 0.10f : 0.42f, night ? 0.12f : 0.45f, night ? 0.16f : 0.50f);
    rect(x,y,w,h*0.78f);
    rect(x+w*0.18f,y+h*0.78f,w*0.64f,h*0.22f);
    glColor3f(night ? 0.13f : 0.58f, night ? 0.22f : 0.76f, night ? 0.30f : 0.86f);
    for (float row=y+22; row<y+h-22; row+=34) {
        rect(x+18,row,w-36,12);
    }
    glColor3f(night ? 0.06f : 0.26f, night ? 0.08f : 0.36f, night ? 0.10f : 0.42f);
    for (float gx=x+42; gx<x+w-20; gx+=45) rect(gx,y+12,2,h-24);
}

void skylineTrees(float baseY, bool night) {
    glColor3f(night ? 0.08f : 0.18f, night ? 0.20f : 0.42f, night ? 0.10f : 0.20f);
    for (float x=22; x<1280; x+=68) {
        ellipse(x, baseY+8, 28, 12, 18);
        ellipse(x+20, baseY+12, 24, 15, 18);
        ellipse(x-18, baseY+11, 22, 14, 18);
    }
}

void parallaxCity(float off, bool night) {
    float farLayerOffset = off * 0.28f;
    float nearLayerOffset = off * 0.62f;
    glColor3f(night ? 0.08f : 0.42f, night ? 0.10f : 0.50f, night ? 0.14f : 0.58f);
    rect(0,246,1280,18);
    glassTower(-130+farLayerOffset,292,112,235,-22,night);
    apartmentBlock(30+farLayerOffset,282,128,205,night);
    steppedOfficeBlock(214+farLayerOffset,300,160,248,night);
    glassTower(430+farLayerOffset,286,120,270,18,night);
    apartmentBlock(632+farLayerOffset,276,146,220,night);
    steppedOfficeBlock(852+farLayerOffset,292,174,260,night);
    glassTower(1090+farLayerOffset,282,130,245,-18,night);
    glassTower(-40+nearLayerOffset,250,96,175,12,night);
    steppedOfficeBlock(128+nearLayerOffset,248,130,150,night);
    apartmentBlock(322+nearLayerOffset,250,126,170,night);
    glassTower(520+nearLayerOffset,252,110,190,-14,night);
    steppedOfficeBlock(705+nearLayerOffset,250,150,165,night);
    apartmentBlock(925+nearLayerOffset,248,132,172,night);
    glassTower(1126+nearLayerOffset,250,112,184,16,night);
    skylineTrees(246, night);
}

void character(float px, float py) {
    const float hip = 38;
    const float torsoH = 42;
    const float headR = 10;
    glPushMatrix();
    glTranslatef(px,py,0);
    if (!characterFacingRight) glScalef(-1,1,1);
    glColor3f(0.35f,0.40f,0.25f);
    if (characterSittingPose) {
        rect(-10, hip-6, 22,8);
        rect(4,0,8,hip-6);
        glColor3f(0.30f,0.35f,0.20f);
        rect(8,0,8,hip-4);
    } else {
        glPushMatrix(); glTranslatef(-4,hip,0); glRotatef(characterLegSwingAngleDegrees,0,0,1); rect(-4,-hip,8,hip); glPopMatrix();
        glPushMatrix(); glTranslatef(4,hip,0); glRotatef(-characterLegSwingAngleDegrees,0,0,1); rect(-4,-hip,8,hip); glPopMatrix();
    }
    glColor3f(0.08f,0.08f,0.08f);
    rect(-10,-4,14,6); rect(-2,-4,14,6);
    glColor3f(0.2f,0.1f,0.0f); rect(-12, hip, 24,4);
    glColor3f(0.88f,0.90f,0.92f);
    glBegin(GL_POLYGON);
    glVertex2f(-12, hip+4); glVertex2f(12, hip+4); glVertex2f(14, hip+torsoH); glVertex2f(-14, hip+torsoH);
    glEnd();
    glColor3f(0.12f,0.18f,0.32f);
    glBegin(GL_POLYGON);
    glVertex2f(-2, hip+torsoH); glVertex2f(2, hip+torsoH); glVertex2f(3, hip+15); glVertex2f(0, hip+10); glVertex2f(-3, hip+15);
    glEnd();
    glPushMatrix();
    glTranslatef(-12, hip+torsoH-4,0);
    glRotatef(characterLeftArmAngleDegrees,0,0,1);
    glColor3f(0.86f,0.88f,0.90f); rect(-4,-22,8,22);
    glColor3f(0.92f,0.78f,0.65f); circle(0,-24,4,12);
    glPopMatrix();
    float rAng = characterPointingPose ? -58.0f + pointerOscillationAngle_scene6 : characterRightArmAngleDegrees;
    glPushMatrix();
    glTranslatef(12, hip+torsoH-4,0);
    glRotatef(rAng,0,0,1);
    glColor3f(0.88f,0.90f,0.92f); rect(-4,-22,8,22);
    glColor3f(0.92f,0.78f,0.65f); circle(0,-24,4,12);
    if (characterCarryBriefcase) {
        glColor3f(0.28f,0.18f,0.10f); rect(-8,-44,20,16);
        glColor3f(0.1f,0.1f,0.1f); rect(-2,-28,8,4);
    }
    glPopMatrix();
    glColor3f(0.92f,0.78f,0.65f); rect(-3, hip+torsoH, 6,6); circle(0, hip+torsoH+14, headR,24);
    glColor3f(0.15f,0.10f,0.05f);
    glBegin(GL_POLYGON);
    glVertex2f(-10, hip+torsoH+16); glVertex2f(-8, hip+torsoH+24); glVertex2f(3, hip+torsoH+26);
    glVertex2f(10, hip+torsoH+20); glVertex2f(8, hip+torsoH+15); glVertex2f(-5, hip+torsoH+21);
    glEnd();
    glPopMatrix();
}

void initRain() {
    for (int i=0; i<RAIN_DROP_COUNT; i++) {
        rainDropX[i] = rand() % WINDOW_WIDTH;
        rainDropY[i] = rand() % WINDOW_HEIGHT;
    }
}

void drawRain() {
    glColor3f(0.70f,0.74f,0.80f);
    glLineWidth(1.4f);
    glBegin(GL_LINES);
    for (int i=0; i<RAIN_DROP_COUNT; i++) {
        glVertex2f(rainDropX[i], rainDropY[i]);
        glVertex2f(rainDropX[i]+3, rainDropY[i]-14);
    }
    glEnd();
    glLineWidth(1.0f);
}

void tree(float bx, float by) {
    glColor3f(0.42f,0.26f,0.14f); rect(bx-9,by,18,78);
    glColor3f(0.16f,0.46f,0.18f);
    glBegin(GL_POLYGON);
    glVertex2f(bx-44,by+78); glVertex2f(bx-26,by+112); glVertex2f(bx+10,by+124);
    glVertex2f(bx+46,by+98); glVertex2f(bx+26,by+68); glVertex2f(bx-16,by+62);
    glEnd();
    glColor3f(0.20f,0.56f,0.22f);
    glBegin(GL_POLYGON);
    glVertex2f(bx-30,by+96); glVertex2f(bx-8,by+136); glVertex2f(bx+22,by+140);
    glVertex2f(bx+40,by+108); glVertex2f(bx+14,by+86); glVertex2f(bx-18,by+84);
    glEnd();
}

void homeGround(bool night) {
    glColor3f(night ? 0.10f : 0.20f, night ? 0.10f : 0.20f, night ? 0.12f : 0.22f);
    rect(0,homeRoadBottomY,1280,homeRoadHeight);
    glColor3f(0.66f,0.66f,0.70f); rect(0, homeRoadBottomY+homeRoadHeight-3, 1280,3);
    glColor3f(night ? 0.12f : 0.26f, night ? 0.24f : 0.58f, night ? 0.14f : 0.30f);
    rect(0, homeGrassBottomY, 1280, homeGrassHeight);
    glColor3f(night ? 0.30f : 0.52f, night ? 0.31f : 0.52f, night ? 0.34f : 0.56f);
    glBegin(GL_POLYGON);
    glVertex2f(250,110); glVertex2f(430,110); glVertex2f(500,300); glVertex2f(300,300);
    glEnd();
}

void houseWindow(float lx, float by, float w, float h, bool morning, bool lit) {
    glColor3f(0.20f,0.18f,0.16f); rect(lx, by, w, h);
    if (morning) glColor3f(0.60f,0.74f,0.84f);
    else if (lit) glColor3f(0.98f,0.88f,0.54f);
    else glColor3f(0.12f,0.14f,0.18f);
    rect(lx+3, by+3, w-6, h-6);
    glColor3f(0.18f,0.16f,0.14f);
    rect(lx+w*0.5f-1, by+3, 2, h-6);
    rect(lx+3, by+h*0.5f-1, w-6, 2);
}

void house(bool isLit) {
    const float hBaseX = homeHouseLeftX+14;
    const float hBaseY = homeHouseBottomY;
    const float secW = 300;
    const float secH = 270;
    const float gStartX = homeGarageLeftX;
    const float gBaseY = homeGarageBottomY;
    glColor3f(0.85f,0.79f,0.69f); rect(hBaseX, hBaseY, secW, 126);
    glColor3f(0.80f,0.74f,0.65f); rect(hBaseX, hBaseY+126, secW, secH-126);
    glColor3f(0.77f,0.72f,0.64f); rect(gStartX, gBaseY, 240, 170);
    glColor3f(0.24f,0.25f,0.28f);
    glBegin(GL_TRIANGLES);
    glVertex2f(hBaseX-16, hBaseY+secH+8); glVertex2f(hBaseX+secW+16, hBaseY+secH+8); glVertex2f(hBaseX+174, hBaseY+secH+130);
    glEnd();
    glBegin(GL_TRIANGLES);
    glVertex2f(gStartX-16, gBaseY+176); glVertex2f(gStartX+256, gBaseY+176); glVertex2f(gStartX+112, gBaseY+244);
    glEnd();
    float doorX = hBaseX+94;
    glColor3f(0.48f,0.31f,0.17f); rect(doorX, hBaseY, 56,120);
    glColor3f(0.92f,0.76f,0.26f); circle(doorX+46, hBaseY+58, 3.2f,18);
    bool morning = currentScene == 1;
    houseWindow(hBaseX+18, hBaseY+58, 56,54, morning, false);
    houseWindow(hBaseX+168, hBaseY+58, 56,54, morning, isLit);
    houseWindow(hBaseX+18, hBaseY+166, 56,54, morning, false);
    houseWindow(hBaseX+122, hBaseY+166, 56,54, morning, isLit);
    houseWindow(hBaseX+226, hBaseY+166, 56,54, morning, isLit);
    houseWindow(gStartX+12, gBaseY+98, 42,38, morning, isLit);
    glColor3f(0.08f,0.09f,0.11f); rect(homeGarageDoorLeftX, homeGarageDoorBottomY, homeGarageDoorWidth, homeGarageDoorHeight);
    glColor3f(0.46f,0.48f,0.52f);
    for (int i=1; i<6; i++) rect(homeGarageDoorLeftX+8, homeGarageDoorBottomY+(126/6)*i, homeGarageDoorWidth-16,2);
}

void headlightCone(float cx, float cy, bool en) {
    if (!en) return;
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    glColor4f(1,0.94f,0.46f,0.26f);
    glBegin(GL_TRIANGLES);
    glVertex2f(cx+70, cy+20); glVertex2f(cx+252, cy+58); glVertex2f(cx+252, cy-14);
    glEnd();
    glDisable(GL_BLEND);
}

void officeComplex(bool evening) {
    float x = 640.0f;
    float y = 238.0f;
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    glColor4f(0,0,0,0.14f);
    ellipse(x+270,y-14,350,28,36);
    glDisable(GL_BLEND);

    glColor3f(evening ? 0.74f : 0.92f, evening ? 0.75f : 0.93f, evening ? 0.72f : 0.90f);
    glBegin(GL_POLYGON);
    glVertex2f(x+18,y); glVertex2f(x+168,y); glVertex2f(x+140,y+310); glVertex2f(x-22,y+298);
    glEnd();
    glBegin(GL_POLYGON);
    glVertex2f(x+160,y+236); glVertex2f(x+558,y+236); glVertex2f(x+538,y+302); glVertex2f(x+190,y+302);
    glEnd();
    glBegin(GL_POLYGON);
    glVertex2f(x+146,y+120); glVertex2f(x+550,y+120); glVertex2f(x+528,y+184); glVertex2f(x+166,y+184);
    glEnd();
    glBegin(GL_POLYGON);
    glVertex2f(x+132,y); glVertex2f(x+520,y); glVertex2f(x+540,y+66); glVertex2f(x+150,y+66);
    glEnd();

    glColor3f(evening ? 0.12f : 0.30f, evening ? 0.28f : 0.68f, evening ? 0.36f : 0.78f);
    glBegin(GL_POLYGON);
    glVertex2f(x+36,y+12); glVertex2f(x+150,y+12); glVertex2f(x+124,y+280); glVertex2f(x+10,y+286);
    glEnd();
    glBegin(GL_POLYGON);
    glVertex2f(x+188,y+206); glVertex2f(x+522,y+206); glVertex2f(x+510,y+268); glVertex2f(x+205,y+268);
    glEnd();
    glBegin(GL_POLYGON);
    glVertex2f(x+178,y+88); glVertex2f(x+504,y+88); glVertex2f(x+522,y+148); glVertex2f(x+164,y+148);
    glEnd();
    glBegin(GL_POLYGON);
    glVertex2f(x+164,y+12); glVertex2f(x+478,y+12); glVertex2f(x+498,y+52); glVertex2f(x+152,y+52);
    glEnd();

    glColor3f(evening ? 0.05f : 0.13f, evening ? 0.11f : 0.32f, evening ? 0.16f : 0.40f);
    glBegin(GL_LINES);
    for (float gx=x+58; gx<x+520; gx+=44) {
        glVertex2f(gx,y+12);
        glVertex2f(gx+8,y+286);
    }
    for (float gy=y+36; gy<y+278; gy+=38) {
        glVertex2f(x+12,gy);
        glVertex2f(x+520,gy);
    }
    glEnd();

    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    glColor4f(1,1,1, evening ? 0.08f : 0.26f);
    glBegin(GL_POLYGON);
    glVertex2f(x+44,y+232); glVertex2f(x+118,y+246); glVertex2f(x+106,y+280); glVertex2f(x+28,y+272);
    glEnd();
    glBegin(GL_POLYGON);
    glVertex2f(x+244,y+230); glVertex2f(x+456,y+230); glVertex2f(x+494,y+266); glVertex2f(x+226,y+266);
    glEnd();
    glDisable(GL_BLEND);

    glColor3f(evening ? 0.09f : 0.15f, evening ? 0.12f : 0.21f, evening ? 0.16f : 0.25f);
    rect(x+280,y,78,80);
    glColor3f(evening ? 0.92f : 0.62f, evening ? 0.82f : 0.82f, evening ? 0.46f : 0.92f);
    rect(x+292,y+12,24,58);
    rect(x+322,y+12,24,58);

    glColor3f(evening ? 0.14f : 0.18f, evening ? 0.16f : 0.24f, evening ? 0.20f : 0.30f);
    rect(x+198,y+304,292,32);
    glColor3f(1,1,1);
    bmpText(x+230,y+325,"URBAN TECH HQ");
}

void officeRampDown() {
    glColor3f(0.42f,0.43f,0.45f);
    glBegin(GL_POLYGON);
    glVertex2f(470,150); glVertex2f(625,150); glVertex2f(820,246); glVertex2f(675,246);
    glEnd();
    glColor3f(0.58f,0.60f,0.61f);
    glBegin(GL_LINES);
    glVertex2f(548,150); glVertex2f(746,246);
    glEnd();
}

void officeRampUp() {
    glColor3f(0.40f,0.40f,0.43f);
    glBegin(GL_POLYGON);
    glVertex2f(470,150); glVertex2f(625,150); glVertex2f(820,246); glVertex2f(675,246);
    glEnd();
}

void parkingBarrier(float px, float py, float ang) {
    glColor3f(0.30f,0.30f,0.34f); rect(px-14, py-58, 28,58);
    glPushMatrix();
    glTranslatef(px,py,0);
    glRotatef(ang,0,0,1);
    glColor3f(0.90f,0.90f,0.92f); rect(-170,-6,170,12);
    glColor3f(0.88f,0.14f,0.14f);
    rect(-154,-6,18,12); rect(-116,-6,18,12); rect(-78,-6,18,12); rect(-40,-6,18,12);
    glPopMatrix();
}

void streetLight(float bx, float by, bool glow) {
    glColor3f(0.22f,0.22f,0.25f); rect(bx-4, by, 8,150);
    glColor3f(0.55f,0.55f,0.58f); rect(bx-14, by+142, 28,8);
    if (glow) {
        glEnable(GL_BLEND);
        glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
        glColor4f(1,0.92f,0.45f,0.28f); circle(bx, by+140, 30,30);
        glDisable(GL_BLEND);
    }
}

void modernChair(float bx, float by, bool fr) {
    glPushMatrix();
    glTranslatef(bx,by,0);
    if (!fr) glScalef(-1,1,1);
    glColor3f(0.25f,0.25f,0.28f); rect(-12, -8, 24,6);
    glColor3f(0.4f,0.4f,0.44f); circle(-8,-8,4,10); circle(8,-8,4,10);
    glColor3f(0.35f,0.35f,0.38f); rect(-3,-8,6,22);
    glColor3f(0.18f,0.32f,0.58f); ellipse(0,14,18,8,20); rect(-14,10,28,8);
    glColor3f(0.14f,0.26f,0.48f); ellipse(4,34,16,22,24);
    glPopMatrix();
}

void pottedPlant(float x, float y) {
    glColor3f(0.78f,0.78f,0.74f);
    glBegin(GL_POLYGON);
    glVertex2f(x+5,y); glVertex2f(x+25,y); glVertex2f(x+30,y+60); glVertex2f(x,y+60);
    glEnd();
    glColor3f(0.25f,0.50f,0.25f);
    rect(x+13, y+60, 4,50);
    ellipse(x+5, y+80, 15,8,16); ellipse(x+25, y+90, 18,10,16);
    ellipse(x+10, y+105, 16,10,16); ellipse(x+22, y+115, 15,9,16);
}

void wallClock(float cx, float cy, float hand) {
    glColor3f(0.96f,0.96f,0.95f); circle(cx,cy,22,32);
    glColor3f(0.25f,0.25f,0.25f); circle(cx,cy,18,30);
    glColor3f(1,1,1); circle(cx,cy,16,28);
    float rad = hand * PI_VALUE / 180.0f;
    glColor3f(0.1f,0.1f,0.1f);
    glBegin(GL_LINES);
    glVertex2f(cx,cy); glVertex2f(cx + cos(rad)*10, cy + sin(rad)*10);
    glVertex2f(cx,cy); glVertex2f(cx, cy+7);
    glEnd();
}

void drawInteriorShell(float wallR, float wallG, float wallB, float floorR, float floorG, float floorB) {
    glColor3f(wallR,wallG,wallB); rect(0,220,1280,500);
    glColor3f(floorR,floorG,floorB); rect(0,0,1280,220);
    glColor3f(0.54f,0.56f,0.58f); rect(0,218,1280,5);
    glColor3f(floorR-0.08f,floorG-0.08f,floorB-0.08f);
    glBegin(GL_LINES);
    for (int i=0; i<9; i++) {
        glVertex2f(i*160.0f, 0);
        glVertex2f(460+i*45.0f, 220);
    }
    for (int y=35; y<220; y+=35) {
        glVertex2f(0,y);
        glVertex2f(1280,y);
    }
    glEnd();
}

void ceilingPanels() {
    glColor3f(0.82f,0.83f,0.84f); rect(0,640,1280,80);
    glColor3f(0.62f,0.64f,0.66f);
    for (int x=0; x<1280; x+=160) rect((float)x,640,2,80);
    for (int x=80; x<1280; x+=320) {
        glColor3f(0.95f,0.93f,0.78f); rect((float)x,666,160,18);
    }
}

void officeWindowWall() {
    glColor3f(0.28f,0.30f,0.33f);
    rect(60,300,420,260);
    glColor3f(0.56f,0.74f,0.86f);
    rect(72,312,396,236);
    glColor3f(0.35f,0.45f,0.52f);
    for (int i=1; i<4; i++) rect(72+i*99,312,3,236);
    for (int i=1; i<3; i++) rect(72,312+i*78,396,3);
    buildingWindowNightMode = false;
    building(96,316,60,130); building(190,316,70,170); building(300,316,54,120); building(386,316,64,150);
}

void workstation(float x, float y, float scale) {
    glPushMatrix();
    glTranslatef(x,y,0);
    glScalef(scale,scale,1);
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    glColor4f(0,0,0,0.16f); ellipse(112,18,135,22,32);
    glDisable(GL_BLEND);
    glColor3f(0.76f,0.68f,0.56f);
    glBegin(GL_POLYGON);
    glVertex2f(0,72); glVertex2f(240,72); glVertex2f(206,38); glVertex2f(22,38);
    glEnd();
    glColor3f(0.38f,0.36f,0.34f);
    rect(20,12,8,30); rect(204,12,8,30);
    glColor3f(0.18f,0.19f,0.21f); rect(92,82,78,48);
    glColor3f(0.08f,0.10f,0.13f); rect(98,88,66,36);
    glColor3f(0.28f,0.54f,0.76f); rect(103,94,56,22);
    glColor3f(0.18f,0.18f,0.19f); rect(126,72,10,10); rect(112,68,38,5);
    glColor3f(0.12f,0.12f,0.13f); rect(54,74,44,7);
    glColor3f(0.86f,0.86f,0.82f); rect(178,76,22,4);
    glPopMatrix();
}

void officeDeskSurface(float x, float y, float scale) {
    glPushMatrix();
    glTranslatef(x,y,0);
    glScalef(scale,scale,1);
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    glColor4f(0,0,0,0.16f); ellipse(118,10,142,20,32);
    glDisable(GL_BLEND);
    glColor3f(0.34f,0.36f,0.38f);
    rect(24,22,10,45);
    rect(210,22,10,45);
    glColor3f(0.67f,0.58f,0.47f);
    glBegin(GL_POLYGON);
    glVertex2f(10,92); glVertex2f(240,92); glVertex2f(212,50); glVertex2f(36,50);
    glEnd();
    glColor3f(0.48f,0.40f,0.32f);
    glBegin(GL_POLYGON);
    glVertex2f(36,50); glVertex2f(212,50); glVertex2f(198,30); glVertex2f(48,30);
    glEnd();
    glColor3f(0.09f,0.10f,0.11f);
    rect(132,94,82,52);
    glColor3f(0.16f,0.18f,0.20f);
    rect(138,100,70,40);
    glColor3f(0.26f,0.54f,0.76f);
    rect(144,106,58,28);
    glColor3f(0.10f,0.10f,0.11f);
    rect(166,82,14,12);
    rect(148,78,50,5);
    glColor3f(0.12f,0.12f,0.13f);
    rect(58,96,48,6);
    glColor3f(0.86f,0.86f,0.82f);
    rect(180,96,24,4);
    glPopMatrix();
}

void lowOfficeChair(float x, float y, float scale, bool facingRight) {
    glPushMatrix();
    glTranslatef(x,y,0);
    glScalef(scale,scale,1);
    if (!facingRight) glScalef(-1,1,1);
    glColor3f(0.15f,0.24f,0.42f);
    ellipse(0,22,24,10,22);
    rect(-18,18,36,10);
    glColor3f(0.12f,0.20f,0.36f);
    ellipse(5,46,18,24,24);
    glColor3f(0.28f,0.28f,0.30f);
    rect(-3,2,6,18);
    rect(-20,0,40,5);
    circle(-18,-2,4,10);
    circle(18,-2,4,10);
    glPopMatrix();
}

void glassPartition(float x, float y, float w, float h) {
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    glColor4f(0.68f,0.84f,0.92f,0.28f); rect(x,y,w,h);
    glDisable(GL_BLEND);
    glColor3f(0.46f,0.52f,0.56f);
    rect(x,y,w,3); rect(x,y+h-3,w,3); rect(x,y,3,h); rect(x+w-3,y,3,h);
}

void coffeeMachine(float lx, float by) {
    glColor3f(0.18f,0.18f,0.20f); rect(lx, by, 90,120);
    glColor3f(0.30f,0.54f,0.68f); rect(lx+20, by+80, 50,24);
    glColor3f(0.70f,0.70f,0.72f); rect(lx+42, by+56, 8,16);
    glColor3f(0.88f,0.88f,0.88f); rect(lx+35, by+22, 22,20);
    glColor3f(0.28f,0.20f,0.12f); rect(lx+38, by+30, 16,10);
}

void initSteam() {
    const float nozzleX = 1048.0f;
    const float startY = 326.0f;
    for (int i=0; i<STEAM_PARTICLE_COUNT; i++) {
        int col = i % 5;
        coffeeSteamParticles[i].x = nozzleX + (col-2)*6.0f;
        coffeeSteamParticles[i].y = startY + (i%6)*6.0f;
        coffeeSteamParticles[i].speed = 0.55f + (i%4)*0.16f;
        coffeeSteamParticles[i].alpha = 0.40f + (i%6)*0.08f;
    }
}

void steamParticles() {
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    for (int i=0; i<STEAM_PARTICLE_COUNT; i++) {
        glColor4f(0.95f,0.95f,0.95f, clamp(coffeeSteamParticles[i].alpha,0,1));
        circle(coffeeSteamParticles[i].x, coffeeSteamParticles[i].y, 5, 20);
    }
    glDisable(GL_BLEND);
}

void cafeteriaTable(float x, float y, float scale) {
    glPushMatrix();
    glTranslatef(x,y,0);
    glScalef(scale,scale,1);
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    glColor4f(0,0,0,0.16f); ellipse(110,6,130,18,30);
    glDisable(GL_BLEND);
    glColor3f(0.73f,0.62f,0.50f);
    glBegin(GL_POLYGON);
    glVertex2f(0,70); glVertex2f(220,70); glVertex2f(190,38); glVertex2f(28,38);
    glEnd();
    glColor3f(0.34f,0.34f,0.35f);
    rect(28,10,7,32); rect(184,10,7,32);
    glColor3f(0.34f,0.30f,0.27f);
    rect(-24,20,50,10); rect(194,20,50,10);
    glPopMatrix();
}

void cafeteriaTableSurface(float x, float y, float scale) {
    glPushMatrix();
    glTranslatef(x,y,0);
    glScalef(scale,scale,1);
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    glColor4f(0,0,0,0.16f); ellipse(125,2,146,20,30);
    glDisable(GL_BLEND);
    glColor3f(0.34f,0.34f,0.35f);
    rect(34,12,7,42);
    rect(204,12,7,42);
    glColor3f(0.68f,0.56f,0.43f);
    glBegin(GL_POLYGON);
    glVertex2f(0,90); glVertex2f(250,90); glVertex2f(214,48); glVertex2f(32,48);
    glEnd();
    glColor3f(0.46f,0.35f,0.26f);
    glBegin(GL_POLYGON);
    glVertex2f(32,48); glVertex2f(214,48); glVertex2f(202,34); glVertex2f(44,34);
    glEnd();
    glColor3f(0.36f,0.31f,0.27f);
    rect(-28,30,64,11);
    rect(214,30,64,11);
    glPopMatrix();
}

void foodTray(float x, float y) {
    glColor3f(0.72f,0.72f,0.74f); rect(x,y,42,26);
    glColor3f(1.0f,0.93f,0.82f); ellipse(x+18,y+14,14,8,20);
    glColor3f(0.78f,0.30f,0.20f); ellipse(x+18,y+14,8,5,16);
    glColor3f(0.20f,0.62f,0.24f); ellipse(x+9,y+11,4,3,10);
    glColor3f(0.95f,0.95f,1.0f); rect(x+31,y+5,7,12);
}

void servingCounter(float x, float y) {
    glColor3f(0.72f,0.74f,0.74f); rect(x,y,330,108);
    glColor3f(0.52f,0.54f,0.55f); rect(x-8,y+108,346,10);
    glColor3f(0.88f,0.84f,0.74f); rect(x+18,y+70,58,36); rect(x+94,y+70,58,36); rect(x+170,y+70,58,36);
    glColor3f(0.18f,0.18f,0.20f); rect(x+250,y+20,50,86);
    glColor3f(0.94f,0.94f,0.90f); rect(x+24,y+126,230,45);
    glColor3f(0.18f,0.18f,0.18f); bmpText(x+42,y+154,"TODAY'S LUNCH");
}

void conferenceTable(float x, float y) {
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    glColor4f(0,0,0,0.18f); ellipse(x+260,y+16,310,34,36);
    glDisable(GL_BLEND);
    glColor3f(0.42f,0.31f,0.22f);
    glBegin(GL_POLYGON);
    glVertex2f(x+40,y+130); glVertex2f(x+500,y+130); glVertex2f(x+620,y+32); glVertex2f(x-40,y+32);
    glEnd();
    glColor3f(0.62f,0.48f,0.34f);
    glBegin(GL_POLYGON);
    glVertex2f(x+60,y+118); glVertex2f(x+480,y+118); glVertex2f(x+560,y+48); glVertex2f(x+0,y+48);
    glEnd();
    glColor3f(0.22f,0.22f,0.23f);
    rect(x+170,y+12,12,40); rect(x+370,y+12,12,40);
    glColor3f(0.08f,0.09f,0.10f); rect(x+245,y+88,72,8);
    glColor3f(0.18f,0.19f,0.21f); rect(x+258,y+96,46,30);
    glColor3f(0.30f,0.52f,0.68f); rect(x+262,y+101,38,20);
}

void meetingChair(float x, float y, float scale, bool frontSide) {
    glPushMatrix();
    glTranslatef(x,y,0);
    glScalef(scale,scale,1);
    glColor3f(0.16f,0.26f,0.48f);
    ellipse(0,18,22,10,22);
    rect(-18,14,36,10);
    glColor3f(0.12f,0.22f,0.42f);
    if (frontSide) {
        ellipse(0,42,20,24,24);
    } else {
        ellipse(0,36,19,21,24);
    }
    glColor3f(0.32f,0.32f,0.34f);
    rect(-3,0,6,18);
    rect(-22,-2,44,5);
    circle(-18,-4,4,10);
    circle(18,-4,4,10);
    glPopMatrix();
}

void scene1() {
    if (!isRainEnabled) {
        float skyBottomR = 0.68f;
        float skyBottomG = 0.88f;
        float skyBottomB = 0.99f;
        float skyTopR = 0.88f;
        float skyTopG = 0.96f;
        float skyTopB = 1.00f;
        glBegin(GL_QUADS);
        glColor3f(skyBottomR, skyBottomG, skyBottomB);
        glVertex2f(0.0f, 0.0f);
        glColor3f(skyBottomR, skyBottomG, skyBottomB);
        glVertex2f(1280.0f, 0.0f);
        glColor3f(skyTopR, skyTopG, skyTopB);
        glVertex2f(1280.0f, 720.0f);
        glColor3f(skyTopR, skyTopG, skyTopB);
        glVertex2f(0.0f, 720.0f);
        glEnd();

        float sunCenterX = 1020.0f + sunHorizontalOffset * 0.20f;
        float sunCenterY = 620.0f;
        glColor3f(1.0f, 0.92f, 0.32f);
        {
            float centerX = sunCenterX;
            float centerY = sunCenterY;
            float radiusX = 36.0f;
            float radiusY = 36.0f;
            float step = 2.0f * PI_VALUE / 36.0f;
            glBegin(GL_POLYGON);
            glVertex2f(centerX + cos(step * 0.0f) * radiusX, centerY + sin(step * 0.0f) * radiusY);
            glVertex2f(centerX + cos(step * 1.0f) * radiusX, centerY + sin(step * 1.0f) * radiusY);
            glVertex2f(centerX + cos(step * 2.0f) * radiusX, centerY + sin(step * 2.0f) * radiusY);
            glVertex2f(centerX + cos(step * 3.0f) * radiusX, centerY + sin(step * 3.0f) * radiusY);
            glVertex2f(centerX + cos(step * 4.0f) * radiusX, centerY + sin(step * 4.0f) * radiusY);
            glVertex2f(centerX + cos(step * 5.0f) * radiusX, centerY + sin(step * 5.0f) * radiusY);
            glVertex2f(centerX + cos(step * 6.0f) * radiusX, centerY + sin(step * 6.0f) * radiusY);
            glVertex2f(centerX + cos(step * 7.0f) * radiusX, centerY + sin(step * 7.0f) * radiusY);
            glVertex2f(centerX + cos(step * 8.0f) * radiusX, centerY + sin(step * 8.0f) * radiusY);
            glVertex2f(centerX + cos(step * 9.0f) * radiusX, centerY + sin(step * 9.0f) * radiusY);
            glVertex2f(centerX + cos(step * 10.0f) * radiusX, centerY + sin(step * 10.0f) * radiusY);
            glVertex2f(centerX + cos(step * 11.0f) * radiusX, centerY + sin(step * 11.0f) * radiusY);
            glVertex2f(centerX + cos(step * 12.0f) * radiusX, centerY + sin(step * 12.0f) * radiusY);
            glVertex2f(centerX + cos(step * 13.0f) * radiusX, centerY + sin(step * 13.0f) * radiusY);
            glVertex2f(centerX + cos(step * 14.0f) * radiusX, centerY + sin(step * 14.0f) * radiusY);
            glVertex2f(centerX + cos(step * 15.0f) * radiusX, centerY + sin(step * 15.0f) * radiusY);
            glVertex2f(centerX + cos(step * 16.0f) * radiusX, centerY + sin(step * 16.0f) * radiusY);
            glVertex2f(centerX + cos(step * 17.0f) * radiusX, centerY + sin(step * 17.0f) * radiusY);
            glVertex2f(centerX + cos(step * 18.0f) * radiusX, centerY + sin(step * 18.0f) * radiusY);
            glVertex2f(centerX + cos(step * 19.0f) * radiusX, centerY + sin(step * 19.0f) * radiusY);
            glVertex2f(centerX + cos(step * 20.0f) * radiusX, centerY + sin(step * 20.0f) * radiusY);
            glVertex2f(centerX + cos(step * 21.0f) * radiusX, centerY + sin(step * 21.0f) * radiusY);
            glVertex2f(centerX + cos(step * 22.0f) * radiusX, centerY + sin(step * 22.0f) * radiusY);
            glVertex2f(centerX + cos(step * 23.0f) * radiusX, centerY + sin(step * 23.0f) * radiusY);
            glVertex2f(centerX + cos(step * 24.0f) * radiusX, centerY + sin(step * 24.0f) * radiusY);
            glVertex2f(centerX + cos(step * 25.0f) * radiusX, centerY + sin(step * 25.0f) * radiusY);
            glVertex2f(centerX + cos(step * 26.0f) * radiusX, centerY + sin(step * 26.0f) * radiusY);
            glVertex2f(centerX + cos(step * 27.0f) * radiusX, centerY + sin(step * 27.0f) * radiusY);
            glVertex2f(centerX + cos(step * 28.0f) * radiusX, centerY + sin(step * 28.0f) * radiusY);
            glVertex2f(centerX + cos(step * 29.0f) * radiusX, centerY + sin(step * 29.0f) * radiusY);
            glVertex2f(centerX + cos(step * 30.0f) * radiusX, centerY + sin(step * 30.0f) * radiusY);
            glVertex2f(centerX + cos(step * 31.0f) * radiusX, centerY + sin(step * 31.0f) * radiusY);
            glVertex2f(centerX + cos(step * 32.0f) * radiusX, centerY + sin(step * 32.0f) * radiusY);
            glVertex2f(centerX + cos(step * 33.0f) * radiusX, centerY + sin(step * 33.0f) * radiusY);
            glVertex2f(centerX + cos(step * 34.0f) * radiusX, centerY + sin(step * 34.0f) * radiusY);
            glVertex2f(centerX + cos(step * 35.0f) * radiusX, centerY + sin(step * 35.0f) * radiusY);
            glEnd();
        }
        glColor3f(1.0f, 0.84f, 0.22f);
        glLineWidth(2.0f);
        glBegin(GL_LINES);
        glVertex2f(sunCenterX - 54.0f, sunCenterY);
        glVertex2f(sunCenterX - 38.0f, sunCenterY);
        glVertex2f(sunCenterX + 38.0f, sunCenterY);
        glVertex2f(sunCenterX + 54.0f, sunCenterY);
        glVertex2f(sunCenterX, sunCenterY - 54.0f);
        glVertex2f(sunCenterX, sunCenterY - 38.0f);
        glVertex2f(sunCenterX, sunCenterY + 38.0f);
        glVertex2f(sunCenterX, sunCenterY + 54.0f);
        glEnd();
        glLineWidth(1.0f);

        float cloudA_CenterX = 210.0f + cloudOffsetX_layerA * 0.70f;
        float cloudA_CenterY = 620.0f;
        glColor3f(0.98f, 0.98f, 0.99f);
        {
            float centerX = cloudA_CenterX;
            float centerY = cloudA_CenterY;
            float radiusX = 28.0f;
            float radiusY = 28.0f;
            float step = 2.0f * PI_VALUE / 28.0f;
            glBegin(GL_POLYGON);
            glVertex2f(centerX + cos(step * 0.0f) * radiusX, centerY + sin(step * 0.0f) * radiusY);
            glVertex2f(centerX + cos(step * 1.0f) * radiusX, centerY + sin(step * 1.0f) * radiusY);
            glVertex2f(centerX + cos(step * 2.0f) * radiusX, centerY + sin(step * 2.0f) * radiusY);
            glVertex2f(centerX + cos(step * 3.0f) * radiusX, centerY + sin(step * 3.0f) * radiusY);
            glVertex2f(centerX + cos(step * 4.0f) * radiusX, centerY + sin(step * 4.0f) * radiusY);
            glVertex2f(centerX + cos(step * 5.0f) * radiusX, centerY + sin(step * 5.0f) * radiusY);
            glVertex2f(centerX + cos(step * 6.0f) * radiusX, centerY + sin(step * 6.0f) * radiusY);
            glVertex2f(centerX + cos(step * 7.0f) * radiusX, centerY + sin(step * 7.0f) * radiusY);
            glVertex2f(centerX + cos(step * 8.0f) * radiusX, centerY + sin(step * 8.0f) * radiusY);
            glVertex2f(centerX + cos(step * 9.0f) * radiusX, centerY + sin(step * 9.0f) * radiusY);
            glVertex2f(centerX + cos(step * 10.0f) * radiusX, centerY + sin(step * 10.0f) * radiusY);
            glVertex2f(centerX + cos(step * 11.0f) * radiusX, centerY + sin(step * 11.0f) * radiusY);
            glVertex2f(centerX + cos(step * 12.0f) * radiusX, centerY + sin(step * 12.0f) * radiusY);
            glVertex2f(centerX + cos(step * 13.0f) * radiusX, centerY + sin(step * 13.0f) * radiusY);
            glVertex2f(centerX + cos(step * 14.0f) * radiusX, centerY + sin(step * 14.0f) * radiusY);
            glVertex2f(centerX + cos(step * 15.0f) * radiusX, centerY + sin(step * 15.0f) * radiusY);
            glVertex2f(centerX + cos(step * 16.0f) * radiusX, centerY + sin(step * 16.0f) * radiusY);
            glVertex2f(centerX + cos(step * 17.0f) * radiusX, centerY + sin(step * 17.0f) * radiusY);
            glVertex2f(centerX + cos(step * 18.0f) * radiusX, centerY + sin(step * 18.0f) * radiusY);
            glVertex2f(centerX + cos(step * 19.0f) * radiusX, centerY + sin(step * 19.0f) * radiusY);
            glVertex2f(centerX + cos(step * 20.0f) * radiusX, centerY + sin(step * 20.0f) * radiusY);
            glVertex2f(centerX + cos(step * 21.0f) * radiusX, centerY + sin(step * 21.0f) * radiusY);
            glVertex2f(centerX + cos(step * 22.0f) * radiusX, centerY + sin(step * 22.0f) * radiusY);
            glVertex2f(centerX + cos(step * 23.0f) * radiusX, centerY + sin(step * 23.0f) * radiusY);
            glVertex2f(centerX + cos(step * 24.0f) * radiusX, centerY + sin(step * 24.0f) * radiusY);
            glVertex2f(centerX + cos(step * 25.0f) * radiusX, centerY + sin(step * 25.0f) * radiusY);
            glVertex2f(centerX + cos(step * 26.0f) * radiusX, centerY + sin(step * 26.0f) * radiusY);
            glVertex2f(centerX + cos(step * 27.0f) * radiusX, centerY + sin(step * 27.0f) * radiusY);
            glEnd();
        }
        {
            float centerX = cloudA_CenterX - 30.0f;
            float centerY = cloudA_CenterY - 4.0f;
            float radiusX = 22.0f;
            float radiusY = 22.0f;
            float step = 2.0f * PI_VALUE / 24.0f;
            glBegin(GL_POLYGON);
            glVertex2f(centerX + cos(step * 0.0f) * radiusX, centerY + sin(step * 0.0f) * radiusY);
            glVertex2f(centerX + cos(step * 1.0f) * radiusX, centerY + sin(step * 1.0f) * radiusY);
            glVertex2f(centerX + cos(step * 2.0f) * radiusX, centerY + sin(step * 2.0f) * radiusY);
            glVertex2f(centerX + cos(step * 3.0f) * radiusX, centerY + sin(step * 3.0f) * radiusY);
            glVertex2f(centerX + cos(step * 4.0f) * radiusX, centerY + sin(step * 4.0f) * radiusY);
            glVertex2f(centerX + cos(step * 5.0f) * radiusX, centerY + sin(step * 5.0f) * radiusY);
            glVertex2f(centerX + cos(step * 6.0f) * radiusX, centerY + sin(step * 6.0f) * radiusY);
            glVertex2f(centerX + cos(step * 7.0f) * radiusX, centerY + sin(step * 7.0f) * radiusY);
            glVertex2f(centerX + cos(step * 8.0f) * radiusX, centerY + sin(step * 8.0f) * radiusY);
            glVertex2f(centerX + cos(step * 9.0f) * radiusX, centerY + sin(step * 9.0f) * radiusY);
            glVertex2f(centerX + cos(step * 10.0f) * radiusX, centerY + sin(step * 10.0f) * radiusY);
            glVertex2f(centerX + cos(step * 11.0f) * radiusX, centerY + sin(step * 11.0f) * radiusY);
            glVertex2f(centerX + cos(step * 12.0f) * radiusX, centerY + sin(step * 12.0f) * radiusY);
            glVertex2f(centerX + cos(step * 13.0f) * radiusX, centerY + sin(step * 13.0f) * radiusY);
            glVertex2f(centerX + cos(step * 14.0f) * radiusX, centerY + sin(step * 14.0f) * radiusY);
            glVertex2f(centerX + cos(step * 15.0f) * radiusX, centerY + sin(step * 15.0f) * radiusY);
            glVertex2f(centerX + cos(step * 16.0f) * radiusX, centerY + sin(step * 16.0f) * radiusY);
            glVertex2f(centerX + cos(step * 17.0f) * radiusX, centerY + sin(step * 17.0f) * radiusY);
            glVertex2f(centerX + cos(step * 18.0f) * radiusX, centerY + sin(step * 18.0f) * radiusY);
            glVertex2f(centerX + cos(step * 19.0f) * radiusX, centerY + sin(step * 19.0f) * radiusY);
            glVertex2f(centerX + cos(step * 20.0f) * radiusX, centerY + sin(step * 20.0f) * radiusY);
            glVertex2f(centerX + cos(step * 21.0f) * radiusX, centerY + sin(step * 21.0f) * radiusY);
            glVertex2f(centerX + cos(step * 22.0f) * radiusX, centerY + sin(step * 22.0f) * radiusY);
            glVertex2f(centerX + cos(step * 23.0f) * radiusX, centerY + sin(step * 23.0f) * radiusY);
            glEnd();
        }
        {
            float centerX = cloudA_CenterX + 30.0f;
            float centerY = cloudA_CenterY - 2.0f;
            float radiusX = 24.0f;
            float radiusY = 24.0f;
            float step = 2.0f * PI_VALUE / 24.0f;
            glBegin(GL_POLYGON);
            glVertex2f(centerX + cos(step * 0.0f) * radiusX, centerY + sin(step * 0.0f) * radiusY);
            glVertex2f(centerX + cos(step * 1.0f) * radiusX, centerY + sin(step * 1.0f) * radiusY);
            glVertex2f(centerX + cos(step * 2.0f) * radiusX, centerY + sin(step * 2.0f) * radiusY);
            glVertex2f(centerX + cos(step * 3.0f) * radiusX, centerY + sin(step * 3.0f) * radiusY);
            glVertex2f(centerX + cos(step * 4.0f) * radiusX, centerY + sin(step * 4.0f) * radiusY);
            glVertex2f(centerX + cos(step * 5.0f) * radiusX, centerY + sin(step * 5.0f) * radiusY);
            glVertex2f(centerX + cos(step * 6.0f) * radiusX, centerY + sin(step * 6.0f) * radiusY);
            glVertex2f(centerX + cos(step * 7.0f) * radiusX, centerY + sin(step * 7.0f) * radiusY);
            glVertex2f(centerX + cos(step * 8.0f) * radiusX, centerY + sin(step * 8.0f) * radiusY);
            glVertex2f(centerX + cos(step * 9.0f) * radiusX, centerY + sin(step * 9.0f) * radiusY);
            glVertex2f(centerX + cos(step * 10.0f) * radiusX, centerY + sin(step * 10.0f) * radiusY);
            glVertex2f(centerX + cos(step * 11.0f) * radiusX, centerY + sin(step * 11.0f) * radiusY);
            glVertex2f(centerX + cos(step * 12.0f) * radiusX, centerY + sin(step * 12.0f) * radiusY);
            glVertex2f(centerX + cos(step * 13.0f) * radiusX, centerY + sin(step * 13.0f) * radiusY);
            glVertex2f(centerX + cos(step * 14.0f) * radiusX, centerY + sin(step * 14.0f) * radiusY);
            glVertex2f(centerX + cos(step * 15.0f) * radiusX, centerY + sin(step * 15.0f) * radiusY);
            glVertex2f(centerX + cos(step * 16.0f) * radiusX, centerY + sin(step * 16.0f) * radiusY);
            glVertex2f(centerX + cos(step * 17.0f) * radiusX, centerY + sin(step * 17.0f) * radiusY);
            glVertex2f(centerX + cos(step * 18.0f) * radiusX, centerY + sin(step * 18.0f) * radiusY);
            glVertex2f(centerX + cos(step * 19.0f) * radiusX, centerY + sin(step * 19.0f) * radiusY);
            glVertex2f(centerX + cos(step * 20.0f) * radiusX, centerY + sin(step * 20.0f) * radiusY);
            glVertex2f(centerX + cos(step * 21.0f) * radiusX, centerY + sin(step * 21.0f) * radiusY);
            glVertex2f(centerX + cos(step * 22.0f) * radiusX, centerY + sin(step * 22.0f) * radiusY);
            glVertex2f(centerX + cos(step * 23.0f) * radiusX, centerY + sin(step * 23.0f) * radiusY);
            glEnd();
        }
        {
            float centerX = cloudA_CenterX + 2.0f;
            float centerY = cloudA_CenterY + 18.0f;
            float radiusX = 20.0f;
            float radiusY = 20.0f;
            float step = 2.0f * PI_VALUE / 24.0f;
            glBegin(GL_POLYGON);
            glVertex2f(centerX + cos(step * 0.0f) * radiusX, centerY + sin(step * 0.0f) * radiusY);
            glVertex2f(centerX + cos(step * 1.0f) * radiusX, centerY + sin(step * 1.0f) * radiusY);
            glVertex2f(centerX + cos(step * 2.0f) * radiusX, centerY + sin(step * 2.0f) * radiusY);
            glVertex2f(centerX + cos(step * 3.0f) * radiusX, centerY + sin(step * 3.0f) * radiusY);
            glVertex2f(centerX + cos(step * 4.0f) * radiusX, centerY + sin(step * 4.0f) * radiusY);
            glVertex2f(centerX + cos(step * 5.0f) * radiusX, centerY + sin(step * 5.0f) * radiusY);
            glVertex2f(centerX + cos(step * 6.0f) * radiusX, centerY + sin(step * 6.0f) * radiusY);
            glVertex2f(centerX + cos(step * 7.0f) * radiusX, centerY + sin(step * 7.0f) * radiusY);
            glVertex2f(centerX + cos(step * 8.0f) * radiusX, centerY + sin(step * 8.0f) * radiusY);
            glVertex2f(centerX + cos(step * 9.0f) * radiusX, centerY + sin(step * 9.0f) * radiusY);
            glVertex2f(centerX + cos(step * 10.0f) * radiusX, centerY + sin(step * 10.0f) * radiusY);
            glVertex2f(centerX + cos(step * 11.0f) * radiusX, centerY + sin(step * 11.0f) * radiusY);
            glVertex2f(centerX + cos(step * 12.0f) * radiusX, centerY + sin(step * 12.0f) * radiusY);
            glVertex2f(centerX + cos(step * 13.0f) * radiusX, centerY + sin(step * 13.0f) * radiusY);
            glVertex2f(centerX + cos(step * 14.0f) * radiusX, centerY + sin(step * 14.0f) * radiusY);
            glVertex2f(centerX + cos(step * 15.0f) * radiusX, centerY + sin(step * 15.0f) * radiusY);
            glVertex2f(centerX + cos(step * 16.0f) * radiusX, centerY + sin(step * 16.0f) * radiusY);
            glVertex2f(centerX + cos(step * 17.0f) * radiusX, centerY + sin(step * 17.0f) * radiusY);
            glVertex2f(centerX + cos(step * 18.0f) * radiusX, centerY + sin(step * 18.0f) * radiusY);
            glVertex2f(centerX + cos(step * 19.0f) * radiusX, centerY + sin(step * 19.0f) * radiusY);
            glVertex2f(centerX + cos(step * 20.0f) * radiusX, centerY + sin(step * 20.0f) * radiusY);
            glVertex2f(centerX + cos(step * 21.0f) * radiusX, centerY + sin(step * 21.0f) * radiusY);
            glVertex2f(centerX + cos(step * 22.0f) * radiusX, centerY + sin(step * 22.0f) * radiusY);
            glVertex2f(centerX + cos(step * 23.0f) * radiusX, centerY + sin(step * 23.0f) * radiusY);
            glEnd();
        }

        float cloudB_CenterX = 520.0f + cloudOffsetX_layerB * 0.60f;
        float cloudB_CenterY = 650.0f;
        glColor3f(0.98f, 0.98f, 0.99f);
        {
            float centerX = cloudB_CenterX;
            float centerY = cloudB_CenterY;
            float radiusX = 28.0f;
            float radiusY = 28.0f;
            float step = 2.0f * PI_VALUE / 28.0f;
            glBegin(GL_POLYGON);
            glVertex2f(centerX + cos(step * 0.0f) * radiusX, centerY + sin(step * 0.0f) * radiusY);
            glVertex2f(centerX + cos(step * 1.0f) * radiusX, centerY + sin(step * 1.0f) * radiusY);
            glVertex2f(centerX + cos(step * 2.0f) * radiusX, centerY + sin(step * 2.0f) * radiusY);
            glVertex2f(centerX + cos(step * 3.0f) * radiusX, centerY + sin(step * 3.0f) * radiusY);
            glVertex2f(centerX + cos(step * 4.0f) * radiusX, centerY + sin(step * 4.0f) * radiusY);
            glVertex2f(centerX + cos(step * 5.0f) * radiusX, centerY + sin(step * 5.0f) * radiusY);
            glVertex2f(centerX + cos(step * 6.0f) * radiusX, centerY + sin(step * 6.0f) * radiusY);
            glVertex2f(centerX + cos(step * 7.0f) * radiusX, centerY + sin(step * 7.0f) * radiusY);
            glVertex2f(centerX + cos(step * 8.0f) * radiusX, centerY + sin(step * 8.0f) * radiusY);
            glVertex2f(centerX + cos(step * 9.0f) * radiusX, centerY + sin(step * 9.0f) * radiusY);
            glVertex2f(centerX + cos(step * 10.0f) * radiusX, centerY + sin(step * 10.0f) * radiusY);
            glVertex2f(centerX + cos(step * 11.0f) * radiusX, centerY + sin(step * 11.0f) * radiusY);
            glVertex2f(centerX + cos(step * 12.0f) * radiusX, centerY + sin(step * 12.0f) * radiusY);
            glVertex2f(centerX + cos(step * 13.0f) * radiusX, centerY + sin(step * 13.0f) * radiusY);
            glVertex2f(centerX + cos(step * 14.0f) * radiusX, centerY + sin(step * 14.0f) * radiusY);
            glVertex2f(centerX + cos(step * 15.0f) * radiusX, centerY + sin(step * 15.0f) * radiusY);
            glVertex2f(centerX + cos(step * 16.0f) * radiusX, centerY + sin(step * 16.0f) * radiusY);
            glVertex2f(centerX + cos(step * 17.0f) * radiusX, centerY + sin(step * 17.0f) * radiusY);
            glVertex2f(centerX + cos(step * 18.0f) * radiusX, centerY + sin(step * 18.0f) * radiusY);
            glVertex2f(centerX + cos(step * 19.0f) * radiusX, centerY + sin(step * 19.0f) * radiusY);
            glVertex2f(centerX + cos(step * 20.0f) * radiusX, centerY + sin(step * 20.0f) * radiusY);
            glVertex2f(centerX + cos(step * 21.0f) * radiusX, centerY + sin(step * 21.0f) * radiusY);
            glVertex2f(centerX + cos(step * 22.0f) * radiusX, centerY + sin(step * 22.0f) * radiusY);
            glVertex2f(centerX + cos(step * 23.0f) * radiusX, centerY + sin(step * 23.0f) * radiusY);
            glVertex2f(centerX + cos(step * 24.0f) * radiusX, centerY + sin(step * 24.0f) * radiusY);
            glVertex2f(centerX + cos(step * 25.0f) * radiusX, centerY + sin(step * 25.0f) * radiusY);
            glVertex2f(centerX + cos(step * 26.0f) * radiusX, centerY + sin(step * 26.0f) * radiusY);
            glVertex2f(centerX + cos(step * 27.0f) * radiusX, centerY + sin(step * 27.0f) * radiusY);
            glEnd();
        }
        {
            float centerX = cloudB_CenterX - 30.0f;
            float centerY = cloudB_CenterY - 4.0f;
            float radiusX = 22.0f;
            float radiusY = 22.0f;
            float step = 2.0f * PI_VALUE / 24.0f;
            glBegin(GL_POLYGON);
            glVertex2f(centerX + cos(step * 0.0f) * radiusX, centerY + sin(step * 0.0f) * radiusY);
            glVertex2f(centerX + cos(step * 1.0f) * radiusX, centerY + sin(step * 1.0f) * radiusY);
            glVertex2f(centerX + cos(step * 2.0f) * radiusX, centerY + sin(step * 2.0f) * radiusY);
            glVertex2f(centerX + cos(step * 3.0f) * radiusX, centerY + sin(step * 3.0f) * radiusY);
            glVertex2f(centerX + cos(step * 4.0f) * radiusX, centerY + sin(step * 4.0f) * radiusY);
            glVertex2f(centerX + cos(step * 5.0f) * radiusX, centerY + sin(step * 5.0f) * radiusY);
            glVertex2f(centerX + cos(step * 6.0f) * radiusX, centerY + sin(step * 6.0f) * radiusY);
            glVertex2f(centerX + cos(step * 7.0f) * radiusX, centerY + sin(step * 7.0f) * radiusY);
            glVertex2f(centerX + cos(step * 8.0f) * radiusX, centerY + sin(step * 8.0f) * radiusY);
            glVertex2f(centerX + cos(step * 9.0f) * radiusX, centerY + sin(step * 9.0f) * radiusY);
            glVertex2f(centerX + cos(step * 10.0f) * radiusX, centerY + sin(step * 10.0f) * radiusY);
            glVertex2f(centerX + cos(step * 11.0f) * radiusX, centerY + sin(step * 11.0f) * radiusY);
            glVertex2f(centerX + cos(step * 12.0f) * radiusX, centerY + sin(step * 12.0f) * radiusY);
            glVertex2f(centerX + cos(step * 13.0f) * radiusX, centerY + sin(step * 13.0f) * radiusY);
            glVertex2f(centerX + cos(step * 14.0f) * radiusX, centerY + sin(step * 14.0f) * radiusY);
            glVertex2f(centerX + cos(step * 15.0f) * radiusX, centerY + sin(step * 15.0f) * radiusY);
            glVertex2f(centerX + cos(step * 16.0f) * radiusX, centerY + sin(step * 16.0f) * radiusY);
            glVertex2f(centerX + cos(step * 17.0f) * radiusX, centerY + sin(step * 17.0f) * radiusY);
            glVertex2f(centerX + cos(step * 18.0f) * radiusX, centerY + sin(step * 18.0f) * radiusY);
            glVertex2f(centerX + cos(step * 19.0f) * radiusX, centerY + sin(step * 19.0f) * radiusY);
            glVertex2f(centerX + cos(step * 20.0f) * radiusX, centerY + sin(step * 20.0f) * radiusY);
            glVertex2f(centerX + cos(step * 21.0f) * radiusX, centerY + sin(step * 21.0f) * radiusY);
            glVertex2f(centerX + cos(step * 22.0f) * radiusX, centerY + sin(step * 22.0f) * radiusY);
            glVertex2f(centerX + cos(step * 23.0f) * radiusX, centerY + sin(step * 23.0f) * radiusY);
            glEnd();
        }
        {
            float centerX = cloudB_CenterX + 30.0f;
            float centerY = cloudB_CenterY - 2.0f;
            float radiusX = 24.0f;
            float radiusY = 24.0f;
            float step = 2.0f * PI_VALUE / 24.0f;
            glBegin(GL_POLYGON);
            glVertex2f(centerX + cos(step * 0.0f) * radiusX, centerY + sin(step * 0.0f) * radiusY);
            glVertex2f(centerX + cos(step * 1.0f) * radiusX, centerY + sin(step * 1.0f) * radiusY);
            glVertex2f(centerX + cos(step * 2.0f) * radiusX, centerY + sin(step * 2.0f) * radiusY);
            glVertex2f(centerX + cos(step * 3.0f) * radiusX, centerY + sin(step * 3.0f) * radiusY);
            glVertex2f(centerX + cos(step * 4.0f) * radiusX, centerY + sin(step * 4.0f) * radiusY);
            glVertex2f(centerX + cos(step * 5.0f) * radiusX, centerY + sin(step * 5.0f) * radiusY);
            glVertex2f(centerX + cos(step * 6.0f) * radiusX, centerY + sin(step * 6.0f) * radiusY);
            glVertex2f(centerX + cos(step * 7.0f) * radiusX, centerY + sin(step * 7.0f) * radiusY);
            glVertex2f(centerX + cos(step * 8.0f) * radiusX, centerY + sin(step * 8.0f) * radiusY);
            glVertex2f(centerX + cos(step * 9.0f) * radiusX, centerY + sin(step * 9.0f) * radiusY);
            glVertex2f(centerX + cos(step * 10.0f) * radiusX, centerY + sin(step * 10.0f) * radiusY);
            glVertex2f(centerX + cos(step * 11.0f) * radiusX, centerY + sin(step * 11.0f) * radiusY);
            glVertex2f(centerX + cos(step * 12.0f) * radiusX, centerY + sin(step * 12.0f) * radiusY);
            glVertex2f(centerX + cos(step * 13.0f) * radiusX, centerY + sin(step * 13.0f) * radiusY);
            glVertex2f(centerX + cos(step * 14.0f) * radiusX, centerY + sin(step * 14.0f) * radiusY);
            glVertex2f(centerX + cos(step * 15.0f) * radiusX, centerY + sin(step * 15.0f) * radiusY);
            glVertex2f(centerX + cos(step * 16.0f) * radiusX, centerY + sin(step * 16.0f) * radiusY);
            glVertex2f(centerX + cos(step * 17.0f) * radiusX, centerY + sin(step * 17.0f) * radiusY);
            glVertex2f(centerX + cos(step * 18.0f) * radiusX, centerY + sin(step * 18.0f) * radiusY);
            glVertex2f(centerX + cos(step * 19.0f) * radiusX, centerY + sin(step * 19.0f) * radiusY);
            glVertex2f(centerX + cos(step * 20.0f) * radiusX, centerY + sin(step * 20.0f) * radiusY);
            glVertex2f(centerX + cos(step * 21.0f) * radiusX, centerY + sin(step * 21.0f) * radiusY);
            glVertex2f(centerX + cos(step * 22.0f) * radiusX, centerY + sin(step * 22.0f) * radiusY);
            glVertex2f(centerX + cos(step * 23.0f) * radiusX, centerY + sin(step * 23.0f) * radiusY);
            glEnd();
        }
        {
            float centerX = cloudB_CenterX + 2.0f;
            float centerY = cloudB_CenterY + 18.0f;
            float radiusX = 20.0f;
            float radiusY = 20.0f;
            float step = 2.0f * PI_VALUE / 24.0f;
            glBegin(GL_POLYGON);
            glVertex2f(centerX + cos(step * 0.0f) * radiusX, centerY + sin(step * 0.0f) * radiusY);
            glVertex2f(centerX + cos(step * 1.0f) * radiusX, centerY + sin(step * 1.0f) * radiusY);
            glVertex2f(centerX + cos(step * 2.0f) * radiusX, centerY + sin(step * 2.0f) * radiusY);
            glVertex2f(centerX + cos(step * 3.0f) * radiusX, centerY + sin(step * 3.0f) * radiusY);
            glVertex2f(centerX + cos(step * 4.0f) * radiusX, centerY + sin(step * 4.0f) * radiusY);
            glVertex2f(centerX + cos(step * 5.0f) * radiusX, centerY + sin(step * 5.0f) * radiusY);
            glVertex2f(centerX + cos(step * 6.0f) * radiusX, centerY + sin(step * 6.0f) * radiusY);
            glVertex2f(centerX + cos(step * 7.0f) * radiusX, centerY + sin(step * 7.0f) * radiusY);
            glVertex2f(centerX + cos(step * 8.0f) * radiusX, centerY + sin(step * 8.0f) * radiusY);
            glVertex2f(centerX + cos(step * 9.0f) * radiusX, centerY + sin(step * 9.0f) * radiusY);
            glVertex2f(centerX + cos(step * 10.0f) * radiusX, centerY + sin(step * 10.0f) * radiusY);
            glVertex2f(centerX + cos(step * 11.0f) * radiusX, centerY + sin(step * 11.0f) * radiusY);
            glVertex2f(centerX + cos(step * 12.0f) * radiusX, centerY + sin(step * 12.0f) * radiusY);
            glVertex2f(centerX + cos(step * 13.0f) * radiusX, centerY + sin(step * 13.0f) * radiusY);
            glVertex2f(centerX + cos(step * 14.0f) * radiusX, centerY + sin(step * 14.0f) * radiusY);
            glVertex2f(centerX + cos(step * 15.0f) * radiusX, centerY + sin(step * 15.0f) * radiusY);
            glVertex2f(centerX + cos(step * 16.0f) * radiusX, centerY + sin(step * 16.0f) * radiusY);
            glVertex2f(centerX + cos(step * 17.0f) * radiusX, centerY + sin(step * 17.0f) * radiusY);
            glVertex2f(centerX + cos(step * 18.0f) * radiusX, centerY + sin(step * 18.0f) * radiusY);
            glVertex2f(centerX + cos(step * 19.0f) * radiusX, centerY + sin(step * 19.0f) * radiusY);
            glVertex2f(centerX + cos(step * 20.0f) * radiusX, centerY + sin(step * 20.0f) * radiusY);
            glVertex2f(centerX + cos(step * 21.0f) * radiusX, centerY + sin(step * 21.0f) * radiusY);
            glVertex2f(centerX + cos(step * 22.0f) * radiusX, centerY + sin(step * 22.0f) * radiusY);
            glVertex2f(centerX + cos(step * 23.0f) * radiusX, centerY + sin(step * 23.0f) * radiusY);
            glEnd();
        }

        glColor3f(0.20f, 0.20f, 0.22f);
        {
            float left = 0.0f;
            float bottom = homeRoadBottomY;
            float right = left + 1280.0f;
            float top = bottom + homeRoadHeight;
            glBegin(GL_QUADS);
            glVertex2f(left, bottom);
            glVertex2f(right, bottom);
            glVertex2f(right, top);
            glVertex2f(left, top);
            glEnd();
        }
        glColor3f(0.66f, 0.66f, 0.70f);
        {
            float left = 0.0f;
            float bottom = homeRoadBottomY + homeRoadHeight - 3.0f;
            float right = left + 1280.0f;
            float top = bottom + 3.0f;
            glBegin(GL_QUADS);
            glVertex2f(left, bottom);
            glVertex2f(right, bottom);
            glVertex2f(right, top);
            glVertex2f(left, top);
            glEnd();
        }
        glColor3f(0.26f, 0.58f, 0.30f);
        {
            float left = 0.0f;
            float bottom = homeGrassBottomY;
            float right = left + 1280.0f;
            float top = bottom + homeGrassHeight;
            glBegin(GL_QUADS);
            glVertex2f(left, bottom);
            glVertex2f(right, bottom);
            glVertex2f(right, top);
            glVertex2f(left, top);
            glEnd();
        }
        glColor3f(0.52f, 0.52f, 0.56f);
        glBegin(GL_POLYGON);
        glVertex2f(250.0f, 110.0f);
        glVertex2f(430.0f, 110.0f);
        glVertex2f(500.0f, 300.0f);
        glVertex2f(300.0f, 300.0f);
        glEnd();
    } else {
        float skyBottomR = 0.24f;
        float skyBottomG = 0.26f;
        float skyBottomB = 0.29f;
        float skyTopR = 0.36f;
        float skyTopG = 0.38f;
        float skyTopB = 0.41f;
        glBegin(GL_QUADS);
        glColor3f(skyBottomR, skyBottomG, skyBottomB);
        glVertex2f(0.0f, 0.0f);
        glColor3f(skyBottomR, skyBottomG, skyBottomB);
        glVertex2f(1280.0f, 0.0f);
        glColor3f(skyTopR, skyTopG, skyTopB);
        glVertex2f(1280.0f, 720.0f);
        glColor3f(skyTopR, skyTopG, skyTopB);
        glVertex2f(0.0f, 720.0f);
        glEnd();

        glColor3f(0.28f, 0.30f, 0.32f);
        {
            float centerX = 220.0f + cloudOffsetX_layerA * 0.30f;
            float centerY = 632.0f;
            float radiusX = 170.0f;
            float radiusY = 46.0f;
            float step = 2.0f * PI_VALUE / 40.0f;
            glBegin(GL_POLYGON);
            glVertex2f(centerX + cos(step * 0.0f) * radiusX, centerY + sin(step * 0.0f) * radiusY);
            glVertex2f(centerX + cos(step * 1.0f) * radiusX, centerY + sin(step * 1.0f) * radiusY);
            glVertex2f(centerX + cos(step * 2.0f) * radiusX, centerY + sin(step * 2.0f) * radiusY);
            glVertex2f(centerX + cos(step * 3.0f) * radiusX, centerY + sin(step * 3.0f) * radiusY);
            glVertex2f(centerX + cos(step * 4.0f) * radiusX, centerY + sin(step * 4.0f) * radiusY);
            glVertex2f(centerX + cos(step * 5.0f) * radiusX, centerY + sin(step * 5.0f) * radiusY);
            glVertex2f(centerX + cos(step * 6.0f) * radiusX, centerY + sin(step * 6.0f) * radiusY);
            glVertex2f(centerX + cos(step * 7.0f) * radiusX, centerY + sin(step * 7.0f) * radiusY);
            glVertex2f(centerX + cos(step * 8.0f) * radiusX, centerY + sin(step * 8.0f) * radiusY);
            glVertex2f(centerX + cos(step * 9.0f) * radiusX, centerY + sin(step * 9.0f) * radiusY);
            glVertex2f(centerX + cos(step * 10.0f) * radiusX, centerY + sin(step * 10.0f) * radiusY);
            glVertex2f(centerX + cos(step * 11.0f) * radiusX, centerY + sin(step * 11.0f) * radiusY);
            glVertex2f(centerX + cos(step * 12.0f) * radiusX, centerY + sin(step * 12.0f) * radiusY);
            glVertex2f(centerX + cos(step * 13.0f) * radiusX, centerY + sin(step * 13.0f) * radiusY);
            glVertex2f(centerX + cos(step * 14.0f) * radiusX, centerY + sin(step * 14.0f) * radiusY);
            glVertex2f(centerX + cos(step * 15.0f) * radiusX, centerY + sin(step * 15.0f) * radiusY);
            glVertex2f(centerX + cos(step * 16.0f) * radiusX, centerY + sin(step * 16.0f) * radiusY);
            glVertex2f(centerX + cos(step * 17.0f) * radiusX, centerY + sin(step * 17.0f) * radiusY);
            glVertex2f(centerX + cos(step * 18.0f) * radiusX, centerY + sin(step * 18.0f) * radiusY);
            glVertex2f(centerX + cos(step * 19.0f) * radiusX, centerY + sin(step * 19.0f) * radiusY);
            glVertex2f(centerX + cos(step * 20.0f) * radiusX, centerY + sin(step * 20.0f) * radiusY);
            glVertex2f(centerX + cos(step * 21.0f) * radiusX, centerY + sin(step * 21.0f) * radiusY);
            glVertex2f(centerX + cos(step * 22.0f) * radiusX, centerY + sin(step * 22.0f) * radiusY);
            glVertex2f(centerX + cos(step * 23.0f) * radiusX, centerY + sin(step * 23.0f) * radiusY);
            glVertex2f(centerX + cos(step * 24.0f) * radiusX, centerY + sin(step * 24.0f) * radiusY);
            glVertex2f(centerX + cos(step * 25.0f) * radiusX, centerY + sin(step * 25.0f) * radiusY);
            glVertex2f(centerX + cos(step * 26.0f) * radiusX, centerY + sin(step * 26.0f) * radiusY);
            glVertex2f(centerX + cos(step * 27.0f) * radiusX, centerY + sin(step * 27.0f) * radiusY);
            glVertex2f(centerX + cos(step * 28.0f) * radiusX, centerY + sin(step * 28.0f) * radiusY);
            glVertex2f(centerX + cos(step * 29.0f) * radiusX, centerY + sin(step * 29.0f) * radiusY);
            glVertex2f(centerX + cos(step * 30.0f) * radiusX, centerY + sin(step * 30.0f) * radiusY);
            glVertex2f(centerX + cos(step * 31.0f) * radiusX, centerY + sin(step * 31.0f) * radiusY);
            glVertex2f(centerX + cos(step * 32.0f) * radiusX, centerY + sin(step * 32.0f) * radiusY);
            glVertex2f(centerX + cos(step * 33.0f) * radiusX, centerY + sin(step * 33.0f) * radiusY);
            glVertex2f(centerX + cos(step * 34.0f) * radiusX, centerY + sin(step * 34.0f) * radiusY);
            glVertex2f(centerX + cos(step * 35.0f) * radiusX, centerY + sin(step * 35.0f) * radiusY);
            glVertex2f(centerX + cos(step * 36.0f) * radiusX, centerY + sin(step * 36.0f) * radiusY);
            glVertex2f(centerX + cos(step * 37.0f) * radiusX, centerY + sin(step * 37.0f) * radiusY);
            glVertex2f(centerX + cos(step * 38.0f) * radiusX, centerY + sin(step * 38.0f) * radiusY);
            glVertex2f(centerX + cos(step * 39.0f) * radiusX, centerY + sin(step * 39.0f) * radiusY);
            glEnd();
        }
        {
            float centerX = 620.0f + cloudOffsetX_layerB * 0.30f;
            float centerY = 654.0f;
            float radiusX = 190.0f;
            float radiusY = 52.0f;
            float step = 2.0f * PI_VALUE / 40.0f;
            glBegin(GL_POLYGON);
            glVertex2f(centerX + cos(step * 0.0f) * radiusX, centerY + sin(step * 0.0f) * radiusY);
            glVertex2f(centerX + cos(step * 1.0f) * radiusX, centerY + sin(step * 1.0f) * radiusY);
            glVertex2f(centerX + cos(step * 2.0f) * radiusX, centerY + sin(step * 2.0f) * radiusY);
            glVertex2f(centerX + cos(step * 3.0f) * radiusX, centerY + sin(step * 3.0f) * radiusY);
            glVertex2f(centerX + cos(step * 4.0f) * radiusX, centerY + sin(step * 4.0f) * radiusY);
            glVertex2f(centerX + cos(step * 5.0f) * radiusX, centerY + sin(step * 5.0f) * radiusY);
            glVertex2f(centerX + cos(step * 6.0f) * radiusX, centerY + sin(step * 6.0f) * radiusY);
            glVertex2f(centerX + cos(step * 7.0f) * radiusX, centerY + sin(step * 7.0f) * radiusY);
            glVertex2f(centerX + cos(step * 8.0f) * radiusX, centerY + sin(step * 8.0f) * radiusY);
            glVertex2f(centerX + cos(step * 9.0f) * radiusX, centerY + sin(step * 9.0f) * radiusY);
            glVertex2f(centerX + cos(step * 10.0f) * radiusX, centerY + sin(step * 10.0f) * radiusY);
            glVertex2f(centerX + cos(step * 11.0f) * radiusX, centerY + sin(step * 11.0f) * radiusY);
            glVertex2f(centerX + cos(step * 12.0f) * radiusX, centerY + sin(step * 12.0f) * radiusY);
            glVertex2f(centerX + cos(step * 13.0f) * radiusX, centerY + sin(step * 13.0f) * radiusY);
            glVertex2f(centerX + cos(step * 14.0f) * radiusX, centerY + sin(step * 14.0f) * radiusY);
            glVertex2f(centerX + cos(step * 15.0f) * radiusX, centerY + sin(step * 15.0f) * radiusY);
            glVertex2f(centerX + cos(step * 16.0f) * radiusX, centerY + sin(step * 16.0f) * radiusY);
            glVertex2f(centerX + cos(step * 17.0f) * radiusX, centerY + sin(step * 17.0f) * radiusY);
            glVertex2f(centerX + cos(step * 18.0f) * radiusX, centerY + sin(step * 18.0f) * radiusY);
            glVertex2f(centerX + cos(step * 19.0f) * radiusX, centerY + sin(step * 19.0f) * radiusY);
            glVertex2f(centerX + cos(step * 20.0f) * radiusX, centerY + sin(step * 20.0f) * radiusY);
            glVertex2f(centerX + cos(step * 21.0f) * radiusX, centerY + sin(step * 21.0f) * radiusY);
            glVertex2f(centerX + cos(step * 22.0f) * radiusX, centerY + sin(step * 22.0f) * radiusY);
            glVertex2f(centerX + cos(step * 23.0f) * radiusX, centerY + sin(step * 23.0f) * radiusY);
            glVertex2f(centerX + cos(step * 24.0f) * radiusX, centerY + sin(step * 24.0f) * radiusY);
            glVertex2f(centerX + cos(step * 25.0f) * radiusX, centerY + sin(step * 25.0f) * radiusY);
            glVertex2f(centerX + cos(step * 26.0f) * radiusX, centerY + sin(step * 26.0f) * radiusY);
            glVertex2f(centerX + cos(step * 27.0f) * radiusX, centerY + sin(step * 27.0f) * radiusY);
            glVertex2f(centerX + cos(step * 28.0f) * radiusX, centerY + sin(step * 28.0f) * radiusY);
            glVertex2f(centerX + cos(step * 29.0f) * radiusX, centerY + sin(step * 29.0f) * radiusY);
            glVertex2f(centerX + cos(step * 30.0f) * radiusX, centerY + sin(step * 30.0f) * radiusY);
            glVertex2f(centerX + cos(step * 31.0f) * radiusX, centerY + sin(step * 31.0f) * radiusY);
            glVertex2f(centerX + cos(step * 32.0f) * radiusX, centerY + sin(step * 32.0f) * radiusY);
            glVertex2f(centerX + cos(step * 33.0f) * radiusX, centerY + sin(step * 33.0f) * radiusY);
            glVertex2f(centerX + cos(step * 34.0f) * radiusX, centerY + sin(step * 34.0f) * radiusY);
            glVertex2f(centerX + cos(step * 35.0f) * radiusX, centerY + sin(step * 35.0f) * radiusY);
            glVertex2f(centerX + cos(step * 36.0f) * radiusX, centerY + sin(step * 36.0f) * radiusY);
            glVertex2f(centerX + cos(step * 37.0f) * radiusX, centerY + sin(step * 37.0f) * radiusY);
            glVertex2f(centerX + cos(step * 38.0f) * radiusX, centerY + sin(step * 38.0f) * radiusY);
            glVertex2f(centerX + cos(step * 39.0f) * radiusX, centerY + sin(step * 39.0f) * radiusY);
            glEnd();
        }

        glColor3f(0.20f, 0.20f, 0.22f);
        {
            float left = 0.0f;
            float bottom = homeRoadBottomY;
            float right = left + 1280.0f;
            float top = bottom + homeRoadHeight;
            glBegin(GL_QUADS);
            glVertex2f(left, bottom);
            glVertex2f(right, bottom);
            glVertex2f(right, top);
            glVertex2f(left, top);
            glEnd();
        }
        glColor3f(0.66f, 0.66f, 0.70f);
        {
            float left = 0.0f;
            float bottom = homeRoadBottomY + homeRoadHeight - 3.0f;
            float right = left + 1280.0f;
            float top = bottom + 3.0f;
            glBegin(GL_QUADS);
            glVertex2f(left, bottom);
            glVertex2f(right, bottom);
            glVertex2f(right, top);
            glVertex2f(left, top);
            glEnd();
        }
        glColor3f(0.26f, 0.58f, 0.30f);
        {
            float left = 0.0f;
            float bottom = homeGrassBottomY;
            float right = left + 1280.0f;
            float top = bottom + homeGrassHeight;
            glBegin(GL_QUADS);
            glVertex2f(left, bottom);
            glVertex2f(right, bottom);
            glVertex2f(right, top);
            glVertex2f(left, top);
            glEnd();
        }
        glColor3f(0.52f, 0.52f, 0.56f);
        glBegin(GL_POLYGON);
        glVertex2f(250.0f, 110.0f);
        glVertex2f(430.0f, 110.0f);
        glVertex2f(500.0f, 300.0f);
        glVertex2f(300.0f, 300.0f);
        glEnd();
    }

    {
        float treeBaseX = 88.0f;
        float treeBaseY = 120.0f;
        glColor3f(0.42f, 0.26f, 0.14f);
        {
            float left = treeBaseX - 9.0f;
            float bottom = treeBaseY;
            float right = treeBaseX + 9.0f;
            float top = treeBaseY + 78.0f;
            glBegin(GL_QUADS);
            glVertex2f(left, bottom);
            glVertex2f(right, bottom);
            glVertex2f(right, top);
            glVertex2f(left, top);
            glEnd();
        }
        glColor3f(0.16f, 0.46f, 0.18f);
        glBegin(GL_POLYGON);
        glVertex2f(treeBaseX - 44.0f, treeBaseY + 78.0f);
        glVertex2f(treeBaseX - 26.0f, treeBaseY + 112.0f);
        glVertex2f(treeBaseX + 10.0f, treeBaseY + 124.0f);
        glVertex2f(treeBaseX + 46.0f, treeBaseY + 98.0f);
        glVertex2f(treeBaseX + 26.0f, treeBaseY + 68.0f);
        glVertex2f(treeBaseX - 16.0f, treeBaseY + 62.0f);
        glEnd();
        glColor3f(0.20f, 0.56f, 0.22f);
        glBegin(GL_POLYGON);
        glVertex2f(treeBaseX - 30.0f, treeBaseY + 96.0f);
        glVertex2f(treeBaseX - 8.0f, treeBaseY + 136.0f);
        glVertex2f(treeBaseX + 22.0f, treeBaseY + 140.0f);
        glVertex2f(treeBaseX + 40.0f, treeBaseY + 108.0f);
        glVertex2f(treeBaseX + 14.0f, treeBaseY + 86.0f);
        glVertex2f(treeBaseX - 18.0f, treeBaseY + 84.0f);
        glEnd();
    }

    {
        float treeBaseX = 640.0f;
        float treeBaseY = 132.0f;
        glColor3f(0.42f, 0.26f, 0.14f);
        {
            float left = treeBaseX - 9.0f;
            float bottom = treeBaseY;
            float right = treeBaseX + 9.0f;
            float top = treeBaseY + 78.0f;
            glBegin(GL_QUADS);
            glVertex2f(left, bottom);
            glVertex2f(right, bottom);
            glVertex2f(right, top);
            glVertex2f(left, top);
            glEnd();
        }
        glColor3f(0.16f, 0.46f, 0.18f);
        glBegin(GL_POLYGON);
        glVertex2f(treeBaseX - 44.0f, treeBaseY + 78.0f);
        glVertex2f(treeBaseX - 26.0f, treeBaseY + 112.0f);
        glVertex2f(treeBaseX + 10.0f, treeBaseY + 124.0f);
        glVertex2f(treeBaseX + 46.0f, treeBaseY + 98.0f);
        glVertex2f(treeBaseX + 26.0f, treeBaseY + 68.0f);
        glVertex2f(treeBaseX - 16.0f, treeBaseY + 62.0f);
        glEnd();
        glColor3f(0.20f, 0.56f, 0.22f);
        glBegin(GL_POLYGON);
        glVertex2f(treeBaseX - 30.0f, treeBaseY + 96.0f);
        glVertex2f(treeBaseX - 8.0f, treeBaseY + 136.0f);
        glVertex2f(treeBaseX + 22.0f, treeBaseY + 140.0f);
        glVertex2f(treeBaseX + 40.0f, treeBaseY + 108.0f);
        glVertex2f(treeBaseX + 14.0f, treeBaseY + 86.0f);
        glVertex2f(treeBaseX - 18.0f, treeBaseY + 84.0f);
        glEnd();
    }

    {
        float treeBaseX = 748.0f;
        float treeBaseY = 124.0f;
        glColor3f(0.42f, 0.26f, 0.14f);
        {
            float left = treeBaseX - 9.0f;
            float bottom = treeBaseY;
            float right = treeBaseX + 9.0f;
            float top = treeBaseY + 78.0f;
            glBegin(GL_QUADS);
            glVertex2f(left, bottom);
            glVertex2f(right, bottom);
            glVertex2f(right, top);
            glVertex2f(left, top);
            glEnd();
        }
        glColor3f(0.16f, 0.46f, 0.18f);
        glBegin(GL_POLYGON);
        glVertex2f(treeBaseX - 44.0f, treeBaseY + 78.0f);
        glVertex2f(treeBaseX - 26.0f, treeBaseY + 112.0f);
        glVertex2f(treeBaseX + 10.0f, treeBaseY + 124.0f);
        glVertex2f(treeBaseX + 46.0f, treeBaseY + 98.0f);
        glVertex2f(treeBaseX + 26.0f, treeBaseY + 68.0f);
        glVertex2f(treeBaseX - 16.0f, treeBaseY + 62.0f);
        glEnd();
        glColor3f(0.20f, 0.56f, 0.22f);
        glBegin(GL_POLYGON);
        glVertex2f(treeBaseX - 30.0f, treeBaseY + 96.0f);
        glVertex2f(treeBaseX - 8.0f, treeBaseY + 136.0f);
        glVertex2f(treeBaseX + 22.0f, treeBaseY + 140.0f);
        glVertex2f(treeBaseX + 40.0f, treeBaseY + 108.0f);
        glVertex2f(treeBaseX + 14.0f, treeBaseY + 86.0f);
        glVertex2f(treeBaseX - 18.0f, treeBaseY + 84.0f);
        glEnd();
    }

    {
        float houseBaseX = homeHouseLeftX + 14.0f;
        float houseBaseY = homeHouseBottomY;
        float houseSectionW = 300.0f;
        float houseSectionH = 270.0f;
        float garageStartX = homeGarageLeftX;
        float garageBaseY = homeGarageBottomY;
        glColor3f(0.85f, 0.79f, 0.69f);
        {
            float left = houseBaseX;
            float bottom = houseBaseY;
            float right = left + houseSectionW;
            float top = bottom + 126.0f;
            glBegin(GL_QUADS);
            glVertex2f(left, bottom);
            glVertex2f(right, bottom);
            glVertex2f(right, top);
            glVertex2f(left, top);
            glEnd();
        }
        glColor3f(0.80f, 0.74f, 0.65f);
        {
            float left = houseBaseX;
            float bottom = houseBaseY + 126.0f;
            float right = left + houseSectionW;
            float top = houseBaseY + houseSectionH;
            glBegin(GL_QUADS);
            glVertex2f(left, bottom);
            glVertex2f(right, bottom);
            glVertex2f(right, top);
            glVertex2f(left, top);
            glEnd();
        }
        glColor3f(0.77f, 0.72f, 0.64f);
        {
            float left = garageStartX;
            float bottom = garageBaseY;
            float right = left + 240.0f;
            float top = bottom + 170.0f;
            glBegin(GL_QUADS);
            glVertex2f(left, bottom);
            glVertex2f(right, bottom);
            glVertex2f(right, top);
            glVertex2f(left, top);
            glEnd();
        }
        glColor3f(0.24f, 0.25f, 0.28f);
        glBegin(GL_TRIANGLES);
        glVertex2f(houseBaseX - 16.0f, houseBaseY + houseSectionH + 8.0f);
        glVertex2f(houseBaseX + houseSectionW + 16.0f, houseBaseY + houseSectionH + 8.0f);
        glVertex2f(houseBaseX + 174.0f, houseBaseY + houseSectionH + 130.0f);
        glEnd();
        glBegin(GL_TRIANGLES);
        glVertex2f(garageStartX - 16.0f, garageBaseY + 176.0f);
        glVertex2f(garageStartX + 256.0f, garageBaseY + 176.0f);
        glVertex2f(garageStartX + 112.0f, garageBaseY + 244.0f);
        glEnd();

        float doorX = houseBaseX + 94.0f;
        glColor3f(0.48f, 0.31f, 0.17f);
        {
            float left = doorX;
            float bottom = houseBaseY;
            float right = left + 56.0f;
            float top = bottom + 120.0f;
            glBegin(GL_QUADS);
            glVertex2f(left, bottom);
            glVertex2f(right, bottom);
            glVertex2f(right, top);
            glVertex2f(left, top);
            glEnd();
        }
        glColor3f(0.92f, 0.76f, 0.26f);
        {
            float centerX = doorX + 46.0f;
            float centerY = houseBaseY + 58.0f;
            float radiusX = 3.2f;
            float radiusY = 3.2f;
            float step = 2.0f * PI_VALUE / 18.0f;
            glBegin(GL_POLYGON);
            glVertex2f(centerX + cos(step * 0.0f) * radiusX, centerY + sin(step * 0.0f) * radiusY);
            glVertex2f(centerX + cos(step * 1.0f) * radiusX, centerY + sin(step * 1.0f) * radiusY);
            glVertex2f(centerX + cos(step * 2.0f) * radiusX, centerY + sin(step * 2.0f) * radiusY);
            glVertex2f(centerX + cos(step * 3.0f) * radiusX, centerY + sin(step * 3.0f) * radiusY);
            glVertex2f(centerX + cos(step * 4.0f) * radiusX, centerY + sin(step * 4.0f) * radiusY);
            glVertex2f(centerX + cos(step * 5.0f) * radiusX, centerY + sin(step * 5.0f) * radiusY);
            glVertex2f(centerX + cos(step * 6.0f) * radiusX, centerY + sin(step * 6.0f) * radiusY);
            glVertex2f(centerX + cos(step * 7.0f) * radiusX, centerY + sin(step * 7.0f) * radiusY);
            glVertex2f(centerX + cos(step * 8.0f) * radiusX, centerY + sin(step * 8.0f) * radiusY);
            glVertex2f(centerX + cos(step * 9.0f) * radiusX, centerY + sin(step * 9.0f) * radiusY);
            glVertex2f(centerX + cos(step * 10.0f) * radiusX, centerY + sin(step * 10.0f) * radiusY);
            glVertex2f(centerX + cos(step * 11.0f) * radiusX, centerY + sin(step * 11.0f) * radiusY);
            glVertex2f(centerX + cos(step * 12.0f) * radiusX, centerY + sin(step * 12.0f) * radiusY);
            glVertex2f(centerX + cos(step * 13.0f) * radiusX, centerY + sin(step * 13.0f) * radiusY);
            glVertex2f(centerX + cos(step * 14.0f) * radiusX, centerY + sin(step * 14.0f) * radiusY);
            glVertex2f(centerX + cos(step * 15.0f) * radiusX, centerY + sin(step * 15.0f) * radiusY);
            glVertex2f(centerX + cos(step * 16.0f) * radiusX, centerY + sin(step * 16.0f) * radiusY);
            glVertex2f(centerX + cos(step * 17.0f) * radiusX, centerY + sin(step * 17.0f) * radiusY);
            glEnd();
        }

        bool morning = currentScene == 1;
        bool houseLightOn = false;

        {
            float windowLeft = houseBaseX + 18.0f;
            float windowBottom = houseBaseY + 58.0f;
            float windowWidth = 56.0f;
            float windowHeight = 54.0f;
            glColor3f(0.20f, 0.18f, 0.16f);
            {
                float left = windowLeft;
                float bottom = windowBottom;
                float right = left + windowWidth;
                float top = bottom + windowHeight;
                glBegin(GL_QUADS);
                glVertex2f(left, bottom);
                glVertex2f(right, bottom);
                glVertex2f(right, top);
                glVertex2f(left, top);
                glEnd();
            }
            if (morning) glColor3f(0.60f, 0.74f, 0.84f);
            else if (houseLightOn) glColor3f(0.98f, 0.88f, 0.54f);
            else glColor3f(0.12f, 0.14f, 0.18f);
            {
                float left = windowLeft + 3.0f;
                float bottom = windowBottom + 3.0f;
                float right = windowLeft + windowWidth - 3.0f;
                float top = windowBottom + windowHeight - 3.0f;
                glBegin(GL_QUADS);
                glVertex2f(left, bottom);
                glVertex2f(right, bottom);
                glVertex2f(right, top);
                glVertex2f(left, top);
                glEnd();
            }
            glColor3f(0.18f, 0.16f, 0.14f);
            {
                float left = windowLeft + windowWidth * 0.5f - 1.0f;
                float bottom = windowBottom + 3.0f;
                float right = left + 2.0f;
                float top = windowBottom + windowHeight - 3.0f;
                glBegin(GL_QUADS);
                glVertex2f(left, bottom);
                glVertex2f(right, bottom);
                glVertex2f(right, top);
                glVertex2f(left, top);
                glEnd();
            }
            {
                float left = windowLeft + 3.0f;
                float bottom = windowBottom + windowHeight * 0.5f - 1.0f;
                float right = windowLeft + windowWidth - 3.0f;
                float top = bottom + 2.0f;
                glBegin(GL_QUADS);
                glVertex2f(left, bottom);
                glVertex2f(right, bottom);
                glVertex2f(right, top);
                glVertex2f(left, top);
                glEnd();
            }
        }

        {
            float windowLeft = houseBaseX + 168.0f;
            float windowBottom = houseBaseY + 58.0f;
            float windowWidth = 56.0f;
            float windowHeight = 54.0f;
            glColor3f(0.20f, 0.18f, 0.16f);
            {
                float left = windowLeft;
                float bottom = windowBottom;
                float right = left + windowWidth;
                float top = bottom + windowHeight;
                glBegin(GL_QUADS);
                glVertex2f(left, bottom);
                glVertex2f(right, bottom);
                glVertex2f(right, top);
                glVertex2f(left, top);
                glEnd();
            }
            if (morning) glColor3f(0.60f, 0.74f, 0.84f);
            else if (houseLightOn) glColor3f(0.98f, 0.88f, 0.54f);
            else glColor3f(0.12f, 0.14f, 0.18f);
            {
                float left = windowLeft + 3.0f;
                float bottom = windowBottom + 3.0f;
                float right = windowLeft + windowWidth - 3.0f;
                float top = windowBottom + windowHeight - 3.0f;
                glBegin(GL_QUADS);
                glVertex2f(left, bottom);
                glVertex2f(right, bottom);
                glVertex2f(right, top);
                glVertex2f(left, top);
                glEnd();
            }
            glColor3f(0.18f, 0.16f, 0.14f);
            {
                float left = windowLeft + windowWidth * 0.5f - 1.0f;
                float bottom = windowBottom + 3.0f;
                float right = left + 2.0f;
                float top = windowBottom + windowHeight - 3.0f;
                glBegin(GL_QUADS);
                glVertex2f(left, bottom);
                glVertex2f(right, bottom);
                glVertex2f(right, top);
                glVertex2f(left, top);
                glEnd();
            }
            {
                float left = windowLeft + 3.0f;
                float bottom = windowBottom + windowHeight * 0.5f - 1.0f;
                float right = windowLeft + windowWidth - 3.0f;
                float top = bottom + 2.0f;
                glBegin(GL_QUADS);
                glVertex2f(left, bottom);
                glVertex2f(right, bottom);
                glVertex2f(right, top);
                glVertex2f(left, top);
                glEnd();
            }
        }

        {
            float windowLeft = houseBaseX + 18.0f;
            float windowBottom = houseBaseY + 166.0f;
            float windowWidth = 56.0f;
            float windowHeight = 54.0f;
            glColor3f(0.20f, 0.18f, 0.16f);
            {
                float left = windowLeft;
                float bottom = windowBottom;
                float right = left + windowWidth;
                float top = bottom + windowHeight;
                glBegin(GL_QUADS);
                glVertex2f(left, bottom);
                glVertex2f(right, bottom);
                glVertex2f(right, top);
                glVertex2f(left, top);
                glEnd();
            }
            if (morning) glColor3f(0.60f, 0.74f, 0.84f);
            else if (houseLightOn) glColor3f(0.98f, 0.88f, 0.54f);
            else glColor3f(0.12f, 0.14f, 0.18f);
            {
                float left = windowLeft + 3.0f;
                float bottom = windowBottom + 3.0f;
                float right = windowLeft + windowWidth - 3.0f;
                float top = windowBottom + windowHeight - 3.0f;
                glBegin(GL_QUADS);
                glVertex2f(left, bottom);
                glVertex2f(right, bottom);
                glVertex2f(right, top);
                glVertex2f(left, top);
                glEnd();
            }
            glColor3f(0.18f, 0.16f, 0.14f);
            {
                float left = windowLeft + windowWidth * 0.5f - 1.0f;
                float bottom = windowBottom + 3.0f;
                float right = left + 2.0f;
                float top = windowBottom + windowHeight - 3.0f;
                glBegin(GL_QUADS);
                glVertex2f(left, bottom);
                glVertex2f(right, bottom);
                glVertex2f(right, top);
                glVertex2f(left, top);
                glEnd();
            }
            {
                float left = windowLeft + 3.0f;
                float bottom = windowBottom + windowHeight * 0.5f - 1.0f;
                float right = windowLeft + windowWidth - 3.0f;
                float top = bottom + 2.0f;
                glBegin(GL_QUADS);
                glVertex2f(left, bottom);
                glVertex2f(right, bottom);
                glVertex2f(right, top);
                glVertex2f(left, top);
                glEnd();
            }
        }

        {
            float windowLeft = houseBaseX + 122.0f;
            float windowBottom = houseBaseY + 166.0f;
            float windowWidth = 56.0f;
            float windowHeight = 54.0f;
            glColor3f(0.20f, 0.18f, 0.16f);
            {
                float left = windowLeft;
                float bottom = windowBottom;
                float right = left + windowWidth;
                float top = bottom + windowHeight;
                glBegin(GL_QUADS);
                glVertex2f(left, bottom);
                glVertex2f(right, bottom);
                glVertex2f(right, top);
                glVertex2f(left, top);
                glEnd();
            }
            if (morning) glColor3f(0.60f, 0.74f, 0.84f);
            else if (houseLightOn) glColor3f(0.98f, 0.88f, 0.54f);
            else glColor3f(0.12f, 0.14f, 0.18f);
            {
                float left = windowLeft + 3.0f;
                float bottom = windowBottom + 3.0f;
                float right = windowLeft + windowWidth - 3.0f;
                float top = windowBottom + windowHeight - 3.0f;
                glBegin(GL_QUADS);
                glVertex2f(left, bottom);
                glVertex2f(right, bottom);
                glVertex2f(right, top);
                glVertex2f(left, top);
                glEnd();
            }
            glColor3f(0.18f, 0.16f, 0.14f);
            {
                float left = windowLeft + windowWidth * 0.5f - 1.0f;
                float bottom = windowBottom + 3.0f;
                float right = left + 2.0f;
                float top = windowBottom + windowHeight - 3.0f;
                glBegin(GL_QUADS);
                glVertex2f(left, bottom);
                glVertex2f(right, bottom);
                glVertex2f(right, top);
                glVertex2f(left, top);
                glEnd();
            }
            {
                float left = windowLeft + 3.0f;
                float bottom = windowBottom + windowHeight * 0.5f - 1.0f;
                float right = windowLeft + windowWidth - 3.0f;
                float top = bottom + 2.0f;
                glBegin(GL_QUADS);
                glVertex2f(left, bottom);
                glVertex2f(right, bottom);
                glVertex2f(right, top);
                glVertex2f(left, top);
                glEnd();
            }
        }

        {
            float windowLeft = houseBaseX + 226.0f;
            float windowBottom = houseBaseY + 166.0f;
            float windowWidth = 56.0f;
            float windowHeight = 54.0f;
            glColor3f(0.20f, 0.18f, 0.16f);
            {
                float left = windowLeft;
                float bottom = windowBottom;
                float right = left + windowWidth;
                float top = bottom + windowHeight;
                glBegin(GL_QUADS);
                glVertex2f(left, bottom);
                glVertex2f(right, bottom);
                glVertex2f(right, top);
                glVertex2f(left, top);
                glEnd();
            }
            if (morning) glColor3f(0.60f, 0.74f, 0.84f);
            else if (houseLightOn) glColor3f(0.98f, 0.88f, 0.54f);
            else glColor3f(0.12f, 0.14f, 0.18f);
            {
                float left = windowLeft + 3.0f;
                float bottom = windowBottom + 3.0f;
                float right = windowLeft + windowWidth - 3.0f;
                float top = windowBottom + windowHeight - 3.0f;
                glBegin(GL_QUADS);
                glVertex2f(left, bottom);
                glVertex2f(right, bottom);
                glVertex2f(right, top);
                glVertex2f(left, top);
                glEnd();
            }
            glColor3f(0.18f, 0.16f, 0.14f);
            {
                float left = windowLeft + windowWidth * 0.5f - 1.0f;
                float bottom = windowBottom + 3.0f;
                float right = left + 2.0f;
                float top = windowBottom + windowHeight - 3.0f;
                glBegin(GL_QUADS);
                glVertex2f(left, bottom);
                glVertex2f(right, bottom);
                glVertex2f(right, top);
                glVertex2f(left, top);
                glEnd();
            }
            {
                float left = windowLeft + 3.0f;
                float bottom = windowBottom + windowHeight * 0.5f - 1.0f;
                float right = windowLeft + windowWidth - 3.0f;
                float top = bottom + 2.0f;
                glBegin(GL_QUADS);
                glVertex2f(left, bottom);
                glVertex2f(right, bottom);
                glVertex2f(right, top);
                glVertex2f(left, top);
                glEnd();
            }
        }

        {
            float windowLeft = garageStartX + 12.0f;
            float windowBottom = garageBaseY + 98.0f;
            float windowWidth = 42.0f;
            float windowHeight = 38.0f;
            glColor3f(0.20f, 0.18f, 0.16f);
            {
                float left = windowLeft;
                float bottom = windowBottom;
                float right = left + windowWidth;
                float top = bottom + windowHeight;
                glBegin(GL_QUADS);
                glVertex2f(left, bottom);
                glVertex2f(right, bottom);
                glVertex2f(right, top);
                glVertex2f(left, top);
                glEnd();
            }
            if (morning) glColor3f(0.60f, 0.74f, 0.84f);
            else if (houseLightOn) glColor3f(0.98f, 0.88f, 0.54f);
            else glColor3f(0.12f, 0.14f, 0.18f);
            {
                float left = windowLeft + 3.0f;
                float bottom = windowBottom + 3.0f;
                float right = windowLeft + windowWidth - 3.0f;
                float top = windowBottom + windowHeight - 3.0f;
                glBegin(GL_QUADS);
                glVertex2f(left, bottom);
                glVertex2f(right, bottom);
                glVertex2f(right, top);
                glVertex2f(left, top);
                glEnd();
            }
            glColor3f(0.18f, 0.16f, 0.14f);
            {
                float left = windowLeft + windowWidth * 0.5f - 1.0f;
                float bottom = windowBottom + 3.0f;
                float right = left + 2.0f;
                float top = windowBottom + windowHeight - 3.0f;
                glBegin(GL_QUADS);
                glVertex2f(left, bottom);
                glVertex2f(right, bottom);
                glVertex2f(right, top);
                glVertex2f(left, top);
                glEnd();
            }
            {
                float left = windowLeft + 3.0f;
                float bottom = windowBottom + windowHeight * 0.5f - 1.0f;
                float right = windowLeft + windowWidth - 3.0f;
                float top = bottom + 2.0f;
                glBegin(GL_QUADS);
                glVertex2f(left, bottom);
                glVertex2f(right, bottom);
                glVertex2f(right, top);
                glVertex2f(left, top);
                glEnd();
            }
        }

        glColor3f(0.08f, 0.09f, 0.11f);
        {
            float left = homeGarageDoorLeftX;
            float bottom = homeGarageDoorBottomY;
            float right = left + homeGarageDoorWidth;
            float top = bottom + homeGarageDoorHeight;
            glBegin(GL_QUADS);
            glVertex2f(left, bottom);
            glVertex2f(right, bottom);
            glVertex2f(right, top);
            glVertex2f(left, top);
            glEnd();
        }
        glColor3f(0.46f, 0.48f, 0.52f);
        {
            float stripeStep = 126.0f / 6.0f;
            float stripeW = homeGarageDoorWidth - 16.0f;
            float stripeH = 2.0f;
            float stripeLeft = homeGarageDoorLeftX + 8.0f;
            float stripeY1 = homeGarageDoorBottomY + stripeStep * 1.0f;
            float stripeY2 = homeGarageDoorBottomY + stripeStep * 2.0f;
            float stripeY3 = homeGarageDoorBottomY + stripeStep * 3.0f;
            float stripeY4 = homeGarageDoorBottomY + stripeStep * 4.0f;
            float stripeY5 = homeGarageDoorBottomY + stripeStep * 5.0f;
            float stripeRight = stripeLeft + stripeW;
            float stripeTop = stripeY1 + stripeH;
            glBegin(GL_QUADS);
            glVertex2f(stripeLeft, stripeY1);
            glVertex2f(stripeRight, stripeY1);
            glVertex2f(stripeRight, stripeTop);
            glVertex2f(stripeLeft, stripeTop);
            glEnd();
            stripeTop = stripeY2 + stripeH;
            glBegin(GL_QUADS);
            glVertex2f(stripeLeft, stripeY2);
            glVertex2f(stripeRight, stripeY2);
            glVertex2f(stripeRight, stripeTop);
            glVertex2f(stripeLeft, stripeTop);
            glEnd();
            stripeTop = stripeY3 + stripeH;
            glBegin(GL_QUADS);
            glVertex2f(stripeLeft, stripeY3);
            glVertex2f(stripeRight, stripeY3);
            glVertex2f(stripeRight, stripeTop);
            glVertex2f(stripeLeft, stripeTop);
            glEnd();
            stripeTop = stripeY4 + stripeH;
            glBegin(GL_QUADS);
            glVertex2f(stripeLeft, stripeY4);
            glVertex2f(stripeRight, stripeY4);
            glVertex2f(stripeRight, stripeTop);
            glVertex2f(stripeLeft, stripeTop);
            glEnd();
            stripeTop = stripeY5 + stripeH;
            glBegin(GL_QUADS);
            glVertex2f(stripeLeft, stripeY5);
            glVertex2f(stripeRight, stripeY5);
            glVertex2f(stripeRight, stripeTop);
            glVertex2f(stripeLeft, stripeTop);
            glEnd();
        }
    }

    {
        float carX = scene1_carPosX;
        float carY = scene1_carPosY;
        float carR = 0.84f;
        float carG = 0.20f;
        float carB = 0.18f;
        float wheelAngle = wheelRotationAngle;
        bool headlightOn = false;
        bool tailLightOn = false;
        bool facingRight = true;

        glPushMatrix();
        glTranslatef(carX, carY, 0.0f);
        if (!facingRight) glScalef(-1.0f, 1.0f, 1.0f);

        glEnable(GL_BLEND);
        glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
        glColor4f(0.0f, 0.0f, 0.0f, 0.24f);
        {
            float centerX = 0.0f;
            float centerY = -10.0f;
            float radiusX = 74.0f;
            float radiusY = 11.0f;
            float step = 2.0f * PI_VALUE / 36.0f;
            glBegin(GL_POLYGON);
            glVertex2f(centerX + cos(step * 0.0f) * radiusX, centerY + sin(step * 0.0f) * radiusY);
            glVertex2f(centerX + cos(step * 1.0f) * radiusX, centerY + sin(step * 1.0f) * radiusY);
            glVertex2f(centerX + cos(step * 2.0f) * radiusX, centerY + sin(step * 2.0f) * radiusY);
            glVertex2f(centerX + cos(step * 3.0f) * radiusX, centerY + sin(step * 3.0f) * radiusY);
            glVertex2f(centerX + cos(step * 4.0f) * radiusX, centerY + sin(step * 4.0f) * radiusY);
            glVertex2f(centerX + cos(step * 5.0f) * radiusX, centerY + sin(step * 5.0f) * radiusY);
            glVertex2f(centerX + cos(step * 6.0f) * radiusX, centerY + sin(step * 6.0f) * radiusY);
            glVertex2f(centerX + cos(step * 7.0f) * radiusX, centerY + sin(step * 7.0f) * radiusY);
            glVertex2f(centerX + cos(step * 8.0f) * radiusX, centerY + sin(step * 8.0f) * radiusY);
            glVertex2f(centerX + cos(step * 9.0f) * radiusX, centerY + sin(step * 9.0f) * radiusY);
            glVertex2f(centerX + cos(step * 10.0f) * radiusX, centerY + sin(step * 10.0f) * radiusY);
            glVertex2f(centerX + cos(step * 11.0f) * radiusX, centerY + sin(step * 11.0f) * radiusY);
            glVertex2f(centerX + cos(step * 12.0f) * radiusX, centerY + sin(step * 12.0f) * radiusY);
            glVertex2f(centerX + cos(step * 13.0f) * radiusX, centerY + sin(step * 13.0f) * radiusY);
            glVertex2f(centerX + cos(step * 14.0f) * radiusX, centerY + sin(step * 14.0f) * radiusY);
            glVertex2f(centerX + cos(step * 15.0f) * radiusX, centerY + sin(step * 15.0f) * radiusY);
            glVertex2f(centerX + cos(step * 16.0f) * radiusX, centerY + sin(step * 16.0f) * radiusY);
            glVertex2f(centerX + cos(step * 17.0f) * radiusX, centerY + sin(step * 17.0f) * radiusY);
            glVertex2f(centerX + cos(step * 18.0f) * radiusX, centerY + sin(step * 18.0f) * radiusY);
            glVertex2f(centerX + cos(step * 19.0f) * radiusX, centerY + sin(step * 19.0f) * radiusY);
            glVertex2f(centerX + cos(step * 20.0f) * radiusX, centerY + sin(step * 20.0f) * radiusY);
            glVertex2f(centerX + cos(step * 21.0f) * radiusX, centerY + sin(step * 21.0f) * radiusY);
            glVertex2f(centerX + cos(step * 22.0f) * radiusX, centerY + sin(step * 22.0f) * radiusY);
            glVertex2f(centerX + cos(step * 23.0f) * radiusX, centerY + sin(step * 23.0f) * radiusY);
            glVertex2f(centerX + cos(step * 24.0f) * radiusX, centerY + sin(step * 24.0f) * radiusY);
            glVertex2f(centerX + cos(step * 25.0f) * radiusX, centerY + sin(step * 25.0f) * radiusY);
            glVertex2f(centerX + cos(step * 26.0f) * radiusX, centerY + sin(step * 26.0f) * radiusY);
            glVertex2f(centerX + cos(step * 27.0f) * radiusX, centerY + sin(step * 27.0f) * radiusY);
            glVertex2f(centerX + cos(step * 28.0f) * radiusX, centerY + sin(step * 28.0f) * radiusY);
            glVertex2f(centerX + cos(step * 29.0f) * radiusX, centerY + sin(step * 29.0f) * radiusY);
            glVertex2f(centerX + cos(step * 30.0f) * radiusX, centerY + sin(step * 30.0f) * radiusY);
            glVertex2f(centerX + cos(step * 31.0f) * radiusX, centerY + sin(step * 31.0f) * radiusY);
            glVertex2f(centerX + cos(step * 32.0f) * radiusX, centerY + sin(step * 32.0f) * radiusY);
            glVertex2f(centerX + cos(step * 33.0f) * radiusX, centerY + sin(step * 33.0f) * radiusY);
            glVertex2f(centerX + cos(step * 34.0f) * radiusX, centerY + sin(step * 34.0f) * radiusY);
            glVertex2f(centerX + cos(step * 35.0f) * radiusX, centerY + sin(step * 35.0f) * radiusY);
            glEnd();
        }

        glColor3f(carR, carG, carB);
        {
            float left = -71.0f;
            float bottom = 0.0f;
            float right = 71.0f;
            float top = 32.0f;
            glBegin(GL_QUADS);
            glVertex2f(left, bottom);
            glVertex2f(right, bottom);
            glVertex2f(right, top);
            glVertex2f(left, top);
            glEnd();
        }
        glBegin(GL_POLYGON);
        glVertex2f(-38.0f, 32.0f);
        glVertex2f(42.0f, 32.0f);
        glVertex2f(20.0f, 58.0f);
        glVertex2f(-20.0f, 58.0f);
        glEnd();
        glColor3f(0.64f, 0.84f, 0.96f);
        glBegin(GL_POLYGON);
        glVertex2f(-30.0f, 35.0f);
        glVertex2f(34.0f, 35.0f);
        glVertex2f(16.0f, 54.0f);
        glVertex2f(-14.0f, 54.0f);
        glEnd();
        glColor3f(0.18f, 0.18f, 0.20f);
        {
            float left = 64.0f;
            float bottom = 8.0f;
            float right = left + 10.0f;
            float top = bottom + 14.0f;
            glBegin(GL_QUADS);
            glVertex2f(left, bottom);
            glVertex2f(right, bottom);
            glVertex2f(right, top);
            glVertex2f(left, top);
            glEnd();
        }
        {
            float left = -74.0f;
            float bottom = 8.0f;
            float right = left + 10.0f;
            float top = bottom + 14.0f;
            glBegin(GL_QUADS);
            glVertex2f(left, bottom);
            glVertex2f(right, bottom);
            glVertex2f(right, top);
            glVertex2f(left, top);
            glEnd();
        }

        glPushMatrix();
        glTranslatef(-42.0f, -2.0f, 0.0f);
        glRotatef(wheelAngle, 0.0f, 0.0f, 1.0f);
        glColor3f(0.05f, 0.05f, 0.06f);
        {
            float centerX = 0.0f;
            float centerY = 0.0f;
            float radiusX = 16.0f;
            float radiusY = 16.0f;
            float step = 2.0f * PI_VALUE / 36.0f;
            glBegin(GL_POLYGON);
            glVertex2f(centerX + cos(step * 0.0f) * radiusX, centerY + sin(step * 0.0f) * radiusY);
            glVertex2f(centerX + cos(step * 1.0f) * radiusX, centerY + sin(step * 1.0f) * radiusY);
            glVertex2f(centerX + cos(step * 2.0f) * radiusX, centerY + sin(step * 2.0f) * radiusY);
            glVertex2f(centerX + cos(step * 3.0f) * radiusX, centerY + sin(step * 3.0f) * radiusY);
            glVertex2f(centerX + cos(step * 4.0f) * radiusX, centerY + sin(step * 4.0f) * radiusY);
            glVertex2f(centerX + cos(step * 5.0f) * radiusX, centerY + sin(step * 5.0f) * radiusY);
            glVertex2f(centerX + cos(step * 6.0f) * radiusX, centerY + sin(step * 6.0f) * radiusY);
            glVertex2f(centerX + cos(step * 7.0f) * radiusX, centerY + sin(step * 7.0f) * radiusY);
            glVertex2f(centerX + cos(step * 8.0f) * radiusX, centerY + sin(step * 8.0f) * radiusY);
            glVertex2f(centerX + cos(step * 9.0f) * radiusX, centerY + sin(step * 9.0f) * radiusY);
            glVertex2f(centerX + cos(step * 10.0f) * radiusX, centerY + sin(step * 10.0f) * radiusY);
            glVertex2f(centerX + cos(step * 11.0f) * radiusX, centerY + sin(step * 11.0f) * radiusY);
            glVertex2f(centerX + cos(step * 12.0f) * radiusX, centerY + sin(step * 12.0f) * radiusY);
            glVertex2f(centerX + cos(step * 13.0f) * radiusX, centerY + sin(step * 13.0f) * radiusY);
            glVertex2f(centerX + cos(step * 14.0f) * radiusX, centerY + sin(step * 14.0f) * radiusY);
            glVertex2f(centerX + cos(step * 15.0f) * radiusX, centerY + sin(step * 15.0f) * radiusY);
            glVertex2f(centerX + cos(step * 16.0f) * radiusX, centerY + sin(step * 16.0f) * radiusY);
            glVertex2f(centerX + cos(step * 17.0f) * radiusX, centerY + sin(step * 17.0f) * radiusY);
            glVertex2f(centerX + cos(step * 18.0f) * radiusX, centerY + sin(step * 18.0f) * radiusY);
            glVertex2f(centerX + cos(step * 19.0f) * radiusX, centerY + sin(step * 19.0f) * radiusY);
            glVertex2f(centerX + cos(step * 20.0f) * radiusX, centerY + sin(step * 20.0f) * radiusY);
            glVertex2f(centerX + cos(step * 21.0f) * radiusX, centerY + sin(step * 21.0f) * radiusY);
            glVertex2f(centerX + cos(step * 22.0f) * radiusX, centerY + sin(step * 22.0f) * radiusY);
            glVertex2f(centerX + cos(step * 23.0f) * radiusX, centerY + sin(step * 23.0f) * radiusY);
            glVertex2f(centerX + cos(step * 24.0f) * radiusX, centerY + sin(step * 24.0f) * radiusY);
            glVertex2f(centerX + cos(step * 25.0f) * radiusX, centerY + sin(step * 25.0f) * radiusY);
            glVertex2f(centerX + cos(step * 26.0f) * radiusX, centerY + sin(step * 26.0f) * radiusY);
            glVertex2f(centerX + cos(step * 27.0f) * radiusX, centerY + sin(step * 27.0f) * radiusY);
            glVertex2f(centerX + cos(step * 28.0f) * radiusX, centerY + sin(step * 28.0f) * radiusY);
            glVertex2f(centerX + cos(step * 29.0f) * radiusX, centerY + sin(step * 29.0f) * radiusY);
            glVertex2f(centerX + cos(step * 30.0f) * radiusX, centerY + sin(step * 30.0f) * radiusY);
            glVertex2f(centerX + cos(step * 31.0f) * radiusX, centerY + sin(step * 31.0f) * radiusY);
            glVertex2f(centerX + cos(step * 32.0f) * radiusX, centerY + sin(step * 32.0f) * radiusY);
            glVertex2f(centerX + cos(step * 33.0f) * radiusX, centerY + sin(step * 33.0f) * radiusY);
            glVertex2f(centerX + cos(step * 34.0f) * radiusX, centerY + sin(step * 34.0f) * radiusY);
            glVertex2f(centerX + cos(step * 35.0f) * radiusX, centerY + sin(step * 35.0f) * radiusY);
            glEnd();
        }
        glColor3f(0.15f, 0.15f, 0.17f);
        {
            float centerX = 0.0f;
            float centerY = 0.0f;
            float radiusX = 12.0f;
            float radiusY = 12.0f;
            float step = 2.0f * PI_VALUE / 30.0f;
            glBegin(GL_POLYGON);
            glVertex2f(centerX + cos(step * 0.0f) * radiusX, centerY + sin(step * 0.0f) * radiusY);
            glVertex2f(centerX + cos(step * 1.0f) * radiusX, centerY + sin(step * 1.0f) * radiusY);
            glVertex2f(centerX + cos(step * 2.0f) * radiusX, centerY + sin(step * 2.0f) * radiusY);
            glVertex2f(centerX + cos(step * 3.0f) * radiusX, centerY + sin(step * 3.0f) * radiusY);
            glVertex2f(centerX + cos(step * 4.0f) * radiusX, centerY + sin(step * 4.0f) * radiusY);
            glVertex2f(centerX + cos(step * 5.0f) * radiusX, centerY + sin(step * 5.0f) * radiusY);
            glVertex2f(centerX + cos(step * 6.0f) * radiusX, centerY + sin(step * 6.0f) * radiusY);
            glVertex2f(centerX + cos(step * 7.0f) * radiusX, centerY + sin(step * 7.0f) * radiusY);
            glVertex2f(centerX + cos(step * 8.0f) * radiusX, centerY + sin(step * 8.0f) * radiusY);
            glVertex2f(centerX + cos(step * 9.0f) * radiusX, centerY + sin(step * 9.0f) * radiusY);
            glVertex2f(centerX + cos(step * 10.0f) * radiusX, centerY + sin(step * 10.0f) * radiusY);
            glVertex2f(centerX + cos(step * 11.0f) * radiusX, centerY + sin(step * 11.0f) * radiusY);
            glVertex2f(centerX + cos(step * 12.0f) * radiusX, centerY + sin(step * 12.0f) * radiusY);
            glVertex2f(centerX + cos(step * 13.0f) * radiusX, centerY + sin(step * 13.0f) * radiusY);
            glVertex2f(centerX + cos(step * 14.0f) * radiusX, centerY + sin(step * 14.0f) * radiusY);
            glVertex2f(centerX + cos(step * 15.0f) * radiusX, centerY + sin(step * 15.0f) * radiusY);
            glVertex2f(centerX + cos(step * 16.0f) * radiusX, centerY + sin(step * 16.0f) * radiusY);
            glVertex2f(centerX + cos(step * 17.0f) * radiusX, centerY + sin(step * 17.0f) * radiusY);
            glVertex2f(centerX + cos(step * 18.0f) * radiusX, centerY + sin(step * 18.0f) * radiusY);
            glVertex2f(centerX + cos(step * 19.0f) * radiusX, centerY + sin(step * 19.0f) * radiusY);
            glVertex2f(centerX + cos(step * 20.0f) * radiusX, centerY + sin(step * 20.0f) * radiusY);
            glVertex2f(centerX + cos(step * 21.0f) * radiusX, centerY + sin(step * 21.0f) * radiusY);
            glVertex2f(centerX + cos(step * 22.0f) * radiusX, centerY + sin(step * 22.0f) * radiusY);
            glVertex2f(centerX + cos(step * 23.0f) * radiusX, centerY + sin(step * 23.0f) * radiusY);
            glVertex2f(centerX + cos(step * 24.0f) * radiusX, centerY + sin(step * 24.0f) * radiusY);
            glVertex2f(centerX + cos(step * 25.0f) * radiusX, centerY + sin(step * 25.0f) * radiusY);
            glVertex2f(centerX + cos(step * 26.0f) * radiusX, centerY + sin(step * 26.0f) * radiusY);
            glVertex2f(centerX + cos(step * 27.0f) * radiusX, centerY + sin(step * 27.0f) * radiusY);
            glVertex2f(centerX + cos(step * 28.0f) * radiusX, centerY + sin(step * 28.0f) * radiusY);
            glVertex2f(centerX + cos(step * 29.0f) * radiusX, centerY + sin(step * 29.0f) * radiusY);
            glEnd();
        }
        glColor3f(0.74f, 0.74f, 0.78f);
        {
            float centerX = 0.0f;
            float centerY = 0.0f;
            float radiusX = 7.0f;
            float radiusY = 7.0f;
            float step = 2.0f * PI_VALUE / 28.0f;
            glBegin(GL_POLYGON);
            glVertex2f(centerX + cos(step * 0.0f) * radiusX, centerY + sin(step * 0.0f) * radiusY);
            glVertex2f(centerX + cos(step * 1.0f) * radiusX, centerY + sin(step * 1.0f) * radiusY);
            glVertex2f(centerX + cos(step * 2.0f) * radiusX, centerY + sin(step * 2.0f) * radiusY);
            glVertex2f(centerX + cos(step * 3.0f) * radiusX, centerY + sin(step * 3.0f) * radiusY);
            glVertex2f(centerX + cos(step * 4.0f) * radiusX, centerY + sin(step * 4.0f) * radiusY);
            glVertex2f(centerX + cos(step * 5.0f) * radiusX, centerY + sin(step * 5.0f) * radiusY);
            glVertex2f(centerX + cos(step * 6.0f) * radiusX, centerY + sin(step * 6.0f) * radiusY);
            glVertex2f(centerX + cos(step * 7.0f) * radiusX, centerY + sin(step * 7.0f) * radiusY);
            glVertex2f(centerX + cos(step * 8.0f) * radiusX, centerY + sin(step * 8.0f) * radiusY);
            glVertex2f(centerX + cos(step * 9.0f) * radiusX, centerY + sin(step * 9.0f) * radiusY);
            glVertex2f(centerX + cos(step * 10.0f) * radiusX, centerY + sin(step * 10.0f) * radiusY);
            glVertex2f(centerX + cos(step * 11.0f) * radiusX, centerY + sin(step * 11.0f) * radiusY);
            glVertex2f(centerX + cos(step * 12.0f) * radiusX, centerY + sin(step * 12.0f) * radiusY);
            glVertex2f(centerX + cos(step * 13.0f) * radiusX, centerY + sin(step * 13.0f) * radiusY);
            glVertex2f(centerX + cos(step * 14.0f) * radiusX, centerY + sin(step * 14.0f) * radiusY);
            glVertex2f(centerX + cos(step * 15.0f) * radiusX, centerY + sin(step * 15.0f) * radiusY);
            glVertex2f(centerX + cos(step * 16.0f) * radiusX, centerY + sin(step * 16.0f) * radiusY);
            glVertex2f(centerX + cos(step * 17.0f) * radiusX, centerY + sin(step * 17.0f) * radiusY);
            glVertex2f(centerX + cos(step * 18.0f) * radiusX, centerY + sin(step * 18.0f) * radiusY);
            glVertex2f(centerX + cos(step * 19.0f) * radiusX, centerY + sin(step * 19.0f) * radiusY);
            glVertex2f(centerX + cos(step * 20.0f) * radiusX, centerY + sin(step * 20.0f) * radiusY);
            glVertex2f(centerX + cos(step * 21.0f) * radiusX, centerY + sin(step * 21.0f) * radiusY);
            glVertex2f(centerX + cos(step * 22.0f) * radiusX, centerY + sin(step * 22.0f) * radiusY);
            glVertex2f(centerX + cos(step * 23.0f) * radiusX, centerY + sin(step * 23.0f) * radiusY);
            glVertex2f(centerX + cos(step * 24.0f) * radiusX, centerY + sin(step * 24.0f) * radiusY);
            glVertex2f(centerX + cos(step * 25.0f) * radiusX, centerY + sin(step * 25.0f) * radiusY);
            glVertex2f(centerX + cos(step * 26.0f) * radiusX, centerY + sin(step * 26.0f) * radiusY);
            glVertex2f(centerX + cos(step * 27.0f) * radiusX, centerY + sin(step * 27.0f) * radiusY);
            glEnd();
        }
        glColor3f(0.95f, 0.95f, 0.96f);
        glLineWidth(2.0f);
        glBegin(GL_LINES);
        glVertex2f(-8.0f, 0.0f);
        glVertex2f(8.0f, 0.0f);
        glVertex2f(0.0f, -8.0f);
        glVertex2f(0.0f, 8.0f);
        glVertex2f(-5.5f, -5.5f);
        glVertex2f(5.5f, 5.5f);
        glVertex2f(-5.5f, 5.5f);
        glVertex2f(5.5f, -5.5f);
        glEnd();
        glLineWidth(1.0f);
        glPopMatrix();

        glPushMatrix();
        glTranslatef(42.0f, -2.0f, 0.0f);
        glRotatef(wheelAngle, 0.0f, 0.0f, 1.0f);
        glColor3f(0.05f, 0.05f, 0.06f);
        {
            float centerX = 0.0f;
            float centerY = 0.0f;
            float radiusX = 16.0f;
            float radiusY = 16.0f;
            float step = 2.0f * PI_VALUE / 36.0f;
            glBegin(GL_POLYGON);
            glVertex2f(centerX + cos(step * 0.0f) * radiusX, centerY + sin(step * 0.0f) * radiusY);
            glVertex2f(centerX + cos(step * 1.0f) * radiusX, centerY + sin(step * 1.0f) * radiusY);
            glVertex2f(centerX + cos(step * 2.0f) * radiusX, centerY + sin(step * 2.0f) * radiusY);
            glVertex2f(centerX + cos(step * 3.0f) * radiusX, centerY + sin(step * 3.0f) * radiusY);
            glVertex2f(centerX + cos(step * 4.0f) * radiusX, centerY + sin(step * 4.0f) * radiusY);
            glVertex2f(centerX + cos(step * 5.0f) * radiusX, centerY + sin(step * 5.0f) * radiusY);
            glVertex2f(centerX + cos(step * 6.0f) * radiusX, centerY + sin(step * 6.0f) * radiusY);
            glVertex2f(centerX + cos(step * 7.0f) * radiusX, centerY + sin(step * 7.0f) * radiusY);
            glVertex2f(centerX + cos(step * 8.0f) * radiusX, centerY + sin(step * 8.0f) * radiusY);
            glVertex2f(centerX + cos(step * 9.0f) * radiusX, centerY + sin(step * 9.0f) * radiusY);
            glVertex2f(centerX + cos(step * 10.0f) * radiusX, centerY + sin(step * 10.0f) * radiusY);
            glVertex2f(centerX + cos(step * 11.0f) * radiusX, centerY + sin(step * 11.0f) * radiusY);
            glVertex2f(centerX + cos(step * 12.0f) * radiusX, centerY + sin(step * 12.0f) * radiusY);
            glVertex2f(centerX + cos(step * 13.0f) * radiusX, centerY + sin(step * 13.0f) * radiusY);
            glVertex2f(centerX + cos(step * 14.0f) * radiusX, centerY + sin(step * 14.0f) * radiusY);
            glVertex2f(centerX + cos(step * 15.0f) * radiusX, centerY + sin(step * 15.0f) * radiusY);
            glVertex2f(centerX + cos(step * 16.0f) * radiusX, centerY + sin(step * 16.0f) * radiusY);
            glVertex2f(centerX + cos(step * 17.0f) * radiusX, centerY + sin(step * 17.0f) * radiusY);
            glVertex2f(centerX + cos(step * 18.0f) * radiusX, centerY + sin(step * 18.0f) * radiusY);
            glVertex2f(centerX + cos(step * 19.0f) * radiusX, centerY + sin(step * 19.0f) * radiusY);
            glVertex2f(centerX + cos(step * 20.0f) * radiusX, centerY + sin(step * 20.0f) * radiusY);
            glVertex2f(centerX + cos(step * 21.0f) * radiusX, centerY + sin(step * 21.0f) * radiusY);
            glVertex2f(centerX + cos(step * 22.0f) * radiusX, centerY + sin(step * 22.0f) * radiusY);
            glVertex2f(centerX + cos(step * 23.0f) * radiusX, centerY + sin(step * 23.0f) * radiusY);
            glVertex2f(centerX + cos(step * 24.0f) * radiusX, centerY + sin(step * 24.0f) * radiusY);
            glVertex2f(centerX + cos(step * 25.0f) * radiusX, centerY + sin(step * 25.0f) * radiusY);
            glVertex2f(centerX + cos(step * 26.0f) * radiusX, centerY + sin(step * 26.0f) * radiusY);
            glVertex2f(centerX + cos(step * 27.0f) * radiusX, centerY + sin(step * 27.0f) * radiusY);
            glVertex2f(centerX + cos(step * 28.0f) * radiusX, centerY + sin(step * 28.0f) * radiusY);
            glVertex2f(centerX + cos(step * 29.0f) * radiusX, centerY + sin(step * 29.0f) * radiusY);
            glVertex2f(centerX + cos(step * 30.0f) * radiusX, centerY + sin(step * 30.0f) * radiusY);
            glVertex2f(centerX + cos(step * 31.0f) * radiusX, centerY + sin(step * 31.0f) * radiusY);
            glVertex2f(centerX + cos(step * 32.0f) * radiusX, centerY + sin(step * 32.0f) * radiusY);
            glVertex2f(centerX + cos(step * 33.0f) * radiusX, centerY + sin(step * 33.0f) * radiusY);
            glVertex2f(centerX + cos(step * 34.0f) * radiusX, centerY + sin(step * 34.0f) * radiusY);
            glVertex2f(centerX + cos(step * 35.0f) * radiusX, centerY + sin(step * 35.0f) * radiusY);
            glEnd();
        }
        glColor3f(0.15f, 0.15f, 0.17f);
        {
            float centerX = 0.0f;
            float centerY = 0.0f;
            float radiusX = 12.0f;
            float radiusY = 12.0f;
            float step = 2.0f * PI_VALUE / 30.0f;
            glBegin(GL_POLYGON);
            glVertex2f(centerX + cos(step * 0.0f) * radiusX, centerY + sin(step * 0.0f) * radiusY);
            glVertex2f(centerX + cos(step * 1.0f) * radiusX, centerY + sin(step * 1.0f) * radiusY);
            glVertex2f(centerX + cos(step * 2.0f) * radiusX, centerY + sin(step * 2.0f) * radiusY);
            glVertex2f(centerX + cos(step * 3.0f) * radiusX, centerY + sin(step * 3.0f) * radiusY);
            glVertex2f(centerX + cos(step * 4.0f) * radiusX, centerY + sin(step * 4.0f) * radiusY);
            glVertex2f(centerX + cos(step * 5.0f) * radiusX, centerY + sin(step * 5.0f) * radiusY);
            glVertex2f(centerX + cos(step * 6.0f) * radiusX, centerY + sin(step * 6.0f) * radiusY);
            glVertex2f(centerX + cos(step * 7.0f) * radiusX, centerY + sin(step * 7.0f) * radiusY);
            glVertex2f(centerX + cos(step * 8.0f) * radiusX, centerY + sin(step * 8.0f) * radiusY);
            glVertex2f(centerX + cos(step * 9.0f) * radiusX, centerY + sin(step * 9.0f) * radiusY);
            glVertex2f(centerX + cos(step * 10.0f) * radiusX, centerY + sin(step * 10.0f) * radiusY);
            glVertex2f(centerX + cos(step * 11.0f) * radiusX, centerY + sin(step * 11.0f) * radiusY);
            glVertex2f(centerX + cos(step * 12.0f) * radiusX, centerY + sin(step * 12.0f) * radiusY);
            glVertex2f(centerX + cos(step * 13.0f) * radiusX, centerY + sin(step * 13.0f) * radiusY);
            glVertex2f(centerX + cos(step * 14.0f) * radiusX, centerY + sin(step * 14.0f) * radiusY);
            glVertex2f(centerX + cos(step * 15.0f) * radiusX, centerY + sin(step * 15.0f) * radiusY);
            glVertex2f(centerX + cos(step * 16.0f) * radiusX, centerY + sin(step * 16.0f) * radiusY);
            glVertex2f(centerX + cos(step * 17.0f) * radiusX, centerY + sin(step * 17.0f) * radiusY);
            glVertex2f(centerX + cos(step * 18.0f) * radiusX, centerY + sin(step * 18.0f) * radiusY);
            glVertex2f(centerX + cos(step * 19.0f) * radiusX, centerY + sin(step * 19.0f) * radiusY);
            glVertex2f(centerX + cos(step * 20.0f) * radiusX, centerY + sin(step * 20.0f) * radiusY);
            glVertex2f(centerX + cos(step * 21.0f) * radiusX, centerY + sin(step * 21.0f) * radiusY);
            glVertex2f(centerX + cos(step * 22.0f) * radiusX, centerY + sin(step * 22.0f) * radiusY);
            glVertex2f(centerX + cos(step * 23.0f) * radiusX, centerY + sin(step * 23.0f) * radiusY);
            glVertex2f(centerX + cos(step * 24.0f) * radiusX, centerY + sin(step * 24.0f) * radiusY);
            glVertex2f(centerX + cos(step * 25.0f) * radiusX, centerY + sin(step * 25.0f) * radiusY);
            glVertex2f(centerX + cos(step * 26.0f) * radiusX, centerY + sin(step * 26.0f) * radiusY);
            glVertex2f(centerX + cos(step * 27.0f) * radiusX, centerY + sin(step * 27.0f) * radiusY);
            glVertex2f(centerX + cos(step * 28.0f) * radiusX, centerY + sin(step * 28.0f) * radiusY);
            glVertex2f(centerX + cos(step * 29.0f) * radiusX, centerY + sin(step * 29.0f) * radiusY);
            glEnd();
        }
        glColor3f(0.74f, 0.74f, 0.78f);
        {
            float centerX = 0.0f;
            float centerY = 0.0f;
            float radiusX = 7.0f;
            float radiusY = 7.0f;
            float step = 2.0f * PI_VALUE / 28.0f;
            glBegin(GL_POLYGON);
            glVertex2f(centerX + cos(step * 0.0f) * radiusX, centerY + sin(step * 0.0f) * radiusY);
            glVertex2f(centerX + cos(step * 1.0f) * radiusX, centerY + sin(step * 1.0f) * radiusY);
            glVertex2f(centerX + cos(step * 2.0f) * radiusX, centerY + sin(step * 2.0f) * radiusY);
            glVertex2f(centerX + cos(step * 3.0f) * radiusX, centerY + sin(step * 3.0f) * radiusY);
            glVertex2f(centerX + cos(step * 4.0f) * radiusX, centerY + sin(step * 4.0f) * radiusY);
            glVertex2f(centerX + cos(step * 5.0f) * radiusX, centerY + sin(step * 5.0f) * radiusY);
            glVertex2f(centerX + cos(step * 6.0f) * radiusX, centerY + sin(step * 6.0f) * radiusY);
            glVertex2f(centerX + cos(step * 7.0f) * radiusX, centerY + sin(step * 7.0f) * radiusY);
            glVertex2f(centerX + cos(step * 8.0f) * radiusX, centerY + sin(step * 8.0f) * radiusY);
            glVertex2f(centerX + cos(step * 9.0f) * radiusX, centerY + sin(step * 9.0f) * radiusY);
            glVertex2f(centerX + cos(step * 10.0f) * radiusX, centerY + sin(step * 10.0f) * radiusY);
            glVertex2f(centerX + cos(step * 11.0f) * radiusX, centerY + sin(step * 11.0f) * radiusY);
            glVertex2f(centerX + cos(step * 12.0f) * radiusX, centerY + sin(step * 12.0f) * radiusY);
            glVertex2f(centerX + cos(step * 13.0f) * radiusX, centerY + sin(step * 13.0f) * radiusY);
            glVertex2f(centerX + cos(step * 14.0f) * radiusX, centerY + sin(step * 14.0f) * radiusY);
            glVertex2f(centerX + cos(step * 15.0f) * radiusX, centerY + sin(step * 15.0f) * radiusY);
            glVertex2f(centerX + cos(step * 16.0f) * radiusX, centerY + sin(step * 16.0f) * radiusY);
            glVertex2f(centerX + cos(step * 17.0f) * radiusX, centerY + sin(step * 17.0f) * radiusY);
            glVertex2f(centerX + cos(step * 18.0f) * radiusX, centerY + sin(step * 18.0f) * radiusY);
            glVertex2f(centerX + cos(step * 19.0f) * radiusX, centerY + sin(step * 19.0f) * radiusY);
            glVertex2f(centerX + cos(step * 20.0f) * radiusX, centerY + sin(step * 20.0f) * radiusY);
            glVertex2f(centerX + cos(step * 21.0f) * radiusX, centerY + sin(step * 21.0f) * radiusY);
            glVertex2f(centerX + cos(step * 22.0f) * radiusX, centerY + sin(step * 22.0f) * radiusY);
            glVertex2f(centerX + cos(step * 23.0f) * radiusX, centerY + sin(step * 23.0f) * radiusY);
            glVertex2f(centerX + cos(step * 24.0f) * radiusX, centerY + sin(step * 24.0f) * radiusY);
            glVertex2f(centerX + cos(step * 25.0f) * radiusX, centerY + sin(step * 25.0f) * radiusY);
            glVertex2f(centerX + cos(step * 26.0f) * radiusX, centerY + sin(step * 26.0f) * radiusY);
            glVertex2f(centerX + cos(step * 27.0f) * radiusX, centerY + sin(step * 27.0f) * radiusY);
            glEnd();
        }
        glColor3f(0.95f, 0.95f, 0.96f);
        glLineWidth(2.0f);
        glBegin(GL_LINES);
        glVertex2f(-8.0f, 0.0f);
        glVertex2f(8.0f, 0.0f);
        glVertex2f(0.0f, -8.0f);
        glVertex2f(0.0f, 8.0f);
        glVertex2f(-5.5f, -5.5f);
        glVertex2f(5.5f, 5.5f);
        glVertex2f(-5.5f, 5.5f);
        glVertex2f(5.5f, -5.5f);
        glEnd();
        glLineWidth(1.0f);
        glPopMatrix();

        if (headlightOn) {
            glColor4f(1.0f, 0.96f, 0.42f, 0.30f);
            glBegin(GL_TRIANGLES);
            glVertex2f(70.0f, 20.0f);
            glVertex2f(186.0f, 58.0f);
            glVertex2f(186.0f, -10.0f);
            glEnd();
            glColor3f(1.0f, 1.0f, 0.72f);
            {
                float left = 66.0f;
                float bottom = 14.0f;
                float right = left + 8.0f;
                float top = bottom + 8.0f;
                glBegin(GL_QUADS);
                glVertex2f(left, bottom);
                glVertex2f(right, bottom);
                glVertex2f(right, top);
                glVertex2f(left, top);
                glEnd();
            }
        }
        if (tailLightOn) {
            glColor3f(0.95f, 0.08f, 0.08f);
            {
                float left = -74.0f;
                float bottom = 16.0f;
                float right = left + 8.0f;
                float top = bottom + 8.0f;
                glBegin(GL_QUADS);
                glVertex2f(left, bottom);
                glVertex2f(right, bottom);
                glVertex2f(right, top);
                glVertex2f(left, top);
                glEnd();
            }
            {
                float left = -74.0f;
                float bottom = 6.0f;
                float right = left + 8.0f;
                float top = bottom + 8.0f;
                glBegin(GL_QUADS);
                glVertex2f(left, bottom);
                glVertex2f(right, bottom);
                glVertex2f(right, top);
                glVertex2f(left, top);
                glEnd();
            }
        }
        glDisable(GL_BLEND);
        glPopMatrix();
    }
}

void scene2() {
    if (!isRainEnabled) {
        float skyBottomR = 0.44f;
        float skyBottomG = 0.74f;
        float skyBottomB = 0.95f;
        float skyTopR = 0.68f;
        float skyTopG = 0.89f;
        float skyTopB = 0.99f;
        glBegin(GL_QUADS);
        glColor3f(skyBottomR, skyBottomG, skyBottomB);
        glVertex2f(0.0f, 0.0f);
        glColor3f(skyBottomR, skyBottomG, skyBottomB);
        glVertex2f(1280.0f, 0.0f);
        glColor3f(skyTopR, skyTopG, skyTopB);
        glVertex2f(1280.0f, 720.0f);
        glColor3f(skyTopR, skyTopG, skyTopB);
        glVertex2f(0.0f, 720.0f);
        glEnd();

        float sunCenterX = 980.0f + sunHorizontalOffset * 0.35f;
        float sunCenterY = 610.0f;
        glColor3f(1.0f, 0.92f, 0.32f);
        {
            float centerX = sunCenterX;
            float centerY = sunCenterY;
            float radiusX = 36.0f;
            float radiusY = 36.0f;
            float step = 2.0f * PI_VALUE / 36.0f;
            glBegin(GL_POLYGON);
            glVertex2f(centerX + cos(step * 0.0f) * radiusX, centerY + sin(step * 0.0f) * radiusY);
            glVertex2f(centerX + cos(step * 1.0f) * radiusX, centerY + sin(step * 1.0f) * radiusY);
            glVertex2f(centerX + cos(step * 2.0f) * radiusX, centerY + sin(step * 2.0f) * radiusY);
            glVertex2f(centerX + cos(step * 3.0f) * radiusX, centerY + sin(step * 3.0f) * radiusY);
            glVertex2f(centerX + cos(step * 4.0f) * radiusX, centerY + sin(step * 4.0f) * radiusY);
            glVertex2f(centerX + cos(step * 5.0f) * radiusX, centerY + sin(step * 5.0f) * radiusY);
            glVertex2f(centerX + cos(step * 6.0f) * radiusX, centerY + sin(step * 6.0f) * radiusY);
            glVertex2f(centerX + cos(step * 7.0f) * radiusX, centerY + sin(step * 7.0f) * radiusY);
            glVertex2f(centerX + cos(step * 8.0f) * radiusX, centerY + sin(step * 8.0f) * radiusY);
            glVertex2f(centerX + cos(step * 9.0f) * radiusX, centerY + sin(step * 9.0f) * radiusY);
            glVertex2f(centerX + cos(step * 10.0f) * radiusX, centerY + sin(step * 10.0f) * radiusY);
            glVertex2f(centerX + cos(step * 11.0f) * radiusX, centerY + sin(step * 11.0f) * radiusY);
            glVertex2f(centerX + cos(step * 12.0f) * radiusX, centerY + sin(step * 12.0f) * radiusY);
            glVertex2f(centerX + cos(step * 13.0f) * radiusX, centerY + sin(step * 13.0f) * radiusY);
            glVertex2f(centerX + cos(step * 14.0f) * radiusX, centerY + sin(step * 14.0f) * radiusY);
            glVertex2f(centerX + cos(step * 15.0f) * radiusX, centerY + sin(step * 15.0f) * radiusY);
            glVertex2f(centerX + cos(step * 16.0f) * radiusX, centerY + sin(step * 16.0f) * radiusY);
            glVertex2f(centerX + cos(step * 17.0f) * radiusX, centerY + sin(step * 17.0f) * radiusY);
            glVertex2f(centerX + cos(step * 18.0f) * radiusX, centerY + sin(step * 18.0f) * radiusY);
            glVertex2f(centerX + cos(step * 19.0f) * radiusX, centerY + sin(step * 19.0f) * radiusY);
            glVertex2f(centerX + cos(step * 20.0f) * radiusX, centerY + sin(step * 20.0f) * radiusY);
            glVertex2f(centerX + cos(step * 21.0f) * radiusX, centerY + sin(step * 21.0f) * radiusY);
            glVertex2f(centerX + cos(step * 22.0f) * radiusX, centerY + sin(step * 22.0f) * radiusY);
            glVertex2f(centerX + cos(step * 23.0f) * radiusX, centerY + sin(step * 23.0f) * radiusY);
            glVertex2f(centerX + cos(step * 24.0f) * radiusX, centerY + sin(step * 24.0f) * radiusY);
            glVertex2f(centerX + cos(step * 25.0f) * radiusX, centerY + sin(step * 25.0f) * radiusY);
            glVertex2f(centerX + cos(step * 26.0f) * radiusX, centerY + sin(step * 26.0f) * radiusY);
            glVertex2f(centerX + cos(step * 27.0f) * radiusX, centerY + sin(step * 27.0f) * radiusY);
            glVertex2f(centerX + cos(step * 28.0f) * radiusX, centerY + sin(step * 28.0f) * radiusY);
            glVertex2f(centerX + cos(step * 29.0f) * radiusX, centerY + sin(step * 29.0f) * radiusY);
            glVertex2f(centerX + cos(step * 30.0f) * radiusX, centerY + sin(step * 30.0f) * radiusY);
            glVertex2f(centerX + cos(step * 31.0f) * radiusX, centerY + sin(step * 31.0f) * radiusY);
            glVertex2f(centerX + cos(step * 32.0f) * radiusX, centerY + sin(step * 32.0f) * radiusY);
            glVertex2f(centerX + cos(step * 33.0f) * radiusX, centerY + sin(step * 33.0f) * radiusY);
            glVertex2f(centerX + cos(step * 34.0f) * radiusX, centerY + sin(step * 34.0f) * radiusY);
            glVertex2f(centerX + cos(step * 35.0f) * radiusX, centerY + sin(step * 35.0f) * radiusY);
            glEnd();
        }
        glColor3f(1.0f, 0.84f, 0.22f);
        glLineWidth(2.0f);
        glBegin(GL_LINES);
        glVertex2f(sunCenterX - 54.0f, sunCenterY);
        glVertex2f(sunCenterX - 38.0f, sunCenterY);
        glVertex2f(sunCenterX + 38.0f, sunCenterY);
        glVertex2f(sunCenterX + 54.0f, sunCenterY);
        glVertex2f(sunCenterX, sunCenterY - 54.0f);
        glVertex2f(sunCenterX, sunCenterY - 38.0f);
        glVertex2f(sunCenterX, sunCenterY + 38.0f);
        glVertex2f(sunCenterX, sunCenterY + 54.0f);
        glEnd();
        glLineWidth(1.0f);

        float cloudA_CenterX = 220.0f + cloudOffsetX_layerA;
        float cloudA_CenterY = 610.0f;
        glColor3f(0.98f, 0.98f, 0.99f);
        {
            float centerX = cloudA_CenterX;
            float centerY = cloudA_CenterY;
            float radiusX = 28.0f;
            float radiusY = 28.0f;
            float step = 2.0f * PI_VALUE / 28.0f;
            glBegin(GL_POLYGON);
            glVertex2f(centerX + cos(step * 0.0f) * radiusX, centerY + sin(step * 0.0f) * radiusY);
            glVertex2f(centerX + cos(step * 1.0f) * radiusX, centerY + sin(step * 1.0f) * radiusY);
            glVertex2f(centerX + cos(step * 2.0f) * radiusX, centerY + sin(step * 2.0f) * radiusY);
            glVertex2f(centerX + cos(step * 3.0f) * radiusX, centerY + sin(step * 3.0f) * radiusY);
            glVertex2f(centerX + cos(step * 4.0f) * radiusX, centerY + sin(step * 4.0f) * radiusY);
            glVertex2f(centerX + cos(step * 5.0f) * radiusX, centerY + sin(step * 5.0f) * radiusY);
            glVertex2f(centerX + cos(step * 6.0f) * radiusX, centerY + sin(step * 6.0f) * radiusY);
            glVertex2f(centerX + cos(step * 7.0f) * radiusX, centerY + sin(step * 7.0f) * radiusY);
            glVertex2f(centerX + cos(step * 8.0f) * radiusX, centerY + sin(step * 8.0f) * radiusY);
            glVertex2f(centerX + cos(step * 9.0f) * radiusX, centerY + sin(step * 9.0f) * radiusY);
            glVertex2f(centerX + cos(step * 10.0f) * radiusX, centerY + sin(step * 10.0f) * radiusY);
            glVertex2f(centerX + cos(step * 11.0f) * radiusX, centerY + sin(step * 11.0f) * radiusY);
            glVertex2f(centerX + cos(step * 12.0f) * radiusX, centerY + sin(step * 12.0f) * radiusY);
            glVertex2f(centerX + cos(step * 13.0f) * radiusX, centerY + sin(step * 13.0f) * radiusY);
            glVertex2f(centerX + cos(step * 14.0f) * radiusX, centerY + sin(step * 14.0f) * radiusY);
            glVertex2f(centerX + cos(step * 15.0f) * radiusX, centerY + sin(step * 15.0f) * radiusY);
            glVertex2f(centerX + cos(step * 16.0f) * radiusX, centerY + sin(step * 16.0f) * radiusY);
            glVertex2f(centerX + cos(step * 17.0f) * radiusX, centerY + sin(step * 17.0f) * radiusY);
            glVertex2f(centerX + cos(step * 18.0f) * radiusX, centerY + sin(step * 18.0f) * radiusY);
            glVertex2f(centerX + cos(step * 19.0f) * radiusX, centerY + sin(step * 19.0f) * radiusY);
            glVertex2f(centerX + cos(step * 20.0f) * radiusX, centerY + sin(step * 20.0f) * radiusY);
            glVertex2f(centerX + cos(step * 21.0f) * radiusX, centerY + sin(step * 21.0f) * radiusY);
            glVertex2f(centerX + cos(step * 22.0f) * radiusX, centerY + sin(step * 22.0f) * radiusY);
            glVertex2f(centerX + cos(step * 23.0f) * radiusX, centerY + sin(step * 23.0f) * radiusY);
            glVertex2f(centerX + cos(step * 24.0f) * radiusX, centerY + sin(step * 24.0f) * radiusY);
            glVertex2f(centerX + cos(step * 25.0f) * radiusX, centerY + sin(step * 25.0f) * radiusY);
            glVertex2f(centerX + cos(step * 26.0f) * radiusX, centerY + sin(step * 26.0f) * radiusY);
            glVertex2f(centerX + cos(step * 27.0f) * radiusX, centerY + sin(step * 27.0f) * radiusY);
            glEnd();
        }
        {
            float centerX = cloudA_CenterX - 30.0f;
            float centerY = cloudA_CenterY - 4.0f;
            float radiusX = 22.0f;
            float radiusY = 22.0f;
            float step = 2.0f * PI_VALUE / 24.0f;
            glBegin(GL_POLYGON);
            glVertex2f(centerX + cos(step * 0.0f) * radiusX, centerY + sin(step * 0.0f) * radiusY);
            glVertex2f(centerX + cos(step * 1.0f) * radiusX, centerY + sin(step * 1.0f) * radiusY);
            glVertex2f(centerX + cos(step * 2.0f) * radiusX, centerY + sin(step * 2.0f) * radiusY);
            glVertex2f(centerX + cos(step * 3.0f) * radiusX, centerY + sin(step * 3.0f) * radiusY);
            glVertex2f(centerX + cos(step * 4.0f) * radiusX, centerY + sin(step * 4.0f) * radiusY);
            glVertex2f(centerX + cos(step * 5.0f) * radiusX, centerY + sin(step * 5.0f) * radiusY);
            glVertex2f(centerX + cos(step * 6.0f) * radiusX, centerY + sin(step * 6.0f) * radiusY);
            glVertex2f(centerX + cos(step * 7.0f) * radiusX, centerY + sin(step * 7.0f) * radiusY);
            glVertex2f(centerX + cos(step * 8.0f) * radiusX, centerY + sin(step * 8.0f) * radiusY);
            glVertex2f(centerX + cos(step * 9.0f) * radiusX, centerY + sin(step * 9.0f) * radiusY);
            glVertex2f(centerX + cos(step * 10.0f) * radiusX, centerY + sin(step * 10.0f) * radiusY);
            glVertex2f(centerX + cos(step * 11.0f) * radiusX, centerY + sin(step * 11.0f) * radiusY);
            glVertex2f(centerX + cos(step * 12.0f) * radiusX, centerY + sin(step * 12.0f) * radiusY);
            glVertex2f(centerX + cos(step * 13.0f) * radiusX, centerY + sin(step * 13.0f) * radiusY);
            glVertex2f(centerX + cos(step * 14.0f) * radiusX, centerY + sin(step * 14.0f) * radiusY);
            glVertex2f(centerX + cos(step * 15.0f) * radiusX, centerY + sin(step * 15.0f) * radiusY);
            glVertex2f(centerX + cos(step * 16.0f) * radiusX, centerY + sin(step * 16.0f) * radiusY);
            glVertex2f(centerX + cos(step * 17.0f) * radiusX, centerY + sin(step * 17.0f) * radiusY);
            glVertex2f(centerX + cos(step * 18.0f) * radiusX, centerY + sin(step * 18.0f) * radiusY);
            glVertex2f(centerX + cos(step * 19.0f) * radiusX, centerY + sin(step * 19.0f) * radiusY);
            glVertex2f(centerX + cos(step * 20.0f) * radiusX, centerY + sin(step * 20.0f) * radiusY);
            glVertex2f(centerX + cos(step * 21.0f) * radiusX, centerY + sin(step * 21.0f) * radiusY);
            glVertex2f(centerX + cos(step * 22.0f) * radiusX, centerY + sin(step * 22.0f) * radiusY);
            glVertex2f(centerX + cos(step * 23.0f) * radiusX, centerY + sin(step * 23.0f) * radiusY);
            glEnd();
        }
        {
            float centerX = cloudA_CenterX + 30.0f;
            float centerY = cloudA_CenterY - 2.0f;
            float radiusX = 24.0f;
            float radiusY = 24.0f;
            float step = 2.0f * PI_VALUE / 24.0f;
            glBegin(GL_POLYGON);
            glVertex2f(centerX + cos(step * 0.0f) * radiusX, centerY + sin(step * 0.0f) * radiusY);
            glVertex2f(centerX + cos(step * 1.0f) * radiusX, centerY + sin(step * 1.0f) * radiusY);
            glVertex2f(centerX + cos(step * 2.0f) * radiusX, centerY + sin(step * 2.0f) * radiusY);
            glVertex2f(centerX + cos(step * 3.0f) * radiusX, centerY + sin(step * 3.0f) * radiusY);
            glVertex2f(centerX + cos(step * 4.0f) * radiusX, centerY + sin(step * 4.0f) * radiusY);
            glVertex2f(centerX + cos(step * 5.0f) * radiusX, centerY + sin(step * 5.0f) * radiusY);
            glVertex2f(centerX + cos(step * 6.0f) * radiusX, centerY + sin(step * 6.0f) * radiusY);
            glVertex2f(centerX + cos(step * 7.0f) * radiusX, centerY + sin(step * 7.0f) * radiusY);
            glVertex2f(centerX + cos(step * 8.0f) * radiusX, centerY + sin(step * 8.0f) * radiusY);
            glVertex2f(centerX + cos(step * 9.0f) * radiusX, centerY + sin(step * 9.0f) * radiusY);
            glVertex2f(centerX + cos(step * 10.0f) * radiusX, centerY + sin(step * 10.0f) * radiusY);
            glVertex2f(centerX + cos(step * 11.0f) * radiusX, centerY + sin(step * 11.0f) * radiusY);
            glVertex2f(centerX + cos(step * 12.0f) * radiusX, centerY + sin(step * 12.0f) * radiusY);
            glVertex2f(centerX + cos(step * 13.0f) * radiusX, centerY + sin(step * 13.0f) * radiusY);
            glVertex2f(centerX + cos(step * 14.0f) * radiusX, centerY + sin(step * 14.0f) * radiusY);
            glVertex2f(centerX + cos(step * 15.0f) * radiusX, centerY + sin(step * 15.0f) * radiusY);
            glVertex2f(centerX + cos(step * 16.0f) * radiusX, centerY + sin(step * 16.0f) * radiusY);
            glVertex2f(centerX + cos(step * 17.0f) * radiusX, centerY + sin(step * 17.0f) * radiusY);
            glVertex2f(centerX + cos(step * 18.0f) * radiusX, centerY + sin(step * 18.0f) * radiusY);
            glVertex2f(centerX + cos(step * 19.0f) * radiusX, centerY + sin(step * 19.0f) * radiusY);
            glVertex2f(centerX + cos(step * 20.0f) * radiusX, centerY + sin(step * 20.0f) * radiusY);
            glVertex2f(centerX + cos(step * 21.0f) * radiusX, centerY + sin(step * 21.0f) * radiusY);
            glVertex2f(centerX + cos(step * 22.0f) * radiusX, centerY + sin(step * 22.0f) * radiusY);
            glVertex2f(centerX + cos(step * 23.0f) * radiusX, centerY + sin(step * 23.0f) * radiusY);
            glEnd();
        }
        {
            float centerX = cloudA_CenterX + 2.0f;
            float centerY = cloudA_CenterY + 18.0f;
            float radiusX = 20.0f;
            float radiusY = 20.0f;
            float step = 2.0f * PI_VALUE / 24.0f;
            glBegin(GL_POLYGON);
            glVertex2f(centerX + cos(step * 0.0f) * radiusX, centerY + sin(step * 0.0f) * radiusY);
            glVertex2f(centerX + cos(step * 1.0f) * radiusX, centerY + sin(step * 1.0f) * radiusY);
            glVertex2f(centerX + cos(step * 2.0f) * radiusX, centerY + sin(step * 2.0f) * radiusY);
            glVertex2f(centerX + cos(step * 3.0f) * radiusX, centerY + sin(step * 3.0f) * radiusY);
            glVertex2f(centerX + cos(step * 4.0f) * radiusX, centerY + sin(step * 4.0f) * radiusY);
            glVertex2f(centerX + cos(step * 5.0f) * radiusX, centerY + sin(step * 5.0f) * radiusY);
            glVertex2f(centerX + cos(step * 6.0f) * radiusX, centerY + sin(step * 6.0f) * radiusY);
            glVertex2f(centerX + cos(step * 7.0f) * radiusX, centerY + sin(step * 7.0f) * radiusY);
            glVertex2f(centerX + cos(step * 8.0f) * radiusX, centerY + sin(step * 8.0f) * radiusY);
            glVertex2f(centerX + cos(step * 9.0f) * radiusX, centerY + sin(step * 9.0f) * radiusY);
            glVertex2f(centerX + cos(step * 10.0f) * radiusX, centerY + sin(step * 10.0f) * radiusY);
            glVertex2f(centerX + cos(step * 11.0f) * radiusX, centerY + sin(step * 11.0f) * radiusY);
            glVertex2f(centerX + cos(step * 12.0f) * radiusX, centerY + sin(step * 12.0f) * radiusY);
            glVertex2f(centerX + cos(step * 13.0f) * radiusX, centerY + sin(step * 13.0f) * radiusY);
            glVertex2f(centerX + cos(step * 14.0f) * radiusX, centerY + sin(step * 14.0f) * radiusY);
            glVertex2f(centerX + cos(step * 15.0f) * radiusX, centerY + sin(step * 15.0f) * radiusY);
            glVertex2f(centerX + cos(step * 16.0f) * radiusX, centerY + sin(step * 16.0f) * radiusY);
            glVertex2f(centerX + cos(step * 17.0f) * radiusX, centerY + sin(step * 17.0f) * radiusY);
            glVertex2f(centerX + cos(step * 18.0f) * radiusX, centerY + sin(step * 18.0f) * radiusY);
            glVertex2f(centerX + cos(step * 19.0f) * radiusX, centerY + sin(step * 19.0f) * radiusY);
            glVertex2f(centerX + cos(step * 20.0f) * radiusX, centerY + sin(step * 20.0f) * radiusY);
            glVertex2f(centerX + cos(step * 21.0f) * radiusX, centerY + sin(step * 21.0f) * radiusY);
            glVertex2f(centerX + cos(step * 22.0f) * radiusX, centerY + sin(step * 22.0f) * radiusY);
            glVertex2f(centerX + cos(step * 23.0f) * radiusX, centerY + sin(step * 23.0f) * radiusY);
            glEnd();
        }

        float cloudB_CenterX = 560.0f + cloudOffsetX_layerB;
        float cloudB_CenterY = 640.0f;
        glColor3f(0.98f, 0.98f, 0.99f);
        {
            float centerX = cloudB_CenterX;
            float centerY = cloudB_CenterY;
            float radiusX = 28.0f;
            float radiusY = 28.0f;
            float step = 2.0f * PI_VALUE / 28.0f;
            glBegin(GL_POLYGON);
            glVertex2f(centerX + cos(step * 0.0f) * radiusX, centerY + sin(step * 0.0f) * radiusY);
            glVertex2f(centerX + cos(step * 1.0f) * radiusX, centerY + sin(step * 1.0f) * radiusY);
            glVertex2f(centerX + cos(step * 2.0f) * radiusX, centerY + sin(step * 2.0f) * radiusY);
            glVertex2f(centerX + cos(step * 3.0f) * radiusX, centerY + sin(step * 3.0f) * radiusY);
            glVertex2f(centerX + cos(step * 4.0f) * radiusX, centerY + sin(step * 4.0f) * radiusY);
            glVertex2f(centerX + cos(step * 5.0f) * radiusX, centerY + sin(step * 5.0f) * radiusY);
            glVertex2f(centerX + cos(step * 6.0f) * radiusX, centerY + sin(step * 6.0f) * radiusY);
            glVertex2f(centerX + cos(step * 7.0f) * radiusX, centerY + sin(step * 7.0f) * radiusY);
            glVertex2f(centerX + cos(step * 8.0f) * radiusX, centerY + sin(step * 8.0f) * radiusY);
            glVertex2f(centerX + cos(step * 9.0f) * radiusX, centerY + sin(step * 9.0f) * radiusY);
            glVertex2f(centerX + cos(step * 10.0f) * radiusX, centerY + sin(step * 10.0f) * radiusY);
            glVertex2f(centerX + cos(step * 11.0f) * radiusX, centerY + sin(step * 11.0f) * radiusY);
            glVertex2f(centerX + cos(step * 12.0f) * radiusX, centerY + sin(step * 12.0f) * radiusY);
            glVertex2f(centerX + cos(step * 13.0f) * radiusX, centerY + sin(step * 13.0f) * radiusY);
            glVertex2f(centerX + cos(step * 14.0f) * radiusX, centerY + sin(step * 14.0f) * radiusY);
            glVertex2f(centerX + cos(step * 15.0f) * radiusX, centerY + sin(step * 15.0f) * radiusY);
            glVertex2f(centerX + cos(step * 16.0f) * radiusX, centerY + sin(step * 16.0f) * radiusY);
            glVertex2f(centerX + cos(step * 17.0f) * radiusX, centerY + sin(step * 17.0f) * radiusY);
            glVertex2f(centerX + cos(step * 18.0f) * radiusX, centerY + sin(step * 18.0f) * radiusY);
            glVertex2f(centerX + cos(step * 19.0f) * radiusX, centerY + sin(step * 19.0f) * radiusY);
            glVertex2f(centerX + cos(step * 20.0f) * radiusX, centerY + sin(step * 20.0f) * radiusY);
            glVertex2f(centerX + cos(step * 21.0f) * radiusX, centerY + sin(step * 21.0f) * radiusY);
            glVertex2f(centerX + cos(step * 22.0f) * radiusX, centerY + sin(step * 22.0f) * radiusY);
            glVertex2f(centerX + cos(step * 23.0f) * radiusX, centerY + sin(step * 23.0f) * radiusY);
            glVertex2f(centerX + cos(step * 24.0f) * radiusX, centerY + sin(step * 24.0f) * radiusY);
            glVertex2f(centerX + cos(step * 25.0f) * radiusX, centerY + sin(step * 25.0f) * radiusY);
            glVertex2f(centerX + cos(step * 26.0f) * radiusX, centerY + sin(step * 26.0f) * radiusY);
            glVertex2f(centerX + cos(step * 27.0f) * radiusX, centerY + sin(step * 27.0f) * radiusY);
            glEnd();
        }
        {
            float centerX = cloudB_CenterX - 30.0f;
            float centerY = cloudB_CenterY - 4.0f;
            float radiusX = 22.0f;
            float radiusY = 22.0f;
            float step = 2.0f * PI_VALUE / 24.0f;
            glBegin(GL_POLYGON);
            glVertex2f(centerX + cos(step * 0.0f) * radiusX, centerY + sin(step * 0.0f) * radiusY);
            glVertex2f(centerX + cos(step * 1.0f) * radiusX, centerY + sin(step * 1.0f) * radiusY);
            glVertex2f(centerX + cos(step * 2.0f) * radiusX, centerY + sin(step * 2.0f) * radiusY);
            glVertex2f(centerX + cos(step * 3.0f) * radiusX, centerY + sin(step * 3.0f) * radiusY);
            glVertex2f(centerX + cos(step * 4.0f) * radiusX, centerY + sin(step * 4.0f) * radiusY);
            glVertex2f(centerX + cos(step * 5.0f) * radiusX, centerY + sin(step * 5.0f) * radiusY);
            glVertex2f(centerX + cos(step * 6.0f) * radiusX, centerY + sin(step * 6.0f) * radiusY);
            glVertex2f(centerX + cos(step * 7.0f) * radiusX, centerY + sin(step * 7.0f) * radiusY);
            glVertex2f(centerX + cos(step * 8.0f) * radiusX, centerY + sin(step * 8.0f) * radiusY);
            glVertex2f(centerX + cos(step * 9.0f) * radiusX, centerY + sin(step * 9.0f) * radiusY);
            glVertex2f(centerX + cos(step * 10.0f) * radiusX, centerY + sin(step * 10.0f) * radiusY);
            glVertex2f(centerX + cos(step * 11.0f) * radiusX, centerY + sin(step * 11.0f) * radiusY);
            glVertex2f(centerX + cos(step * 12.0f) * radiusX, centerY + sin(step * 12.0f) * radiusY);
            glVertex2f(centerX + cos(step * 13.0f) * radiusX, centerY + sin(step * 13.0f) * radiusY);
            glVertex2f(centerX + cos(step * 14.0f) * radiusX, centerY + sin(step * 14.0f) * radiusY);
            glVertex2f(centerX + cos(step * 15.0f) * radiusX, centerY + sin(step * 15.0f) * radiusY);
            glVertex2f(centerX + cos(step * 16.0f) * radiusX, centerY + sin(step * 16.0f) * radiusY);
            glVertex2f(centerX + cos(step * 17.0f) * radiusX, centerY + sin(step * 17.0f) * radiusY);
            glVertex2f(centerX + cos(step * 18.0f) * radiusX, centerY + sin(step * 18.0f) * radiusY);
            glVertex2f(centerX + cos(step * 19.0f) * radiusX, centerY + sin(step * 19.0f) * radiusY);
            glVertex2f(centerX + cos(step * 20.0f) * radiusX, centerY + sin(step * 20.0f) * radiusY);
            glVertex2f(centerX + cos(step * 21.0f) * radiusX, centerY + sin(step * 21.0f) * radiusY);
            glVertex2f(centerX + cos(step * 22.0f) * radiusX, centerY + sin(step * 22.0f) * radiusY);
            glVertex2f(centerX + cos(step * 23.0f) * radiusX, centerY + sin(step * 23.0f) * radiusY);
            glEnd();
        }
        {
            float centerX = cloudB_CenterX + 30.0f;
            float centerY = cloudB_CenterY - 2.0f;
            float radiusX = 24.0f;
            float radiusY = 24.0f;
            float step = 2.0f * PI_VALUE / 24.0f;
            glBegin(GL_POLYGON);
            glVertex2f(centerX + cos(step * 0.0f) * radiusX, centerY + sin(step * 0.0f) * radiusY);
            glVertex2f(centerX + cos(step * 1.0f) * radiusX, centerY + sin(step * 1.0f) * radiusY);
            glVertex2f(centerX + cos(step * 2.0f) * radiusX, centerY + sin(step * 2.0f) * radiusY);
            glVertex2f(centerX + cos(step * 3.0f) * radiusX, centerY + sin(step * 3.0f) * radiusY);
            glVertex2f(centerX + cos(step * 4.0f) * radiusX, centerY + sin(step * 4.0f) * radiusY);
            glVertex2f(centerX + cos(step * 5.0f) * radiusX, centerY + sin(step * 5.0f) * radiusY);
            glVertex2f(centerX + cos(step * 6.0f) * radiusX, centerY + sin(step * 6.0f) * radiusY);
            glVertex2f(centerX + cos(step * 7.0f) * radiusX, centerY + sin(step * 7.0f) * radiusY);
            glVertex2f(centerX + cos(step * 8.0f) * radiusX, centerY + sin(step * 8.0f) * radiusY);
            glVertex2f(centerX + cos(step * 9.0f) * radiusX, centerY + sin(step * 9.0f) * radiusY);
            glVertex2f(centerX + cos(step * 10.0f) * radiusX, centerY + sin(step * 10.0f) * radiusY);
            glVertex2f(centerX + cos(step * 11.0f) * radiusX, centerY + sin(step * 11.0f) * radiusY);
            glVertex2f(centerX + cos(step * 12.0f) * radiusX, centerY + sin(step * 12.0f) * radiusY);
            glVertex2f(centerX + cos(step * 13.0f) * radiusX, centerY + sin(step * 13.0f) * radiusY);
            glVertex2f(centerX + cos(step * 14.0f) * radiusX, centerY + sin(step * 14.0f) * radiusY);
            glVertex2f(centerX + cos(step * 15.0f) * radiusX, centerY + sin(step * 15.0f) * radiusY);
            glVertex2f(centerX + cos(step * 16.0f) * radiusX, centerY + sin(step * 16.0f) * radiusY);
            glVertex2f(centerX + cos(step * 17.0f) * radiusX, centerY + sin(step * 17.0f) * radiusY);
            glVertex2f(centerX + cos(step * 18.0f) * radiusX, centerY + sin(step * 18.0f) * radiusY);
            glVertex2f(centerX + cos(step * 19.0f) * radiusX, centerY + sin(step * 19.0f) * radiusY);
            glVertex2f(centerX + cos(step * 20.0f) * radiusX, centerY + sin(step * 20.0f) * radiusY);
            glVertex2f(centerX + cos(step * 21.0f) * radiusX, centerY + sin(step * 21.0f) * radiusY);
            glVertex2f(centerX + cos(step * 22.0f) * radiusX, centerY + sin(step * 22.0f) * radiusY);
            glVertex2f(centerX + cos(step * 23.0f) * radiusX, centerY + sin(step * 23.0f) * radiusY);
            glEnd();
        }
        {
            float centerX = cloudB_CenterX + 2.0f;
            float centerY = cloudB_CenterY + 18.0f;
            float radiusX = 20.0f;
            float radiusY = 20.0f;
            float step = 2.0f * PI_VALUE / 24.0f;
            glBegin(GL_POLYGON);
            glVertex2f(centerX + cos(step * 0.0f) * radiusX, centerY + sin(step * 0.0f) * radiusY);
            glVertex2f(centerX + cos(step * 1.0f) * radiusX, centerY + sin(step * 1.0f) * radiusY);
            glVertex2f(centerX + cos(step * 2.0f) * radiusX, centerY + sin(step * 2.0f) * radiusY);
            glVertex2f(centerX + cos(step * 3.0f) * radiusX, centerY + sin(step * 3.0f) * radiusY);
            glVertex2f(centerX + cos(step * 4.0f) * radiusX, centerY + sin(step * 4.0f) * radiusY);
            glVertex2f(centerX + cos(step * 5.0f) * radiusX, centerY + sin(step * 5.0f) * radiusY);
            glVertex2f(centerX + cos(step * 6.0f) * radiusX, centerY + sin(step * 6.0f) * radiusY);
            glVertex2f(centerX + cos(step * 7.0f) * radiusX, centerY + sin(step * 7.0f) * radiusY);
            glVertex2f(centerX + cos(step * 8.0f) * radiusX, centerY + sin(step * 8.0f) * radiusY);
            glVertex2f(centerX + cos(step * 9.0f) * radiusX, centerY + sin(step * 9.0f) * radiusY);
            glVertex2f(centerX + cos(step * 10.0f) * radiusX, centerY + sin(step * 10.0f) * radiusY);
            glVertex2f(centerX + cos(step * 11.0f) * radiusX, centerY + sin(step * 11.0f) * radiusY);
            glVertex2f(centerX + cos(step * 12.0f) * radiusX, centerY + sin(step * 12.0f) * radiusY);
            glVertex2f(centerX + cos(step * 13.0f) * radiusX, centerY + sin(step * 13.0f) * radiusY);
            glVertex2f(centerX + cos(step * 14.0f) * radiusX, centerY + sin(step * 14.0f) * radiusY);
            glVertex2f(centerX + cos(step * 15.0f) * radiusX, centerY + sin(step * 15.0f) * radiusY);
            glVertex2f(centerX + cos(step * 16.0f) * radiusX, centerY + sin(step * 16.0f) * radiusY);
            glVertex2f(centerX + cos(step * 17.0f) * radiusX, centerY + sin(step * 17.0f) * radiusY);
            glVertex2f(centerX + cos(step * 18.0f) * radiusX, centerY + sin(step * 18.0f) * radiusY);
            glVertex2f(centerX + cos(step * 19.0f) * radiusX, centerY + sin(step * 19.0f) * radiusY);
            glVertex2f(centerX + cos(step * 20.0f) * radiusX, centerY + sin(step * 20.0f) * radiusY);
            glVertex2f(centerX + cos(step * 21.0f) * radiusX, centerY + sin(step * 21.0f) * radiusY);
            glVertex2f(centerX + cos(step * 22.0f) * radiusX, centerY + sin(step * 22.0f) * radiusY);
            glVertex2f(centerX + cos(step * 23.0f) * radiusX, centerY + sin(step * 23.0f) * radiusY);
            glEnd();
        }
    } else {
        float skyBottomR = 0.26f;
        float skyBottomG = 0.28f;
        float skyBottomB = 0.32f;
        float skyTopR = 0.36f;
        float skyTopG = 0.38f;
        float skyTopB = 0.42f;
        glBegin(GL_QUADS);
        glColor3f(skyBottomR, skyBottomG, skyBottomB);
        glVertex2f(0.0f, 0.0f);
        glColor3f(skyBottomR, skyBottomG, skyBottomB);
        glVertex2f(1280.0f, 0.0f);
        glColor3f(skyTopR, skyTopG, skyTopB);
        glVertex2f(1280.0f, 720.0f);
        glColor3f(skyTopR, skyTopG, skyTopB);
        glVertex2f(0.0f, 720.0f);
        glEnd();

        glColor3f(0.28f, 0.30f, 0.34f);
        {
            float centerX = 240.0f + cloudOffsetX_layerA * 0.32f;
            float centerY = 620.0f;
            float radiusX = 170.0f;
            float radiusY = 45.0f;
            float step = 2.0f * PI_VALUE / 36.0f;
            glBegin(GL_POLYGON);
            glVertex2f(centerX + cos(step * 0.0f) * radiusX, centerY + sin(step * 0.0f) * radiusY);
            glVertex2f(centerX + cos(step * 1.0f) * radiusX, centerY + sin(step * 1.0f) * radiusY);
            glVertex2f(centerX + cos(step * 2.0f) * radiusX, centerY + sin(step * 2.0f) * radiusY);
            glVertex2f(centerX + cos(step * 3.0f) * radiusX, centerY + sin(step * 3.0f) * radiusY);
            glVertex2f(centerX + cos(step * 4.0f) * radiusX, centerY + sin(step * 4.0f) * radiusY);
            glVertex2f(centerX + cos(step * 5.0f) * radiusX, centerY + sin(step * 5.0f) * radiusY);
            glVertex2f(centerX + cos(step * 6.0f) * radiusX, centerY + sin(step * 6.0f) * radiusY);
            glVertex2f(centerX + cos(step * 7.0f) * radiusX, centerY + sin(step * 7.0f) * radiusY);
            glVertex2f(centerX + cos(step * 8.0f) * radiusX, centerY + sin(step * 8.0f) * radiusY);
            glVertex2f(centerX + cos(step * 9.0f) * radiusX, centerY + sin(step * 9.0f) * radiusY);
            glVertex2f(centerX + cos(step * 10.0f) * radiusX, centerY + sin(step * 10.0f) * radiusY);
            glVertex2f(centerX + cos(step * 11.0f) * radiusX, centerY + sin(step * 11.0f) * radiusY);
            glVertex2f(centerX + cos(step * 12.0f) * radiusX, centerY + sin(step * 12.0f) * radiusY);
            glVertex2f(centerX + cos(step * 13.0f) * radiusX, centerY + sin(step * 13.0f) * radiusY);
            glVertex2f(centerX + cos(step * 14.0f) * radiusX, centerY + sin(step * 14.0f) * radiusY);
            glVertex2f(centerX + cos(step * 15.0f) * radiusX, centerY + sin(step * 15.0f) * radiusY);
            glVertex2f(centerX + cos(step * 16.0f) * radiusX, centerY + sin(step * 16.0f) * radiusY);
            glVertex2f(centerX + cos(step * 17.0f) * radiusX, centerY + sin(step * 17.0f) * radiusY);
            glVertex2f(centerX + cos(step * 18.0f) * radiusX, centerY + sin(step * 18.0f) * radiusY);
            glVertex2f(centerX + cos(step * 19.0f) * radiusX, centerY + sin(step * 19.0f) * radiusY);
            glVertex2f(centerX + cos(step * 20.0f) * radiusX, centerY + sin(step * 20.0f) * radiusY);
            glVertex2f(centerX + cos(step * 21.0f) * radiusX, centerY + sin(step * 21.0f) * radiusY);
            glVertex2f(centerX + cos(step * 22.0f) * radiusX, centerY + sin(step * 22.0f) * radiusY);
            glVertex2f(centerX + cos(step * 23.0f) * radiusX, centerY + sin(step * 23.0f) * radiusY);
            glVertex2f(centerX + cos(step * 24.0f) * radiusX, centerY + sin(step * 24.0f) * radiusY);
            glVertex2f(centerX + cos(step * 25.0f) * radiusX, centerY + sin(step * 25.0f) * radiusY);
            glVertex2f(centerX + cos(step * 26.0f) * radiusX, centerY + sin(step * 26.0f) * radiusY);
            glVertex2f(centerX + cos(step * 27.0f) * radiusX, centerY + sin(step * 27.0f) * radiusY);
            glVertex2f(centerX + cos(step * 28.0f) * radiusX, centerY + sin(step * 28.0f) * radiusY);
            glVertex2f(centerX + cos(step * 29.0f) * radiusX, centerY + sin(step * 29.0f) * radiusY);
            glVertex2f(centerX + cos(step * 30.0f) * radiusX, centerY + sin(step * 30.0f) * radiusY);
            glVertex2f(centerX + cos(step * 31.0f) * radiusX, centerY + sin(step * 31.0f) * radiusY);
            glVertex2f(centerX + cos(step * 32.0f) * radiusX, centerY + sin(step * 32.0f) * radiusY);
            glVertex2f(centerX + cos(step * 33.0f) * radiusX, centerY + sin(step * 33.0f) * radiusY);
            glVertex2f(centerX + cos(step * 34.0f) * radiusX, centerY + sin(step * 34.0f) * radiusY);
            glVertex2f(centerX + cos(step * 35.0f) * radiusX, centerY + sin(step * 35.0f) * radiusY);
            glEnd();
        }
        {
            float centerX = 640.0f + cloudOffsetX_layerB * 0.30f;
            float centerY = 646.0f;
            float radiusX = 190.0f;
            float radiusY = 50.0f;
            float step = 2.0f * PI_VALUE / 36.0f;
            glBegin(GL_POLYGON);
            glVertex2f(centerX + cos(step * 0.0f) * radiusX, centerY + sin(step * 0.0f) * radiusY);
            glVertex2f(centerX + cos(step * 1.0f) * radiusX, centerY + sin(step * 1.0f) * radiusY);
            glVertex2f(centerX + cos(step * 2.0f) * radiusX, centerY + sin(step * 2.0f) * radiusY);
            glVertex2f(centerX + cos(step * 3.0f) * radiusX, centerY + sin(step * 3.0f) * radiusY);
            glVertex2f(centerX + cos(step * 4.0f) * radiusX, centerY + sin(step * 4.0f) * radiusY);
            glVertex2f(centerX + cos(step * 5.0f) * radiusX, centerY + sin(step * 5.0f) * radiusY);
            glVertex2f(centerX + cos(step * 6.0f) * radiusX, centerY + sin(step * 6.0f) * radiusY);
            glVertex2f(centerX + cos(step * 7.0f) * radiusX, centerY + sin(step * 7.0f) * radiusY);
            glVertex2f(centerX + cos(step * 8.0f) * radiusX, centerY + sin(step * 8.0f) * radiusY);
            glVertex2f(centerX + cos(step * 9.0f) * radiusX, centerY + sin(step * 9.0f) * radiusY);
            glVertex2f(centerX + cos(step * 10.0f) * radiusX, centerY + sin(step * 10.0f) * radiusY);
            glVertex2f(centerX + cos(step * 11.0f) * radiusX, centerY + sin(step * 11.0f) * radiusY);
            glVertex2f(centerX + cos(step * 12.0f) * radiusX, centerY + sin(step * 12.0f) * radiusY);
            glVertex2f(centerX + cos(step * 13.0f) * radiusX, centerY + sin(step * 13.0f) * radiusY);
            glVertex2f(centerX + cos(step * 14.0f) * radiusX, centerY + sin(step * 14.0f) * radiusY);
            glVertex2f(centerX + cos(step * 15.0f) * radiusX, centerY + sin(step * 15.0f) * radiusY);
            glVertex2f(centerX + cos(step * 16.0f) * radiusX, centerY + sin(step * 16.0f) * radiusY);
            glVertex2f(centerX + cos(step * 17.0f) * radiusX, centerY + sin(step * 17.0f) * radiusY);
            glVertex2f(centerX + cos(step * 18.0f) * radiusX, centerY + sin(step * 18.0f) * radiusY);
            glVertex2f(centerX + cos(step * 19.0f) * radiusX, centerY + sin(step * 19.0f) * radiusY);
            glVertex2f(centerX + cos(step * 20.0f) * radiusX, centerY + sin(step * 20.0f) * radiusY);
            glVertex2f(centerX + cos(step * 21.0f) * radiusX, centerY + sin(step * 21.0f) * radiusY);
            glVertex2f(centerX + cos(step * 22.0f) * radiusX, centerY + sin(step * 22.0f) * radiusY);
            glVertex2f(centerX + cos(step * 23.0f) * radiusX, centerY + sin(step * 23.0f) * radiusY);
            glVertex2f(centerX + cos(step * 24.0f) * radiusX, centerY + sin(step * 24.0f) * radiusY);
            glVertex2f(centerX + cos(step * 25.0f) * radiusX, centerY + sin(step * 25.0f) * radiusY);
            glVertex2f(centerX + cos(step * 26.0f) * radiusX, centerY + sin(step * 26.0f) * radiusY);
            glVertex2f(centerX + cos(step * 27.0f) * radiusX, centerY + sin(step * 27.0f) * radiusY);
            glVertex2f(centerX + cos(step * 28.0f) * radiusX, centerY + sin(step * 28.0f) * radiusY);
            glVertex2f(centerX + cos(step * 29.0f) * radiusX, centerY + sin(step * 29.0f) * radiusY);
            glVertex2f(centerX + cos(step * 30.0f) * radiusX, centerY + sin(step * 30.0f) * radiusY);
            glVertex2f(centerX + cos(step * 31.0f) * radiusX, centerY + sin(step * 31.0f) * radiusY);
            glVertex2f(centerX + cos(step * 32.0f) * radiusX, centerY + sin(step * 32.0f) * radiusY);
            glVertex2f(centerX + cos(step * 33.0f) * radiusX, centerY + sin(step * 33.0f) * radiusY);
            glVertex2f(centerX + cos(step * 34.0f) * radiusX, centerY + sin(step * 34.0f) * radiusY);
            glVertex2f(centerX + cos(step * 35.0f) * radiusX, centerY + sin(step * 35.0f) * radiusY);
            glEnd();
        }
    }
    parallaxCity(parallaxOffset_scene2, false);
    glColor3f(0.70f,0.74f,0.74f);
    {
        float left = 0.0f;
        float bottom = 340.0f;
        float right = left + 1280.0f;
        float top = bottom + 28.0f;
        glBegin(GL_QUADS);
        glVertex2f(left, bottom);
        glVertex2f(right, bottom);
        glVertex2f(right, top);
        glVertex2f(left, top);
        glEnd();
    }
    glColor3f(0.52f,0.56f,0.58f);
    {
        float left = 0.0f;
        float bottom = 340.0f;
        float right = left + 1280.0f;
        float top = bottom + 5.0f;
        glBegin(GL_QUADS);
        glVertex2f(left, bottom);
        glVertex2f(right, bottom);
        glVertex2f(right, top);
        glVertex2f(left, top);
        glEnd();
    }
    glColor3f(0.28f,0.48f,0.30f);
    {
        float left = 0.0f;
        float bottom = 368.0f;
        float right = left + 1280.0f;
        float top = bottom + 20.0f;
        glBegin(GL_QUADS);
        glVertex2f(left, bottom);
        glVertex2f(right, bottom);
        glVertex2f(right, top);
        glVertex2f(left, top);
        glEnd();
    }
    skylineTrees(366, false);
    glColor3f(0.34f,0.36f,0.38f);
    {
        float postX = 70.0f;
        rect(postX,342,5,70);
        glColor3f(0.20f,0.22f,0.24f);
        rect(postX-18,410,42,5);
        glColor3f(0.95f,0.88f,0.58f);
        rect(postX-14,402,34,8);
        glColor3f(0.34f,0.36f,0.38f);
    }
    {
        float postX = 250.0f;
        rect(postX,342,5,70);
        glColor3f(0.20f,0.22f,0.24f);
        rect(postX-18,410,42,5);
        glColor3f(0.95f,0.88f,0.58f);
        rect(postX-14,402,34,8);
        glColor3f(0.34f,0.36f,0.38f);
    }
    {
        float postX = 430.0f;
        rect(postX,342,5,70);
        glColor3f(0.20f,0.22f,0.24f);
        rect(postX-18,410,42,5);
        glColor3f(0.95f,0.88f,0.58f);
        rect(postX-14,402,34,8);
        glColor3f(0.34f,0.36f,0.38f);
    }
    {
        float postX = 610.0f;
        rect(postX,342,5,70);
        glColor3f(0.20f,0.22f,0.24f);
        rect(postX-18,410,42,5);
        glColor3f(0.95f,0.88f,0.58f);
        rect(postX-14,402,34,8);
        glColor3f(0.34f,0.36f,0.38f);
    }
    {
        float postX = 790.0f;
        rect(postX,342,5,70);
        glColor3f(0.20f,0.22f,0.24f);
        rect(postX-18,410,42,5);
        glColor3f(0.95f,0.88f,0.58f);
        rect(postX-14,402,34,8);
        glColor3f(0.34f,0.36f,0.38f);
    }
    {
        float postX = 970.0f;
        rect(postX,342,5,70);
        glColor3f(0.20f,0.22f,0.24f);
        rect(postX-18,410,42,5);
        glColor3f(0.95f,0.88f,0.58f);
        rect(postX-14,402,34,8);
        glColor3f(0.34f,0.36f,0.38f);
    }
    {
        float postX = 1150.0f;
        rect(postX,342,5,70);
        glColor3f(0.20f,0.22f,0.24f);
        rect(postX-18,410,42,5);
        glColor3f(0.95f,0.88f,0.58f);
        rect(postX-14,402,34,8);
        glColor3f(0.34f,0.36f,0.38f);
    }
    roadBottomY_current=120; roadHeight_current=220; roadLaneCount_current=4; roadNightMode_current=false;
    {
        float roadLeft = 0.0f;
        float roadBottom = roadBottomY_current;
        float roadRight = roadLeft + 1280.0f;
        float roadTop = roadBottom + roadHeight_current;
        glColor3f(0.23f, 0.23f, 0.25f);
        glBegin(GL_QUADS);
        glVertex2f(roadLeft, roadBottom);
        glVertex2f(roadRight, roadBottom);
        glVertex2f(roadRight, roadTop);
        glVertex2f(roadLeft, roadTop);
        glEnd();

        glColor3f(0.75f, 0.75f, 0.78f);
        {
            float left = roadLeft;
            float bottom = roadBottom;
            float right = roadRight;
            float top = bottom + 5.0f;
            glBegin(GL_QUADS);
            glVertex2f(left, bottom);
            glVertex2f(right, bottom);
            glVertex2f(right, top);
            glVertex2f(left, top);
            glEnd();
        }
        {
            float left = roadLeft;
            float bottom = roadBottom + roadHeight_current - 5.0f;
            float right = roadRight;
            float top = bottom + 5.0f;
            glBegin(GL_QUADS);
            glVertex2f(left, bottom);
            glVertex2f(right, bottom);
            glVertex2f(right, top);
            glVertex2f(left, top);
            glEnd();
        }

        float laneHeight = roadHeight_current / roadLaneCount_current;
        float laneY1 = roadBottom + laneHeight * 1.0f;
        float laneY2 = roadBottom + laneHeight * 2.0f;
        float laneY3 = roadBottom + laneHeight * 3.0f;
        float dashW = 48.0f;
        float dashH = 4.0f;
        float dashX0 = 20.0f;
        float dashX1 = 96.0f;
        float dashX2 = 172.0f;
        float dashX3 = 248.0f;
        float dashX4 = 324.0f;
        float dashX5 = 400.0f;
        float dashX6 = 476.0f;
        float dashX7 = 552.0f;
        float dashX8 = 628.0f;
        float dashX9 = 704.0f;
        float dashX10 = 780.0f;
        float dashX11 = 856.0f;
        float dashX12 = 932.0f;
        float dashX13 = 1008.0f;
        float dashX14 = 1084.0f;
        float dashX15 = 1160.0f;
        float dashX16 = 1236.0f;
        glColor3f(0.95f, 0.95f, 0.72f);
        {
            float bottom = laneY1 - 2.0f;
            float top = bottom + dashH;
            glBegin(GL_QUADS);
            glVertex2f(dashX0, bottom); glVertex2f(dashX0 + dashW, bottom); glVertex2f(dashX0 + dashW, top); glVertex2f(dashX0, top);
            glVertex2f(dashX1, bottom); glVertex2f(dashX1 + dashW, bottom); glVertex2f(dashX1 + dashW, top); glVertex2f(dashX1, top);
            glVertex2f(dashX2, bottom); glVertex2f(dashX2 + dashW, bottom); glVertex2f(dashX2 + dashW, top); glVertex2f(dashX2, top);
            glVertex2f(dashX3, bottom); glVertex2f(dashX3 + dashW, bottom); glVertex2f(dashX3 + dashW, top); glVertex2f(dashX3, top);
            glVertex2f(dashX4, bottom); glVertex2f(dashX4 + dashW, bottom); glVertex2f(dashX4 + dashW, top); glVertex2f(dashX4, top);
            glVertex2f(dashX5, bottom); glVertex2f(dashX5 + dashW, bottom); glVertex2f(dashX5 + dashW, top); glVertex2f(dashX5, top);
            glVertex2f(dashX6, bottom); glVertex2f(dashX6 + dashW, bottom); glVertex2f(dashX6 + dashW, top); glVertex2f(dashX6, top);
            glVertex2f(dashX7, bottom); glVertex2f(dashX7 + dashW, bottom); glVertex2f(dashX7 + dashW, top); glVertex2f(dashX7, top);
            glVertex2f(dashX8, bottom); glVertex2f(dashX8 + dashW, bottom); glVertex2f(dashX8 + dashW, top); glVertex2f(dashX8, top);
            glVertex2f(dashX9, bottom); glVertex2f(dashX9 + dashW, bottom); glVertex2f(dashX9 + dashW, top); glVertex2f(dashX9, top);
            glVertex2f(dashX10, bottom); glVertex2f(dashX10 + dashW, bottom); glVertex2f(dashX10 + dashW, top); glVertex2f(dashX10, top);
            glVertex2f(dashX11, bottom); glVertex2f(dashX11 + dashW, bottom); glVertex2f(dashX11 + dashW, top); glVertex2f(dashX11, top);
            glVertex2f(dashX12, bottom); glVertex2f(dashX12 + dashW, bottom); glVertex2f(dashX12 + dashW, top); glVertex2f(dashX12, top);
            glVertex2f(dashX13, bottom); glVertex2f(dashX13 + dashW, bottom); glVertex2f(dashX13 + dashW, top); glVertex2f(dashX13, top);
            glVertex2f(dashX14, bottom); glVertex2f(dashX14 + dashW, bottom); glVertex2f(dashX14 + dashW, top); glVertex2f(dashX14, top);
            glVertex2f(dashX15, bottom); glVertex2f(dashX15 + dashW, bottom); glVertex2f(dashX15 + dashW, top); glVertex2f(dashX15, top);
            glVertex2f(dashX16, bottom); glVertex2f(dashX16 + dashW, bottom); glVertex2f(dashX16 + dashW, top); glVertex2f(dashX16, top);
            glEnd();
        }
        {
            float bottom = laneY2 - 2.0f;
            float top = bottom + dashH;
            glBegin(GL_QUADS);
            glVertex2f(dashX0, bottom); glVertex2f(dashX0 + dashW, bottom); glVertex2f(dashX0 + dashW, top); glVertex2f(dashX0, top);
            glVertex2f(dashX1, bottom); glVertex2f(dashX1 + dashW, bottom); glVertex2f(dashX1 + dashW, top); glVertex2f(dashX1, top);
            glVertex2f(dashX2, bottom); glVertex2f(dashX2 + dashW, bottom); glVertex2f(dashX2 + dashW, top); glVertex2f(dashX2, top);
            glVertex2f(dashX3, bottom); glVertex2f(dashX3 + dashW, bottom); glVertex2f(dashX3 + dashW, top); glVertex2f(dashX3, top);
            glVertex2f(dashX4, bottom); glVertex2f(dashX4 + dashW, bottom); glVertex2f(dashX4 + dashW, top); glVertex2f(dashX4, top);
            glVertex2f(dashX5, bottom); glVertex2f(dashX5 + dashW, bottom); glVertex2f(dashX5 + dashW, top); glVertex2f(dashX5, top);
            glVertex2f(dashX6, bottom); glVertex2f(dashX6 + dashW, bottom); glVertex2f(dashX6 + dashW, top); glVertex2f(dashX6, top);
            glVertex2f(dashX7, bottom); glVertex2f(dashX7 + dashW, bottom); glVertex2f(dashX7 + dashW, top); glVertex2f(dashX7, top);
            glVertex2f(dashX8, bottom); glVertex2f(dashX8 + dashW, bottom); glVertex2f(dashX8 + dashW, top); glVertex2f(dashX8, top);
            glVertex2f(dashX9, bottom); glVertex2f(dashX9 + dashW, bottom); glVertex2f(dashX9 + dashW, top); glVertex2f(dashX9, top);
            glVertex2f(dashX10, bottom); glVertex2f(dashX10 + dashW, bottom); glVertex2f(dashX10 + dashW, top); glVertex2f(dashX10, top);
            glVertex2f(dashX11, bottom); glVertex2f(dashX11 + dashW, bottom); glVertex2f(dashX11 + dashW, top); glVertex2f(dashX11, top);
            glVertex2f(dashX12, bottom); glVertex2f(dashX12 + dashW, bottom); glVertex2f(dashX12 + dashW, top); glVertex2f(dashX12, top);
            glVertex2f(dashX13, bottom); glVertex2f(dashX13 + dashW, bottom); glVertex2f(dashX13 + dashW, top); glVertex2f(dashX13, top);
            glVertex2f(dashX14, bottom); glVertex2f(dashX14 + dashW, bottom); glVertex2f(dashX14 + dashW, top); glVertex2f(dashX14, top);
            glVertex2f(dashX15, bottom); glVertex2f(dashX15 + dashW, bottom); glVertex2f(dashX15 + dashW, top); glVertex2f(dashX15, top);
            glVertex2f(dashX16, bottom); glVertex2f(dashX16 + dashW, bottom); glVertex2f(dashX16 + dashW, top); glVertex2f(dashX16, top);
            glEnd();
        }
        {
            float bottom = laneY3 - 2.0f;
            float top = bottom + dashH;
            glBegin(GL_QUADS);
            glVertex2f(dashX0, bottom); glVertex2f(dashX0 + dashW, bottom); glVertex2f(dashX0 + dashW, top); glVertex2f(dashX0, top);
            glVertex2f(dashX1, bottom); glVertex2f(dashX1 + dashW, bottom); glVertex2f(dashX1 + dashW, top); glVertex2f(dashX1, top);
            glVertex2f(dashX2, bottom); glVertex2f(dashX2 + dashW, bottom); glVertex2f(dashX2 + dashW, top); glVertex2f(dashX2, top);
            glVertex2f(dashX3, bottom); glVertex2f(dashX3 + dashW, bottom); glVertex2f(dashX3 + dashW, top); glVertex2f(dashX3, top);
            glVertex2f(dashX4, bottom); glVertex2f(dashX4 + dashW, bottom); glVertex2f(dashX4 + dashW, top); glVertex2f(dashX4, top);
            glVertex2f(dashX5, bottom); glVertex2f(dashX5 + dashW, bottom); glVertex2f(dashX5 + dashW, top); glVertex2f(dashX5, top);
            glVertex2f(dashX6, bottom); glVertex2f(dashX6 + dashW, bottom); glVertex2f(dashX6 + dashW, top); glVertex2f(dashX6, top);
            glVertex2f(dashX7, bottom); glVertex2f(dashX7 + dashW, bottom); glVertex2f(dashX7 + dashW, top); glVertex2f(dashX7, top);
            glVertex2f(dashX8, bottom); glVertex2f(dashX8 + dashW, bottom); glVertex2f(dashX8 + dashW, top); glVertex2f(dashX8, top);
            glVertex2f(dashX9, bottom); glVertex2f(dashX9 + dashW, bottom); glVertex2f(dashX9 + dashW, top); glVertex2f(dashX9, top);
            glVertex2f(dashX10, bottom); glVertex2f(dashX10 + dashW, bottom); glVertex2f(dashX10 + dashW, top); glVertex2f(dashX10, top);
            glVertex2f(dashX11, bottom); glVertex2f(dashX11 + dashW, bottom); glVertex2f(dashX11 + dashW, top); glVertex2f(dashX11, top);
            glVertex2f(dashX12, bottom); glVertex2f(dashX12 + dashW, bottom); glVertex2f(dashX12 + dashW, top); glVertex2f(dashX12, top);
            glVertex2f(dashX13, bottom); glVertex2f(dashX13 + dashW, bottom); glVertex2f(dashX13 + dashW, top); glVertex2f(dashX13, top);
            glVertex2f(dashX14, bottom); glVertex2f(dashX14 + dashW, bottom); glVertex2f(dashX14 + dashW, top); glVertex2f(dashX14, top);
            glVertex2f(dashX15, bottom); glVertex2f(dashX15 + dashW, bottom); glVertex2f(dashX15 + dashW, top); glVertex2f(dashX15, top);
            glVertex2f(dashX16, bottom); glVertex2f(dashX16 + dashW, bottom); glVertex2f(dashX16 + dashW, top); glVertex2f(dashX16, top);
            glEnd();
        }
    }
    glColor3f(0.21f,0.21f,0.23f);
    {
        float left = 0.0f;
        float bottom = 226.0f;
        float right = left + 1280.0f;
        float top = bottom + 4.0f;
        glBegin(GL_QUADS);
        glVertex2f(left, bottom);
        glVertex2f(right, bottom);
        glVertex2f(right, top);
        glVertex2f(left, top);
        glEnd();
    }
    glColor3f(1.0f, 0.98f, 0.72f);
    {
        float dashStart = -80.0f + roadDashOffset_scene2;
        float dashSpacing = 92.0f;
        float dashW = 54.0f;
        float dashH = 4.0f;
        float dashOffset = 24.0f;
        float dashY1 = 171.0f;
        float dashY2 = 282.0f;
        float dashX0 = dashStart;
        float dashX1 = dashStart + dashSpacing;
        float dashX2 = dashStart + dashSpacing * 2.0f;
        float dashX3 = dashStart + dashSpacing * 3.0f;
        float dashX4 = dashStart + dashSpacing * 4.0f;
        float dashX5 = dashStart + dashSpacing * 5.0f;
        float dashX6 = dashStart + dashSpacing * 6.0f;
        float dashX7 = dashStart + dashSpacing * 7.0f;
        float dashX8 = dashStart + dashSpacing * 8.0f;
        float dashX9 = dashStart + dashSpacing * 9.0f;
        float dashX10 = dashStart + dashSpacing * 10.0f;
        float dashX11 = dashStart + dashSpacing * 11.0f;
        float dashX12 = dashStart + dashSpacing * 12.0f;
        float dashX13 = dashStart + dashSpacing * 13.0f;
        float dashX14 = dashStart + dashSpacing * 14.0f;
        glBegin(GL_QUADS);
        glVertex2f(dashX0, dashY1); glVertex2f(dashX0 + dashW, dashY1); glVertex2f(dashX0 + dashW, dashY1 + dashH); glVertex2f(dashX0, dashY1 + dashH);
        glVertex2f(dashX0 + dashOffset, dashY2); glVertex2f(dashX0 + dashOffset + dashW, dashY2); glVertex2f(dashX0 + dashOffset + dashW, dashY2 + dashH); glVertex2f(dashX0 + dashOffset, dashY2 + dashH);
        glVertex2f(dashX1, dashY1); glVertex2f(dashX1 + dashW, dashY1); glVertex2f(dashX1 + dashW, dashY1 + dashH); glVertex2f(dashX1, dashY1 + dashH);
        glVertex2f(dashX1 + dashOffset, dashY2); glVertex2f(dashX1 + dashOffset + dashW, dashY2); glVertex2f(dashX1 + dashOffset + dashW, dashY2 + dashH); glVertex2f(dashX1 + dashOffset, dashY2 + dashH);
        glVertex2f(dashX2, dashY1); glVertex2f(dashX2 + dashW, dashY1); glVertex2f(dashX2 + dashW, dashY1 + dashH); glVertex2f(dashX2, dashY1 + dashH);
        glVertex2f(dashX2 + dashOffset, dashY2); glVertex2f(dashX2 + dashOffset + dashW, dashY2); glVertex2f(dashX2 + dashOffset + dashW, dashY2 + dashH); glVertex2f(dashX2 + dashOffset, dashY2 + dashH);
        glVertex2f(dashX3, dashY1); glVertex2f(dashX3 + dashW, dashY1); glVertex2f(dashX3 + dashW, dashY1 + dashH); glVertex2f(dashX3, dashY1 + dashH);
        glVertex2f(dashX3 + dashOffset, dashY2); glVertex2f(dashX3 + dashOffset + dashW, dashY2); glVertex2f(dashX3 + dashOffset + dashW, dashY2 + dashH); glVertex2f(dashX3 + dashOffset, dashY2 + dashH);
        glVertex2f(dashX4, dashY1); glVertex2f(dashX4 + dashW, dashY1); glVertex2f(dashX4 + dashW, dashY1 + dashH); glVertex2f(dashX4, dashY1 + dashH);
        glVertex2f(dashX4 + dashOffset, dashY2); glVertex2f(dashX4 + dashOffset + dashW, dashY2); glVertex2f(dashX4 + dashOffset + dashW, dashY2 + dashH); glVertex2f(dashX4 + dashOffset, dashY2 + dashH);
        glVertex2f(dashX5, dashY1); glVertex2f(dashX5 + dashW, dashY1); glVertex2f(dashX5 + dashW, dashY1 + dashH); glVertex2f(dashX5, dashY1 + dashH);
        glVertex2f(dashX5 + dashOffset, dashY2); glVertex2f(dashX5 + dashOffset + dashW, dashY2); glVertex2f(dashX5 + dashOffset + dashW, dashY2 + dashH); glVertex2f(dashX5 + dashOffset, dashY2 + dashH);
        glVertex2f(dashX6, dashY1); glVertex2f(dashX6 + dashW, dashY1); glVertex2f(dashX6 + dashW, dashY1 + dashH); glVertex2f(dashX6, dashY1 + dashH);
        glVertex2f(dashX6 + dashOffset, dashY2); glVertex2f(dashX6 + dashOffset + dashW, dashY2); glVertex2f(dashX6 + dashOffset + dashW, dashY2 + dashH); glVertex2f(dashX6 + dashOffset, dashY2 + dashH);
        glVertex2f(dashX7, dashY1); glVertex2f(dashX7 + dashW, dashY1); glVertex2f(dashX7 + dashW, dashY1 + dashH); glVertex2f(dashX7, dashY1 + dashH);
        glVertex2f(dashX7 + dashOffset, dashY2); glVertex2f(dashX7 + dashOffset + dashW, dashY2); glVertex2f(dashX7 + dashOffset + dashW, dashY2 + dashH); glVertex2f(dashX7 + dashOffset, dashY2 + dashH);
        glVertex2f(dashX8, dashY1); glVertex2f(dashX8 + dashW, dashY1); glVertex2f(dashX8 + dashW, dashY1 + dashH); glVertex2f(dashX8, dashY1 + dashH);
        glVertex2f(dashX8 + dashOffset, dashY2); glVertex2f(dashX8 + dashOffset + dashW, dashY2); glVertex2f(dashX8 + dashOffset + dashW, dashY2 + dashH); glVertex2f(dashX8 + dashOffset, dashY2 + dashH);
        glVertex2f(dashX9, dashY1); glVertex2f(dashX9 + dashW, dashY1); glVertex2f(dashX9 + dashW, dashY1 + dashH); glVertex2f(dashX9, dashY1 + dashH);
        glVertex2f(dashX9 + dashOffset, dashY2); glVertex2f(dashX9 + dashOffset + dashW, dashY2); glVertex2f(dashX9 + dashOffset + dashW, dashY2 + dashH); glVertex2f(dashX9 + dashOffset, dashY2 + dashH);
        glVertex2f(dashX10, dashY1); glVertex2f(dashX10 + dashW, dashY1); glVertex2f(dashX10 + dashW, dashY1 + dashH); glVertex2f(dashX10, dashY1 + dashH);
        glVertex2f(dashX10 + dashOffset, dashY2); glVertex2f(dashX10 + dashOffset + dashW, dashY2); glVertex2f(dashX10 + dashOffset + dashW, dashY2 + dashH); glVertex2f(dashX10 + dashOffset, dashY2 + dashH);
        glVertex2f(dashX11, dashY1); glVertex2f(dashX11 + dashW, dashY1); glVertex2f(dashX11 + dashW, dashY1 + dashH); glVertex2f(dashX11, dashY1 + dashH);
        glVertex2f(dashX11 + dashOffset, dashY2); glVertex2f(dashX11 + dashOffset + dashW, dashY2); glVertex2f(dashX11 + dashOffset + dashW, dashY2 + dashH); glVertex2f(dashX11 + dashOffset, dashY2 + dashH);
        glVertex2f(dashX12, dashY1); glVertex2f(dashX12 + dashW, dashY1); glVertex2f(dashX12 + dashW, dashY1 + dashH); glVertex2f(dashX12, dashY1 + dashH);
        glVertex2f(dashX12 + dashOffset, dashY2); glVertex2f(dashX12 + dashOffset + dashW, dashY2); glVertex2f(dashX12 + dashOffset + dashW, dashY2 + dashH); glVertex2f(dashX12 + dashOffset, dashY2 + dashH);
        glVertex2f(dashX13, dashY1); glVertex2f(dashX13 + dashW, dashY1); glVertex2f(dashX13 + dashW, dashY1 + dashH); glVertex2f(dashX13, dashY1 + dashH);
        glVertex2f(dashX13 + dashOffset, dashY2); glVertex2f(dashX13 + dashOffset + dashW, dashY2); glVertex2f(dashX13 + dashOffset + dashW, dashY2 + dashH); glVertex2f(dashX13 + dashOffset, dashY2 + dashH);
        glVertex2f(dashX14, dashY1); glVertex2f(dashX14 + dashW, dashY1); glVertex2f(dashX14 + dashW, dashY1 + dashH); glVertex2f(dashX14, dashY1 + dashH);
        glVertex2f(dashX14 + dashOffset, dashY2); glVertex2f(dashX14 + dashOffset + dashW, dashY2); glVertex2f(dashX14 + dashOffset + dashW, dashY2 + dashH); glVertex2f(dashX14 + dashOffset, dashY2 + dashH);
        glEnd();
    }
    glColor3f(0.24f,0.52f,0.26f);
    {
        float left = 0.0f;
        float bottom = 0.0f;
        float right = left + 1280.0f;
        float top = bottom + 120.0f;
        glBegin(GL_QUADS);
        glVertex2f(left, bottom);
        glVertex2f(right, bottom);
        glVertex2f(right, top);
        glVertex2f(left, top);
        glEnd();
    }
    glColor3f(0.30f,0.58f,0.34f);
    ellipse(26.0f,112.0f,38.0f,10.0f,18);
    ellipse(112.0f,112.0f,38.0f,10.0f,18);
    ellipse(198.0f,112.0f,38.0f,10.0f,18);
    ellipse(284.0f,112.0f,38.0f,10.0f,18);
    ellipse(370.0f,112.0f,38.0f,10.0f,18);
    ellipse(456.0f,112.0f,38.0f,10.0f,18);
    ellipse(542.0f,112.0f,38.0f,10.0f,18);
    ellipse(628.0f,112.0f,38.0f,10.0f,18);
    ellipse(714.0f,112.0f,38.0f,10.0f,18);
    ellipse(800.0f,112.0f,38.0f,10.0f,18);
    ellipse(886.0f,112.0f,38.0f,10.0f,18);
    ellipse(972.0f,112.0f,38.0f,10.0f,18);
    ellipse(1058.0f,112.0f,38.0f,10.0f,18);
    ellipse(1144.0f,112.0f,38.0f,10.0f,18);
    ellipse(1230.0f,112.0f,38.0f,10.0f,18);
    glColor3f(0.68f,0.68f,0.64f);
    rect(0,112,1280,8);
    float bobA = sin(sceneFrameCounter*0.18f + trafficCarPositionX_scene2_A*0.03f) * 1.2f;
    float bobB = sin(sceneFrameCounter*0.12f + trafficCarPositionX_scene2_B*0.02f) * 0.8f;
    float bobC = sin(sceneFrameCounter*0.20f + trafficCarPositionX_scene2_C*0.025f) * 1.0f;
    {
        float carX = trafficCarPositionX_scene2_A;
        float carY = 168.0f + bobA;
        float carR = 0.88f;
        float carG = 0.22f;
        float carB = 0.18f;
        float wheelAngle = trafficCarWheelAngle_scene2_A;
        bool headlightOn = false;
        bool tailLightOn = false;
        bool facingRight = true;

        glPushMatrix();
        glTranslatef(carX, carY, 0.0f);
        if (!facingRight) glScalef(-1.0f, 1.0f, 1.0f);

        glEnable(GL_BLEND);
        glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
        glColor4f(0.0f, 0.0f, 0.0f, 0.24f);
        {
            float centerX = 0.0f;
            float centerY = -10.0f;
            float radiusX = 74.0f;
            float radiusY = 11.0f;
            float step = 2.0f * PI_VALUE / 36.0f;
            glBegin(GL_POLYGON);
            glVertex2f(centerX + cos(step * 0.0f) * radiusX, centerY + sin(step * 0.0f) * radiusY);
            glVertex2f(centerX + cos(step * 1.0f) * radiusX, centerY + sin(step * 1.0f) * radiusY);
            glVertex2f(centerX + cos(step * 2.0f) * radiusX, centerY + sin(step * 2.0f) * radiusY);
            glVertex2f(centerX + cos(step * 3.0f) * radiusX, centerY + sin(step * 3.0f) * radiusY);
            glVertex2f(centerX + cos(step * 4.0f) * radiusX, centerY + sin(step * 4.0f) * radiusY);
            glVertex2f(centerX + cos(step * 5.0f) * radiusX, centerY + sin(step * 5.0f) * radiusY);
            glVertex2f(centerX + cos(step * 6.0f) * radiusX, centerY + sin(step * 6.0f) * radiusY);
            glVertex2f(centerX + cos(step * 7.0f) * radiusX, centerY + sin(step * 7.0f) * radiusY);
            glVertex2f(centerX + cos(step * 8.0f) * radiusX, centerY + sin(step * 8.0f) * radiusY);
            glVertex2f(centerX + cos(step * 9.0f) * radiusX, centerY + sin(step * 9.0f) * radiusY);
            glVertex2f(centerX + cos(step * 10.0f) * radiusX, centerY + sin(step * 10.0f) * radiusY);
            glVertex2f(centerX + cos(step * 11.0f) * radiusX, centerY + sin(step * 11.0f) * radiusY);
            glVertex2f(centerX + cos(step * 12.0f) * radiusX, centerY + sin(step * 12.0f) * radiusY);
            glVertex2f(centerX + cos(step * 13.0f) * radiusX, centerY + sin(step * 13.0f) * radiusY);
            glVertex2f(centerX + cos(step * 14.0f) * radiusX, centerY + sin(step * 14.0f) * radiusY);
            glVertex2f(centerX + cos(step * 15.0f) * radiusX, centerY + sin(step * 15.0f) * radiusY);
            glVertex2f(centerX + cos(step * 16.0f) * radiusX, centerY + sin(step * 16.0f) * radiusY);
            glVertex2f(centerX + cos(step * 17.0f) * radiusX, centerY + sin(step * 17.0f) * radiusY);
            glVertex2f(centerX + cos(step * 18.0f) * radiusX, centerY + sin(step * 18.0f) * radiusY);
            glVertex2f(centerX + cos(step * 19.0f) * radiusX, centerY + sin(step * 19.0f) * radiusY);
            glVertex2f(centerX + cos(step * 20.0f) * radiusX, centerY + sin(step * 20.0f) * radiusY);
            glVertex2f(centerX + cos(step * 21.0f) * radiusX, centerY + sin(step * 21.0f) * radiusY);
            glVertex2f(centerX + cos(step * 22.0f) * radiusX, centerY + sin(step * 22.0f) * radiusY);
            glVertex2f(centerX + cos(step * 23.0f) * radiusX, centerY + sin(step * 23.0f) * radiusY);
            glVertex2f(centerX + cos(step * 24.0f) * radiusX, centerY + sin(step * 24.0f) * radiusY);
            glVertex2f(centerX + cos(step * 25.0f) * radiusX, centerY + sin(step * 25.0f) * radiusY);
            glVertex2f(centerX + cos(step * 26.0f) * radiusX, centerY + sin(step * 26.0f) * radiusY);
            glVertex2f(centerX + cos(step * 27.0f) * radiusX, centerY + sin(step * 27.0f) * radiusY);
            glVertex2f(centerX + cos(step * 28.0f) * radiusX, centerY + sin(step * 28.0f) * radiusY);
            glVertex2f(centerX + cos(step * 29.0f) * radiusX, centerY + sin(step * 29.0f) * radiusY);
            glVertex2f(centerX + cos(step * 30.0f) * radiusX, centerY + sin(step * 30.0f) * radiusY);
            glVertex2f(centerX + cos(step * 31.0f) * radiusX, centerY + sin(step * 31.0f) * radiusY);
            glVertex2f(centerX + cos(step * 32.0f) * radiusX, centerY + sin(step * 32.0f) * radiusY);
            glVertex2f(centerX + cos(step * 33.0f) * radiusX, centerY + sin(step * 33.0f) * radiusY);
            glVertex2f(centerX + cos(step * 34.0f) * radiusX, centerY + sin(step * 34.0f) * radiusY);
            glVertex2f(centerX + cos(step * 35.0f) * radiusX, centerY + sin(step * 35.0f) * radiusY);
            glEnd();
        }

        glColor3f(carR, carG, carB);
        {
            float left = -71.0f;
            float bottom = 0.0f;
            float right = left + 142.0f;
            float top = bottom + 32.0f;
            glBegin(GL_QUADS);
            glVertex2f(left, bottom);
            glVertex2f(right, bottom);
            glVertex2f(right, top);
            glVertex2f(left, top);
            glEnd();
        }
        glBegin(GL_POLYGON);
        glVertex2f(-38.0f, 32.0f);
        glVertex2f(42.0f, 32.0f);
        glVertex2f(20.0f, 58.0f);
        glVertex2f(-20.0f, 58.0f);
        glEnd();
        glColor3f(0.64f, 0.84f, 0.96f);
        glBegin(GL_POLYGON);
        glVertex2f(-30.0f, 35.0f);
        glVertex2f(34.0f, 35.0f);
        glVertex2f(16.0f, 54.0f);
        glVertex2f(-14.0f, 54.0f);
        glEnd();
        glColor3f(0.18f, 0.18f, 0.20f);
        {
            float left = 64.0f;
            float bottom = 8.0f;
            float right = left + 10.0f;
            float top = bottom + 14.0f;
            glBegin(GL_QUADS);
            glVertex2f(left, bottom);
            glVertex2f(right, bottom);
            glVertex2f(right, top);
            glVertex2f(left, top);
            glEnd();
        }
        {
            float left = -74.0f;
            float bottom = 8.0f;
            float right = left + 10.0f;
            float top = bottom + 14.0f;
            glBegin(GL_QUADS);
            glVertex2f(left, bottom);
            glVertex2f(right, bottom);
            glVertex2f(right, top);
            glVertex2f(left, top);
            glEnd();
        }

        glPushMatrix();
        glTranslatef(-42.0f, -2.0f, 0.0f);
        glRotatef(wheelAngle, 0.0f, 0.0f, 1.0f);
        glColor3f(0.05f, 0.05f, 0.06f);
        {
            float centerX = 0.0f;
            float centerY = 0.0f;
            float radiusX = 16.0f;
            float radiusY = 16.0f;
            float step = 2.0f * PI_VALUE / 36.0f;
            glBegin(GL_POLYGON);
            glVertex2f(centerX + cos(step * 0.0f) * radiusX, centerY + sin(step * 0.0f) * radiusY);
            glVertex2f(centerX + cos(step * 1.0f) * radiusX, centerY + sin(step * 1.0f) * radiusY);
            glVertex2f(centerX + cos(step * 2.0f) * radiusX, centerY + sin(step * 2.0f) * radiusY);
            glVertex2f(centerX + cos(step * 3.0f) * radiusX, centerY + sin(step * 3.0f) * radiusY);
            glVertex2f(centerX + cos(step * 4.0f) * radiusX, centerY + sin(step * 4.0f) * radiusY);
            glVertex2f(centerX + cos(step * 5.0f) * radiusX, centerY + sin(step * 5.0f) * radiusY);
            glVertex2f(centerX + cos(step * 6.0f) * radiusX, centerY + sin(step * 6.0f) * radiusY);
            glVertex2f(centerX + cos(step * 7.0f) * radiusX, centerY + sin(step * 7.0f) * radiusY);
            glVertex2f(centerX + cos(step * 8.0f) * radiusX, centerY + sin(step * 8.0f) * radiusY);
            glVertex2f(centerX + cos(step * 9.0f) * radiusX, centerY + sin(step * 9.0f) * radiusY);
            glVertex2f(centerX + cos(step * 10.0f) * radiusX, centerY + sin(step * 10.0f) * radiusY);
            glVertex2f(centerX + cos(step * 11.0f) * radiusX, centerY + sin(step * 11.0f) * radiusY);
            glVertex2f(centerX + cos(step * 12.0f) * radiusX, centerY + sin(step * 12.0f) * radiusY);
            glVertex2f(centerX + cos(step * 13.0f) * radiusX, centerY + sin(step * 13.0f) * radiusY);
            glVertex2f(centerX + cos(step * 14.0f) * radiusX, centerY + sin(step * 14.0f) * radiusY);
            glVertex2f(centerX + cos(step * 15.0f) * radiusX, centerY + sin(step * 15.0f) * radiusY);
            glVertex2f(centerX + cos(step * 16.0f) * radiusX, centerY + sin(step * 16.0f) * radiusY);
            glVertex2f(centerX + cos(step * 17.0f) * radiusX, centerY + sin(step * 17.0f) * radiusY);
            glVertex2f(centerX + cos(step * 18.0f) * radiusX, centerY + sin(step * 18.0f) * radiusY);
            glVertex2f(centerX + cos(step * 19.0f) * radiusX, centerY + sin(step * 19.0f) * radiusY);
            glVertex2f(centerX + cos(step * 20.0f) * radiusX, centerY + sin(step * 20.0f) * radiusY);
            glVertex2f(centerX + cos(step * 21.0f) * radiusX, centerY + sin(step * 21.0f) * radiusY);
            glVertex2f(centerX + cos(step * 22.0f) * radiusX, centerY + sin(step * 22.0f) * radiusY);
            glVertex2f(centerX + cos(step * 23.0f) * radiusX, centerY + sin(step * 23.0f) * radiusY);
            glVertex2f(centerX + cos(step * 24.0f) * radiusX, centerY + sin(step * 24.0f) * radiusY);
            glVertex2f(centerX + cos(step * 25.0f) * radiusX, centerY + sin(step * 25.0f) * radiusY);
            glVertex2f(centerX + cos(step * 26.0f) * radiusX, centerY + sin(step * 26.0f) * radiusY);
            glVertex2f(centerX + cos(step * 27.0f) * radiusX, centerY + sin(step * 27.0f) * radiusY);
            glVertex2f(centerX + cos(step * 28.0f) * radiusX, centerY + sin(step * 28.0f) * radiusY);
            glVertex2f(centerX + cos(step * 29.0f) * radiusX, centerY + sin(step * 29.0f) * radiusY);
            glVertex2f(centerX + cos(step * 30.0f) * radiusX, centerY + sin(step * 30.0f) * radiusY);
            glVertex2f(centerX + cos(step * 31.0f) * radiusX, centerY + sin(step * 31.0f) * radiusY);
            glVertex2f(centerX + cos(step * 32.0f) * radiusX, centerY + sin(step * 32.0f) * radiusY);
            glVertex2f(centerX + cos(step * 33.0f) * radiusX, centerY + sin(step * 33.0f) * radiusY);
            glVertex2f(centerX + cos(step * 34.0f) * radiusX, centerY + sin(step * 34.0f) * radiusY);
            glVertex2f(centerX + cos(step * 35.0f) * radiusX, centerY + sin(step * 35.0f) * radiusY);
            glEnd();
        }
        glColor3f(0.15f, 0.15f, 0.17f);
        {
            float centerX = 0.0f;
            float centerY = 0.0f;
            float radiusX = 12.0f;
            float radiusY = 12.0f;
            float step = 2.0f * PI_VALUE / 30.0f;
            glBegin(GL_POLYGON);
            glVertex2f(centerX + cos(step * 0.0f) * radiusX, centerY + sin(step * 0.0f) * radiusY);
            glVertex2f(centerX + cos(step * 1.0f) * radiusX, centerY + sin(step * 1.0f) * radiusY);
            glVertex2f(centerX + cos(step * 2.0f) * radiusX, centerY + sin(step * 2.0f) * radiusY);
            glVertex2f(centerX + cos(step * 3.0f) * radiusX, centerY + sin(step * 3.0f) * radiusY);
            glVertex2f(centerX + cos(step * 4.0f) * radiusX, centerY + sin(step * 4.0f) * radiusY);
            glVertex2f(centerX + cos(step * 5.0f) * radiusX, centerY + sin(step * 5.0f) * radiusY);
            glVertex2f(centerX + cos(step * 6.0f) * radiusX, centerY + sin(step * 6.0f) * radiusY);
            glVertex2f(centerX + cos(step * 7.0f) * radiusX, centerY + sin(step * 7.0f) * radiusY);
            glVertex2f(centerX + cos(step * 8.0f) * radiusX, centerY + sin(step * 8.0f) * radiusY);
            glVertex2f(centerX + cos(step * 9.0f) * radiusX, centerY + sin(step * 9.0f) * radiusY);
            glVertex2f(centerX + cos(step * 10.0f) * radiusX, centerY + sin(step * 10.0f) * radiusY);
            glVertex2f(centerX + cos(step * 11.0f) * radiusX, centerY + sin(step * 11.0f) * radiusY);
            glVertex2f(centerX + cos(step * 12.0f) * radiusX, centerY + sin(step * 12.0f) * radiusY);
            glVertex2f(centerX + cos(step * 13.0f) * radiusX, centerY + sin(step * 13.0f) * radiusY);
            glVertex2f(centerX + cos(step * 14.0f) * radiusX, centerY + sin(step * 14.0f) * radiusY);
            glVertex2f(centerX + cos(step * 15.0f) * radiusX, centerY + sin(step * 15.0f) * radiusY);
            glVertex2f(centerX + cos(step * 16.0f) * radiusX, centerY + sin(step * 16.0f) * radiusY);
            glVertex2f(centerX + cos(step * 17.0f) * radiusX, centerY + sin(step * 17.0f) * radiusY);
            glVertex2f(centerX + cos(step * 18.0f) * radiusX, centerY + sin(step * 18.0f) * radiusY);
            glVertex2f(centerX + cos(step * 19.0f) * radiusX, centerY + sin(step * 19.0f) * radiusY);
            glVertex2f(centerX + cos(step * 20.0f) * radiusX, centerY + sin(step * 20.0f) * radiusY);
            glVertex2f(centerX + cos(step * 21.0f) * radiusX, centerY + sin(step * 21.0f) * radiusY);
            glVertex2f(centerX + cos(step * 22.0f) * radiusX, centerY + sin(step * 22.0f) * radiusY);
            glVertex2f(centerX + cos(step * 23.0f) * radiusX, centerY + sin(step * 23.0f) * radiusY);
            glVertex2f(centerX + cos(step * 24.0f) * radiusX, centerY + sin(step * 24.0f) * radiusY);
            glVertex2f(centerX + cos(step * 25.0f) * radiusX, centerY + sin(step * 25.0f) * radiusY);
            glVertex2f(centerX + cos(step * 26.0f) * radiusX, centerY + sin(step * 26.0f) * radiusY);
            glVertex2f(centerX + cos(step * 27.0f) * radiusX, centerY + sin(step * 27.0f) * radiusY);
            glVertex2f(centerX + cos(step * 28.0f) * radiusX, centerY + sin(step * 28.0f) * radiusY);
            glVertex2f(centerX + cos(step * 29.0f) * radiusX, centerY + sin(step * 29.0f) * radiusY);
            glEnd();
        }
        glColor3f(0.74f, 0.74f, 0.78f);
        {
            float centerX = 0.0f;
            float centerY = 0.0f;
            float radiusX = 7.0f;
            float radiusY = 7.0f;
            float step = 2.0f * PI_VALUE / 28.0f;
            glBegin(GL_POLYGON);
            glVertex2f(centerX + cos(step * 0.0f) * radiusX, centerY + sin(step * 0.0f) * radiusY);
            glVertex2f(centerX + cos(step * 1.0f) * radiusX, centerY + sin(step * 1.0f) * radiusY);
            glVertex2f(centerX + cos(step * 2.0f) * radiusX, centerY + sin(step * 2.0f) * radiusY);
            glVertex2f(centerX + cos(step * 3.0f) * radiusX, centerY + sin(step * 3.0f) * radiusY);
            glVertex2f(centerX + cos(step * 4.0f) * radiusX, centerY + sin(step * 4.0f) * radiusY);
            glVertex2f(centerX + cos(step * 5.0f) * radiusX, centerY + sin(step * 5.0f) * radiusY);
            glVertex2f(centerX + cos(step * 6.0f) * radiusX, centerY + sin(step * 6.0f) * radiusY);
            glVertex2f(centerX + cos(step * 7.0f) * radiusX, centerY + sin(step * 7.0f) * radiusY);
            glVertex2f(centerX + cos(step * 8.0f) * radiusX, centerY + sin(step * 8.0f) * radiusY);
            glVertex2f(centerX + cos(step * 9.0f) * radiusX, centerY + sin(step * 9.0f) * radiusY);
            glVertex2f(centerX + cos(step * 10.0f) * radiusX, centerY + sin(step * 10.0f) * radiusY);
            glVertex2f(centerX + cos(step * 11.0f) * radiusX, centerY + sin(step * 11.0f) * radiusY);
            glVertex2f(centerX + cos(step * 12.0f) * radiusX, centerY + sin(step * 12.0f) * radiusY);
            glVertex2f(centerX + cos(step * 13.0f) * radiusX, centerY + sin(step * 13.0f) * radiusY);
            glVertex2f(centerX + cos(step * 14.0f) * radiusX, centerY + sin(step * 14.0f) * radiusY);
            glVertex2f(centerX + cos(step * 15.0f) * radiusX, centerY + sin(step * 15.0f) * radiusY);
            glVertex2f(centerX + cos(step * 16.0f) * radiusX, centerY + sin(step * 16.0f) * radiusY);
            glVertex2f(centerX + cos(step * 17.0f) * radiusX, centerY + sin(step * 17.0f) * radiusY);
            glVertex2f(centerX + cos(step * 18.0f) * radiusX, centerY + sin(step * 18.0f) * radiusY);
            glVertex2f(centerX + cos(step * 19.0f) * radiusX, centerY + sin(step * 19.0f) * radiusY);
            glVertex2f(centerX + cos(step * 20.0f) * radiusX, centerY + sin(step * 20.0f) * radiusY);
            glVertex2f(centerX + cos(step * 21.0f) * radiusX, centerY + sin(step * 21.0f) * radiusY);
            glVertex2f(centerX + cos(step * 22.0f) * radiusX, centerY + sin(step * 22.0f) * radiusY);
            glVertex2f(centerX + cos(step * 23.0f) * radiusX, centerY + sin(step * 23.0f) * radiusY);
            glVertex2f(centerX + cos(step * 24.0f) * radiusX, centerY + sin(step * 24.0f) * radiusY);
            glVertex2f(centerX + cos(step * 25.0f) * radiusX, centerY + sin(step * 25.0f) * radiusY);
            glVertex2f(centerX + cos(step * 26.0f) * radiusX, centerY + sin(step * 26.0f) * radiusY);
            glVertex2f(centerX + cos(step * 27.0f) * radiusX, centerY + sin(step * 27.0f) * radiusY);
            glEnd();
        }
        glColor3f(0.95f, 0.95f, 0.96f);
        glLineWidth(2.0f);
        glBegin(GL_LINES);
        glVertex2f(-8.0f, 0.0f);
        glVertex2f(8.0f, 0.0f);
        glVertex2f(0.0f, -8.0f);
        glVertex2f(0.0f, 8.0f);
        glVertex2f(-5.5f, -5.5f);
        glVertex2f(5.5f, 5.5f);
        glVertex2f(-5.5f, 5.5f);
        glVertex2f(5.5f, -5.5f);
        glEnd();
        glLineWidth(1.0f);
        glPopMatrix();

        glPushMatrix();
        glTranslatef(42.0f, -2.0f, 0.0f);
        glRotatef(wheelAngle, 0.0f, 0.0f, 1.0f);
        glColor3f(0.05f, 0.05f, 0.06f);
        {
            float centerX = 0.0f;
            float centerY = 0.0f;
            float radiusX = 16.0f;
            float radiusY = 16.0f;
            float step = 2.0f * PI_VALUE / 36.0f;
            glBegin(GL_POLYGON);
            glVertex2f(centerX + cos(step * 0.0f) * radiusX, centerY + sin(step * 0.0f) * radiusY);
            glVertex2f(centerX + cos(step * 1.0f) * radiusX, centerY + sin(step * 1.0f) * radiusY);
            glVertex2f(centerX + cos(step * 2.0f) * radiusX, centerY + sin(step * 2.0f) * radiusY);
            glVertex2f(centerX + cos(step * 3.0f) * radiusX, centerY + sin(step * 3.0f) * radiusY);
            glVertex2f(centerX + cos(step * 4.0f) * radiusX, centerY + sin(step * 4.0f) * radiusY);
            glVertex2f(centerX + cos(step * 5.0f) * radiusX, centerY + sin(step * 5.0f) * radiusY);
            glVertex2f(centerX + cos(step * 6.0f) * radiusX, centerY + sin(step * 6.0f) * radiusY);
            glVertex2f(centerX + cos(step * 7.0f) * radiusX, centerY + sin(step * 7.0f) * radiusY);
            glVertex2f(centerX + cos(step * 8.0f) * radiusX, centerY + sin(step * 8.0f) * radiusY);
            glVertex2f(centerX + cos(step * 9.0f) * radiusX, centerY + sin(step * 9.0f) * radiusY);
            glVertex2f(centerX + cos(step * 10.0f) * radiusX, centerY + sin(step * 10.0f) * radiusY);
            glVertex2f(centerX + cos(step * 11.0f) * radiusX, centerY + sin(step * 11.0f) * radiusY);
            glVertex2f(centerX + cos(step * 12.0f) * radiusX, centerY + sin(step * 12.0f) * radiusY);
            glVertex2f(centerX + cos(step * 13.0f) * radiusX, centerY + sin(step * 13.0f) * radiusY);
            glVertex2f(centerX + cos(step * 14.0f) * radiusX, centerY + sin(step * 14.0f) * radiusY);
            glVertex2f(centerX + cos(step * 15.0f) * radiusX, centerY + sin(step * 15.0f) * radiusY);
            glVertex2f(centerX + cos(step * 16.0f) * radiusX, centerY + sin(step * 16.0f) * radiusY);
            glVertex2f(centerX + cos(step * 17.0f) * radiusX, centerY + sin(step * 17.0f) * radiusY);
            glVertex2f(centerX + cos(step * 18.0f) * radiusX, centerY + sin(step * 18.0f) * radiusY);
            glVertex2f(centerX + cos(step * 19.0f) * radiusX, centerY + sin(step * 19.0f) * radiusY);
            glVertex2f(centerX + cos(step * 20.0f) * radiusX, centerY + sin(step * 20.0f) * radiusY);
            glVertex2f(centerX + cos(step * 21.0f) * radiusX, centerY + sin(step * 21.0f) * radiusY);
            glVertex2f(centerX + cos(step * 22.0f) * radiusX, centerY + sin(step * 22.0f) * radiusY);
            glVertex2f(centerX + cos(step * 23.0f) * radiusX, centerY + sin(step * 23.0f) * radiusY);
            glVertex2f(centerX + cos(step * 24.0f) * radiusX, centerY + sin(step * 24.0f) * radiusY);
            glVertex2f(centerX + cos(step * 25.0f) * radiusX, centerY + sin(step * 25.0f) * radiusY);
            glVertex2f(centerX + cos(step * 26.0f) * radiusX, centerY + sin(step * 26.0f) * radiusY);
            glVertex2f(centerX + cos(step * 27.0f) * radiusX, centerY + sin(step * 27.0f) * radiusY);
            glVertex2f(centerX + cos(step * 28.0f) * radiusX, centerY + sin(step * 28.0f) * radiusY);
            glVertex2f(centerX + cos(step * 29.0f) * radiusX, centerY + sin(step * 29.0f) * radiusY);
            glVertex2f(centerX + cos(step * 30.0f) * radiusX, centerY + sin(step * 30.0f) * radiusY);
            glVertex2f(centerX + cos(step * 31.0f) * radiusX, centerY + sin(step * 31.0f) * radiusY);
            glVertex2f(centerX + cos(step * 32.0f) * radiusX, centerY + sin(step * 32.0f) * radiusY);
            glVertex2f(centerX + cos(step * 33.0f) * radiusX, centerY + sin(step * 33.0f) * radiusY);
            glVertex2f(centerX + cos(step * 34.0f) * radiusX, centerY + sin(step * 34.0f) * radiusY);
            glVertex2f(centerX + cos(step * 35.0f) * radiusX, centerY + sin(step * 35.0f) * radiusY);
            glEnd();
        }
        glColor3f(0.15f, 0.15f, 0.17f);
        {
            float centerX = 0.0f;
            float centerY = 0.0f;
            float radiusX = 12.0f;
            float radiusY = 12.0f;
            float step = 2.0f * PI_VALUE / 30.0f;
            glBegin(GL_POLYGON);
            glVertex2f(centerX + cos(step * 0.0f) * radiusX, centerY + sin(step * 0.0f) * radiusY);
            glVertex2f(centerX + cos(step * 1.0f) * radiusX, centerY + sin(step * 1.0f) * radiusY);
            glVertex2f(centerX + cos(step * 2.0f) * radiusX, centerY + sin(step * 2.0f) * radiusY);
            glVertex2f(centerX + cos(step * 3.0f) * radiusX, centerY + sin(step * 3.0f) * radiusY);
            glVertex2f(centerX + cos(step * 4.0f) * radiusX, centerY + sin(step * 4.0f) * radiusY);
            glVertex2f(centerX + cos(step * 5.0f) * radiusX, centerY + sin(step * 5.0f) * radiusY);
            glVertex2f(centerX + cos(step * 6.0f) * radiusX, centerY + sin(step * 6.0f) * radiusY);
            glVertex2f(centerX + cos(step * 7.0f) * radiusX, centerY + sin(step * 7.0f) * radiusY);
            glVertex2f(centerX + cos(step * 8.0f) * radiusX, centerY + sin(step * 8.0f) * radiusY);
            glVertex2f(centerX + cos(step * 9.0f) * radiusX, centerY + sin(step * 9.0f) * radiusY);
            glVertex2f(centerX + cos(step * 10.0f) * radiusX, centerY + sin(step * 10.0f) * radiusY);
            glVertex2f(centerX + cos(step * 11.0f) * radiusX, centerY + sin(step * 11.0f) * radiusY);
            glVertex2f(centerX + cos(step * 12.0f) * radiusX, centerY + sin(step * 12.0f) * radiusY);
            glVertex2f(centerX + cos(step * 13.0f) * radiusX, centerY + sin(step * 13.0f) * radiusY);
            glVertex2f(centerX + cos(step * 14.0f) * radiusX, centerY + sin(step * 14.0f) * radiusY);
            glVertex2f(centerX + cos(step * 15.0f) * radiusX, centerY + sin(step * 15.0f) * radiusY);
            glVertex2f(centerX + cos(step * 16.0f) * radiusX, centerY + sin(step * 16.0f) * radiusY);
            glVertex2f(centerX + cos(step * 17.0f) * radiusX, centerY + sin(step * 17.0f) * radiusY);
            glVertex2f(centerX + cos(step * 18.0f) * radiusX, centerY + sin(step * 18.0f) * radiusY);
            glVertex2f(centerX + cos(step * 19.0f) * radiusX, centerY + sin(step * 19.0f) * radiusY);
            glVertex2f(centerX + cos(step * 20.0f) * radiusX, centerY + sin(step * 20.0f) * radiusY);
            glVertex2f(centerX + cos(step * 21.0f) * radiusX, centerY + sin(step * 21.0f) * radiusY);
            glVertex2f(centerX + cos(step * 22.0f) * radiusX, centerY + sin(step * 22.0f) * radiusY);
            glVertex2f(centerX + cos(step * 23.0f) * radiusX, centerY + sin(step * 23.0f) * radiusY);
            glVertex2f(centerX + cos(step * 24.0f) * radiusX, centerY + sin(step * 24.0f) * radiusY);
            glVertex2f(centerX + cos(step * 25.0f) * radiusX, centerY + sin(step * 25.0f) * radiusY);
            glVertex2f(centerX + cos(step * 26.0f) * radiusX, centerY + sin(step * 26.0f) * radiusY);
            glVertex2f(centerX + cos(step * 27.0f) * radiusX, centerY + sin(step * 27.0f) * radiusY);
            glVertex2f(centerX + cos(step * 28.0f) * radiusX, centerY + sin(step * 28.0f) * radiusY);
            glVertex2f(centerX + cos(step * 29.0f) * radiusX, centerY + sin(step * 29.0f) * radiusY);
            glEnd();
        }
        glColor3f(0.74f, 0.74f, 0.78f);
        {
            float centerX = 0.0f;
            float centerY = 0.0f;
            float radiusX = 7.0f;
            float radiusY = 7.0f;
            float step = 2.0f * PI_VALUE / 28.0f;
            glBegin(GL_POLYGON);
            glVertex2f(centerX + cos(step * 0.0f) * radiusX, centerY + sin(step * 0.0f) * radiusY);
            glVertex2f(centerX + cos(step * 1.0f) * radiusX, centerY + sin(step * 1.0f) * radiusY);
            glVertex2f(centerX + cos(step * 2.0f) * radiusX, centerY + sin(step * 2.0f) * radiusY);
            glVertex2f(centerX + cos(step * 3.0f) * radiusX, centerY + sin(step * 3.0f) * radiusY);
            glVertex2f(centerX + cos(step * 4.0f) * radiusX, centerY + sin(step * 4.0f) * radiusY);
            glVertex2f(centerX + cos(step * 5.0f) * radiusX, centerY + sin(step * 5.0f) * radiusY);
            glVertex2f(centerX + cos(step * 6.0f) * radiusX, centerY + sin(step * 6.0f) * radiusY);
            glVertex2f(centerX + cos(step * 7.0f) * radiusX, centerY + sin(step * 7.0f) * radiusY);
            glVertex2f(centerX + cos(step * 8.0f) * radiusX, centerY + sin(step * 8.0f) * radiusY);
            glVertex2f(centerX + cos(step * 9.0f) * radiusX, centerY + sin(step * 9.0f) * radiusY);
            glVertex2f(centerX + cos(step * 10.0f) * radiusX, centerY + sin(step * 10.0f) * radiusY);
            glVertex2f(centerX + cos(step * 11.0f) * radiusX, centerY + sin(step * 11.0f) * radiusY);
            glVertex2f(centerX + cos(step * 12.0f) * radiusX, centerY + sin(step * 12.0f) * radiusY);
            glVertex2f(centerX + cos(step * 13.0f) * radiusX, centerY + sin(step * 13.0f) * radiusY);
            glVertex2f(centerX + cos(step * 14.0f) * radiusX, centerY + sin(step * 14.0f) * radiusY);
            glVertex2f(centerX + cos(step * 15.0f) * radiusX, centerY + sin(step * 15.0f) * radiusY);
            glVertex2f(centerX + cos(step * 16.0f) * radiusX, centerY + sin(step * 16.0f) * radiusY);
            glVertex2f(centerX + cos(step * 17.0f) * radiusX, centerY + sin(step * 17.0f) * radiusY);
            glVertex2f(centerX + cos(step * 18.0f) * radiusX, centerY + sin(step * 18.0f) * radiusY);
            glVertex2f(centerX + cos(step * 19.0f) * radiusX, centerY + sin(step * 19.0f) * radiusY);
            glVertex2f(centerX + cos(step * 20.0f) * radiusX, centerY + sin(step * 20.0f) * radiusY);
            glVertex2f(centerX + cos(step * 21.0f) * radiusX, centerY + sin(step * 21.0f) * radiusY);
            glVertex2f(centerX + cos(step * 22.0f) * radiusX, centerY + sin(step * 22.0f) * radiusY);
            glVertex2f(centerX + cos(step * 23.0f) * radiusX, centerY + sin(step * 23.0f) * radiusY);
            glVertex2f(centerX + cos(step * 24.0f) * radiusX, centerY + sin(step * 24.0f) * radiusY);
            glVertex2f(centerX + cos(step * 25.0f) * radiusX, centerY + sin(step * 25.0f) * radiusY);
            glVertex2f(centerX + cos(step * 26.0f) * radiusX, centerY + sin(step * 26.0f) * radiusY);
            glVertex2f(centerX + cos(step * 27.0f) * radiusX, centerY + sin(step * 27.0f) * radiusY);
            glEnd();
        }
        glColor3f(0.95f, 0.95f, 0.96f);
        glLineWidth(2.0f);
        glBegin(GL_LINES);
        glVertex2f(-8.0f, 0.0f);
        glVertex2f(8.0f, 0.0f);
        glVertex2f(0.0f, -8.0f);
        glVertex2f(0.0f, 8.0f);
        glVertex2f(-5.5f, -5.5f);
        glVertex2f(5.5f, 5.5f);
        glVertex2f(-5.5f, 5.5f);
        glVertex2f(5.5f, -5.5f);
        glEnd();
        glLineWidth(1.0f);
        glPopMatrix();

        if (headlightOn) {
            glColor4f(1.0f, 0.96f, 0.42f, 0.30f);
            glBegin(GL_TRIANGLES);
            glVertex2f(70.0f, 20.0f);
            glVertex2f(186.0f, 58.0f);
            glVertex2f(186.0f, -10.0f);
            glEnd();
            glColor3f(1.0f, 1.0f, 0.72f);
            {
                float left = 66.0f;
                float bottom = 14.0f;
                float right = left + 8.0f;
                float top = bottom + 8.0f;
                glBegin(GL_QUADS);
                glVertex2f(left, bottom);
                glVertex2f(right, bottom);
                glVertex2f(right, top);
                glVertex2f(left, top);
                glEnd();
            }
        }
        if (tailLightOn) {
            glColor3f(0.95f, 0.08f, 0.08f);
            {
                float left = -74.0f;
                float bottom = 16.0f;
                float right = left + 8.0f;
                float top = bottom + 8.0f;
                glBegin(GL_QUADS);
                glVertex2f(left, bottom);
                glVertex2f(right, bottom);
                glVertex2f(right, top);
                glVertex2f(left, top);
                glEnd();
            }
            {
                float left = -74.0f;
                float bottom = 6.0f;
                float right = left + 8.0f;
                float top = bottom + 8.0f;
                glBegin(GL_QUADS);
                glVertex2f(left, bottom);
                glVertex2f(right, bottom);
                glVertex2f(right, top);
                glVertex2f(left, top);
                glEnd();
            }
        }
        glDisable(GL_BLEND);
        glPopMatrix();
    }
    {
        float busX = trafficCarPositionX_scene2_B;
        float busY = 224.0f + bobB;
        float busR = 0.92f;
        float busG = 0.68f;
        float busB = 0.18f;
        float wheelAngle = trafficCarWheelAngle_scene2_B;
        bool facingRight = false;

        glPushMatrix();
        glTranslatef(busX, busY, 0.0f);
        if (!facingRight) glScalef(-1.0f, 1.0f, 1.0f);
        glEnable(GL_BLEND);
        glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
        glColor4f(0.0f, 0.0f, 0.0f, 0.22f);
        {
            float centerX = 0.0f;
            float centerY = -12.0f;
            float radiusX = 112.0f;
            float radiusY = 13.0f;
            float step = 2.0f * PI_VALUE / 36.0f;
            glBegin(GL_POLYGON);
            glVertex2f(centerX + cos(step * 0.0f) * radiusX, centerY + sin(step * 0.0f) * radiusY);
            glVertex2f(centerX + cos(step * 1.0f) * radiusX, centerY + sin(step * 1.0f) * radiusY);
            glVertex2f(centerX + cos(step * 2.0f) * radiusX, centerY + sin(step * 2.0f) * radiusY);
            glVertex2f(centerX + cos(step * 3.0f) * radiusX, centerY + sin(step * 3.0f) * radiusY);
            glVertex2f(centerX + cos(step * 4.0f) * radiusX, centerY + sin(step * 4.0f) * radiusY);
            glVertex2f(centerX + cos(step * 5.0f) * radiusX, centerY + sin(step * 5.0f) * radiusY);
            glVertex2f(centerX + cos(step * 6.0f) * radiusX, centerY + sin(step * 6.0f) * radiusY);
            glVertex2f(centerX + cos(step * 7.0f) * radiusX, centerY + sin(step * 7.0f) * radiusY);
            glVertex2f(centerX + cos(step * 8.0f) * radiusX, centerY + sin(step * 8.0f) * radiusY);
            glVertex2f(centerX + cos(step * 9.0f) * radiusX, centerY + sin(step * 9.0f) * radiusY);
            glVertex2f(centerX + cos(step * 10.0f) * radiusX, centerY + sin(step * 10.0f) * radiusY);
            glVertex2f(centerX + cos(step * 11.0f) * radiusX, centerY + sin(step * 11.0f) * radiusY);
            glVertex2f(centerX + cos(step * 12.0f) * radiusX, centerY + sin(step * 12.0f) * radiusY);
            glVertex2f(centerX + cos(step * 13.0f) * radiusX, centerY + sin(step * 13.0f) * radiusY);
            glVertex2f(centerX + cos(step * 14.0f) * radiusX, centerY + sin(step * 14.0f) * radiusY);
            glVertex2f(centerX + cos(step * 15.0f) * radiusX, centerY + sin(step * 15.0f) * radiusY);
            glVertex2f(centerX + cos(step * 16.0f) * radiusX, centerY + sin(step * 16.0f) * radiusY);
            glVertex2f(centerX + cos(step * 17.0f) * radiusX, centerY + sin(step * 17.0f) * radiusY);
            glVertex2f(centerX + cos(step * 18.0f) * radiusX, centerY + sin(step * 18.0f) * radiusY);
            glVertex2f(centerX + cos(step * 19.0f) * radiusX, centerY + sin(step * 19.0f) * radiusY);
            glVertex2f(centerX + cos(step * 20.0f) * radiusX, centerY + sin(step * 20.0f) * radiusY);
            glVertex2f(centerX + cos(step * 21.0f) * radiusX, centerY + sin(step * 21.0f) * radiusY);
            glVertex2f(centerX + cos(step * 22.0f) * radiusX, centerY + sin(step * 22.0f) * radiusY);
            glVertex2f(centerX + cos(step * 23.0f) * radiusX, centerY + sin(step * 23.0f) * radiusY);
            glVertex2f(centerX + cos(step * 24.0f) * radiusX, centerY + sin(step * 24.0f) * radiusY);
            glVertex2f(centerX + cos(step * 25.0f) * radiusX, centerY + sin(step * 25.0f) * radiusY);
            glVertex2f(centerX + cos(step * 26.0f) * radiusX, centerY + sin(step * 26.0f) * radiusY);
            glVertex2f(centerX + cos(step * 27.0f) * radiusX, centerY + sin(step * 27.0f) * radiusY);
            glVertex2f(centerX + cos(step * 28.0f) * radiusX, centerY + sin(step * 28.0f) * radiusY);
            glVertex2f(centerX + cos(step * 29.0f) * radiusX, centerY + sin(step * 29.0f) * radiusY);
            glVertex2f(centerX + cos(step * 30.0f) * radiusX, centerY + sin(step * 30.0f) * radiusY);
            glVertex2f(centerX + cos(step * 31.0f) * radiusX, centerY + sin(step * 31.0f) * radiusY);
            glVertex2f(centerX + cos(step * 32.0f) * radiusX, centerY + sin(step * 32.0f) * radiusY);
            glVertex2f(centerX + cos(step * 33.0f) * radiusX, centerY + sin(step * 33.0f) * radiusY);
            glVertex2f(centerX + cos(step * 34.0f) * radiusX, centerY + sin(step * 34.0f) * radiusY);
            glVertex2f(centerX + cos(step * 35.0f) * radiusX, centerY + sin(step * 35.0f) * radiusY);
            glEnd();
        }
        glColor3f(busR, busG, busB);
        {
            float left = -110.0f;
            float bottom = 0.0f;
            float right = left + 220.0f;
            float top = bottom + 64.0f;
            glBegin(GL_QUADS);
            glVertex2f(left, bottom);
            glVertex2f(right, bottom);
            glVertex2f(right, top);
            glVertex2f(left, top);
            glEnd();
        }
        glColor3f(0.78f, 0.90f, 0.96f);
        {
            float left = -92.0f;
            float bottom = 34.0f;
            float right = left + 26.0f;
            float top = bottom + 20.0f;
            glBegin(GL_QUADS);
            glVertex2f(left, bottom); glVertex2f(right, bottom); glVertex2f(right, top); glVertex2f(left, top);
            glVertex2f(left + 34.0f, bottom); glVertex2f(right + 34.0f, bottom); glVertex2f(right + 34.0f, top); glVertex2f(left + 34.0f, top);
            glVertex2f(left + 68.0f, bottom); glVertex2f(right + 68.0f, bottom); glVertex2f(right + 68.0f, top); glVertex2f(left + 68.0f, top);
            glVertex2f(left + 102.0f, bottom); glVertex2f(right + 102.0f, bottom); glVertex2f(right + 102.0f, top); glVertex2f(left + 102.0f, top);
            glVertex2f(left + 136.0f, bottom); glVertex2f(right + 136.0f, bottom); glVertex2f(right + 136.0f, top); glVertex2f(left + 136.0f, top);
            glEnd();
        }
        glColor3f(0.20f, 0.20f, 0.22f);
        {
            float left = 78.0f;
            float bottom = 8.0f;
            float right = left + 22.0f;
            float top = bottom + 46.0f;
            glBegin(GL_QUADS);
            glVertex2f(left, bottom);
            glVertex2f(right, bottom);
            glVertex2f(right, top);
            glVertex2f(left, top);
            glEnd();
        }
        glColor3f(0.95f, 0.72f, 0.18f);
        {
            float left = -110.0f;
            float bottom = 18.0f;
            float right = left + 220.0f;
            float top = bottom + 7.0f;
            glBegin(GL_QUADS);
            glVertex2f(left, bottom);
            glVertex2f(right, bottom);
            glVertex2f(right, top);
            glVertex2f(left, top);
            glEnd();
        }

        glPushMatrix();
        glTranslatef(-70.0f, -2.0f, 0.0f);
        glRotatef(wheelAngle, 0.0f, 0.0f, 1.0f);
        glColor3f(0.05f, 0.05f, 0.06f);
        {
            float centerX = 0.0f;
            float centerY = 0.0f;
            float radiusX = 16.0f;
            float radiusY = 16.0f;
            float step = 2.0f * PI_VALUE / 36.0f;
            glBegin(GL_POLYGON);
            glVertex2f(centerX + cos(step * 0.0f) * radiusX, centerY + sin(step * 0.0f) * radiusY);
            glVertex2f(centerX + cos(step * 1.0f) * radiusX, centerY + sin(step * 1.0f) * radiusY);
            glVertex2f(centerX + cos(step * 2.0f) * radiusX, centerY + sin(step * 2.0f) * radiusY);
            glVertex2f(centerX + cos(step * 3.0f) * radiusX, centerY + sin(step * 3.0f) * radiusY);
            glVertex2f(centerX + cos(step * 4.0f) * radiusX, centerY + sin(step * 4.0f) * radiusY);
            glVertex2f(centerX + cos(step * 5.0f) * radiusX, centerY + sin(step * 5.0f) * radiusY);
            glVertex2f(centerX + cos(step * 6.0f) * radiusX, centerY + sin(step * 6.0f) * radiusY);
            glVertex2f(centerX + cos(step * 7.0f) * radiusX, centerY + sin(step * 7.0f) * radiusY);
            glVertex2f(centerX + cos(step * 8.0f) * radiusX, centerY + sin(step * 8.0f) * radiusY);
            glVertex2f(centerX + cos(step * 9.0f) * radiusX, centerY + sin(step * 9.0f) * radiusY);
            glVertex2f(centerX + cos(step * 10.0f) * radiusX, centerY + sin(step * 10.0f) * radiusY);
            glVertex2f(centerX + cos(step * 11.0f) * radiusX, centerY + sin(step * 11.0f) * radiusY);
            glVertex2f(centerX + cos(step * 12.0f) * radiusX, centerY + sin(step * 12.0f) * radiusY);
            glVertex2f(centerX + cos(step * 13.0f) * radiusX, centerY + sin(step * 13.0f) * radiusY);
            glVertex2f(centerX + cos(step * 14.0f) * radiusX, centerY + sin(step * 14.0f) * radiusY);
            glVertex2f(centerX + cos(step * 15.0f) * radiusX, centerY + sin(step * 15.0f) * radiusY);
            glVertex2f(centerX + cos(step * 16.0f) * radiusX, centerY + sin(step * 16.0f) * radiusY);
            glVertex2f(centerX + cos(step * 17.0f) * radiusX, centerY + sin(step * 17.0f) * radiusY);
            glVertex2f(centerX + cos(step * 18.0f) * radiusX, centerY + sin(step * 18.0f) * radiusY);
            glVertex2f(centerX + cos(step * 19.0f) * radiusX, centerY + sin(step * 19.0f) * radiusY);
            glVertex2f(centerX + cos(step * 20.0f) * radiusX, centerY + sin(step * 20.0f) * radiusY);
            glVertex2f(centerX + cos(step * 21.0f) * radiusX, centerY + sin(step * 21.0f) * radiusY);
            glVertex2f(centerX + cos(step * 22.0f) * radiusX, centerY + sin(step * 22.0f) * radiusY);
            glVertex2f(centerX + cos(step * 23.0f) * radiusX, centerY + sin(step * 23.0f) * radiusY);
            glVertex2f(centerX + cos(step * 24.0f) * radiusX, centerY + sin(step * 24.0f) * radiusY);
            glVertex2f(centerX + cos(step * 25.0f) * radiusX, centerY + sin(step * 25.0f) * radiusY);
            glVertex2f(centerX + cos(step * 26.0f) * radiusX, centerY + sin(step * 26.0f) * radiusY);
            glVertex2f(centerX + cos(step * 27.0f) * radiusX, centerY + sin(step * 27.0f) * radiusY);
            glVertex2f(centerX + cos(step * 28.0f) * radiusX, centerY + sin(step * 28.0f) * radiusY);
            glVertex2f(centerX + cos(step * 29.0f) * radiusX, centerY + sin(step * 29.0f) * radiusY);
            glVertex2f(centerX + cos(step * 30.0f) * radiusX, centerY + sin(step * 30.0f) * radiusY);
            glVertex2f(centerX + cos(step * 31.0f) * radiusX, centerY + sin(step * 31.0f) * radiusY);
            glVertex2f(centerX + cos(step * 32.0f) * radiusX, centerY + sin(step * 32.0f) * radiusY);
            glVertex2f(centerX + cos(step * 33.0f) * radiusX, centerY + sin(step * 33.0f) * radiusY);
            glVertex2f(centerX + cos(step * 34.0f) * radiusX, centerY + sin(step * 34.0f) * radiusY);
            glVertex2f(centerX + cos(step * 35.0f) * radiusX, centerY + sin(step * 35.0f) * radiusY);
            glEnd();
        }
        glColor3f(0.15f, 0.15f, 0.17f);
        {
            float centerX = 0.0f;
            float centerY = 0.0f;
            float radiusX = 12.0f;
            float radiusY = 12.0f;
            float step = 2.0f * PI_VALUE / 30.0f;
            glBegin(GL_POLYGON);
            glVertex2f(centerX + cos(step * 0.0f) * radiusX, centerY + sin(step * 0.0f) * radiusY);
            glVertex2f(centerX + cos(step * 1.0f) * radiusX, centerY + sin(step * 1.0f) * radiusY);
            glVertex2f(centerX + cos(step * 2.0f) * radiusX, centerY + sin(step * 2.0f) * radiusY);
            glVertex2f(centerX + cos(step * 3.0f) * radiusX, centerY + sin(step * 3.0f) * radiusY);
            glVertex2f(centerX + cos(step * 4.0f) * radiusX, centerY + sin(step * 4.0f) * radiusY);
            glVertex2f(centerX + cos(step * 5.0f) * radiusX, centerY + sin(step * 5.0f) * radiusY);
            glVertex2f(centerX + cos(step * 6.0f) * radiusX, centerY + sin(step * 6.0f) * radiusY);
            glVertex2f(centerX + cos(step * 7.0f) * radiusX, centerY + sin(step * 7.0f) * radiusY);
            glVertex2f(centerX + cos(step * 8.0f) * radiusX, centerY + sin(step * 8.0f) * radiusY);
            glVertex2f(centerX + cos(step * 9.0f) * radiusX, centerY + sin(step * 9.0f) * radiusY);
            glVertex2f(centerX + cos(step * 10.0f) * radiusX, centerY + sin(step * 10.0f) * radiusY);
            glVertex2f(centerX + cos(step * 11.0f) * radiusX, centerY + sin(step * 11.0f) * radiusY);
            glVertex2f(centerX + cos(step * 12.0f) * radiusX, centerY + sin(step * 12.0f) * radiusY);
            glVertex2f(centerX + cos(step * 13.0f) * radiusX, centerY + sin(step * 13.0f) * radiusY);
            glVertex2f(centerX + cos(step * 14.0f) * radiusX, centerY + sin(step * 14.0f) * radiusY);
            glVertex2f(centerX + cos(step * 15.0f) * radiusX, centerY + sin(step * 15.0f) * radiusY);
            glVertex2f(centerX + cos(step * 16.0f) * radiusX, centerY + sin(step * 16.0f) * radiusY);
            glVertex2f(centerX + cos(step * 17.0f) * radiusX, centerY + sin(step * 17.0f) * radiusY);
            glVertex2f(centerX + cos(step * 18.0f) * radiusX, centerY + sin(step * 18.0f) * radiusY);
            glVertex2f(centerX + cos(step * 19.0f) * radiusX, centerY + sin(step * 19.0f) * radiusY);
            glVertex2f(centerX + cos(step * 20.0f) * radiusX, centerY + sin(step * 20.0f) * radiusY);
            glVertex2f(centerX + cos(step * 21.0f) * radiusX, centerY + sin(step * 21.0f) * radiusY);
            glVertex2f(centerX + cos(step * 22.0f) * radiusX, centerY + sin(step * 22.0f) * radiusY);
            glVertex2f(centerX + cos(step * 23.0f) * radiusX, centerY + sin(step * 23.0f) * radiusY);
            glVertex2f(centerX + cos(step * 24.0f) * radiusX, centerY + sin(step * 24.0f) * radiusY);
            glVertex2f(centerX + cos(step * 25.0f) * radiusX, centerY + sin(step * 25.0f) * radiusY);
            glVertex2f(centerX + cos(step * 26.0f) * radiusX, centerY + sin(step * 26.0f) * radiusY);
            glVertex2f(centerX + cos(step * 27.0f) * radiusX, centerY + sin(step * 27.0f) * radiusY);
            glVertex2f(centerX + cos(step * 28.0f) * radiusX, centerY + sin(step * 28.0f) * radiusY);
            glVertex2f(centerX + cos(step * 29.0f) * radiusX, centerY + sin(step * 29.0f) * radiusY);
            glEnd();
        }
        glColor3f(0.74f, 0.74f, 0.78f);
        {
            float centerX = 0.0f;
            float centerY = 0.0f;
            float radiusX = 7.0f;
            float radiusY = 7.0f;
            float step = 2.0f * PI_VALUE / 28.0f;
            glBegin(GL_POLYGON);
            glVertex2f(centerX + cos(step * 0.0f) * radiusX, centerY + sin(step * 0.0f) * radiusY);
            glVertex2f(centerX + cos(step * 1.0f) * radiusX, centerY + sin(step * 1.0f) * radiusY);
            glVertex2f(centerX + cos(step * 2.0f) * radiusX, centerY + sin(step * 2.0f) * radiusY);
            glVertex2f(centerX + cos(step * 3.0f) * radiusX, centerY + sin(step * 3.0f) * radiusY);
            glVertex2f(centerX + cos(step * 4.0f) * radiusX, centerY + sin(step * 4.0f) * radiusY);
            glVertex2f(centerX + cos(step * 5.0f) * radiusX, centerY + sin(step * 5.0f) * radiusY);
            glVertex2f(centerX + cos(step * 6.0f) * radiusX, centerY + sin(step * 6.0f) * radiusY);
            glVertex2f(centerX + cos(step * 7.0f) * radiusX, centerY + sin(step * 7.0f) * radiusY);
            glVertex2f(centerX + cos(step * 8.0f) * radiusX, centerY + sin(step * 8.0f) * radiusY);
            glVertex2f(centerX + cos(step * 9.0f) * radiusX, centerY + sin(step * 9.0f) * radiusY);
            glVertex2f(centerX + cos(step * 10.0f) * radiusX, centerY + sin(step * 10.0f) * radiusY);
            glVertex2f(centerX + cos(step * 11.0f) * radiusX, centerY + sin(step * 11.0f) * radiusY);
            glVertex2f(centerX + cos(step * 12.0f) * radiusX, centerY + sin(step * 12.0f) * radiusY);
            glVertex2f(centerX + cos(step * 13.0f) * radiusX, centerY + sin(step * 13.0f) * radiusY);
            glVertex2f(centerX + cos(step * 14.0f) * radiusX, centerY + sin(step * 14.0f) * radiusY);
            glVertex2f(centerX + cos(step * 15.0f) * radiusX, centerY + sin(step * 15.0f) * radiusY);
            glVertex2f(centerX + cos(step * 16.0f) * radiusX, centerY + sin(step * 16.0f) * radiusY);
            glVertex2f(centerX + cos(step * 17.0f) * radiusX, centerY + sin(step * 17.0f) * radiusY);
            glVertex2f(centerX + cos(step * 18.0f) * radiusX, centerY + sin(step * 18.0f) * radiusY);
            glVertex2f(centerX + cos(step * 19.0f) * radiusX, centerY + sin(step * 19.0f) * radiusY);
            glVertex2f(centerX + cos(step * 20.0f) * radiusX, centerY + sin(step * 20.0f) * radiusY);
            glVertex2f(centerX + cos(step * 21.0f) * radiusX, centerY + sin(step * 21.0f) * radiusY);
            glVertex2f(centerX + cos(step * 22.0f) * radiusX, centerY + sin(step * 22.0f) * radiusY);
            glVertex2f(centerX + cos(step * 23.0f) * radiusX, centerY + sin(step * 23.0f) * radiusY);
            glVertex2f(centerX + cos(step * 24.0f) * radiusX, centerY + sin(step * 24.0f) * radiusY);
            glVertex2f(centerX + cos(step * 25.0f) * radiusX, centerY + sin(step * 25.0f) * radiusY);
            glVertex2f(centerX + cos(step * 26.0f) * radiusX, centerY + sin(step * 26.0f) * radiusY);
            glVertex2f(centerX + cos(step * 27.0f) * radiusX, centerY + sin(step * 27.0f) * radiusY);
            glEnd();
        }
        glColor3f(0.95f, 0.95f, 0.96f);
        glLineWidth(2.0f);
        glBegin(GL_LINES);
        glVertex2f(-8.0f, 0.0f);
        glVertex2f(8.0f, 0.0f);
        glVertex2f(0.0f, -8.0f);
        glVertex2f(0.0f, 8.0f);
        glVertex2f(-5.5f, -5.5f);
        glVertex2f(5.5f, 5.5f);
        glVertex2f(-5.5f, 5.5f);
        glVertex2f(5.5f, -5.5f);
        glEnd();
        glLineWidth(1.0f);
        glPopMatrix();

        glPushMatrix();
        glTranslatef(72.0f, -2.0f, 0.0f);
        glRotatef(wheelAngle, 0.0f, 0.0f, 1.0f);
        glColor3f(0.05f, 0.05f, 0.06f);
        {
            float centerX = 0.0f;
            float centerY = 0.0f;
            float radiusX = 16.0f;
            float radiusY = 16.0f;
            float step = 2.0f * PI_VALUE / 36.0f;
            glBegin(GL_POLYGON);
            glVertex2f(centerX + cos(step * 0.0f) * radiusX, centerY + sin(step * 0.0f) * radiusY);
            glVertex2f(centerX + cos(step * 1.0f) * radiusX, centerY + sin(step * 1.0f) * radiusY);
            glVertex2f(centerX + cos(step * 2.0f) * radiusX, centerY + sin(step * 2.0f) * radiusY);
            glVertex2f(centerX + cos(step * 3.0f) * radiusX, centerY + sin(step * 3.0f) * radiusY);
            glVertex2f(centerX + cos(step * 4.0f) * radiusX, centerY + sin(step * 4.0f) * radiusY);
            glVertex2f(centerX + cos(step * 5.0f) * radiusX, centerY + sin(step * 5.0f) * radiusY);
            glVertex2f(centerX + cos(step * 6.0f) * radiusX, centerY + sin(step * 6.0f) * radiusY);
            glVertex2f(centerX + cos(step * 7.0f) * radiusX, centerY + sin(step * 7.0f) * radiusY);
            glVertex2f(centerX + cos(step * 8.0f) * radiusX, centerY + sin(step * 8.0f) * radiusY);
            glVertex2f(centerX + cos(step * 9.0f) * radiusX, centerY + sin(step * 9.0f) * radiusY);
            glVertex2f(centerX + cos(step * 10.0f) * radiusX, centerY + sin(step * 10.0f) * radiusY);
            glVertex2f(centerX + cos(step * 11.0f) * radiusX, centerY + sin(step * 11.0f) * radiusY);
            glVertex2f(centerX + cos(step * 12.0f) * radiusX, centerY + sin(step * 12.0f) * radiusY);
            glVertex2f(centerX + cos(step * 13.0f) * radiusX, centerY + sin(step * 13.0f) * radiusY);
            glVertex2f(centerX + cos(step * 14.0f) * radiusX, centerY + sin(step * 14.0f) * radiusY);
            glVertex2f(centerX + cos(step * 15.0f) * radiusX, centerY + sin(step * 15.0f) * radiusY);
            glVertex2f(centerX + cos(step * 16.0f) * radiusX, centerY + sin(step * 16.0f) * radiusY);
            glVertex2f(centerX + cos(step * 17.0f) * radiusX, centerY + sin(step * 17.0f) * radiusY);
            glVertex2f(centerX + cos(step * 18.0f) * radiusX, centerY + sin(step * 18.0f) * radiusY);
            glVertex2f(centerX + cos(step * 19.0f) * radiusX, centerY + sin(step * 19.0f) * radiusY);
            glVertex2f(centerX + cos(step * 20.0f) * radiusX, centerY + sin(step * 20.0f) * radiusY);
            glVertex2f(centerX + cos(step * 21.0f) * radiusX, centerY + sin(step * 21.0f) * radiusY);
            glVertex2f(centerX + cos(step * 22.0f) * radiusX, centerY + sin(step * 22.0f) * radiusY);
            glVertex2f(centerX + cos(step * 23.0f) * radiusX, centerY + sin(step * 23.0f) * radiusY);
            glVertex2f(centerX + cos(step * 24.0f) * radiusX, centerY + sin(step * 24.0f) * radiusY);
            glVertex2f(centerX + cos(step * 25.0f) * radiusX, centerY + sin(step * 25.0f) * radiusY);
            glVertex2f(centerX + cos(step * 26.0f) * radiusX, centerY + sin(step * 26.0f) * radiusY);
            glVertex2f(centerX + cos(step * 27.0f) * radiusX, centerY + sin(step * 27.0f) * radiusY);
            glVertex2f(centerX + cos(step * 28.0f) * radiusX, centerY + sin(step * 28.0f) * radiusY);
            glVertex2f(centerX + cos(step * 29.0f) * radiusX, centerY + sin(step * 29.0f) * radiusY);
            glVertex2f(centerX + cos(step * 30.0f) * radiusX, centerY + sin(step * 30.0f) * radiusY);
            glVertex2f(centerX + cos(step * 31.0f) * radiusX, centerY + sin(step * 31.0f) * radiusY);
            glVertex2f(centerX + cos(step * 32.0f) * radiusX, centerY + sin(step * 32.0f) * radiusY);
            glVertex2f(centerX + cos(step * 33.0f) * radiusX, centerY + sin(step * 33.0f) * radiusY);
            glVertex2f(centerX + cos(step * 34.0f) * radiusX, centerY + sin(step * 34.0f) * radiusY);
            glVertex2f(centerX + cos(step * 35.0f) * radiusX, centerY + sin(step * 35.0f) * radiusY);
            glEnd();
        }
        glColor3f(0.15f, 0.15f, 0.17f);
        {
            float centerX = 0.0f;
            float centerY = 0.0f;
            float radiusX = 12.0f;
            float radiusY = 12.0f;
            float step = 2.0f * PI_VALUE / 30.0f;
            glBegin(GL_POLYGON);
            glVertex2f(centerX + cos(step * 0.0f) * radiusX, centerY + sin(step * 0.0f) * radiusY);
            glVertex2f(centerX + cos(step * 1.0f) * radiusX, centerY + sin(step * 1.0f) * radiusY);
            glVertex2f(centerX + cos(step * 2.0f) * radiusX, centerY + sin(step * 2.0f) * radiusY);
            glVertex2f(centerX + cos(step * 3.0f) * radiusX, centerY + sin(step * 3.0f) * radiusY);
            glVertex2f(centerX + cos(step * 4.0f) * radiusX, centerY + sin(step * 4.0f) * radiusY);
            glVertex2f(centerX + cos(step * 5.0f) * radiusX, centerY + sin(step * 5.0f) * radiusY);
            glVertex2f(centerX + cos(step * 6.0f) * radiusX, centerY + sin(step * 6.0f) * radiusY);
            glVertex2f(centerX + cos(step * 7.0f) * radiusX, centerY + sin(step * 7.0f) * radiusY);
            glVertex2f(centerX + cos(step * 8.0f) * radiusX, centerY + sin(step * 8.0f) * radiusY);
            glVertex2f(centerX + cos(step * 9.0f) * radiusX, centerY + sin(step * 9.0f) * radiusY);
            glVertex2f(centerX + cos(step * 10.0f) * radiusX, centerY + sin(step * 10.0f) * radiusY);
            glVertex2f(centerX + cos(step * 11.0f) * radiusX, centerY + sin(step * 11.0f) * radiusY);
            glVertex2f(centerX + cos(step * 12.0f) * radiusX, centerY + sin(step * 12.0f) * radiusY);
            glVertex2f(centerX + cos(step * 13.0f) * radiusX, centerY + sin(step * 13.0f) * radiusY);
            glVertex2f(centerX + cos(step * 14.0f) * radiusX, centerY + sin(step * 14.0f) * radiusY);
            glVertex2f(centerX + cos(step * 15.0f) * radiusX, centerY + sin(step * 15.0f) * radiusY);
            glVertex2f(centerX + cos(step * 16.0f) * radiusX, centerY + sin(step * 16.0f) * radiusY);
            glVertex2f(centerX + cos(step * 17.0f) * radiusX, centerY + sin(step * 17.0f) * radiusY);
            glVertex2f(centerX + cos(step * 18.0f) * radiusX, centerY + sin(step * 18.0f) * radiusY);
            glVertex2f(centerX + cos(step * 19.0f) * radiusX, centerY + sin(step * 19.0f) * radiusY);
            glVertex2f(centerX + cos(step * 20.0f) * radiusX, centerY + sin(step * 20.0f) * radiusY);
            glVertex2f(centerX + cos(step * 21.0f) * radiusX, centerY + sin(step * 21.0f) * radiusY);
            glVertex2f(centerX + cos(step * 22.0f) * radiusX, centerY + sin(step * 22.0f) * radiusY);
            glVertex2f(centerX + cos(step * 23.0f) * radiusX, centerY + sin(step * 23.0f) * radiusY);
            glVertex2f(centerX + cos(step * 24.0f) * radiusX, centerY + sin(step * 24.0f) * radiusY);
            glVertex2f(centerX + cos(step * 25.0f) * radiusX, centerY + sin(step * 25.0f) * radiusY);
            glVertex2f(centerX + cos(step * 26.0f) * radiusX, centerY + sin(step * 26.0f) * radiusY);
            glVertex2f(centerX + cos(step * 27.0f) * radiusX, centerY + sin(step * 27.0f) * radiusY);
            glVertex2f(centerX + cos(step * 28.0f) * radiusX, centerY + sin(step * 28.0f) * radiusY);
            glVertex2f(centerX + cos(step * 29.0f) * radiusX, centerY + sin(step * 29.0f) * radiusY);
            glEnd();
        }
        glColor3f(0.74f, 0.74f, 0.78f);
        {
            float centerX = 0.0f;
            float centerY = 0.0f;
            float radiusX = 7.0f;
            float radiusY = 7.0f;
            float step = 2.0f * PI_VALUE / 28.0f;
            glBegin(GL_POLYGON);
            glVertex2f(centerX + cos(step * 0.0f) * radiusX, centerY + sin(step * 0.0f) * radiusY);
            glVertex2f(centerX + cos(step * 1.0f) * radiusX, centerY + sin(step * 1.0f) * radiusY);
            glVertex2f(centerX + cos(step * 2.0f) * radiusX, centerY + sin(step * 2.0f) * radiusY);
            glVertex2f(centerX + cos(step * 3.0f) * radiusX, centerY + sin(step * 3.0f) * radiusY);
            glVertex2f(centerX + cos(step * 4.0f) * radiusX, centerY + sin(step * 4.0f) * radiusY);
            glVertex2f(centerX + cos(step * 5.0f) * radiusX, centerY + sin(step * 5.0f) * radiusY);
            glVertex2f(centerX + cos(step * 6.0f) * radiusX, centerY + sin(step * 6.0f) * radiusY);
            glVertex2f(centerX + cos(step * 7.0f) * radiusX, centerY + sin(step * 7.0f) * radiusY);
            glVertex2f(centerX + cos(step * 8.0f) * radiusX, centerY + sin(step * 8.0f) * radiusY);
            glVertex2f(centerX + cos(step * 9.0f) * radiusX, centerY + sin(step * 9.0f) * radiusY);
            glVertex2f(centerX + cos(step * 10.0f) * radiusX, centerY + sin(step * 10.0f) * radiusY);
            glVertex2f(centerX + cos(step * 11.0f) * radiusX, centerY + sin(step * 11.0f) * radiusY);
            glVertex2f(centerX + cos(step * 12.0f) * radiusX, centerY + sin(step * 12.0f) * radiusY);
            glVertex2f(centerX + cos(step * 13.0f) * radiusX, centerY + sin(step * 13.0f) * radiusY);
            glVertex2f(centerX + cos(step * 14.0f) * radiusX, centerY + sin(step * 14.0f) * radiusY);
            glVertex2f(centerX + cos(step * 15.0f) * radiusX, centerY + sin(step * 15.0f) * radiusY);
            glVertex2f(centerX + cos(step * 16.0f) * radiusX, centerY + sin(step * 16.0f) * radiusY);
            glVertex2f(centerX + cos(step * 17.0f) * radiusX, centerY + sin(step * 17.0f) * radiusY);
            glVertex2f(centerX + cos(step * 18.0f) * radiusX, centerY + sin(step * 18.0f) * radiusY);
            glVertex2f(centerX + cos(step * 19.0f) * radiusX, centerY + sin(step * 19.0f) * radiusY);
            glVertex2f(centerX + cos(step * 20.0f) * radiusX, centerY + sin(step * 20.0f) * radiusY);
            glVertex2f(centerX + cos(step * 21.0f) * radiusX, centerY + sin(step * 21.0f) * radiusY);
            glVertex2f(centerX + cos(step * 22.0f) * radiusX, centerY + sin(step * 22.0f) * radiusY);
            glVertex2f(centerX + cos(step * 23.0f) * radiusX, centerY + sin(step * 23.0f) * radiusY);
            glVertex2f(centerX + cos(step * 24.0f) * radiusX, centerY + sin(step * 24.0f) * radiusY);
            glVertex2f(centerX + cos(step * 25.0f) * radiusX, centerY + sin(step * 25.0f) * radiusY);
            glVertex2f(centerX + cos(step * 26.0f) * radiusX, centerY + sin(step * 26.0f) * radiusY);
            glVertex2f(centerX + cos(step * 27.0f) * radiusX, centerY + sin(step * 27.0f) * radiusY);
            glEnd();
        }
        glColor3f(0.95f, 0.95f, 0.96f);
        glLineWidth(2.0f);
        glBegin(GL_LINES);
        glVertex2f(-8.0f, 0.0f);
        glVertex2f(8.0f, 0.0f);
        glVertex2f(0.0f, -8.0f);
        glVertex2f(0.0f, 8.0f);
        glVertex2f(-5.5f, -5.5f);
        glVertex2f(5.5f, 5.5f);
        glVertex2f(-5.5f, 5.5f);
        glVertex2f(5.5f, -5.5f);
        glEnd();
        glLineWidth(1.0f);
        glPopMatrix();

        glDisable(GL_BLEND);
        glPopMatrix();
    }
    {
        float vanX = trafficCarPositionX_scene2_C;
        float vanY = 294.0f + bobC;
        float vanR = 0.88f;
        float vanG = 0.88f;
        float vanB = 0.84f;
        float wheelAngle = trafficCarWheelAngle_scene2_C;
        bool facingRight = true;

        glPushMatrix();
        glTranslatef(vanX, vanY, 0.0f);
        if (!facingRight) glScalef(-1.0f, 1.0f, 1.0f);
        glEnable(GL_BLEND);
        glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
        glColor4f(0.0f, 0.0f, 0.0f, 0.23f);
        {
            float centerX = 0.0f;
            float centerY = -11.0f;
            float radiusX = 92.0f;
            float radiusY = 12.0f;
            float step = 2.0f * PI_VALUE / 36.0f;
            glBegin(GL_POLYGON);
            glVertex2f(centerX + cos(step * 0.0f) * radiusX, centerY + sin(step * 0.0f) * radiusY);
            glVertex2f(centerX + cos(step * 1.0f) * radiusX, centerY + sin(step * 1.0f) * radiusY);
            glVertex2f(centerX + cos(step * 2.0f) * radiusX, centerY + sin(step * 2.0f) * radiusY);
            glVertex2f(centerX + cos(step * 3.0f) * radiusX, centerY + sin(step * 3.0f) * radiusY);
            glVertex2f(centerX + cos(step * 4.0f) * radiusX, centerY + sin(step * 4.0f) * radiusY);
            glVertex2f(centerX + cos(step * 5.0f) * radiusX, centerY + sin(step * 5.0f) * radiusY);
            glVertex2f(centerX + cos(step * 6.0f) * radiusX, centerY + sin(step * 6.0f) * radiusY);
            glVertex2f(centerX + cos(step * 7.0f) * radiusX, centerY + sin(step * 7.0f) * radiusY);
            glVertex2f(centerX + cos(step * 8.0f) * radiusX, centerY + sin(step * 8.0f) * radiusY);
            glVertex2f(centerX + cos(step * 9.0f) * radiusX, centerY + sin(step * 9.0f) * radiusY);
            glVertex2f(centerX + cos(step * 10.0f) * radiusX, centerY + sin(step * 10.0f) * radiusY);
            glVertex2f(centerX + cos(step * 11.0f) * radiusX, centerY + sin(step * 11.0f) * radiusY);
            glVertex2f(centerX + cos(step * 12.0f) * radiusX, centerY + sin(step * 12.0f) * radiusY);
            glVertex2f(centerX + cos(step * 13.0f) * radiusX, centerY + sin(step * 13.0f) * radiusY);
            glVertex2f(centerX + cos(step * 14.0f) * radiusX, centerY + sin(step * 14.0f) * radiusY);
            glVertex2f(centerX + cos(step * 15.0f) * radiusX, centerY + sin(step * 15.0f) * radiusY);
            glVertex2f(centerX + cos(step * 16.0f) * radiusX, centerY + sin(step * 16.0f) * radiusY);
            glVertex2f(centerX + cos(step * 17.0f) * radiusX, centerY + sin(step * 17.0f) * radiusY);
            glVertex2f(centerX + cos(step * 18.0f) * radiusX, centerY + sin(step * 18.0f) * radiusY);
            glVertex2f(centerX + cos(step * 19.0f) * radiusX, centerY + sin(step * 19.0f) * radiusY);
            glVertex2f(centerX + cos(step * 20.0f) * radiusX, centerY + sin(step * 20.0f) * radiusY);
            glVertex2f(centerX + cos(step * 21.0f) * radiusX, centerY + sin(step * 21.0f) * radiusY);
            glVertex2f(centerX + cos(step * 22.0f) * radiusX, centerY + sin(step * 22.0f) * radiusY);
            glVertex2f(centerX + cos(step * 23.0f) * radiusX, centerY + sin(step * 23.0f) * radiusY);
            glVertex2f(centerX + cos(step * 24.0f) * radiusX, centerY + sin(step * 24.0f) * radiusY);
            glVertex2f(centerX + cos(step * 25.0f) * radiusX, centerY + sin(step * 25.0f) * radiusY);
            glVertex2f(centerX + cos(step * 26.0f) * radiusX, centerY + sin(step * 26.0f) * radiusY);
            glVertex2f(centerX + cos(step * 27.0f) * radiusX, centerY + sin(step * 27.0f) * radiusY);
            glVertex2f(centerX + cos(step * 28.0f) * radiusX, centerY + sin(step * 28.0f) * radiusY);
            glVertex2f(centerX + cos(step * 29.0f) * radiusX, centerY + sin(step * 29.0f) * radiusY);
            glVertex2f(centerX + cos(step * 30.0f) * radiusX, centerY + sin(step * 30.0f) * radiusY);
            glVertex2f(centerX + cos(step * 31.0f) * radiusX, centerY + sin(step * 31.0f) * radiusY);
            glVertex2f(centerX + cos(step * 32.0f) * radiusX, centerY + sin(step * 32.0f) * radiusY);
            glVertex2f(centerX + cos(step * 33.0f) * radiusX, centerY + sin(step * 33.0f) * radiusY);
            glVertex2f(centerX + cos(step * 34.0f) * radiusX, centerY + sin(step * 34.0f) * radiusY);
            glVertex2f(centerX + cos(step * 35.0f) * radiusX, centerY + sin(step * 35.0f) * radiusY);
            glEnd();
        }
        glColor3f(vanR, vanG, vanB);
        {
            float left = -88.0f;
            float bottom = 0.0f;
            float right = left + 176.0f;
            float top = bottom + 48.0f;
            glBegin(GL_QUADS);
            glVertex2f(left, bottom);
            glVertex2f(right, bottom);
            glVertex2f(right, top);
            glVertex2f(left, top);
            glEnd();
        }
        glBegin(GL_POLYGON);
        glVertex2f(20.0f, 48.0f);
        glVertex2f(68.0f, 48.0f);
        glVertex2f(86.0f, 28.0f);
        glVertex2f(86.0f, 0.0f);
        glVertex2f(20.0f, 0.0f);
        glEnd();
        glColor3f(0.72f, 0.86f, 0.94f);
        {
            float left = 42.0f;
            float bottom = 29.0f;
            float right = left + 30.0f;
            float top = bottom + 15.0f;
            glBegin(GL_QUADS);
            glVertex2f(left, bottom);
            glVertex2f(right, bottom);
            glVertex2f(right, top);
            glVertex2f(left, top);
            glEnd();
        }
        glColor3f(0.92f, 0.92f, 0.94f);
        {
            float left = -72.0f;
            float bottom = 18.0f;
            float right = left + 66.0f;
            float top = bottom + 18.0f;
            glBegin(GL_QUADS);
            glVertex2f(left, bottom);
            glVertex2f(right, bottom);
            glVertex2f(right, top);
            glVertex2f(left, top);
            glEnd();
        }
        glColor3f(0.16f, 0.16f, 0.18f);
        {
            float left = 80.0f;
            float bottom = 10.0f;
            float right = left + 8.0f;
            float top = bottom + 16.0f;
            glBegin(GL_QUADS);
            glVertex2f(left, bottom);
            glVertex2f(right, bottom);
            glVertex2f(right, top);
            glVertex2f(left, top);
            glEnd();
        }

        glPushMatrix();
        glTranslatef(-52.0f, -2.0f, 0.0f);
        glRotatef(wheelAngle, 0.0f, 0.0f, 1.0f);
        glColor3f(0.05f, 0.05f, 0.06f);
        {
            float centerX = 0.0f;
            float centerY = 0.0f;
            float radiusX = 16.0f;
            float radiusY = 16.0f;
            float step = 2.0f * PI_VALUE / 36.0f;
            glBegin(GL_POLYGON);
            glVertex2f(centerX + cos(step * 0.0f) * radiusX, centerY + sin(step * 0.0f) * radiusY);
            glVertex2f(centerX + cos(step * 1.0f) * radiusX, centerY + sin(step * 1.0f) * radiusY);
            glVertex2f(centerX + cos(step * 2.0f) * radiusX, centerY + sin(step * 2.0f) * radiusY);
            glVertex2f(centerX + cos(step * 3.0f) * radiusX, centerY + sin(step * 3.0f) * radiusY);
            glVertex2f(centerX + cos(step * 4.0f) * radiusX, centerY + sin(step * 4.0f) * radiusY);
            glVertex2f(centerX + cos(step * 5.0f) * radiusX, centerY + sin(step * 5.0f) * radiusY);
            glVertex2f(centerX + cos(step * 6.0f) * radiusX, centerY + sin(step * 6.0f) * radiusY);
            glVertex2f(centerX + cos(step * 7.0f) * radiusX, centerY + sin(step * 7.0f) * radiusY);
            glVertex2f(centerX + cos(step * 8.0f) * radiusX, centerY + sin(step * 8.0f) * radiusY);
            glVertex2f(centerX + cos(step * 9.0f) * radiusX, centerY + sin(step * 9.0f) * radiusY);
            glVertex2f(centerX + cos(step * 10.0f) * radiusX, centerY + sin(step * 10.0f) * radiusY);
            glVertex2f(centerX + cos(step * 11.0f) * radiusX, centerY + sin(step * 11.0f) * radiusY);
            glVertex2f(centerX + cos(step * 12.0f) * radiusX, centerY + sin(step * 12.0f) * radiusY);
            glVertex2f(centerX + cos(step * 13.0f) * radiusX, centerY + sin(step * 13.0f) * radiusY);
            glVertex2f(centerX + cos(step * 14.0f) * radiusX, centerY + sin(step * 14.0f) * radiusY);
            glVertex2f(centerX + cos(step * 15.0f) * radiusX, centerY + sin(step * 15.0f) * radiusY);
            glVertex2f(centerX + cos(step * 16.0f) * radiusX, centerY + sin(step * 16.0f) * radiusY);
            glVertex2f(centerX + cos(step * 17.0f) * radiusX, centerY + sin(step * 17.0f) * radiusY);
            glVertex2f(centerX + cos(step * 18.0f) * radiusX, centerY + sin(step * 18.0f) * radiusY);
            glVertex2f(centerX + cos(step * 19.0f) * radiusX, centerY + sin(step * 19.0f) * radiusY);
            glVertex2f(centerX + cos(step * 20.0f) * radiusX, centerY + sin(step * 20.0f) * radiusY);
            glVertex2f(centerX + cos(step * 21.0f) * radiusX, centerY + sin(step * 21.0f) * radiusY);
            glVertex2f(centerX + cos(step * 22.0f) * radiusX, centerY + sin(step * 22.0f) * radiusY);
            glVertex2f(centerX + cos(step * 23.0f) * radiusX, centerY + sin(step * 23.0f) * radiusY);
            glVertex2f(centerX + cos(step * 24.0f) * radiusX, centerY + sin(step * 24.0f) * radiusY);
            glVertex2f(centerX + cos(step * 25.0f) * radiusX, centerY + sin(step * 25.0f) * radiusY);
            glVertex2f(centerX + cos(step * 26.0f) * radiusX, centerY + sin(step * 26.0f) * radiusY);
            glVertex2f(centerX + cos(step * 27.0f) * radiusX, centerY + sin(step * 27.0f) * radiusY);
            glVertex2f(centerX + cos(step * 28.0f) * radiusX, centerY + sin(step * 28.0f) * radiusY);
            glVertex2f(centerX + cos(step * 29.0f) * radiusX, centerY + sin(step * 29.0f) * radiusY);
            glVertex2f(centerX + cos(step * 30.0f) * radiusX, centerY + sin(step * 30.0f) * radiusY);
            glVertex2f(centerX + cos(step * 31.0f) * radiusX, centerY + sin(step * 31.0f) * radiusY);
            glVertex2f(centerX + cos(step * 32.0f) * radiusX, centerY + sin(step * 32.0f) * radiusY);
            glVertex2f(centerX + cos(step * 33.0f) * radiusX, centerY + sin(step * 33.0f) * radiusY);
            glVertex2f(centerX + cos(step * 34.0f) * radiusX, centerY + sin(step * 34.0f) * radiusY);
            glVertex2f(centerX + cos(step * 35.0f) * radiusX, centerY + sin(step * 35.0f) * radiusY);
            glEnd();
        }
        glColor3f(0.15f, 0.15f, 0.17f);
        {
            float centerX = 0.0f;
            float centerY = 0.0f;
            float radiusX = 12.0f;
            float radiusY = 12.0f;
            float step = 2.0f * PI_VALUE / 30.0f;
            glBegin(GL_POLYGON);
            glVertex2f(centerX + cos(step * 0.0f) * radiusX, centerY + sin(step * 0.0f) * radiusY);
            glVertex2f(centerX + cos(step * 1.0f) * radiusX, centerY + sin(step * 1.0f) * radiusY);
            glVertex2f(centerX + cos(step * 2.0f) * radiusX, centerY + sin(step * 2.0f) * radiusY);
            glVertex2f(centerX + cos(step * 3.0f) * radiusX, centerY + sin(step * 3.0f) * radiusY);
            glVertex2f(centerX + cos(step * 4.0f) * radiusX, centerY + sin(step * 4.0f) * radiusY);
            glVertex2f(centerX + cos(step * 5.0f) * radiusX, centerY + sin(step * 5.0f) * radiusY);
            glVertex2f(centerX + cos(step * 6.0f) * radiusX, centerY + sin(step * 6.0f) * radiusY);
            glVertex2f(centerX + cos(step * 7.0f) * radiusX, centerY + sin(step * 7.0f) * radiusY);
            glVertex2f(centerX + cos(step * 8.0f) * radiusX, centerY + sin(step * 8.0f) * radiusY);
            glVertex2f(centerX + cos(step * 9.0f) * radiusX, centerY + sin(step * 9.0f) * radiusY);
            glVertex2f(centerX + cos(step * 10.0f) * radiusX, centerY + sin(step * 10.0f) * radiusY);
            glVertex2f(centerX + cos(step * 11.0f) * radiusX, centerY + sin(step * 11.0f) * radiusY);
            glVertex2f(centerX + cos(step * 12.0f) * radiusX, centerY + sin(step * 12.0f) * radiusY);
            glVertex2f(centerX + cos(step * 13.0f) * radiusX, centerY + sin(step * 13.0f) * radiusY);
            glVertex2f(centerX + cos(step * 14.0f) * radiusX, centerY + sin(step * 14.0f) * radiusY);
            glVertex2f(centerX + cos(step * 15.0f) * radiusX, centerY + sin(step * 15.0f) * radiusY);
            glVertex2f(centerX + cos(step * 16.0f) * radiusX, centerY + sin(step * 16.0f) * radiusY);
            glVertex2f(centerX + cos(step * 17.0f) * radiusX, centerY + sin(step * 17.0f) * radiusY);
            glVertex2f(centerX + cos(step * 18.0f) * radiusX, centerY + sin(step * 18.0f) * radiusY);
            glVertex2f(centerX + cos(step * 19.0f) * radiusX, centerY + sin(step * 19.0f) * radiusY);
            glVertex2f(centerX + cos(step * 20.0f) * radiusX, centerY + sin(step * 20.0f) * radiusY);
            glVertex2f(centerX + cos(step * 21.0f) * radiusX, centerY + sin(step * 21.0f) * radiusY);
            glVertex2f(centerX + cos(step * 22.0f) * radiusX, centerY + sin(step * 22.0f) * radiusY);
            glVertex2f(centerX + cos(step * 23.0f) * radiusX, centerY + sin(step * 23.0f) * radiusY);
            glVertex2f(centerX + cos(step * 24.0f) * radiusX, centerY + sin(step * 24.0f) * radiusY);
            glVertex2f(centerX + cos(step * 25.0f) * radiusX, centerY + sin(step * 25.0f) * radiusY);
            glVertex2f(centerX + cos(step * 26.0f) * radiusX, centerY + sin(step * 26.0f) * radiusY);
            glVertex2f(centerX + cos(step * 27.0f) * radiusX, centerY + sin(step * 27.0f) * radiusY);
            glVertex2f(centerX + cos(step * 28.0f) * radiusX, centerY + sin(step * 28.0f) * radiusY);
            glVertex2f(centerX + cos(step * 29.0f) * radiusX, centerY + sin(step * 29.0f) * radiusY);
            glEnd();
        }
        glColor3f(0.74f, 0.74f, 0.78f);
        {
            float centerX = 0.0f;
            float centerY = 0.0f;
            float radiusX = 7.0f;
            float radiusY = 7.0f;
            float step = 2.0f * PI_VALUE / 28.0f;
            glBegin(GL_POLYGON);
            glVertex2f(centerX + cos(step * 0.0f) * radiusX, centerY + sin(step * 0.0f) * radiusY);
            glVertex2f(centerX + cos(step * 1.0f) * radiusX, centerY + sin(step * 1.0f) * radiusY);
            glVertex2f(centerX + cos(step * 2.0f) * radiusX, centerY + sin(step * 2.0f) * radiusY);
            glVertex2f(centerX + cos(step * 3.0f) * radiusX, centerY + sin(step * 3.0f) * radiusY);
            glVertex2f(centerX + cos(step * 4.0f) * radiusX, centerY + sin(step * 4.0f) * radiusY);
            glVertex2f(centerX + cos(step * 5.0f) * radiusX, centerY + sin(step * 5.0f) * radiusY);
            glVertex2f(centerX + cos(step * 6.0f) * radiusX, centerY + sin(step * 6.0f) * radiusY);
            glVertex2f(centerX + cos(step * 7.0f) * radiusX, centerY + sin(step * 7.0f) * radiusY);
            glVertex2f(centerX + cos(step * 8.0f) * radiusX, centerY + sin(step * 8.0f) * radiusY);
            glVertex2f(centerX + cos(step * 9.0f) * radiusX, centerY + sin(step * 9.0f) * radiusY);
            glVertex2f(centerX + cos(step * 10.0f) * radiusX, centerY + sin(step * 10.0f) * radiusY);
            glVertex2f(centerX + cos(step * 11.0f) * radiusX, centerY + sin(step * 11.0f) * radiusY);
            glVertex2f(centerX + cos(step * 12.0f) * radiusX, centerY + sin(step * 12.0f) * radiusY);
            glVertex2f(centerX + cos(step * 13.0f) * radiusX, centerY + sin(step * 13.0f) * radiusY);
            glVertex2f(centerX + cos(step * 14.0f) * radiusX, centerY + sin(step * 14.0f) * radiusY);
            glVertex2f(centerX + cos(step * 15.0f) * radiusX, centerY + sin(step * 15.0f) * radiusY);
            glVertex2f(centerX + cos(step * 16.0f) * radiusX, centerY + sin(step * 16.0f) * radiusY);
            glVertex2f(centerX + cos(step * 17.0f) * radiusX, centerY + sin(step * 17.0f) * radiusY);
            glVertex2f(centerX + cos(step * 18.0f) * radiusX, centerY + sin(step * 18.0f) * radiusY);
            glVertex2f(centerX + cos(step * 19.0f) * radiusX, centerY + sin(step * 19.0f) * radiusY);
            glVertex2f(centerX + cos(step * 20.0f) * radiusX, centerY + sin(step * 20.0f) * radiusY);
            glVertex2f(centerX + cos(step * 21.0f) * radiusX, centerY + sin(step * 21.0f) * radiusY);
            glVertex2f(centerX + cos(step * 22.0f) * radiusX, centerY + sin(step * 22.0f) * radiusY);
            glVertex2f(centerX + cos(step * 23.0f) * radiusX, centerY + sin(step * 23.0f) * radiusY);
            glVertex2f(centerX + cos(step * 24.0f) * radiusX, centerY + sin(step * 24.0f) * radiusY);
            glVertex2f(centerX + cos(step * 25.0f) * radiusX, centerY + sin(step * 25.0f) * radiusY);
            glVertex2f(centerX + cos(step * 26.0f) * radiusX, centerY + sin(step * 26.0f) * radiusY);
            glVertex2f(centerX + cos(step * 27.0f) * radiusX, centerY + sin(step * 27.0f) * radiusY);
            glEnd();
        }
        glColor3f(0.95f, 0.95f, 0.96f);
        glLineWidth(2.0f);
        glBegin(GL_LINES);
        glVertex2f(-8.0f, 0.0f);
        glVertex2f(8.0f, 0.0f);
        glVertex2f(0.0f, -8.0f);
        glVertex2f(0.0f, 8.0f);
        glVertex2f(-5.5f, -5.5f);
        glVertex2f(5.5f, 5.5f);
        glVertex2f(-5.5f, 5.5f);
        glVertex2f(5.5f, -5.5f);
        glEnd();
        glLineWidth(1.0f);
        glPopMatrix();

        glPushMatrix();
        glTranslatef(54.0f, -2.0f, 0.0f);
        glRotatef(wheelAngle, 0.0f, 0.0f, 1.0f);
        glColor3f(0.05f, 0.05f, 0.06f);
        {
            float centerX = 0.0f;
            float centerY = 0.0f;
            float radiusX = 16.0f;
            float radiusY = 16.0f;
            float step = 2.0f * PI_VALUE / 36.0f;
            glBegin(GL_POLYGON);
            glVertex2f(centerX + cos(step * 0.0f) * radiusX, centerY + sin(step * 0.0f) * radiusY);
            glVertex2f(centerX + cos(step * 1.0f) * radiusX, centerY + sin(step * 1.0f) * radiusY);
            glVertex2f(centerX + cos(step * 2.0f) * radiusX, centerY + sin(step * 2.0f) * radiusY);
            glVertex2f(centerX + cos(step * 3.0f) * radiusX, centerY + sin(step * 3.0f) * radiusY);
            glVertex2f(centerX + cos(step * 4.0f) * radiusX, centerY + sin(step * 4.0f) * radiusY);
            glVertex2f(centerX + cos(step * 5.0f) * radiusX, centerY + sin(step * 5.0f) * radiusY);
            glVertex2f(centerX + cos(step * 6.0f) * radiusX, centerY + sin(step * 6.0f) * radiusY);
            glVertex2f(centerX + cos(step * 7.0f) * radiusX, centerY + sin(step * 7.0f) * radiusY);
            glVertex2f(centerX + cos(step * 8.0f) * radiusX, centerY + sin(step * 8.0f) * radiusY);
            glVertex2f(centerX + cos(step * 9.0f) * radiusX, centerY + sin(step * 9.0f) * radiusY);
            glVertex2f(centerX + cos(step * 10.0f) * radiusX, centerY + sin(step * 10.0f) * radiusY);
            glVertex2f(centerX + cos(step * 11.0f) * radiusX, centerY + sin(step * 11.0f) * radiusY);
            glVertex2f(centerX + cos(step * 12.0f) * radiusX, centerY + sin(step * 12.0f) * radiusY);
            glVertex2f(centerX + cos(step * 13.0f) * radiusX, centerY + sin(step * 13.0f) * radiusY);
            glVertex2f(centerX + cos(step * 14.0f) * radiusX, centerY + sin(step * 14.0f) * radiusY);
            glVertex2f(centerX + cos(step * 15.0f) * radiusX, centerY + sin(step * 15.0f) * radiusY);
            glVertex2f(centerX + cos(step * 16.0f) * radiusX, centerY + sin(step * 16.0f) * radiusY);
            glVertex2f(centerX + cos(step * 17.0f) * radiusX, centerY + sin(step * 17.0f) * radiusY);
            glVertex2f(centerX + cos(step * 18.0f) * radiusX, centerY + sin(step * 18.0f) * radiusY);
            glVertex2f(centerX + cos(step * 19.0f) * radiusX, centerY + sin(step * 19.0f) * radiusY);
            glVertex2f(centerX + cos(step * 20.0f) * radiusX, centerY + sin(step * 20.0f) * radiusY);
            glVertex2f(centerX + cos(step * 21.0f) * radiusX, centerY + sin(step * 21.0f) * radiusY);
            glVertex2f(centerX + cos(step * 22.0f) * radiusX, centerY + sin(step * 22.0f) * radiusY);
            glVertex2f(centerX + cos(step * 23.0f) * radiusX, centerY + sin(step * 23.0f) * radiusY);
            glVertex2f(centerX + cos(step * 24.0f) * radiusX, centerY + sin(step * 24.0f) * radiusY);
            glVertex2f(centerX + cos(step * 25.0f) * radiusX, centerY + sin(step * 25.0f) * radiusY);
            glVertex2f(centerX + cos(step * 26.0f) * radiusX, centerY + sin(step * 26.0f) * radiusY);
            glVertex2f(centerX + cos(step * 27.0f) * radiusX, centerY + sin(step * 27.0f) * radiusY);
            glVertex2f(centerX + cos(step * 28.0f) * radiusX, centerY + sin(step * 28.0f) * radiusY);
            glVertex2f(centerX + cos(step * 29.0f) * radiusX, centerY + sin(step * 29.0f) * radiusY);
            glVertex2f(centerX + cos(step * 30.0f) * radiusX, centerY + sin(step * 30.0f) * radiusY);
            glVertex2f(centerX + cos(step * 31.0f) * radiusX, centerY + sin(step * 31.0f) * radiusY);
            glVertex2f(centerX + cos(step * 32.0f) * radiusX, centerY + sin(step * 32.0f) * radiusY);
            glVertex2f(centerX + cos(step * 33.0f) * radiusX, centerY + sin(step * 33.0f) * radiusY);
            glVertex2f(centerX + cos(step * 34.0f) * radiusX, centerY + sin(step * 34.0f) * radiusY);
            glVertex2f(centerX + cos(step * 35.0f) * radiusX, centerY + sin(step * 35.0f) * radiusY);
            glEnd();
        }
        glColor3f(0.15f, 0.15f, 0.17f);
        {
            float centerX = 0.0f;
            float centerY = 0.0f;
            float radiusX = 12.0f;
            float radiusY = 12.0f;
            float step = 2.0f * PI_VALUE / 30.0f;
            glBegin(GL_POLYGON);
            glVertex2f(centerX + cos(step * 0.0f) * radiusX, centerY + sin(step * 0.0f) * radiusY);
            glVertex2f(centerX + cos(step * 1.0f) * radiusX, centerY + sin(step * 1.0f) * radiusY);
            glVertex2f(centerX + cos(step * 2.0f) * radiusX, centerY + sin(step * 2.0f) * radiusY);
            glVertex2f(centerX + cos(step * 3.0f) * radiusX, centerY + sin(step * 3.0f) * radiusY);
            glVertex2f(centerX + cos(step * 4.0f) * radiusX, centerY + sin(step * 4.0f) * radiusY);
            glVertex2f(centerX + cos(step * 5.0f) * radiusX, centerY + sin(step * 5.0f) * radiusY);
            glVertex2f(centerX + cos(step * 6.0f) * radiusX, centerY + sin(step * 6.0f) * radiusY);
            glVertex2f(centerX + cos(step * 7.0f) * radiusX, centerY + sin(step * 7.0f) * radiusY);
            glVertex2f(centerX + cos(step * 8.0f) * radiusX, centerY + sin(step * 8.0f) * radiusY);
            glVertex2f(centerX + cos(step * 9.0f) * radiusX, centerY + sin(step * 9.0f) * radiusY);
            glVertex2f(centerX + cos(step * 10.0f) * radiusX, centerY + sin(step * 10.0f) * radiusY);
            glVertex2f(centerX + cos(step * 11.0f) * radiusX, centerY + sin(step * 11.0f) * radiusY);
            glVertex2f(centerX + cos(step * 12.0f) * radiusX, centerY + sin(step * 12.0f) * radiusY);
            glVertex2f(centerX + cos(step * 13.0f) * radiusX, centerY + sin(step * 13.0f) * radiusY);
            glVertex2f(centerX + cos(step * 14.0f) * radiusX, centerY + sin(step * 14.0f) * radiusY);
            glVertex2f(centerX + cos(step * 15.0f) * radiusX, centerY + sin(step * 15.0f) * radiusY);
            glVertex2f(centerX + cos(step * 16.0f) * radiusX, centerY + sin(step * 16.0f) * radiusY);
            glVertex2f(centerX + cos(step * 17.0f) * radiusX, centerY + sin(step * 17.0f) * radiusY);
            glVertex2f(centerX + cos(step * 18.0f) * radiusX, centerY + sin(step * 18.0f) * radiusY);
            glVertex2f(centerX + cos(step * 19.0f) * radiusX, centerY + sin(step * 19.0f) * radiusY);
            glVertex2f(centerX + cos(step * 20.0f) * radiusX, centerY + sin(step * 20.0f) * radiusY);
            glVertex2f(centerX + cos(step * 21.0f) * radiusX, centerY + sin(step * 21.0f) * radiusY);
            glVertex2f(centerX + cos(step * 22.0f) * radiusX, centerY + sin(step * 22.0f) * radiusY);
            glVertex2f(centerX + cos(step * 23.0f) * radiusX, centerY + sin(step * 23.0f) * radiusY);
            glVertex2f(centerX + cos(step * 24.0f) * radiusX, centerY + sin(step * 24.0f) * radiusY);
            glVertex2f(centerX + cos(step * 25.0f) * radiusX, centerY + sin(step * 25.0f) * radiusY);
            glVertex2f(centerX + cos(step * 26.0f) * radiusX, centerY + sin(step * 26.0f) * radiusY);
            glVertex2f(centerX + cos(step * 27.0f) * radiusX, centerY + sin(step * 27.0f) * radiusY);
            glVertex2f(centerX + cos(step * 28.0f) * radiusX, centerY + sin(step * 28.0f) * radiusY);
            glVertex2f(centerX + cos(step * 29.0f) * radiusX, centerY + sin(step * 29.0f) * radiusY);
            glEnd();
        }
        glColor3f(0.74f, 0.74f, 0.78f);
        {
            float centerX = 0.0f;
            float centerY = 0.0f;
            float radiusX = 7.0f;
            float radiusY = 7.0f;
            float step = 2.0f * PI_VALUE / 28.0f;
            glBegin(GL_POLYGON);
            glVertex2f(centerX + cos(step * 0.0f) * radiusX, centerY + sin(step * 0.0f) * radiusY);
            glVertex2f(centerX + cos(step * 1.0f) * radiusX, centerY + sin(step * 1.0f) * radiusY);
            glVertex2f(centerX + cos(step * 2.0f) * radiusX, centerY + sin(step * 2.0f) * radiusY);
            glVertex2f(centerX + cos(step * 3.0f) * radiusX, centerY + sin(step * 3.0f) * radiusY);
            glVertex2f(centerX + cos(step * 4.0f) * radiusX, centerY + sin(step * 4.0f) * radiusY);
            glVertex2f(centerX + cos(step * 5.0f) * radiusX, centerY + sin(step * 5.0f) * radiusY);
            glVertex2f(centerX + cos(step * 6.0f) * radiusX, centerY + sin(step * 6.0f) * radiusY);
            glVertex2f(centerX + cos(step * 7.0f) * radiusX, centerY + sin(step * 7.0f) * radiusY);
            glVertex2f(centerX + cos(step * 8.0f) * radiusX, centerY + sin(step * 8.0f) * radiusY);
            glVertex2f(centerX + cos(step * 9.0f) * radiusX, centerY + sin(step * 9.0f) * radiusY);
            glVertex2f(centerX + cos(step * 10.0f) * radiusX, centerY + sin(step * 10.0f) * radiusY);
            glVertex2f(centerX + cos(step * 11.0f) * radiusX, centerY + sin(step * 11.0f) * radiusY);
            glVertex2f(centerX + cos(step * 12.0f) * radiusX, centerY + sin(step * 12.0f) * radiusY);
            glVertex2f(centerX + cos(step * 13.0f) * radiusX, centerY + sin(step * 13.0f) * radiusY);
            glVertex2f(centerX + cos(step * 14.0f) * radiusX, centerY + sin(step * 14.0f) * radiusY);
            glVertex2f(centerX + cos(step * 15.0f) * radiusX, centerY + sin(step * 15.0f) * radiusY);
            glVertex2f(centerX + cos(step * 16.0f) * radiusX, centerY + sin(step * 16.0f) * radiusY);
            glVertex2f(centerX + cos(step * 17.0f) * radiusX, centerY + sin(step * 17.0f) * radiusY);
            glVertex2f(centerX + cos(step * 18.0f) * radiusX, centerY + sin(step * 18.0f) * radiusY);
            glVertex2f(centerX + cos(step * 19.0f) * radiusX, centerY + sin(step * 19.0f) * radiusY);
            glVertex2f(centerX + cos(step * 20.0f) * radiusX, centerY + sin(step * 20.0f) * radiusY);
            glVertex2f(centerX + cos(step * 21.0f) * radiusX, centerY + sin(step * 21.0f) * radiusY);
            glVertex2f(centerX + cos(step * 22.0f) * radiusX, centerY + sin(step * 22.0f) * radiusY);
            glVertex2f(centerX + cos(step * 23.0f) * radiusX, centerY + sin(step * 23.0f) * radiusY);
            glVertex2f(centerX + cos(step * 24.0f) * radiusX, centerY + sin(step * 24.0f) * radiusY);
            glVertex2f(centerX + cos(step * 25.0f) * radiusX, centerY + sin(step * 25.0f) * radiusY);
            glVertex2f(centerX + cos(step * 26.0f) * radiusX, centerY + sin(step * 26.0f) * radiusY);
            glVertex2f(centerX + cos(step * 27.0f) * radiusX, centerY + sin(step * 27.0f) * radiusY);
            glEnd();
        }
        glColor3f(0.95f, 0.95f, 0.96f);
        glLineWidth(2.0f);
        glBegin(GL_LINES);
        glVertex2f(-8.0f, 0.0f);
        glVertex2f(8.0f, 0.0f);
        glVertex2f(0.0f, -8.0f);
        glVertex2f(0.0f, 8.0f);
        glVertex2f(-5.5f, -5.5f);
        glVertex2f(5.5f, 5.5f);
        glVertex2f(-5.5f, 5.5f);
        glVertex2f(5.5f, -5.5f);
        glEnd();
        glLineWidth(1.0f);
        glPopMatrix();

        glDisable(GL_BLEND);
        glPopMatrix();
    }
}

void scene3() {
    gradSky(0.50f,0.73f,0.92f, 0.72f,0.88f,0.98f);
    sun(1090+sunHorizontalOffset*0.22f,610);
    cloud(230+cloudOffsetX_layerA*0.7f,598);
    parallaxCity(parallaxOffset_scene2*0.25f, false);

    glColor3f(0.28f,0.50f,0.30f);
    rect(0,150,1280,130);
    skylineTrees(238, false);

    glColor3f(0.70f,0.72f,0.70f);
    rect(520,210,720,46);
    glColor3f(0.54f,0.56f,0.56f);
    rect(520,252,720,8);
    glColor3f(0.64f,0.66f,0.66f);
    for (float x=548; x<1200; x+=74) rect(x,218,40,4);

    officeComplex(false);

    roadBottomY_current=55; roadHeight_current=95; roadLaneCount_current=2; roadNightMode_current=false;
    road();
    officeRampDown();

    glColor3f(0.18f,0.36f,0.20f);
    rect(0,150,470,36);
    rect(625,150,655,36);
    glColor3f(0.28f,0.54f,0.30f);
    for (float x=0; x<1280; x+=82) ellipse(x+28,150,36,9,18);

    glColor3f(0.70f,0.72f,0.70f);
    rect(520,210,720,18);
    glColor3f(0.42f,0.44f,0.44f);
    rect(520,226,720,4);

    parkingBarrier(630,155, barrierRotationAngle_scene3);
    car(carPositionX_scene3, carPositionY_scene3, 0.86f,0.24f,0.20f, wheelRotationAngle_scene3, false,false,true);
}

void scene4() {
    drawInteriorShell(0.86f,0.88f,0.89f, 0.60f,0.62f,0.64f);
    ceilingPanels();
    officeWindowWall();
    glassPartition(545,310,270,230);
    glColor3f(0.55f,0.58f,0.60f); rect(875,305,325,222);
    glColor3f(0.80f,0.82f,0.84f); rect(892,323,291,184);
    glColor3f(0.62f,0.64f,0.65f);
    rect(918,470,220,8);
    rect(918,438,170,8);
    rect(918,406,245,8);
    wallClock(1210,570,wallClockHandAngle_scene4);
    pottedPlant(32,205);
    pottedPlant(1210,205);

    glColor3f(0.72f,0.73f,0.74f);
    rect(120,248,300,18);
    rect(460,250,300,18);
    rect(800,248,300,18);
    glColor3f(0.44f,0.47f,0.50f);
    rect(120,266,300,4);
    rect(460,268,300,4);
    rect(800,266,300,4);

    characterSittingPose = true;
    characterCarryBriefcase = false;
    characterPointingPose = false;
    characterFacingRight = true;
    characterLeftArmAngleDegrees = -28 - sin(sceneFrameCounter*0.3f)*12;
    characterRightArmAngleDegrees = -18 + sin(sceneFrameCounter*0.3f)*12;
    lowOfficeChair(272,116,1.0f,true);
    character(278,122);
    officeDeskSurface(155,82,0.98f);

    characterLeftArmAngleDegrees = -30 - typingArmAngle_scene4*0.4f;
    characterRightArmAngleDegrees = -22 + typingArmAngle_scene4;
    lowOfficeChair(616,126,1.0f,true);
    character(622,132);
    officeDeskSurface(500,96,0.95f);

    characterLeftArmAngleDegrees = -24 - sin(sceneFrameCounter*0.35f+2.0f)*10;
    characterRightArmAngleDegrees = -18 + sin(sceneFrameCounter*0.35f+2.0f)*10;
    lowOfficeChair(962,112,1.0f,true);
    character(968,118);
    officeDeskSurface(838,80,0.96f);

    characterSittingPose = false;
}

void scene5() {
    drawInteriorShell(0.88f,0.87f,0.83f, 0.57f,0.59f,0.56f);
    ceilingPanels();
    glColor3f(0.30f,0.32f,0.34f); rect(0,552,1280,6);
    glColor3f(0.72f,0.74f,0.72f);
    rect(0,220,1280,22);
    servingCounter(72,255);
    glColor3f(0.48f,0.50f,0.48f);
    rect(970,238,158,18);
    glColor3f(0.74f,0.75f,0.72f);
    rect(958,256,182,18);
    coffeeMachine(1000,270);
    steamParticles();
    pottedPlant(1170,220);

    characterSittingPose = true;
    characterPointingPose = false;
    characterCarryBriefcase = false;
    characterFacingRight = true;
    characterLeftArmAngleDegrees = 12 + sin(sceneFrameCounter*0.2f)*8;
    characterRightArmAngleDegrees = -28;
    lowOfficeChair(390,86,0.92f,true);
    character(398,92);
    cafeteriaTableSurface(270,55,1.08f);
    foodTray(382,120);

    characterFacingRight = false;
    lowOfficeChair(905,82,0.92f,false);
    character(896,88);
    cafeteriaTableSurface(730,50,1.04f);
    foodTray(842,113);

    characterFacingRight = true;
    lowOfficeChair(560,232,0.78f,true);
    character(566,238);
    cafeteriaTableSurface(472,202,0.74f);
    foodTray(548,250);

    characterSittingPose = false;
    characterFacingRight = false;
    characterLeftArmAngleDegrees = -10;
    characterRightArmAngleDegrees = -22;
    character(260,250);
    characterFacingRight = true;
    characterLeftArmAngleDegrees = -workerWalkLegSwing_scene5*0.8f;
    characterRightArmAngleDegrees = workerWalkLegSwing_scene5*0.8f;
    characterLegSwingAngleDegrees = workerWalkLegSwing_scene5;
    character(workerWalkPositionX_scene5,220);
}

void scene6() {
    drawInteriorShell(0.82f,0.85f,0.87f, 0.49f,0.51f,0.54f);
    ceilingPanels();
    glColor3f(0.68f,0.73f,0.76f);
    rect(0,220,1280,10);
    glColor3f(0.70f,0.76f,0.80f);
    rect(52,292,360,236);
    glColor3f(0.38f,0.48f,0.54f);
    rect(52,292,360,4); rect(52,524,360,4); rect(52,292,4,236); rect(408,292,4,236);
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    glColor4f(0.88f,0.96f,1.0f,0.20f);
    rect(68,310,326,196);
    glDisable(GL_BLEND);
    glColor3f(0.30f,0.36f,0.40f);
    for (int i=1; i<4; i++) rect(68+i*82,310,3,196);

    glColor3f(0.18f,0.20f,0.24f);
    rect(690,250,510,330);
    glColor3f(0.28f,0.31f,0.35f);
    rect(706,266,478,298);
    glColor3f(0.94f,0.95f,0.92f);
    rect(730,302,430,224);
    glColor3f(0.18f,0.18f,0.20f);
    bmpText(792,505,"QUARTERLY PROJECT UPDATE");
    glColor3f(0.72f,0.74f,0.74f);
    rect(775,472,300,5);
    rect(775,454,242,5);
    float barBase = 340, barW = 38, gap = 38;
    float heights[] = {50,80,112,146};
    glColor3f(0.24f,0.60f,0.84f); rect(805,barBase,barW,heights[0]*presentationBarGrowRatio_scene6);
    glColor3f(0.28f,0.72f,0.46f); rect(805+(barW+gap),barBase,barW,heights[1]*presentationBarGrowRatio_scene6);
    glColor3f(0.94f,0.66f,0.20f); rect(805+2*(barW+gap),barBase,barW,heights[2]*presentationBarGrowRatio_scene6);
    glColor3f(0.88f,0.32f,0.28f); rect(805+3*(barW+gap),barBase,barW,heights[3]*presentationBarGrowRatio_scene6);
    glColor3f(0.34f,0.34f,0.34f); rect(785,barBase-5,318,5);

    glColor3f(0.58f,0.47f,0.36f);
    glBegin(GL_POLYGON);
    glVertex2f(130,178); glVertex2f(610,178); glVertex2f(735,54); glVertex2f(45,54);
    glEnd();
    glColor3f(0.44f,0.34f,0.25f);
    glBegin(GL_POLYGON);
    glVertex2f(45,54); glVertex2f(735,54); glVertex2f(696,30); glVertex2f(84,30);
    glEnd();
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    glColor4f(0,0,0,0.20f);
    ellipse(390,44,320,34,36);
    glDisable(GL_BLEND);

    meetingChair(180,172,0.88f,false);
    meetingChair(330,188,0.88f,false);
    meetingChair(500,174,0.88f,false);
    characterSittingPose = true;
    characterPointingPose = false;
    characterCarryBriefcase = false;
    characterLeftArmAngleDegrees = -10;
    characterRightArmAngleDegrees = 10;
    characterFacingRight = true;
    character(188,184);
    character(338,200);
    character(508,186);

    glColor3f(0.40f,0.29f,0.20f);
    glBegin(GL_POLYGON);
    glVertex2f(110,180); glVertex2f(620,180); glVertex2f(700,72); glVertex2f(72,72);
    glEnd();
    glColor3f(0.64f,0.50f,0.34f);
    glBegin(GL_POLYGON);
    glVertex2f(130,166); glVertex2f(596,166); glVertex2f(654,88); glVertex2f(112,88);
    glEnd();
    glColor3f(0.20f,0.20f,0.22f);
    rect(350,118,92,8);
    rect(366,126,58,36);
    glColor3f(0.34f,0.58f,0.72f);
    rect(372,132,46,24);
    glColor3f(0.86f,0.86f,0.82f);
    rect(492,145,34,4);
    rect(230,140,34,4);

    meetingChair(245,45,0.98f,true);
    meetingChair(475,48,0.98f,true);
    characterFacingRight = true;
    character(254,64);
    characterFacingRight = false;
    character(484,67);

    characterSittingPose = false;
    characterFacingRight = true;
    characterPointingPose = true;
    characterLegSwingAngleDegrees = 0;
    characterLeftArmAngleDegrees = 10;
    characterRightArmAngleDegrees = -48;
    character(652,230);
    glColor3f(0.88f,0.08f,0.08f);
    glLineWidth(2);
    glBegin(GL_LINES);
    glVertex2f(678,370);
    glVertex2f(918,416);
    glEnd();
    glLineWidth(1);
    characterPointingPose = false;
    pottedPlant(1220,205);
}

void scene7() {
    gradSky(0.34f,0.24f,0.32f, 0.92f,0.60f,0.34f);
    sun(180+sunHorizontalOffset*0.25f,580);
    officeComplex(true);
    officeRampUp();
    roadBottomY_current=250; roadHeight_current=90; roadLaneCount_current=2; roadNightMode_current=false;
    road();
    parkingBarrier(860,170, barrierRotationAngle_scene7);
    car(carPositionX_scene7, carPositionY_scene7, 0.86f,0.24f,0.20f, wheelRotationAngle_scene7, true,false,true);
}

void scene8() {
    gradSky(0.06f,0.08f,0.16f, 0.12f,0.16f,0.28f);
    moon(1060,618);
    stars();
    parallaxCity(parallaxOffset_scene8, true);
    roadBottomY_current=110; roadHeight_current=230; roadLaneCount_current=4; roadNightMode_current=true;
    road();
    glColor3f(0.10f,0.20f,0.12f); rect(0,0,1280,110);
    for (float px=70; px<=1230; px+=170) streetLight(px,340,true);
    car(trafficCarPositionX_scene8_A,160,0.80f,0.26f,0.20f, trafficCarWheelAngle_scene8_A, true,true,true);
    suv(trafficCarPositionX_scene8_B,230,0.20f,0.52f,0.84f, trafficCarWheelAngle_scene8_B, false);
    deliveryVan(trafficCarPositionX_scene8_C,300,0.30f,0.72f,0.44f, trafficCarWheelAngle_scene8_C, true);
}

void scene9() {
    gradSky(0.05f,0.08f,0.18f, 0.11f,0.16f,0.28f);
    stars();
    moon(1030,620);
    homeGround(true);
    tree(88,120); tree(640,132); tree(748,124);
    house(isHouseLightOn);
    bool headOn = carState_scene9 != 2;
    car(scene9_carPosX, scene9_carPosY, 0.84f,0.20f,0.18f, wheelRotationAngle, headOn, true, true);
    headlightCone(scene9_carPosX, scene9_carPosY, headOn);
}

void resetScene(int idx) {
    if (idx==1) {
        scene1_carPosX=scene1GarageStartX; scene1_carPosY=scene1GarageStartY; wheelRotationAngle=0;
        carState_scene1=0; scene1HasCarExitedScreen=false; initRain();
    }
    if (idx==2) {
        trafficCarPositionX_scene2_A=-180; trafficCarPositionX_scene2_B=1350; trafficCarPositionX_scene2_C=-420;
        trafficCarWheelAngle_scene2_A=0; trafficCarWheelAngle_scene2_B=0; trafficCarWheelAngle_scene2_C=0;
        parallaxOffset_scene2=0; roadDashOffset_scene2=0;
    }
    if (idx==3) {
        carPositionX_scene3=-220; carPositionY_scene3=96; wheelRotationAngle_scene3=0; barrierRotationAngle_scene3=0;
    }
    if (idx==4) { typingArmAngle_scene4=0; wallClockHandAngle_scene4=90; }
    if (idx==5) { workerWalkPositionX_scene5=650; workerWalkLegSwing_scene5=0; initSteam(); }
    if (idx==6) { presentationBarGrowRatio_scene6=0; pointerOscillationAngle_scene6=0; }
    if (idx==7) {
        carPositionX_scene7=520; carPositionY_scene7=170; wheelRotationAngle_scene7=0; barrierRotationAngle_scene7=0;
    }
    if (idx==8) {
        trafficCarPositionX_scene8_A=-260; trafficCarPositionX_scene8_B=1380; trafficCarPositionX_scene8_C=-560;
        trafficCarWheelAngle_scene8_A=0; trafficCarWheelAngle_scene8_B=0; trafficCarWheelAngle_scene8_C=0;
        parallaxOffset_scene8=0;
    }
    if (idx==9) {
        scene9_carPosX=-220; scene9_carPosY=72; wheelRotationAngle=0; carState_scene9=0;
        scene9ParkedFrameCounter=0; scene9ParkingCompleted=false; isHouseLightOn=false;
    }
}

void nextScene() {
    currentScene++;
    if (currentScene > LAST_SCENE_INDEX) currentScene = FIRST_SCENE_INDEX;
    sceneFrameCounter = 0;
    resetScene(currentScene);
}

void startScene1CarMovement() {
    if (carState_scene1 == 0) carState_scene1 = 1;
}

void moveScene1CarDownToRoad() {
    if (carState_scene1 != 1) return;
    float baseSpeed = 1.6f;
    float step = baseSpeed * animationSpeed;
    float rotationStep = step * 2.0f;
    scene1_carPosY -= step;
    wheelRotationAngle -= rotationStep;
    if (scene1_carPosY <= scene1RoadTravelY) {
        scene1_carPosY = scene1RoadTravelY;
        carState_scene1 = 2;
    }
}

void moveScene1CarAlongRoad() {
    if (carState_scene1 != 2) return;
    float baseSpeed = 2.0f;
    float step = baseSpeed * animationSpeed;
    float rotationStep = step * 2.0f;
    scene1_carPosX += step;
    wheelRotationAngle -= rotationStep;
    if (scene1_carPosX > scene1ExitCheckX) scene1HasCarExitedScreen = true;
}

void computeScene2TrafficSpeeds(float &speedA, float &speedB, float &speedC) {
    float trafficPulse = sin(sceneFrameCounter * 0.035f);
    float baseA = 3.25f;
    float baseB = 2.05f;
    float baseC = 3.85f;
    float extraA = 0.32f * trafficPulse;
    float extraB = 0.18f * sin(sceneFrameCounter * 0.026f + 1.7f);
    float extraC = 0.42f * sin(sceneFrameCounter * 0.031f + 3.2f);
    speedA = baseA + extraA;
    speedB = baseB + extraB;
    speedC = baseC + extraC;
    if (trafficCarPositionX_scene2_A > 470 && trafficCarPositionX_scene2_A < 640) speedA *= 0.72f;
    if (trafficCarPositionX_scene2_C > 380 && trafficCarPositionX_scene2_C < 540) speedC *= 0.82f;
}

void moveScene2TrafficPositions(float speedA, float speedB, float speedC) {
    trafficCarPositionX_scene2_A += speedA;
    trafficCarPositionX_scene2_B -= speedB;
    trafficCarPositionX_scene2_C += speedC;
    if (trafficCarPositionX_scene2_A > 1440) trafficCarPositionX_scene2_A = -220;
    if (trafficCarPositionX_scene2_B < -280) trafficCarPositionX_scene2_B = 1480;
    if (trafficCarPositionX_scene2_C > 1500) trafficCarPositionX_scene2_C = -360;
}

void spinScene2TrafficWheels(float speedA, float speedB, float speedC) {
    trafficCarWheelAngle_scene2_A -= speedA * 5.0f;
    trafficCarWheelAngle_scene2_B += speedB * 4.0f;
    trafficCarWheelAngle_scene2_C -= speedC * 4.8f;
}

void moveScene2RoadDashes() {
    float dashSpeed = 1.9f;
    roadDashOffset_scene2 = wrap(roadDashOffset_scene2 - dashSpeed, 0, 92);
}

void moveScene2Parallax() {
    float drift = 0.55f;
    parallaxOffset_scene2 -= drift;
    if (parallaxOffset_scene2 < -220) parallaxOffset_scene2 += 220;
}

void raiseScene3BarrierIfNeeded() {
    if (carPositionX_scene3 <= 470) return;
    float newAngle = barrierRotationAngle_scene3 + 2.8f;
    barrierRotationAngle_scene3 = clamp(newAngle, 0, 85);
}

void anim1() {
    startScene1CarMovement();
    moveScene1CarDownToRoad();
    moveScene1CarAlongRoad();
}

void anim2() {
    float speedA = 0.0f;
    float speedB = 0.0f;
    float speedC = 0.0f;
    computeScene2TrafficSpeeds(speedA, speedB, speedC);
    moveScene2TrafficPositions(speedA, speedB, speedC);
    spinScene2TrafficWheels(speedA, speedB, speedC);
    moveScene2RoadDashes();
    moveScene2Parallax();
}

void anim3() {
    float baseSpeed = 2.65f;
    if (carPositionX_scene3 < 500) {
        carPositionX_scene3 += baseSpeed;
        carPositionY_scene3 = 96.0f;
        wheelRotationAngle_scene3 -= baseSpeed * 4.2f;
    } else if (carPositionX_scene3 < 760) {
        float drivewaySpeed = 1.85f;
        carPositionX_scene3 += drivewaySpeed;
        float progress = (carPositionX_scene3 - 500.0f) / 260.0f;
        float t = clamp(progress, 0.0f, 1.0f);
        float startY = 96.0f;
        float rise = 134.0f;
        carPositionY_scene3 = startY + t * rise;
        wheelRotationAngle_scene3 -= drivewaySpeed * 4.0f;
    } else {
        carPositionX_scene3 = 760.0f;
        carPositionY_scene3 = 230.0f;
    }
    raiseScene3BarrierIfNeeded();
}

void anim4() {
    typingArmAngle_scene4 = sin(sceneFrameCounter*0.32f)*16.0f;
    wallClockHandAngle_scene4 -= 1.6f;
    if (wallClockHandAngle_scene4 < -360) wallClockHandAngle_scene4 += 360;
}

void anim5() {
    if (workerWalkPositionX_scene5 > 430) {
        workerWalkPositionX_scene5 -= 1.6f;
        workerWalkLegSwing_scene5 = sin(sceneFrameCounter*0.32f)*18.0f;
    } else {
        workerWalkPositionX_scene5 = 430;
        workerWalkLegSwing_scene5 = 2.0f * sin(sceneFrameCounter*0.12f);
    }
    for (int i=0; i<STEAM_PARTICLE_COUNT; i++) {
        coffeeSteamParticles[i].y += coffeeSteamParticles[i].speed;
        coffeeSteamParticles[i].x += sin(starTwinkleCounter*0.5f+i)*0.18f;
        coffeeSteamParticles[i].alpha -= 0.006f;
        if (coffeeSteamParticles[i].y > 430 || coffeeSteamParticles[i].alpha <= 0.02f) {
            int col = i%5;
            coffeeSteamParticles[i].x = 1048 + (col-2)*6.0f;
            coffeeSteamParticles[i].y = 326 + (i%6)*4.0f;
            coffeeSteamParticles[i].alpha = 0.82f;
        }
    }
}

void anim6() {
    presentationBarGrowRatio_scene6 = clamp(presentationBarGrowRatio_scene6+0.007f, 0,1);
    pointerOscillationAngle_scene6 = sin(sceneFrameCounter*0.15f)*6.0f;
}

void anim7() {
    float speed=2.9f;
    if (carPositionX_scene7 < 1320) {
        carPositionX_scene7 += speed;
        wheelRotationAngle_scene7 -= speed*4.1f;
    }
    if (carPositionX_scene7 < 580) carPositionY_scene7 = 170;
    else carPositionY_scene7 = clamp(170+(carPositionX_scene7-580)*0.34f, 170,286);
    if (carPositionX_scene7 > 700) barrierRotationAngle_scene7 = clamp(barrierRotationAngle_scene7+2.6f, 0,85);
}

void anim8() {
    const float sA=3.6f, sB=2.7f, sC=4.3f;
    trafficCarPositionX_scene8_A += sA;
    trafficCarPositionX_scene8_B -= sB;
    trafficCarPositionX_scene8_C += sC;
    if (trafficCarPositionX_scene8_A > 1460) trafficCarPositionX_scene8_A = -260;
    if (trafficCarPositionX_scene8_B < -260) trafficCarPositionX_scene8_B = 1460;
    if (trafficCarPositionX_scene8_C > 1540) trafficCarPositionX_scene8_C = -520;
    trafficCarWheelAngle_scene8_A -= sA*4;
    trafficCarWheelAngle_scene8_B -= sB*4;
    trafficCarWheelAngle_scene8_C -= sC*4;
    parallaxOffset_scene8 -= 0.65f;
    if (parallaxOffset_scene8 < -220) parallaxOffset_scene8 += 220;
}

void anim9() {
    if (carState_scene9==0) {
        float step = 1.8f * animationSpeed;
        scene9_carPosX += step; scene9_carPosY = 72; wheelRotationAngle -= step*2.0f;
        if (scene9_carPosX >= 340) { scene9_carPosX = 340; carState_scene9=1; }
    }
    if (carState_scene9==1) {
        float up = 1.4f * animationSpeed;
        float right = 0.7f * animationSpeed;
        if (scene9_carPosX < 392) scene9_carPosX += right;
        if (scene9_carPosY < 318) scene9_carPosY += up;
        wheelRotationAngle -= (up+right)*1.8f;
        if (scene9_carPosX >= 392) scene9_carPosX = 392;
        if (scene9_carPosY >= 318) scene9_carPosY = 318;
        if (scene9_carPosX >= 392 && scene9_carPosY >= 318) {
            carState_scene9=2; scene9ParkingCompleted=true; isHouseLightOn=true;
        }
    }
    if (carState_scene9==2) scene9ParkedFrameCounter++;
}

void controls() {
    const float px=14, py=706, pw=430, ph=196;
    const char* names[10] = {"","Scene 1: Leaving Home","Scene 2: Morning Traffic","Scene 3: Office Parking",
        "Scene 4: Open Office","Scene 5: Cafeteria","Scene 6: Meeting Room","Scene 7: Leaving Office",
        "Scene 8: Night Traffic","Scene 9: Home Again"};
    char frame[80];
    snprintf(frame, sizeof(frame), "Frame: %d", totalFrameCounter);
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    glColor4f(0,0,0,0.38f); rect(px, py-ph, pw, ph);
    glDisable(GL_BLEND);
    glColor3f(1,1,1);
    bmpText(px+10, py-20, "Scene info:");
    bmpText(px+10, py-44, names[currentScene]);
    bmpText(px+10, py-74, "Keybinds:");
    bmpText(px+10, py-98,  "SPACE : Pause");
    bmpText(px+10, py-120, "R     : Rain ON");
    bmpText(px+10, py-142, "V     : Rain OFF");
    bmpText(px+10, py-164, "N     : Restart");
    bmpText(px+10, py-186, frame);
}

void textIfNeeded() {
    if (!showTextOverlay) return;
    char buf[128];
    snprintf(buf, sizeof(buf), "Scene %d  Frame: %d", currentScene, sceneFrameCounter);
    glColor3f(0,0,0); bmpText(23,692,buf);
    glColor3f(1,1,1); bmpText(20,695,buf);
}

void reshape(int w, int h) {
    if (h <= 0) h = 1;
    float asp = (float)w / h;
    int vx=0, vy=0, vw=w, vh=h;
    if (asp > TARGET_ASPECT) { vw = (int)(h * TARGET_ASPECT); vx = (w - vw)/2; }
    else if (asp < TARGET_ASPECT) { vh = (int)(w / TARGET_ASPECT); vy = (h - vh)/2; }
    glViewport(vx, vy, vw, vh);
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    gluOrtho2D(0,1280,0,720);
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
}

void display() {
    if (currentScene==1 || currentScene==2) glClearColor(0.62f,0.84f,0.97f,1);
    else if (currentScene>=3 && currentScene<=6) glClearColor(0.85f,0.90f,0.95f,1);
    else if (currentScene==7) glClearColor(0.60f,0.40f,0.30f,1);
    else glClearColor(0.05f,0.07f,0.15f,1);
    glClear(GL_COLOR_BUFFER_BIT);
    glLoadIdentity();
    switch(currentScene) {
        case 1: scene1(); break;
        case 2: scene2(); break;
        case 3: scene3(); break;
        case 4: scene4(); break;
        case 5: scene5(); break;
        case 6: scene6(); break;
        case 7: scene7(); break;
        case 8: scene8(); break;
        case 9: scene9(); break;
        default: scene1(); break;
    }
    bool outdoor = currentScene==1 || currentScene==2 || currentScene==3 || currentScene==7 || currentScene==8 || currentScene==9;
    if (isRainEnabled && outdoor) drawRain();
    controls();
    textIfNeeded();
    if (fadeOverlayAlpha > 0.001f) {
        glEnable(GL_BLEND);
        glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
        glColor4f(0,0,0, fadeOverlayAlpha);
        rect(0,0,1280,720);
        glDisable(GL_BLEND);
    }
    glutSwapBuffers();
}

void update(int val) {
    totalFrameCounter++;
    if (!isPaused) {
        cloudOffsetX_layerA = wrap(cloudOffsetX_layerA+0.32f, -260,1320);
        cloudOffsetX_layerB = wrap(cloudOffsetX_layerB+0.22f, -360,1320);
        sunHorizontalOffset += 0.18f;
        if (sunHorizontalOffset > 220) sunHorizontalOffset = -220;
        starTwinkleCounter += 0.06f;
        if (isRainEnabled) {
            float fall = 1.1f * animationSpeed;
            for (int i=0; i<RAIN_DROP_COUNT; i++) {
                rainDropY[i] -= fall;
                if (rainDropY[i] < -20) {
                    rainDropY[i] = 720 + (rand()%120);
                    rainDropX[i] = rand() % WINDOW_WIDTH;
                }
            }
        }
        switch(currentScene) {
            case 1: anim1(); break;
            case 2: anim2(); break;
            case 3: anim3(); break;
            case 4: anim4(); break;
            case 5: anim5(); break;
            case 6: anim6(); break;
            case 7: anim7(); break;
            case 8: anim8(); break;
            case 9: anim9(); break;
        }
        if (!isSceneTransitionActive) {
            sceneFrameCounter++;
            int trigger = sceneDurationFrameCount[currentScene] - FADE_START_BEFORE_END_FRAMES;
            bool ready = true;
            if (currentScene==1) ready = scene1HasCarExitedScreen;
            if (currentScene==9) ready = scene9ParkingCompleted && scene9ParkedFrameCounter > 45;
            if (sceneFrameCounter >= trigger && ready) {
                isSceneTransitionActive = true;
                isFadeOutPhase = true;
            }
        }
        if (isSceneTransitionActive) {
            if (isFadeOutPhase) {
                fadeOverlayAlpha += FADE_ALPHA_STEP;
                if (fadeOverlayAlpha >= 1.0f) {
                    fadeOverlayAlpha = 1.0f;
                    nextScene();
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
    }
    glutPostRedisplay();
    glutTimerFunc(TIMER_INTERVAL_MS, update, 0);
}

void keyboard(unsigned char key, int x, int y) {
    if (key==' ') isPaused = !isPaused;
    if (key=='r'||key=='R') isRainEnabled = true;
    if (key=='v'||key=='V') isRainEnabled = false;
    if (key=='t'||key=='T') showTextOverlay = !showTextOverlay;
    if (key=='n'||key=='N') {
        currentScene=1;
        sceneFrameCounter=0;
        totalFrameCounter=0;
        fadeOverlayAlpha=0;
        isSceneTransitionActive=false;
        isFadeOutPhase=false;
        resetScene(1);
    }
    if (key==27) exit(0);
}

int main(int argc, char** argv) {
    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGBA);
    glutInitWindowSize(WINDOW_WIDTH, WINDOW_HEIGHT);
    glutInitWindowPosition(80,40);
    glutCreateWindow("Life-of-A-Office-Worker-In-A-Modern-Urban-Area");
    glDisable(GL_DEPTH_TEST);
    resetScene(1);
    glutDisplayFunc(display);
    glutReshapeFunc(reshape);
    glutKeyboardFunc(keyboard);
    glutTimerFunc(TIMER_INTERVAL_MS, update, 0);
    glutMainLoop();
    return 0;
}
