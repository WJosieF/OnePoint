#include <GL/glut.h>
#include <iostream>
#include <cmath>

// 四面体顶点坐标
GLfloat vertices[5][3] = {
    {2.0f, -2.0f, 2.0f},  // P
    {1.0f, 1.0f, 1.0f},   // D
    {0.0f, 1.0f, 0.0f},   // C
    {2.0f, 1.0f, 0.0f},   // B
    {2.0f, 0.0f, 0.0f}    // A
};

int faces[4][3] = {
    {0, 1, 2}, {0, 2, 3}, {0, 3, 4}, {1, 3, 2}
};

GLfloat translateX = 1.0f, translateY = 1.0f, translateZ = 1.0f;

// 视点设置在 (0,2,0)
GLfloat eyeX = 0.0f, eyeY = 2.0f, eyeZ = 0.0f;
// 视点到投影面(XOZ平面)的距离
GLfloat d = 2.0f;  // 视点在y=2，投影面y=0，所以距离为2

int window1, window2;

// 透视投影：将3D点投影到XOZ平面
void projectToXOZ(float x, float y, float z, float& projX, float& projZ) {
    // 根据PPT公式：x' = x * d / (d + y)
    // 因为视点在y轴上，投影面是XOZ平面(y=0)
    float relativeY = y - eyeY;  // 相对于视点的y坐标

    // 防止除以0
    if (d + relativeY == 0) {
        projX = 0;
        projZ = 0;
        return;
    }

    projX = (x - eyeX) * d / (d + relativeY);
    projZ = (z - eyeZ) * d / (d + relativeY);
}

// 绘制坐标轴
void drawAxes(float xMin, float xMax, float zMin, float zMax) {
    glLineWidth(2.0f);

    // X轴 (红色)
    glColor3f(1.0, 0.0, 0.0);
    glBegin(GL_LINES);
    glVertex2f(xMin, 0);
    glVertex2f(xMax, 0);
    glEnd();

    // Z轴 (蓝色)
    glColor3f(0.0, 0.0, 1.0);
    glBegin(GL_LINES);
    glVertex2f(0, zMin);
    glVertex2f(0, zMax);
    glEnd();

    // 坐标轴标签
    glColor3f(1.0, 0.0, 0.0);
    glRasterPos2f(xMax - 0.5, -0.5);
    glutBitmapCharacter(GLUT_BITMAP_HELVETICA_12, 'X');

    glColor3f(0.0, 0.0, 1.0);
    glRasterPos2f(-0.5, zMax - 0.5);
    glutBitmapCharacter(GLUT_BITMAP_HELVETICA_12, 'Z');
}

// 绘制三视图中的一个视图
void drawOneView(int viewType, float xMin, float xMax, float yMin, float yMax,
    const char* xLabel, const char* yLabel) {
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    gluOrtho2D(xMin, xMax, yMin, yMax);
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();

    // 绘制坐标轴
    glLineWidth(2.0f);
    glBegin(GL_LINES);

    // X轴 (红色)
    glColor3f(1.0, 0.0, 0.0);
    glVertex2f(xMin, 0);
    glVertex2f(xMax, 0);

    // Y轴 (绿色) 或 Z轴 (蓝色)
    if (viewType == 2) { // 俯视图用Y轴
        glColor3f(0.0, 1.0, 0.0);
        glVertex2f(0, yMin);
        glVertex2f(0, yMax);
    }
    else { // 主视图和侧视图用Z轴
        glColor3f(0.0, 0.0, 1.0);
        glVertex2f(0, yMin);
        glVertex2f(0, yMax);
    }
    glEnd();

    // 坐标轴标签
    glColor3f(1.0, 0.0, 0.0);
    glRasterPos2f(xMax - 0.5, -0.5);
    glutBitmapCharacter(GLUT_BITMAP_HELVETICA_12, xLabel[0]);

    if (viewType == 2) { // 俯视图
        glColor3f(0.0, 1.0, 0.0);
        glRasterPos2f(-0.5, yMax - 0.5);
        glutBitmapCharacter(GLUT_BITMAP_HELVETICA_12, yLabel[0]);
    }
    else { // 主视图和侧视图
        glColor3f(0.0, 0.0, 1.0);
        glRasterPos2f(-0.5, yMax - 0.5);
        glutBitmapCharacter(GLUT_BITMAP_HELVETICA_12, yLabel[0]);
    }

    // 绘制平移后的四面体
    glPushMatrix();

    // 绘制面
    glBegin(GL_TRIANGLES);
    glColor3f(0.6f, 0.6f, 0.8f);
    for (int i = 0; i < 4; i++) {
        for (int j = 0; j < 3; j++) {
            int idx = faces[i][j];
            float x = vertices[idx][0] + translateX;
            float y = vertices[idx][1] + translateY;
            float z = vertices[idx][2] + translateZ;

            switch (viewType) {
            case 1: // 主视图 (XOZ平面)
                glVertex2f(x, z);
                break;
            case 2: // 俯视图 (XOY平面)
                glVertex2f(x, y);
                break;
            case 3: // 侧视图 (YOZ平面)
                glVertex2f(y, z);
                break;
            }
        }
    }
    glEnd();

    // 绘制顶点
    glPointSize(6.0f);
    glBegin(GL_POINTS);
    glColor3f(1.0, 0.0, 0.0);
    for (int i = 0; i < 5; i++) {
        float x = vertices[i][0] + translateX;
        float y = vertices[i][1] + translateY;
        float z = vertices[i][2] + translateZ;

        switch (viewType) {
        case 1:
            glVertex2f(x, z);
            break;
        case 2:
            glVertex2f(x, y);
            break;
        case 3:
            glVertex2f(y, z);
            break;
        }
    }
    glEnd();

    glPopMatrix();
}

