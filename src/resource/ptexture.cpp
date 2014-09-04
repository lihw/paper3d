// ptexture.h
// The texture resource
// 
// Copyright 2012 - 2014 Future Interface. 
// This software is licensed under the terms of the MIT license.
//
// Hongwei Li lihw81@gmail.com
//

#include "ptexture.h"

#include <Paper3D/presourcemanager.h>
#include <Paper3D/pframebuffer.h>

#include <PFoundation/pimage.h>
#include <PFoundation/pcontext.h>
#include <PFoundation/pgltexture.h>
#include <PFoundation/pglframebuffer.h>

#include "cache/presourcecache_private.h"


PTexture::PTexture(const pchar *id, PImage *image, PResourceManager* resourceManager, pbool mipmapEnabled) 
    : PResourceObject(id, resourceManager, P_RESOURCE_GPU)
    , m_isDefault(false)
    , m_mipmap(mipmapEnabled)
    , m_framebuffer(P_NULL)
{
    m_cubemap = false;

    m_width = 0;
    m_height = 0;

    PASSERT(image != P_NULL);
    m_textureObject = createTexture(image);
    if (m_textureObject == P_NULL)
    {
        m_isDefault = true;
        m_textureObject = createDefaultTexture();
        PASSERT(m_textureObject != P_NULL);
    }

    setLinearFilteringEnabled(true, true);
    setRepeatWrappingEnabled(false, true);

    m_available = true;
}

PTexture::PTexture(const pchar *id, PImage **images, PResourceManager* resourceManager, pbool mipmapEnabled) 
    : PResourceObject(id, resourceManager, P_RESOURCE_GPU)
    , m_isDefault(false)
    , m_mipmap(mipmapEnabled)
    , m_framebuffer(P_NULL)
{
    m_cubemap = true;
    
    m_width = 0;
    m_height = 0;

    m_textureObject = createCubeMapTexture(images);
    if (m_textureObject == P_NULL)
    {
        m_isDefault = true;
        m_textureObject = createDefaultCubeMapTexture();
        PASSERT(m_textureObject != P_NULL);
    }

    setLinearFilteringEnabled(true, true);
    setRepeatWrappingEnabled(false, true);
    
    m_available = true;
}

PTexture::PTexture(const pchar *id, PFrameBuffer* framebuffer)
    : PResourceObject(id, P_NULL, P_RESOURCE_GPU)
    , m_framebuffer(framebuffer)
    , m_isDefault(false)
    , m_mipmap(false)
{
    m_cubemap = false;

    m_textureObject = const_cast<PGlTexture *>(framebuffer->framebufferObject()->colorBuffer());

    setLinearFilteringEnabled(true, true);
    setRepeatWrappingEnabled(false, true);

    m_width  = framebuffer->width();
    m_height = framebuffer->height();
    
    m_available = true;
}

PTexture::~PTexture()
{
    // FIXME: what if the texture is in use now?
    if (m_framebuffer == P_NULL)
    {
        PDELETE(m_textureObject);
    }
}

puint32 PTexture::textureBytes() const
{
    if (m_textureObject != P_NULL)
    {
        return m_textureObject->bytes();
    }

    return 0;
}

void PTexture::release()
{
    if (m_framebuffer == P_NULL)
    {
        PResourceObject::release();
    }
    else
    {
        m_framebuffer->release();
    }
}

void PTexture::retain()
{
    if (m_framebuffer == P_NULL)
    {
        PResourceObject::retain();
    }
    else
    {
        m_framebuffer->retain();
    }
}
    
void PTexture::setRepeatWrappingEnabled(bool enabled, bool force)
{
    if (m_repeatWrapping == enabled && !force)
    {
        return ;
    }

    if (m_textureObject != P_NULL)
    {
        if (enabled)
        {
            m_textureObject->setWrapMode(P_GLTEXTURE_WRAPMODE_REPEAT,
                    P_GLTEXTURE_WRAPMODE_REPEAT);
        }
        else
        {
            m_textureObject->setWrapMode(P_GLTEXTURE_WRAPMODE_CLAMP_TO_EDGE,
                    P_GLTEXTURE_WRAPMODE_CLAMP_TO_EDGE);
        }
        m_repeatWrapping = enabled;
    }
}

void PTexture::setLinearFilteringEnabled(bool enabled, bool force)
{
    if (m_linearFiltering == enabled && !force)
    {
        return ;
    }

    if (m_textureObject != P_NULL)
    {
        if (enabled)
        {
            m_textureObject->setFiltering(P_GLTEXTURE_FILTERING_LINEAR, 
                                          P_GLTEXTURE_FILTERING_LINEAR);
        }
        else 
        {
            m_textureObject->setFiltering(P_GLTEXTURE_FILTERING_NEAREST, 
                                          P_GLTEXTURE_FILTERING_NEAREST);
        }
    }

    m_linearFiltering = enabled;
}

void PTexture::discardResource()
{
    if (m_framebuffer == P_NULL)
    {
        PDELETE(m_textureObject);
    }
    else
    {
        m_textureObject = P_NULL;
    }
}

void PTexture::restoreResource(PGlTexture *textureObject)
{
    PASSERT(m_textureObject == P_NULL);
    if (m_textureObject == P_NULL)
    {
        m_textureObject = textureObject;

        setLinearFilteringEnabled(m_linearFiltering, true);
        setRepeatWrappingEnabled(m_repeatWrapping, true);
    }
}

