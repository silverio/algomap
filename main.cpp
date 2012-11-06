#include <stdio.h> 

#include <glpainter.h>
#include <imgui.h>
#include <algomap.h>


#include <GL/glut.h>

IMGUI imgui;
AlgoMap algomap;


unsigned int screenWidth = 0;
unsigned int screenHeight = 0;

static void display(void)
{
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    Rect ext(1024 - 902, 0, screenWidth, screenHeight - 68);

    algomap.draw(ext);

    imgui.panel(Rect(0, 0, ext.l, screenHeight).inflated(-2), 0xFF444477);
    imgui.listBox(Rect(0, 0, ext.l, screenHeight).inflated(-4), 15.0f, algomap.poiNames, algomap.curPOI);
    imgui.listBox(Rect(0, 600, ext.l, screenHeight).inflated(-4), 15.0f, algomap.algorithmNames, algomap.curAlgorithm);

    Rect legendExt(ext.l, ext.b, screenWidth, screenHeight);
    imgui.panel(legendExt.inflated(-1), 0xFF444455);
    imgui.label(legendExt, "Select POI in the listbox to the left, also choose the"\
        " algorithm to apply from the second listbox.", 0xFFFFFFFF);

    imgui.frame();

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

static void update(int value)
{
    int time = (int)glutGet(GLUT_ELAPSED_TIME) / 1000.0;
    glutTimerFunc(25, update, 0);
    glutPostRedisplay();
} 

void onMouse(int button, int state, int x, int y)
{
    imgui.onMouseClick(x, y);
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

