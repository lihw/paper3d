// myscene.cpp
// "Geometry" example 
//
// Copyright 2012 - 2014 Future Interface. 
// This software is licensed under the terms of the MIT license.
//
// Hongwei Li lihw81@gmail.com

#include "myscene.h"

#include <Paper3D/paper3d.h>

#include <PFoundation/pfoundation.h>
#include <PFoundation/pcontext.h>


MyScene::MyScene(PContext *context)
    : PScene("my-scene", context)
{
     const puint32 *rect = context->rect();

    // -------------------------------------------------------------- 
    // Add camera
    // -------------------------------------------------------------- 
    PCamera *camera = PNEW(PCamera("camera", this));
    camera->setFixed(true);
    camera->transform().setLookAt(0.0f, 0.0f, 10.0f, 0.0f, 0.0f, 0.0f, 0.0f, 1.0f, 0.0f);
    camera->projection().perspective(45.0f, (pfloat32)rect[2] / (pfloat32)rect[3], 0.1f, 100.0f);
    setMainCamera(camera);
    
    // -------------------------------------------------------------- 
    // Scene objects
    // -------------------------------------------------------------- 
	PResourceManager *resMgr = context->module<PResourceManager>("resource-manager");

    PMaterial *material = PNEW(PMaterial("color.pmt", resMgr));
    material->parameter("color") = P_COLOR_BLUE;
        
    m_sphere = PNEW(PDrawable("sphere", this));
    m_plane  = PNEW(PDrawable("plane", this));
    m_cube   = PNEW(PDrawable("cube", this));
    m_mesh   = PNEW(PDrawable("mesh", this));
   
    m_sphere->setGeometry(PNEW(PGeometrySphere(resMgr)));
    m_cube->setGeometry(PNEW(PGeometryCube(resMgr)));
    m_plane->setGeometry(PNEW(PGeometryPlane(resMgr)));
    m_mesh->setGeometry(PNEW(PGeometryMesh("diamond_a.pmh", resMgr)));

    m_sphere->setMaterial(material);
    m_cube->setMaterial(material);
    m_plane->setMaterial(material);
    m_mesh->setMaterial(material);

    m_mesh->transform().setScaling(pVector3(0.015f, 0.015f, 0.015f));

    m_sphere->transform().setTranslation(pVector3(-1.5f, -1.5f, 0.0f));
    m_cube->transform().setTranslation(pVector3(1.5f, -1.5f, 0.0f));
    m_plane->transform().setTranslation(pVector3(-1.5f, 1.5f, 0.0f));
    m_mesh->transform().setTranslation(pVector3(1.5f, 1.5f, 0.0f));

    setBackgroundColor(pColorRGBA(.5f, .5f, .5f, 0.0f));
}

MyScene::~MyScene()
{
}
    
void MyScene::update()
{
  pfloat32 dt = context()->clock().deltaTime() * 0.01f;

    PVector3 rot;
    
    rot = m_sphere->transform().rotation();
    rot.m_v[0] += dt * 0.01f;
    rot.m_v[1] += dt * 0.02f;
    rot.m_v[2] += dt * 0.03f;
    m_sphere->transform().setRotation(rot);

    rot = m_cube->transform().rotation();
    rot.m_v[0] += dt * 0.01f;
    rot.m_v[1] += dt * 0.02f;
    rot.m_v[2] += dt * 0.03f;
    m_cube->transform().setRotation(rot);

    rot = m_plane->transform().rotation();
    rot.m_v[0] += dt * 0.01f;
    rot.m_v[1] += dt * 0.02f;
    rot.m_v[2] += dt * 0.03f;
    m_plane->transform().setRotation(rot);

    rot = m_mesh->transform().rotation();
    rot.m_v[0] += dt * 0.01f;
    rot.m_v[1] += dt * 0.02f;
    rot.m_v[2] += dt * 0.03f;
    m_mesh->transform().setRotation(rot);
}
