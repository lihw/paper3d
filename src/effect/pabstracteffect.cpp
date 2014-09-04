// pabstracteffect.cpp
// Full screen post processing effect.
//
// Copyright 2012 - 2014 Future Interface. 
// This software is licensed under the terms of the MIT license.
//
// Hongwei Li lihw81@gmail.com
//

#include "pabstracteffect.h"

#include <Paper3D/pgeometryplane.h>
#include <Paper3D/pmaterial.h>
#include <Paper3D/pscene.h>
#include <Paper3D/presourcemanager.h>
#include <Paper3D/prenderstate.h>
#include <Paper3D/pmaterialparameter.h>

#include <PFoundation/pxmlelement.h>
#include <PFoundation/pcontext.h>
#include <PFoundation/pglframebuffer.h>

#include "peffectblendmaterial_private.h"


PAbstractEffect::PAbstractEffect(PScene *scene)
{
    PASSERT(m_scene != P_NULL);
    m_scene = scene;
    m_scene->addEffect(this);
    
    m_blendMode = P_EFFECTBLEND_DEFAULT;

    PResourceManager *resourceManager = scene->context()->module<PResourceManager>("resource-manager");

    m_blendRect = PNEW(PGeometryPlane(resourceManager));
    m_blendRect->retain();
    m_effectRect = PNEW(PGeometryPlane(resourceManager));
    m_effectRect->retain();

    m_material = PNEW(PMaterial("effectmix.pmt", 
        P_EFFECTBLEND_MATERIAL_MIX, true, resourceManager));
    m_material->retain();
    m_blendRect->calibrate(m_material->vertexFormat());

    const puint32 *rect = scene->context()->rect();
    m_renderTarget.setBufferClearEnabled(false, false, false);
    m_renderTarget.setDepthTest(P_GLDEPTHTEST_DISABLED);
    m_renderTarget.setDepthWriteEnabled(false);
    m_renderTarget.setViewport(rect[0], rect[1], rect[2], rect[3]);

    m_framebuffers[0] = resourceManager->createFrameBuffer(P_NULL,
        rect[2], rect[3], P_GLTEXTURE_FORMAT_RGBA8888);
    m_framebuffers[0]->retain();
    
    m_framebuffers[1] = resourceManager->createFrameBuffer(P_NULL,
        rect[2], rect[3], P_GLTEXTURE_FORMAT_RGBA8888);
    m_framebuffers[1]->retain();

    m_framebuffers[0]->colorBuffer()->setLinearFilteringEnabled(false);
    m_framebuffers[1]->colorBuffer()->setLinearFilteringEnabled(false);

    m_readFrameBuffer = 0;
}

PAbstractEffect::~PAbstractEffect()
{
    if (m_scene != P_NULL)
    {
        m_scene->removeEffect(this);
    }

    m_effectRect->release();
    m_blendRect->release();

    m_material->release();

    m_framebuffers[0]->release();
    m_framebuffers[1]->release();
}
    
void PAbstractEffect::setBlendMode(PEffectBlendModeEnum blendMode)
{
    if (m_blendMode != blendMode)
    {
        PResourceManager *resourceManager = m_scene->context()->module<PResourceManager>("resource-manager");


        m_blendMode = blendMode;

        m_material->release();

        switch (m_blendMode)
        {
            case P_EFFECTBLEND_MIX:
                m_material = PNEW(PMaterial("effectmix.pmt", 
                     P_EFFECTBLEND_MATERIAL_MIX, true, resourceManager));
                m_material->retain();
                break;
            case P_EFFECTBLEND_OVERLAY:
                m_material = PNEW(PMaterial("effectoverlay.pmt", 
                     P_EFFECTBLEND_MATERIAL_OVERLAY, true, resourceManager));
                m_material->retain();
                break;
            case P_EFFECTBLEND_ADD:
                m_material = PNEW(PMaterial("effectadd.pmt", 
                     P_EFFECTBLEND_MATERIAL_ADD, true, resourceManager));
                m_material->retain();
                break;
            default:
                PASSERT_NOTREACHABLE("Unknown effect blending type.");
                break;
        }

        m_blendRect->calibrate(m_material->vertexFormat());

    }
}

PFrameBuffer *PAbstractEffect::readFrameBuffer() const
{
    return m_framebuffers[m_readFrameBuffer];
}

PFrameBuffer *PAbstractEffect::writeFrameBuffer() const
{
    return m_framebuffers[1 - m_readFrameBuffer];
}

void PAbstractEffect::swapFrameBuffers()
{
    m_readFrameBuffer = 1 - m_readFrameBuffer;
}
    
void PAbstractEffect::render(PRenderState *renderState)
{
    m_readFrameBuffer = 0;
    
    m_renderTarget.use(renderState);
    
    // Copy the current frame buffer into read framebuffer
    readFrameBuffer()->copyFrameBuffer(renderState);
    
    //
    // post-processing algorithm.
    //
    renderEffect(renderState);   

    //
    // Blend the original framebuffer with target buffer.
    //
    renderState->useFrameBuffer(P_NULL);
    // write framebuffer contains the content in the window buffer.
    // read framebuffer contains the content of post processing output.
    writeFrameBuffer()->copyFrameBuffer(renderState); 

    m_material->parameter("source") = readFrameBuffer()->colorBuffer();
    m_material->parameter("target") = writeFrameBuffer()->colorBuffer();
    m_material->apply(renderState);

    m_blendRect->render(renderState);
}
    
void PAbstractEffect::unpack(PXmlElement *xmlElement)
{
    const pchar *blendValue = xmlElement->attribute("blend");
    if (pstrcmp(blendValue, "mix") == 0)
    {
        m_blendMode = P_EFFECTBLEND_MIX;
    }
    else if (pstrcmp(blendValue, "overlay") == 0)
    {
        m_blendMode = P_EFFECTBLEND_OVERLAY;
    }
    else if (pstrcmp(blendValue, "add") == 0)
    {
        m_blendMode = P_EFFECTBLEND_ADD;
    }
}
