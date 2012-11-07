#ifndef __GLPAINTER_H__
#define __GLPAINTER_H__

typedef unsigned int Color;

//  Interface to OpenGL painting facilities.
//  Note that it's not OpenGL-specific, while the implementation is
class GLPainter
{
public:
    enum BlendMode
    {
        BlendMode_None        = 0,
        BlendMode_Normal      = 1,
        BlendMode_Additive    = 2
    };

    void drawLine(float x1, float y1, float x2, float y2, Color color, float width = 1.0f) const;
    void drawRect(float xLeft, float yTop, float xRight, float yBottom, Color color) const;
    void drawTriangle(float x1, float y1, float x2, float y2, float x3, float y3, Color color) const;
    void drawCircle(float x, float y, float r, Color color, int numSegments = 256) const;
    
    void drawText(float x, float y, const char* text, Color color) const;
    int getTextWidth(const char* text) const;

    unsigned int loadTexture(const char* fileName) const;
    void setTexture(unsigned int texID) const;
    void setBlendMode(BlendMode mode) const;
};

extern GLPainter* g_pGLPainter;

#endif
