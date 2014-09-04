// prendertarget.h
// The rendering target and parameters
//
// Copyright 2012 - 2014 Future Interface. 
// This software is licensed under the terms of the MIT license.
//
// Hongwei Li lihw81@gmail.com
//
// 

#ifndef PRENDERTARGET_H
#define PRENDERTARGET_H

#include <Paper3D/pframebuffer.h>

#include <PFoundation/pcolorrgba.h>
#include <PFoundation/pglstate.h>


class PRenderState;


class P_DLLEXPORT PRenderTarget
{
public:
    PRenderTarget();
    ~PRenderTarget();

    void setFrameBuffer(PFrameBuffer *framebuffer);
    
    void setBufferClearEnabled(pbool color, pbool depth, pbool stencil);
    
    // Default is (true, true, true, true)
    void setColorWriteMode(pbool red, pbool green, pbool blue, pbool alpha);
    void setColorClearValue(const PColorRGBA &color);
    P_INLINE const PColorRGBA &colorClearValue() const { return m_colorClearValue; }

    void setDepthWriteEnabled(pbool enabled);
    void setDepthClearValue(pfloat32 value);
    void setDepthTest(PGlStateEnum mode);

    void setStencilWriteEnabled(pbool enabled);
    // TODO: more about stencil manipulation.
    
    void setScissorTestEnabled(pbool enabled);
    void setScissor(puint32 x, puint32 y, puint32 width, puint32 height);

    void setBlend(PGlStateEnum mode);

    void setViewport(puint32 x, puint32 y, puint32 w, puint32 h);

    void use(PRenderState *renderState);

protected:
    PFrameBuffer           *m_framebuffer;

    puint32                m_viewport[4];

    pbool                  m_colorWriteMode[4];
    PColorRGBA             m_colorClearValue;

    pbool                  m_depthWriteEnabled; // Depth 
    pfloat32               m_depthClearValue;
    PGlStateEnum           m_depthTest;

    pbool                  m_stencilWriteEnabled;

    pbool                  m_scissorTestEnabled;
    puint32                m_scissor[4];

    // TODO: remove blend control here as the blending is turned on/off in PRenderQueue where
    // PRenderTarget is not accessible.
    PGlStateEnum           m_blend;

    pbool                  m_bufferClearEnabled[3];
};


#endif // !PRENDERTARGET_H
