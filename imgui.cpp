
#include <imgui.h>
#include <glpainter.h>
#include <colorf.h>

static const float FONT_HEIGHT = 10.0f;

void IMGUI::onMouseClick(float x, float y)
{
    m_bMouseClicked = true;
    m_MouseX = x;
    m_MouseY = y;
}

bool IMGUI::isMouseClicked(const Rect& ext)
{
    return m_bMouseClicked && ext.contains(m_MouseX, m_MouseY);
}
    
void IMGUI::frame()
{
    m_bMouseClicked = false;
}

void IMGUI::listBox(const Rect& ext, float rowHeight, const std::vector<std::string>& items, int& selIdx)
{
    size_t nItems = items.size();
    float width = ext.r - ext.l;
    for (size_t i = 0; i < nItems; i++)
    {
        if (isMouseClicked(Rect(ext.l, ext.t + i*rowHeight, ext.r, ext.t + (i + 1)*rowHeight)))
        {
            selIdx = i;
        }
    }

    float y = ext.t;
    for (size_t i = 0; i < nItems; i++)
    {
        float textWidth = (float)g_pGLPainter->getTextWidth(items[i].c_str());
        Color textColor = 0xFFDDDDDD;

        if (i == selIdx)
        {
            panel(Rect(ext.l, y, ext.r, y + rowHeight), textColor);
            textColor = 0xFF222222;
        }
        float textY = y + (rowHeight - FONT_HEIGHT)*0.5f + FONT_HEIGHT;
        float textX = ext.l + (width - textWidth)*0.5f;
        g_pGLPainter->drawText(textX, textY, items[i].c_str(), textColor);
        y += rowHeight;
    }
}

void drawBevel(const Rect& ext, Color topColor, Color bottomColor, float width = 1.0f, bool isTop = true)
{
    if (isTop)
    {
        g_pGLPainter->drawLine(ext.l, ext.t, ext.r, ext.t, topColor,    width );
        g_pGLPainter->drawLine(ext.l, ext.t, ext.l, ext.b, topColor,    width );
        g_pGLPainter->drawLine(ext.l, ext.b, ext.r + 0.5f, ext.b, bottomColor, width );
        g_pGLPainter->drawLine(ext.r, ext.t, ext.r, ext.b, bottomColor, width );
    }
    else
    {
        g_pGLPainter->drawLine(ext.l, ext.t, ext.r, ext.t, topColor,    width );
        g_pGLPainter->drawLine(ext.l, ext.t, ext.l, ext.b, topColor,    width );
        g_pGLPainter->drawLine(ext.l, ext.b, ext.r + 0.5f, ext.b, bottomColor, width );
        g_pGLPainter->drawLine(ext.r, ext.t, ext.r, ext.b, bottomColor, width );
    }
} 

void IMGUI::panel(const Rect& ext, Color color, bool colorBorder)
{
    g_pGLPainter->setTexture(0);

    Color topColor = 0xFFEEEEEE;
    Color botColor = 0xFF848284;
    if (colorBorder)
    {
        ColorF c(color);
        topColor = c*1.4f;
        botColor = c*0.7f;
    }

    Rect ext1 = ext.inflated(-1.0f, -1.0f, 0.0f, 0.0f);
    g_pGLPainter->drawRect(ext1.l, ext1.t, ext1.r, ext1.b, color);
    ext1.inflate(1.0f, 1.0f, 0.0f, 0.0f);
    drawBevel(ext1, topColor, botColor, true);
}

void IMGUI::label(const Rect& ext, const char* text, Color color)
{
    float x = ext.l + 5.0f;
    float y = ext.b - FONT_HEIGHT - 3.0f;
    g_pGLPainter->drawText(x, y, text, color);
}

