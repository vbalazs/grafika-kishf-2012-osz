//=============================================================================================
// Szamitogepes grafika hazi feladat keret. Ervenyes 2012-tol.          
// A //~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// sorokon beluli reszben celszeru garazdalkodni, mert a tobbit ugyis toroljuk. 
// A beadott program csak ebben a fajlban lehet, a fajl 1 byte-os ASCII karaktereket tartalmazhat. 
// Tilos:
// - mast "beincludolni", illetve mas konyvtarat hasznalni
// - faljmuveleteket vegezni (printf is fajlmuvelet!)
// - new operatort hivni az onInitialization f√ºggv√©nyt kiv√©ve, a lefoglalt adat korrekt felszabad√≠t√°sa n√©lk√ºl 
// - felesleges programsorokat a beadott programban hagyni
// - tovabbi kommenteket a beadott programba irni a forrasmegjelolest kommentjeit kiveve
// ---------------------------------------------------------------------------------------------
// A feladatot ANSI C++ nyelvu forditoprogrammal ellenorizzuk, a Visual Studio-hoz kepesti elteresekrol
// es a leggyakoribb hibakrol (pl. ideiglenes objektumot nem lehet referencia tipusnak ertekul adni)
// a hazibeado portal ad egy osszefoglalot.
// ---------------------------------------------------------------------------------------------
// A feladatmegoldasokban csak olyan gl/glu/glut fuggvenyek hasznalhatok, amelyek
// 1. Az oran a feladatkiadasig elhangzottak ES (logikai AND muvelet)
// 2. Az alabbi listaban szerepelnek:  
// Rendering pass: glBegin, glVertex[2|3]f, glColor3f, glNormal3f, glTexCoord2f, glEnd, glDrawPixels
// Transzformaciok: glViewport, glMatrixMode, glLoadIdentity, glMultMatrixf, gluOrtho2D, 
// glTranslatef, glRotatef, glScalef, gluLookAt, gluPerspective, glPushMatrix, glPopMatrix,
// Illuminacio: glMaterialfv, glMaterialfv, glMaterialf, glLightfv
// Texturazas: glGenTextures, glBindTexture, glTexParameteri, glTexImage2D, glTexEnvi, 
// Pipeline vezerles: glShadeModel, glEnable/Disable a kovetkezokre:
// GL_LIGHTING, GL_NORMALIZE, GL_DEPTH_TEST, GL_CULL_FACE, GL_TEXTURE_2D, GL_BLEND, GL_LIGHT[0..7]
//
// NYILATKOZAT
// ---------------------------------------------------------------------------------------------
// Nev    : VARGA BALAZS
// Neptun : IFAW8V
// ---------------------------------------------------------------------------------------------
// ezennel kijelentem, hogy a feladatot magam keszitettem, es ha barmilyen segitseget igenybe vettem vagy 
// mas szellemi termeket felhasznaltam, akkor a forrast es az atvett reszt kommentekben egyertelmuen jeloltem. 
// A forrasmegjeloles kotelme vonatkozik az eloadas foliakat es a targy oktatoi, illetve a 
// grafhazi doktor tanacsait kiveve barmilyen csatornan (szoban, irasban, Interneten, stb.) erkezo minden egyeb 
// informaciora (keplet, program, algoritmus, stb.). Kijelentem, hogy a forrasmegjelolessel atvett reszeket is ertem, 
// azok helyessegere matematikai bizonyitast tudok adni. Tisztaban vagyok azzal, hogy az atvett reszek nem szamitanak
// a sajat kontribucioba, igy a feladat elfogadasarol a tobbi resz mennyisege es minosege alapjan szuletik dontes.  
// Tudomasul veszem, hogy a forrasmegjeloles kotelmenek megsertese eseten a hazifeladatra adhato pontokat 
// negativ elojellel szamoljak el es ezzel parhuzamosan eljaras is indul velem szemben.
//=============================================================================================

#include <math.h>
#include <stdlib.h>

#include <GL/gl.h>
#include <GL/glu.h>
#include <GL/glut.h>

//torlendo
#include <iostream>
using namespace std;
//
// <editor-fold defaultstate="collapsed" desc="osztalyok, strukturak">

const double d = 0.02;

struct Vector {
    float x, y, z;

    Vector() {
        x = y = z = 0;
    }

    Vector(float x0, float y0, float z0 = 0) {
        x = x0;
        y = y0;
        z = z0;
    }

    static Vector X() {
        return Vector(1.0f, 0.0f, 0.0f);
    }