// 三视图显示函数
void displayThreeViews() {
    glutSetWindow(window1);
    glClear(GL_COLOR_BUFFER_BIT);
    glClearColor(1.0, 1.0, 1.0, 1.0);

    // 主视图 (左上，投影到XOZ平面)
    glViewport(0, 250, 250, 250);
    drawOneView(1, -1, 5, -1, 5, "X", "Z");

    // 俯视图 (右上，投影到XOY平面)
    glViewport(250, 250, 250, 250);
    drawOneView(2, -1, 5, -1, 5, "X", "Y");

    // 侧视图 (下方，投影到YOZ平面)
    glViewport(125, 0, 250, 250);
    drawOneView(3, -1, 5, -1, 5, "Y", "Z");

    // 添加标题
    glColor3f(0.0, 0.0, 0.0);

    // 主视图标题
    glViewport(0, 500, 250, 20);
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    gluOrtho2D(0, 250, 0, 20);
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
    glRasterPos2f(10, 10);
    const char* title1 = "主视图(XOZ)";
    for (int i = 0; title1[i] != '\0'; i++) {
        glutBitmapCharacter(GLUT_BITMAP_HELVETICA_12, title1[i]);
    }

    // 俯视图标题
    glViewport(250, 500, 250, 20);
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    gluOrtho2D(0, 250, 0, 20);
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
    glRasterPos2f(10, 10);
    const char* title2 = "俯视图(XOY)";
    for (int i = 0; title2[i] != '\0'; i++) {
        glutBitmapCharacter(GLUT_BITMAP_HELVETICA_12, title2[i]);
    }

    // 侧视图标题
    glViewport(125, 250, 250, 20);
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    gluOrtho2D(0, 250, 0, 20);
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
    glRasterPos2f(10, 10);
    const char* title3 = "侧视图(YOZ)";
    for (int i = 0; title3[i] != '\0'; i++) {
        glutBitmapCharacter(GLUT_BITMAP_HELVETICA_12, title3[i]);
    }

    glutSwapBuffers();
}

