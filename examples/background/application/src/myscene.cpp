// myscene.h
// "Background" example. 
//
// Copyright 2012 - 2014 Future Interface. 
// This software is licensed under the terms of the MIT license.
//
// Hongwei Li lihw81@gmail.com
//

#include "myscene.h"

#include <Paper3D/paper3d.h>

#include <PFoundation/pfoundation.h>
#include <PFoundation/pcontext.h>


MyScene::MyScene(PContext *context)
    : PScene("my-scene", context)
{
    m_backgroundMovement = 0;

    const puint32 *rect = context->rect();

    PResourceManager *resMgr = context->module<PResourceManager>("resource-manager");
    
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

    m_background = PNEW(PBackground("bg", this));
    PTexture *texture = resMgr->getTexture("bg.png");
    texture->setRepeatWrappingEnabled(true);
    m_background->setTexture(texture);
    pfloat32 height = (pfloat32)texture->height() / (pfloat32)rect[3];
    m_background->setSize(1.0f, height * 2.0f);
    m_background->setLayout(PBackground::LAYOUT_CENTER | PBackground::LAYOUT_TOP);

    setBackgroundColor(pColorRGBA(1.0f, 1.0f, 1.0f, 0.0f));
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

void MyScene::rotate(pfloat32 offset)
{
    m_backgroundMovement += offset;
    m_background->setTextureOffset(m_backgroundMovement, 0);
}

void MyScene::switchBackgroundFillMode()
{
    static puint32 mode = PBackground::FILL_DEFAULT;

    mode = (mode + 1) % (PBackground::FILL_MODE_COUNT + 1);

    m_background->setTextureFillMode((PBackground::FillModeEnum)mode);
}
