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

const int screenWidth = 600;
const int screenHeight = 600;
const int FOREST_WIDTH = 10000;
const int MIN_HEIGHT = 250;
const int MAX_HEIGHT = 1014;
const int TOWER_HEIGHT = 20;
const int SIGN_HG_CIRCLE_LINE_NUM = 100;
const int SIGN_HG_CIRCLE_RADIUS = 250;
const Color COLOR_HANSEL = Color(117.0 / 255, 148.0 / 255, 202.0 / 255);
const Color COLOR_GRETA = Color(1.0, 160.0 / 255, 180.0 / 255.0);
const Color COLOR_TOWER = Color(1.0, 1.0, 0.0);
const Color COLOR_COVERAGE = Color(1.0, 0.0, 0.0);
const int BASE_SPEED = 1000;

Color image[screenWidth*screenHeight];
Vector centerHansel = Vector(5000, 5000);
Vector centerGreta = Vector(3000, 3000);
Vector centerTower = Vector(5000, 7000);

double angleHansel = 0.0;
double angleGreta = 0.0;

long time = 0;
long timeCovered = 0;
long timeTmp = 0;
bool isCovered = false;

const bool fequals(float f1, float f2) {
    if (fabs(f1 - f2) < 0.001) return true;
    return false;
}

unsigned psrnd() {
    //forras: levlista
    static unsigned seed = 2012;
    return (seed = seed * 199 + 1989) % 100;
}

const Vector convertPixelsToVariable(const Vector pixel) {
    return Vector((double) pixel.x / 100.0, (double) pixel.y / 100.0);
}

const Vector convertNatToPixel(const Vector natVector) {
    return Vector((int) (screenWidth * (natVector.x / FOREST_WIDTH)),
            (int) (screenWidth * (natVector.y / FOREST_WIDTH)));
}

const double calculateHeightValue(const Vector varVector) {
    return fabs(sin(2 * varVector.x) + cos(3 * varVector.y)
            + sin(varVector.x * varVector.y) * sin(varVector.x));
}

const double calculateHeightValueFromPixel(const Vector pixel) {
    const Vector v = convertPixelsToVariable(pixel);
    return calculateHeightValue(v);
}

const Vector convertNatToGl(const Vector natCoord) {
    return Vector(
            (natCoord.x - FOREST_WIDTH / 2) * (1.0 / (FOREST_WIDTH / 2)),
            (natCoord.y - FOREST_WIDTH / 2) * (1.0 / (FOREST_WIDTH / 2)));
}

