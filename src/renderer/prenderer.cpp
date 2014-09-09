// prenderer.cpp
// Classic forward rendering. In this renderer, we only support
// shadow casted by one directional light. The renderer first
// execute a shadow map generation pass if exists, which follows
// an forward render pass and finally executes a list of post
// processing passes.
//
// Copyright 2012 - 2014 Future Interface . 
// This software is licensed under the terms of the MIT license.
//
// Hongwei Li lihw81@gmail.com
//

#include "prenderer.h"

#include <Paper3D/pscene.h>
#include <Paper3D/prenderpass.h>
#include <Paper3D/prendertarget.h>
#include <Paper3D/prenderqueue.h>
#include <Paper3D/pscene.h>
#include <Paper3D/pshadowpass.h>

#include <PFoundation/passert.h>
#include <PFoundation/pcontext.h>


PRenderer::PRenderer(PScene *scene)
{
    PASSERT(scene != P_NULL);
    m_scene = scene;

    m_backgroundColor = P_COLOR_BLACK;
}

PRenderer::~PRenderer()
{
    for (puint32 i = 0; i < m_renderPassArray.size(); ++i)
    {
        m_renderPassArray[i]->release();
    }
}

void PRenderer::render(PRenderState *renderState)
{
    PASSERT(m_scene->mainCamera() != P_NULL);

    // Create a default render pass if none exists
    if (m_renderPassArray.isEmpty())
    {
        PRenderPass *renderPass = PNEW(PRenderPass("default", m_scene));
        renderPass->setCamera(m_scene->mainCamera());
        renderPass->queue()->addNodes(m_scene->root());
        renderPass->target()->setColorClearValue(m_backgroundColor);

        PShadowPass *shadowPass = PNEW(PShadowPass("shadow", m_scene));
        
        addRenderPass(shadowPass);
        addRenderPass(renderPass);
    }

    if (m_renderPassArray.isEmpty())
    {
        PLOG_WARNING("There is no valid render pass existing.");
        return ;
    }

    for (puint32 i = 0; i < m_renderPassArray.size(); ++i)
    {
        m_renderPassArray[i]->render(renderState);
    }
}
    
void PRenderer::addRenderPass(PRenderPass *pass)
{
    // TODO: check if a render pass with the same name exists already.
    m_renderPassArray.append(pass);
    pass->retain();
}

void PRenderer::removeRenderPass(const pchar *name)
{
    // Remove the all render passes with the give name.
    for (puint32 i = 0; i < m_renderPassArray.size(); ++i)
    {
        if (m_renderPassArray[i]->name() == name)
        {
            m_renderPassArray[i]->release();
            m_renderPassArray[i] = P_NULL;
        }
    }

    puint32 numDeleted = 0;
    puint32 n = m_renderPassArray.size();
    for (puint32 i = 0; i < n; ++i)
    {
        if (m_renderPassArray[i] == P_NULL)
        {
            for (puint32 j = i; j < n - 1; ++j)
            {
                m_renderPassArray[j] = m_renderPassArray[j + 1];
            }
            numDeleted++;
        }
    }

    m_renderPassArray.resize(n - numDeleted);
}
    
void PRenderer::setBackgroundColor(const PColorRGBA &color)
{
    m_backgroundColor = color;
}

const PColorRGBA &PRenderer::backgroundColor() const
{
    return m_backgroundColor;
}

PRenderPass *PRenderer::renderPass(const pchar *name) const
{
    puint32 n = m_renderPassArray.count();
    for (puint32 i = 0; i < n; ++i)
    {
        if (m_renderPassArray[i]->name() == name)
        {
            return m_renderPassArray[i];
        }
    }

    PLOG_WARNING("Failed to find render pass %s in renderer.", name);
    return P_NULL;
}
