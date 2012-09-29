//=============================================================================================
// Szamitogepes grafika hazi feladat keret. Ervenyes 2012-tol.
// A //~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// sorokon beluli reszben celszeru garazdalkodni, mert a tobbit ugyis toroljuk.
// A beadott program csak ebben a fajlban lehet, a fajl 1 byte-os ASCII karaktereket tartalmazhat.
// Tilos:
// - mast "beincludolni", illetve mas konyvtarat hasznalni
// - faljmuveleteket vegezni (printf is fajlmuvelet!)
// - new operatort hivni az onInitialization függvényt kivéve, a lefoglalt adat korrekt felszabadítása nélkül
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
#include <stdio.h>

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// Innentol modosithatod...

//--------------------------------------------------------
// 3D Vektor
//--------------------------------------------------------

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

const int screenWidth = 600; // alkalmazás ablak felbontása
const int screenHeight = 600;
const double PIXEL_RATE = 100.0;
const int COORD_NAT = 5000;
const int MIN_HEIGHT = 250;
const int MAX_HEIGHT = 1014;
const int TOWER_HEIGHT = 20;
const Color COLOR_HANSEL = Color(117.0 / 255, 148.0 / 255, 202.0 / 255);
const Color COLOR_GRETA = Color(1.0, 160.0 / 255, 180.0 / 255.0);

Color image[screenWidth*screenHeight];
Vector centerHansel = Vector(3000, 0);
Vector centerGreta = Vector(-3000, -3000);
long time = 0;
bool working = false;

const Vector convertPixelsToVariable(const Vector pixel) {
    return Vector((double) pixel.x / PIXEL_RATE, (double) pixel.y / PIXEL_RATE);
}

const double calculateHeightValue(const Vector varVector) {
    return fabs(sin(2 * varVector.x) + cos(3 * varVector.y)
            + sin(varVector.x * varVector.y) * sin(varVector.x));
}

const double calculateHeightValueFromPixel(const Vector pixel) {
    const Vector v = convertPixelsToVariable(pixel);
    return calculateHeightValue(v);
}

void ThisEnterpriseMethodGeneratesTheMagicForestSoHeresANotFunnyJoke___I_used_to_be_a_werewolf_But_I_am_much_better_nooooooooooooooooooooooooow() {
    double height;
    double greyCode;

    for (int y = 0; y < screenHeight; y++) {
        for (int x = 0; x < screenWidth; x++) {
            height = calculateHeightValueFromPixel(Vector(x, y));
            greyCode = height * (1.0 / 2.0);
            image[y * screenWidth + x] = Color(greyCode, greyCode, greyCode);
        }
    }
}

void drawHansel(const Vector center) {

}

void drawGreta(const Vector center) {

}

void onInitialization() {
    glViewport(0, 0, screenWidth, screenHeight);

    ThisEnterpriseMethodGeneratesTheMagicForestSoHeresANotFunnyJoke___I_used_to_be_a_werewolf_But_I_am_much_better_nooooooooooooooooooooooooow();

}

void onDisplay() {
    glClearColor(0.1f, 0.2f, 0.3f, 1.0f); // torlesi szin beallitasa
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT); // kepernyo torles

    glDrawPixels(screenWidth, screenHeight, GL_RGB, GL_FLOAT, image);

    drawGreta(centerGreta);
    drawHansel(centerHansel);

    glColor3f(COLOR_GRETA.r, COLOR_GRETA.g, COLOR_GRETA.b);
    glBegin(GL_TRIANGLES);
    glVertex2f(-0.2f, -0.2f);
    glVertex2f(0.2f, -0.2f);
    glVertex2f(0.0f, 0.2f);
    glEnd();

    glutSwapBuffers(); // Buffercsere: rajzolas vege

}

void onKeyboard(unsigned char key, int x, int y) {
    if (key == 'd') glutPostRedisplay(); // d beture rajzold ujra a kepet
    if (key == 't') { //torony áthelyezése
        printf("->torony athelyezese\n");
    }

}

void onMouse(int button, int state, int x, int y) {
    if (button == GLUT_LEFT_BUTTON && state == GLUT_DOWN) {// hansel
        printf("->hansel iranyvalt\n");
        glutPostRedisplay(); // Ilyenkor rajzold ujra a kepet
    } else if (button == GLUT_RIGHT_BUTTON && state == GLUT_DOWN) {// greta
        printf("->greta iranyvalt\n");
        glutPostRedisplay(); // Ilyenkor rajzold ujra a kepet
    }
}

void simulateWorld(long tstart, long tend) {
    float dt = 50;
    for (float ts = tstart; ts < tend; ts += dt) {
        float te;
        if (tend >= ts + dt) {
            te = ts + dt;
        } else {
            te = tend;
        }

        //do stuff
    }
}

void onIdle() {
    if (!working) {
        working = true;
        long old_time = time;
        time = glutGet(GLUT_ELAPSED_TIME);

        simulateWorld(old_time, time);

        working = false;
        glutPostRedisplay();
    }
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

