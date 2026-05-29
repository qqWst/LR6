#include <GL/glut.h>
#include <iostream>
#include <cmath>

using namespace std;

int level = 0;               // уровень рекурсии
int windowWidth = 800;
int windowHeight = 600;

// Рисование закрашенного треугольника по трём точкам
void drawTriangle(float x1, float y1, float x2, float y2, float x3, float y3) {
    glBegin(GL_TRIANGLES);
        glVertex2f(x1, y1);
        glVertex2f(x2, y2);
        glVertex2f(x3, y3);
    glEnd();
}

// Рекурсивное построение треугольника Серпинского
void sierpinski(float x1, float y1, float x2, float y2, float x3, float y3, int curLevel) {
    if (curLevel == 0) {
        drawTriangle(x1, y1, x2, y2, x3, y3);
        return;
    }

    // Вычисляем середины сторон
    float mx12 = (x1 + x2) / 2.0f;
    float my12 = (y1 + y2) / 2.0f;
    float mx23 = (x2 + x3) / 2.0f;
    float my23 = (y2 + y3) / 2.0f;
    float mx31 = (x3 + x1) / 2.0f;
    float my31 = (y3 + y1) / 2.0f;

    // Три подтреугольника (центральный не рисуется)
    sierpinski(x1, y1, mx12, my12, mx31, my31, curLevel - 1);
    sierpinski(mx12, my12, x2, y2, mx23, my23, curLevel - 1);
    sierpinski(mx31, my31, mx23, my23, x3, y3, curLevel - 1);
}

// Функция отображения
void display() {
    glClear(GL_COLOR_BUFFER_BIT);
    glColor3f(1.0f, 0.0f, 0.0f);   // красный цвет

    // Координаты большого равностороннего треугольника
    float x1 = -0.5f, y1 = -0.3f;      // левая нижняя вершина
    float x2 =  0.5f, y2 = -0.3f;      // правая нижняя вершина
    float x3 =  0.0f, y3 =  0.6f;      // верхняя вершина

    sierpinski(x1, y1, x2, y2, x3, y3, level);

    glutSwapBuffers();
}

// Функция изменения размеров окна
void reshape(int w, int h) {
    windowWidth = w;
    windowHeight = h;
    glViewport(0, 0, w, h);
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    // Ортогональная проекция: координаты от -1 до 1 по X, от -1 до 1 по Y с учётом пропорций
    if (w <= h)
        gluOrtho2D(-1.0, 1.0, -1.0 * (float)h / w, 1.0 * (float)h / w);
    else
        gluOrtho2D(-1.0 * (float)w / h, 1.0 * (float)w / h, -1.0, 1.0);
    glMatrixMode(GL_MODELVIEW);
}

// Обработка нажатий клавиш
void keyboard(unsigned char key, int x, int y) {
    switch (key) {
        case 'q':
        case 27: // ESC
            exit(0);
            break;
        case '+':
            level++;
            glutPostRedisplay();
            break;
        case '-':
            if (level > 0) level--;
            glutPostRedisplay();
            break;
        default:
            break;
    }
}

int main(int argc, char** argv) {
    cout << "Введите уровень рекурсии (0 и выше): ";
    cin >> level;
    if (level < 0) level = 0;

    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB);
    glutInitWindowSize(windowWidth, windowHeight);
    glutCreateWindow("Треугольник Серпинского");

    glClearColor(0.0f, 0.0f, 0.0f, 1.0f); // чёрный фон

    glutDisplayFunc(display);
    glutReshapeFunc(reshape);
    glutKeyboardFunc(keyboard);

    glutMainLoop();
    return 0;
}