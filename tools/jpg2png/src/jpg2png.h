// jpg2png.h
// convert jpg to png
//
// Copyright 2012 - 2014 Future Interface. 
// This software is licensed under the terms of the MIT license.
//
// Hongwei Li lihw81@gmail.com
//

#ifndef JPG2PNG_H
#define JPG2PNG_H

#include <string>


class Jpg2Png
{
public:
    Jpg2Png(const char *inputFile, const char *outputFile);

    bool execute();
    
private:
    std::string m_png;
    std::string m_jpg;
};

#endif // !JPG2PNG_H
