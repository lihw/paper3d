// pmeshcache.cpp
// Mesh data
//
// Copyright 2012 - 2014 Future Interface. 
// This software is licensed under the terms of the MIT license.
//
// Hongwei Li lihw81@gmail.com
//

#include "pmeshcache_private.h"

#include <PFoundation/parchivefile.h>
#include <PFoundation/pmathutility.h>
#include <PFoundation/pinputstream.h>
#include <PFoundation/pglvertexbuffer.h>

#include "pmeshloader_private.h"

#include <math.h>


PMeshCache::PMeshCache(PResourceCache *parent, const pchar *id)
    : PCacheObject(parent, id, false)
{
    m_vertices            = P_NULL;
    m_indices             = P_NULL;
    m_numVertices         = 0;
    m_numIndices          = 0;
    m_vertexFormat        = P_NULL;

    if (createMesh(id))
    {
        m_available = true;
    }
    else
    {
        m_available = false;
        m_storageType = P_RESOURCE_STORAGE_UNKNOWN;
    }
}
    
PMeshCache::PMeshCache(PResourceCache *parent, const pchar *id, pfloat32 *vertices, puint32 numVertices, 
    puint16 *indices, puint32 numIndices, const PBox &box, PGlVertexFormat *vertexFormat, pbool autoRelease) 
    : PCacheObject(parent, id, autoRelease)
{
    m_vertexFormat = P_NULL;

    m_available = true;

    m_storageType = P_RESOURCE_FROM_MEMORY;

    m_vertices         = P_NULL;
    m_indices          = P_NULL;

    update(vertices, numVertices, indices, numIndices, box, vertexFormat);
}

PMeshCache::~PMeshCache()
{
    if (!m_autoRelease)
    {
        PDELETEARRAY(m_vertices);
        PDELETEARRAY(m_indices);
        PDELETE(m_vertexFormat);
    }
}

void PMeshCache::swapIn()
{
    PASSERT(!m_available);
    if (!m_autoRelease && !m_available)
    {
        if (m_storageType == P_RESOURCE_FROM_ARCHIVE ||
            m_storageType == P_RESOURCE_FROM_FILE)
        {
            if (createMesh(m_id))
            {
                m_available = true;
            }
        }
        else if (m_storageType == P_RESOURCE_FROM_MEMORY)
        {
            // TODO: read in the temporary file written at swapping out or
            // use createMesh when it is native type.
            m_available = true;
        }
        else
        {
            PLOG_ERROR("Unknown storage type of mesh data %s", m_id);
        }
    }
}

void PMeshCache::swapOut()
{
    PASSERT(m_available);
    if (!m_autoRelease && m_available)
    {
        if (m_storageType == P_RESOURCE_FROM_MEMORY)
        {
            // TODO: output the mesh to the temporary file.
        }

        PDELETEARRAY(m_vertices);
        PDELETEARRAY(m_indices);
        
        m_available = false;
    }
}

pbool PMeshCache::createMesh(const pchar *path)
{
    PInputStream inputStream;
    if (!archive()->createInputStream(path, &inputStream))
    {
        PLOG_ERROR("Failed to read mesh from %s.", path);
        return false;
    }

    m_vertices     = P_NULL;
    m_indices      = P_NULL;
    m_vertexFormat = P_NULL;
    m_numVertices  = 0;
    m_numIndices   = 0;

    PMeshLoader loader;
    if (!loader.load(&inputStream, this))
    {
        PLOG_ERROR("Failed to load mesh file %s.", path);
        return false;
    }

	 PLOG_DEBUG("Load mesh %s succeeded", path);
        
    if (archive()->isPathInArchive(m_id))
    {
        m_storageType = P_RESOURCE_FROM_ARCHIVE;
    }
    else
    {
        m_storageType = P_RESOURCE_FROM_FILE;
    }

    return true;
}
    
void PMeshCache::update(pfloat32 *vertices, puint32 numVertices, puint16 *indices, puint32 numIndices,
    const PBox &box, PGlVertexFormat *vertexFormat)
{
    if (m_autoRelease)
    {
        m_vertices     = vertices;
        m_numVertices  = numVertices;
        m_indices      = indices;
        m_numIndices   = numIndices;
        m_boundingBox  = box;

        m_vertexFormat = vertexFormat;
    }
    else
    {
        PDELETEARRAY(m_vertices);
        PDELETEARRAY(m_indices);
        PDELETE(m_vertexFormat);

        m_vertexFormat = PNEW(PGlVertexFormat(*vertexFormat));

        puint32 vertexNumBytes = m_vertexFormat->vertexSize();

        m_vertices = (pfloat32*)PNEWARRAY(pfloat32, numVertices * vertexNumBytes);
        m_indices  = PNEWARRAY(puint16, numIndices);

        pmemcpy(m_vertices, vertices, numVertices * vertexNumBytes);
        pmemcpy(m_indices, indices, numIndices * sizeof(puint16));

        m_numVertices = numVertices;
        m_numIndices = numIndices;
        
        m_boundingBox  = box;
    }
}
