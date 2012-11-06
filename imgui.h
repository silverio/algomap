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
    void listBox(const Rect& ext, float rowHeight, const std::vector<std::string>& items, int& selIdx);
    void panel(const Rect& ext, Color color, bool colorBorder = true);
    void label(const Rect& ext, const char* text, Color color);

    void onMouseClick(float x, float y);
    bool isMouseClicked(const Rect& ext);
    
    void frame();

private:
    float m_MouseX, m_MouseY;
    bool  m_bMouseClicked;
};

#endif
