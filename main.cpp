/* Life of An Office Worker In A Modern Urban Area */

#ifdef __APPLE__
#include <GLUT/glut.h>
#else
#include <GL/glut.h>
#endif

#include <cmath>
#include <cstdlib>

static int gWindowWidth = 1280;
static int gWindowHeight = 720;

static int currentScene = 1;
static double gLastTimeSec = 0.0;
static bool gFollowCamera = true;

static float gSmoothCamX = 0.0f;
static float gSmoothCamY = 2.8f;
static float gSmoothCamZ = 13.6f;
static bool gCameraInitialized = false;

static float degToRad(float degree) {
    return degree * 3.14159265f / 180.0f;
}

static void normalize3(float& x, float& y, float& z) {
    const float len = std::sqrt(x * x + y * y + z * z);
    if (len > 0.00001f) {
        x /= len;
        y /= len;
        z /= len;
    }
}

static void setPerspective(float fovYDeg, float aspect, float zNear, float zFar) {
    const float ymax = zNear * std::tan(degToRad(fovYDeg * 0.5f));
    const float xmax = ymax * aspect;
    glFrustum(-xmax, xmax, -ymax, ymax, zNear, zFar);
}

static void setLookAt(float eyeX, float eyeY, float eyeZ,
                      float centerX, float centerY, float centerZ,
                      float upX, float upY, float upZ) {
    float fx = centerX - eyeX;
    float fy = centerY - eyeY;
    float fz = centerZ - eyeZ;
    normalize3(fx, fy, fz);

    float sx = fy * upZ - fz * upY;
    float sy = fz * upX - fx * upZ;
    float sz = fx * upY - fy * upX;
    normalize3(sx, sy, sz);

    float ux = sy * fz - sz * fy;
    float uy = sz * fx - sx * fz;
    float uz = sx * fy - sy * fx;

    GLfloat m[16] = {
        sx, ux, -fx, 0.0f,
        sy, uy, -fy, 0.0f,
        sz, uz, -fz, 0.0f,
        0.0f, 0.0f, 0.0f, 1.0f
    };

    glMultMatrixf(m);
    glTranslatef(-eyeX, -eyeY, -eyeZ);
}

static float clamp01(float value) {
    if (value < 0.0f) {
        return 0.0f;
    }
    if (value > 1.0f) {
        return 1.0f;
    }
    return value;
}

static void setMaterial(const GLfloat ambient[4],
                        const GLfloat diffuse[4],
                        const GLfloat specular[4],
                        GLfloat shininess) {
    glMaterialfv(GL_FRONT_AND_BACK, GL_AMBIENT, ambient);
    glMaterialfv(GL_FRONT_AND_BACK, GL_DIFFUSE, diffuse);
    glMaterialfv(GL_FRONT_AND_BACK, GL_SPECULAR, specular);
    glMaterialf(GL_FRONT_AND_BACK, GL_SHININESS, shininess);
}

static void drawScaledCube(float sx, float sy, float sz) {
    glPushMatrix();
    glScalef(sx, sy, sz);
    glutSolidCube(1.0);
    glPopMatrix();
}

class Car {
public:
    enum Direction {
        FORWARD,
        RIGHT
    };

    Car() :
        m_x(-2.6f),
        m_y(-0.72f),
        m_z(-1.30f),
        m_speed(0.0f),
        m_wheelRotationDeg(0.0f),
        m_headingDeg(90.0f),
        m_direction(FORWARD) {
    }

    void resetInGarage() {
        m_x = -2.6f;
        m_y = -0.72f;
        m_z = -1.30f;
        m_speed = 0.0f;
        m_wheelRotationDeg = 0.0f;
        m_headingDeg = 90.0f;
        m_direction = FORWARD;
    }

    void setSpeed(float speedUnitsPerSec) {
        m_speed = speedUnitsPerSec;
    }

    void setDirection(Direction direction) {
        m_direction = direction;
    }

    void setHeading(float headingDeg) {
        m_headingDeg = headingDeg;
    }

    void update(float deltaTimeSec) {
        const float distance = m_speed * deltaTimeSec;
        if (m_direction == FORWARD) {
            m_z += distance;
        } else {
            m_x += distance;
        }

        const float wheelRadius = 0.23f;
        const float circumference = 2.0f * 3.14159265f * wheelRadius;
        if (circumference > 0.0f) {
            m_wheelRotationDeg -= (distance / circumference) * 360.0f;
        }
    }

