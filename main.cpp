/* Life of An Office Worker In A Modern Urban Area */

#ifdef __APPLE__
#include <GLUT/glut.h>
#else
#include <GL/glut.h>
#endif

#include <cmath>
#include <cstdlib>
#include <vector>
#include <algorithm>
#include <utility>

static int gWindowWidth  = 1280;
static int gWindowHeight = 720;

static int  currentScene       = 1;
static double gLastTimeSec     = 0.0;
static bool gFollowCamera      = true;

static float gSmoothCamX       = 0.0f;
static float gSmoothCamY       = 2.8f;
static float gSmoothCamZ       = 13.6f;
static bool  gCameraInitialized = false;

static float degToRad(float degree) {
    return degree * 3.14159265f / 180.0f;
}

static void normalize3(float& x, float& y, float& z) {
    const float len = std::sqrt(x*x + y*y + z*z);
    if (len > 0.00001f) { x /= len; y /= len; z /= len; }
}

static void setPerspective(float fovYDeg, float aspect, float zNear, float zFar) {
    const float ymax = zNear * std::tan(degToRad(fovYDeg * 0.5f));
    const float xmax = ymax * aspect;
    glFrustum(-xmax, xmax, -ymax, ymax, zNear, zFar);
}

static void setLookAt(float eyeX, float eyeY, float eyeZ,
                      float centerX, float centerY, float centerZ,
                      float upX, float upY, float upZ) {
    float fx = centerX - eyeX, fy = centerY - eyeY, fz = centerZ - eyeZ;
    normalize3(fx, fy, fz);
    float sx = fy*upZ - fz*upY, sy = fz*upX - fx*upZ, sz = fx*upY - fy*upX;
    normalize3(sx, sy, sz);
    float ux = sy*fz - sz*fy, uy = sz*fx - sx*fz, uz = sx*fy - sy*fx;
    GLfloat m[16] = {
        sx, ux, -fx, 0.0f,
        sy, uy, -fy, 0.0f,
        sz, uz, -fz, 0.0f,
        0.0f, 0.0f, 0.0f, 1.0f
    };
    glMultMatrixf(m);
    glTranslatef(-eyeX, -eyeY, -eyeZ);
}

static float clamp01(float v) {
    return v < 0.0f ? 0.0f : (v > 1.0f ? 1.0f : v);
}

static void setMaterial(const GLfloat ambient[4], const GLfloat diffuse[4],
                        const GLfloat specular[4], GLfloat shininess) {
    glMaterialfv(GL_FRONT_AND_BACK, GL_AMBIENT,   ambient);
    glMaterialfv(GL_FRONT_AND_BACK, GL_DIFFUSE,   diffuse);
    glMaterialfv(GL_FRONT_AND_BACK, GL_SPECULAR,  specular);
    glMaterialf (GL_FRONT_AND_BACK, GL_SHININESS, shininess);
}

static void drawScaledCube(float sx, float sy, float sz) {
    glPushMatrix();
    glScalef(sx, sy, sz);
    glutSolidCube(1.0);
    glPopMatrix();
}

// ============================================================================
//  Car
// ============================================================================
class Car {
public:
    enum Direction { FORWARD, RIGHT };

    Car() : m_x(-2.6f), m_y(-0.72f), m_z(-1.30f),
            m_speed(0.0f), m_wheelRotationDeg(0.0f),
            m_headingDeg(90.0f), m_direction(FORWARD) {}

    void resetInGarage() {
        m_x = -2.6f; m_y = -0.72f; m_z = -1.30f;
        m_speed = 0.0f; m_wheelRotationDeg = 0.0f;
        m_headingDeg = 90.0f; m_direction = FORWARD;
    }
    void setSpeed(float s)       { m_speed = s; }
    void setDirection(Direction d){ m_direction = d; }
    void setHeading(float h)     { m_headingDeg = h; }

    void update(float dt) {
        const float dist = m_speed * dt;
        if (m_direction == FORWARD) m_z += dist;
        else                        m_x += dist;
        const float circ = 2.0f * 3.14159265f * 0.23f;
        if (circ > 0.0f) m_wheelRotationDeg -= (dist / circ) * 360.0f;
    }

    void render() const {
        const GLfloat mA[] = {0.16f,0.08f,0.08f,1.0f};
        const GLfloat mD[] = {0.75f,0.15f,0.16f,1.0f};
        const GLfloat mS[] = {0.78f,0.78f,0.82f,1.0f};
        setMaterial(mA,mD,mS,96.0f);

        glPushMatrix();
        glTranslatef(m_x, m_y, m_z);
        glRotatef(m_headingDeg, 0,1,0);

        glPushMatrix(); glTranslatef(0,0.23f,0); drawScaledCube(3.20f,0.50f,1.46f); glPopMatrix();
        glPushMatrix(); glTranslatef(-0.18f,0.60f,0); drawScaledCube(1.62f,0.34f,1.25f); glPopMatrix();
        glPushMatrix(); glTranslatef(1.42f,0.20f,0); glRotatef(18,0,0,1); drawScaledCube(0.70f,0.24f,1.26f); glPopMatrix();
        glPushMatrix(); glTranslatef(-1.45f,0.24f,0); glRotatef(-11,0,0,1); drawScaledCube(0.62f,0.24f,1.24f); glPopMatrix();

        const GLfloat gA[]={0.08f,0.11f,0.16f,1}; const GLfloat gD[]={0.28f,0.42f,0.66f,1}; const GLfloat gS[]={0.90f,0.90f,0.95f,1};
        setMaterial(gA,gD,gS,110.0f);
        glPushMatrix(); glTranslatef(-0.18f,0.64f,0); drawScaledCube(1.42f,0.20f,1.12f); glPopMatrix();

        const GLfloat tA[]={0.06f,0.06f,0.06f,1}; const GLfloat tD[]={0.20f,0.20f,0.20f,1}; const GLfloat tS[]={0.45f,0.45f,0.45f,1};
        setMaterial(tA,tD,tS,52.0f);
        glPushMatrix(); glTranslatef(1.60f,0.08f,0); drawScaledCube(0.12f,0.34f,1.18f); glPopMatrix();
        glPushMatrix(); glTranslatef(-1.62f,0.10f,0); drawScaledCube(0.10f,0.28f,1.10f); glPopMatrix();

        const GLfloat lA[]={0.35f,0.35f,0.30f,1}; const GLfloat lD[]={0.90f,0.90f,0.70f,1}; const GLfloat lS[]={0.95f,0.95f,0.80f,1};
        setMaterial(lA,lD,lS,120.0f);
        glPushMatrix(); glTranslatef(1.78f,0.10f,0.48f); drawScaledCube(0.04f,0.14f,0.20f); glPopMatrix();
        glPushMatrix(); glTranslatef(1.78f,0.10f,-0.48f); drawScaledCube(0.04f,0.14f,0.20f); glPopMatrix();

        const GLfloat rA[]={0.22f,0.03f,0.03f,1}; const GLfloat rD[]={0.74f,0.08f,0.08f,1}; const GLfloat rS[]={0.50f,0.18f,0.18f,1};
        setMaterial(rA,rD,rS,72.0f);
        glPushMatrix(); glTranslatef(-1.80f,0.10f,0.46f); drawScaledCube(0.03f,0.13f,0.18f); glPopMatrix();
        glPushMatrix(); glTranslatef(-1.80f,0.10f,-0.46f); drawScaledCube(0.03f,0.13f,0.18f); glPopMatrix();

        drawWheel(-1.12f,-0.11f, 0.68f);
        drawWheel( 1.15f,-0.11f, 0.68f);
        drawWheel(-1.12f,-0.11f,-0.68f);
        drawWheel( 1.15f,-0.11f,-0.68f);

        glPopMatrix();
    }

