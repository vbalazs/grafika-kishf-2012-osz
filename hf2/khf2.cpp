//=============================================================================================
// Szamitogepes grafika hazi feladat keret. Ervenyes 2012-tol.
// A //~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// sorokon beluli reszben celszeru garazdalkodni, mert a tobbit ugyis toroljuk.
// A beadott program csak ebben a fajlban lehet, a fajl 1 byte-os ASCII karaktereket tartalmazhat.
// Tilos:
// - mast "beincludolni", illetve mas konyvtarat hasznalni
// - faljmuveleteket vegezni (printf is fajlmuvelet!)
// - new operatort hivni az onInitialization fï¿½ggvï¿½nyt kivï¿½ve, a lefoglalt adat korrekt felszabadï¿½tï¿½sa nï¿½lkï¿½l
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
// Nev    : Varga Balazs
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

#if defined(WIN32) || defined(_WIN32) || defined(__WIN32__)
// MsWindows-on ez is kell
#include <windows.h>
#endif // Win32 platform

#include <GL/gl.h>
#include <GL/glu.h>
// A GLUT-ot le kell tolteni: http://www.opengl.org/resources/libraries/glut/
#include <GL/glut.h>

/**
 * Források:
 * [1]: tavalyi 2. házim: https://github.com/vbalazs/grafika-kishf-2012-tavasz/
 * [2]: On-Line Geometric Modeling Notes - CATMULL-ROM SPLINES(Kenneth I. Joy,
 * Visualization and Graphics Research Group, Department of Computer Science, University of California, Davis)
 * [3]: Dr. Szirmay-Kalos László, Antal György, Csonka Ferenc 
 *              - Háromdimenziós Grafika, animáció és játékfejlesztés, 2004 - Computerbooks, Budapest
 * [4]: http://en.wikipedia.org/wiki/Cohen%E2%80%93Sutherland_algorithm
 */

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// Innentol modosithatod...

//--------------------------------------------------------
// 3D Vektor
//--------------------------------------------------------

struct Vector {
    double x, y, z;

    Vector() {
        x = y = z = 0;
    }

    Vector(double x0, double y0, double z0 = 0) {
        x = x0;
        y = y0;
        z = z0;
    }

    Vector operator*(double a) {
        return Vector(x * a, y * a, z * a);
    }

    Vector operator+(const Vector & v) {
        return Vector(x + v.x, y + v.y, z + v.z);
    }

    Vector operator-(const Vector & v) {
        return Vector(x - v.x, y - v.y, z - v.z);
    }

    double operator*(const Vector & v) { // dot product
        return (x * v.x + y * v.y + z * v.z);
    }

    Vector operator%(const Vector & v) { // cross product
        return Vector(y * v.z - z * v.y, z * v.x - x * v.z, x * v.y - y * v.x);
    }

    double Length() {
        return sqrt(x * x + y * y + z * z);
    }
};

//--------------------------------------------------------
// Spektrum illetve szin
//--------------------------------------------------------

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

const int screenWidth = 600;
const int screenHeight = 600;

const int VIRT_WIDTH = 1000;
const int MAX_NR_OF_CTRPs = 100;

Vector virtcam_bottom_left = Vector(100.0, 100.0);
Vector virtcam_top_right = Vector(500.0, 500.0);

const Color COLOR_CR = Color(0.0, 1.0, 0.0);
const Color COLOR_KK = Color(1.0, 0.0, 0.0);

double params[MAX_NR_OF_CTRPs];
double rate = 0;
bool is_moving = false;
Vector moving_from_px;

/**
 * Forras: [1]
 */
Vector getWorldCoordsFromPixels(const double px_x, const double px_y) {
    const double width = virtcam_top_right.x - virtcam_bottom_left.x;

    return Vector(virtcam_bottom_left.x + (width / (double) screenWidth) * px_x,
            virtcam_bottom_left.y + (width - (width / (double) screenWidth) * px_y));
}

/*
 * Cohen-Sutherland vágás: Forrás: [4]
 */
const int INSIDE = 0;
const int LEFT = 1;
const int RIGHT = 2;
const int BOTTOM = 4;
const int TOP = 8;

