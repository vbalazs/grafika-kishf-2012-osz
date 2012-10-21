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
#include <iostream>
using namespace std;

/**
 * Források:
 * [1]: tavalyi 2. házim: https://github.com/vbalazs/grafika-kishf-2012-tavasz/
 * [2]: On-Line Geometric Modeling Notes - CATMULL-ROM SPLINES(Kenneth I. Joy,
 * Visualization and Graphics Research Group, Department of Computer Science, University of California, Davis)
 * [3]: Dr. Szirmay-Kalos László, Antal György, Csonka Ferenc 
 *              - Háromdimenziós Grafika, animáció és játékfejlesztés, 2004 - Computerbooks, Budapest
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

const int VIRT_WIDTH = 1000; //1000mm
const int MAX_NR_OF_CTRPs = 100; //number of control points

Vector virtcam_bottom_left = Vector(100.0, 100.0);
Vector virtcam_top_right = Vector(500.0, 500.0);

const Color COLOR_CR = Color(0.0, 1.0, 0.0);
const Color COLOR_KK = Color(1.0, 0.0, 0.0);

//fibonacci
double fibonacci[MAX_NR_OF_CTRPs];

/**
 * Forrás: [1], átalakítva
 */
class CurveManager {
private:

    /**
     * Forrás: [2] és [3] alapján
     */
    Vector CatmullRomMagic(double t, int i) {
        Vector a, b, c, d;

        double t_i = fibonacci[i];
        double t_i_plus_1 = fibonacci[i + 1];
        double t_i_plus_2 = fibonacci[i + 2];
        double t_i_minus_1 = fibonacci[i - 1];

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

        // t{i} <= t < t{i+1}
        //t{i} = fibonacci[i]
        const Vector f_t = a * pow(t - t_i, 3) + b * pow(t - t_i, 2) +
                c * (t - t_i) + d;

        return f_t;
    }
public:
    Vector ctrlPoints[MAX_NR_OF_CTRPs];
    int numOfPoints;

    CurveManager() {
        numOfPoints = 0;
    }

    void draw() {
        glColor3f(COLOR_CR.r, COLOR_CR.g, COLOR_CR.b);
        glBegin(GL_LINE_STRIP);
        for (int i = 1; i < numOfPoints - 2; ++i) {
            double rate = (fibonacci[i + 1] - fibonacci[i]) / 1000.0;
            for (double t = fibonacci[i]; t < fibonacci[i + 1]; t += rate) {
                Vector v = CatmullRomMagic(t, i);
                glVertex2f(v.x, v.y);
            }

        }
        glEnd();

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

    void addVector(const Vector v) {
        if (numOfPoints < MAX_NR_OF_CTRPs && v.x <= VIRT_WIDTH && v.y <= VIRT_WIDTH) {
            ctrlPoints[numOfPoints++] = v;
        }
    }

    bool isPointNearby(Vector clickedPixel) {
        return false; //TODO
    }
};

CurveManager curveManager;

/*
 * Binet form
 * Forras: [1]
 */
const double getFibonacciNr(int n) {
    const double sqrt5 = sqrt(5);
    return (pow(1 + sqrt5, n) - pow(1 - sqrt5, n)) / (sqrt5 * pow(2, n));
}

/**
 * Forras: [1]
 */
Vector getWorldCoordsFromPixels(const int px_x, const int px_y) {
    const double width = virtcam_top_right.x - virtcam_bottom_left.x;

    return Vector(virtcam_bottom_left.x + (width / (double) screenWidth) * px_x,
            virtcam_bottom_left.y + (width - (width / (double) screenWidth) * px_y));
}

const bool fequals(const float f1, const float f2) {
    if (fabs(f1 - f2) < 0.001) return true;
    return false;
}

void onInitialization() {
    glViewport(0, 0, screenWidth, screenHeight);
    glLoadIdentity();
    gluOrtho2D(virtcam_bottom_left.x, //left
            virtcam_top_right.x, //right
            virtcam_bottom_left.y, //bottom
            virtcam_top_right.y); //top

    //fill up array with fibonacci numbers - Binet form
    for (int i = 2; i <= MAX_NR_OF_CTRPs; i++) {
        fibonacci[i - 1] = getFibonacciNr(i);
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

    const double width = virtcam_top_right.x - virtcam_bottom_left.x;

    if (key == 'z') { //zoom in
        const double change = (width - width / 10.0) / 2.0;

        virtcam_bottom_left.x += change;
        virtcam_bottom_left.y += change;

        virtcam_top_right.x -= change;
        virtcam_top_right.y -= change;

        glLoadIdentity();
        gluOrtho2D(virtcam_bottom_left.x, //left
                virtcam_top_right.x, //right
                virtcam_bottom_left.y, //bottom
                virtcam_top_right.y); //top

    } else if (key == 'Z') { //zoom out
        const double change = width / 2.0 * 10.0 - width / 2.0;

        virtcam_bottom_left.x -= change;
        virtcam_bottom_left.y -= change;

        virtcam_top_right.x += change;
        virtcam_top_right.y += change;

        glLoadIdentity();
        gluOrtho2D(virtcam_bottom_left.x, //left
                virtcam_top_right.x, //right
                virtcam_bottom_left.y, //bottom
                virtcam_top_right.y); //top
    }
}

void onMouse(int button, int state, int x, int y) {
    if (button == GLUT_LEFT && state == GLUT_DOWN) {
        cout << "INFO: onMouse leftClicked. x=" << x << " ; y=" << y << endl;

        curveManager.addVector(getWorldCoordsFromPixels(x, y));
    } else if (button == GLUT_RIGHT_BUTTON && state == GLUT_DOWN) {
        //select
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

