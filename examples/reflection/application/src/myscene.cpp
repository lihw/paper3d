// myscene.cpp
// "Reflection" example
//
// Copyright 2012 - 2014 Future Interface. 
// This software is licensed under the terms of the MIT license.
//
// Hongwei Li lihw81@gmail.com

#include "myscene.h"

#include <Paper3D/paper3d.h>

#include <PFoundation/pfoundation.h>

#include "pmirror.h"

MyScene::MyScene(PContext *context)
    : PScene("my-scene", context)
{
    PResourceManager *resMgr = context->module<PResourceManager>("resource-manager");

    const puint32 *rect = context->rect();
    
    // -------------------------------------------------------------- 
    // Add camera
    // -------------------------------------------------------------- 
    PCamera *camera = PNEW(PCamera("main-camera", this));
    camera->setFixed(true);
    camera->transform().setLookAt(0.0f, 2.0f, 10.0f, 0.0f, 0.0f, 0.0f, 0.0f, 1.0f, 0.0f);
    camera->projection().perspective(45.0f, (pfloat32)rect[2] / (pfloat32)rect[3], 0.1f, 100.0f);
    setMainCamera(camera);

    PMaterial *material;
    PAbstractGeometry *geometry;

    geometry = PNEW(PGeometryCube(resMgr));
    m_cube = PNEW(PDrawable("cube", this));
    material = PNEW(PMaterial("texture.pmt", resMgr));
    material->parameter("texture") = resMgr->getTexture("texture.png");
    m_cube->setGeometry(geometry);
    m_cube->setMaterial(material);

    m_mirror = PNEW(PMirror("mirror", this));
    m_mirror->transform().setScaling(pVector3(8.0f, 8.0f, 8.0f));
    m_mirror->transform().setTranslation(pVector3(0.0f, -1.0f, 0.0f));
    m_mirror->transform().setRotation(pVector3(-P_PI * 0.5f, 0.0f, 0.0f));
}

MyScene::~MyScene()
{
}
    
void MyScene::update()
{
    pfloat32 dt = 0.1f * context()->clock().deltaTime();

    PVector3 rot;

    rot = m_cube->transform().rotation();
    rot[0] += 0.005f * dt;
    rot[1] += 0.001f * dt;
    rot[2] += 0.002f * dt;
    m_cube->transform().setRotation(rot);
}