    void render() const {
        const GLfloat metalAmb[] = {0.16f, 0.08f, 0.08f, 1.0f};
        const GLfloat metalDiff[] = {0.75f, 0.15f, 0.16f, 1.0f};
        const GLfloat metalSpec[] = {0.78f, 0.78f, 0.82f, 1.0f};
        setMaterial(metalAmb, metalDiff, metalSpec, 96.0f);

        glPushMatrix();
        glTranslatef(m_x, m_y, m_z);
        glRotatef(m_headingDeg, 0.0f, 1.0f, 0.0f);

        glPushMatrix();
        glTranslatef(0.0f, 0.23f, 0.0f);
        drawScaledCube(3.20f, 0.50f, 1.46f);
        glPopMatrix();

        glPushMatrix();
        glTranslatef(-0.18f, 0.60f, 0.0f);
        drawScaledCube(1.62f, 0.34f, 1.25f);
        glPopMatrix();

        glPushMatrix();
        glTranslatef(1.42f, 0.20f, 0.0f);
        glRotatef(18.0f, 0.0f, 0.0f, 1.0f);
        drawScaledCube(0.70f, 0.24f, 1.26f);
        glPopMatrix();

        glPushMatrix();
        glTranslatef(-1.45f, 0.24f, 0.0f);
        glRotatef(-11.0f, 0.0f, 0.0f, 1.0f);
        drawScaledCube(0.62f, 0.24f, 1.24f);
        glPopMatrix();

        const GLfloat glassAmb[] = {0.08f, 0.11f, 0.16f, 1.0f};
        const GLfloat glassDiff[] = {0.28f, 0.42f, 0.66f, 1.0f};
        const GLfloat glassSpec[] = {0.90f, 0.90f, 0.95f, 1.0f};
        setMaterial(glassAmb, glassDiff, glassSpec, 110.0f);

        glPushMatrix();
        glTranslatef(-0.18f, 0.64f, 0.0f);
        drawScaledCube(1.42f, 0.20f, 1.12f);
        glPopMatrix();

        const GLfloat trimAmb[] = {0.06f, 0.06f, 0.06f, 1.0f};
        const GLfloat trimDiff[] = {0.20f, 0.20f, 0.20f, 1.0f};
        const GLfloat trimSpec[] = {0.45f, 0.45f, 0.45f, 1.0f};
        setMaterial(trimAmb, trimDiff, trimSpec, 52.0f);

        glPushMatrix();
        glTranslatef(1.60f, 0.08f, 0.0f);
        drawScaledCube(0.12f, 0.34f, 1.18f);
        glPopMatrix();

        glPushMatrix();
        glTranslatef(-1.62f, 0.10f, 0.0f);
        drawScaledCube(0.10f, 0.28f, 1.10f);
        glPopMatrix();

        const GLfloat lightAmb[] = {0.35f, 0.35f, 0.30f, 1.0f};
        const GLfloat lightDiff[] = {0.90f, 0.90f, 0.70f, 1.0f};
        const GLfloat lightSpec[] = {0.95f, 0.95f, 0.80f, 1.0f};
        setMaterial(lightAmb, lightDiff, lightSpec, 120.0f);

        glPushMatrix();
        glTranslatef(1.78f, 0.10f, 0.48f);
        drawScaledCube(0.04f, 0.14f, 0.20f);
        glPopMatrix();

        glPushMatrix();
        glTranslatef(1.78f, 0.10f, -0.48f);
        drawScaledCube(0.04f, 0.14f, 0.20f);
        glPopMatrix();

        const GLfloat tailAmb[] = {0.22f, 0.03f, 0.03f, 1.0f};
        const GLfloat tailDiff[] = {0.74f, 0.08f, 0.08f, 1.0f};
        const GLfloat tailSpec[] = {0.50f, 0.18f, 0.18f, 1.0f};
        setMaterial(tailAmb, tailDiff, tailSpec, 72.0f);

        glPushMatrix();
        glTranslatef(-1.80f, 0.10f, 0.46f);
        drawScaledCube(0.03f, 0.13f, 0.18f);
        glPopMatrix();

        glPushMatrix();
        glTranslatef(-1.80f, 0.10f, -0.46f);
        drawScaledCube(0.03f, 0.13f, 0.18f);
        glPopMatrix();

        drawWheel(-1.12f, -0.11f, 0.68f);
        drawWheel(1.15f, -0.11f, 0.68f);
        drawWheel(-1.12f, -0.11f, -0.68f);
        drawWheel(1.15f, -0.11f, -0.68f);

        glPopMatrix();
    }

    float getZ() const {
        return m_z;
    }

    float getX() const {
        return m_x;
    }

private:
    void drawWheel(float x, float y, float z) const {
        const GLfloat tireAmb[] = {0.05f, 0.05f, 0.05f, 1.0f};
        const GLfloat tireDiff[] = {0.12f, 0.12f, 0.12f, 1.0f};
        const GLfloat tireSpec[] = {0.24f, 0.24f, 0.24f, 1.0f};
        setMaterial(tireAmb, tireDiff, tireSpec, 45.0f);

        glPushMatrix();
        glTranslatef(x, y, z);
        glRotatef(m_wheelRotationDeg, 0.0f, 0.0f, 1.0f);

        glutSolidTorus(0.07, 0.23, 18, 24);

        const GLfloat rimAmb[] = {0.35f, 0.35f, 0.35f, 1.0f};
        const GLfloat rimDiff[] = {0.72f, 0.72f, 0.74f, 1.0f};
        const GLfloat rimSpec[] = {0.92f, 0.92f, 0.95f, 1.0f};
        setMaterial(rimAmb, rimDiff, rimSpec, 120.0f);

        glPushMatrix();
        glTranslatef(0.0f, 0.0f, 0.04f);
        drawScaledCube(0.25f, 0.25f, 0.08f);
        glPopMatrix();

        const GLfloat spokeAmb[] = {0.30f, 0.30f, 0.30f, 1.0f};
        const GLfloat spokeDiff[] = {0.60f, 0.60f, 0.62f, 1.0f};
        const GLfloat spokeSpec[] = {0.86f, 0.86f, 0.88f, 1.0f};
        setMaterial(spokeAmb, spokeDiff, spokeSpec, 70.0f);
        for (int i = 0; i < 4; ++i) {
            glPushMatrix();
            glRotatef(i * 45.0f, 0.0f, 0.0f, 1.0f);
            glTranslatef(0.07f, 0.0f, 0.04f);
            drawScaledCube(0.12f, 0.02f, 0.03f);
            glPopMatrix();
        }
        glPopMatrix();
    }

    float m_x;
    float m_y;
    float m_z;
    float m_speed;
    float m_wheelRotationDeg;
    float m_headingDeg;
    Direction m_direction;
};

class Human {
public:
    Human() : m_x(2.4f), m_y(-0.62f), m_z(1.25f), m_walkSwingDeg(0.0f), m_visible(true) {
    }

