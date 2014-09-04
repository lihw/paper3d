// prenderqueue.cpp
// The queue in default render pass of forward rendering.
//
// Copyright 2012 - 2014 Future Interface. 
// This software is licensed under the terms of the MIT license.
//
// Hongwei Li lihw81@gmail.com
//

#include "prenderqueue.h"

#include <Paper3D/pcamera.h>
#include <Paper3D/pmaterial.h>
#include <Paper3D/pdrawable.h>
#include <Paper3D/pabstractlight.h>
#include <Paper3D/pscene.h>
#include <Paper3D/pabstractgeometry.h>
#include <Paper3D/prenderstate.h>
#include <Paper3D/pskybox.h>
#include <Paper3D/pbackground.h>

#include <PFoundation/pglstate.h>


PRenderQueue::PRenderQueue()
    : m_transparents(true)
    , m_opaques(false)
    , m_zSorting(true)
{
}

PRenderQueue::~PRenderQueue()
{
}
    

void PRenderQueue::addNode(PNode *node)
{
    if (node->isDrawable())
    {
        PDrawable *drawable = reinterpret_cast<PDrawable *>(node);
        PASSERT(drawable != P_NULL);

        if (drawable->material())
        {
            if (drawable->material()->isTransparent())
            {
                m_transparents.addDrawable(drawable);
            }
            else 
            {
                m_opaques.addDrawable(drawable);
            }
        }
    }
    else
    {
        PAbstractLight *light = dynamic_cast<PAbstractLight *>(node);
        if (light != P_NULL)
        {
            m_lights.addLight(light);
        }
    }
}
    
void PRenderQueue::addNodes(PNode *node)
{
    PNode::iterator ni(node);

    PNode *n = *ni; 

    // Skip the root
    if (n->parent() == P_NULL)
    {
        n = *(++ni);
    }

    while (n != P_NULL)
    {
        if (n->isDrawable())
        {
            PDrawable *drawable = reinterpret_cast<PDrawable *>(n);
            PASSERT(drawable != P_NULL);

            if (drawable->material())
            {
                if (drawable->material()->isTransparent())
                {
                    m_transparents.addDrawable(drawable);
                }
                else 
                {
                    m_opaques.addDrawable(drawable);
                }
            }
        }
        else
        {
            PAbstractLight *light = dynamic_cast<PAbstractLight *>(n);
            if (light != P_NULL)
            {
                m_lights.addLight(light);
            }
        }

        n = *(++ni);
    }
}

void PRenderQueue::removeNodes(PNode *node)
{
    PNode::iterator ni(node);

    PNode *n = *ni;
    // Skip the root node if it is
    if (n->parent() == P_NULL)
    {
        n = *(++ni);
    }

    while (n != P_NULL)
    {
        if (n->isDrawable())
        {
            PDrawable *drawable = reinterpret_cast<PDrawable *>(n);
            PASSERT(drawable != P_NULL);

            if (drawable->material())
            {
                if (drawable->material()->isTransparent())
                {
                    m_transparents.removeDrawable(drawable);
                }
                else 
                {
                    m_opaques.removeDrawable(drawable);
                }
            }
        }
        else
        {
            PAbstractLight *light = dynamic_cast<PAbstractLight *>(n);
            if (light != P_NULL)
            {
                m_lights.removeLight(light);
            }
        }

        n = *(++ni);
    }
}
    
void PRenderQueue::render(PScene *scene, PCamera *camera, PRenderState *renderState)
{
    if (m_opaques.count() == 0 && m_transparents.count() == 0)
    {
        return ;
    }

    PGlState *rso = renderState->renderStateObject();

    // Render opaques first
    rso->setBlend(P_GLBLEND_OPAQUE);
    for (puint32 i = 0; i < m_opaques.count(); ++i) 
    {
        doRender(renderState, 
                 camera, 
                 m_opaques[i], 
                 m_opaques[i]->material(), 
                 m_lights);
    }

    // Render transparent next
    if (m_transparents.count() != 0)
    {
        if (m_zSorting)
        {
            m_transparents.sort(camera);
        }

        // Do the rendering.
        rso->setBlend(P_GLBLEND_ALPHA);
        for (puint32 i = 0; i < m_transparents.count(); ++i) 
        {
            doRender(renderState, 
                     camera, 
                     m_transparents[i], 
                     m_transparents[i]->material(), 
                     m_lights);
        }
    }
}
    
void PRenderQueue::doRender(PRenderState            *renderState,
                            PCamera                 *camera,
                            PDrawable               *drawable,
                            PMaterial               *material,
                            const PRenderLightQueue &lights)
{
    if (drawable->isVisible())
    {
        drawable->prepareRender(renderState);

        material->apply(renderState, drawable, camera, lights); 
        drawable->geometry()->render(renderState);

        drawable->postRender();
    }
}

void PRenderQueue::setZSortingEnabled(pbool enabled)
{
    m_zSorting = enabled;
}
    
