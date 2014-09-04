// pmhmodel.h
// The reading and write of PMH model
//
// Copyright 2012 - 2014 Future Interface. 
// This software is licensed under the terms of the MIT license.
//
// Hongwei Li lihw81@gmail.com
//

#ifndef PMHMODEL_H
#define PMHMODEL_H

#include <string>

// pmh format version 0.9
//
// 'p''m''h''0'           # The very first 4 bytes are the always "pmh0"
// 0x1 0x2 0x3 0x4        # 4 bytes are endianness checker
// 0x0 0x9                # 2 bytes for the version of pmh format.
// 0 0 0 0 0 0            # bounding box (min, max), 6 floating numbers
// 0x2                    # 4 bytes are the number of vertex attributes
// position 3 float '\n'  # each line is an attribute containing a name, the number of elements and data type
//                        # e.g., position 3 float
//                        #       normal 3 float
//                        #       normal0 3 float
//                        #       texcoord 2 float
//                        # properties names can only be one of following ones:
//                        # position, normal, texcoord, binormal, tangent, normal, 
//                        # texcoord0, texcoord1, texcoord2, texcoord3, color.
// ...                    # 128 bytes reservation area for future extension.
// 0x01                   # 4 bytes, number of vertices
// 0x01                   # 4 bytes, number of indicies
// ...                    # vertices data
// ...                    # indices data; each index is a short integer.
// 'p''m''h''1'           # The very last 4 bytes are the always "pmh1"

struct PmhModel
{
    PmhModel();
    ~PmhModel();

    unsigned int numVertices;
    unsigned int numIndices;
    float *vertices;
    unsigned short *indices;
    struct Attrib
    {
        std::string name;
        std::string type;
        int size;
    } attribs[8];
    unsigned int numAttribs;
    unsigned int vertexSize;
    float bb[2][3];

    bool write(const char *path);
    bool read(const char *path);

    const static unsigned short VERSION = 0x0090;
};



#endif // !PMHMODEL_H