    void resetAtDoor() {
        m_x = 2.4f;
        m_y = -0.62f;
        m_z = 1.25f;
        m_walkSwingDeg = 0.0f;
        m_visible = true;
    }

    void setPosition(float x, float y, float z) {
        m_x = x;
        m_y = y;
        m_z = z;
    }

    void setVisible(bool visible) {
        m_visible = visible;
    }

    void setWalkSwing(float walkSwingDeg) {
        m_walkSwingDeg = walkSwingDeg;
    }

    void render() const {
        if (!m_visible) {
            return;
        }

        glPushMatrix();
        glTranslatef(m_x, m_y, m_z);

        const GLfloat clothAmb[] = {0.10f, 0.16f, 0.24f, 1.0f};
        const GLfloat clothDiff[] = {0.18f, 0.32f, 0.62f, 1.0f};
        const GLfloat clothSpec[] = {0.10f, 0.10f, 0.10f, 1.0f};
        setMaterial(clothAmb, clothDiff, clothSpec, 20.0f);

        glPushMatrix();
        glTranslatef(0.0f, 0.56f, 0.0f);
        drawScaledCube(0.32f, 0.56f, 0.24f);
        glPopMatrix();

        const GLfloat pantsAmb[] = {0.06f, 0.06f, 0.08f, 1.0f};
        const GLfloat pantsDiff[] = {0.14f, 0.14f, 0.18f, 1.0f};
        const GLfloat pantsSpec[] = {0.08f, 0.08f, 0.10f, 1.0f};
        setMaterial(pantsAmb, pantsDiff, pantsSpec, 14.0f);

        glPushMatrix();
        glTranslatef(0.0f, 0.22f, 0.0f);
        drawScaledCube(0.34f, 0.24f, 0.24f);
        glPopMatrix();

        const GLfloat skinAmb[] = {0.25f, 0.18f, 0.14f, 1.0f};
        const GLfloat skinDiff[] = {0.75f, 0.58f, 0.45f, 1.0f};
        const GLfloat skinSpec[] = {0.18f, 0.14f, 0.12f, 1.0f};
        setMaterial(skinAmb, skinDiff, skinSpec, 18.0f);

        glPushMatrix();
        glTranslatef(0.0f, 0.88f, 0.0f);
        drawScaledCube(0.08f, 0.08f, 0.08f);
        glPopMatrix();

        glPushMatrix();
        glTranslatef(0.0f, 1.05f, 0.0f);
        glutSolidSphere(0.16, 18, 18);
        glPopMatrix();

        setMaterial(clothAmb, clothDiff, clothSpec, 20.0f);

        glPushMatrix();
        glTranslatef(-0.22f, 0.62f, 0.0f);
        glRotatef(m_walkSwingDeg, 1.0f, 0.0f, 0.0f);
        glTranslatef(0.0f, -0.18f, 0.0f);
        drawScaledCube(0.09f, 0.36f, 0.10f);
        glPopMatrix();

        glPushMatrix();
        glTranslatef(0.22f, 0.62f, 0.0f);
        glRotatef(-m_walkSwingDeg, 1.0f, 0.0f, 0.0f);
        glTranslatef(0.0f, -0.18f, 0.0f);
        drawScaledCube(0.09f, 0.36f, 0.10f);
        glPopMatrix();

        setMaterial(pantsAmb, pantsDiff, pantsSpec, 14.0f);

        glPushMatrix();
        glTranslatef(-0.10f, 0.02f, 0.0f);
        glRotatef(-m_walkSwingDeg, 1.0f, 0.0f, 0.0f);
        glTranslatef(0.0f, -0.20f, 0.0f);
        drawScaledCube(0.12f, 0.40f, 0.12f);
        glPopMatrix();

        glPushMatrix();
        glTranslatef(0.10f, 0.02f, 0.0f);
        glRotatef(m_walkSwingDeg, 1.0f, 0.0f, 0.0f);
        glTranslatef(0.0f, -0.20f, 0.0f);
        drawScaledCube(0.12f, 0.40f, 0.12f);
        glPopMatrix();

        const GLfloat shoeAmb[] = {0.02f, 0.02f, 0.02f, 1.0f};
        const GLfloat shoeDiff[] = {0.08f, 0.08f, 0.08f, 1.0f};
        const GLfloat shoeSpec[] = {0.05f, 0.05f, 0.05f, 1.0f};
        setMaterial(shoeAmb, shoeDiff, shoeSpec, 6.0f);

        glPushMatrix();
        glTranslatef(-0.10f, -0.40f, 0.05f);
        drawScaledCube(0.14f, 0.06f, 0.20f);
        glPopMatrix();

        glPushMatrix();
        glTranslatef(0.10f, -0.40f, 0.05f);
        drawScaledCube(0.14f, 0.06f, 0.20f);
        glPopMatrix();

        glPopMatrix();
    }

private:
    float m_x;
    float m_y;
    float m_z;
    float m_walkSwingDeg;
    bool m_visible;
};

class HomeEnvironment {
public:
    HomeEnvironment() :
        m_cloudOffsetX(0.0f),
        m_garageOpen(0.0f),
        m_phase(Phase_Walk),
        m_phaseTime(0.0f),
        m_morningSequenceDone(false) {
    }

    void resetMorningSequence() {
        m_car.resetInGarage();
        m_human.resetAtDoor();
        m_cloudOffsetX = 0.0f;
        m_garageOpen = 0.0f;
        m_phase = Phase_Walk;
        m_phaseTime = 0.0f;
        m_morningSequenceDone = false;
    }

