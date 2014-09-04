// myscene.cpp
// "Multipass" example.
//
// Copyright 2012 - 2014 Future Interface. 
// This software is licensed under the terms of the MIT license.
//
// Hongwei Li lihw81@gmail.com

#include "myscene.h"

#include <Paper3D/paper3d.h>

#include <PFoundation/pfoundation.h>


MyScene::MyScene(PContext *context)
    : PScene("my-scene", context)
{
    m_useSinglePass = true;

    PResourceManager *resMgr = context->module<PResourceManager>("resource-manager");

    const puint32 *rect = context->rect();

    PCamera *camera = PNEW(PCamera("camera", this));
    camera->setFixed(true);
    camera->transform().setLookAt(0.0f, 4.0f, 10.0f, 0.0f, 0.0f, 0.0f, 0.0f, 1.0f, 0.0f);
    camera->projection().perspective(45.0f, (pfloat32)rect[2] / (pfloat32)rect[3], 0.1f, 100.0f);
    setMainCamera(camera);

    PDrawable *ground = PNEW(PDrawable("ground", this));
    ground->setGeometry(PNEW(PGeometryPlane(resMgr)));
    ground->setMaterial(PNEW(PMaterial("material/texture.pmt", resMgr)));
    ground->material()->parameter("texture") = resMgr->getTexture("texture/ground.png");
    ground->transform().setScaling(pVector3(3.0f, 3.0f, 1.0f));
    ground->transform().setRotation(pVector3(P_PI * -0.5f, 0.0f, 0.0f));
    ground->material()->setTransparent(true);

    PMaterial *grassMaterial = PNEW(PMaterial("material/texture.pmt", resMgr));
    PTexture *grassTexture   = resMgr->getTexture("texture/grass.png");
    PAbstractGeometry *grassGeometry = PNEW(PGeometryPlane(resMgr));
        
    grassMaterial->parameter("texture") = grassTexture;

    pfloat32 theta = 0;
    PDrawable *grass;
    for (puint32 i = 0; i < 6; ++i)
    {
        pchar name[] = "grass1";
        name[5] = '0' + i;
        grass = PNEW(PDrawable(name, this));
        grass->setGeometry(grassGeometry);
        grass->setMaterial(grassMaterial);
        grass->material()->setTransparent(true);
        grass->geometry()->setCullMode(P_GLCULL_NONE);

        PVector3 t = grass->transform().translation();
        t[2] = (pfloat32)i * 0.5f - 1.4f;
        grass->transform().setTranslation(t);
    } 

    // Single render pass
    m_singlePass = PNEW(PRenderPass("single", this));
    m_singlePass->setCamera(mainCamera());
    m_singlePass->retain();
    m_singlePass->queue()->addNodes(root());

    // Two render passes
    m_twoPass0 = PNEW(PRenderPass("two0", this));
    m_twoPass1 = PNEW(PRenderPass("two1", this));
    m_twoPass0->retain();
    m_twoPass1->retain();
    
    m_twoPass0->setCamera(mainCamera());
    m_twoPass0->queue()->addNodes(ground);

    m_twoPass1->setCamera(mainCamera());
    m_twoPass1->queue()->addNodes(root());
    m_twoPass1->queue()->removeNodes(ground);
    m_twoPass1->target()->setBufferClearEnabled(false, false, false);

    // By default, we use single render pass.
    renderer()->addRenderPass(m_singlePass);
}

MyScene::~MyScene()
{
    m_twoPass0->release();
    m_twoPass1->release();
    m_singlePass->release();
}
    
void MyScene::update()
{
    PVector3 r = root()->transform().rotation();
    r[1] -= 0.01f;
    root()->transform().setRotation(r);
}
    
void MyScene::switchRenderPass()
{
    m_useSinglePass = !m_useSinglePass;
    if (m_useSinglePass)
    {
        // In the renderer, we sort transparent objects based on the center
        // of their bounding box to the camera. Some times, the center of 
        // bounding box doesn't precisely present the position of object
        // in the space which cause the sorting result useless. For example,
        // the disc in this scene. In this case, we'd better separate these
        // objects into different render passes. That's why using two render
        // passes in this scene gives correct result.
        renderer()->removeRenderPass("two0");
        renderer()->removeRenderPass("two1");
        renderer()->addRenderPass(m_singlePass);
    
        PLOG_INFO("Use single render pass.");
    }
    else
    {
        renderer()->removeRenderPass("single");
        renderer()->addRenderPass(m_twoPass0);
        renderer()->addRenderPass(m_twoPass1);
        
        PLOG_INFO("Use two render passes.");
    }

}
