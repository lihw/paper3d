// pframebuffer.cpp
// The framebuffer resource
// 
// Copyright 2012 - 2014 Future Interface. 
// This software is licensed under the terms of the MIT license.
//
// Hongwei Li lihw81@gmail.com
//

#include "pframebuffer.h"

#include <Paper3D/prenderstate.h>

#include <PFoundation/pstring.h>
#include <PFoundation/pglframebuffer.h>
#include <PFoundation/pnew.h>
#include <PFoundation/passert.h>
#include <PFoundation/plog.h>


PFrameBuffer::PFrameBuffer(puint32 width, 
                           puint32 height, 
                           PGlTextureFormatEnum colorBufferFormat, 
                           PGlTextureFormatEnum depthBufferFormat, 
                           PGlTextureFormatEnum stencilBufferFormat, 
                           const pchar* name, 
                           PResourceManager* resourceManager)
    : PResourceObject(name, resourceManager, P_RESOURCE_GPU)
{
    m_width = width;
    m_height = height;

    m_colorTexture = P_NULL;

    m_colorBufferFormat = colorBufferFormat;
    m_depthBufferFormat = depthBufferFormat;
    m_stencilBufferFormat = stencilBufferFormat;

    if (createFramebuffer())
    {
        m_available = true;
    }
}
    
PFrameBuffer::PFrameBuffer()
    : PResourceObject("default-framebuffer", P_NULL, P_RESOURCE_GPU)
{
    m_width  = 0xffffffff;
    m_height = 0xffffffff;

    m_colorTexture = P_NULL;

    m_colorBufferFormat   = P_GLTEXTURE_FORMAT_UNKNOWN;
    m_depthBufferFormat   = P_GLTEXTURE_FORMAT_UNKNOWN;
    m_stencilBufferFormat = P_GLTEXTURE_FORMAT_UNKNOWN;

    m_framebufferObject = P_NULL;
}
    
PFrameBuffer::~PFrameBuffer()
{
    PDELETE(m_framebufferObject);

    PDELETE(m_colorTexture);
}

void PFrameBuffer::discardResource()
{
    PDELETE(m_framebufferObject);

    m_colorTexture->discardResource();
}

pbool PFrameBuffer::restoreResource()
{
    m_framebufferObject = PNEW(PGlFramebuffer);
    if (!m_framebufferObject->create(m_width, 
                                     m_height, 
                                     m_colorBufferFormat,
                                     m_depthBufferFormat, 
                                     m_stencilBufferFormat))
    {
        PDELETE(m_framebufferObject);
        PLOG_ERROR("Failed to create framebuffer object %s", m_id);
        return false;
    }

    m_colorTexture->restoreResource(const_cast<PGlTexture *>(m_framebufferObject->colorBuffer()));

    return true;
}

pbool PFrameBuffer::createFramebuffer()
{
    // Create framebuffer object.
    m_framebufferObject = PNEW(PGlFramebuffer);
    if (!m_framebufferObject->create(m_width, 
                                     m_height, 
                                     m_colorBufferFormat,
                                     m_depthBufferFormat, 
                                     m_stencilBufferFormat))
    {
        PDELETE(m_framebufferObject);
        PLOG_ERROR("Failed to create framebuffer object %s", m_id);
        return false;
    }

    // Create texture
    if (m_framebufferObject)
    {
        PString textureName;
        if (m_framebufferObject->colorBuffer())
        {
            textureName = m_id;
            textureName += ":color-texture";
            m_colorTexture = PNEW(PTexture(textureName.c_str(), this));
        }
    }

    return true;
}
    
void PFrameBuffer::copyFrameBuffer(PRenderState *renderState)
{
    // Check if the m_colorTexture is currently in use. Enable it if not.
    renderState->useTexture(m_colorTexture, 0);

    m_colorTexture->textureObject()->copyTexture(P_GLTEXTURE_FORMAT_RGBA8888, 
        0, 0, m_width, m_height, 0);
}
    
PFrameBuffer *PFrameBuffer::createDefaultFrameBuffer()
{
    PFrameBuffer *ret = PNEW(PFrameBuffer);
    ret->m_available = true;
    ret->m_framebufferObject = PGlFramebuffer::createDefaultFramebuffer();
    return ret;
}