    static Vector Y() {
        return Vector(0.0f, 1.0f, 0.0f);
    }

    static Vector Z() {
        return Vector(0.0f, 0.0f, 1.0f);
    }

    Vector operator*(float a) {
        return Vector(x * a, y * a, z * a);
    }

    Vector operator+(const Vector & v) {
        return Vector(x + v.x, y + v.y, z + v.z);
    }

    Vector operator-(const Vector & v) {
        return Vector(x - v.x, y - v.y, z - v.z);
    }

    float operator*(const Vector & v) { // dot product
        return (x * v.x + y * v.y + z * v.z);
    }

    Vector operator%(const Vector & v) { // cross product
        return Vector(y * v.z - z * v.y, z * v.x - x * v.z, x * v.y - y * v.x);
    }

    float Length() {
        return sqrt(x * x + y * y + z * z);
    }

    Vector getNormalized() {
        return (*this) * (1.0 / Length());
    }

    void dump() {
        cout << "x,y,z=" << x << "; " << y << "; " << z << endl;
    }
};

struct Color {
    float r, g, b;

    Color() {
        r = g = b = 0;
    }

    Color(float r0, float g0, float b0) {
        r = r0;
        g = g0;
        b = b0;
    }

    Color operator*(float a) {
        return Color(r * a, g * a, b * a);
    }

    Color operator*(const Color & c) {
        return Color(r * c.r, g * c.g, b * c.b);
    }

    Color operator+(const Color & c) {
        return Color(r + c.r, g + c.g, b + c.b);
    }
};

/*
 * forras: https://github.com/tmichel/Grafika-hf-2011-osz/blob/master/khf4.cpp#L889
 */
class Camera {
public:
    Vector pos, dir, up;

    Camera() {
        pos = dir = Vector(0.0, 0.0, 0.0);
        up = Vector::Y();
    }

    void lookAt() {
        gluLookAt(pos.x, pos.y, pos.z, dir.x, dir.y, dir.z, up.x, up.y, up.z);
    }

    void dump() {
        cout << "pos=" << pos.x << "; " << pos.y << "; " << pos.z << " || ";
        cout << "dir=" << dir.x << "; " << dir.y << "; " << dir.z << " || ";
        cout << "up=" << up.x << "; " << up.y << "; " << up.z << endl;
    }
};

/*
 * Forras alapjan (mÛdosÌtva): Szirmay-Kalos L·szlÛ, 2001. November.
 */
struct Quaternion {
    double s;
    Vector d;
public:

    Quaternion() : d(0, 0, 0) {
        s = 1;
    }

    Quaternion(double s0, double x0, double y0, double z0) : d(x0, y0, z0) {
        s = s0;
    }

    Quaternion(double s0, Vector d0) : d(d0) {
        s = s0;
    }

    Quaternion operator+(Quaternion q) {
        return Quaternion(s + q.s, d + q.d);
    }

    Quaternion operator*(double f) {
        return Quaternion(s * f, d * f);
    }

    double operator*(Quaternion q) {
        return (s * s + d * d);
    }

    void Normalize() {
        float length = (*this) * (*this);
        (*this) = (*this) * (1 / sqrt(length));
    }

    Quaternion operator%(Quaternion q) {
        return Quaternion(s * q.s - d * q.d, q.d * s + d * q.s + d % q.d);
    }

    float GetRotationAngle() {
        float cosa2 = s;
        float sina2 = d.Length();
        float angRad = atan2(sina2, cosa2) * 2;
        return angRad * 180 / M_PI;
    }

    Vector GetAxis() {
        return d;
    }

    /*
     * Keplet forr·sa:
     * http://www.mathworks.com/help/aeroblks/quaternioninverse.html
     */
    Quaternion getInversed() {
        Quaternion num = Quaternion(s, (d * -1));
        const float denom = pow(s, 2) + pow(d.x, 2) + pow(d.y, 2) + pow(d.z, 2);

        return (num * (1 / denom));
    }
};

class Bullet {
public:
    Vector pos, dir;
    bool use;
    int bounced;

    Bullet() {
    }

