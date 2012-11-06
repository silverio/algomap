#ifndef __COLORF_H__
#define __COLORF_H__

inline float clamp(float val, float minv, float maxv)
{
    return val < minv ? minv : (val > maxv ? maxv : val);
}

struct ColorF
{
    float a, r, g, b;

    ColorF() : a(1.0f), r(1.0f), g(1.0f), b(1.0f) {}
    ColorF(float a_, float r_, float g_, float b_) : a(a_), r(r_), g(g_), b(b_) {}
    ColorF(unsigned int abgr) 
    {
        a = ((abgr >> 24) & 0xFF)/255.0f;
        b = ((abgr >> 16) & 0xFF)/255.0f;
        g = ((abgr >>  8) & 0xFF)/255.0f;
        r = (abgr & 0xFF)/255.0f;
    }
    
    inline ColorF operator *(float s) const { return ColorF(a, r*s, g*s, b*s); }
    inline operator unsigned int() const
    {
        return  ((unsigned int)(clamp(a, 0.0f, 1.0f)*255.0f)) << 24 |
                ((unsigned int)(clamp(b, 0.0f, 1.0f)*255.0f)) << 16 |
                ((unsigned int)(clamp(g, 0.0f, 1.0f)*255.0f)) << 8  |
                ((unsigned int)(clamp(r, 0.0f, 1.0f)*255.0f));
    }

};

#endif
