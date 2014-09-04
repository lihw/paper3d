// presourcecache.h
// The manager of all asset objects. It provides data for resource manager.
//
// Copyright 2012 - 2014 Future Interface. 
// This software is licensed under the terms of the MIT license.
//
// Hongwei Li lihw81@gmail.com
//

#ifndef PRESOURCECACHE_PRIVATE_H
#define PRESOURCECACHE_PRIVATE_H

#include "pcacheobject_private.h"

#include <PFoundation/pstringmap.h>


class PArchiveFile;
class PMeshCache;
class PImage;
class PMaterialCache;
class PAnimationResource;
struct PGlVertexFormat;
class PBox;


class P_DLLEXPORT PResourceCache
{
    friend class PResourceManager;
    friend class PCacheObject;
    friend class PTexture;
    friend class PMesh;
    friend class PMaterialResource;

    PResourceCache(const PResourceCache &other) {}
    void operator=(const PResourceCache &other) {}

private:
    // -------------------------------------------------------------- 
    // Interface to PResourceManager
    // -------------------------------------------------------------- 
    PResourceCache(PResourceManager *parent);
    ~PResourceCache();

    const pchar *addImages(const pchar *id, PImage** images, puint32 numImages, pbool autoRelease = true);
    pbool getImages(const pchar *id, PImage **out_images, puint32 *out_numImages);

    PMaterialCache *getMaterialCache(const pchar *id);
    const pchar *addMaterialCache(const pchar *id, const pchar *text, pbool autoRelease = true);

    const pchar *addMeshCache(const pchar *id, pfloat32 *vertices, puint32 numVertices, puint16 *indices,
        puint32 numIndices, const PBox &box, PGlVertexFormat *vertexFormat, pbool autoRelease = true); 
    PMeshCache *getMeshCache(const pchar *id); 

    P_INLINE PResourceManager *resourceManager() const { return m_parent; }

private:
    // -------------------------------------------------------------- 
    // Helper utilities
    // -------------------------------------------------------------- 
    void insertObject(PCacheObject *object);
    void destroyObject(const pchar *id);
    PCacheObject* findObject(const pchar *id) const;
    void optimize(); // optimize the memory use.
    const pchar *generateUniqueId() const;

private:
    PStringMap<PCacheObject *>    m_objects;
    PResourceManager             *m_parent;
    // A temporary storage buffer used in generating unique Id.
    mutable union    
    {
        puint8  c[5];
        puint32 i;
    } m_nextId;
};


#endif // !PRESOURCECACHE_PRIVATE_H