    void draw() {
        if (!use) {
            return;
        }

        const GLfloat diff[] = {1.0, 0.08, 0.57, 1.0};
        const GLfloat spec[] = {1.0, 0.08, 0.57, 1.0};
        const GLfloat amb[] = {0.1, 0.1, 0.1, 1.0};

        glMaterialfv(GL_FRONT, GL_AMBIENT, amb);
        glMaterialfv(GL_FRONT, GL_DIFFUSE, diff);
        glMaterialfv(GL_FRONT, GL_SPECULAR, spec);
        glMaterialf(GL_FRONT, GL_SHININESS, 30.0);

        glPushMatrix();
        glTranslatef(pos.x, pos.y, pos.z);
        glScalef(0.1, 0.1, 0.1);
        /*
         * Forras: https://github.com/tmichel/Grafika-hf-2011-osz/blob/master/khf4.cpp#L297
         */
        glBegin(GL_QUADS);
        for (float u = 0.0f; u < 1.0f; u += d) {
            for (float v = 0.0f; v < 1.0f; v += d) {
                float x = cos(2 * M_PI * u) * sin(M_PI * v);
                float y = sin(2 * M_PI * u) * sin(M_PI * v);
                float z = cos(M_PI * v);
                glNormal3f(x, y, z);
                glVertex3f(x, y, z);

                x = cos(2 * M_PI * (u)) * sin(M_PI * (v + d));
                y = sin(2 * M_PI * (u)) * sin(M_PI * (v + d));
                z = cos(M_PI * (v + d));
                glNormal3f(x, y, z);
                glVertex3f(x, y, z);

                x = cos(2 * M_PI * (u + d)) * sin(M_PI * (v + d));
                y = sin(2 * M_PI * (u + d)) * sin(M_PI * (v + d));
                z = cos(M_PI * (v + d));
                glNormal3f(x, y, z);
                glVertex3f(x, y, z);

                x = cos(2 * M_PI * (u + d)) * sin(M_PI * v);
                y = sin(2 * M_PI * (u + d)) * sin(M_PI * v);
                z = cos(M_PI * v);
                glNormal3f(x, y, z);
                glVertex3f(x, y, z);
            }
        }
        glEnd();
        glPopMatrix();
    }
};
// </editor-fold>

const int screenWidth = 600;
const int screenHeight = 600;

Color image[screenWidth*screenHeight];

Vector sunPos;
Quaternion globalQuat = Quaternion();
Vector chopperDirection(0, 0, -1);
Vector chopperPosition(0, 0, 0);

Vector arrowAxisOfRot;
const double rotation = 20 * (M_PI / 180.0) / 2.0;

int shotCounter = 0;
const int AMMO = 10;
Bullet bullets[AMMO];
const double BOUNCINESS = 0.9;

Camera cam;
Vector camBallSpeed(0, 0, 0);
const double CAM_BALL_AIR_RES = 0.01;
const double CAM_SPRING_K = 0.001;
const double CAM_SPRING_LENGTH = 1;
const double CAM_SPRING_MAX_LENGTH = 2;
const Vector CAM_UP_AND_GRAV_FORCE = Vector(0, 0.002, 0);

unsigned int fieldTexture;
long glut_elapsed_time = 0;
long timeTmp = 0;
double mainRotorDeg = 25;
double tailRotorDeg = 25;

const int FIELD_WIDTH = 30;
const int FIELD_LONG = 50;

void drawCuboid(double a, double b, double height) {
    glBegin(GL_QUADS);
    double d_a = a / 2.0;

    //bottom
    glNormal3f(0, -1, 0);
    glVertex3f(-d_a, 0, 0);

    glNormal3f(0, -1, 0);
    glVertex3f(-d_a, 0, -b);

    glNormal3f(0, -1, 0);
    glVertex3f(d_a, 0, -b);

    glNormal3f(0, -1, 0);
    glVertex3f(d_a, 0, 0);

    //top
    glNormal3f(0, 1, 0);
    glVertex3f(-d_a, height, 0);

    glNormal3f(0, 1, 0);
    glVertex3f(-d_a, height, -b);

    glNormal3f(0, 1, 0);
    glVertex3f(d_a, height, -b);

    glNormal3f(0, 1, 0);
    glVertex3f(d_a, height, 0);

    //back
    glNormal3f(0, 0, -1);
    glVertex3f(-d_a, 0, -b);

    glNormal3f(0, 0, -1);
    glVertex3f(-d_a, height, -b);

    glNormal3f(0, 0, -1);
    glVertex3f(d_a, height, -b);

    glNormal3f(0, 0, -1);
    glVertex3f(d_a, 0, -b);

    //front
    glNormal3f(0, 0, 1);
    glVertex3f(-d_a, 0, 0);

    glNormal3f(0, 0, 1);
    glVertex3f(-d_a, height, 0);

    glNormal3f(0, 0, 1);
    glVertex3f(d_a, height, 0);

    glNormal3f(0, 0, 1);
    glVertex3f(d_a, 0, 0);

    //right
    glNormal3f(1, 0, 0);
    glVertex3f(d_a, 0, 0);

    glNormal3f(1, 0, 0);
    glVertex3f(d_a, 0, -b);

    glNormal3f(1, 0, 0);
    glVertex3f(d_a, height, -b);

    glNormal3f(1, 0, 0);
    glVertex3f(d_a, height, 0);

    //left
    glNormal3f(-1, 0, 0);
    glVertex3f(-d_a, 0, 0);

    glNormal3f(-1, 0, 0);
    glVertex3f(-d_a, height, 0);

    glNormal3f(-1, 0, 0);
    glVertex3f(-d_a, height, -b);

    glNormal3f(-1, 0, 0);
    glVertex3f(-d_a, 0, -b);

    glEnd();
}

