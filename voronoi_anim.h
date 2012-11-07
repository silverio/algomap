#ifndef __VORONOI_ANIM_H__
#define __VORONOI_ANIM_H__

#include <geom.h>
#include <voronoi.h>

#include <colorf.h>

class VoronoiAnim
{
public:
    VoronoiAnim();

    void draw(const Rect& ext, Voronoi& v);
    void reset();

    float   animSpeed;
    bool    paused;
    bool    pauseOnEvent;

private:
    float   m_lineY;

    void drawArc(const Rect& ext, size_t arcIdx, const Voronoi::BeachLine& bl, ColorF color) const;
};

#endif // __VORONOI_ANIM_H__