    float getZ() const { return m_z; }
    float getX() const { return m_x; }

private:
    void drawWheel(float x, float y, float z) const {
        const GLfloat tA[]={0.05f,0.05f,0.05f,1}; const GLfloat tD[]={0.12f,0.12f,0.12f,1}; const GLfloat tS[]={0.24f,0.24f,0.24f,1};
        setMaterial(tA,tD,tS,45.0f);
        glPushMatrix();
        glTranslatef(x,y,z);
        glRotatef(m_wheelRotationDeg,0,0,1);
        glutSolidTorus(0.07,0.23,18,24);
        const GLfloat rA[]={0.35f,0.35f,0.35f,1}; const GLfloat rD[]={0.72f,0.72f,0.74f,1}; const GLfloat rS[]={0.92f,0.92f,0.95f,1};
        setMaterial(rA,rD,rS,120.0f);
        glPushMatrix(); glTranslatef(0,0,0.04f); drawScaledCube(0.25f,0.25f,0.08f); glPopMatrix();
        const GLfloat sA[]={0.30f,0.30f,0.30f,1}; const GLfloat sD[]={0.60f,0.60f,0.62f,1}; const GLfloat sS[]={0.86f,0.86f,0.88f,1};
        setMaterial(sA,sD,sS,70.0f);
        for (int i = 0; i < 4; ++i) {
            glPushMatrix();
            glRotatef(i*45.0f,0,0,1);
            glTranslatef(0.07f,0,0.04f);
            drawScaledCube(0.12f,0.02f,0.03f);
            glPopMatrix();
        }
        glPopMatrix();
    }

    float m_x, m_y, m_z, m_speed, m_wheelRotationDeg, m_headingDeg;
    Direction m_direction;
};

// ============================================================================
//  Human  --  realistic 25-year-old formal-dressed office worker
//
//  Coordinate convention:
//    Local origin is BETWEEN THE FEET at ground level.
//    setPosition(x, groundY, z) places the shoe soles ON groundY.
//    Character faces +Z by default; use setFacing() to yaw.
//    Total height ≈ 1.80 units.
//
//  Bresenham midpoint circle algorithm (integer form, 8-way symmetry) is
//  used for: 2 jacket buttons, iris of each eye, pupil of each eye, watch
//  face -- all rendered as GL_TRIANGLE_FAN discs.
// ============================================================================
class Human {
public:
    Human();

    // Pose
    void setPosition(float x, float y, float z);
    void setFacing(float angleDeg);        // yaw around Y-axis
    void setVisible(bool v);

    // Animation
    void setWalkSwing(float degrees);      // ± degrees about X-axis
    void setWalkPhase(float phase01);      // drives swing via sine

    // Gestures / accessories
    void setArmRaise(float degrees);       // raise right arm forward
    void setBriefcaseVisible(bool show);

    // Render
    void render() const;

    float getX() const { return m_x; }
    float getY() const { return m_y; }
    float getZ() const { return m_z; }
    float getHeight() const { return 1.80f; }

private:
    // ---- Bresenham midpoint circle ----
    // d0 = 1-r, if d<0: d+=2x+3, else: d+=2(x-y)+5, y--; 8-way symmetry.
    static std::vector<std::pair<float,float> >
        computeBresenhamCircle(float radius, int qSteps);
    static void drawBresenhamDisc(float radius, int qSteps);

    // ---- Internal helpers ----
    static void applyMat(const GLfloat a[4], const GLfloat d[4],
                         const GLfloat s[4], GLfloat sh);
    static void box(float sx, float sy, float sz);

    // ---- Body-part renderers ----
    void renderLegsAndFeet()    const;
    void renderTorsoJacket()    const;
    void renderCollarAndLapels()const;
    void renderTie()            const;
    void renderJacketButtons()  const;   // Bresenham
    void renderLeftArm()        const;
    void renderRightArm()       const;   // Bresenham (watch face)
    void renderNeck()           const;
    void renderHead()           const;
    void renderHair()           const;
    void renderFacialFeatures() const;   // Bresenham (iris, pupil)

    float m_x, m_y, m_z;
    float m_facingDeg;
    float m_walkSwingDeg;
    float m_armRaiseDeg;
    bool  m_visible;
    bool  m_showBriefcase;
};

// ---- Constructor ----
Human::Human()
    : m_x(0.0f), m_y(0.0f), m_z(0.0f),
      m_facingDeg(0.0f), m_walkSwingDeg(0.0f), m_armRaiseDeg(0.0f),
      m_visible(true), m_showBriefcase(false) {}

void Human::setPosition(float x, float y, float z) { m_x=x; m_y=y; m_z=z; }
void Human::setFacing(float d)           { m_facingDeg    = d; }
void Human::setVisible(bool v)           { m_visible      = v; }
void Human::setWalkSwing(float d)        { m_walkSwingDeg = d; }
void Human::setArmRaise(float d)         { m_armRaiseDeg  = d; }
void Human::setBriefcaseVisible(bool b)  { m_showBriefcase= b; }

void Human::setWalkPhase(float p) {
    const float TAU = 6.28318530718f;
    float ph = p - std::floor(p);
    m_walkSwingDeg = 26.0f * std::sin(TAU * ph);
}

void Human::applyMat(const GLfloat a[4], const GLfloat d[4],
                     const GLfloat s[4], GLfloat sh) {
    glMaterialfv(GL_FRONT_AND_BACK, GL_AMBIENT,   a);
    glMaterialfv(GL_FRONT_AND_BACK, GL_DIFFUSE,   d);
    glMaterialfv(GL_FRONT_AND_BACK, GL_SPECULAR,  s);
    glMaterialf (GL_FRONT_AND_BACK, GL_SHININESS, sh);
}

void Human::box(float sx, float sy, float sz) {
    glPushMatrix();
    glScalef(sx, sy, sz);
    glutSolidCube(1.0);
    glPopMatrix();
}

// ---------------------------------------------------------------------------
//  Bresenham midpoint circle -- integer form, 8-way symmetry
// ---------------------------------------------------------------------------
std::vector<std::pair<float,float> >
Human::computeBresenhamCircle(float radius, int qSteps) {
    int r    = (qSteps < 2) ? 2 : qSteps;
    float unit = radius / static_cast<float>(r);

    std::vector<std::pair<int,int> > raw;
    int x = 0, y = r;
    int d = 1 - r;          // decision parameter d0

    while (x <= y) {
        raw.push_back(std::make_pair( x,  y));
        raw.push_back(std::make_pair( y,  x));
        raw.push_back(std::make_pair( y, -x));
        raw.push_back(std::make_pair( x, -y));
        raw.push_back(std::make_pair(-x, -y));
        raw.push_back(std::make_pair(-y, -x));
        raw.push_back(std::make_pair(-y,  x));
        raw.push_back(std::make_pair(-x,  y));

        if (d < 0) {
            d += 2*x + 3;
        } else {
            d += 2*(x - y) + 5;
            --y;
        }
        ++x;
    }

    std::vector<std::pair<float,float> > pts;
    pts.reserve(raw.size());
    for (size_t i = 0; i < raw.size(); ++i)
        pts.push_back(std::make_pair(raw[i].first * unit, raw[i].second * unit));

    std::sort(pts.begin(), pts.end(),
        [](const std::pair<float,float>& a, const std::pair<float,float>& b){
            return std::atan2(a.second, a.first) < std::atan2(b.second, b.first);
        });

    pts.erase(std::unique(pts.begin(), pts.end(),
        [](const std::pair<float,float>& a, const std::pair<float,float>& b){
            return std::fabs(a.first -b.first)<1e-5f && std::fabs(a.second-b.second)<1e-5f;
        }), pts.end());

    return pts;
}

