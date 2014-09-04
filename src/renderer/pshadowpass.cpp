// pshadowpass.cpp
// Render pass that generates shadow map
//
// Copyright 2012 - 2014 Future Interface . 
// This software is licensed under the terms of the MIT license.
//
// Hongwei Li lihw81@gmail.com
//

#include "pshadowpass.h"


#include <Paper3D/pframebuffer.h>
#include <Paper3D/presourcemanager.h>
#include <Paper3D/pscene.h>
#include <Paper3D/pmaterial.h>
#include <Paper3D/pdrawable.h>
#include <Paper3D/prenderqueue.h>
#include <Paper3D/pdirectionallight.h>
#include <Paper3D/pcamera.h>
#include <Paper3D/prendertarget.h>

#include <PFoundation/pcontext.h>

    
PShadowPass::PShadowPass(const pchar *name, PScene *scene)
    : PRenderPass(name, scene)
{
    PResourceManager *resMgr = m_scene->context()->module<PResourceManager>("resource-manager");
    
    const puint32 *r = m_scene->context()->rect();

    puint32 bufferWidth;
    puint32 bufferHeight;
    switch (scene->shadowQuality())
    {
        case PScene::SHADOWQUALITY_HIGH:
            bufferWidth  = r[2];
            bufferHeight = r[3];
            break;
        case PScene::SHADOWQUALITY_NORMAL:
            bufferWidth  = r[2] / 2;
            bufferHeight = r[3] / 2;
            break;
        case PScene::SHADOWQUALITY_LOW:
            bufferWidth  = r[2] / 4;
            bufferHeight = r[3] / 4;
            break;
    } 

    m_framebuffer = P_NULL;

    PNode::iterator ni(m_scene->root());

    PNode *n = *ni; 

    n = *(++ni);

    puint32 numDrawables = 0;

    while (n != P_NULL)
    {
        // Put opaque drawables which cast shadows into the render queue.
        if (n->isDrawable())
        {
            PDrawable *drawable = reinterpret_cast<PDrawable *>(n);
            PASSERT(drawable != P_NULL);

            if (drawable->castShadow())
            {
                PMaterial *material = drawable->material();
                if ((material != P_NULL && !drawable->material()->isTransparent()) ||
                    material == P_NULL)
                {
                    m_renderQueue->addNode(drawable);
                    numDrawables++;
                }
            }
        }
        else
        {
            // The first directional light that cast shadow will be the 
            // shadow light source.
            PAbstractLight *light = dynamic_cast<PAbstractLight *>(n);
            if (light != P_NULL && 
                light->type() == P_LIGHT_DIRECTIONAL)
            {
                if (((PDirectionalLight *)light)->castShadow())
                {
                    m_camera = PNEW(PCamera("shadow-camera", m_scene));
                    m_camera->fromLight((PDirectionalLight *)light, scene->bbox());
                    m_camera->setFixed(true);
                }
            }
        }

        n = *(++ni);
    }

    if (m_camera != P_NULL && numDrawables)
    {
        m_framebuffer = resMgr->createFrameBuffer(P_NULL,
                                                  bufferWidth,
                                                  bufferHeight,
                                                  P_GLTEXTURE_FORMAT_RGBA8888,
                                                  P_GLTEXTURE_FORMAT_DEPTH_COMPONENT16);
    }

    if (numDrawables == 0)
    {
        PDELETE(m_camera);
    }
}

PShadowPass::~PShadowPass()
{
    if (m_framebuffer != P_NULL)
    {
        m_framebuffer->release();
    }
}

void PShadowPass::render(PRenderState *renderState)
{
    PASSERT(renderState != P_NULL);
    
    if (m_camera != P_NULL)
    {
        m_renderTarget->use(renderState);
        
        m_renderQueue->render(m_scene, m_camera, renderState);
    }
}
    

