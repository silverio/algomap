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
    bool listBox(const Rect& ext, float rowHeight, const std::vector<std::string>& items, int& selIdx);
    bool toggleButton(const Rect& ext, const char* text, bool& bPressed);
    bool checkBox(const Rect& ext, const char* text, bool& bPressed);

    void panel(const Rect& ext, Color color, bool colorBorder = true);
    void label(const Rect& ext, const char* text, Color color);

    void onMouseClick(float x, float y);
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