void Human::drawBresenhamDisc(float radius, int qSteps) {
    std::vector<std::pair<float,float> > pts = computeBresenhamCircle(radius, qSteps);
    if (pts.size() < 3) return;
    glBegin(GL_TRIANGLE_FAN);
    glNormal3f(0,0,1);
    glVertex3f(0,0,0);
    for (size_t i = 0; i < pts.size(); ++i)
        glVertex3f(pts[i].first, pts[i].second, 0.0f);
    glVertex3f(pts.front().first, pts.front().second, 0.0f);
    glEnd();
}

// ---------------------------------------------------------------------------
//  render()
// ---------------------------------------------------------------------------
void Human::render() const {
    if (!m_visible) return;

    glPushMatrix();
    glTranslatef(m_x, m_y, m_z);
    glRotatef(m_facingDeg, 0,1,0);

    renderLegsAndFeet();
    renderTorsoJacket();
    renderCollarAndLapels();
    renderTie();
    renderJacketButtons();
    renderLeftArm();
    renderRightArm();
    renderNeck();
    renderHead();
    renderHair();
    renderFacialFeatures();

    glPopMatrix();
}

// ---------------------------------------------------------------------------
//  Legs -- navy dress trousers, dark socks, polished oxford shoes
// ---------------------------------------------------------------------------
void Human::renderLegsAndFeet() const {
    const GLfloat pA[]={0.030f,0.030f,0.060f,1}; const GLfloat pD[]={0.110f,0.120f,0.190f,1}; const GLfloat pS[]={0.080f,0.080f,0.100f,1};
    const GLfloat cA[]={0.020f,0.020f,0.040f,1}; const GLfloat cD[]={0.055f,0.065f,0.120f,1}; const GLfloat cS[]={0.030f,0.030f,0.040f,1};
    const GLfloat kA[]={0.04f,0.04f,0.05f,1};    const GLfloat kD[]={0.10f,0.10f,0.13f,1};    const GLfloat kS[]={0.03f,0.03f,0.04f,1};
    const GLfloat sA[]={0.020f,0.020f,0.020f,1}; const GLfloat sD[]={0.060f,0.050f,0.050f,1}; const GLfloat sS[]={0.620f,0.590f,0.560f,1};

    const float hipY = 0.96f;

    for (int i = 0; i < 2; ++i) {
        const int   side  = (i == 0) ? -1 : +1;
        const float swing = (side > 0) ? m_walkSwingDeg : -m_walkSwingDeg;

        glPushMatrix();
        glTranslatef(side * 0.095f, hipY, 0);
        glRotatef(swing, 1,0,0);

        // Thigh
        applyMat(pA,pD,pS, 10.0f);
        glPushMatrix(); glTranslatef(0,-0.24f,0); box(0.17f,0.48f,0.17f); glPopMatrix();

        // Calf (slight taper)
        glPushMatrix(); glTranslatef(0,-0.71f,0); box(0.15f,0.46f,0.15f); glPopMatrix();

        // Front crease line
        applyMat(cA,cD,cS, 4.0f);
        glPushMatrix(); glTranslatef(0,-0.48f,0.086f); box(0.005f,0.92f,0.004f); glPopMatrix();

        // Dark sock
        applyMat(kA,kD,kS, 6.0f);
        glPushMatrix(); glTranslatef(0,-0.95f,0); box(0.145f,0.06f,0.145f); glPopMatrix();

        // Oxford shoe body
        applyMat(sA,sD,sS, 90.0f);
        glPushMatrix(); glTranslatef(0,-0.985f,0.035f); box(0.155f,0.055f,0.24f); glPopMatrix();

        // Rounded toe cap
        glPushMatrix();
        glTranslatef(0,-0.985f,0.16f);
        glScalef(1.0f,0.65f,1.35f);
        glutSolidSphere(0.077,14,14);
        glPopMatrix();

        // Heel block
        glPushMatrix(); glTranslatef(0,-0.993f,-0.075f); box(0.13f,0.04f,0.08f); glPopMatrix();

        glPopMatrix();
    }
}

// ---------------------------------------------------------------------------
//  Torso -- navy wool suit jacket, shoulder pads, hem, back seam
// ---------------------------------------------------------------------------
void Human::renderTorsoJacket() const {
    const GLfloat jA[]={0.035f,0.035f,0.070f,1}; const GLfloat jD[]={0.115f,0.130f,0.200f,1}; const GLfloat jS[]={0.080f,0.080f,0.100f,1};
    applyMat(jA,jD,jS, 14.0f);

    // Chest
    glPushMatrix(); glTranslatef(0,1.37f,0); box(0.46f,0.26f,0.28f); glPopMatrix();
    // Waist (tapered)
    glPushMatrix(); glTranslatef(0,1.08f,0); box(0.42f,0.32f,0.26f); glPopMatrix();
    // Shoulder pads
    glPushMatrix(); glTranslatef(-0.22f,1.51f,0); box(0.12f,0.08f,0.26f); glPopMatrix();
    glPushMatrix(); glTranslatef( 0.22f,1.51f,0); box(0.12f,0.08f,0.26f); glPopMatrix();
    // Hem
    glPushMatrix(); glTranslatef(0,0.92f,0); box(0.44f,0.06f,0.27f); glPopMatrix();

    // Back seam (slightly darker)
    const GLfloat sA[]={0.020f,0.020f,0.050f,1}; const GLfloat sD[]={0.075f,0.085f,0.150f,1}; const GLfloat sS[]={0.060f,0.060f,0.080f,1};
    applyMat(sA,sD,sS, 8.0f);
    glPushMatrix(); glTranslatef(0,0.98f,-0.141f); box(0.010f,0.14f,0.003f); glPopMatrix();
}

// ---------------------------------------------------------------------------
//  Collar -- white shirt patch, collar points, navy lapels
// ---------------------------------------------------------------------------
void Human::renderCollarAndLapels() const {
    const GLfloat shA[]={0.46f,0.46f,0.46f,1}; const GLfloat shD[]={0.95f,0.95f,0.93f,1}; const GLfloat shS[]={0.32f,0.32f,0.32f,1};
    const GLfloat lA[] ={0.025f,0.025f,0.055f,1}; const GLfloat lD[]={0.090f,0.105f,0.170f,1}; const GLfloat lS[]={0.120f,0.120f,0.150f,1};

    // White shirt between lapels
    applyMat(shA,shD,shS, 18.0f);
    glPushMatrix(); glTranslatef(0,1.50f,0.142f); box(0.15f,0.10f,0.012f); glPopMatrix();

    // Shirt collar band
    glPushMatrix(); glTranslatef(0,1.575f,0.04f); box(0.17f,0.05f,0.20f); glPopMatrix();

    // Collar points
    glPushMatrix(); glTranslatef(-0.062f,1.545f,0.138f); glRotatef(-18,0,0,1); box(0.045f,0.090f,0.018f); glPopMatrix();
    glPushMatrix(); glTranslatef( 0.062f,1.545f,0.138f); glRotatef( 18,0,0,1); box(0.045f,0.090f,0.018f); glPopMatrix();

    // Lapels
    applyMat(lA,lD,lS, 22.0f);
    glPushMatrix(); glTranslatef(-0.11f,1.37f,0.141f); glRotatef(-9,0,0,1); box(0.11f,0.34f,0.016f); glPopMatrix();
    glPushMatrix(); glTranslatef( 0.11f,1.37f,0.141f); glRotatef( 9,0,0,1); box(0.11f,0.34f,0.016f); glPopMatrix();
}

