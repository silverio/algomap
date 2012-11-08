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
    void reset();

    struct FArc
    {
        FArc() : active(true) {}

        bool    active;
        Point   pt;
    };

    typedef std::vector<FArc*> BeachLine;

private:
    bool checkCircleEvent(int startArc);
    int getArcIdx(const FArc* arc) const;

    friend class VoronoiAnim;

    //  "Event" data structure for the Fortune's algorithm
    struct FEvent
    {
        FEvent() : arc(0) {}

        enum Type { Site, Circle };
        bool operator <(const FEvent& ev) const { return pt.y < ev.pt.y; }

        Point   pt;         //  corresponding point (site or circle)
        Type    type;       //  event type
        FArc*   arc;        //  the arc that created the event (for the circle events)
    };

    std::priority_queue<FEvent> m_events;
    BeachLine                   m_beachLine;

    const Point*                m_pSites;
    size_t                      m_numSites;
};

float parabolaPoint(const Point& c, float yL, float x);
bool computeBreakPoint(const Point& p0, const Point& p1, float yL, float& bpx);
bool circleEventPoint(const Point& p0, const Point& p1, const Point& p2, Point& xc, float* pR = NULL);
int findArc(float x, float yL, const Voronoi::BeachLine& beachLine);

#endif
