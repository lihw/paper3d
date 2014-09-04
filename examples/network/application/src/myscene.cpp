// myscene.cpp
// "Network" example
//
// Copyright 2012 - 2014 Future Interface. All rights reserved.
//
// Future Interface  lihw81@gmail.com

#include "myscene.h"

#include <Paper3D/paper3d.h>

#include <PFoundation/pfoundation.h>


MyScene::MyScene(PContext *context)
    : PScene("my-scene", context)
{
    PResourceManager *resMgr = context->module<PResourceManager>("resource-manager");
    
    const puint32 *rect = context->rect();

    pfloat32 aspect = (pfloat32)rect[2] / (pfloat32)rect[3];

    PCamera *camera = PNEW(PCamera("camera", this));
    camera->setFixed(true);
    camera->transform().setLookAt(0.0f, 4.0f, 10.0f, 0.0f, 0.0f, 0.0f, 0.0f, 1.0f, 0.0f);
    camera->projection().perspective(45.0f, (pfloat32)rect[2] / (pfloat32)rect[3], 0.1f, 100.0f);
    setMainCamera(camera);

    PTexture *textures[4];

    textures[0] = resMgr->getTexture("texture/tip.png");
    textures[1] = resMgr->getTexture("texture/object.png");

    m_type = PNEW(PSprite2D("type", this));
    m_type->transform().setScaling(pVector3(aspect * 0.5f, aspect * 0.125f, 1.0f));
    m_type->setTextureAtlas(&textures[0], 1, 1, 1, true);
    m_type->setAnimationDuration(1, 10000);
    m_type->transform().setTranslation(pVector3(0, 0.5f, -1.0f));

    m_object = PNEW(PSprite2D("object", this));
    m_object->transform().setScaling(pVector3(aspect * 0.1f, aspect * 0.1f, 1.0f));
    m_object->setTextureAtlas(&textures[1], 1, 1, 1, true);
    m_object->setAnimationDuration(1, 10000);
    m_object->transform().setTranslation(pVector3(0, 0, -1.0f));
    m_object->setVisible(false);

    setBackgroundColor(pColorRGBA(1.0f, 1.0f, 1.0f, 0.0f));
}

MyScene::~MyScene()
{
}
    
void MyScene::update()
{
}
    
void MyScene::setupServerScene()
{
    PResourceManager *resMgr = context()->module<PResourceManager>("resource-manager");
    PTexture *texture = resMgr->getTexture("texture/server.png");
    m_type->setTextureAtlas(&texture, 1, 1, 1, true);

    m_object->setVisible(true);
}

void MyScene::setupClientScene()
{
    PResourceManager *resMgr = context()->module<PResourceManager>("resource-manager");
    PTexture *texture = resMgr->getTexture("texture/client.png");
    m_type->setTextureAtlas(&texture, 1, 1, 1, true);
}
    
void MyScene::moveObject(pint32 dx, pint32 dy)
{
    const puint32 *r = context()->rect();

    pfloat32 dxf = 2.0f * (pfloat32)dx / (pfloat32)r[2];
    pfloat32 dyf = -2.0f * (pfloat32)dy / (pfloat32)r[3];
    
    PVector3 pos = m_object->transform().translation();

    pos[0] += dxf;
    pos[1] += dyf;

    m_object->transform().setTranslation(pos);
}