// ---------------------------------------------------------------------------
//  Red silk tie (knot + body + wider tip)
// ---------------------------------------------------------------------------
void Human::renderTie() const {
    const GLfloat tA[]={0.22f,0.03f,0.04f,1}; const GLfloat tD[]={0.72f,0.11f,0.13f,1}; const GLfloat tS[]={0.50f,0.28f,0.28f,1};
    applyMat(tA,tD,tS, 55.0f);
    glPushMatrix(); glTranslatef(0,1.475f,0.152f); box(0.075f,0.065f,0.028f); glPopMatrix();  // knot
    glPushMatrix(); glTranslatef(0,1.27f, 0.154f); box(0.070f,0.360f,0.018f); glPopMatrix();  // body
    glPushMatrix(); glTranslatef(0,1.075f,0.155f); box(0.085f,0.060f,0.019f); glPopMatrix();  // tip
}

// ---------------------------------------------------------------------------
//  Jacket buttons -- Bresenham midpoint circle (GL_TRIANGLE_FAN disc)
// ---------------------------------------------------------------------------
void Human::renderJacketButtons() const {
    const GLfloat bA[]={0.06f,0.06f,0.06f,1}; const GLfloat bD[]={0.22f,0.22f,0.24f,1}; const GLfloat bS[]={0.60f,0.60f,0.62f,1};
    applyMat(bA,bD,bS, 80.0f);
    glPushMatrix(); glTranslatef(0.135f,1.16f,0.143f); drawBresenhamDisc(0.019f,7); glPopMatrix();
    glPushMatrix(); glTranslatef(0.135f,1.00f,0.143f); drawBresenhamDisc(0.019f,7); glPopMatrix();
}

// ---------------------------------------------------------------------------
//  Left arm -- jacket sleeve, shirt cuff, hand
// ---------------------------------------------------------------------------
void Human::renderLeftArm() const {
    const GLfloat jA[]={0.035f,0.035f,0.070f,1}; const GLfloat jD[]={0.115f,0.130f,0.200f,1}; const GLfloat jS[]={0.080f,0.080f,0.100f,1};
    const GLfloat cA[]={0.46f,0.46f,0.46f,1};    const GLfloat cD[]={0.95f,0.95f,0.93f,1};    const GLfloat cS[]={0.30f,0.30f,0.30f,1};
    const GLfloat sA[]={0.22f,0.15f,0.11f,1};    const GLfloat sD[]={0.78f,0.60f,0.45f,1};    const GLfloat sS[]={0.14f,0.12f,0.10f,1};

    glPushMatrix();
    glTranslatef(-0.27f,1.50f,0);
    glRotatef(m_walkSwingDeg, 1,0,0);  // opposite phase to right leg

    applyMat(jA,jD,jS, 14.0f);
    glPushMatrix(); glTranslatef(0,-0.18f,0); box(0.130f,0.36f,0.14f); glPopMatrix();
    glPushMatrix(); glTranslatef(0,-0.52f,0); box(0.120f,0.30f,0.13f); glPopMatrix();

    applyMat(cA,cD,cS, 18.0f);
    glPushMatrix(); glTranslatef(0,-0.685f,0); box(0.124f,0.04f,0.132f); glPopMatrix();

    applyMat(sA,sD,sS, 16.0f);
    glPushMatrix();
    glTranslatef(0,-0.76f,0);
    glScalef(0.9f,1.5f,0.65f);
    glutSolidSphere(0.070,16,16);
    glPopMatrix();

    glPopMatrix();
}

// ---------------------------------------------------------------------------
//  Right arm -- + wristwatch face (Bresenham), + optional briefcase
// ---------------------------------------------------------------------------
void Human::renderRightArm() const {
    const GLfloat jA[]={0.035f,0.035f,0.070f,1}; const GLfloat jD[]={0.115f,0.130f,0.200f,1}; const GLfloat jS[]={0.080f,0.080f,0.100f,1};
    const GLfloat cA[]={0.46f,0.46f,0.46f,1};    const GLfloat cD[]={0.95f,0.95f,0.93f,1};    const GLfloat cS[]={0.30f,0.30f,0.30f,1};
    const GLfloat sA[]={0.22f,0.15f,0.11f,1};    const GLfloat sD[]={0.78f,0.60f,0.45f,1};    const GLfloat sS[]={0.14f,0.12f,0.10f,1};

    glPushMatrix();
    glTranslatef(0.27f,1.50f,0);
    if (std::fabs(m_armRaiseDeg) > 0.1f)
        glRotatef(-m_armRaiseDeg, 1,0,0);
    else
        glRotatef(-m_walkSwingDeg, 1,0,0);

    applyMat(jA,jD,jS, 14.0f);
    glPushMatrix(); glTranslatef(0,-0.18f,0); box(0.130f,0.36f,0.14f); glPopMatrix();
    glPushMatrix(); glTranslatef(0,-0.52f,0); box(0.120f,0.30f,0.13f); glPopMatrix();

    applyMat(cA,cD,cS, 18.0f);
    glPushMatrix(); glTranslatef(0,-0.685f,0); box(0.124f,0.04f,0.132f); glPopMatrix();

    // Watch strap
    const GLfloat wA[]={0.05f,0.04f,0.03f,1}; const GLfloat wD[]={0.18f,0.13f,0.08f,1}; const GLfloat wS[]={0.10f,0.10f,0.10f,1};
    applyMat(wA,wD,wS, 20.0f);
    glPushMatrix(); glTranslatef(0,-0.71f,0.071f); box(0.110f,0.055f,0.008f); glPopMatrix();

    // Watch face -- Bresenham midpoint circle, polished metal
    const GLfloat fA[]={0.27f,0.27f,0.29f,1}; const GLfloat fD[]={0.82f,0.82f,0.86f,1}; const GLfloat fS[]={0.96f,0.96f,0.98f,1};
    applyMat(fA,fD,fS, 120.0f);
    glPushMatrix(); glTranslatef(0,-0.71f,0.079f); drawBresenhamDisc(0.028f,8); glPopMatrix();

    // Hand
    applyMat(sA,sD,sS, 16.0f);
    glPushMatrix();
    glTranslatef(0,-0.76f,0);
    glScalef(0.9f,1.5f,0.65f);
    glutSolidSphere(0.070,16,16);
    glPopMatrix();

    // Optional briefcase (follows arm)
    if (m_showBriefcase) {
        const GLfloat bA[]={0.09f,0.06f,0.04f,1}; const GLfloat bD[]={0.26f,0.17f,0.09f,1}; const GLfloat bS[]={0.12f,0.10f,0.08f,1};
        applyMat(bA,bD,bS, 20.0f);
        glPushMatrix(); glTranslatef(0,-0.98f,0); box(0.110f,0.22f,0.32f); glPopMatrix();
        const GLfloat mA[]={0.30f,0.30f,0.32f,1}; const GLfloat mD[]={0.72f,0.72f,0.74f,1}; const GLfloat mS[]={0.92f,0.92f,0.95f,1};
        applyMat(mA,mD,mS, 100.0f);
        glPushMatrix(); glTranslatef(0,-0.855f,0); box(0.018f,0.06f,0.10f); glPopMatrix();
        glPushMatrix(); glTranslatef( 0.058f,-0.935f,0.165f); box(0.024f,0.022f,0.008f); glPopMatrix();
        glPushMatrix(); glTranslatef(-0.058f,-0.935f,0.165f); box(0.024f,0.022f,0.008f); glPopMatrix();
    }

    glPopMatrix();
}

// ---------------------------------------------------------------------------
//  Neck
// ---------------------------------------------------------------------------
void Human::renderNeck() const {
    const GLfloat sA[]={0.22f,0.15f,0.11f,1}; const GLfloat sD[]={0.78f,0.60f,0.45f,1}; const GLfloat sS[]={0.14f,0.12f,0.10f,1};
    applyMat(sA,sD,sS, 14.0f);
    glPushMatrix(); glTranslatef(0,1.605f,0); box(0.095f,0.08f,0.095f); glPopMatrix();
}

