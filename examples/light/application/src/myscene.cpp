// myscene.cpp
// "Light" example
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
	m_isHighQuality = false;

    const puint32 *rect = context->rect();

    PResourceManager *resMgr = context->module<PResourceManager>("resource-manager");

    // -------------------------------------------------------------- 
    // Add camera
    // -------------------------------------------------------------- 
    PCamera *camera = PNEW(PCamera("camera", this));
    camera->setFixed(true);
    camera->transform().setLookAt(0.0f, 2.0f, 10.0f, 0.0f, 0.0f, 0.0f, 0.0f, 1.0f, 0.0f);
    camera->projection().perspective(45.0f, (pfloat32)rect[2] / (pfloat32)rect[3], 0.1f, 100.0f);
    setMainCamera(camera);
    

    // -------------------------------------------------------------- 
    // Scene objects
    // -------------------------------------------------------------- 
    PMaterial *material;
    PAbstractGeometry *geometry;

    geometry = PNEW(PGeometrySphere(resMgr));
    m_drawable = PNEW(PDrawable("directional-light", this));
    material = PNEW(PMaterial("phong-low.pmt", resMgr));
    material->parameter("albedo") = pVector3(1.0f, 0.0f, 0.0f);
    material->parameter("shininess") = 10.0f;

    m_drawable->setGeometry(geometry);
    m_drawable->setMaterial(material);
    m_drawable->transform().setScaling(pVector3(2.0f, 2.0f, 2.0f));
    
    // -------------------------------------------------------------- 
    // Scene lights
    // -------------------------------------------------------------- 
    m_emptyNode = PNEW(PNode("rotation", this, root(), false));
    
    m_directionalLight = PNEW(PDirectionalLight("directional", m_emptyNode));
    m_pointLight       = PNEW(PPointLight("point", m_emptyNode));
    m_spotLight        = PNEW(PSpotLight("spot", m_emptyNode));

    m_directionalLight->setDirection(-1.0f, -1.0f, -1.0f);
    m_directionalLight->setColor(P_COLOR_WHITE);
    m_directionalLight->setIntensity(0.8f);

    m_pointLight->transform().setTranslation(pVector3(2.0f, 2.0f, 2.0f));
    m_pointLight->setColor(P_COLOR_WHITE);
    m_pointLight->setIntensity(0.0f);
    m_pointLight->setRange(10.0f);
    
    m_spotLight->transform().setTranslation(pVector3(2.0f, 2.0f, 2.0f));
    m_spotLight->setColor(P_COLOR_WHITE);
    m_spotLight->setIntensity(0.0f);
    m_spotLight->setCutoff(P_PI / 6.0f);
    m_spotLight->setExponent(0.0f);
}

MyScene::~MyScene()
{
}
    
void MyScene::update()
{
	PVector3 rot = root()->transform().rotation();
    rot[1] += 0.001f;
	root()->transform().setRotation(rot);
}

void MyScene::enablePointLight()
{
    m_pointLight->setIntensity(0.8f);
    m_directionalLight->setIntensity(0.0f);
    m_spotLight->setIntensity(0.0f);

    PLOG_INFO("The point light is turned on.");
}

void MyScene::enableDirectionalLight()
{
    m_pointLight->setIntensity(0.0f);
    m_directionalLight->setIntensity(0.8f);
    m_spotLight->setIntensity(0.0f);
    
    PLOG_INFO("The directional light is turned on.");
}

void MyScene::enableSpotLight()
{
    m_pointLight->setIntensity(0.0f);
    m_directionalLight->setIntensity(0.0f);
    m_spotLight->setIntensity(0.8f);
    
    PLOG_INFO("The spot light is turned on.");
}

void MyScene::switchHighQualityLighting()
{
	PResourceManager *resMgr = context()->module<PResourceManager>("resource-manager");

    if (m_isHighQuality)
    {
        PMaterial *material = PNEW(PMaterial("phong-low.pmt", resMgr));
        material->parameter("albedo") = pVector3(1.0f, 0.0f, 0.0f);
        material->parameter("shininess") = 10.0f;

        m_drawable->setMaterial(material);

        m_isHighQuality = false;

		PLOG_INFO("Use low quality lighting now.");
    }
    else
    {
        PMaterial *material = PNEW(PMaterial("phong-high.pmt", resMgr));
        material->parameter("albedo") = pVector3(1.0f, 0.0f, 0.0f);
        material->parameter("shininess") = 10.0f;
        
        m_drawable->setMaterial(material);

        m_isHighQuality = true;

		PLOG_INFO("Use high quality lighting now.");
    }
}

