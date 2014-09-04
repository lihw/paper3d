// prenderstate.h
// The rendering state
//
// Copyright 2012 - 2014 Future Interface. 
// This software is licensed under the terms of the MIT license.
//
// Hongwei Li lihw81@gmail.com
//

#ifndef PRENDERSTATE_H
#define PRENDERSTATE_H

#include <PFoundation/pglstate.h>
#include <PFoundation/pglvertexbuffer.h>
    
// FIXME: the texture unit number should be defined system layer or in global
// configuration.
#ifndef P_TEXTURE_UNIT_NUMBER
# define P_TEXTURE_UNIT_NUMBER 32
#endif


class PGlState;
class PMaterialResource;
class PFrameBuffer;
class PMesh;
class PTexture;



class P_DLLEXPORT PRenderState
{
public:
    PRenderState(const puint32 *initialViewport);
    ~PRenderState();  

    // Bind the current shader to GPU. It is already the current one, return immediately.
    void useMaterial(PMaterialResource *material);
    // Ditto.
    void useFrameBuffer(PFrameBuffer *frameBuffer);
    // Use the given mesh and vertex format. 
    void useVertexBuffer(PMesh* mesh);
    // Bind the texture to the GPU at the given the texture unit. 
    puint32 useTexture(PTexture *texture, puint32 textureUnit);

    // Reset the rendering state to initial value.
    void reset();

    // Right before rendering.
    void beginDraw();
    // Right after rendering.
    void endDraw();

    P_INLINE PGlState *renderStateObject() const { return m_state; }

    P_INLINE puint32 getFramebufferSwitchTimes() const { return m_framebufferSwitchTimes; }
    P_INLINE puint32 getShaderSwitchTimes() const { return m_shaderSwitchTimes; }
    P_INLINE puint32 getVBufferSwitchTimes() const { return m_vBufferSwitchTimes; }
    P_INLINE puint32 getTextureSwitchTimes() const { return m_textureSwitchTimes; }
    P_INLINE puint32 getTriangleCount() const { return m_triangleCount; }
    P_INLINE puint32 getVertexCount() const { return m_vertexCount; }

private:
    void releaseUsedResources();
    
private:
    PGlState           *m_state;
    PMaterialResource  *m_material;
    PFrameBuffer       *m_framebuffer;
    PMesh              *m_mesh;
    PTexture           *m_textures[P_TEXTURE_UNIT_NUMBER];
    puint32             m_numTextures;

    PFrameBuffer       *m_defaultFramebuffer;

    // Performance data
    // TODO: 
    //  - enable performance monitoring only after users requests.
    //  - separate profiling from the render state
    puint32         m_framebufferSwitchTimes;
    puint32         m_shaderSwitchTimes;
    puint32         m_vBufferSwitchTimes;
    puint32         m_textureSwitchTimes;
    puint32         m_triangleCount;
    puint32         m_vertexCount;
};

#endif // !PRENDERSTATE_H
