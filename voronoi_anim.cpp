#include <assert.h>

#include <voronoi_anim.h>
#include <voronoi.h>
#include <glpainter.h>
#include <imgui.h>

VoronoiAnim::VoronoiAnim() : 
    animSpeed   (0.03f), 
    m_lineY     (1.0f), 
    paused      (false), 
    pauseOnEvent(false) 
{
}

void VoronoiAnim::reset()
{
    m_lineY = 1.0f;
}

void VoronoiAnim::drawArc(const Rect& ext, size_t arcIdx, const Voronoi::BeachLine& bl, ColorF color) const
{
    //  the arc center point
    const Point& c = bl[arcIdx]->pt;
    Point pt = ext.mapFromUnit(c);
    float r = 5;
    g_pGLPainter->drawRect(pt.x - r, pt.y - r, pt.x + r, pt.y + r, color);

    //  find the arc limits (breakpoints)
    float a = 0.0f;
    float b = 1.0f;
    if (arcIdx > 0) computeBreakPoint(bl[arcIdx - 1]->pt, bl[arcIdx]->pt, m_lineY, a);
    if (arcIdx < bl.size() - 1) computeBreakPoint(bl[arcIdx]->pt, bl[arcIdx + 1]->pt, m_lineY, b);
    
    // draw the arc
    static const float STEP = 0.01f;
    float x = a;
    float y = parabolaPoint(c, m_lineY, x);
    while (x < b)
    {
        float x1 = std::min(x + STEP, b);
        float y1 = parabolaPoint(c, m_lineY, x1);
        Point p1 = ext.mapFromUnit(Point(x, y));
        Point p2 = ext.mapFromUnit(Point(x1, y1));
        g_pGLPainter->drawLine(p1.x, p1.y, p2.x, p2.y, color, 2);
        x = x1;
        y = y1;
    }
}


void VoronoiAnim::draw(const Rect& ext, Voronoi& v)
{
    g_pGLPainter->setTexture(0);
    
    //  draw the sweep line
    float y = m_lineY*(ext.b - ext.t) + ext.t;
    g_pGLPainter->drawLine(ext.l, y, ext.r, y, 0xAA557755, 3); 

    //  draw the event queue points
    std::priority_queue<Voronoi::FEvent> events  = v.m_events;
    while (!events.empty())
    {
        Voronoi::FEvent e = events.top();
        Point pt = ext.mapFromUnit(e.pt);
        events.pop();
        
        Color color = 0xFFFF00FF;
        if (e.type == Voronoi::FEvent::Circle)
        {
            //  draw the event circle
            int nArc = v.getArcIdx(e.arc);
            if (nArc >= 0)
            {
                float R;
                Point xe;
                if (circleEventPoint(v.m_beachLine[nArc - 1]->pt, 
                                     v.m_beachLine[nArc    ]->pt,
                                     v.m_beachLine[nArc + 1]->pt, xe, &R))
                {
                    //assert(fabs(xc.x - e.pt.x) < 1e-5f && fabs(xc.y - e.pt.y) < 1e-5f);
                    Point mxe = ext.mapFromUnit(xe);
                    Point mxc = ext.mapFromUnit(Point(xe.x, xe.y + R));
                    g_pGLPainter->drawCircle(mxc.x, mxc.y, mxc.y - mxe.y, 0xAA00FFFF, 1);
                    g_pGLPainter->drawRect(mxc.x - 5, mxc.y - 5, mxc.x + 5, mxc.y + 5, 0xFF000000);
                }
                color = 0xFF00FFFF;
            }
            else color = 0xFF888888;
        }
        
        float r = 4;
        g_pGLPainter->drawRect(pt.x - r, pt.y - r, pt.x + r, pt.y + r, color);
    }

    //  draw the beachline
    size_t nPt = v.m_beachLine.size();
    for (size_t i = 0; i < nPt; i++)
    {
        drawArc(ext, i, v.m_beachLine, 0xFFFF00FF);
    }

    //  draw the diagram built so far
    // ...

    //  draw the current event
    float dy = (float)g_IMGUI.getDeltaTime()*(paused ? 0.0f : animSpeed);
    while (!v.m_events.empty())
    {
        Voronoi::FEvent topEv = v.m_events.top();
        if (topEv.pt.y < m_lineY - dy)
        {
            break;
        }
        //  the sweep line is about to pass the next event

        //  draw the event graphics
        if (topEv.type == Voronoi::FEvent::Site)
        {
            //  draw the tracing line to the corresponding arc
            if (v.m_beachLine.size() > 0)
            {
                int nArc = findArc(topEv.pt.x, m_lineY, v.m_beachLine);
                if (nArc >= 0)
                {
                    drawArc(ext, nArc, v.m_beachLine, 0xFF0000FF);
                    Point a = ext.mapFromUnit(Point(topEv.pt.x, 
                        parabolaPoint(v.m_beachLine[nArc]->pt, m_lineY, topEv.pt.x)));
                    Point b = ext.mapFromUnit(Point(topEv.pt.x, m_lineY));
                    g_pGLPainter->drawLine(a.x, a.y, b.x, b.y, 0xFF000000, 3.0f);
                }
            }
        }

        //  pause the animation
        if (pauseOnEvent) 
        { 
            paused = true;
            dy = 0.0f;
        }
        if (!paused)
        {
            // step the algorithm
            v.step();
        }
    }
    m_lineY -= dy;
        
    if (m_lineY < 0.0f) 
    {
        m_lineY = 1.0f;
        v.reset();
    }
}
