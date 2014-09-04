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
}


MyScene::~MyScene()
{
}

void MyScene::update()
{
    if (!m_rotating)
    {
 	    PVector3 r = m_drawable->transform().rotation();
	    r[0] -= 0.01f;
        r[1] -= 0.005f;
	    m_drawable->transform().setRotation(r);   
    }
}

void MyScene::setRotating(pbool rotating)
{
    m_rotating = rotating;
}

void MyScene::rotate(const PMatrix3x3 &matrix)
{
    PVector3 position = m_drawable->transform().translation();

    pfloat32 m[16] = 
    {
        matrix.m_m[0],
        matrix.m_m[1],
        matrix.m_m[2],
        0,

        matrix.m_m[3],
        matrix.m_m[4],
        matrix.m_m[5],
        0.0f,
        
        matrix.m_m[6],
        matrix.m_m[7],
        matrix.m_m[8],
        0.0f,

        position[0],
        position[1],
        position[2],
        1.0f,
    };

    m_drawable->transform().setMatrix4x4(m);
}

