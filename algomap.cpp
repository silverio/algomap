#include <algomap.h>

#include <fstream>
#include <sstream>

#include <glpainter.h>

AlgoMap::AlgoMap() : 
    curPOI              (18),
    curAlgorithm        (Algorithm_VoronoiAnim),
    blobImage           (0),
    knobImage           (0),
    bgImage             (0),
    heatMapBlobRadius   (50)
{
    algorithmNames.push_back("None");
    algorithmNames.push_back("cHeatmap");
    algorithmNames.push_back("Voronoi");
    algorithmNames.push_back("VoronoiAnim");
}

bool AlgoMap::loadPOI(const char* fileName)
{
    std::fstream fin(fileName);
    if (!fin)
    {
        return false;
    }
    
    poiLocations.clear();
    poiNames.clear();

    //  read bounds
    float minlat, minlon, maxlat, maxlon;
    std::string tag;
    fin >> tag >> minlat >> minlon >> maxlat >> maxlon;
    if (tag != "bounds:")
    {
        return false;
    }
    fin.ignore(1, '\n');

    //  read poi types/lists of locations
    std::string type, locations, sep;
    float lat, lon;
    while (!fin.eof()) 
    {
        std::getline(fin, type);
        if (type == "")
        {
            break;
        }
        poiNames.push_back(type);

        poiLocations.push_back(PointList());
        PointList& loc = poiLocations.back();
        std::getline(fin, locations);
        std::stringstream locstream(locations);
        while (!locstream.eof())
        {
            locstream >> lat >> lon >> sep;
            if (sep != ";")
            {
                return false;
            }
            //  normalize the location
            float x = (lon - minlon)/(maxlon - minlon);
            float y = 1.0f - (lat - minlat)/(maxlat - minlat);
            if (x >= 0.0f && x <= 1.0f && y >= 0.0f && y <= 1.0f)
            {
                loc.push_back(Point(x, y));
            }
        }
    }
    return true;
}

void AlgoMap::init()
{
    loadPOI("data/poi.txt");
}

void AlgoMap::draw(const Rect& ext)
{
    g_pGLPainter->setTexture(bgImage);
    g_pGLPainter->setBlendMode(GLPainter::BlendMode_Normal);
    Rect imgExt(ext);
    imgExt.shift(0.5f, -0.5f);
    g_pGLPainter->drawRect(imgExt.l, imgExt.t, imgExt.r, imgExt.b, 0xEEFFFFFF);

    const AlgoMap::PointList& poi = poiLocations[curPOI];

    if (curAlgorithm == AlgoMap::Algorithm_Voronoi)
    {
        //g_pGLPainter->setTexture(0);

        //VoronoiDiagramGenerator v;
        //
        //size_t npt = poi.size();
        //std::vector<float> px(npt), py(npt);
        //for (size_t i = 0; i < npt; i++)
        //{
        //    const Point& a = poi[i];
        //    px[i] = ext.l + a.x*float(ext.r - ext.l);
        //    py[i] = ext.t + a.y*float(ext.b - ext.t);            
        //}

        //v.generateVoronoi(&px[0], &py[0], npt, ext.l, ext.r, ext.t, ext.b, 1e-2f);
        //
        //v.resetIterator();
        //float ax, ay, bx, by;
        //while (v.getNext(ax, ay, bx, by))
        //{
        //    g_pGLPainter->drawLine(ax, ay, bx, by, 0xF01111FF, 2);
        //}
    }

    //  points
    if (curAlgorithm == AlgoMap::Algorithm_HeatMap)
    {
        g_pGLPainter->setTexture(0);
        g_pGLPainter->drawRect(ext.l, ext.t, ext.r, ext.b, 0x55000022);

        g_pGLPainter->setBlendMode(GLPainter::BlendMode_Additive);
        g_pGLPainter->setTexture(blobImage);
        size_t nPt = poi.size();
        for (size_t i = 0; i < nPt; i++)
        {
            const Point& pt = poi[i];
            float x = ext.l + pt.x*float(ext.r - ext.l);
            float y = ext.t + pt.y*float(ext.b - ext.t);
            float r = heatMapBlobRadius;
            g_pGLPainter->drawRect(x - r, y - r, x + r, y + r, 0xFFFFFFFF);
        }
        g_pGLPainter->setBlendMode(GLPainter::BlendMode_Normal);
    }
    else
    {
        g_pGLPainter->setTexture(knobImage);
        size_t nPt = poi.size();
        for (size_t i = 0; i < nPt; i++)
        {
            const Point& pt = poi[i];
            float x = ext.l + pt.x*float(ext.r - ext.l);
            float y = ext.t + pt.y*float(ext.b - ext.t);
            float r = 12;
            g_pGLPainter->drawRect(x - r, y - r, x + r, y + r, 0xFFFFFFFF);
        }
    }
}

void AlgoMap::onResize()
{
    blobImage = g_pGLPainter->loadTexture("data/blob.tga");
    knobImage = g_pGLPainter->loadTexture("data/knob.tga");    
    bgImage = g_pGLPainter->loadTexture("data/oslo_downtown.tga");
}
