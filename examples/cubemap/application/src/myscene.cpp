// myscene.h
// "Cubemap" example. 
//
// Copyright 2012 - 2014 Future Interface. 
// This software is licensed under the terms of the MIT license.
//
// Hongwei Li lihw81@gmail.com
//

#include "myscene.h"

#include <Paper3D/paper3d.h>

#include <PFoundation/pfoundation.h>


MyScene::MyScene(PContext *context)
    : PScene("my-scene", context)
{
    const puint32 *rect = context->rect();
    
    // -------------------------------------------------------------- 
    // Add camera
    // -------------------------------------------------------------- 
    PCamera *camera = PNEW(PCamera("main-camera", this));
    camera->setFixed(true);
    camera->transform().setLookAt(0.0f, 0.0f, 5.0f, 0.0f, 0.0f, 0.0f, 0.0f, 1.0f, 0.0f);
    camera->projection().perspective(60.0f, (pfloat32)rect[2] / (pfloat32)rect[3], 0.1f, 100.0f);
    setMainCamera(camera);

    PResourceManager *resMgr = context->module<PResourceManager>("resource-manager");

    m_teapot = PNEW(PDrawable("teapot", this));
    PAbstractGeometry *geometry = PNEW(PGeometryMesh("teapot.pmh", resMgr));
    PMaterial *material = PNEW(PMaterial("teapot.pmt", resMgr));
    material->parameter("envmap") = resMgr->getTexture("skybox/*.png");
    m_teapot->setGeometry(geometry);
    m_teapot->setMaterial(material);

    m_skybox = PNEW(PSkyBox("skybox", this));
    m_skybox->setTexture(resMgr->getTexture("skybox/*.png"));
}

MyScene::~MyScene()
{
}
    
void MyScene::update()
{
    // Rotate the teapot
    pfloat32 dt = 0.1f * context()->clock().deltaTime();

    PVector3 rot;

    rot = m_teapot->transform().rotation();
    rot[0] += 0.005f * dt;
    rot[1] += 0.001f * dt;
    rot[2] += 0.002f * dt;
    m_teapot->transform().setRotation(rot);

    // Rotate the scene
    rot = root()->transform().rotation();
    rot[1] += 0.0005f * dt;
    root()->transform().setRotation(rot);
}
