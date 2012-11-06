#ifndef __IMAGE_H__
#define __IMAGE_H__

#include <vector>

struct Image
{
    std::vector<unsigned int>   rgba;   //  rgba pixel array
    int                         width;
    int                         height;
};

//  Loads image from TGA file
bool LoadTGA(const char* fileName, Image& image);

#endif // __IMAGE_H__
