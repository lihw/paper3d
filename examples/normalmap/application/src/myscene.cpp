// myscene.cpp
// "NormalMap" example
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

    if (!load("scene.psc")) 
    {
        PASSERT(!"Failed to load scene.psc");
        return ;
    }

    const puint32 *rect = context->rect();

    m_drawable = (PDrawable *)(root()->child("brain"));
    m_drawable->material()->parameter("shininess") = 80.0f;

    m_light = PNEW(PDirectionalLight("directional", root()));
    m_light->setDirection(-1.0f, -1.0f, -1.0f);
    m_light->setColor(pColorRGBA(0.3f, 0.3f, 0.3f, 0.5f));
    m_light->setIntensity(1.0f);
    
    PCamera *camera;

    camera = PNEW(PCamera("camera", this));
    camera->setFixed(true);
    camera->transform().setLookAt(0.0f, 1.9f, 80.0f, 0.0f, 0.0f, 0.0f, 0.0f, 1.0f, 0.0f);
    camera->projection().perspective(45.0f, (pfloat32)rect[2] / (pfloat32)rect[3], 0.1f, 100.0f);
    setMainCamera(camera);

    m_rotating = false;

    m_rotation = pVector3(0, 0, 0);
}


MyScene::~MyScene()
{
}

void MyScene::update()
{   
    if (!m_rotating)
    {
 	    PVector3 r = m_drawable->transform().rotation();
	    r[0] -= 0.001f;
        r[1] -= 0.0005f;
	    m_drawable->transform().setRotation(r); 
    }
}

void MyScene::setRotating(pbool rotating)
{
    m_rotating = rotating;
    m_rotation = m_drawable->transform().rotation();
}

void MyScene::rotate(pfloat32 rx, pfloat32 ry, pfloat32 rz)
{
    if (m_rotating)
    {
        m_drawable->transform().setRotation(pVector3(rx, ry, rz) + m_rotation);
    }
}