    void update(float deltaTimeSec) {
        m_cloudOffsetX += deltaTimeSec * 0.65f;
        if (m_cloudOffsetX > 18.0f) {
            m_cloudOffsetX = -18.0f;
        }

        if (currentScene != 1 || m_morningSequenceDone) {
            return;
        }

        m_phaseTime += deltaTimeSec;

        if (m_phase == Phase_Walk) {
            const float walkDuration = 4.0f;
            const float t = clamp01(m_phaseTime / walkDuration);
            const float startX = 2.4f;
            const float startZ = 1.25f;
            const float endX = -2.6f;
            const float endZ = 1.10f;
            const float zLinear = startZ + (endZ - startZ) * t;
            const float forwardArc = 0.38f * std::sin(3.14159265f * t);
            m_human.setPosition(startX + (endX - startX) * t, -0.62f, zLinear + forwardArc);
            m_human.setWalkSwing(26.0f * std::sin(2.0f * 3.14159265f * t));

            if (m_phaseTime >= walkDuration) {
                m_phase = Phase_OpenGarage;
                m_phaseTime = 0.0f;
            }
        } else if (m_phase == Phase_OpenGarage) {
            const float openDuration = 2.2f;
            m_garageOpen = clamp01(m_phaseTime / openDuration);
            m_human.setWalkSwing(0.0f);
            if (m_phaseTime >= openDuration) {
                m_phase = Phase_EnterCar;
                m_phaseTime = 0.0f;
            }
        } else if (m_phase == Phase_EnterCar) {
            const float enterDuration = 1.7f;
            const float t = clamp01(m_phaseTime / enterDuration);
            const float startX = -2.6f;
            const float startZ = 1.10f;
            const float endX = -3.25f;
            const float endZ = -1.10f;
            m_human.setPosition(startX + (endX - startX) * t, -0.62f, startZ + (endZ - startZ) * t);
            m_human.setWalkSwing(10.0f * std::sin(2.0f * 3.14159265f * t));

            if (m_phaseTime > 1.2f) {
                m_human.setVisible(false);
            }

            if (m_phaseTime >= enterDuration) {
                m_phase = Phase_DriveForward;
                m_phaseTime = 0.0f;
                m_car.setDirection(Car::FORWARD);
                m_car.setHeading(90.0f);
                m_car.setSpeed(2.0f);
            }
        } else if (m_phase == Phase_DriveForward) {
            m_human.setWalkSwing(0.0f);
            m_car.update(deltaTimeSec);
            if (m_car.getZ() >= 4.0f) {
                m_phase = Phase_TurnRight;
                m_phaseTime = 0.0f;
                m_car.setDirection(Car::RIGHT);
                m_car.setSpeed(1.45f);
            }
        } else if (m_phase == Phase_TurnRight) {
            const float turnDuration = 1.1f;
            const float turnT = clamp01(m_phaseTime / turnDuration);
            m_car.setHeading(90.0f * (1.0f - turnT));
            m_car.update(deltaTimeSec);

            if (m_phaseTime >= turnDuration) {
                m_phase = Phase_DriveOnRoad;
                m_phaseTime = 0.0f;
                m_car.setHeading(0.0f);
                m_car.setSpeed(2.8f);
            }
        } else if (m_phase == Phase_DriveOnRoad) {
            m_car.update(deltaTimeSec);
            if (m_car.getX() >= 14.0f) {
                m_car.setSpeed(0.0f);
                m_phase = Phase_Done;
                m_phaseTime = 0.0f;
            }
        } else if (m_phase == Phase_Done) {
            m_morningSequenceDone = true;
        }
    }

    float getCarX() const {
        return m_car.getX();
    }

    float getCarZ() const {
        return m_car.getZ();
    }

    void renderScene() const {
        drawSkyMorning8AM();
        drawForegroundGrass();
        drawRearGrass();
        drawRoad();
        drawDriveway();
        drawHouseBody();
        drawGarage();
        drawTreeAndBushes();
        drawFence();

        if (currentScene == 1) {
            m_human.render();
            m_car.render();
        }
    }

private:
    enum SequencePhase {
        Phase_Walk,
        Phase_OpenGarage,
        Phase_EnterCar,
        Phase_DriveForward,
        Phase_TurnRight,
        Phase_DriveOnRoad,
        Phase_Done
    };

    void drawSkyMorning8AM() const {
        glDisable(GL_LIGHTING);

        glColor3f(1.0f, 0.90f, 0.50f);
        glPushMatrix();
        glTranslatef(-9.5f, 6.4f, -24.0f);
        glutSolidSphere(1.2, 20, 20);
        glPopMatrix();

        glColor3f(0.98f, 0.98f, 0.99f);
        drawCloud(-12.0f + m_cloudOffsetX, 5.0f, -18.0f);
        drawCloud(-5.5f + m_cloudOffsetX, 5.7f, -20.0f);
        drawCloud(0.8f + m_cloudOffsetX, 4.9f, -17.0f);
        drawCloud(8.0f + m_cloudOffsetX, 5.5f, -19.2f);

        glEnable(GL_LIGHTING);
    }

    void drawCloud(float x, float y, float z) const {
        glPushMatrix();
        glTranslatef(x, y, z);
        glutSolidSphere(0.55, 14, 14);
        glTranslatef(0.6f, 0.12f, 0.1f);
        glutSolidSphere(0.65, 14, 14);
        glTranslatef(-1.1f, -0.02f, 0.0f);
        glutSolidSphere(0.52, 14, 14);
        glPopMatrix();
    }

    void drawForegroundGrass() const {
        const GLfloat amb[] = {0.08f, 0.20f, 0.08f, 1.0f};
        const GLfloat diff[] = {0.16f, 0.45f, 0.16f, 1.0f};
        const GLfloat spec[] = {0.03f, 0.03f, 0.03f, 1.0f};
        setMaterial(amb, diff, spec, 6.0f);

        glPushMatrix();
        glTranslatef(0.0f, -1.22f, 9.0f);
        drawScaledCube(34.0f, 0.18f, 15.5f);
        glPopMatrix();
    }

