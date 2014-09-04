// PResourceCache.cpp
// The manager of all asset objects. It provides data for resource manager.
//
// Copyright 2012 - 2014 Future Interface. 
// This software is licensed under the terms of the MIT license.
//
// Hongwei Li lihw81@gmail.com
//

#include "presourcecache_private.h"

#include <Paper3D/panimationresource.h>

#include <PFoundation/parchivefile.h>
#include <PFoundation/pinputstream.h>

#include <PFoundation/ptime.h>

#include "pmaterialcache_private.h"
#include "ptexturecache_private.h"
#include "pmeshcache_private.h"


PResourceCache::PResourceCache(PResourceManager *parent)
{
    m_parent = parent;

    // Initialize the next Id.
    m_nextId.i = 0;
    m_nextId.c[4] = 0;
}

PResourceCache::~PResourceCache()
{
    PStringMap<PCacheObject *>::iterator it = m_objects.begin();
    PStringMap<PCacheObject *>::iterator ie = m_objects.end();
    while (it != ie)
    {
        PDELETE(it.value());
        ++it;
    }
}

bool PResourceCache::getImages(const pchar *id, 
                              PImage **out_images, 
                              puint32 *out_numImages)
{
    PCacheObject *imageDataObject = findObject(id);
    if (imageDataObject == P_NULL)
    {
        PTextureCache *cache = PNEW(PTextureCache(this, id));
        if (cache->getNumberOfImages() == 0)
        {
            PLOG_ERROR("Failed to load image data at %s", id);
            PDELETE(cache);
            return false;
        }
        cache->getImages(out_images, out_numImages);
        insertObject(cache);

        return true;
    }
        
    PTextureCache *cache = dynamic_cast<PTextureCache *>(imageDataObject);
    cache->getImages(out_images, out_numImages);

    return true;
}

const pchar *PResourceCache::addImages(const pchar *id, 
                                      PImage **images, 
                                      puint32 numImages, 
                                      pbool autoRelease)
{
    if (id == P_NULL)
    {
        id = generateUniqueId();
    }
    // FIXME: check the uniqueness of the user specified id in asset space.
    PTextureCache *cache = PNEW(PTextureCache(this, id, images, numImages, autoRelease));
    insertObject(cache);
    return cache->id();
}
    
PMaterialCache *PResourceCache::getMaterialCache(const pchar *id) 
{
    PCacheObject *materialCacheObject = findObject(id);
    if (materialCacheObject == P_NULL)
    {
        PMaterialCache *materialCache = PNEW(PMaterialCache(this, id));
        if (materialCache->text() == P_NULL) 
        {
            PDELETE(materialCache);
            return P_NULL;
        }

        insertObject(materialCache);

        return materialCache;
    }

    return dynamic_cast<PMaterialCache *>(materialCacheObject);
}

const pchar *PResourceCache::addMaterialCache(const pchar *id, 
                                              const pchar *text,
                                              pbool autoRelease)
{
    if (id == P_NULL)
    {
        id = generateUniqueId();
    }
    // Check the uniqueness of the user specified id in asset space.
    PCacheObject *obj = findObject(id);
    if (obj == P_NULL)
    {
        obj = PNEW(PMaterialCache(this, id, text, autoRelease));
        insertObject(obj);
    }
    
    return obj->id();
}

PMeshCache *PResourceCache::getMeshCache(const pchar *id)
{
    PCacheObject *meshCacheObject = findObject(id);
    if (meshCacheObject == P_NULL)
    {
        PMeshCache *cache = PNEW(PMeshCache(this, id));
        if (cache->numberOfVertices() == 0)
        {
            PDELETE(cache);
            return P_NULL;
        }

        insertObject(cache);
        return cache;
    }

    return dynamic_cast<PMeshCache *>(meshCacheObject);
}
    
const pchar *PResourceCache::addMeshCache(const pchar *id, pfloat32 *vertices, puint32 numVertices, puint16 *indices,
        puint32 numIndices, const PBox &box, PGlVertexFormat *vertexFormat, pbool autoRelease) 
{
    if (id == P_NULL)
    {
        id = generateUniqueId();
    }
    // FIXME: check the uniqueness of the user specified id in asset space.
    PMeshCache *cache = PNEW(PMeshCache(this, id, vertices, numVertices, indices, numIndices, box, vertexFormat, autoRelease));
    insertObject(cache);
    return cache->id();
}

PCacheObject *PResourceCache::findObject(const pchar *id) const
{
     PStringMap<PCacheObject *>::const_iterator it = m_objects.find(const_cast<pchar*>(id));
    if (it != m_objects.end())
    {
        PCacheObject *object = it.value();
        
        // Swap in the object's content into memory if it is not there.
        if (!object->isAvailable())
        {
            object->swapIn();
        }

        return object;
    }

    return P_NULL;
}
    
void PResourceCache::insertObject(PCacheObject *object)
{
    pchar *id = const_cast<pchar *>(object->id());
    m_objects.insert(id, object);
    
    optimize();
}

void PResourceCache::destroyObject(const pchar *id)
{
    PStringMap<PCacheObject *>::iterator it = m_objects.find(const_cast<pchar*>(id));
    if (it != m_objects.end())
    {
        PCacheObject *object = it.value();
        m_objects.erase(it);
        PDELETE(object);
    }
    else
    {
        PLOG_ERROR("Failed to find the asset called %s", id);
    }
}

void PResourceCache::optimize()
{
    // TODO:
    // PASSERT_NOTIMPLEMENTED();
}
    
const pchar *PResourceCache::generateUniqueId() const
{
    m_nextId.i++;

    return (pchar *)m_nextId.c;
}