int ComputeOutCode(double x, double y, double xmin, double ymin, double xmax, double ymax) {
    int code = INSIDE;

    if (x < xmin)
        code |= LEFT;
    else if (x > xmax)
        code |= RIGHT;
    if (y < ymin)
        code |= BOTTOM;
    else if (y > ymax)
        code |= TOP;

    return code;
}

bool CohenSutherlandLineInRect(double x0, double y0, double x1, double y1, double xmin, double ymin, double xmax, double ymax) {
    int outcode0 = ComputeOutCode(x0, y0, xmin, ymin, xmax, ymax);
    int outcode1 = ComputeOutCode(x1, y1, xmin, ymin, xmax, ymax);
    bool accept = false;

    while (true) {
        if (!(outcode0 | outcode1)) {
            accept = true;
            break;
        } else if (outcode0 & outcode1) {
            break;
        } else {
            double x, y;

            int outcodeOut = outcode0 ? outcode0 : outcode1;

            if (outcodeOut & TOP) {
                x = x0 + (x1 - x0) * (ymax - y0) / (y1 - y0);
                y = ymax;
            } else if (outcodeOut & BOTTOM) {
                x = x0 + (x1 - x0) * (ymin - y0) / (y1 - y0);
                y = ymin;
            } else if (outcodeOut & RIGHT) {
                y = y0 + (y1 - y0) * (xmax - x0) / (x1 - x0);
                x = xmax;
            } else {
                y = y0 + (y1 - y0) * (xmin - x0) / (x1 - x0);
                x = xmin;
            }

            if (outcodeOut == outcode0) {
                x0 = x;
                y0 = y;
                outcode0 = ComputeOutCode(x0, y0, xmin, ymin, xmax, ymax);
            } else {
                x1 = x;
                y1 = y;
                outcode1 = ComputeOutCode(x1, y1, xmin, ymin, xmax, ymax);
            }
        }
    }

    return accept;
}

/**
 * Forrás: [1], átalakítva
 */
class CurveManager {
private:

    /**
     * Forrás: [1] ([2] és [3] alapján)
     */
    Vector CatmullRomMagic(const double t, const int i) {
        Vector a, b, c, d;

        double t_i = params[i];
        double t_i_plus_1 = params[i + 1];
        double t_i_plus_2 = params[i + 2];
        double t_i_minus_1 = params[i - 1];

        Vector f_i = ctrlPoints[i];
        Vector f_i_plus_1 = ctrlPoints[i + 1];
        Vector f_i_plus_2 = ctrlPoints[i + 2];
        Vector f_i_minus_1 = ctrlPoints[i - 1];

        Vector v_i = ((f_i - f_i_minus_1) * (1 / (t_i - t_i_minus_1)) +
                (f_i_plus_1 - f_i) * (1 / (t_i_plus_1 - t_i))) * 0.5;

        Vector v_i_plus_1 = ((f_i_plus_1 - f_i) * (1 / (t_i_plus_1 - t_i)) +
                (f_i_plus_2 - f_i_plus_1) * (1 / (t_i_plus_2 - t_i_plus_1))) * 0.5;

        d = f_i;
        c = v_i;

        b = ((f_i_plus_1 - f_i) * 3) * (1 / pow(t_i_plus_1 - t_i, 2)) -
                (v_i_plus_1 + v_i * 2) * (1 / (t_i_plus_1 - t_i));

        a = (v_i_plus_1 + v_i) * (1 / pow(t_i_plus_1 - t_i, 2)) -
                ((f_i_plus_1 - f_i) * 2) * (1 / pow(t_i_plus_1 - t_i, 3));

        const Vector f_t = a * pow(t - t_i, 3) + b * pow(t - t_i, 2) +
                c * (t - t_i) + d;

        return f_t;
    }

    void drawCR() {
        glColor3f(COLOR_CR.r, COLOR_CR.g, COLOR_CR.b);
        glBegin(GL_LINE_STRIP);
        for (int i = 1; i < numOfPoints - 2; ++i) {
            for (double t = params[i]; t < params[i + 1]; t += rate) {
                Vector v = CatmullRomMagic(t, i);
                glVertex2f(v.x, v.y);
            }
        }
        glEnd();
    }