void setChopperKhakiColor() {
    const GLfloat diff[] = {0.623529, 0.623529, 0.372549, 1.0};
    const GLfloat spec[] = {0.623529, 0.623529, 0.372549, 1.0};
    const GLfloat amb[] = {0.1, 0.1, 0.1, 1.0};

    glMaterialfv(GL_FRONT, GL_AMBIENT, amb);
    glMaterialfv(GL_FRONT, GL_DIFFUSE, diff);
    glMaterialfv(GL_FRONT, GL_SPECULAR, spec);
    glMaterialf(GL_FRONT, GL_SHININESS, 80);
}

void setChopperRotorColor() {
    const GLfloat diff[] = {0.6, 0.6, 0.6, 1.0};
    const GLfloat spec[] = {0.6, 0.6, 0.6, 1.0};
    const GLfloat amb[] = {0.1, 0.1, 0.1, 1.0};

    glMaterialfv(GL_FRONT, GL_AMBIENT, amb);
    glMaterialfv(GL_FRONT, GL_DIFFUSE, diff);
    glMaterialfv(GL_FRONT, GL_SPECULAR, spec);
    glMaterialf(GL_FRONT, GL_SHININESS, 30.0);
}

void drawCylinder(double r, double height) {
    const int stacks = 40;
    glBegin(GL_TRIANGLE_STRIP);
    for (int i = 0; i <= stacks; i++) {
        float rad = (2 * M_PI * (i % stacks)) / stacks;
        float x = r * sin(rad);
        float z = r * cos(rad);
        glNormal3f(sin(rad), 0, cos(rad));
        glVertex3f(x, 0, z);
        glVertex3f(x, height, z);
    }
    glEnd();
}

void drawQuatArrow() {
    int stacks = 40;
    float height = 3.9 * fabs(globalQuat.s);
    float r_cylinder = 0.02;
    float r_cone = r_cylinder + 0.05;

    const GLfloat diff[] = {0.9, 0.0, 0.0, 1.0};
    const GLfloat spec[] = {0.9, 0.0, 0.0, 1.0};
    const GLfloat amb[] = {0.0, 0.0, 0.0, 0.0};

    glMaterialfv(GL_FRONT, GL_AMBIENT, amb);
    glMaterialfv(GL_FRONT, GL_DIFFUSE, diff);
    glMaterialfv(GL_FRONT, GL_SPECULAR, spec);
    glMaterialf(GL_FRONT, GL_SHININESS, 80);

    glPushMatrix();
    glRotatef(180, arrowAxisOfRot.x, arrowAxisOfRot.y, arrowAxisOfRot.z);

    //henger palast
    drawCylinder(r_cylinder, height);

    //kup alapja
    glBegin(GL_TRIANGLE_FAN);
    glNormal3f(0, 1, 0);
    glVertex3f(0, height, 0);
    for (int i = 0; i <= stacks; i++) {
        float rad = (2 * M_PI * (i % stacks)) / stacks;
        glNormal3f(0, -1, 0);
        glVertex3f(r_cone * sin(rad), height, r_cone * cos(rad));
    }
    glEnd();

    // kup pal·st
    glBegin(GL_TRIANGLE_FAN);
    glNormal3f(1, 1, 1);
    glVertex3f(0, height + r_cone, 0);
    for (int i = 0; i <= stacks; i++) {
        float rad = (2 * M_PI * (i % stacks)) / stacks;
        glNormal3f(sin(rad), 0, cos(rad));
        glVertex3f(r_cone * sin(rad), height, r_cone * cos(rad));
    }
    glEnd();

    glPopMatrix();
}

