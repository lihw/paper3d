// jpg2png.cpp
// convert jpg to png
//
// Copyright 2012 - 2014 Future Interface. 
// This software is licensed under the terms of the MIT license.
//
// Hongwei Li lihw81@gmail.com
//

#include "jpg2png.h"

#include <common/log.h>

#include <cstdio>



extern bool readJPG(FILE* fp, unsigned int& width, unsigned int& height, unsigned int& nchannels, unsigned char*& data);
extern bool writeJPG(FILE* fp, unsigned int width, unsigned int height, unsigned int quality, unsigned int nchannels, const unsigned char* data); 

bool readPNG(FILE* fp, unsigned int& width, unsigned int& height, unsigned int& nchannels, unsigned char*& data);
bool writePNG(FILE* fp, unsigned int width, unsigned int height, unsigned int nchannels, const unsigned char* data);

    
Jpg2Png::Jpg2Png(const char *inputFile, const char *outputFile)
    : m_jpg(inputFile)
    , m_png(outputFile)
{
}

bool Jpg2Png::execute()
{
    unsigned int width;
    unsigned int height;
    unsigned int nchannels;
    unsigned char *data;

    FILE *jpgFp;
    FILE *pngFp;

    fopen_s(&jpgFp, m_jpg.c_str(), "rb");
    fopen_s(&pngFp, m_png.c_str(), "wb");

    if (jpgFp == NULL || pngFp == NULL)
    {
        logError("Failed to open JPG or PNG image.");
        return false;
    }

    //unsigned char buffer[1024];
    //fread(buffer, 1, 1024, jpgFp);

    if (!readJPG(jpgFp, width, height, nchannels, data))
    {
        logError("Failed to open image file %s.", m_jpg.c_str());
        return false;
    }

    if (!writePNG(pngFp, width, height, nchannels, data))
    {
        logError("Failed to save image file %s.", m_jpg.c_str());
        return false;
    }

    delete [] data;

    fclose(jpgFp);
    fclose(pngFp);

    return true;
}
