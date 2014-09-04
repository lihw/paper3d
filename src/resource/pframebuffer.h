// pframebuffer.h
// The framebuffer resource
// 
// Copyright 2012 - 2014 Future Interface. 
// This software is licensed under the terms of the MIT license.
//
// Hongwei Li lihw81@gmail.com
//

#ifndef PFRAMEBUFFER_H
#define PFRAMEBUFFER_H


#include <Paper3D/presourceobject.h>
#include <Paper3D/ptexture.h>

#include <PFoundation/pgltexture.h>

class PRenderState;
class PGlFramebuffer;


class P_DLLEXPORT PFrameBuffer : public PResourceObject
{
    friend class PResourceManager;
    
    PFrameBuffer(const PFrameBuffer& other) : PResourceObject(P_NULL, P_NULL, P_RESOURCE_TYPE_UNKNOWN) {};
    void operator=(PFrameBuffer& other) {};

public:
    P_INLINE PGlFramebuffer *framebufferObject() const { return m_framebufferObject; }
    P_INLINE PTexture *colorBuffer() const { return m_colorTexture; }
    P_INLINE puint32 width() const { return m_width; }
    P_INLINE puint32 height() const { return m_height; }

    // Dump the current framebuffer content into this framebuffer.
    void copyFrameBuffer(PRenderState *renderState);

    // Create the default frame buffer. In iOS, the default framebuffer is created by user 
    // explicityly in the platform layer. It is not 0 as in win32 and android. We have to 
    // create a corresponding PFrameBuffer object in the paper3d layer to encapsulate this
    // platform specific implementation.
    static PFrameBuffer *createDefaultFrameBuffer();

protected:
    virtual void discardResource();
    virtual pbool restoreResource();

private:
    PFrameBuffer(puint32 width, 
                 puint32 height, 
                 PGlTextureFormatEnum colorBufferFormat, 
                 PGlTextureFormatEnum depthBufferFormat, 
                 PGlTextureFormatEnum stencilBufferFormat, 
                 const pchar* name, 
                 PResourceManager* resourceManager); 
    virtual ~PFrameBuffer();

    // Only used by createDefaultFrameBuffer();
    PFrameBuffer();

    pbool createFramebuffer(); 

private:
    PGlFramebuffer         *m_framebufferObject;
    puint32                 m_width;
    puint32                 m_height;
    PTexture               *m_colorTexture;
    PGlTextureFormatEnum    m_colorBufferFormat; 
    PGlTextureFormatEnum    m_depthBufferFormat; 
    PGlTextureFormatEnum    m_stencilBufferFormat;
};



#endif // !PFRAMEBUFFER_H


