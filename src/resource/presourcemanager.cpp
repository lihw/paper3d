// presourcemanager.cpp
// Resource management, e.g., shader, texture, vertex buffer, and framebuffer
// Copyright 2012 - 2014 Future Interface. 
// This software is licensed under the terms of the MIT license.
//
// Hongwei Li lihw81@gmail.com
//

#include "presourcemanager.h"

#include <Paper3D/pmesh.h>
#include <Paper3D/ptexture.h>
#include <Paper3D/pframebuffer.h>
#include <Paper3D/pmaterialresource.h>
#include <Paper3D/panimationresource.h>

#include <PFoundation/parchivefile.h>
#include <PFoundation/pconststring.h>
#include <PFoundation/pcontext.h>

#include "cache/presourcecache_private.h"
#include "cache/pmaterialcache_private.h"


// TODO: 
// - use as few PString as possible, especially avoid temporary use of PString
// - restructure the resource directory to have cache and resource subfolders


PResourceManager::PResourceManager(PContext* context)
    : PModule("resource-manager", context)
{
    m_archive = PNEW(PArchiveFile);
    m_cache   = PNEW(PResourceCache(this));
}

PResourceManager::~PResourceManager()
{
    deleteResourceObjects();
    PDELETE(m_cache);
    PDELETE(m_archive);
}

pbool PResourceManager::initialize()
{
    // Load archive.
    if (PString("") != m_context->properties()->m_archiveName)
    {
        return m_archive->load(m_context->properties()->m_archiveName.c_str());
    }
    
    return true;
}

PMesh *PResourceManager::getMesh(const pchar *id, pbool useGPUMemory)
{
    pchar trueId[1024];
    pStringToLower(id, 1024, trueId);
    PResourceObject* ret = findResourceObject(trueId);
    if (ret == P_NULL)
    {
        // Create the mesh
        PMeshCache *meshCache = m_cache->getMeshCache(trueId);
        if (meshCache == P_NULL)
        {
            return P_NULL;
        }
        PMesh *mesh = PNEW(PMesh(id, meshCache, this, useGPUMemory));
        if (!mesh->isAvailable())
        {
            // FIXME: do we need to delete the corresponding mesh cache?
			PASSERT(mesh->isAvailable());
            PDELETE(mesh);
            return P_NULL;
        }

        insertResourceObject(mesh);
        return mesh;
    }

    return dynamic_cast<PMesh*>(ret);
}
    
PMesh *PResourceManager::createMesh(pfloat32 *vertices, puint32 numVertices, puint16 *indices,
    puint32 numIndices, const PBox &box, PGlVertexFormat *vertexFormat, const pchar *id, 
    pbool autoRelease, pbool useGPUMemory)
{
    // FIXME: check the duplication of the user specified id with existing objects.
    id = m_cache->addMeshCache(id, vertices, numVertices, indices, numIndices, box, 
        vertexFormat, autoRelease);

    PMeshCache *meshCache = m_cache->getMeshCache(id);
    PASSERT(meshCache != P_NULL);
    
    PMesh *mesh = PNEW(PMesh(id, meshCache, this, useGPUMemory));

    if (!mesh->isAvailable())
    {
        PDELETE(mesh);
        return P_NULL;
    }

    insertResourceObject(mesh);
    return mesh;
}

PTexture *PResourceManager::getTexture(const pchar *id, pbool mipmapEnabled)
{
    pchar trueId[1024];
    pStringToLower(id, 1024, trueId);
    PResourceObject* ret = findResourceObject(trueId);
    if (ret == P_NULL)
    {
        // Create the texture
        PImage* images[6];
        puint32 numImages;
        if (!m_cache->getImages(trueId, images, &numImages))
        {
            return P_NULL;
        }

        PTexture* texture;
        if (numImages == 6)
        { 
            texture = PNEW(PTexture(trueId, images, this, mipmapEnabled));
        }
        else if (numImages == 1)
        {
            texture = PNEW(PTexture(trueId, images[0], this, mipmapEnabled));
        }
        else
        {
            PLOG_ERROR("Unsupported texture format");
            return P_NULL;
        }
        
        if (!texture->isAvailable())
        {
            // FIXME: do we need to delete the corresponding image cache?
			PASSERT(texture->isAvailable());
            PDELETE(texture);
            return P_NULL;
        }

        insertResourceObject(texture);
        return texture;
    }

    return dynamic_cast<PTexture*>(ret);
}

PTexture *PResourceManager::createTexture(PImage *image, 
    const pchar *id, pbool autoRelease, pbool mipmapEnabled)
{
    // FIXME: check the duplication of the user specified id with existing objects.
    id = m_cache->addImages(id, &image, 1, autoRelease);
    
    PTexture *texture = PNEW(PTexture(id, image, this, mipmapEnabled));

    if (!texture->isAvailable())
    {
        // FIXME: do we need to delete the just inserted image cache?
        PDELETE(texture);
        return P_NULL;
    }

    insertResourceObject(texture);
    return texture;
}

PTexture *PResourceManager::createTexture(PImage **images, 
    const pchar *id, pbool autoRelease, pbool mipmapEnabled)
{
    // FIXME: check the duplication of the user specified id with existing objects.
    
    id = m_cache->addImages(id, images, 6, autoRelease);

    PTexture *texture = PNEW(PTexture(id, images, this, mipmapEnabled));

    if (!texture->isAvailable())
    {
        // FIXME: do we need to delete the just inserted image cache?
        PDELETE(texture);
        return P_NULL;
    }

    insertResourceObject(texture);
    return texture;
}

