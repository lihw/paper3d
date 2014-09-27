// ptexturecache.cpp
// Image data of a texture
//
// Copyright 2012 - 2014 Future Interface. 
// This software is licensed under the terms of the MIT license.
//
// Hongwei Li lihw81@gmail.com
//

#include "ptexturecache_private.h"

#include <PFoundation/pimage.h>
#include <PFoundation/parchivefile.h>
#include <PFoundation/pinputstream.h>

    
PTextureCache::PTextureCache(PResourceCache *parent, const pchar *id)
    : PCacheObject(parent, id, false)
{
    loadImages(id);

    if (archive()->isPathInArchive(id))
    {
        m_storageType = P_RESOURCE_FROM_ARCHIVE;
    }
    else
    {
        m_storageType = P_RESOURCE_FROM_FILE;
    }
}

PTextureCache::PTextureCache(PResourceCache *parent, const pchar *id, 
    PImage **images, puint32 numImages, pbool autoRelease)
    : PCacheObject(parent, id, autoRelease)
{
    m_storageType = P_RESOURCE_FROM_MEMORY;

    PASSERT(numImages > 0);
    m_numImages = numImages;
    for (puint32 i = 0; i < m_numImages; ++i)
    {
        if (autoRelease)
        {
            m_images[i] = images[i];
        }
        else
        {
            m_images[i] = PNEW(PImage(*images[i]));
        }
    }
}

PTextureCache::~PTextureCache()
{
    // The memory is managed by asset itself.
    if (!m_autoRelease)
    {
        for (puint32 i = 0; i < m_numImages; ++i)
        {
            PDELETE(m_images[i]);
        }
    }
}
    
void PTextureCache::getImages(PImage **out_images, puint32 *out_numImages) const
{
    *out_numImages = m_numImages;
    for (puint32 i = 0; i < m_numImages; ++i)
    {
        out_images[i] = m_images[i];
    }
}

void PTextureCache::swapIn()
{
    PASSERT(!m_available);
    if (!m_autoRelease && !m_available)
    {
        if (m_storageType == P_RESOURCE_FROM_ARCHIVE ||
            m_storageType == P_RESOURCE_FROM_FILE)
        {
            if (loadImages(m_id))
            {
                m_available = true;
            }
        }
        else if (m_storageType == P_RESOURCE_FROM_MEMORY)
        {
            if (!m_autoRelease)
            {
                // TODO: read in the temporary file written at swapping out.
            }
            m_available = true;
        }
        else
        {
            PLOG_ERROR("Unknown storage type of image data %s", m_id);
        }
    }
}

void PTextureCache::swapOut()
{
    PASSERT(m_available);
    if (!m_autoRelease && m_available)
    {
        if (m_storageType == P_RESOURCE_FROM_MEMORY)
        {
            // TODO: output the image to the temporary file.
        }

        for (puint32 i = 0; i < m_numImages; ++i)
        {
            PDELETE(m_images[i]);
        }

        m_available = false;
    }
}
    
pbool PTextureCache::loadImages(const pchar *id)
{
    const pchar *asterisk = pstrchr(id, '*');
    if (asterisk != P_NULL)
    {
        const pchar *suffix = pstrchr(asterisk, '.');
        const pchar *faceNames[6] = 
        {
            "posx",
            "negx",
            "posy",
            "negy",
            "posz",
            "negz"
        };

        m_numImages = 6;
        m_images[0] = P_NULL;
        m_images[1] = P_NULL;
        m_images[2] = P_NULL;
        m_images[3] = P_NULL;
        m_images[4] = P_NULL;
        m_images[5] = P_NULL;

        pchar dirPath[1024];
        pstrncpy(dirPath, id, asterisk - id);

        for (pint32 i = 0; i < 6; i++)
        {
            pchar path[1024];
            psprintf(path, 1024, "%s%s%s", dirPath, faceNames[i], suffix);

            PInputStream inputStream;
            if (!archive()->createInputStream(path, &inputStream))
            {
                PLOG_ERROR("Failed to read image at %s", path);
                goto fail_ret;
            }

            m_images[i] = PNEW(PImage(path, inputStream));
            if (m_images[i]->data() == P_NULL)
            {
                PLOG_ERROR("Failed to read image at %s", path);
                goto fail_ret;
            }
        }

        return true;
    }
    else
    {
        m_numImages = 0;
            
        PInputStream inputStream;
        if (!archive()->createInputStream(id, &inputStream))
        {
            return false;
        }

        PImage* image = PNEW(PImage(id, inputStream));
        if (image->data() == P_NULL)
        {
            PLOG_ERROR("Failed to read image at %s", id);
            PDELETE(image);

            return false;
        }
        else
        {
            m_numImages = 1;
            m_images[0] = image;
        
            return true;
        }
    }

    return true;
                
fail_ret:
    PDELETE(m_images[0]);
    PDELETE(m_images[1]);
    PDELETE(m_images[2]);
    PDELETE(m_images[3]);
    PDELETE(m_images[4]);
    PDELETE(m_images[5]);

    m_numImages = 0;

    m_numImages = 0;

    return false;
}