    void drawRearGrass() const {
        const GLfloat amb[] = {0.08f, 0.20f, 0.08f, 1.0f};
        const GLfloat diff[] = {0.16f, 0.45f, 0.16f, 1.0f};
        const GLfloat spec[] = {0.03f, 0.03f, 0.03f, 1.0f};
        setMaterial(amb, diff, spec, 6.0f);

        glPushMatrix();
        glTranslatef(0.0f, -1.20f, -2.0f);
        drawScaledCube(34.0f, 0.14f, 9.0f);
        glPopMatrix();
    }

    void drawRoad() const {
        const GLfloat amb[] = {0.07f, 0.07f, 0.07f, 1.0f};
        const GLfloat diff[] = {0.16f, 0.16f, 0.16f, 1.0f};
        const GLfloat spec[] = {0.22f, 0.22f, 0.22f, 1.0f};
        setMaterial(amb, diff, spec, 26.0f);

        glPushMatrix();
        glTranslatef(0.0f, -1.14f, 4.2f);
        drawScaledCube(33.0f, 0.08f, 5.2f);
        glPopMatrix();

        const GLfloat curbAmb[] = {0.30f, 0.30f, 0.30f, 1.0f};
        const GLfloat curbDiff[] = {0.62f, 0.62f, 0.62f, 1.0f};
        const GLfloat curbSpec[] = {0.08f, 0.08f, 0.08f, 1.0f};
        setMaterial(curbAmb, curbDiff, curbSpec, 8.0f);

        glPushMatrix();
        glTranslatef(0.0f, -1.08f, 1.6f);
        drawScaledCube(33.0f, 0.06f, 0.16f);
        glPopMatrix();

        glPushMatrix();
        glTranslatef(0.0f, -1.08f, 6.8f);
        drawScaledCube(33.0f, 0.06f, 0.16f);
        glPopMatrix();

        const GLfloat dashAmb[] = {0.22f, 0.22f, 0.18f, 1.0f};
        const GLfloat dashDiff[] = {0.90f, 0.90f, 0.72f, 1.0f};
        const GLfloat dashSpec[] = {0.15f, 0.15f, 0.12f, 1.0f};
        setMaterial(dashAmb, dashDiff, dashSpec, 8.0f);

        for (int i = -8; i <= 8; ++i) {
            glPushMatrix();
            glTranslatef(i * 1.9f, -1.09f, 4.2f);
            drawScaledCube(1.25f, 0.01f, 0.14f);
            glPopMatrix();
        }
    }

    void drawDriveway() const {
        const GLfloat amb[] = {0.15f, 0.15f, 0.16f, 1.0f};
        const GLfloat diff[] = {0.34f, 0.34f, 0.36f, 1.0f};
        const GLfloat spec[] = {0.10f, 0.10f, 0.10f, 1.0f};
        setMaterial(amb, diff, spec, 14.0f);

        glPushMatrix();
        glTranslatef(-2.6f, -1.13f, 1.6f);
        drawScaledCube(2.8f, 0.05f, 3.0f);
        glPopMatrix();
    }

    void drawHouseBody() const {
        const GLfloat wallAmb[] = {0.32f, 0.27f, 0.22f, 1.0f};
        const GLfloat wallDiff[] = {0.68f, 0.58f, 0.48f, 1.0f};
        const GLfloat wallSpec[] = {0.08f, 0.08f, 0.08f, 1.0f};
        setMaterial(wallAmb, wallDiff, wallSpec, 10.0f);

        glPushMatrix();
        glTranslatef(2.6f, -1.00f, -1.1f);
        drawScaledCube(8.1f, 0.24f, 4.8f);
        glPopMatrix();

        glPushMatrix();
        glTranslatef(2.6f, -0.28f, -1.1f);
        drawScaledCube(8.0f, 1.65f, 4.6f);
        glPopMatrix();

        glPushMatrix();
        glTranslatef(2.6f, 1.46f, -1.1f);
        drawScaledCube(6.4f, 1.55f, 4.2f);
        glPopMatrix();

        const GLfloat roofAmb[] = {0.18f, 0.08f, 0.07f, 1.0f};
        const GLfloat roofDiff[] = {0.52f, 0.18f, 0.16f, 1.0f};
        const GLfloat roofSpec[] = {0.10f, 0.08f, 0.08f, 1.0f};
        setMaterial(roofAmb, roofDiff, roofSpec, 18.0f);

        glPushMatrix();
        glTranslatef(2.6f, 2.62f, -1.1f);
        glRotatef(45.0f, 0.0f, 0.0f, 1.0f);
        drawScaledCube(5.2f, 0.20f, 4.4f);
        glPopMatrix();

        glPushMatrix();
        glTranslatef(2.6f, 2.62f, -1.1f);
        glRotatef(-45.0f, 0.0f, 0.0f, 1.0f);
        drawScaledCube(5.2f, 0.20f, 4.4f);
        glPopMatrix();

        const GLfloat ridgeAmb[] = {0.14f, 0.06f, 0.05f, 1.0f};
        const GLfloat ridgeDiff[] = {0.46f, 0.14f, 0.12f, 1.0f};
        const GLfloat ridgeSpec[] = {0.10f, 0.08f, 0.08f, 1.0f};
        setMaterial(ridgeAmb, ridgeDiff, ridgeSpec, 20.0f);

        glPushMatrix();
        glTranslatef(2.6f, 3.65f, -1.1f);
        drawScaledCube(0.22f, 0.18f, 4.32f);
        glPopMatrix();

        glPushMatrix();
        glTranslatef(2.6f, 2.65f, 1.02f);
        drawScaledCube(6.9f, 1.05f, 0.05f);
        glPopMatrix();

        const GLfloat trimAmb[] = {0.24f, 0.20f, 0.16f, 1.0f};
        const GLfloat trimDiff[] = {0.76f, 0.68f, 0.58f, 1.0f};
        const GLfloat trimSpec[] = {0.12f, 0.12f, 0.12f, 1.0f};
        setMaterial(trimAmb, trimDiff, trimSpec, 14.0f);

        glPushMatrix();
        glTranslatef(2.6f, 0.35f, 1.22f);
        drawScaledCube(8.05f, 0.08f, 0.07f);
        glPopMatrix();

        drawFrontDoorAndCanopy();
        drawMainWindows();
    }

