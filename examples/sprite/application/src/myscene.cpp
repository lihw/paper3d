// myscene.cpp
// "Sprite" example 
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
    m_nextSprite = 0;

    PResourceManager *resMgr = context->module<PResourceManager>("resource-manager");

    const puint32 *rect = context->rect();

    PCamera *camera = PNEW(PCamera("camera", this));
    camera->setFixed(true);
    camera->transform().setLookAt(0.0f, 10.0f, 7.0f, 0.0f, 5.0f, 0.0f, 0.0f, 1.0f, 0.0f);
    camera->projection().perspective(60.0f, (pfloat32)rect[2] / (pfloat32)rect[3], 0.1f, 100.0f);
    setMainCamera(camera);
    
    m_plane = PNEW(PDrawable("plane", this));
    m_plane->setGeometry(PNEW(PGeometryPlane(resMgr)));
    m_plane->setMaterial(PNEW(PMaterial("color.pmt", resMgr)));
    m_plane->material()->parameter("color") = P_COLOR_RED;

    PTexture* textures[1];
    textures[0] = resMgr->getTexture("texture.png");

    pchar name[] = "sprite0";
    for (puint32 i = 0; i < NUM_SPRITES; ++i)
    {
        name[6]++;        
        m_sprites[i] = PNEW(PSprite(name, this));
        m_sprites[i]->setAnimationDuration(4, 2000);
        m_sprites[i]->setTextureAtlas(textures, 1, 2, 2, true);

        m_sprites[i]->transform().setScaling(pVector3(0.25f, 0.25f, 1.0f));
        m_sprites[i]->transform().setTranslation(pVector3(-100000.0f, -100000.0f, -100000.0f));
    }

    pfloat32 aspect = (pfloat32)rect[2] / (pfloat32)rect[3];
    PSprite2D *legend = PNEW(PSprite2D("legend", this));
    legend->transform().setScaling(pVector3(aspect * 0.25f, aspect * 0.0625f, 1.0f));
    textures[0] = resMgr->getTexture("legend.png");
    legend->setTextureAtlas(textures, 1, 1, 1, true);
    legend->setAnimationDuration(1, 10000);
    legend->transform().setTranslation(pVector3(0, 0.4f, -1.0f));
}

MyScene::~MyScene()
{
}
    
void MyScene::update()
{
    const pfloat32 GRAVITY = -9.8f;

    pfloat32 dt = context()->clock().deltaTime() * 0.001f;

    for (puint32 i = 0; i < NUM_SPRITES; ++i)
    {
        PVector3 pos = m_sprites[i]->transform().translation();
        pos += m_velocity[i] * dt;
        m_sprites[i]->transform().setTranslation(pos);

        m_velocity[i][1] += GRAVITY * dt;
    }

	PVector3 r = m_plane->transform().rotation();
	r[2] -= dt * 1.0f;
	m_plane->transform().setRotation(r);
}

void MyScene::fire()
{
    PSprite* sprite = m_sprites[m_nextSprite];
    sprite->transform().setTranslation(pVector3(0, 0, 0));

    
    pfloat32 theta = P_PI * 2.0f * m_random.getFloat32();
    pfloat32 phi = 0.48f * P_PI;
    pfloat32 speed = m_random.getFloat32() * 5.0f + 10.0f;

    pfloat32 y = sinf(phi);
    pfloat32 x = cosf(phi) * sinf(theta);
    pfloat32 z = cosf(phi) * cosf(theta);

    m_velocity[m_nextSprite] = pVector3(x, y, z) * speed;

    m_nextSprite = (m_nextSprite + 1) % NUM_SPRITES;
}

