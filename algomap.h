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

    typedef std::vector<Point> PointList;

    std::vector<std::string>    poiNames;
    std::vector<PointList>      poiLocations;
    
    int                         curPOI;
    int                         curAlgorithm;

    std::vector<std::string>    algorithmNames;

    unsigned int                bgImage;
    unsigned int                blobImage;
    unsigned int                knobImage;

    float                       heatMapBlobRadius;

private:
    bool loadPOI(const char* fileName);
   

    Voronoi                     m_Voronoi;
    VoronoiAnim                 m_VoronoiAnim;
};


#endif