    void drawPointMarkers() {
        const double point_marker_size = (virtcam_top_right.x - virtcam_bottom_left.x) / 200;
        glBegin(GL_TRIANGLES);
        glColor3f(0.0, 0.0, 1.0);
        for (int i = 0; i < numOfPoints; i++) {
            glVertex2f(ctrlPoints[i].x, ctrlPoints[i].y);
            glVertex2f(ctrlPoints[i].x - point_marker_size, ctrlPoints[i].y - point_marker_size);
            glVertex2f(ctrlPoints[i].x + point_marker_size, ctrlPoints[i].y - point_marker_size);
        }
        glEnd();
    }

    Vector convexComb(const int i, const double t, const double t0,
            const double t1, const double t2, const double t3) {

        const double a0 = (t - t1) / (t0 - t1) * (t - t2) / (t0 - t2);
        const double a1 = (t - t0) / (t1 - t0) * (t - t2) / (t1 - t2);
        const double a2 = (t - t0) / (t2 - t0) * (t - t1) / (t2 - t1);

        const double b0 = (t - t2) / (t1 - t2) * (t - t3) / (t1 - t3);
        const double b1 = (t - t1) / (t2 - t1) * (t - t3) / (t2 - t3);
        const double b2 = (t - t1) / (t3 - t1) * (t - t2) / (t3 - t2);

        Vector curvePointA = ctrlPoints[i] * a0 + ctrlPoints[i + 1] * a1 + ctrlPoints[i + 2] * a2;
        Vector curvePointB = ctrlPoints[i + 1] * b0 + ctrlPoints[i + 2] * b1 + ctrlPoints[i + 3] * b2;

        return curvePointA * ((t - t1) / (t2 - t1)) + curvePointB * ((t2 - t) / (t2 - t1));
    }

    void drawKK() {
        glColor3f(COLOR_KK.r, COLOR_KK.g, COLOR_KK.b);
        glBegin(GL_LINE_STRIP);
        for (int i = 0; i < numOfPoints - 3; ++i) {
            const double t0 = params[i];
            const double t1 = params[i + 1];
            const double t2 = params[i + 2];
            const double t3 = params[i + 3];

            for (double t = t1; t < t2; t += rate) {
                const Vector v = convexComb(i, t, t0, t1, t2, t3);
                glVertex2f(v.x, v.y);
            }
        }
        glEnd();
    }
public:
    Vector ctrlPoints[MAX_NR_OF_CTRPs];
    int numOfPoints;

    CurveManager() : numOfPoints(0) {
    }

    void draw() {
        drawPointMarkers();
        drawCR();
        drawKK();
    }

    void addPoint(const Vector v) {
        if (numOfPoints < MAX_NR_OF_CTRPs && v.x <= VIRT_WIDTH && v.y <= VIRT_WIDTH) {
            ctrlPoints[numOfPoints++] = v;
        }
    }

    bool isPointNearby(Vector clickedPixel) {

        const Vector split_rect_bottom_left = getWorldCoordsFromPixels(clickedPixel.x - 2.5, clickedPixel.y + 2.5);
        const Vector split_rect_top_right = getWorldCoordsFromPixels(clickedPixel.x + 2.5, clickedPixel.y - 2.5);

        for (int i = 0; i < numOfPoints - 3; ++i) {
            const double t0 = params[i];
            const double t1 = params[i + 1];
            const double t2 = params[i + 2];
            const double t3 = params[i + 3];

            for (double t = t1; t < t2 - rate; t += rate * 2) {

                const Vector curve_line_point1 = convexComb(i, t, t0, t1, t2, t3);
                const Vector curve_line_point2 = convexComb(i, t + rate, t0, t1, t2, t3);

                if (CohenSutherlandLineInRect(curve_line_point1.x, curve_line_point1.y, curve_line_point2.x, curve_line_point2.y,
                        split_rect_bottom_left.x, split_rect_bottom_left.y, split_rect_top_right.x, split_rect_top_right.y)) {

                    return true;
                }
            }
        }

        for (int i = 1; i < numOfPoints - 2; ++i) {
            for (double t = params[i]; t < params[i + 1] - rate; t += rate * 2) {

                Vector curve_line_point1 = CatmullRomMagic(t, i);
                Vector curve_line_point2 = CatmullRomMagic(t + rate, i);

                if (CohenSutherlandLineInRect(curve_line_point1.x, curve_line_point1.y, curve_line_point2.x, curve_line_point2.y,
                        split_rect_bottom_left.x, split_rect_bottom_left.y, split_rect_top_right.x, split_rect_top_right.y)) {

                    return true;
                }
            }
        }

        return false;
    }
};

