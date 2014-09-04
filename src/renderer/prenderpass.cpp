// prenderpass.cpp
// The rendering pass
//
// Copyright 2012 - 2014 Future Interface . 
// This software is licensed under the terms of the MIT license.
//
// Hongwei Li lihw81@gmail.com and Hongwei Li lihw81@gmail.com
//

#include "prenderpass.h"

#include <Paper3D/pmaterial.h>
#include <Paper3D/pcamera.h>
#include <Paper3D/prendertarget.h>
#include <Paper3D/prenderqueue.h>
#include <Paper3D/pscene.h>
#include <Paper3D/pmaterial.h>

#include <PFoundation/pcontext.h>


PRenderPass::PRenderPass(const pchar *name, PScene *scene)
    : PEntity()
    , m_name(name)
{
    PASSERT(scene != P_NULL);
    m_scene             = scene;
    m_camera            = P_NULL;
    m_renderTarget      = P_NULL;
    m_renderQueue       = P_NULL;
    m_overridedMaterial = P_NULL;
    
    PRenderTarget *renderTarget = PNEW(PRenderTarget);

    renderTarget->setViewport(m_scene->context()->rect()[0],
                              m_scene->context()->rect()[1],
                              m_scene->context()->rect()[2],
                              m_scene->context()->rect()[3]);

    setRenderTarget(renderTarget);

    PRenderQueue *renderQueue = PNEW(PRenderQueue);
    setRenderQueue(renderQueue);
}

PRenderPass::~PRenderPass()
{
    PDELETE(m_renderQueue);
    PDELETE(m_renderTarget);
    if (m_overridedMaterial != P_NULL)
    {
        m_overridedMaterial->release();
    }
}

void PRenderPass::setCamera(PCamera *camera)
{
    PASSERT(camera != P_NULL);
    if (m_camera != camera)
    {
        m_camera = camera; 
    }
}

void PRenderPass::setRenderTarget(PRenderTarget *renderTarget)
{
    PASSERT(renderTarget != P_NULL);
    PDELETE(m_renderTarget);
    if (m_renderTarget != renderTarget)
    {
        m_renderTarget = renderTarget;
    }
}

void PRenderPass::setRenderQueue(PRenderQueue *renderQueue)
{
    PASSERT(renderQueue != P_NULL);
    if (m_renderQueue != renderQueue)
    {
        PDELETE(m_renderQueue);
        m_renderQueue = renderQueue;
    }
}

void PRenderPass::setOverridedMaterial(PMaterial *material)
{
    if (m_overridedMaterial != P_NULL)
    {
        m_overridedMaterial->release();
    }
    m_overridedMaterial = material;
    if (m_overridedMaterial != P_NULL)
    {
        m_overridedMaterial->retain();
    }
}

void PRenderPass::render(PRenderState *renderState)
{
    PASSERT(renderState != P_NULL);
    
    if (m_camera == P_NULL)
    {
        PLOG_WARNING("Render pass %s doesn't have a camera.", m_name.c_str());
        return ;
    }

    m_renderTarget->use(renderState);
    
    m_renderQueue->render(m_scene, m_camera, renderState);
}
    
