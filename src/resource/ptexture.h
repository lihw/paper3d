// ptexture.h
// The texture resource
// 
// Copyright 2012 - 2014 Future Interface. 
// This software is licensed under the terms of the MIT license.
//
// Hongwei Li lihw81@gmail.com
//

#ifndef PTEXTURE_H
#define PTEXTURE_H

#include <Paper3D/presourceobject.h>

#include <PFoundation/pimage.h>

// TODO: reduce the overlapping of ptexture and pgltexture

class PFrameBuffer;
class PGlTexture;
class PFrameBuffer;

class P_DLLEXPORT PTexture : public PResourceObject
{
    friend class PResourceManager;
    friend class PFrameBuffer;
    
    PTexture(const PTexture& other) : PResourceObject(P_NULL, P_NULL, P_RESOURCE_TYPE_UNKNOWN) {};
    void operator=(PTexture& other) {};

public:
    P_INLINE pbool isMipmap() const { return m_mipmap; }
    P_INLINE pbool isLinearFiltering() const { return m_linearFiltering; }
    P_INLINE pbool isCubeMap() const { return m_cubemap; }
    P_INLINE pbool isRepeatWrapping() const { return m_repeatWrapping; }
    P_INLINE puint32 width() const { return m_width; }
    P_INLINE puint32 height() const { return m_height; }

    void setRepeatWrappingEnabled(pbool enabled, pbool force = false);
    void setLinearFilteringEnabled(pbool enabled, pbool force = false);
    puint32 textureBytes() const;
    PGlTexture *textureObject() const { return m_textureObject; }

    virtual void release();
    virtual void retain();

    // TODO: void updateData();

protected:
    virtual void discardResource();
    virtual pbool restoreResource();

private:
    PTexture(const pchar *id, PImage *image, PResourceManager *resourceManager, pbool mipmapEnabled = false); 
    PTexture(const pchar *id, PImage **images, PResourceManager *resourceManager, pbool mipmapEnabled = false); 
    PTexture(const pchar *id, PFrameBuffer *frameBuffer); 
    virtual ~PTexture();

    PGlTexture *createDefaultTexture();
    PGlTexture *createDefaultCubeMapTexture();
    PGlTexture *createTexture(const PImage *image);
    PGlTexture *createCubeMapTexture(PImage **images);

    // Called by PFrameBuffer to restore this texture object.
    void restoreResource(PGlTexture *textureObject);

private:
    PGlTexture     *m_textureObject;
    pbool           m_mipmap;
    pbool           m_linearFiltering;
    puint32         m_width;
    puint32         m_height;
    pbool           m_cubemap;
    PFrameBuffer   *m_framebuffer; // If it is an attachment of a framebuffer
    pbool           m_isDefault;
    pbool           m_repeatWrapping;
};



#endif // !PTEXTURE_H