PMaterialResource *PResourceManager::getMaterial(const pchar *id) 
{
    pchar trueId[1024];
    pStringToLower(id, 1024, trueId);
    PResourceObject *ret = findResourceObject(trueId);
    if (ret == P_NULL)
    {
        // Create the material at the first time.
        PMaterialCache *cache = m_cache->getMaterialCache(trueId);
        PMaterialResource *material = P_NULL;

        if (cache == P_NULL)
        {
            // Use default material when the needed material is not found.
            PResourceObject *defaultMaterial = findResourceObject("default.pmt");
            if (defaultMaterial != P_NULL)
            {
                return dynamic_cast<PMaterialResource *>(defaultMaterial);
            }

            material = PNEW(PMaterialResource("default.pmt", P_NULL, this));
        }
        else
        {
            material = PNEW(PMaterialResource(trueId, cache->text(), this));
            if (material->isDefault())
            {
                // We delete the corresponding material cache when it can't be 
                // compiled.
                m_cache->destroyObject(trueId);
            }
        }

        PASSERT(material->isAvailable());
            
        insertResourceObject(material);

        return material;
    }

    return dynamic_cast<PMaterialResource *>(ret);
}
    
PMaterialResource *PResourceManager::createMaterial(const pchar *text, 
                                                    const pchar *id,
                                                    pbool autoRelease)
{
    // Check if the same material has been created
    if (id != P_NULL)
    {
        pchar trueId[1024];
        pStringToLower(id, 1024, trueId);
        PResourceObject *ret = findResourceObject(trueId);

        if (ret != P_NULL)
        {
            return dynamic_cast<PMaterialResource *>(ret);
        }
    }
    
    // Create a new material resource.
    id = m_cache->addMaterialCache(id, text, autoRelease);
    
    PMaterialResource *material = PNEW(PMaterialResource(id, text, this));
    if (material->isDefault())
    {
        // We delete the corresponding material cache when it can't be 
        // compiled.
        m_cache->destroyObject(id);
    }

    insertResourceObject(material);
    return material;
}

PFrameBuffer *PResourceManager::createFrameBuffer(const pchar *id,
                                                  puint32 width, 
                                                  puint32 height,
                                                  PGlTextureFormatEnum colorBufferFormat,
                                                  PGlTextureFormatEnum depthBufferFormat,
                                                  PGlTextureFormatEnum stencilBufferFormat)
{
    // FIXME: check the duplication of the user specified id with existing objects.
    
    PFrameBuffer* frameBuffer = PNEW(PFrameBuffer(width, 
                                                  height, 
                                                  colorBufferFormat, 
                                                  depthBufferFormat, 
                                                  stencilBufferFormat, 
                                                  id, 
                                                  this));
    if (!frameBuffer->isAvailable())
    {
        PDELETE(frameBuffer);
        return P_NULL;
    }
        
    insertResourceObject(frameBuffer);

    return frameBuffer;
}
    
PAnimationResource *PResourceManager::getAnimation(const pchar* id)
{
    pchar trueId[1024];
    pStringToLower(id, 1024, trueId);
    PResourceObject *animationResourceObject = findResourceObject(trueId);
    if (animationResourceObject == P_NULL)
    {
        PAnimationResource *animationResource = PNEW(PAnimationResource(this, trueId));
        if (!animationResource->load())
        {
            PDELETE(animationResource);
            return P_NULL;
        }

        insertResourceObject(animationResource);
        return animationResource;
    }
    
    return dynamic_cast<PAnimationResource *>(animationResourceObject);
}

PAnimationResource *PResourceManager::createAnimation(PAnimationTypeEnum animationType, const pchar* id)
{
    if (id != P_NULL)
    {
        pchar trueId[1024];
        pStringToLower(id, 1024, trueId);
        PResourceObject *animationResourceObject = findResourceObject(trueId);
        if (animationResourceObject != P_NULL)
        {
            PLOG_ERROR("An animation resource with same id (%s) already exists.", id);
            return P_NULL;
        }
    }
    
    PAnimationResource *animationResource = PNEW(PAnimationResource(this, id, animationType));
    insertResourceObject(animationResource);
    return animationResource;
}

void PResourceManager::optimize()
{
    // TODO: the memory management of resource manager.
}

pbool PResourceManager::resume()
{
    Iterator it = m_items.begin();
    Iterator ie = m_items.end();
    while (it != ie)
    {
        it.value()->restore();
        ++it;
    }
    return true;
}

void PResourceManager::pause()
{
    Iterator it = m_items.begin();
    Iterator ie = m_items.end();
    while (it != ie)
    {
        it.value()->discard();
        ++it;
    }
}

PResourceObject* PResourceManager::findResourceObject(const pchar *id)
{
    ConstIterator it = m_items.find(const_cast<pchar*>(id));
    if (it != m_items.end())
    {
        PResourceObject *object = it.value();
        
        // Swap in the object's content into memory if it is not there.
        if (!object->isAvailable())
        {
            object->restore();
        }
 
        return object;
    }

    return P_NULL;
}
    
void PResourceManager::insertResourceObject(PResourceObject *object)
{
    pchar *id = const_cast<pchar *>(object->id());
    m_items.insert(id, object);
    
    optimize();
}
    
void PResourceManager::deleteResourceObjects()
{
    Iterator it = m_items.begin();
    Iterator ie = m_items.end();
    while (it != ie)
    {
        PDELETE(it.value());
        ++it;
    }
}

void PResourceManager::removeResourceObject(const pchar *id)
{
    Iterator it = m_items.find(const_cast<pchar*>(id));
    if (it != m_items.end())
    {
        m_items.erase(it);
    }
    else
    {
        PLOG_ERROR("Failed to find the asset called %s", id);
    }
}
    
