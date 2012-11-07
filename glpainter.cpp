#include <assert.h>
#include <stdlib.h>

#include <GL/glut.h>

#include <glpainter.h>
#include <image.h>

GLPainter* g_pGLPainter = NULL;

void* FONT = GLUT_BITMAP_HELVETICA_12;

int GLPainter::getTextWidth(const char* text) const
{
    assert(text);
    return glutBitmapLength(FONT, reinterpret_cast<const unsigned char*>(text));
}

void GLPainter::drawText(float x, float y, const char* text, Color color) const
{
    assert(text);
    glColor4ubv(reinterpret_cast<const GLubyte*>(&color));
    glRasterPos2f(x, y);
    
    const char* c = text;
    while (*c != '\0')
    {
        glutBitmapCharacter(FONT, *c);
        c++;
    }
}

void GLPainter::drawLine(float x1, float y1, float x2, float y2, Color color, float width) const
{
    glColor4ubv(reinterpret_cast<const GLubyte*>(&color));
    glLineWidth(width);

    glBegin(GL_LINES); 
    glVertex2f(x1, y1);
    glVertex2f(x2, y2); 
    glEnd();
}

void GLPainter::drawTriangle(float x1, float y1, float x2, float y2, float x3, float y3, Color color) const
{
    glColor4ubv(reinterpret_cast<const GLubyte*>(&color));

    glBegin(GL_TRIANGLES); 
    glVertex2f(x1, x2);
    glVertex2f(x2, y2); 
    glVertex2f(x3, y3); 
    glEnd();
}

void GLPainter::drawCircle(float x, float y, float r, Color color, int numSegments) const
{
    glColor4ubv(reinterpret_cast<const GLubyte*>(&color));

    assert(numSegments > 0);
    static const float PI = 3.1415926f;
    float dphi = 2.0f*PI/float(numSegments);

    float c = cosf(dphi);
    float s = sinf(dphi);
    
    float px = r;
    float py = 0.0f; 

    glBegin(GL_LINE_LOOP); 
    for (int i = 0; i < numSegments; i++) 
    { 
        glVertex2f(x + px, y + py);
        float rx = c*px - s*py;
        float ry = s*px + c*py;
        px = rx; py = ry;
    } 
    glEnd(); 
}

void GLPainter::drawRect(float xLeft, float yTop, float xRight, float yBottom, Color color) const
{
    glColor4ubv(reinterpret_cast<const GLubyte*>(&color));

    glBegin(GL_QUADS);
    glTexCoord2f(0.0f, 1.0f);
    glVertex2f(xLeft, yTop);

    glTexCoord2f(1.0f, 1.0f);
    glVertex2f(xRight, yTop);

    glTexCoord2f(1.0f, 0.0f);
    glVertex2f(xRight, yBottom);

    glTexCoord2f(0.0f, 0.0f);
    glVertex2f(xLeft, yBottom);

    glEnd();
}

unsigned int GLPainter::loadTexture(const char* fileName) const
{
    Image image;
    if (!LoadTGA(fileName, image))
    {
        return -1;
    }

    GLuint texID;

    glEnable(GL_TEXTURE_2D);
    glGenTextures(1, &texID);
    glBindTexture(GL_TEXTURE_2D, texID);
    
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP);

    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, image.width, image.height, 0, GL_RGBA, 
        GL_UNSIGNED_BYTE, image.rgba.size() ? &image.rgba[0] : 0);

    return texID;
}

void GLPainter::setTexture(unsigned int texID) const
{
    if (texID > 0)
    {
        glEnable(GL_TEXTURE_2D);
        glBindTexture(GL_TEXTURE_2D, texID);
    }
    else
    {
        glDisable(GL_TEXTURE_2D);
    }
}

void GLPainter::setBlendMode(BlendMode mode) const
{
    if (mode == BlendMode_None)
    {
        glDisable(GL_BLEND);
    }
    else
    {
        glEnable(GL_BLEND);
        if (mode == BlendMode_Normal)
        {
            glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
        }
        else if (mode == BlendMode_Additive)
        {
            glBlendFunc(GL_SRC_ALPHA, GL_ONE);
        }
    }
}
