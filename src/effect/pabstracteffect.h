// pabstracteffect.h
// Full screen post processing effect.
//
// Copyright 2012 - 2014 Future Interface . 
// This software is licensed under the terms of the MIT license.
//
// Hongwei Li lihw81@gmail.com
//

#ifndef PABSTRACTEFFECT_H
#define PABSTRACTEFFECT_H

#include <Paper3D/prendertarget.h>

class PScene;
class PMaterial;
class PGeometryPlane;
class PXmlElement;

enum PEffectBlendModeEnum
{
    P_EFFECTBLEND_MIX,
    P_EFFECTBLEND_OVERLAY,
    P_EFFECTBLEND_ADD,

    P_EFFECTBLEND_DEFAULT = P_EFFECTBLEND_MIX,

    P_EFFECTBLEND_FIRST  = P_EFFECTBLEND_MIX,
    P_EFFECTBLEND_LAST   = P_EFFECTBLEND_ADD,
    P_EFFECTBLEND_NUMBER = P_EFFECTBLEND_LAST - P_EFFECTBLEND_FIRST + 1,
};

class P_DLLEXPORT PAbstractEffect 
{
    friend class PScene;

    PAbstractEffect(const PAbstractEffect &other) {}
    void operator=(const PAbstractEffect &other) {}

protected:
    PAbstractEffect(PScene *scene);
    virtual ~PAbstractEffect();

public:
    P_INLINE PEffectBlendModeEnum blendMode() const { return m_blendMode; }
    P_INLINE PScene *scene() const { return m_scene; }
    
    void render(PRenderState *renderState);
    void setBlendMode(PEffectBlendModeEnum blendMode);
    
    virtual void unpack(PXmlElement *xmlElement);

protected:
    virtual void renderEffect(PRenderState *renderState) = 0;
    PFrameBuffer *readFrameBuffer() const;
    PFrameBuffer *writeFrameBuffer() const;
    void swapFrameBuffers();

protected:
    PEffectBlendModeEnum m_blendMode;
    PScene              *m_scene;
    PRenderTarget        m_renderTarget;
    PGeometryPlane      *m_blendRect;  // full-screen quad for blending
    PGeometryPlane      *m_effectRect; // full-screen quad for post-processing effect
    PMaterial           *m_material;
    PFrameBuffer        *m_framebuffers[2]; // pingpong the framebuffers in a multile pass rendering
    puint32              m_readFrameBuffer;
};


#endif // !PABSTRACTEFFECT_H