// ---------------------------------------------------------------------------
//  Head -- cranium, jaw, ears
// ---------------------------------------------------------------------------
void Human::renderHead() const {
    const GLfloat sA[]={0.22f,0.15f,0.11f,1}; const GLfloat sD[]={0.78f,0.60f,0.45f,1}; const GLfloat sS[]={0.14f,0.12f,0.10f,1};
    applyMat(sA,sD,sS, 14.0f);

    // Cranium (elongated sphere for realistic oval head)
    glPushMatrix(); glTranslatef(0,1.75f,0); glScalef(1.0f,1.12f,1.02f); glutSolidSphere(0.105,28,28); glPopMatrix();
    // Jaw / chin
    glPushMatrix(); glTranslatef(0,1.684f,0.050f); glScalef(1.2f,0.8f,0.9f); glutSolidSphere(0.055,16,16); glPopMatrix();
    // Ears
    for (int i=-1; i<=1; i+=2) {
        glPushMatrix(); glTranslatef(i*0.102f,1.752f,-0.005f); glScalef(0.45f,1.0f,0.8f); glutSolidSphere(0.035,12,12); glPopMatrix();
    }
}

// ---------------------------------------------------------------------------
//  Hair -- short dark professional cut
// ---------------------------------------------------------------------------
void Human::renderHair() const {
    const GLfloat hA[]={0.04f,0.03f,0.02f,1}; const GLfloat hD[]={0.09f,0.07f,0.05f,1}; const GLfloat hS[]={0.24f,0.20f,0.18f,1};
    applyMat(hA,hD,hS, 40.0f);
    // Main cap
    glPushMatrix(); glTranslatef(0,1.81f,-0.012f); glScalef(1.02f,0.75f,1.07f); glutSolidSphere(0.108,24,24); glPopMatrix();
    // Side temples
    for (int i=-1; i<=1; i+=2) {
        glPushMatrix(); glTranslatef(i*0.084f,1.775f,0.020f); glScalef(0.45f,0.65f,0.95f); glutSolidSphere(0.052,14,14); glPopMatrix();
    }
    // Front fringe
    glPushMatrix(); glTranslatef(-0.028f,1.832f,0.075f); box(0.080f,0.018f,0.040f); glPopMatrix();
    glPushMatrix(); glTranslatef( 0.035f,1.828f,0.078f); glRotatef(-10,0,0,1); box(0.035f,0.025f,0.035f); glPopMatrix();
}

// ---------------------------------------------------------------------------
//  Facial features -- eyes (Bresenham iris + pupil), brows, nose, mouth
// ---------------------------------------------------------------------------
void Human::renderFacialFeatures() const {
    const float eyeY = 1.775f, eyeZ = 0.088f, eyeDX = 0.035f;

    // Eye whites
    const GLfloat wA[]={0.42f,0.42f,0.43f,1}; const GLfloat wD[]={0.93f,0.93f,0.90f,1}; const GLfloat wS[]={0.35f,0.35f,0.35f,1};
    applyMat(wA,wD,wS, 22.0f);
    for (int i=-1; i<=1; i+=2) {
        glPushMatrix(); glTranslatef(i*eyeDX,eyeY,eyeZ); glScalef(1.25f,0.75f,0.6f); glutSolidSphere(0.018,16,16); glPopMatrix();
    }

    // Iris -- Bresenham disc, dark brown
    const GLfloat iA[]={0.04f,0.025f,0.015f,1}; const GLfloat iD[]={0.13f,0.09f,0.05f,1}; const GLfloat iS[]={0.45f,0.40f,0.35f,1};
    applyMat(iA,iD,iS, 90.0f);
    for (int i=-1; i<=1; i+=2) {
        glPushMatrix(); glTranslatef(i*eyeDX,eyeY,eyeZ+0.010f); drawBresenhamDisc(0.011f,5); glPopMatrix();
    }

    // Pupil -- smaller Bresenham disc, near black
    const GLfloat pA[]={0.005f,0.005f,0.005f,1}; const GLfloat pD[]={0.020f,0.020f,0.020f,1}; const GLfloat pS[]={0.300f,0.300f,0.300f,1};
    applyMat(pA,pD,pS, 120.0f);
    for (int i=-1; i<=1; i+=2) {
        glPushMatrix(); glTranslatef(i*eyeDX,eyeY,eyeZ+0.012f); drawBresenhamDisc(0.005f,4); glPopMatrix();
    }

    // Eyebrows (arched)
    const GLfloat bA[]={0.03f,0.02f,0.01f,1}; const GLfloat bD[]={0.08f,0.06f,0.04f,1}; const GLfloat bS[]={0.05f,0.05f,0.05f,1};
    applyMat(bA,bD,bS, 12.0f);
    for (int i=-1; i<=1; i+=2) {
        glPushMatrix(); glTranslatef(i*0.036f,1.808f,0.093f); glRotatef(i*5.0f,0,0,1); box(0.044f,0.010f,0.012f); glPopMatrix();
    }

    // Nose
    const GLfloat sA[]={0.22f,0.15f,0.11f,1}; const GLfloat sD[]={0.78f,0.60f,0.45f,1}; const GLfloat sS[]={0.14f,0.12f,0.10f,1};
    applyMat(sA,sD,sS, 14.0f);
    glPushMatrix(); glTranslatef(0,1.748f,0.098f); box(0.018f,0.050f,0.028f); glPopMatrix();  // bridge
    glPushMatrix(); glTranslatef(0,1.722f,0.110f); glutSolidSphere(0.018,14,14); glPopMatrix(); // tip
    applyMat(bA,bD,bS, 6.0f);
    for (int i=-1; i<=1; i+=2) {
        glPushMatrix(); glTranslatef(i*0.009f,1.712f,0.114f); glutSolidSphere(0.0035,8,8); glPopMatrix();
    }

    // Lips
    const GLfloat lA[]={0.20f,0.09f,0.08f,1}; const GLfloat lD[]={0.55f,0.22f,0.20f,1}; const GLfloat lS[]={0.15f,0.10f,0.10f,1};
    applyMat(lA,lD,lS, 20.0f);
    glPushMatrix(); glTranslatef(0,1.685f,0.098f); box(0.044f,0.008f,0.012f); glPopMatrix();
}

// ============================================================================
//  HomeEnvironment
// ============================================================================

// Ground level for the human (shoe soles):
//   grass top  ≈ -1.13,  driveway top ≈ -1.105,  road top ≈ -1.10
// Using -1.10 works well for all walk/driveway/enter-car phases.
static const float kHumanGroundY = -1.10f;

class HomeEnvironment {
public:
    HomeEnvironment() :
        m_cloudOffsetX(0.0f),
        m_garageOpen(0.0f),
        m_phase(Phase_Walk),
        m_phaseTime(0.0f),
        m_morningSequenceDone(false) {}

    void resetMorningSequence() {
        m_car.resetInGarage();
        // New Human: set position, visibility and zero the swing
        m_human.setPosition(2.4f, kHumanGroundY, 1.25f);
        m_human.setFacing(-90.0f);        // faces -X (toward garage)
        m_human.setVisible(true);
        m_human.setWalkSwing(0.0f);
        m_human.setBriefcaseVisible(true);
        m_cloudOffsetX    = 0.0f;
        m_garageOpen      = 0.0f;
        m_phase           = Phase_Walk;
        m_phaseTime       = 0.0f;
        m_morningSequenceDone = false;
    }

