//  Implements Stephen J. Fortune's sweeping line algorithm for building Voronoi diagram
#include <geom.h>
#include <assert.h>

#include <voronoi.h>

static const float EPSILON = 1e-7f;

//  the beachfront parabola equation
float parabolaPoint(const Point& c, float yL, float x)
{
    return ((x - c.x)*(x - c.x) + c.y*c.y - yL*yL)*0.5f/(c.y - yL);
}

//  finds the point where two sites' parabolas meet
bool computeBreakPoint(const Point& p0, const Point& p1, float yL, float& bpx)
{
    float A = p1.y - p0.y;
    if (fabs(A) < EPSILON)
    {
        //  parabolas' sites are on the horizontal line, so breakpoint is in the middle
        bpx = 0.5f*(p0.x + p1.x);
        return true;
    }
    float B = p1.y - yL;
    float C = p0.y - yL;
    float b = 2.0f*(C*p1.x - B*p0.x);
    float c = B*p0.x*p0.x - C*p1.x*p1.x - A*B*C;
    // solve the quadratic equation
    float D = b*b - 4.0f*A*c;
    if (D < 0.0f)
    {
        return false;
    }
    //  taking the "closer" root
    bpx = (-b + sqrtf(D))*0.5f/A;
    return true;
}
    
//  returns the "circle event" point, which is the bottom of the circle through 3 given points
bool circleEventPoint(const Point& p0, const Point& p1, const Point& p2, Point& xc, float* pR)
{
    float A = p1.x - p0.x;
    float B = p1.y - p0.y;
    float C = p2.x - p0.x;
    float D = p2.y - p0.y;
    float E = A*(p0.x + p1.x) + B*(p0.y + p1.y);
    float F = C*(p0.x + p2.x) + D*(p0.y + p2.y);
    float G = 2.0f*(A*(p2.y - p1.y) - B*(p2.x - p1.x));
    if (fabs(G) < EPSILON)
    {
        // all three points are on the same line
        return false;
    }
    float x0 = (D*E - B*F)/G;
    float y0 = (A*F - C*E)/G;
    float r = sqrtf((x0 - p0.x)*(x0 - p0.x) + (y0 - p0.y)*(y0 - p0.y));
    
    xc.x = x0;
    xc.y = y0 - r;

    if (pR) *pR = r;
    return true;
}

// binary searches for the site, corresponding to the given coordinate x on the beachfront 
int findArc(float x, float yL, const Voronoi::BeachLine& beachLine)
{
    size_t n = beachLine.size();
    size_t l = 0;
    size_t r = n - 1;
    float bpx;
    while (r > l)
    {
        size_t k = (l + r)/2;
        const Point& p0 = beachLine[k]->pt;
        const Point& p1 = beachLine[k + 1]->pt;
        bool hasBP = computeBreakPoint(p0, p1, yL, bpx);
        if (!hasBP) return -1;
        if (bpx < x) l = k + 1; else r = k;
    }
    return l;
}

void Voronoi::reset()
{
    // beach line size can not grow bigger than 2n-1
    m_beachLine.clear();
    m_beachLine.reserve(2*m_numSites - 1); 

    //  add all sites as "site events" to the event queue
    m_events.swap(std::priority_queue<FEvent>());
    for (size_t i = 0; i < m_numSites; i++)
    {
        FEvent ev;
        ev.pt = m_pSites[i];
        ev.type = FEvent::Site;
        m_events.push(ev);
    }
}

void Voronoi::init(const Point* sites, int numSites, const Rect& bounds)
{
    m_numSites = numSites;
    m_pSites = sites;

    reset();
}

bool Voronoi::checkCircleEvent(int startArc)
{
    Point xc;
    if (startArc >= 0 && startArc < (int)m_beachLine.size() - 2 &&
        circleEventPoint(m_beachLine[startArc]->pt, m_beachLine[startArc + 1]->pt, 
        m_beachLine[startArc + 2]->pt, xc))
    {
        FEvent cev;
        cev.type = FEvent::Circle;
        cev.pt = xc;
        cev.arc = m_beachLine[startArc + 1];
        m_events.push(cev);
        return true;
    }
    return false;
}

int Voronoi::getArcIdx(const FArc* arc) const
{
    size_t nArc = 0;
    size_t nPt = m_beachLine.size();
    while (nArc < nPt)
    {
        if (m_beachLine[nArc] == arc)
        {
            return nArc;
        }
        nArc++;
    }
    return -1;
}

bool Voronoi::step()
{
    if (m_events.empty())
    {
        return false;
    }

    FEvent ev = m_events.top();
    m_events.pop();
    if (ev.type == FEvent::Site)
    {
        //  handle the site event
        FArc* pNewArc = new FArc();
        pNewArc->pt = ev.pt;
        if (m_beachLine.size() != 0)
        {
            int arcIdx = findArc(ev.pt.x, ev.pt.y, m_beachLine);
            assert(arcIdx >= 0);
            FArc* pHitArc = m_beachLine[arcIdx];
            //  break the arc
            
            m_beachLine.insert(m_beachLine.begin() + arcIdx, pNewArc);
            //  duplicate the existing arc
            FArc* pDupArc = new FArc(*pHitArc);
            m_beachLine.insert(m_beachLine.begin() + arcIdx, pDupArc);

            //  create the new dangling edge
            //  ...

            //  check/add the potential circle events
            checkCircleEvent(arcIdx - 1);
            checkCircleEvent(arcIdx + 1);
        }
        else
        {
            //  this is the very first point in the beach line
            m_beachLine.push_back(pNewArc);
        }
    }
    else
    {
        //  handle circle event
        //  remove the corresponding arc from the beachline
        //  FIXME:
        int nArc = getArcIdx(ev.arc);
        if (nArc >= 0)
        {
            m_beachLine.erase(m_beachLine.begin() + nArc);
            //  create new vertex in the voronoi diagram, start the new dangling edge

            //  check for the new potential circle events
            checkCircleEvent(nArc - 1);
            checkCircleEvent(nArc);
        }
    }

    return true;
}


void Voronoi::run()
{
    while (step());
}