CurveManager curveManager;

void onInitialization() {
    glViewport(0, 0, screenWidth, screenHeight);
    glLoadIdentity();
    gluOrtho2D(virtcam_bottom_left.x,
            virtcam_top_right.x,
            virtcam_bottom_left.y,
            virtcam_top_right.y);

    for (int i = 0; i <= MAX_NR_OF_CTRPs; i++) {
        params[i] = (double) (i + params[i - 1] + 1.0);
    }
}

void onDisplay() {
    glClearColor(0.4f, 0.4f, 0.4f, 0.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    curveManager.draw();

    glutSwapBuffers();

}

void onKeyboard(unsigned char key, int x, int y) {
    if (key == 'd') glutPostRedisplay();
    if (key == 'q') exit(0);

    if (key == 'z') {
        const double new_width = (virtcam_top_right.x - virtcam_bottom_left.x) / 10;
        const Vector clickedWorldCoord = getWorldCoordsFromPixels(x, y);

        virtcam_bottom_left.x = clickedWorldCoord.x
                - (clickedWorldCoord.x - virtcam_bottom_left.x) / 10;

        virtcam_bottom_left.y = clickedWorldCoord.y
                + (virtcam_bottom_left.y - clickedWorldCoord.y) / 10;

        virtcam_top_right.x = virtcam_bottom_left.x + new_width;
        virtcam_top_right.y = virtcam_bottom_left.y + new_width;

        glLoadIdentity();
        gluOrtho2D(virtcam_bottom_left.x,
                virtcam_top_right.x,
                virtcam_bottom_left.y,
                virtcam_top_right.y);
    } else if (key == 'Z') {
        const double new_width = (virtcam_top_right.x - virtcam_bottom_left.x) * 10;
        const Vector clickedWorldCoord = getWorldCoordsFromPixels(x, y);

        virtcam_bottom_left.x = clickedWorldCoord.x
                - (clickedWorldCoord.x - virtcam_bottom_left.x) * 10;

        virtcam_bottom_left.y = clickedWorldCoord.y
                + (virtcam_bottom_left.y - clickedWorldCoord.y) * 10;

        virtcam_top_right.x = virtcam_bottom_left.x + new_width;
        virtcam_top_right.y = virtcam_bottom_left.y + new_width;

        glLoadIdentity();
        gluOrtho2D(virtcam_bottom_left.x,
                virtcam_top_right.x,
                virtcam_bottom_left.y,
                virtcam_top_right.y);
    }
}

void onMouse(int button, int state, int x, int y) {
    if (button == GLUT_LEFT && state == GLUT_DOWN) {
        curveManager.addPoint(getWorldCoordsFromPixels(x, y));

        double param_range_sum = 0;
        for (int i = 0; i < curveManager.numOfPoints; i++) {
            param_range_sum += params[i];
        }
        rate = param_range_sum / 1000.0;

    } else if (button == GLUT_RIGHT_BUTTON && state == GLUT_DOWN) {
        if (is_moving) {
            const Vector moving_from_world =
                    getWorldCoordsFromPixels(moving_from_px.x, moving_from_px.y);

            const Vector moving_to_world = getWorldCoordsFromPixels(x, y);

            for (int i = 0; i < curveManager.numOfPoints; i++) {
                curveManager.ctrlPoints[i].x += moving_to_world.x - moving_from_world.x;
                curveManager.ctrlPoints[i].y += moving_to_world.y - moving_from_world.y;
            }

            is_moving = false;
        } else {
            if (curveManager.isPointNearby(Vector(x, y))) {
                is_moving = true;
                moving_from_px = Vector(x, y);
            }
        }
    }

    glutPostRedisplay();
}

void onIdle() {
    glutPostRedisplay();
}

// ...Idaig modosithatod
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

// A C++ program belepesi pontja, a main fuggvenyt mar nem szabad bantani

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

