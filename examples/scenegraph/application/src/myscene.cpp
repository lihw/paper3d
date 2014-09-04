// myscene.cpp
// "SceneGraph" example
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
    m_stopped = false;

    PResourceManager *resMgr = context->module<PResourceManager>("resource-manager");

    PRandom random(1001);

    const puint32 *rect = context->rect();

    // -------------------------------------------------------------- 
    // Add camera
    // -------------------------------------------------------------- 
    PCamera *camera = PNEW(PCamera("camera", this));
    camera->setFixed(true);
    camera->transform().setLookAt(0.0f, 2.0f, 10.0f, 0.0f, 0.0f, 0.0f, 0.0f, 1.0f, 0.0f);
    camera->projection().perspective(45.0f, (pfloat32)rect[2] / (pfloat32)rect[3], 0.1f, 100.0f);
    setMainCamera(camera);
    
    // -------------------------------------------------------------- 
    // Solar system
    // -------------------------------------------------------------- 
    PMaterial *material;
    PAbstractGeometry *geometry;

    root()->transform().setRotation(pVector3(0, 0, 0.3f));

    geometry = PNEW(PGeometrySphere(resMgr));
    m_sun = PNEW(PDrawable("sun", this));
    material = PNEW(PMaterial("texture.pmt", resMgr));
    material->parameter("texture") = resMgr->getTexture("sun.png");
    m_sun->setGeometry(geometry);
    m_sun->setMaterial(material);
    m_sun->transform().setScaling(pVector3(0.8f, 0.8f, 0.8f));

    m_venusOrbit = PNEW(PNode("venus-orbit", this, root(), false));
    
    geometry = PNEW(PGeometrySphere(resMgr));
    m_venus = PNEW(PDrawable("venus", m_venusOrbit));
    material = PNEW(PMaterial("texture.pmt", resMgr));
    material->parameter("texture") = resMgr->getTexture("venus.png");
    m_venus->setGeometry(geometry);
    m_venus->setMaterial(material);
    m_venus->transform().setScaling(pVector3(0.45f, 0.45f, 0.45f));
    m_venus->transform().setTranslation(pVector3(1.5f, 0.0f, 0.0f));
    
    m_earthOrbit = PNEW(PNode("earth-orbit", this, root(), false));
    
    geometry = PNEW(PGeometrySphere(resMgr));
    m_earth = PNEW(PDrawable("earth", m_earthOrbit));
    material = PNEW(PMaterial("texture.pmt", resMgr));
    material->parameter("texture") = resMgr->getTexture("earth.png");
    m_earth->setGeometry(geometry);
    m_earth->setMaterial(material);
    m_earth->transform().setScaling(pVector3(0.7f, 0.7f, 0.7f));
    m_earth->transform().setTranslation(pVector3(3.2f, 0.0f, 0.0f));
    
    m_moonOrbit = PNEW(PNode("moon-orbit", this, m_earth, false));

    geometry = PNEW(PGeometrySphere(resMgr));
    m_moon = PNEW(PDrawable("moon", m_moonOrbit));
    material = PNEW(PMaterial("color.pmt", resMgr));
    material->parameter("color") = pVector3(0.9f, 0.9f, 0.9f);
    m_moon->setGeometry(geometry);
    m_moon->setMaterial(material);
    m_moon->transform().setScaling(pVector3(0.25f, 0.25f, 0.25f));
    m_moon->transform().setTranslation(pVector3(0.0f, -1.5f, 0.0f));
    
    m_marsOrbit = PNEW(PNode("mars-orbit", this, root(), false));
    
    geometry = PNEW(PGeometrySphere(resMgr));
    m_mars = PNEW(PDrawable("mars", m_marsOrbit));
    material = PNEW(PMaterial("texture.pmt", resMgr));
    material->parameter("texture") = resMgr->getTexture("mars.png");
    m_mars->setGeometry(geometry);
    m_mars->setMaterial(material);
    m_mars->transform().setScaling(pVector3(0.45f, 0.45f, 0.45f));
    m_mars->transform().setTranslation(pVector3(4.8f, 0.0f, 0.0f));
}

MyScene::~MyScene()
{
}
    
void MyScene::update()
{
    PVector3 rot;

    if (!m_stopped)
    {
        // Move on the orbit
        rot = m_venusOrbit->transform().rotation();
        rot[1] += 0.02f;
        m_venusOrbit->transform().setRotation(rot);
       
        rot = m_earthOrbit->transform().rotation();
        rot[1] += 0.01f;
        m_earthOrbit->transform().setRotation(rot);
        
        rot = m_marsOrbit->transform().rotation();
        rot[1] += 0.007f;
        m_marsOrbit->transform().setRotation(rot);
        
        rot = m_moonOrbit->transform().rotation();
        rot[0] += 0.05f;
        m_moonOrbit->transform().setRotation(rot);

        // Self-rotating
        rot = m_venus->transform().rotation();
        rot[1] += 0.01f;
        m_venus->transform().setRotation(rot);

        rot = m_earth->transform().rotation();
        rot[1] += 0.005f;
        m_earth->transform().setRotation(rot);
        
        rot = m_mars->transform().rotation();
        rot[1] += 0.0035f;
        m_mars->transform().setRotation(rot);
    }
}

void MyScene::stop()
{
    m_stopped = !m_stopped;
}