// 透视投影显示函数
void displayPerspective() {
    glutSetWindow(window2);
    glClear(GL_COLOR_BUFFER_BIT);
    glClearColor(1.0, 1.0, 1.0, 1.0);

    // 获取当前窗口大小
    int winWidth = glutGet(GLUT_WINDOW_WIDTH);
    int winHeight = glutGet(GLUT_WINDOW_HEIGHT);

    // 设置绘图视口（标题栏下方）
    glViewport(0, 0, winWidth, winHeight - 20);

    // 计算投影后的坐标范围
    float minX = 9999, maxX = -9999, minZ = 9999, maxZ = -9999;
    for (int i = 0; i < 5; i++) {
        float x = vertices[i][0] + translateX;
        float y = vertices[i][1] + translateY;
        float z = vertices[i][2] + translateZ;

        float projX, projZ;
        projectToXOZ(x, y, z, projX, projZ);

        if (projX < minX) minX = projX;
        if (projX > maxX) maxX = projX;
        if (projZ < minZ) minZ = projZ;
        if (projZ > maxZ) maxZ = projZ;
    }

    // 添加边界余量
    float margin = 1.0f;
    minX -= margin;
    maxX += margin;
    minZ -= margin;
    maxZ += margin;

    // 自适应正交投影
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();

    float aspect = (float)(winWidth) / (winHeight - 20);
    float rangeX = maxX - minX;
    float rangeZ = maxZ - minZ;
    float centerX = (minX + maxX) / 2;
    float centerZ = (minZ + maxZ) / 2;

    // 保持宽高比
    if (rangeX / aspect > rangeZ) {
        rangeZ = rangeX / aspect;
    }
    else {
        rangeX = rangeZ * aspect;
    }

    // 设置自适应正交投影
    gluOrtho2D(centerX - rangeX / 2, centerX + rangeX / 2,
        centerZ - rangeZ / 2, centerZ + rangeZ / 2);

    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();

    // 绘制坐标轴
    drawAxes(centerX - rangeX / 2, centerX + rangeX / 2,
        centerZ - rangeZ / 2, centerZ + rangeZ / 2);

    // 绘制三角形面
    glBegin(GL_TRIANGLES);
    glColor3f(0.6f, 0.6f, 0.8f);
    for (int i = 0; i < 4; i++) {
        for (int j = 0; j < 3; j++) {
            int idx = faces[i][j];
            float x = vertices[idx][0] + translateX;
            float y = vertices[idx][1] + translateY;
            float z = vertices[idx][2] + translateZ;

            float projX, projZ;
            projectToXOZ(x, y, z, projX, projZ);

            glVertex2f(projX, projZ);
        }
    }
    glEnd();

    // 绘制顶点
    glPointSize(8.0f);
    glBegin(GL_POINTS);
    glColor3f(1.0, 0.0, 0.0);
    for (int i = 0; i < 5; i++) {
        float x = vertices[i][0] + translateX;
        float y = vertices[i][1] + translateY;
        float z = vertices[i][2] + translateZ;

        float projX, projZ;
        projectToXOZ(x, y, z, projX, projZ);

        glVertex2f(projX, projZ);
    }
    glEnd();

    // 绘制顶点标签
    glColor3f(0.0, 0.0, 0.0);
    const char* labels[5] = { "P", "D", "C", "B", "A" };
    for (int i = 0; i < 5; i++) {
        float x = vertices[i][0] + translateX;
        float y = vertices[i][1] + translateY;
        float z = vertices[i][2] + translateZ;

        float projX, projZ;
        projectToXOZ(x, y, z, projX, projZ);

        glRasterPos2f(projX + 0.1, projZ + 0.1);
        glutBitmapCharacter(GLUT_BITMAP_HELVETICA_12, labels[i][0]);
    }

    // 添加标题
    glViewport(0, winHeight - 20, winWidth, 20);
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    gluOrtho2D(0, winWidth, 0, 20);
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
    glColor3f(0.0, 0.0, 0.0);
    glRasterPos2f(10, 10);
    const char* title = "透视投影 (视点: (0,2,0), 投影面: XOZ)";
    for (int i = 0; title[i] != '\0'; i++) {
        glutBitmapCharacter(GLUT_BITMAP_HELVETICA_12, title[i]);
    }

    glutSwapBuffers();
}

// 窗口大小变化回调函数（用于透视投影窗口）
void reshapePerspective(int w, int h) {
    glutPostRedisplay();
}

void keyboard(unsigned char key, int x, int y) {
    if (key == 27) exit(0);
}

int main(int argc, char** argv) {
    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB);

    // 创建第一个窗口：三视图
    glutInitWindowSize(500, 520);
    glutInitWindowPosition(100, 100);
    window1 = glutCreateWindow("三视图");
    glutDisplayFunc(displayThreeViews);
    glutKeyboardFunc(keyboard);

    // 创建第二个窗口：透视投影
    glutInitWindowSize(500, 520);
    glutInitWindowPosition(650, 100);
    window2 = glutCreateWindow("透视投影");
    glutDisplayFunc(displayPerspective);
    glutReshapeFunc(reshapePerspective);  // 添加窗口大小变化回调
    glutKeyboardFunc(keyboard);

    std::cout << "=== 四面体三视图与透视投影 ===" << std::endl;
    std::cout << "顶点坐标：" << std::endl;
    std::cout << "P(2, -2, 2)" << std::endl;
    std::cout << "D(1, 1, 1)" << std::endl;
    std::cout << "C(0, 1, 0)" << std::endl;
    std::cout << "B(2, 1, 0)" << std::endl;
    std::cout << "A(2, 0, 0)" << std::endl;
    std::cout << "平移向量: (" << translateX << ", " << translateY << ", " << translateZ << ")" << std::endl;
    std::cout << "视点位置: (" << eyeX << ", " << eyeY << ", " << eyeZ << ")" << std::endl;
    std::cout << "投影面: XOZ平面 (Y=0)" << std::endl;
    std::cout << "\n打开了两个窗口：" << std::endl;
    std::cout << "1. 三视图窗口" << std::endl;
    std::cout << "2. 透视投影窗口（透视投影到XOZ平面的2D图）" << std::endl;
    std::cout << "\n按ESC键退出程序" << std::endl;

    glutMainLoop();
    return 0;
}