void drawChopperTail() {
    setChopperKhakiColor();

    int stacks = 40;
    float height = 1;
    float r = 0.06;

    glPushMatrix();
    glTranslatef(0, 0, 0.6);
    glRotatef(90, 1, 0, 0);

    // pal·st
    drawCylinder(r, height);

    //h·tsÛ fedolap
    glNormal3f(0, 1, 0);
    glBegin(GL_TRIANGLE_FAN);
    glVertex3f(0, height, 0);
    for (int i = 0; i <= stacks; i++) {
        float rad = (2 * M_PI * (i % stacks)) / stacks;
        glVertex3f(r * sin(rad), height, r * cos(rad));
    }
    glEnd();

    glPopMatrix();
}

void drawMainRotor() {
    setChopperRotorColor();

    glPushMatrix();
    glTranslatef(0, 0.3, 0.12);
    glRotatef(mainRotorDeg, 0, 1, 0);
    drawCuboid(0.1, 1.0, 0.02);
    glPopMatrix();

    glPushMatrix();
    glTranslatef(0, 0.3, 0.12);
    glRotatef(mainRotorDeg, 0, 1, 0);
    drawCuboid(0.1, -1.0, 0.02);
    glPopMatrix();
}

void drawTailRotor() {
    setChopperRotorColor();

    glPushMatrix();
    glTranslatef(-0.06, 0, 1.55);
    glRotatef(90, 0, 0, 1);
    glRotatef(tailRotorDeg, 0, 1, 0);

    glPushMatrix();
    glTranslatef(0, 0, 0.03);
    drawCuboid(0.02, 0.3, 0.01);
    glPopMatrix();

    glPushMatrix();
    glRotatef(135, 0, 1, 0);
    drawCuboid(0.02, 0.3, 0.01);
    glPopMatrix();

    glPushMatrix();
    glRotatef(225, 0, 1, 0);
    drawCuboid(0.02, 0.3, 0.01);
    glPopMatrix();

    glPopMatrix();

}

void drawChopper() {
    setChopperKhakiColor();

    glPushMatrix();
    glTranslatef(chopperPosition.x, chopperPosition.y, chopperPosition.z);

    //TODO: tˆrˆlni
    glPushMatrix();
    glBegin(GL_LINES);
    glVertex3f(0, 0, 0);
    glVertex3f(chopperDirection.x, chopperDirection.y, chopperDirection.z);
    glEnd();
    glPopMatrix();
    //eddig

    glPushMatrix();
    glRotatef(globalQuat.GetRotationAngle(), globalQuat.GetAxis().x, globalQuat.GetAxis().y, globalQuat.GetAxis().z);

    const float ellipsoid_a = 0.4;
    const float ellipsoid_b = 0.6;
    const float ellipsoid_c = 0.3;

    const float step1 = (M_PI) / 60.0;
    const float step2 = (M_PI) / 50.0;

    glPushMatrix();
    glRotatef(90, 1, 0, 0);
    glRotatef(180, 0, 0, 1);
    glBegin(GL_TRIANGLE_STRIP);
    for (float t = -M_PI / 2; t <= (M_PI / 2) + 0.0001; t += step1) {
        if (t <= M_PI && t >= -M_PI / 2) {
            for (float s = -M_PI; s <= M_PI + 0.0001; s += step2) {
                if (s <= 0) {
                    glNormal3f(ellipsoid_a * cos(t) * cos(s), ellipsoid_b * cos(t) * sin(s), ellipsoid_c * sin(t));
                    glVertex3f(ellipsoid_a * cos(t) * cos(s), ellipsoid_b * cos(t) * sin(s), ellipsoid_c * sin(t));

                    glNormal3f(ellipsoid_a * cos(t + step1) * cos(s), ellipsoid_b * cos(t + step1) * sin(s), ellipsoid_c * sin(t + step1));
                    glVertex3f(ellipsoid_a * cos(t + step1) * cos(s), ellipsoid_b * cos(t + step1) * sin(s), ellipsoid_c * sin(t + step1));
                }
            }
        }

        if (t >= 0) {
            for (float s = -M_PI; s <= M_PI + 0.0001; s += step2) {
                if (s >= -0.1) {
                    glNormal3f(ellipsoid_a * cos(t) * cos(s), ellipsoid_b * cos(t) * sin(s), ellipsoid_c * sin(t));
                    glVertex3f(ellipsoid_a * cos(t) * cos(s), ellipsoid_b * cos(t) * sin(s), ellipsoid_c * sin(t));

                    glNormal3f(ellipsoid_a * cos(t + step1) * cos(s), ellipsoid_b * cos(t + step1) * sin(s), ellipsoid_c * sin(t + step1));
                    glVertex3f(ellipsoid_a * cos(t + step1) * cos(s), ellipsoid_b * cos(t + step1) * sin(s), ellipsoid_c * sin(t + step1));
                }
            }
        }
    }
    glEnd();

    //¸veg
    const GLfloat glass_diff[] = {1.0, 1.0, 1.0, 0.3};
    const GLfloat glass_spec[] = {0.0, 0.0, 1.0, 0.3};
    const GLfloat glass_amb[] = {0.0, 0.0, 1.0, 0.3};

    glMaterialfv(GL_FRONT, GL_AMBIENT, glass_amb);
    glMaterialfv(GL_FRONT, GL_DIFFUSE, glass_diff);
    glMaterialfv(GL_FRONT, GL_SPECULAR, glass_spec);
    glMaterialf(GL_FRONT, GL_SHININESS, 40);

    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    glBegin(GL_TRIANGLE_STRIP);
    for (float t = -M_PI / 2; t <= (M_PI / 2) + 0.0001; t += step1) {
        if (t <= 0) {
            for (float s = -M_PI; s <= M_PI + 0.0001; s += step2) {
                if (s >= -0.1) {
                    glNormal3f(ellipsoid_a * cos(t) * cos(s), ellipsoid_b * cos(t) * sin(s), ellipsoid_c * sin(t));
                    glVertex3f(ellipsoid_a * cos(t) * cos(s), ellipsoid_b * cos(t) * sin(s), ellipsoid_c * sin(t));

                    glNormal3f(ellipsoid_a * cos(t + step1) * cos(s), ellipsoid_b * cos(t + step1) * sin(s), ellipsoid_c * sin(t + step1));
                    glVertex3f(ellipsoid_a * cos(t + step1) * cos(s), ellipsoid_b * cos(t + step1) * sin(s), ellipsoid_c * sin(t + step1));
                }
            }
        }
    }
    glEnd();
    glDisable(GL_BLEND);
    glPopMatrix();

    drawMainRotor();
    drawChopperTail();
    drawTailRotor();

    glPopMatrix();

    drawQuatArrow();

    glPopMatrix();
}

