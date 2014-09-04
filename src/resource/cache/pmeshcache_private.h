// pmeshcache_private.h
// Mesh data
//
// Copyright 2012 - 2014 Future Interface. 
// This software is licensed under the terms of the MIT license.
//
// Hongwei Li lihw81@gmail.com
//

#ifndef PMESHCACHE_PRIVATE_H
#define PMESHCACHE_PRIVATE_H

#include "pcacheobject_private.h"

#include <PFoundation/pbox.h>
#include <PFoundation/pvector3.h>

#include <PFoundation/pglvertexbuffer.h>


class P_DLLEXPORT PMeshCache : public PCacheObject
{
    friend class PResourceCache;
    friend class PMeshLoader;
    
    PMeshCache(const PMeshCache &other) : PCacheObject(P_NULL, P_NULL, true) {}
    void operator=(const PMeshCache &other) {}

public:
    P_INLINE const PBox &boundingBox() const
    { return m_boundingBox; }
    P_INLINE pfloat32 *vertices() const 
    { return m_vertices; };
    P_INLINE puint16 *indices() const 
    { return m_indices; };
    P_INLINE puint32 numberOfIndices() const
    { return m_numIndices; }
    P_INLINE puint32 numberOfVertices() const
    { return m_numVertices; }
    P_INLINE PGlVertexFormat *vertexFormat() const
    { return m_vertexFormat; }
    
    void update(pfloat32 *vertices, puint32 numVertices, puint16 *indices, puint32 numIndices,
        const PBox &box, PGlVertexFormat *vertexFormat);
    
protected:
    virtual void swapIn();
    virtual void swapOut();

private:
    PMeshCache(PResourceCache *parent, const pchar *id);
    PMeshCache(PResourceCache *parent, const pchar *id, pfloat32 *vertices, puint32 numVertices, 
        puint16 *indices, puint32 numIndices, const PBox &box, PGlVertexFormat *vertexFormat, 
        pbool autoRelease = true); 

    virtual ~PMeshCache();
    
    pbool createMesh(const pchar *path);

private:
    // FIXME: default SRT are exported from FBXNode. In fact, mesh doesn't acquire
    // a local transformat. It should be properties of drawable which the counterpart
    // of node in FBX scene.
    PBox                   m_boundingBox;
    pfloat32              *m_vertices;
    puint32                m_numVertices;
    puint16               *m_indices;
    puint32                m_numIndices;
    PGlVertexFormat       *m_vertexFormat;
};


#endif // !PMESHCACHE_PRIVATE_H