    void drawMainWindows() const {
        const GLfloat glassAmb[] = {0.10f, 0.14f, 0.16f, 1.0f};
        const GLfloat glassDiff[] = {0.30f, 0.45f, 0.52f, 1.0f};
        const GLfloat glassSpec[] = {0.80f, 0.85f, 0.90f, 1.0f};
        setMaterial(glassAmb, glassDiff, glassSpec, 92.0f);

        const float zFront = 1.21f;
        const float xList[4] = {0.9f, 4.3f, 0.9f, 4.3f};
        const float yList[4] = {1.45f, 1.45f, -0.10f, -0.10f};

        for (int i = 0; i < 4; ++i) {
            glPushMatrix();
            glTranslatef(xList[i], yList[i], zFront);
            drawScaledCube(1.05f, 0.82f, 0.08f);
            glPopMatrix();

            const GLfloat frameAmb[] = {0.20f, 0.18f, 0.15f, 1.0f};
            const GLfloat frameDiff[] = {0.52f, 0.44f, 0.34f, 1.0f};
            const GLfloat frameSpec[] = {0.08f, 0.08f, 0.08f, 1.0f};
            setMaterial(frameAmb, frameDiff, frameSpec, 8.0f);

            glPushMatrix();
            glTranslatef(xList[i], yList[i], zFront + 0.05f);
            drawScaledCube(0.08f, 0.82f, 0.03f);
            glPopMatrix();

            glPushMatrix();
            glTranslatef(xList[i], yList[i], zFront + 0.05f);
            drawScaledCube(1.05f, 0.08f, 0.03f);
            glPopMatrix();
        }
    }

    void drawFrontDoorAndCanopy() const {
        const GLfloat doorAmb[] = {0.16f, 0.10f, 0.06f, 1.0f};
        const GLfloat doorDiff[] = {0.45f, 0.28f, 0.15f, 1.0f};
        const GLfloat doorSpec[] = {0.12f, 0.10f, 0.08f, 1.0f};
        setMaterial(doorAmb, doorDiff, doorSpec, 20.0f);

        glPushMatrix();
        glTranslatef(2.5f, -0.25f, 1.22f);
        drawScaledCube(0.95f, 1.75f, 0.14f);
        glPopMatrix();

        const GLfloat canopyAmb[] = {0.22f, 0.11f, 0.09f, 1.0f};
        const GLfloat canopyDiff[] = {0.55f, 0.22f, 0.18f, 1.0f};
        const GLfloat canopySpec[] = {0.10f, 0.08f, 0.08f, 1.0f};
        setMaterial(canopyAmb, canopyDiff, canopySpec, 14.0f);

        glPushMatrix();
        glTranslatef(2.5f, 0.85f, 1.34f);
        glRotatef(45.0f, 0.0f, 0.0f, 1.0f);
        drawScaledCube(1.2f, 0.11f, 1.1f);
        glPopMatrix();

        glPushMatrix();
        glTranslatef(2.5f, 0.85f, 1.34f);
        glRotatef(-45.0f, 0.0f, 0.0f, 1.0f);
        drawScaledCube(1.2f, 0.11f, 1.1f);
        glPopMatrix();
    }

    void drawGarage() const {
        const GLfloat wallAmb[] = {0.30f, 0.26f, 0.22f, 1.0f};
        const GLfloat wallDiff[] = {0.64f, 0.56f, 0.48f, 1.0f};
        const GLfloat wallSpec[] = {0.08f, 0.08f, 0.08f, 1.0f};
        setMaterial(wallAmb, wallDiff, wallSpec, 8.0f);

        glPushMatrix();
        glTranslatef(-2.6f, -0.08f, -1.1f);
        drawScaledCube(3.4f, 2.35f, 4.4f);
        glPopMatrix();

        const GLfloat doorAmb[] = {0.24f, 0.24f, 0.25f, 1.0f};
        const GLfloat doorDiff[] = {0.78f, 0.78f, 0.80f, 1.0f};
        const GLfloat doorSpec[] = {0.20f, 0.20f, 0.20f, 1.0f};
        setMaterial(doorAmb, doorDiff, doorSpec, 24.0f);

        glPushMatrix();
        glTranslatef(-2.6f, -0.25f + m_garageOpen * 1.55f, 1.14f);
        drawScaledCube(2.55f, 1.75f, 0.08f);
        glPopMatrix();

        const GLfloat segAmb[] = {0.18f, 0.18f, 0.20f, 1.0f};
        const GLfloat segDiff[] = {0.52f, 0.52f, 0.55f, 1.0f};
        const GLfloat segSpec[] = {0.10f, 0.10f, 0.10f, 1.0f};
        setMaterial(segAmb, segDiff, segSpec, 8.0f);

        for (int i = -2; i <= 2; ++i) {
            glPushMatrix();
            glTranslatef(-2.6f, (-0.25f + m_garageOpen * 1.55f) + i * 0.37f, 1.18f);
            drawScaledCube(2.55f, 0.03f, 0.02f);
            glPopMatrix();
        }
    }