void ns_glVertex2Vectors(const Vector vectors[], const int size) {
    for (int i = 0; i < size; i++) {
        glVertex2f(vectors[i].x, vectors[i].y);
    }
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

void drawHansel(const Vector natCenter) {
    double angle;
    Vector natCoordConnect;

    Vector circleVectors[SIGN_HG_CIRCLE_LINE_NUM];
    for (int i = 0; i < SIGN_HG_CIRCLE_LINE_NUM; i++) {
        angle = i * 2 * M_PI / SIGN_HG_CIRCLE_LINE_NUM;
        Vector natCoord = Vector(natCenter.x + (cos(angle) * SIGN_HG_CIRCLE_RADIUS),
                natCenter.y + (sin(angle) * SIGN_HG_CIRCLE_RADIUS));

        circleVectors[i] = convertNatToGl(natCoord);

        if (fequals(angle, 0.2 * M_PI)) {
            natCoordConnect = natCoord;
        }
    }

    glColor3f(COLOR_HANSEL.r, COLOR_HANSEL.g, COLOR_HANSEL.b);
    glBegin(GL_LINE_LOOP);
    ns_glVertex2Vectors(circleVectors, SIGN_HG_CIRCLE_LINE_NUM);
    glEnd();

    Vector glCoordConnect = convertNatToGl(natCoordConnect);
    Vector glCoordConnectLineEnd = convertNatToGl(Vector(natCoordConnect.x + 350, natCoordConnect.y + 300));
    Vector glCoordArrowLineEnd_1 = convertNatToGl(Vector(natCoordConnect.x + 100, natCoordConnect.y + 300));
    Vector glCoordArrowLineEnd_2 = convertNatToGl(Vector(natCoordConnect.x + 350, natCoordConnect.y + 50));

    const Vector lineVectors[] = {glCoordConnect, glCoordConnectLineEnd, glCoordConnectLineEnd,
        glCoordArrowLineEnd_1, glCoordConnectLineEnd, glCoordArrowLineEnd_2};
    glBegin(GL_LINES);
    ns_glVertex2Vectors(lineVectors, 6);
    glEnd();
}

void drawGreta(const Vector natCenter) {
    double angle;
    Vector natCoordConnect;

    Vector circleVectors[SIGN_HG_CIRCLE_LINE_NUM];
    for (int i = 0; i < SIGN_HG_CIRCLE_LINE_NUM; i++) {
        angle = i * 2 * M_PI / SIGN_HG_CIRCLE_LINE_NUM;
        Vector natCoord = Vector(natCenter.x + (cos(angle) * SIGN_HG_CIRCLE_RADIUS),
                natCenter.y + (sin(angle) * SIGN_HG_CIRCLE_RADIUS));

        circleVectors[i] = convertNatToGl(natCoord);

        if (fequals(angle, (3.0 / 2) * M_PI)) {
            natCoordConnect = natCoord;
        }
    }

    glColor3f(COLOR_GRETA.r, COLOR_GRETA.g, COLOR_GRETA.b);
    glBegin(GL_LINE_LOOP);
    ns_glVertex2Vectors(circleVectors, SIGN_HG_CIRCLE_LINE_NUM);
    glEnd();

    Vector glCoordConnect = convertNatToGl(natCoordConnect);
    Vector glCoordConnectLineEnd = convertNatToGl(Vector(natCoordConnect.x, natCoordConnect.y - 500));
    Vector glCoordCrossLine = convertNatToGl(Vector(natCoordConnect.x - 150, natCoordConnect.y - 250));
    Vector glCoordCrossLineEnd = convertNatToGl(Vector(natCoordConnect.x + 150, natCoordConnect.y - 250));
    const Vector lineVectors[] = {glCoordConnect, glCoordConnectLineEnd, glCoordCrossLine, glCoordCrossLineEnd};
    glBegin(GL_LINES);
    ns_glVertex2Vectors(lineVectors, 4);
    glEnd();
}

void drawTower(const Vector natCenter) {

    glColor3f(COLOR_TOWER.r, COLOR_TOWER.g, COLOR_TOWER.b);
    Vector glCoordsTower[] = {
        convertNatToGl(Vector(natCenter.x - 150, natCenter.y - 70)),
        convertNatToGl(Vector(natCenter.x + 150, natCenter.y - 70)),
        convertNatToGl(Vector(natCenter.x, natCenter.y + 70))
    };
    glBegin(GL_TRIANGLES);
    ns_glVertex2Vectors(glCoordsTower, 3);
    glEnd();
}

void drawCoverage() {
    Vector points[] = {convertNatToGl(centerGreta), convertNatToGl(centerHansel),
        convertNatToGl(centerTower)};

    glColor3f(COLOR_COVERAGE.r, COLOR_COVERAGE.g, COLOR_COVERAGE.b);
    glBegin(GL_TRIANGLES);
    ns_glVertex2Vectors(points, 3);
    glEnd();
}

void drawCoverageBar() {
    double coveredFragment = (double) timeCovered / time;
    Vector glCoveredBarWidth = convertNatToGl(Vector(coveredFragment * FOREST_WIDTH, 0));

    glColor3f(0.8, 0.8, 0.8);
    glBegin(GL_QUADS);
    glVertex2f(-1.0, 1.0);
    glVertex2f(1.0, 1.0);
    glVertex2f(1.0, 0.94);
    glVertex2f(-1.0, 0.94);

    glColor3f(0.0, 0.90, 0.0);
    glVertex2f(-1.0, 1.0);
    glVertex2f(glCoveredBarWidth.x, 1.0);
    glVertex2f(glCoveredBarWidth.x, 0.94);
    glVertex2f(-1.0, 0.94);
    glEnd();
}

void checkAndDoBounce(const Vector center, double &angle) {
    if (center.x < 0 || center.x >= FOREST_WIDTH) {
        angle = M_PI - angle;
    }

    if (center.y < 0 || center.y >= FOREST_WIDTH) {
        angle *= -1.0;
    }
}

const Vector getGradientVarVector(const Vector v) {

    const double x = ((v.y * sin(v.x) * cos(v.x * v.y) + cos(v.x) * sin(v.x * v.y) + 2 * cos(2 * v.x)) * (sin(v.x) * sin(v.x * v.y) + sin(2 * v.x) + cos(3 * v.y))) / fabs(sin(v.x) * sin(v.x * v.y) + sin(2 * v.x) + cos(3 * v.y)); // d/dx f(v)
    const double y = ((v.x * sin(v.x) * cos(v.x * v.y) - 3 * sin(3 * v.y)) * (sin(v.x) * sin(v.x * v.y) + sin(2 * v.x) + cos(3 * v.y))) / fabs(sin(v.x) * sin(v.x * v.y) + sin(2 * v.x) + cos(3 * v.y)); // d/dy f(v)

    return Vector(x, y);
}

void stepSign(Vector &center, double &angle, const double deltaT_S) {
    const Vector gradVector = getGradientVarVector(center);
    const double m = atan(gradVector.x * cos(angle) * 10 + gradVector.y * sin(angle) * 10);
    const double speed = BASE_SPEED * (1 - m / 90);
    center.x = center.x + cos(angle) * speed * deltaT_S;
    center.y = center.y + sin(angle) * speed * deltaT_S;
    checkAndDoBounce(center, angle);
}

void changeDirection(const Vector center, double &angle, const int clickX, const int clickY) {
    Vector clickNat;
    clickNat.x = ((double) clickX / screenWidth) * FOREST_WIDTH;
    clickNat.y = FOREST_WIDTH - ((double) clickY / screenWidth) * FOREST_WIDTH;

    Vector dir(clickNat.x - center.x, clickNat.y - center.y);
    angle = atan(dir.y / dir.x);

    if (clickNat.x < center.x) {
        angle += M_PI;
    }
}

void onInitialization() {
    glViewport(0, 0, screenWidth, screenHeight);

    ThisEnterpriseMethodGeneratesTheMagicForestSoHeresANotFunnyJoke___I_used_to_be_a_werewolf_But_I_am_much_better_nooooooooooooooooooooooooow();
}

void onDisplay() {
    glClearColor(0.1f, 0.2f, 0.3f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    glDrawPixels(screenWidth, screenHeight, GL_RGB, GL_FLOAT, image);
    drawCoverageBar();

    drawTower(centerTower);
    drawGreta(centerGreta);
    drawHansel(centerHansel);
    if (isCovered) {
        drawCoverage();
    }

    glutSwapBuffers();

}

void onKeyboard(unsigned char key, int x, int y) {
    if (key == 'd') glutPostRedisplay();
    if (key == 'q') exit(0);
    if (key == 't') {
        srand(psrnd());
        double r_x = (double) rand() / RAND_MAX;
        double r_y = (double) rand() / RAND_MAX;
        centerTower.x = (int) (r_x * FOREST_WIDTH);
        centerTower.y = (int) (r_y * FOREST_WIDTH);
    }
}

void onMouse(int button, int state, int x, int y) {
    if (button == GLUT_LEFT && state == GLUT_DOWN) {
        changeDirection(centerHansel, angleHansel, x, y);
    } else if (button == GLUT_RIGHT_BUTTON && state == GLUT_DOWN) {
        changeDirection(centerGreta, angleGreta, x, y);
    }

    glutPostRedisplay();
}

void calcCoverage() {
    const static double towerHeightPlus = (2.0 / (MAX_HEIGHT - MIN_HEIGHT)) * TOWER_HEIGHT;

    isCovered = true;
    centerHansel.z = calculateHeightValueFromPixel(convertNatToPixel(centerHansel));
    centerGreta.z = calculateHeightValueFromPixel(convertNatToPixel(centerGreta));

    centerTower.z = calculateHeightValueFromPixel(convertNatToPixel(centerTower));
    centerTower.z += towerHeightPlus;

    for (double t = 0.0; t < 1; t += 0.01) {
        const double x = centerHansel.x * t + centerTower.x * (1 - t);
        const double y = centerHansel.y * t + centerTower.y * (1 - t);

        const double z = centerHansel.z * t + centerTower.z * (1 - t);
        const double height = calculateHeightValueFromPixel(convertNatToPixel(Vector(x, y)));

        if (z < height) {
            isCovered = false;
            break;
        } else {
            const double x = centerGreta.x * t + centerTower.x * (1 - t);
            const double y = centerGreta.y * t + centerTower.y * (1 - t);

            const double z = centerGreta.z * t + centerTower.z * (1 - t);
            const double height = calculateHeightValueFromPixel(convertNatToPixel(Vector(x, y)));

            if (z < height) {
                isCovered = false;
                break;
            }
        }
    }
}

void simulateWorld(long tstart, long tend) {
    const static double DT_100MS = 100;
    timeTmp += tend - tstart;

    if (timeTmp >= DT_100MS) {
        for (long ts = timeTmp; timeTmp > 0; timeTmp -= DT_100MS) {

            stepSign(centerHansel, angleHansel, DT_100MS / 1000);
            stepSign(centerGreta, angleGreta, DT_100MS / 1000);

            calcCoverage();

            if (isCovered) {
                timeCovered += DT_100MS;
            }
        }
    }
}

void onIdle() {
    long old_time = time;
    time = glutGet(GLUT_ELAPSED_TIME);

    simulateWorld(old_time, time);

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

