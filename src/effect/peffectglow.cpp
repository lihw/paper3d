// peffectglow.cpp
// Full screen glow effect
//
// Copyright 2012 - 2014 Future Interface . 
// This software is licensed under the terms of the MIT license.
//
// Hongwei Li lihw81@gmail.com
//

#include "peffectglow.h"

#include <Paper3D/pscene.h>
#include <Paper3D/pmaterial.h>
#include <Paper3D/pgeometryplane.h>
#include <Paper3D/prenderstate.h>
#include <Paper3D/pmaterialparameter.h>
#include <Paper3D/presourcemanager.h>

#include <PFoundation/pcontext.h>
#include <PFoundation/pconststring.h>
#include <PFoundation/pxmlelement.h>
#include <PFoundation/pglframebuffer.h>


PEffectGlow::PEffectGlow(PScene *scene)
    : PAbstractEffect(scene)
{
     PResourceManager *resourceManager = scene->context()->module<PResourceManager>("resource-manager");

    m_xMaterial = PNEW(PMaterial("glowx.pmt", resourceManager));
    m_xMaterial->retain();
    m_yMaterial = PNEW(PMaterial("glowy.pmt", resourceManager));
    m_yMaterial->retain();
    
    const puint32 *rect = m_scene->context()->rect();
    m_xMaterial->parameter("inversed-viewport") = pVector2(1.0f / pfloat32(rect[2]), 1.0f / pfloat32(rect[3]));
    m_yMaterial->parameter("inversed-viewport") = pVector2(1.0f / pfloat32(rect[2]), 1.0f / pfloat32(rect[3]));

    m_effectRect->calibrate(m_xMaterial->vertexFormat());
}

PEffectGlow::~PEffectGlow()
{
    m_xMaterial->release();
    m_yMaterial->release();
}

void PEffectGlow::setBlurIterations(pint32 number)
{
    m_blurIterations = pMax(number, pint32(1));
}

void PEffectGlow::setBlurKernelSize(pint32 size)
{
    // We choose its nearest odd number to the "size"
    m_blurKernelSize = pMax(size / 2 * 2 + 1, pint32(3));
    
    m_xMaterial->parameter("size") = m_blurKernelSize;
    m_yMaterial->parameter("size") = m_blurKernelSize;
}

void PEffectGlow::setBlurDistance(pint32 step)
{
    m_xMaterial->parameter("step") = step;
    m_yMaterial->parameter("step") = step;

    m_blurDistance = step;
}
    
void PEffectGlow::renderEffect(PRenderState *renderState)
{
    for (pint32 i = 0; i < m_blurIterations; ++i)
    {
        // blur in x direction.
        m_xMaterial->parameter("texture") = readFrameBuffer()->colorBuffer(); 
        m_xMaterial->apply(renderState);
        renderState->useFrameBuffer(writeFrameBuffer());
        PGlFramebuffer::clearFramebuffer(true, false, false);
        m_effectRect->render(renderState);
        swapFrameBuffers();

        // blur in y direction.
        m_yMaterial->parameter("texture") = readFrameBuffer()->colorBuffer(); 
        m_yMaterial->apply(renderState);
        renderState->useFrameBuffer(writeFrameBuffer());
        PGlFramebuffer::clearFramebuffer(true, false, false);
        m_effectRect->render(renderState);
        swapFrameBuffers();
    }
}
    
void PEffectGlow::unpack(PXmlElement *xmlElement)
{
    PASSERT(xmlElement->isValid());

    PAbstractEffect::unpack(xmlElement);

    const pchar *blurIterationsValue = xmlElement->attribute("bluriterations");
    if (blurIterationsValue != P_NULL)
    {
        pStringUnpackInt(blurIterationsValue, &m_blurIterations);
    }
    const pchar *blurKernelSizeValue = xmlElement->attribute("blurkernelsize");
    if (blurKernelSizeValue != P_NULL)
    {
        pStringUnpackInt(blurKernelSizeValue, &m_blurKernelSize);
    }
    const pchar *blurDistanceValue = xmlElement->attribute("blurdistance");
    if (blurDistanceValue != P_NULL)
    {
        pStringUnpackInt(blurDistanceValue, &m_blurDistance);
    }
}
    
