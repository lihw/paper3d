// prendertarget.cpp
// The rendering target. 
//
// Copyright 2012 - 2014 Future Interface. 
// This software is licensed under the terms of the MIT license.
//
// Hongwei Li lihw81@gmail.com and Hongwei Li lihw81@gmail.com
//
// 
// 

#include "prendertarget.h"

#include <Paper3D/prenderstate.h>

#include <PFoundation/pglstate.h>
#include <PFoundation/pglframebuffer.h>
#include <PFoundation/passert.h>
    
PRenderTarget::PRenderTarget()
{
    m_viewport[0] = 0;
    m_viewport[1] = 0;
    m_viewport[2] = 640;
    m_viewport[3] = 480;

    m_framebuffer = P_NULL;

    m_colorWriteMode[0] = true;
    m_colorWriteMode[1] = true;
    m_colorWriteMode[2] = true;
    m_colorWriteMode[3] = true;
    m_colorClearValue   = P_COLOR_GRAY;

    m_depthWriteEnabled = true;
    m_depthClearValue   = 1.0f;
    m_depthTest         = P_GLDEPTHTEST_DEFAULT;

    m_stencilWriteEnabled  = false;
    // TODO:
    // more stencil

    m_scissorTestEnabled = false;
    m_scissor[0] = m_viewport[0];
    m_scissor[1] = m_viewport[1];
    m_scissor[2] = m_viewport[2];
    m_scissor[3] = m_viewport[3];

    m_blend = P_GLBLEND_DEFAULT;

    m_bufferClearEnabled[0] = true;
    m_bufferClearEnabled[1] = true;
    m_bufferClearEnabled[2] = true;
}

PRenderTarget::~PRenderTarget()
{
    if (m_framebuffer != P_NULL)
    {
        m_framebuffer->release();
    }
}

void PRenderTarget::setColorClearValue(const PColorRGBA &color)
{
    m_colorClearValue = color;
}

void PRenderTarget::setFrameBuffer(PFrameBuffer *framebuffer) 
{ 
    // Release the old buffer.
    if (m_framebuffer != P_NULL)
    {
        m_framebuffer->release();
    }

    m_framebuffer = framebuffer; 
    if (m_framebuffer != P_NULL)
    {
        m_framebuffer->retain();
    }
}

void PRenderTarget::setColorWriteMode(pbool red, pbool green, pbool blue, pbool alpha)
{
    m_colorWriteMode[0] = red;
    m_colorWriteMode[1] = green;
    m_colorWriteMode[2] = blue;
    m_colorWriteMode[3] = alpha;
}

void PRenderTarget::setDepthWriteEnabled(pbool enabled)
{
    m_depthWriteEnabled = enabled;
}
    
void PRenderTarget::setDepthTest(PGlStateEnum mode)
{
    m_depthTest = mode;
}

void PRenderTarget::setStencilWriteEnabled(pbool enabled)
{
    m_stencilWriteEnabled = enabled;
}

void PRenderTarget::setScissorTestEnabled(bool enabled)
{
    m_scissorTestEnabled = enabled;
}

void PRenderTarget::setScissor(puint32 left, puint32 bottom, puint32 width, puint32 height)
{
    m_scissor[0] = left;
    m_scissor[1] = bottom;
    m_scissor[2] = width;
    m_scissor[3] = height;
}

void PRenderTarget::setBlend(PGlStateEnum mode)
{
    m_blend = mode;
}

void PRenderTarget::setBufferClearEnabled(bool color, bool depth, bool stencil)
{
    m_bufferClearEnabled[0] = color;
    m_bufferClearEnabled[1] = depth;
    m_bufferClearEnabled[2] = stencil;
}

void PRenderTarget::setViewport(puint32 x, puint32 y, puint32 w, puint32 h)
{
    m_viewport[0] = x;
    m_viewport[1] = y;
    m_viewport[2] = w;
    m_viewport[3] = h;
}

void PRenderTarget::use(PRenderState *renderState)
{
    PGlState *rso = renderState->renderStateObject();
    PASSERT(rso != P_NULL);

    // Viewport
    rso->setViewport(m_viewport[0], m_viewport[1], m_viewport[2], m_viewport[3]);

    // Scissor
    rso->setScissorTestEnabled(m_scissorTestEnabled);
    rso->setScissorRegion(m_scissor[0], m_scissor[1], m_scissor[2], m_scissor[3]);

    // Color 
    rso->setColorClearValue(m_colorClearValue.getRFloat(), 
                            m_colorClearValue.getGFloat(), 
                            m_colorClearValue.getBFloat(), 
                            m_colorClearValue.getAFloat());

    rso->setColorWriteEnabled(m_colorWriteMode[0], 
                              m_colorWriteMode[1], 
                              m_colorWriteMode[2], 
                              m_colorWriteMode[3]);

    // Depth 
    rso->setDepthClearValue(m_depthClearValue);
    rso->setDepthTest(m_depthTest);
    rso->setDepthWriteEnabled(m_depthWriteEnabled);

    // Stencil
    // TODO:
    
    // Blending
    rso->setBlend(m_blend);

    // Enable the framebuffer
    renderState->useFrameBuffer(m_framebuffer);
    
    // Clear the buffer
    PGlFramebuffer::clearFramebuffer(
            m_bufferClearEnabled[0],
            m_bufferClearEnabled[1],
            m_bufferClearEnabled[2]);

    PLOG_INFO("clear value: %f, %f, %f", 
        m_colorClearValue.getRFloat(),
        m_colorClearValue.getGFloat(),
        m_colorClearValue.getBFloat());
}
    