    void update(float dt) {
        m_cloudOffsetX += dt * 0.65f;
        if (m_cloudOffsetX > 18.0f) m_cloudOffsetX = -18.0f;

        if (currentScene != 1 || m_morningSequenceDone) return;

        m_phaseTime += dt;

        if (m_phase == Phase_Walk) {
            const float walkDur = 4.0f;
            const float t   = clamp01(m_phaseTime / walkDur);
            const float startX = 2.4f,  startZ = 1.25f;
            const float endX   = -2.6f, endZ   = 1.10f;
            const float zLin   = startZ + (endZ - startZ) * t;
            const float arc    = 0.38f * std::sin(3.14159265f * t);
            m_human.setPosition(startX + (endX - startX)*t, kHumanGroundY, zLin + arc);
            m_human.setFacing(-90.0f);
            m_human.setWalkSwing(26.0f * std::sin(2.0f * 3.14159265f * t));
            if (m_phaseTime >= walkDur) { m_phase = Phase_OpenGarage; m_phaseTime = 0.0f; }
        }
        else if (m_phase == Phase_OpenGarage) {
            const float openDur = 2.2f;
            m_garageOpen = clamp01(m_phaseTime / openDur);
            m_human.setWalkSwing(0.0f);
            if (m_phaseTime >= openDur) { m_phase = Phase_EnterCar; m_phaseTime = 0.0f; }
        }
        else if (m_phase == Phase_EnterCar) {
            const float enterDur = 1.7f;
            const float t = clamp01(m_phaseTime / enterDur);
            const float startX = -2.6f, startZ = 1.10f;
            const float endX   = -3.25f, endZ  = -1.10f;
            m_human.setPosition(startX + (endX-startX)*t, kHumanGroundY, startZ + (endZ-startZ)*t);
            m_human.setFacing(-90.0f);
            m_human.setWalkSwing(10.0f * std::sin(2.0f * 3.14159265f * t));
            if (m_phaseTime > 1.2f) m_human.setVisible(false);
            if (m_phaseTime >= enterDur) {
                m_phase = Phase_DriveForward; m_phaseTime = 0.0f;
                m_car.setDirection(Car::FORWARD);
                m_car.setHeading(90.0f);
                m_car.setSpeed(2.0f);
            }
        }
        else if (m_phase == Phase_DriveForward) {
            m_human.setWalkSwing(0.0f);
            m_car.update(dt);
            if (m_car.getZ() >= 4.0f) {
                m_phase = Phase_TurnRight; m_phaseTime = 0.0f;
                m_car.setDirection(Car::RIGHT);
                m_car.setSpeed(1.45f);
            }
        }
        else if (m_phase == Phase_TurnRight) {
            const float turnDur = 1.1f;
            const float turnT   = clamp01(m_phaseTime / turnDur);
            m_car.setHeading(90.0f * (1.0f - turnT));
            m_car.update(dt);
            if (m_phaseTime >= turnDur) {
                m_phase = Phase_DriveOnRoad; m_phaseTime = 0.0f;
                m_car.setHeading(0.0f);
                m_car.setSpeed(2.8f);
            }
        }
        else if (m_phase == Phase_DriveOnRoad) {
            m_car.update(dt);
            if (m_car.getX() >= 14.0f) {
                m_car.setSpeed(0.0f);
                m_phase = Phase_Done; m_phaseTime = 0.0f;
            }
        }
        else if (m_phase == Phase_Done) {
            m_morningSequenceDone = true;
        }
    }

