#ifndef __IMGUI_H__
#define __IMGUI_H__

#include <vector>
#include <string>

#include <glpainter.h>
#include <geom.h>

//  Rudimentary "Immediate mode" GUI
class IMGUI
{
public:
    enum MouseAction
    {
        Mouse_NoAction  = 0,
        Mouse_Down      = 1,
        Mouse_Up        = 2,
        Mouse_Move      = 3
    };

    enum MouseButton
    {
        Mouse_NoButton  = 0x00,
        Mouse_Left      = 0x01,
        Mouse_Middle    = 0x02,
        Mouse_Right     = 0x04
    };

    bool listBox(const Rect& ext, float rowHeight, const std::vector<std::string>& items, int& selIdx);
    bool toggleButton(const Rect& ext, const char* text, bool& bPressed);
    bool checkBox(const Rect& ext, const char* text, bool& bPressed);

    void panel(const Rect& ext, Color color, bool colorBorder = true);
    void label(const Rect& ext, const char* text, Color color);

    void onMouseAction(MouseAction action, MouseButton button, float x, float y);
    bool isMouseClicked(const Rect& ext);
    void onTimeStep(double dtSec) { m_DeltaTimeSec = dtSec; }

    void frame();
    
    double getDeltaTime() const { return m_DeltaTimeSec; }

private:
    float m_MouseX, m_MouseY;
    bool  m_bMouseClicked;

    double m_DeltaTimeSec;
};

extern IMGUI g_IMGUI;

#endif
