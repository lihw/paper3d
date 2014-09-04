// obj2pmh.h
// The converter
//
// Copyright 2012 - 2014 Future Interface. 
// This software is licensed under the terms of the MIT license.
//
// Hongwei Li lihw81@gmail.com
//

#ifndef OBJ2PMH_H
#define OBJ2PMH_H

#include <string>

class Obj2Pmh
{
public:
    Obj2Pmh(const char *inputFile, const char *outputFile);
    ~Obj2Pmh();

    bool execute();

private:
    std::string m_inputpath;
    std::string m_outputpath;
};


#endif // !OBJ2PMH_H
