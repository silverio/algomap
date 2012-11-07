#include <voronoi_anim.h>
#include <voronoi.h>
#include <glpainter.h>
#include <imgui.h>

void VoronoiAnim::draw(const Rect& ext, const Voronoi& v)
{
    g_pGLPainter->setTexture(0);

    //  draw sweep line
    float y = m_lineY*(ext.b - ext.t) + ext.t;
    g_pGLPainter->drawLine(ext.l, y, ext.r, y, 0xAA557755, 3); 

    //  draw event queue
    std::priority_queue<Voronoi::FEvent> ev  = v.m_events;
    while (!ev.empty())
    {
        Point pt = ext.mapFromUnit(ev.top().pt);
        ev.pop();
        float r = 2;
        g_pGLPainter->drawRect(pt.x - r, pt.y - r, pt.x + r, pt.y + r, 0xFF0000FF);
    }

    m_lineY -= (float)g_IMGUI.getDeltaTime()*animSpeed;
    
    if (m_lineY < 0.0f) m_lineY = 1.0f;
}
