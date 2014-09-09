// prenderstate.cpp
// The rendering state
//
// Copyright 2012 - 2014 Future Interface. 
// This software is licensed under the terms of the MIT license.
//
// Jihao Xu xujihao@gmail.com and Hongwei Li lihw81@gmail.com
//

#include "prenderstate.h"

#include <Paper3D/pmaterialresource.h>
#include <Paper3D/pframebuffer.h>
#include <Paper3D/pmesh.h>
#include <Paper3D/ptexture.h>

#include <PFoundation/pglstate.h>
#include <PFoundation/pnew.h>
#include <PFoundation/plog.h>
#include <PFoundation/pcrt.h>
#include <PFoundation/passert.h>
#include <PFoundation/pglshader.h>
#include <PFoundation/pglframebuffer.h>
#include <PFoundation/pglvertexbuffer.h>

PRenderState::PRenderState(const puint32 *initialViewport)
{
    m_state = PNEW(PGlState(initialViewport));
    
    m_shaderSwitchTimes         = 0;
    m_vBufferSwitchTimes        = 0;
    m_textureSwitchTimes        = 0;
    m_framebufferSwitchTimes    = 0;
    m_triangleCount             = 0;
    m_vertexCount               = 0;

    m_material                  = P_NULL;
    m_framebuffer               = P_NULL;
    m_mesh                      = P_NULL;
    m_numTextures               = 0;
    pmemset(m_textures, 0, sizeof(m_textures));

    m_defaultFramebuffer        = PFrameBuffer::createDefaultFrameBuffer();
    m_defaultFramebuffer->retain();
    useFrameBuffer(m_defaultFramebuffer);
}

PRenderState::~PRenderState()
{
    releaseUsedResources();

    PDELETE(m_state);
}
    
void PRenderState::useMaterial(PMaterialResource *material)
{
    if (m_material == material)
    {
        return;
    }

    if (m_material != P_NULL)
    {
        PGlShader *shaderObject = m_material->shader();
        shaderObject->disable();
        m_material->release();
    }

    m_material = material;

    if (m_material != P_NULL)
    {
        PGlShader *shaderObject = m_material->shader();
        shaderObject->enable();
        m_material->retain();
    }

    ++m_shaderSwitchTimes;
}

void PRenderState::useFrameBuffer(PFrameBuffer* framebuffer)
{
    if (m_framebuffer == framebuffer ||
        (m_framebuffer == m_defaultFramebuffer && framebuffer == P_NULL))
    {
        return ;
    }

    // Disable old frame buffer and bind default frame buffer (screen buffer).
    if (m_framebuffer != P_NULL)
    {
        m_framebuffer->release();
    }

    m_framebuffer = framebuffer; 

    if (m_framebuffer == P_NULL)
    {
        m_framebuffer = m_defaultFramebuffer;
    }

    m_framebuffer->retain();
    PGlFramebuffer *framebufferObject = m_framebuffer->framebufferObject();
    framebufferObject->enable();

    ++m_framebufferSwitchTimes;
}

void PRenderState::useVertexBuffer(PMesh* mesh)
{
    if (m_mesh != mesh) 
    {
        if (m_mesh != P_NULL)
        {
            PGlVertexBuffer *vertexBufferObject = m_mesh->vertexBufferObject();
            vertexBufferObject->disable();
            m_mesh->release();
        }

        m_mesh = mesh; 

        if (m_mesh != P_NULL)
        {
            PGlVertexBuffer *vertexBufferObject = m_mesh->vertexBufferObject();
            vertexBufferObject->enable();

            m_mesh->retain();
                
            m_triangleCount += vertexBufferObject->numberOfPrimitives();
            m_vertexCount += vertexBufferObject->numberOfVertices();
        }

        ++m_vBufferSwitchTimes;
    }
}

puint32 PRenderState::useTexture(PTexture *texture, puint32 textureUnit)
{
    PASSERT(textureUnit < P_TEXTURE_UNIT_NUMBER);
    if (textureUnit >= P_TEXTURE_UNIT_NUMBER)
    {
        PLOG_WARNING("Exceed maximum supported texture unit.");
        return 0xffffffff;
    }

    // In some cases, the texture has been enabled and attached to 
    // a different texture unit from the input one. 
    if (texture->textureObject()->isEnabled())
    {
        textureUnit = texture->textureObject()->textureUnit(); 
        PASSERT(m_textures[textureUnit] == texture);
        return textureUnit;
    }

    if (m_textures[textureUnit] != P_NULL)
    {
        m_textures[textureUnit]->textureObject()->disable();
        m_textures[textureUnit]->release();

        --m_numTextures;
        ++m_textureSwitchTimes;
    }

    m_textures[textureUnit] = texture;

    if (m_textures[textureUnit] != P_NULL)
    {
        m_textures[textureUnit]->retain();
        m_textures[textureUnit]->textureObject()->enable(textureUnit);

        ++m_numTextures;
    }

    return textureUnit;
}

void PRenderState::beginDraw()
{
    m_shaderSwitchTimes = 0;
    m_vBufferSwitchTimes = 0;
    m_textureSwitchTimes = 0;
    m_framebufferSwitchTimes = 0;
    m_triangleCount = 0;
    m_vertexCount = 0;
}

void PRenderState::endDraw()
{
}

void PRenderState::reset()
{
    m_shaderSwitchTimes      = 0;
    m_vBufferSwitchTimes     = 0;
    m_textureSwitchTimes     = 0;
    m_framebufferSwitchTimes = 0;
    m_triangleCount          = 0;
    m_vertexCount            = 0;

    releaseUsedResources();

    m_material    = P_NULL;
    m_framebuffer = P_NULL;
    m_mesh        = P_NULL;
    m_numTextures = 0;
    pmemset(m_textures, 0, sizeof(m_textures));
    
    m_state->reset();
}

void PRenderState::releaseUsedResources()
{
    if (m_mesh != P_NULL)
    {
        m_mesh->release();
    }
    if (m_material != P_NULL)
    {
        m_material->release();
    }
    for (puint32 i = 0; i < P_TEXTURE_UNIT_NUMBER; ++i)
    {
        if (m_textures[i] != P_NULL)
        {
            m_textures[i]->release();
        }
    }
        
    m_framebuffer->release();
    m_defaultFramebuffer->release();
}