    float getCarX() const { return m_car.getX(); }
    float getCarZ() const { return m_car.getZ(); }

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
        Phase_Walk, Phase_OpenGarage, Phase_EnterCar,
        Phase_DriveForward, Phase_TurnRight, Phase_DriveOnRoad, Phase_Done
    };

    void drawSkyMorning8AM() const {
        glDisable(GL_LIGHTING);
        glColor3f(1.0f,0.90f,0.50f);
        glPushMatrix(); glTranslatef(-9.5f,6.4f,-24.0f); glutSolidSphere(1.2,20,20); glPopMatrix();
        glColor3f(0.98f,0.98f,0.99f);
        drawCloud(-12.0f+m_cloudOffsetX, 5.0f,-18.0f);
        drawCloud( -5.5f+m_cloudOffsetX, 5.7f,-20.0f);
        drawCloud(  0.8f+m_cloudOffsetX, 4.9f,-17.0f);
        drawCloud(  8.0f+m_cloudOffsetX, 5.5f,-19.2f);
        glEnable(GL_LIGHTING);
    }

    void drawCloud(float x, float y, float z) const {
        glPushMatrix(); glTranslatef(x,y,z);
        glutSolidSphere(0.55,14,14);
        glTranslatef(0.6f,0.12f,0.1f); glutSolidSphere(0.65,14,14);
        glTranslatef(-1.1f,-0.02f,0.0f); glutSolidSphere(0.52,14,14);
        glPopMatrix();
    }

    void drawForegroundGrass() const {
        const GLfloat a[]={0.08f,0.20f,0.08f,1}; const GLfloat d[]={0.16f,0.45f,0.16f,1}; const GLfloat s[]={0.03f,0.03f,0.03f,1};
        setMaterial(a,d,s,6.0f);
        glPushMatrix(); glTranslatef(0,-1.22f,9.0f); drawScaledCube(34.0f,0.18f,15.5f); glPopMatrix();
    }

    void drawRearGrass() const {
        const GLfloat a[]={0.08f,0.20f,0.08f,1}; const GLfloat d[]={0.16f,0.45f,0.16f,1}; const GLfloat s[]={0.03f,0.03f,0.03f,1};
        setMaterial(a,d,s,6.0f);
        glPushMatrix(); glTranslatef(0,-1.20f,-2.0f); drawScaledCube(34.0f,0.14f,9.0f); glPopMatrix();
    }

    void drawRoad() const {
        const GLfloat a[]={0.07f,0.07f,0.07f,1}; const GLfloat d[]={0.16f,0.16f,0.16f,1}; const GLfloat s[]={0.22f,0.22f,0.22f,1};
        setMaterial(a,d,s,26.0f);
        glPushMatrix(); glTranslatef(0,-1.14f,4.2f); drawScaledCube(33.0f,0.08f,5.2f); glPopMatrix();

        const GLfloat cA[]={0.30f,0.30f,0.30f,1}; const GLfloat cD[]={0.62f,0.62f,0.62f,1}; const GLfloat cS[]={0.08f,0.08f,0.08f,1};
        setMaterial(cA,cD,cS,8.0f);
        glPushMatrix(); glTranslatef(0,-1.08f,1.6f); drawScaledCube(33.0f,0.06f,0.16f); glPopMatrix();
        glPushMatrix(); glTranslatef(0,-1.08f,6.8f); drawScaledCube(33.0f,0.06f,0.16f); glPopMatrix();

        const GLfloat dA[]={0.22f,0.22f,0.18f,1}; const GLfloat dD[]={0.90f,0.90f,0.72f,1}; const GLfloat dS[]={0.15f,0.15f,0.12f,1};
        setMaterial(dA,dD,dS,8.0f);
        for (int i=-8; i<=8; ++i) {
            glPushMatrix(); glTranslatef(i*1.9f,-1.09f,4.2f); drawScaledCube(1.25f,0.01f,0.14f); glPopMatrix();
        }
    }

    void drawDriveway() const {
        const GLfloat a[]={0.15f,0.15f,0.16f,1}; const GLfloat d[]={0.34f,0.34f,0.36f,1}; const GLfloat s[]={0.10f,0.10f,0.10f,1};
        setMaterial(a,d,s,14.0f);
        glPushMatrix(); glTranslatef(-2.6f,-1.13f,1.6f); drawScaledCube(2.8f,0.05f,3.0f); glPopMatrix();
    }

    void drawHouseBody() const {
        const GLfloat wA[]={0.32f,0.27f,0.22f,1}; const GLfloat wD[]={0.68f,0.58f,0.48f,1}; const GLfloat wS[]={0.08f,0.08f,0.08f,1};
        setMaterial(wA,wD,wS,10.0f);
        glPushMatrix(); glTranslatef(2.6f,-1.00f,-1.1f); drawScaledCube(8.1f,0.24f,4.8f); glPopMatrix();
        glPushMatrix(); glTranslatef(2.6f,-0.28f,-1.1f); drawScaledCube(8.0f,1.65f,4.6f); glPopMatrix();
        glPushMatrix(); glTranslatef(2.6f, 1.46f,-1.1f); drawScaledCube(6.4f,1.55f,4.2f); glPopMatrix();

        const GLfloat rA[]={0.18f,0.08f,0.07f,1}; const GLfloat rD[]={0.52f,0.18f,0.16f,1}; const GLfloat rS[]={0.10f,0.08f,0.08f,1};
        setMaterial(rA,rD,rS,18.0f);
        glPushMatrix(); glTranslatef(2.6f,2.62f,-1.1f); glRotatef(45,0,0,1); drawScaledCube(5.2f,0.20f,4.4f); glPopMatrix();
        glPushMatrix(); glTranslatef(2.6f,2.62f,-1.1f); glRotatef(-45,0,0,1); drawScaledCube(5.2f,0.20f,4.4f); glPopMatrix();

        const GLfloat rgA[]={0.14f,0.06f,0.05f,1}; const GLfloat rgD[]={0.46f,0.14f,0.12f,1}; const GLfloat rgS[]={0.10f,0.08f,0.08f,1};
        setMaterial(rgA,rgD,rgS,20.0f);
        glPushMatrix(); glTranslatef(2.6f,3.65f,-1.1f); drawScaledCube(0.22f,0.18f,4.32f); glPopMatrix();
        glPushMatrix(); glTranslatef(2.6f,2.65f, 1.02f); drawScaledCube(6.9f,1.05f,0.05f); glPopMatrix();

        const GLfloat tA[]={0.24f,0.20f,0.16f,1}; const GLfloat tD[]={0.76f,0.68f,0.58f,1}; const GLfloat tS[]={0.12f,0.12f,0.12f,1};
        setMaterial(tA,tD,tS,14.0f);
        glPushMatrix(); glTranslatef(2.6f,0.35f,1.22f); drawScaledCube(8.05f,0.08f,0.07f); glPopMatrix();

        drawFrontDoorAndCanopy();
        drawMainWindows();
    }

    void drawMainWindows() const {
        const GLfloat gA[]={0.10f,0.14f,0.16f,1}; const GLfloat gD[]={0.30f,0.45f,0.52f,1}; const GLfloat gS[]={0.80f,0.85f,0.90f,1};
        setMaterial(gA,gD,gS,92.0f);
        const float zF=1.21f;
        const float xL[4]={0.9f,4.3f,0.9f,4.3f};
        const float yL[4]={1.45f,1.45f,-0.10f,-0.10f};
        for (int i=0; i<4; ++i) {
            glPushMatrix(); glTranslatef(xL[i],yL[i],zF); drawScaledCube(1.05f,0.82f,0.08f); glPopMatrix();
            const GLfloat fA[]={0.20f,0.18f,0.15f,1}; const GLfloat fD[]={0.52f,0.44f,0.34f,1}; const GLfloat fS[]={0.08f,0.08f,0.08f,1};
            setMaterial(fA,fD,fS,8.0f);
            glPushMatrix(); glTranslatef(xL[i],yL[i],zF+0.05f); drawScaledCube(0.08f,0.82f,0.03f); glPopMatrix();
            glPushMatrix(); glTranslatef(xL[i],yL[i],zF+0.05f); drawScaledCube(1.05f,0.08f,0.03f); glPopMatrix();
        }
    }

    void drawFrontDoorAndCanopy() const {
        const GLfloat dA[]={0.16f,0.10f,0.06f,1}; const GLfloat dD[]={0.45f,0.28f,0.15f,1}; const GLfloat dS[]={0.12f,0.10f,0.08f,1};
        setMaterial(dA,dD,dS,20.0f);
        glPushMatrix(); glTranslatef(2.5f,-0.25f,1.22f); drawScaledCube(0.95f,1.75f,0.14f); glPopMatrix();
        const GLfloat cA[]={0.22f,0.11f,0.09f,1}; const GLfloat cD[]={0.55f,0.22f,0.18f,1}; const GLfloat cS[]={0.10f,0.08f,0.08f,1};
        setMaterial(cA,cD,cS,14.0f);
        glPushMatrix(); glTranslatef(2.5f,0.85f,1.34f); glRotatef( 45,0,0,1); drawScaledCube(1.2f,0.11f,1.1f); glPopMatrix();
        glPushMatrix(); glTranslatef(2.5f,0.85f,1.34f); glRotatef(-45,0,0,1); drawScaledCube(1.2f,0.11f,1.1f); glPopMatrix();
    }

    void drawGarage() const {
        const GLfloat wA[]={0.30f,0.26f,0.22f,1}; const GLfloat wD[]={0.64f,0.56f,0.48f,1}; const GLfloat wS[]={0.08f,0.08f,0.08f,1};
        setMaterial(wA,wD,wS,8.0f);
        glPushMatrix(); glTranslatef(-2.6f,-0.08f,-1.1f); drawScaledCube(3.4f,2.35f,4.4f); glPopMatrix();

        const GLfloat dA[]={0.24f,0.24f,0.25f,1}; const GLfloat dD[]={0.78f,0.78f,0.80f,1}; const GLfloat dS[]={0.20f,0.20f,0.20f,1};
        setMaterial(dA,dD,dS,24.0f);
        glPushMatrix(); glTranslatef(-2.6f,-0.25f+m_garageOpen*1.55f,1.14f); drawScaledCube(2.55f,1.75f,0.08f); glPopMatrix();

        const GLfloat sA[]={0.18f,0.18f,0.20f,1}; const GLfloat sD[]={0.52f,0.52f,0.55f,1}; const GLfloat sS[]={0.10f,0.10f,0.10f,1};
        setMaterial(sA,sD,sS,8.0f);
        for (int i=-2; i<=2; ++i) {
            glPushMatrix(); glTranslatef(-2.6f,(-0.25f+m_garageOpen*1.55f)+i*0.37f,1.18f); drawScaledCube(2.55f,0.03f,0.02f); glPopMatrix();
        }
    }

    void drawTreeAndBushes() const {
        const GLfloat tA[]={0.20f,0.12f,0.06f,1}; const GLfloat tD[]={0.44f,0.24f,0.11f,1}; const GLfloat tS[]={0.05f,0.04f,0.03f,1};
        setMaterial(tA,tD,tS,4.0f);
        glPushMatrix(); glTranslatef(-6.5f,1.20f,-0.6f); drawScaledCube(0.72f,2.70f,0.70f); glPopMatrix();

        const GLfloat lA[]={0.06f,0.18f,0.07f,1}; const GLfloat lD[]={0.18f,0.52f,0.18f,1}; const GLfloat lS[]={0.05f,0.05f,0.05f,1};
        setMaterial(lA,lD,lS,6.0f);
        const float cx[9]={-6.5f,-5.9f,-7.2f,-6.5f,-6.0f,-7.0f,-6.3f,-6.8f,-6.2f};
        const float cy[9]={ 2.6f, 2.2f, 2.2f, 1.9f, 2.9f, 2.9f, 3.1f, 2.6f, 2.4f};
        const float cz[9]={-0.6f,-0.2f,-0.3f,-1.1f,-0.9f,-0.9f,-0.5f,-1.1f,-0.8f};
        const float rs[9]={ 0.92f,0.64f,0.66f,0.76f,0.58f,0.56f,0.48f,0.48f,0.52f};
        for (int i=0; i<9; ++i) {
            if (i%2==0) { const GLfloat a2[]={0.05f,0.15f,0.06f,1}; const GLfloat d2[]={0.16f,0.45f,0.17f,1}; const GLfloat s2[]={0.05f,0.05f,0.05f,1}; setMaterial(a2,d2,s2,7.0f); }
            else setMaterial(lA,lD,lS,6.0f);
            glPushMatrix(); glTranslatef(cx[i],cy[i],cz[i]); glutSolidSphere(rs[i],20,20); glPopMatrix();
        }
        const GLfloat bA[]={0.05f,0.15f,0.06f,1}; const GLfloat bD[]={0.14f,0.42f,0.14f,1}; const GLfloat bS[]={0.04f,0.04f,0.04f,1};
        setMaterial(bA,bD,bS,5.0f);
        glPushMatrix(); glTranslatef(-5.1f,-0.55f,0.7f); glutSolidSphere(0.65,18,18); glPopMatrix();
        glPushMatrix(); glTranslatef(-5.9f,-0.58f,0.5f); glutSolidSphere(0.56,18,18); glPopMatrix();
    }

    void drawFence() const {
        const GLfloat wA[]={0.16f,0.12f,0.08f,1}; const GLfloat wD[]={0.56f,0.42f,0.25f,1}; const GLfloat wS[]={0.06f,0.06f,0.05f,1};
        setMaterial(wA,wD,wS,7.0f);
        for (int i=0; i<32; ++i) {
            const float x=-11.2f+i*0.72f;
            if ((x>1.5f && x<3.5f)||(x>-3.8f && x<-1.5f)) continue;
            glPushMatrix(); glTranslatef(x,-0.26f,1.2f); drawScaledCube(0.12f,1.05f,0.10f); glPopMatrix();
            glPushMatrix(); glTranslatef(x, 0.30f,1.2f); glRotatef(45,0,0,1); drawScaledCube(0.12f,0.18f,0.10f); glPopMatrix();
        }
        glPushMatrix(); glTranslatef(-7.8f,-0.06f,1.2f); drawScaledCube(6.8f,0.08f,0.10f); glPopMatrix();
        glPushMatrix(); glTranslatef(-7.8f,-0.40f,1.2f); drawScaledCube(6.8f,0.08f,0.10f); glPopMatrix();
        glPushMatrix(); glTranslatef( 7.5f,-0.06f,1.2f); drawScaledCube(7.0f,0.08f,0.10f); glPopMatrix();
        glPushMatrix(); glTranslatef( 7.5f,-0.40f,1.2f); drawScaledCube(7.0f,0.08f,0.10f); glPopMatrix();
    }

    float        m_cloudOffsetX;
    Car          m_car;
    Human        m_human;
    float        m_garageOpen;
    SequencePhase m_phase;
    float        m_phaseTime;
    bool         m_morningSequenceDone;
};