    void drawTreeAndBushes() const {
        const GLfloat trunkAmb[] = {0.20f, 0.12f, 0.06f, 1.0f};
        const GLfloat trunkDiff[] = {0.44f, 0.24f, 0.11f, 1.0f};
        const GLfloat trunkSpec[] = {0.05f, 0.04f, 0.03f, 1.0f};
        setMaterial(trunkAmb, trunkDiff, trunkSpec, 4.0f);

        glPushMatrix();
        glTranslatef(-6.5f, 1.20f, -0.6f);
        drawScaledCube(0.72f, 2.70f, 0.70f);
        glPopMatrix();

        const GLfloat leafAmb[] = {0.06f, 0.18f, 0.07f, 1.0f};
        const GLfloat leafDiff[] = {0.18f, 0.52f, 0.18f, 1.0f};
        const GLfloat leafSpec[] = {0.05f, 0.05f, 0.05f, 1.0f};
        setMaterial(leafAmb, leafDiff, leafSpec, 6.0f);

        const float cx[9] = {-6.5f, -5.9f, -7.2f, -6.5f, -6.0f, -7.0f, -6.3f, -6.8f, -6.2f};
        const float cy[9] = {2.6f, 2.2f, 2.2f, 1.9f, 2.9f, 2.9f, 3.1f, 2.6f, 2.4f};
        const float cz[9] = {-0.6f, -0.2f, -0.3f, -1.1f, -0.9f, -0.9f, -0.5f, -1.1f, -0.8f};
        const float rs[9] = {0.92f, 0.64f, 0.66f, 0.76f, 0.58f, 0.56f, 0.48f, 0.48f, 0.52f};

        for (int i = 0; i < 9; ++i) {
            if (i % 2 == 0) {
                const GLfloat leafAmbAlt[] = {0.05f, 0.15f, 0.06f, 1.0f};
                const GLfloat leafDiffAlt[] = {0.16f, 0.45f, 0.17f, 1.0f};
                const GLfloat leafSpecAlt[] = {0.05f, 0.05f, 0.05f, 1.0f};
                setMaterial(leafAmbAlt, leafDiffAlt, leafSpecAlt, 7.0f);
            } else {
                setMaterial(leafAmb, leafDiff, leafSpec, 6.0f);
            }
            glPushMatrix();
            glTranslatef(cx[i], cy[i], cz[i]);
            glutSolidSphere(rs[i], 20, 20);
            glPopMatrix();
        }

        const GLfloat bushAmb[] = {0.05f, 0.15f, 0.06f, 1.0f};
        const GLfloat bushDiff[] = {0.14f, 0.42f, 0.14f, 1.0f};
        const GLfloat bushSpec[] = {0.04f, 0.04f, 0.04f, 1.0f};
        setMaterial(bushAmb, bushDiff, bushSpec, 5.0f);

        glPushMatrix();
        glTranslatef(-5.1f, -0.55f, 0.7f);
        glutSolidSphere(0.65, 18, 18);
        glPopMatrix();

        glPushMatrix();
        glTranslatef(-5.9f, -0.58f, 0.5f);
        glutSolidSphere(0.56, 18, 18);
        glPopMatrix();
    }

    void drawFence() const {
        const GLfloat woodAmb[] = {0.16f, 0.12f, 0.08f, 1.0f};
        const GLfloat woodDiff[] = {0.56f, 0.42f, 0.25f, 1.0f};
        const GLfloat woodSpec[] = {0.06f, 0.06f, 0.05f, 1.0f};
        setMaterial(woodAmb, woodDiff, woodSpec, 7.0f);

        for (int i = 0; i < 32; ++i) {
            const float x = -11.2f + i * 0.72f;

            const bool doorwayGap = (x > 1.5f && x < 3.5f);
            const bool drivewayGap = (x > -3.8f && x < -1.5f);
            if (doorwayGap || drivewayGap) {
                continue;
            }

            glPushMatrix();
            glTranslatef(x, -0.26f, 1.2f);
            drawScaledCube(0.12f, 1.05f, 0.10f);
            glPopMatrix();

            glPushMatrix();
            glTranslatef(x, 0.30f, 1.2f);
            glRotatef(45.0f, 0.0f, 0.0f, 1.0f);
            drawScaledCube(0.12f, 0.18f, 0.10f);
            glPopMatrix();
        }

        glPushMatrix();
        glTranslatef(-7.8f, -0.06f, 1.2f);
        drawScaledCube(6.8f, 0.08f, 0.10f);
        glPopMatrix();

        glPushMatrix();
        glTranslatef(-7.8f, -0.40f, 1.2f);
        drawScaledCube(6.8f, 0.08f, 0.10f);
        glPopMatrix();

        glPushMatrix();
        glTranslatef(7.5f, -0.06f, 1.2f);
        drawScaledCube(7.0f, 0.08f, 0.10f);
        glPopMatrix();

        glPushMatrix();
        glTranslatef(7.5f, -0.40f, 1.2f);
        drawScaledCube(7.0f, 0.08f, 0.10f);
        glPopMatrix();
    }

    float m_cloudOffsetX;
    Car m_car;
    Human m_human;
    float m_garageOpen;
    SequencePhase m_phase;
    float m_phaseTime;
    bool m_morningSequenceDone;
};

static HomeEnvironment gHomeEnvironment;

