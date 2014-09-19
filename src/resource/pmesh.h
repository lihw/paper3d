// pmesh.h
// The mesh resource
// 
// Copyright 2012 - 2014 Future Interface. 
// This software is licensed under the terms of the MIT license.
//
// Hongwei Li lihw81@gmail.com
//

#ifndef PMESH_H
#define PMESH_H

#include <Paper3D/presourceobject.h>

#include <PFoundation/pvector3.h>
#include <PFoundation/pbox.h>

class PMeshCache;
class PGlVertexBuffer;
struct PGlVertexFormat;


class P_DLLEXPORT PMesh : public PResourceObject
{
    friend class PResourceManager;
    
    PMesh(const PMesh& other) : PResourceObject(P_NULL, P_NULL, P_RESOURCE_TYPE_UNKNOWN), m_boundingBox(P_NULL, P_NULL) {}
    void operator=(const PMesh &other) {}

public:
    P_INLINE PGlVertexBuffer *vertexBufferObject() const { return m_vertexBufferObject; }
    P_INLINE const PBox &boundingBox() const { return  m_boundingBox; }

    // Replace the mesh data. Return true if succeed
    pbool updateData(pfloat32 *vertices, puint32 numVertices, puint16 *indices, puint32 numIndices,
        PGlVertexFormat *vertexFormat);
    // Only update the vertex data
    pbool updateVertexData(pfloat32 *vertices);

protected:
    virtual void discardResource();
    virtual pbool restoreResource();

private:
    PMesh(const pchar *id, PMeshCache *meshCache, PResourceManager *resourceManager, pbool useGPUMemory = true);
    virtual ~PMesh();

    pbool createMesh(PMeshCache *meshCache);

private:
    PGlVertexBuffer    *m_vertexBufferObject;
    // FIXME: duplication of with bounding box and centroid inside PMeshData
    PBox                m_boundingBox;
};


#endif // !PMESH_H


