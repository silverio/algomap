#ifndef __VORONOI_ANIM_H__
#define __VORONOI_ANIM_H__

#include <geom.h>
#include <voronoi.h>

class VoronoiAnim
{
public:
    VoronoiAnim() : animSpeed(0.1f), m_lineY(0.0f) {}

    void step(float dt, Voronoi& v);
    void draw(const Rect& ext, const Voronoi& v);

    float   animSpeed;

private:
    float   m_lineY;
};

#endif // __VORONOI_ANIM_H__
