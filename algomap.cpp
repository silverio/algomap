#include <algomap.h>

#include <fstream>
#include <sstream>

#include <glpainter.h>
#include <imgui.h>

AlgoMap::AlgoMap() : 
    m_curPOI            (18),
    m_curAlgorithm      (Algorithm_VoronoiAnim),
    m_blobImage         (0),
    m_knobImage         (0),
    m_bgImage           (0),
    m_heatMapBlobRadius (50),
    m_knobRadius        (10)
{
    m_algorithmNames.push_back("None");
    m_algorithmNames.push_back("cHeatmap");
    m_algorithmNames.push_back("Voronoi");
    m_algorithmNames.push_back("VoronoiAnim");
}

void AlgoMap::init()
{
    loadPOI("data/poi.txt");
    resetCurAlgorithm();
}

bool AlgoMap::loadPOI(const char* fileName)
{
    std::fstream fin(fileName);
    if (!fin)
    {
        return false;
    }
    
    m_poiLocations.clear();
    m_poiNames.clear();

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
        m_poiNames.push_back(type);

        m_poiLocations.push_back(PointList());
        PointList& loc = m_poiLocations.back();
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

void AlgoMap::resetCurAlgorithm()
{
    if (m_curAlgorithm == AlgoMap::Algorithm_VoronoiAnim)
    {
        const AlgoMap::PointList& poi = m_poiLocations[m_curPOI];
        m_Voronoi.init(&poi[0], poi.size(), Rect::unit);
        m_VoronoiAnim.reset();
    }
}

void AlgoMap::draw(const Rect& ext)
{
    g_pGLPainter->setTexture(m_bgImage);
    g_pGLPainter->setBlendMode(GLPainter::BlendMode_Normal);

    Rect imgExt(1024 - 902, 0, ext.r, ext.b - 68);
    imgExt.shift(0.5f, -0.5f);
    g_pGLPainter->drawRect(imgExt.l, imgExt.t, imgExt.r, imgExt.b, 0xEEFFFFFF);

    //  points
    if (m_curAlgorithm == AlgoMap::Algorithm_HeatMap)
    {
        drawHeatMap(imgExt);
    }
    else
    {
        drawPOI(imgExt);
    }

    if (m_curAlgorithm == AlgoMap::Algorithm_VoronoiAnim)
    {
        m_VoronoiAnim.draw(imgExt, m_Voronoi);
    }

    //  GUI
    g_IMGUI.panel(Rect(0, 0, imgExt.l, ext.b).inflated(-2), 0xFF444477);
    bool bPOIChanged = g_IMGUI.listBox(Rect(0, 0, imgExt.l, ext.b).inflated(-4), 15.0f, m_poiNames, m_curPOI);
    bool bAlgoChanged = g_IMGUI.listBox(Rect(0, 550, imgExt.l, ext.b).inflated(-4), 15.0f, 
        m_algorithmNames, m_curAlgorithm);

    if (m_curAlgorithm == AlgoMap::Algorithm_VoronoiAnim)
    {
        Rect pext(5, 670, imgExt.l - 5, ext.b - 5);
        g_IMGUI.panel(pext, 0xFF444477);

        g_IMGUI.toggleButton(Rect(pext.l + 3, pext.t + 5, pext.r - 3, pext.t + 20), "Pause", m_VoronoiAnim.paused);
        g_IMGUI.checkBox(Rect(pext.l + 3, pext.t + 25, pext.r - 3, pext.t + 40), "Event Pause", m_VoronoiAnim.pauseOnEvent);

        g_IMGUI.label(Rect(pext.l + 3, pext.t + 50, pext.r - 3, pext.t + 70), "Animation Speed:", 0xFFAAAAAA);
        g_IMGUI.slider(Rect(pext.l + 3, pext.t + 70, pext.r - 3, pext.t + 85), 0.0f, 0.2f, m_VoronoiAnim.animSpeed);
    }

    if (bPOIChanged || bAlgoChanged)
    {
        resetCurAlgorithm();
    }

    Rect legendExt(imgExt.l - 0.5f, imgExt.b + 1.0f, ext.r, ext.b);
    g_IMGUI.panel(legendExt.inflated(-1), 0xFF444455);
    g_IMGUI.label(legendExt, "Select POI in the listbox to the left, also choose the"\
        " algorithm to apply from the second listbox.", 0xFFFFFFFF);
}

void AlgoMap::drawPOI(const Rect& ext)
{
    g_pGLPainter->setTexture(m_knobImage);

    const AlgoMap::PointList& poi = m_poiLocations[m_curPOI];
    size_t nPt = poi.size();
    for (size_t i = 0; i < nPt; i++)
    {
        Point pt = ext.mapFromUnit(poi[i]);
        float r = m_knobRadius;
        g_pGLPainter->drawRect(pt.x - r, pt.y - r, pt.x + r, pt.y + r, 0xFFFFFFFF);
    }
}

void AlgoMap::drawHeatMap(const Rect& ext)
{
    g_pGLPainter->setTexture(0);
    g_pGLPainter->drawRect(ext.l, ext.t, ext.r, ext.b, 0x55000022);

    g_pGLPainter->setBlendMode(GLPainter::BlendMode_Additive);
    g_pGLPainter->setTexture(m_blobImage);

    const AlgoMap::PointList& poi = m_poiLocations[m_curPOI];
    size_t nPt = poi.size();
    for (size_t i = 0; i < nPt; i++)
    {
        Point pt = ext.mapFromUnit(poi[i]);
        float r = m_heatMapBlobRadius;
        g_pGLPainter->drawRect(pt.x - r, pt.y - r, pt.x + r, pt.y + r, 0xFFFFFFFF);
    }
    g_pGLPainter->setBlendMode(GLPainter::BlendMode_Normal);
}

void AlgoMap::onResize()
{
    m_blobImage = g_pGLPainter->loadTexture("data/blob.tga");
    m_knobImage = g_pGLPainter->loadTexture("data/knob.tga");    
    m_bgImage = g_pGLPainter->loadTexture("data/oslo_downtown.tga");
}