// ============================================================================
//  Globals / GLUT callbacks
// ============================================================================
static HomeEnvironment gHomeEnvironment;

static void setupSceneLighting() {
    const GLfloat morningAmb[] ={0.42f,0.42f,0.45f,1}; const GLfloat morningDif[]={0.95f,0.92f,0.88f,1};
    const GLfloat morningSpc[] ={1.0f,0.98f,0.95f,1};  const GLfloat morningPos[]={4.0f,8.0f,10.0f,1};
    const GLfloat nightAmb[]   ={0.07f,0.08f,0.12f,1}; const GLfloat nightDif[]  ={0.20f,0.22f,0.30f,1};
    const GLfloat nightSpc[]   ={0.22f,0.22f,0.30f,1}; const GLfloat nightPos[]  ={-4.0f,5.0f,6.0f,1};

    if (currentScene == 9) {
        glLightModelfv(GL_LIGHT_MODEL_AMBIENT, nightAmb);
        glLightfv(GL_LIGHT0, GL_DIFFUSE,  nightDif);
        glLightfv(GL_LIGHT0, GL_SPECULAR, nightSpc);
        glLightfv(GL_LIGHT0, GL_POSITION, nightPos);
        glClearColor(0.03f,0.04f,0.08f,1);
    } else {
        glLightModelfv(GL_LIGHT_MODEL_AMBIENT, morningAmb);
        glLightfv(GL_LIGHT0, GL_DIFFUSE,  morningDif);
        glLightfv(GL_LIGHT0, GL_SPECULAR, morningSpc);
        glLightfv(GL_LIGHT0, GL_POSITION, morningPos);
        glClearColor(0.56f,0.78f,0.94f,1);
    }
}

static void resize(int w, int h) {
    if (h <= 0) h = 1;
    gWindowWidth = w; gWindowHeight = h;
    glViewport(0,0,w,h);
    glMatrixMode(GL_PROJECTION); glLoadIdentity();
    setPerspective(60.0f, (float)w/(float)h, 0.1f, 200.0f);
    glMatrixMode(GL_MODELVIEW); glLoadIdentity();
}

static void display() {
    setupSceneLighting();
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glMatrixMode(GL_MODELVIEW); glLoadIdentity();

    if (gFollowCamera && currentScene == 1) {
        const float carX = gHomeEnvironment.getCarX();
        const float carZ = gHomeEnvironment.getCarZ();
        const float tX = carX - 3.8f, tY = 2.6f, tZ = carZ + 4.5f;
        if (!gCameraInitialized) { gSmoothCamX=tX; gSmoothCamY=tY; gSmoothCamZ=tZ; gCameraInitialized=true; }
        const float k = 0.08f;
        gSmoothCamX += (tX - gSmoothCamX)*k;
        gSmoothCamY += (tY - gSmoothCamY)*k;
        gSmoothCamZ += (tZ - gSmoothCamZ)*k;
        setLookAt(gSmoothCamX,gSmoothCamY,gSmoothCamZ, carX+2.3f,0.45f,carZ, 0,1,0);
    } else {
        gCameraInitialized = false;
        setLookAt(0,2.8f,13.6f, 0,0.35f,-0.6f, 0,1,0);
    }

    if (currentScene==1 || currentScene==9) {
        gHomeEnvironment.renderScene();
    } else {
        glDisable(GL_LIGHTING);
        glColor3f(0.15f,0.18f,0.24f);
        glPushMatrix(); glTranslatef(0,-0.5f,-1.5f); drawScaledCube(18.0f,0.02f,12.0f); glPopMatrix();
        glEnable(GL_LIGHTING);
    }
    glutSwapBuffers();
}

static void key(unsigned char k, int, int) {
    switch (k) {
        case 27: case 'q': case 'Q': std::exit(0); break;
        case '1':
            currentScene = 1;
            gHomeEnvironment.resetMorningSequence();
            gCameraInitialized = false;
            break;
        case '9': currentScene = 9; break;
        case 'r': case 'R':
            currentScene = 1;
            gHomeEnvironment.resetMorningSequence();
            gCameraInitialized = false;
            break;
        case 'c': case 'C':
            gFollowCamera = !gFollowCamera;
            gCameraInitialized = false;
            break;
        default: break;
    }
    glutPostRedisplay();
}

static void idle() {
    const double now = glutGet(GLUT_ELAPSED_TIME) / 1000.0;
    float dt = (float)(now - gLastTimeSec);
    if (dt < 0.0f) dt = 0.0f;
    if (dt > 0.1f) dt = 0.1f;
    gLastTimeSec = now;
    gHomeEnvironment.update(dt);
    glutPostRedisplay();
}

int main(int argc, char* argv[]) {
    glutInit(&argc, argv);
    glutInitWindowSize(gWindowWidth, gWindowHeight);
    glutInitWindowPosition(80, 50);
    glutInitDisplayMode(GLUT_RGB | GLUT_DOUBLE | GLUT_DEPTH);
    glutCreateWindow("Life of An Office Worker In A Modern Urban Area");
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
