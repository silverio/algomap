#ifndef __VORONOI_H__
#define __VORONOI_H__

#include <geom.h>

class Voronoi
{
public:
    Voronoi() {}
    
    void init(const Point* sites, int numSites, const Rect& bounds);

    bool step();

};

#endif
