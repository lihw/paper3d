// myscene.cpp
// "HelloPaper3D" example
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
    PRandom random(1001);

    const puint32 *rect = context->rect();

    for (puint32 i = 0; i < NUM_LOGOS; ++i)
    {
        m_velocities[i][0] = (random.getFloat32() - 0.5f) * (pfloat32)rect[2] * 0.001f; 
        m_velocities[i][1] = (random.getFloat32() - 0.5f) * (pfloat32)rect[3] * 0.001f; 
    }

    // -------------------------------------------------------------- 
    // Add camera
    // -------------------------------------------------------------- 
    PCamera *camera = PNEW(PCamera("camera", this));
    camera->setFixed(true);
    camera->transform().setLookAt(0.0f, 0.0f, 10.0f, 0.0f, 0.0f, 0.0f, 0.0f, 1.0f, 0.0f);
    camera->projection().window((pfloat32)rect[2], (pfloat32)rect[3]);
    setMainCamera(camera);
    
	PResourceManager* resMgr = context->module<PResourceManager>("resource-manager");

    // -------------------------------------------------------------- 
    // Scene objects
    // -------------------------------------------------------------- 
    PMaterial *material = PNEW(PMaterial("texture.pmt", resMgr));
    material->parameter("texture") = resMgr->getTexture("texture.png");
    material->setTransparent(true);
        
    PAbstractGeometry *geometry = PNEW(PGeometryPlane(resMgr));

    for (puint32 i = 0; i < NUM_LOGOS; ++i)
    {
        PString name = PString::fromUint(i);
        m_logos[i] = PNEW(PDrawable(name.c_str(), this));
        m_logos[i]->setGeometry(geometry);
        m_logos[i]->setMaterial(material);

        m_logos[i]->transform().setTranslation(pVector3(0.0f, 0.0f, 0.0f));
        m_logos[i]->transform().setScaling(pVector3(64.0f, 16.0f, 1.0f));
    }

    setBackgroundColor(pColorRGBA(1.0f, 1.0f, 1.0f, 0.0f));
}

MyScene::~MyScene()
{
}
    
void MyScene::update()
{
    const puint32 *rect = context()->rect();
    pfloat32 dt = context()->clock().deltaTime();

    // Update logo position.
    for (puint32 i = 0; i < NUM_LOGOS; ++i)
    {
        PVector3 position = m_logos[i]->transform().translation();
        
        position[0] += m_velocities[i][0] * dt;
        position[1] += m_velocities[i][1] * dt;

        if (position[0] < 0)
        {
            position[0] = 0.0f;
            m_velocities[i][0] = -m_velocities[i][0];
        }
        if (position[0] > (pfloat32)rect[2])
        {
            position[0] = (pfloat32)rect[2];
            m_velocities[i][0] = -m_velocities[i][0];
        }
        if (position[1] < 0)
        {
            position[1] = 0.0f;
            m_velocities[i][1] = -m_velocities[i][1];
        }
        if (position[1] > (pfloat32)rect[3])
        {
            position[1] = (pfloat32)rect[3];
            m_velocities[i][1] = -m_velocities[i][1];
        }
    
        m_logos[i]->transform().setTranslation(position);
    }
}
