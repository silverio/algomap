#include <stdio.h> 

#include <glpainter.h>
#include <imgui.h>
#include <algomap.h>


#include <GL/glut.h>

AlgoMap algomap;

unsigned int screenWidth = 0;
unsigned int screenHeight = 0;

static void display(void)
{
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    algomap.draw(Rect(0, 0, screenWidth, screenHeight));
    g_IMGUI.frame();

    glutSwapBuffers();
} 

static void resize(int width, int height)
{
    screenWidth = width;
    screenHeight = height;

    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    gluOrtho2D(0, width, 0, height);
    
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
    glScalef(1, -1, 1);
    glTranslatef(0.5f, -height - 0.5f, 0); // half pixel offset

    glViewport(0, 0, width, height);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    glEnable(GL_LINE_SMOOTH);
    glHint(GL_LINE_SMOOTH_HINT, GL_NICEST);

    algomap.onResize();
} 

double getCurTime()
{
    return ((double)glutGet(GLUT_ELAPSED_TIME))/1000.0;
}

static void update(int value)
{
    static double lastTime = getCurTime();
    double curTime = getCurTime();
    double dt = curTime - lastTime;
    lastTime = curTime;
    
    g_IMGUI.onTimeStep(dt);

    glutTimerFunc(25, update, 0);
    glutPostRedisplay();
} 

void onMouse(int button, int state, int x, int y)
{
    g_IMGUI.onMouseClick(x, y);
}

static const int WINDOW_WIDTH  = 1024;
static const int WINDOW_HEIGHT = 768;

int main(int argc, char *argv[])
{
    g_pGLPainter = new GLPainter();

    glutInit(&argc, argv);

    glutInitWindowPosition((glutGet(GLUT_SCREEN_WIDTH) - WINDOW_WIDTH)/2, 
        (glutGet(GLUT_SCREEN_HEIGHT) - WINDOW_HEIGHT)/2);
    glutInitWindowSize(1024, 768);
    
    glutInitDisplayMode(GLUT_RGB | GLUT_DOUBLE | GLUT_DEPTH);
    glutCreateWindow("Algomap - Oslo Downtown");

    glutReshapeFunc(resize);
    glutDisplayFunc(display);
    glutMouseFunc(onMouse);
    glutTimerFunc(25, update, 0);
    
    algomap.init();
    
    glutMainLoop();

    delete g_pGLPainter;
    return 0;
}

