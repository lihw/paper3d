// pmhmodel.cpp
// The reading and write of PMH model
//
// Copyright 2012 - 2014 Future Interface. 
// This software is licensed under the terms of the MIT license.
//
// Hongwei Li lihw81@gmail.com
//

#include "pmhmodel.h"

#include "log.h"
#include "filestream.h"

#include <math.h>
#include <float.h>
#include <stdio.h>
#include <assert.h>

    
PmhModel::PmhModel()
{
    vertices = NULL;
    indices = NULL;

    bb[0][0] = FLT_MAX;
    bb[0][1] = FLT_MAX;
    bb[0][2] = FLT_MAX;
    
    bb[1][0] = -FLT_MAX;
    bb[1][1] = -FLT_MAX;
    bb[1][2] = -FLT_MAX;

    numAttribs = 0;
    vertexSize = 0;
}

PmhModel::~PmhModel()
{
    delete [] vertices;
    delete [] indices;
}

bool PmhModel::write(const char *path)
{
    //
    // Open the stream.
    //
    FileStream stream(path);
    if (!stream.openToWrite())
    {
        logError("Fail to create %s.", path);
        return false;
    }

    try
    {
        stream.writeUint8('p');
        stream.writeUint8('m');
        stream.writeUint8('h');
        stream.writeUint8('0');

        stream.writeUint32(0x01020304);
        
        stream.writeUint16(VERSION);

        stream.writeFloat32(bb[0][0]);
        stream.writeFloat32(bb[0][1]);
        stream.writeFloat32(bb[0][2]);
        stream.writeFloat32(bb[1][0]);
        stream.writeFloat32(bb[1][1]);
        stream.writeFloat32(bb[1][2]);

        stream.writeUint32(numAttribs);
        for (unsigned int i = 0; i < numAttribs; ++i)
        {
            char buffer[1024];
#if defined WIN32
            size_t n = sprintf_s(buffer, 1024, "%s %d %s\n",
                    attribs[i].name.c_str(),
                    attribs[i].size,
                    attribs[i].type.c_str());
#elif defined __APPLE__
            size_t n = snprintf(buffer, 1024, "%s %d %s\n",
                                attribs[i].name.c_str(),
                                attribs[i].size,
                                attribs[i].type.c_str());
#endif
            stream.writeBytes((unsigned char*)(&buffer[0]), n);
        }

        unsigned char reserved[128];
        stream.writeBytes(reserved, 128);

        //
        // Write vertices and indices
        //
        stream.writeUint32(numVertices);
        stream.writeUint32(numIndices);

        stream.writeBytes((unsigned char *)vertices, numVertices * vertexSize);
        stream.writeBytes((unsigned char *)indices, numIndices * sizeof(unsigned short));

        //
        // Writer the footer
        //
        stream.writeUint8('p');
        stream.writeUint8('m');
        stream.writeUint8('h');
        stream.writeUint8('1');
    }
    catch (std::string &message)
    {
        logError("Fail to write to %s.\n", path);
        logError("\"%s\".\n", message.c_str());
        return false;
    }

    //
    // Close the stream.
    //
    stream.close();

    return true;
}

bool PmhModel::read(const char *path)
{
    assert(0);
    return false;
}