void drawBuilding(double x, double z) {

    const GLfloat diff[] = {0.91, 0.0, 0.0, 1.0};
    const GLfloat spec[] = {0.1, 0.00, 0.0, 1.0};
    const GLfloat amb[] = {0.4, 0.1, 0.1, 1.0};

    glMaterialfv(GL_FRONT, GL_AMBIENT, amb);
    glMaterialfv(GL_FRONT, GL_DIFFUSE, diff);
    glMaterialfv(GL_FRONT, GL_SPECULAR, spec);
    glMaterialf(GL_FRONT, GL_SHININESS, 10.0);

    glPushMatrix();
    glTranslatef(x, -5, z);
    drawCuboid(0.5, 0.75, 0.5);
    glPopMatrix();
}

void drawField() {
    glEnable(GL_TEXTURE_2D);
    glBindTexture(GL_TEXTURE_2D, fieldTexture);

    const GLfloat diff[] = {0.0, 0.91, 0.0, 1.0};
    const GLfloat spec[] = {0.0, 0.0, 0.0, 1.0};
    const GLfloat amb[] = {0.1, 0.9, 0.1, 1.0};

    glMaterialfv(GL_FRONT, GL_AMBIENT, amb);
    glMaterialfv(GL_FRONT, GL_DIFFUSE, diff);
    glMaterialfv(GL_FRONT, GL_SPECULAR, spec);
    glMaterialf(GL_FRONT, GL_SHININESS, 10.0);

    glPushMatrix();
    glScalef(FIELD_WIDTH, 1, FIELD_LONG);
    glTranslatef(0, -5, -0.9);
    glBegin(GL_QUADS);
    for (float x = -1.0f; x < 1.0f; x += d) {
        for (float z = -1.0f; z < 1.0f; z += d) {
            glNormal3f(0, 1, 0);
            glTexCoord2f(x, z);
            glVertex3f(x, 0, -z);

            glNormal3f(0, 1, 0);
            glTexCoord2f(x + d, z);
            glVertex3f(x + d, 0, -z);

            glNormal3f(0, 1, 0);
            glTexCoord2f(x + d, z + d);
            glVertex3f(x + d, 0, -(z + d));

            glNormal3f(0, 1, 0);
            glTexCoord2f(x, z + d);
            glVertex3f(x, 0, -(z + d));
        }
    }
    glEnd();
    glPopMatrix();
    glDisable(GL_TEXTURE_2D);

    drawBuilding(0, -2);
    drawBuilding(0.5, -2);
    drawBuilding(10, -2);
    drawBuilding(10, -20);
    drawBuilding(-25, -20);
    drawBuilding(17, -30);
    drawBuilding(-14, -16);
    drawBuilding(-14, -40);
    drawBuilding(10, -40);
    drawBuilding(2.5, -30);
    drawBuilding(3, -30);
    drawBuilding(3.5, -30);
    drawBuilding(4, -30);

}

