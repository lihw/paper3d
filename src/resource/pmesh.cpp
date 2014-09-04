// pmesh.cpp
// The mesh resource
// 
// Copyright 2012 - 2014 Future Interface. 
// This software is licensed under the terms of the MIT license.
//
// Hongwei Li lihw81@gmail.com
//

#include "pmesh.h"

#include <Paper3D/presourcemanager.h>

#include <PFoundation/pcontext.h>
#include <PFoundation/pbox.h>
#include <PFoundation/pvector3.h>
#include <PFoundation/pinputstream.h>

#include <PFoundation/pglvertexbuffer.h>

#include "cache/pmeshcache_private.h"
#include "cache/presourcecache_private.h"


PMesh::PMesh(const pchar *id, 
            PMeshCache *meshCache, 
            PResourceManager* resourceManager, 
            pbool useGPUMemory)
    : PResourceObject(id, resourceManager, useGPUMemory? P_RESOURCE_GPU : P_RESOURCE_CPU)
    , m_boundingBox()
{
    m_vertexBufferObject = P_NULL;

    if (createMesh(meshCache))
    {
        m_available = true;
    }
}
    
PMesh::~PMesh()
{
    PDELETE(m_vertexBufferObject);
}
    
void PMesh::discardResource()
{
    PDELETE(m_vertexBufferObject);
}

pbool PMesh::restoreResource()
{
    PASSERT(m_vertexBufferObject == P_NULL);
    if (m_vertexBufferObject == P_NULL)
    {
        PResourceCache *cache = parent()->cache();
        PMeshCache *meshCache = cache->getMeshCache(m_id);
        if (meshCache != P_NULL && createMesh(meshCache))
        {
            return true;
        }
        else
        {
            PLOG_ERROR("Failed to load mesh %s", m_id);
        }
    }
    else
    {
        PLOG_ERROR("Fail to resume mesh %s because old vbo is not discarded", m_id);
    }

    return false;
}
    
bool PMesh::createMesh(PMeshCache *meshCache)
{
    PGlVertexBuffer* vbo = PNEW(PGlVertexBuffer);
    // TODO: customized primitive type, not only triangles.
    if (!vbo->create(P_GLTRIANGLES,
                     *(meshCache->vertexFormat()),
                     meshCache->vertices(),
                     meshCache->numberOfVertices(),
                     meshCache->indices(),
                     meshCache->numberOfIndices(),
                     m_type == P_RESOURCE_GPU? true : false))
    {
        PDELETE(vbo);
        return false;
    }

    m_boundingBox = meshCache->boundingBox();

    m_vertexBufferObject = vbo;

    return true;
}

bool PMesh::updateData(pfloat32 *vertices, 
                       puint32 numVertices, 
                       puint16 *indices, 
                       puint32 numIndices,
                       PGlVertexFormat *vertexFormat)
{

    // Regenerate the VBO
    PGlVertexBuffer *vbo = PNEW(PGlVertexBuffer);
    if (!vbo->create(P_GLTRIANGLES,
                     *vertexFormat,
                     vertices,
                     numVertices,
                     indices,
                     numIndices,
                     m_type == P_RESOURCE_GPU? true : false))
    {
        PDELETE(vbo);
        return false;
    }
    
    if (m_vertexBufferObject->isEnabled())
    {
        // It is in rendering
        vbo->enable();
    }
    PDELETE(m_vertexBufferObject);
    m_vertexBufferObject = vbo;
    
    // Update the corresponding mesh data
    PResourceCache *cache = parent()->cache();
    PMeshCache *meshCache = cache->getMeshCache(m_id);

    // TODO: compute the new bounding box.
    PBox box;
    meshCache->update(vertices, numVertices, indices, numIndices, box, vertexFormat);

    return true;
}
