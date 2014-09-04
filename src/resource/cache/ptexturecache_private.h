// ptexturecache_private.h
// Image data of a texture
//
// Copyright 2012 - 2014 Future Interface. 
// This software is licensed under the terms of the MIT license.
//
// Hongwei Li lihw81@gmail.com
//

#ifndef PTEXTURECACHE_PRIVATE_H
#define PTEXTURECACHE_PRIVATE_H

#include <PFoundation/pstring.h>

#include "pcacheobject_private.h"

class PImage;


class P_DLLEXPORT PTextureCache : public PCacheObject
{
    friend class PResourceCache;

    PTextureCache(const PTextureCache &other) : PCacheObject(P_NULL, P_NULL, true) {}
    void operator=(const PTextureCache &other) {}

public:
    void getImages(PImage **out_images, puint32 *out_numImages) const;
    P_INLINE puint32 getNumberOfImages() const
    { return m_numImages; }
    
protected:
    virtual void swapIn();
    virtual void swapOut();

private:
    // Load image from archive or file
    PTextureCache(PResourceCache *parent, const pchar *id);
    // Read image from memory
    PTextureCache(PResourceCache *parent, const pchar *id, 
        PImage **images, puint32 numImages, pbool autoRelease);
    virtual ~PTextureCache();

    pbool loadImages(const pchar *id);

private:
    PImage   *m_images[6];
    puint32   m_numImages;
};


#endif // !PTEXTURECACHE_PRIVATE_H
