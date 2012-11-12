
#include <imgui.h>
#include <glpainter.h>
#include <colorf.h>

IMGUI g_IMGUI;


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

static const float FONT_HEIGHT = 10.0f;

void IMGUI::onMouseAction(MouseAction action, MouseButton button, float x, float y)
{
    m_MouseAction = action;
    m_MouseX = x;
    m_MouseY = y;
}

bool IMGUI::isMouseAction(const Rect& ext, unsigned int actionFlags, unsigned int buttonFlags)
{
    return ((actionFlags & m_MouseAction) != 0) && ext.contains(m_MouseX, m_MouseY);
}
    
void IMGUI::frame()
{
    m_MouseAction = Mouse_NoAction;
    m_DeltaTimeSec = 0.0;
}

bool IMGUI::checkBox(const Rect& ext, const char* text, bool& bPressed)
{
    bool bModified = false;
    if (isMouseAction(ext, Mouse_Down, Mouse_Left))
    {
        bPressed = !bPressed;
        bModified = true;
    }
    Rect thumbExt(ext.l, ext.t, ext.l + ext.h(), ext.b);
    thumbExt.inflate(-1.0f);
    Rect textExt(thumbExt.r + 5, ext.t, ext.r, ext.b);
    
    drawBevel(thumbExt, 0xFFEEEEEE, 0xFF848284);
    if (bPressed)
    {
        panel(thumbExt.inflated(-2), 0xFFDDDDDD, true);
    }

    float textY = textExt.t + (textExt.b - textExt.t - FONT_HEIGHT)*0.5f + FONT_HEIGHT;
    g_pGLPainter->drawText(textExt.l, textY, text, 0xFFDDDDDD);
    return bModified;    
}

bool IMGUI::slider(const Rect& ext, float minVal, float maxVal, float& val)
{
    bool bModified = false;
    if (isMouseAction(ext, Mouse_Down|Mouse_Move, Mouse_Left))
    {
        val = clamp(minVal + (maxVal - minVal)*(m_MouseX - ext.l)/ext.w(), minVal, maxVal);
        bModified = true;
    }

    bool bChanged = false;
    Rect bgExt(ext);
    bgExt.inflate(0.0f, -5.0f, 0.0f, -5.0f);
    panel(bgExt, 0xFFDDDDDD);
    float spos = clamp(val/(maxVal - minVal), 0.0f, 1.0f);
    float tx = ext.l + ext.w()*spos;
    Rect thumbExt(tx - 2.0f, ext.t, tx + 2.0f, ext.b);
    panel(thumbExt, 0xFFDDDDDD);
    return bModified;
}

bool IMGUI::toggleButton(const Rect& ext, const char* text, bool& bPressed)
{
    bool bModified = false;
    if (isMouseAction(ext, Mouse_Down, Mouse_Left))
    {
        bPressed = !bPressed;
        bModified = true;
    }
    Color textColor  = bPressed ? 0xFFDDDDDD : 0xFF222222;
    Color panelColor = bPressed ? 0xFF222222 : 0xFFDDDDDD;
    panel(ext, panelColor);

    float textWidth = (float)g_pGLPainter->getTextWidth(text);
    float textY = ext.t + (ext.b - ext.t - FONT_HEIGHT)*0.5f + FONT_HEIGHT;
    float textX = ext.l + (ext.r - ext.l - textWidth)*0.5f;
    g_pGLPainter->drawText(textX, textY, text, textColor);
    return bModified;
}

bool IMGUI::listBox(const Rect& ext, float rowHeight, const std::vector<std::string>& items, int& selIdx)
{
    bool selChanged = false;

    size_t nItems = items.size();
    float width = ext.r - ext.l;
    for (size_t i = 0; i < nItems; i++)
    {
        if (isMouseAction(Rect(ext.l, ext.t + i*rowHeight, ext.r, ext.t + (i + 1)*rowHeight), Mouse_Down, Mouse_Left) &&
            selIdx != i)
        {
            selChanged = true;
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

    return selChanged;
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