void setLight(Vector sunPosv) {
    float ambient[] = {0.5, 0.5, 0.5, 1.0};
    float sunColor[] = {0.75, 0.75, 0.67, 1.0};
    float glSunPos[] = {sunPosv.x, sunPosv.y, sunPosv.z, 0};

    glLightfv(GL_LIGHT0, GL_AMBIENT, ambient);
    glLightfv(GL_LIGHT0, GL_DIFFUSE, sunColor);
    glLightfv(GL_LIGHT0, GL_POSITION, glSunPos);
    glLightfv(GL_LIGHT0, GL_SPECULAR, sunColor);

    glEnable(GL_LIGHTING);
    glEnable(GL_LIGHT0);
}

void initFieldTexture() {
    /*
     * Forras: https://github.com/tmichel/Grafika-hf-2011-osz/blob/master/khf4.cpp#L919
     */
    unsigned char image[FIELD_WIDTH][FIELD_LONG][3];
    glGenTextures(1, &fieldTexture);

    glBindTexture(GL_TEXTURE_2D, fieldTexture);
    glTexEnvi(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    for (int x = 0; x < FIELD_WIDTH; ++x) {
        for (int y = 0; y < FIELD_LONG; ++y) {
            image[x][y][0] = 51 - (x + y) * 10;
            image[x][y][1] = 58 - (x - y) * 59;
            image[x][y][2] = 16 - (x * y) * 15;
        }
    }
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, FIELD_WIDTH, FIELD_LONG, 0,
            GL_RGB, GL_UNSIGNED_BYTE, image);
}

void recalcChopperDirection() {
    Quaternion chopperDirectionQuat = globalQuat % Quaternion(1, Vector(0, 0, -1)) % globalQuat.getInversed();
    chopperDirection = Vector(
            chopperDirectionQuat.GetAxis().x,
            chopperDirectionQuat.GetAxis().y,
            chopperDirectionQuat.GetAxis().z);

    cout << "chopperDirection: ";
    chopperDirection.dump();
}

void rotateChopper(Vector axis, double rot) {
    Quaternion newRotationAxises = globalQuat % Quaternion(0, axis) % globalQuat.getInversed();

    Quaternion rotationQuat = Quaternion(cos(rot), newRotationAxises.d * sin(rot));
    globalQuat = rotationQuat % globalQuat;

    Vector normalizedQuatAxis = globalQuat.GetAxis().getNormalized();
    arrowAxisOfRot = (Vector::Y() + normalizedQuatAxis)*0.5;

    recalcChopperDirection();
}

void onInitialization() {
    glViewport(0, 0, screenWidth, screenHeight);

    initFieldTexture();

    glMatrixMode(GL_PROJECTION);
    gluPerspective(60.0, 1.0, 0.1, FIELD_LONG);
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();

    glEnable(GL_DEPTH_TEST);
    glDisable(GL_CULL_FACE);
    glEnable(GL_NORMALIZE);
    glShadeModel(GL_SMOOTH);
    glDisable(GL_TEXTURE_2D);

    cam.pos = Vector(0.0, 2.3, 4);
    sunPos = Vector(0.0, 3, 3);
    setLight(sunPos);

    rotateChopper(Vector::X(), M_PI / 360.0);
}

