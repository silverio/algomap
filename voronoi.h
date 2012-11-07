#ifndef __VORONOI_H__
#define __VORONOI_H__

#include <queue>
#include <geom.h>

class Voronoi
{
public:
    Voronoi() {}
    
    void init(const Point* sites, int numSites, const Rect& bounds);

    bool step();

    void run();

    struct FArc
    {
        Point pt;
    };

    typedef std::vector<FArc> BeachLine;

private:
    friend class VoronoiAnim;

    //  "Event" data structure for the Fortune's algorithm
    struct FEvent
    {
        enum Type { Site, Circle };
        bool operator <(const FEvent& ev) const { return pt.y < ev.pt.y; }

        Point   pt;     //  corresponding point (site or vertex)
        Type    type;   //  event type
    };

    std::priority_queue<FEvent> m_events;
    BeachLine                   m_beachLine;

    const Point*                m_pSites;
    size_t                      m_numSites;
};

#endif