static void setupSceneLighting() {
    const GLfloat morningAmbient[] = {0.42f, 0.42f, 0.45f, 1.0f};
    const GLfloat morningDiffuse[] = {0.95f, 0.92f, 0.88f, 1.0f};
    const GLfloat morningSpecular[] = {1.0f, 0.98f, 0.95f, 1.0f};
    const GLfloat morningPosition[] = {4.0f, 8.0f, 10.0f, 1.0f};

    const GLfloat nightAmbient[] = {0.07f, 0.08f, 0.12f, 1.0f};
    const GLfloat nightDiffuse[] = {0.20f, 0.22f, 0.30f, 1.0f};
    const GLfloat nightSpecular[] = {0.22f, 0.22f, 0.30f, 1.0f};
    const GLfloat nightPosition[] = {-4.0f, 5.0f, 6.0f, 1.0f};

    if (currentScene == 9) {
        glLightModelfv(GL_LIGHT_MODEL_AMBIENT, nightAmbient);
        glLightfv(GL_LIGHT0, GL_DIFFUSE, nightDiffuse);
        glLightfv(GL_LIGHT0, GL_SPECULAR, nightSpecular);
        glLightfv(GL_LIGHT0, GL_POSITION, nightPosition);
        glClearColor(0.03f, 0.04f, 0.08f, 1.0f);
    } else {
        glLightModelfv(GL_LIGHT_MODEL_AMBIENT, morningAmbient);
        glLightfv(GL_LIGHT0, GL_DIFFUSE, morningDiffuse);
        glLightfv(GL_LIGHT0, GL_SPECULAR, morningSpecular);
        glLightfv(GL_LIGHT0, GL_POSITION, morningPosition);
        glClearColor(0.56f, 0.78f, 0.94f, 1.0f);
    }
}

static void resize(int width, int height) {
    if (height <= 0) {
        height = 1;
    }
    gWindowWidth = width;
    gWindowHeight = height;

    glViewport(0, 0, width, height);
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    setPerspective(60.0f, static_cast<float>(width) / static_cast<float>(height), 0.1f, 200.0f);

    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
}

static void display() {
    setupSceneLighting();

    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();

    if (gFollowCamera && currentScene == 1) {
        const float carX = gHomeEnvironment.getCarX();
        const float carZ = gHomeEnvironment.getCarZ();

        const float targetCamX = carX - 3.8f;
        const float targetCamY = 2.6f;
        const float targetCamZ = carZ + 4.5f;

        if (!gCameraInitialized) {
            gSmoothCamX = targetCamX;
            gSmoothCamY = targetCamY;
            gSmoothCamZ = targetCamZ;
            gCameraInitialized = true;
        }

        const float smooth = 0.08f;
        gSmoothCamX += (targetCamX - gSmoothCamX) * smooth;
        gSmoothCamY += (targetCamY - gSmoothCamY) * smooth;
        gSmoothCamZ += (targetCamZ - gSmoothCamZ) * smooth;

        setLookAt(gSmoothCamX, gSmoothCamY, gSmoothCamZ,
              carX + 2.3f, 0.45f, carZ,
              0.0f, 1.0f, 0.0f);
    } else {
        gCameraInitialized = false;
        setLookAt(0.0f, 2.8f, 13.6f,
                  0.0f, 0.35f, -0.6f,
                  0.0f, 1.0f, 0.0f);
    }

    if (currentScene == 1 || currentScene == 9) {
        gHomeEnvironment.renderScene();
    } else {
        glDisable(GL_LIGHTING);
        glColor3f(0.15f, 0.18f, 0.24f);
        glPushMatrix();
        glTranslatef(0.0f, -0.5f, -1.5f);
        drawScaledCube(18.0f, 0.02f, 12.0f);
        glPopMatrix();
        glEnable(GL_LIGHTING);
    }

    glutSwapBuffers();
}

static void key(unsigned char key, int, int) {
    switch (key) {
        case 27:
        case 'q':
        case 'Q':
            std::exit(0);
            break;
        case '1':
            currentScene = 1;
            gHomeEnvironment.resetMorningSequence();
            gCameraInitialized = false;
            break;
        case '9':
            currentScene = 9;
            break;
        case 'r':
        case 'R':
            currentScene = 1;
            gHomeEnvironment.resetMorningSequence();
            gCameraInitialized = false;
            break;
        case 'c':
        case 'C':
            gFollowCamera = !gFollowCamera;
            gCameraInitialized = false;
            break;
        default:
            break;
    }

    glutPostRedisplay();
}

static void idle() {
    const double nowSec = glutGet(GLUT_ELAPSED_TIME) / 1000.0;
    float deltaTime = static_cast<float>(nowSec - gLastTimeSec);
    if (deltaTime < 0.0f) {
        deltaTime = 0.0f;
    }
    if (deltaTime > 0.1f) {
        deltaTime = 0.1f;
    }
    gLastTimeSec = nowSec;

    gHomeEnvironment.update(deltaTime);
    glutPostRedisplay();
}

int main(int argc, char* argv[]) {
    glutInit(&argc, argv);
    glutInitWindowSize(gWindowWidth, gWindowHeight);
    glutInitWindowPosition(80, 50);
    glutInitDisplayMode(GLUT_RGB | GLUT_DOUBLE | GLUT_DEPTH);

    glutCreateWindow("Home Module - Morning Departure Sequence");

    glutReshapeFunc(resize);
    glutDisplayFunc(display);
    glutKeyboardFunc(key);
    glutIdleFunc(idle);

    glEnable(GL_DEPTH_TEST);
    glDepthFunc(GL_LESS);
    glShadeModel(GL_SMOOTH);

    glEnable(GL_LIGHTING);
    glEnable(GL_LIGHT0);
    glEnable(GL_NORMALIZE);

    gHomeEnvironment.resetMorningSequence();
    gLastTimeSec = glutGet(GLUT_ELAPSED_TIME) / 1000.0;

    glutMainLoop();
    return EXIT_SUCCESS;
}