void onDisplay() {
    glClearColor(0.6f, 0.77f, 0.75f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();

    cam.lookAt();

    drawField();

    drawChopper();

    glPushMatrix();
    for (int i = 0; i < AMMO; i++) {
        bullets[i].draw();
    }
    glPopMatrix();

    glutSwapBuffers();
}

void onKeyboard(unsigned char key, int x, int y) {
    if (key == 'q') {
        exit(0);
    }

    //Roll
    if (key == 'R') {
        //nˆvel 20 fokkal
        rotateChopper(Vector::Z(), rotation);

    }
    if (key == 'E') {
        //csˆkkent 20 fokkal
        rotateChopper(Vector::Z(), -rotation);
    }

    //Pitch
    if (key == 'P') {
        //nˆvel 20 fokkal
        rotateChopper(Vector::Y(), rotation);
    }
    if (key == 'O') {
        //csˆkkent 20 fokkal
        rotateChopper(Vector::Y(), -rotation);
    }

    //Yaw
    if (key == 'Y') {
        //nˆvel 20 fokkal
        rotateChopper(Vector::X(), rotation);
    }
    if (key == 'X') {
        //csˆkkent 20 fokkal
        rotateChopper(Vector::X(), -rotation);
    }

    //shoot
    if (key == 32) {
        if (shotCounter < AMMO) {
            bullets[shotCounter].pos = chopperPosition;
            bullets[shotCounter].dir = chopperDirection;
            bullets[shotCounter].use = true;
            shotCounter++;
        }
    }

    glutPostRedisplay();
}

void onMouse(int button, int state, int x, int y) {
}

void stepBullets(const double dt) {
    for (int i = 0; i < shotCounter; i++) {
        if (bullets[i].use) {

            bullets[i].dir.y -= 5 * (dt / 2000.0);
            Vector next = bullets[i].pos + bullets[i].dir;

            if (next.y < -5) {
                bullets[i].dir.y *= -BOUNCINESS;
            }

            bullets[i].pos = bullets[i].pos + bullets[i].dir;
        }
    }
}

void stepCamera(const double dt) {
    Vector distanceVector = chopperPosition - cam.pos;
    const double strain = distanceVector.Length() - CAM_SPRING_LENGTH;

    Vector springForce;
    if (strain > 0) {
        const double hooks_law_force = -CAM_SPRING_K * -strain;
        springForce = distanceVector.getNormalized() * hooks_law_force;
    }

    Vector a = springForce + (camBallSpeed * -CAM_BALL_AIR_RES) + CAM_UP_AND_GRAV_FORCE;

    camBallSpeed = a * dt;

    cam.pos = cam.pos + camBallSpeed * dt;
    cam.dir = chopperPosition;
}

void simulateWorld(long tstart, long tend) {
    const static double DT_50MS = 50;
    timeTmp += tend - tstart;

    if (timeTmp >= DT_50MS) {
        for (long ts = timeTmp; timeTmp > 0; timeTmp -= DT_50MS) {

            //helikopter mozgatas
            chopperPosition = chopperPosition + (chopperDirection * (1 / (1000 / DT_50MS)));

            stepBullets(DT_50MS);
            stepCamera(DT_50MS / 10.0);

            mainRotorDeg += 20;
            if (mainRotorDeg > 360) {
                mainRotorDeg -= 360;
            }

            tailRotorDeg -= 20;
            if (tailRotorDeg > 360) {
                tailRotorDeg -= 360;
            }
        }
    }
}

void onIdle() {
    long old_time = glut_elapsed_time;
    glut_elapsed_time = glutGet(GLUT_ELAPSED_TIME);

    simulateWorld(old_time, glut_elapsed_time);

    glutPostRedisplay();
}

int main(int argc, char **argv) {
    glutInit(&argc, argv); // GLUT inicializalasa
    glutInitWindowSize(600, 600); // Alkalmazas ablak kezdeti merete 600x600 pixel
    glutInitWindowPosition(100, 100); // Az elozo alkalmazas ablakhoz kepest hol tunik fel
    glutInitDisplayMode(GLUT_RGBA | GLUT_DOUBLE | GLUT_DEPTH); // 8 bites R,G,B,A + dupla buffer + melyseg buffer

    glutCreateWindow("Grafika hazi feladat"); // Alkalmazas ablak megszuletik es megjelenik a kepernyon

    glMatrixMode(GL_MODELVIEW); // A MODELVIEW transzformaciot egysegmatrixra inicializaljuk
    glLoadIdentity();
    glMatrixMode(GL_PROJECTION); // A PROJECTION transzformaciot egysegmatrixra inicializaljuk
    glLoadIdentity();

    onInitialization(); // Az altalad irt inicializalast lefuttatjuk

    glutDisplayFunc(onDisplay); // Esemenykezelok regisztralasa
    glutMouseFunc(onMouse);
    glutIdleFunc(onIdle);
    glutKeyboardFunc(onKeyboard);

    glutMainLoop(); // Esemenykezelo hurok

    return 0;
}

