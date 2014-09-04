// presourcemanager.h
// Resource management, e.g., shader, texture, vertex buffer, and framebuffer
//
// Copyright 2012 - 2014 Future Interface. 
// This software is licensed under the terms of the MIT license.
//
// Hongwei Li lihw81@gmail.com
//

#ifndef PRESOURCEMANAGER_H
#define PRESOURCEMANAGER_H

#include <Paper3D/presourceobject.h>
#include <Paper3D/panimationresource.h>

#include <PFoundation/pmodule.h>
#include <PFoundation/pstringmap.h>
#include <PFoundation/pgltexture.h>

class PContext;
class PMesh;
class PImage;
class PTexture;
class PMaterialResource;
class PFrameBuffer;
class PAnimationResource;
class PResourceCache;
class PArchiveFile;
struct PGlVertexFormat;
class PBox;


class P_DLLEXPORT PResourceManager : public PModule
{
    friend class PResourceObject;
    friend class PTexture;
    friend class PMaterialResource;
    friend class PMesh;
    friend class PAnimationResource;
    friend class PCacheObject;

    PResourceManager(const PResourceManager &other) : PModule(P_NULL, P_NULL) {}
    void operator=(const PResourceManager &other) {}

public:
    PResourceManager(PContext* context);
    virtual ~PResourceManager();

    // sphere/plane/torus/cube/cylinder are supported natively. 
    PMesh *getMesh(const pchar *id, pbool useGPUMemory = true);
    PMesh *createMesh(pfloat32 *vertices, puint32 numVertices, puint16 *indices,
        puint32 numIndices, const PBox &box, PGlVertexFormat *vertexFormat, 
        const pchar *id = P_NULL, pbool autoRelease = true, pbool useGPUMemory = true);
    
    // Get the texture with specified id. 
    PTexture *getTexture(const pchar *id, pbool mipmapEnabled = false);
    PTexture *createTexture(PImage* image, const pchar *id = P_NULL, pbool autoRelease = true, pbool mipmapEnabled = false); // 2D
    PTexture *createTexture(PImage** images, const pchar *id = P_NULL, pbool autoRelease = true, pbool mipmapEnabled = false); // Cubemap

    PMaterialResource *getMaterial(const pchar *id);
    PMaterialResource *createMaterial(const pchar *text, const pchar *id = P_NULL, pbool autoRelease = true);

    PFrameBuffer *createFrameBuffer(const pchar *id,
        puint32 width,
        puint32 height,
        PGlTextureFormatEnum colorBufferFormat,
        PGlTextureFormatEnum depthBufferFormat = P_GLTEXTURE_FORMAT_UNKNOWN,
        PGlTextureFormatEnum stencilBufferFormat = P_GLTEXTURE_FORMAT_UNKNOWN);

    PAnimationResource *getAnimation(const pchar *id);
    PAnimationResource *createAnimation(PAnimationTypeEnum animationType, const pchar *id = P_NULL);

    // Search the resource object with given name.
    PResourceObject* findResourceObject(const pchar *name);
    
    P_INLINE PContext *context() const { return m_context; }
    P_INLINE PArchiveFile *archive() const { return m_archive; }

    // Delete the recently unused objects and release memory.
    void optimize();

    virtual pbool initialize();
    // Restore the resource object when activity resumes from background.
    virtual pbool resume();
    // Discard resource objects when activity goes to background (paused)
    virtual void pause();

private:
    // Insert the resource object into the database. Note that we
    //string don't check the resource name duplication.
    void insertResourceObject(PResourceObject *object);
    // Delete all resource objects.
    void deleteResourceObjects();
    // Delete given resource object. 
    void removeResourceObject(const pchar *id);
    
    P_INLINE PResourceCache *cache() const { return m_cache; }

private:
    PResourceCache                       *m_cache;
    PArchiveFile                         *m_archive;
    PStringMap<PResourceObject *>         m_items; 

    typedef PStringMap<PResourceObject *>::iterator Iterator; 
    typedef PStringMap<PResourceObject *>::const_iterator ConstIterator; 
};


#endif // !PRESOURCEMANAGER_H
