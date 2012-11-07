#ifndef __ALGOMAP_H__
#define __ALGOMAP_H__

#include <string>
#include <vector>

#include <geom.h>

#include <voronoi.h>
#include <voronoi_anim.h>


//  The "model" of the application
class AlgoMap
{
public:
    AlgoMap();

    enum Algorithm
    {
        Algorithm_None          = 0,
        Algorithm_HeatMap       = 1,
        Algorithm_Voronoi       = 2,
        Algorithm_VoronoiAnim   = 3,
    };
    
    void init();
    void draw(const Rect& ext);
    void onResize();

private:
    bool loadPOI(const char* fileName);

    void resetCurAlgorithm();
    
    void drawHeatMap(const Rect& ext);
    void drawPOI(const Rect& ext);

    typedef std::vector<Point> PointList;

    std::vector<std::string>    m_poiNames;
    std::vector<PointList>      m_poiLocations;
    
    int                         m_curPOI;
    int                         m_curAlgorithm;

    std::vector<std::string>    m_algorithmNames;

    unsigned int                m_bgImage;
    unsigned int                m_blobImage;
    unsigned int                m_knobImage;

    float                       m_heatMapBlobRadius;
    float                       m_knobRadius;

    Voronoi                     m_Voronoi;
    VoronoiAnim                 m_VoronoiAnim;
};


#endif
