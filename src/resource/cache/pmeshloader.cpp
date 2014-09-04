// pmeshloader.h
// load pmh file, convert to mesh.
//
// Copyright 2012 - 2014 Future Interface. 
// This software is licensed under the terms of the MIT license.
//
// Hongwei Li lihw81@gmail.com and Hongwei Li liwh81@gmail.com
//
//
// 
#include "pmeshloader_private.h"

#include <PFoundation/pinputstream.h>

#include <PFoundation/pcrt.h>
#include <PFoundation/pnew.h>

pbool PMeshLoader::load(PInputStream *inputStream, PMeshCache *cache) const
{
    PASSERT(inputStream != P_NULL);
    if (inputStream != P_NULL)
    {
#define SAFE_READ(x) \
        if (!(x)) { PLOG_ERROR("Failed to read the mesh file."); return false; }

        // For the format of the pmh, referring to the meshexporter in the fbx converter tool
        //
        // Header
        //
        puint8 header[4];
        SAFE_READ(inputStream->readBytes(4, header));
        if (header[0] != 'p' && 
            header[1] != 'm' && 
            header[2] != 'h' && 
            header[3] != '0')
        {
            PLOG_ERROR("The mesh header is corrupted.");
            return false;
        }

        //
        // Endianness
        //
        puint8 endianness[4];
        SAFE_READ(inputStream->readBytes(4, endianness));
        if (endianness[0] == 0x04 &&
            endianness[1] == 0x03 &&
            endianness[2] == 0x02 &&
            endianness[3] == 0x01)
        {
            inputStream->setEndianness(P_STREAM_ENDIANNESS_LITTLE);
        }
        else
        {
            inputStream->setEndianness(P_STREAM_ENDIANNESS_BIG);
        }

        //
        // Version
        //
        puint8 version[2];
        SAFE_READ(inputStream->readBytes(2, version));
        // TODO: check the PMH version is smaller than the one supported by engine.

        //
        // Bounding box
        //
        SAFE_READ(inputStream->readFloat32(&cache->m_boundingBox.m_min[0]));
        SAFE_READ(inputStream->readFloat32(&cache->m_boundingBox.m_min[1]));
        SAFE_READ(inputStream->readFloat32(&cache->m_boundingBox.m_min[2]));

        SAFE_READ(inputStream->readFloat32(&cache->m_boundingBox.m_max[0]));
        SAFE_READ(inputStream->readFloat32(&cache->m_boundingBox.m_max[1]));
        SAFE_READ(inputStream->readFloat32(&cache->m_boundingBox.m_max[2]));

        //
        // Vertex format
        //
        puint32 numAttribs;
        SAFE_READ(inputStream->readUint32(&numAttribs));
        cache->m_vertexFormat = PNEW(PGlVertexFormat);

        pchar fields[3][1024];
        for (puint32 i = 0; i < numAttribs; ++i)
        {
            puint8 c;
            pchar *p = fields[0];
            SAFE_READ(inputStream->readUint8(&c));
            puint32 j = 0;
            while (c != '\n')
            {
                if (c == ' ')
                {
                    *p = 0;
                    p = fields[++j];
                }
                else
                {
                    *p++ = c;
                }
                SAFE_READ(inputStream->readUint8(&c));
            }
            
            *p = 0; // End the field 3 string.

            if (j != 2)
            {
                PLOG_ERROR("Unrecognized vertex attribute.");
            }

            if (pstrncmp("float32", fields[2], 7) == 0)
            {
                cache->m_vertexFormat->addAttrib(fields[0], patoi(fields[1]), P_FLOAT32);
            }
            else if (pstrncmp("int32", fields[2], 5) == 0)
            {
                cache->m_vertexFormat->addAttrib(fields[0], patoi(fields[1]), P_INT);
            }
            else if (pstrncmp("uint32", fields[2], 6) == 0)
            {
                cache->m_vertexFormat->addAttrib(fields[0], patoi(fields[1]), P_UNSIGNED_INT);
            }
            else
            {
                PLOG_ERROR("Unknown attribute type (%s).", fields[2]);
            }
        }

        // 
        // Padding (128 bytes)
        //
        puint8 temp[128];
        SAFE_READ(inputStream->readBytes(128, temp));

        SAFE_READ(inputStream->readUint32(&cache->m_numVertices));
        SAFE_READ(inputStream->readUint32(&cache->m_numIndices));

        puint32 num;
        num = cache->m_vertexFormat->vertexSize() * cache->m_numVertices;
        cache->m_vertices = (pfloat32 *)PNEWARRAY(puint8, num);

        cache->m_indices = PNEWARRAY(puint16, cache->m_numIndices);

        SAFE_READ(inputStream->readBytes(num, (puint8 *)cache->m_vertices));
        SAFE_READ(inputStream->readBytes(sizeof(puint16) * cache->m_numIndices, (puint8 *)cache->m_indices));

        puint8 footer[4];
        SAFE_READ(inputStream->readBytes(4, footer));
        if (footer[0] != 'p' && 
            footer[1] != 'm' && 
            footer[2] != 'h' && 
            footer[3] != '1')
        {
            PLOG_ERROR("The mesh header is corrupted.");
        }
    }

#undef SAFE_READ

    return true;
}

PMeshLoader::PMeshLoader()
{
}

PMeshLoader::~PMeshLoader()
{
}