pbool PTexture::restoreResource()
{
    // The framebuffer bound texture will be restored after framebuffer is restored.
    if (m_framebuffer != P_NULL)
    {
        return true;
    }
        
    PASSERT(m_textureObject == P_NULL);
    if (m_textureObject == P_NULL)
    {
        if (m_isDefault)
        {
            m_textureObject = m_cubemap? createDefaultTexture() : createDefaultCubeMapTexture();
        }
        else
        {
            PResourceCache *cache = parent()->cache();
            PImage *images[6];
            puint32 numImages;
            cache->getImages(m_id, images, &numImages);

            if (numImages == 6)
            { 
                m_textureObject = createCubeMapTexture(images);
            }
            else if (numImages == 1)
            {
                m_textureObject = createTexture(images[0]);
            }
            else
            {
                PLOG_ERROR("Unsupported texture format");
                return false;
            }
        }
    }
    else
    {
        PLOG_ERROR("Failed to restore texture %s because old texture has not been discarded");
        return false;
    }

    setLinearFilteringEnabled(m_linearFiltering, true);
    setRepeatWrappingEnabled(m_repeatWrapping, true);

    return true;
}
    
PGlTexture* PTexture::createTexture(const PImage *image)
{
    PGlTexture* ret = PNEW(PGlTexture);
    switch (image->pixelFormat())
    {
        case P_IMAGE_PIXELFORMAT_RGBA8888:
            if (!ret->create(image->data(), image->width(), image->height(),
                    P_GLTEXTURE_FORMAT_RGBA8888, m_mipmap))
            {
                PDELETE(ret);
                return P_NULL;
            }
            break;
        case P_IMAGE_PIXELFORMAT_RGB888:
            if (!ret->create(image->data(), image->width(), image->height(),
                    P_GLTEXTURE_FORMAT_RGB888, m_mipmap))
            {
                PDELETE(ret);
                return P_NULL;
            }
            break;
		case P_IMAGE_PIXELFORMAT_RA88:
            if (!ret->create(image->data(), image->width(), image->height(),
                    P_GLTEXTURE_FORMAT_RA88, m_mipmap))
            {
                PDELETE(ret);
                return P_NULL;
            }
            break;
        case P_IMAGE_PIXELFORMAT_R8:
            if (!ret->create(image->data(), image->width(), image->height(),
                    P_GLTEXTURE_FORMAT_R8, m_mipmap))
            {
                PDELETE(ret);
                return P_NULL;
            }
            break;
        default:
            PLOG_ERROR("Unsupported image pixel format");
            PDELETE(ret);
            return P_NULL;
    }

    m_width = image->width();
    m_height = image->height();

    return ret;
}

PGlTexture *PTexture::createCubeMapTexture(PImage **images)
{
    // Get data from images.
    puint8 *data[6];
    puint32 width[6];
    puint32 height[6];

    for (pint32 i = 0; i < 6; i++)
    {
        data[i] = images[i]->data();
        width[i] = images[i]->width();
        height[i] = images[i]->height();
    }
    
    PGlTexture* ret = PNEW(PGlTexture);

    // Create PGlTexture.
    switch (images[0]->pixelFormat())
    {
        case P_IMAGE_PIXELFORMAT_RGBA8888:
            if (!ret->create(data, width, height, P_GLTEXTURE_FORMAT_RGBA8888, m_mipmap))
            {
                PDELETE(ret);
                return P_NULL;
            }
            break;
        case P_IMAGE_PIXELFORMAT_RGB888:
            if (!ret->create(data, width, height, P_GLTEXTURE_FORMAT_RGB888, m_mipmap))
            {
                PDELETE(ret);
                return P_NULL;
            }
            break;
        case P_IMAGE_PIXELFORMAT_R8:
            if (!ret->create(data, width, height, P_GLTEXTURE_FORMAT_R8, m_mipmap))
            {
                PDELETE(ret);
                return P_NULL;
            }
            break;
        default:
            PLOG_ERROR("Unsupported image pixel format");
            PDELETE(ret);
            return P_NULL;
    }

    m_width = width[0];
    m_height = height[0];

    return ret;
}

PGlTexture* PTexture::createDefaultTexture()
{
    const puint32 imageWidth = 64;
    const puint32 imageHeight = 64;
    
    PImage image(P_NULL, imageWidth, imageHeight, P_IMAGE_PIXELFORMAT_RGB888);

    puint8 *imageData = image.data();

    // TODO: illustrate the positive y direction in this default texture.
    // A chess board pattern
    for (puint32 i = 0; i < imageHeight / 8; ++i)
    {
        for (puint32 j = 0; j < imageWidth / 8; ++j)
        {
            for (puint32 y = 0; y < 8; ++y)
            {
                for (puint32 x = 0; x < 8; ++x)
                {
                    puint32 posx = j * 8 + x;
                    puint32 posy = i * 8 + y;

                    puint32 index = posy * 64 + posx;

                    puint8 c = 255;
                    if ((i + j) % 2 == 0)
                    {
                        c = 127;
                    }

                    imageData[index * 3] = c;
                    imageData[index * 3 + 1] = c;
                    imageData[index * 3 + 2] = c;
                }
            }
        }
    }

    PGlTexture* ret = createTexture(&image);
    if (ret == P_NULL)
    {
        PLOG_ERROR("Failed to create default texture");
    }

    return ret;
}

PGlTexture* PTexture::createDefaultCubeMapTexture()
{
    PASSERT_NOTIMPLEMENTED();
    return P_NULL;
}
