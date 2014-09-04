// myscene.cpp
// "Glow" example
//
// Copyright 2012 - 2014 Future Interface. 
// This software is licensed under the terms of the MIT license.
//
// Hongwei Li lihw81@gmail.com

#include "myscene.h"

#include <Paper3D/paper3d.h>

#include <PFoundation/pfoundation.h>
#include <PFoundation/pcontext.h>

// FIXME: blur is accumulated!


MyScene::MyScene(PContext *context)
    : PScene("my-scene", context)
{
	PResourceManager *resMgr = context->module<PResourceManager>("resource-manager");
    
    // -------------------------------------------------------------- 
    // Add camera
    // -------------------------------------------------------------- 
    const puint32 *rect = context->rect();

    PCamera *camera = PNEW(PCamera("camera", this));
    camera->setFixed(true);
    camera->transform().setLookAt(0.0f, 0.0f, 10.0f, 0.0f, 0.0f, 0.0f, 0.0f, 1.0f, 0.0f);
    camera->projection().orthogonal((pfloat32)rect[2] / (pfloat32)rect[3], 0.1f, 100.0f);
    setMainCamera(camera);
    
    // -------------------------------------------------------------- 
    // Scene background
    // -------------------------------------------------------------- 
    setBackgroundColor(P_COLOR_BLACK_TRANSPARENT);    

    // -------------------------------------------------------------- 
    // Scene objects
    // -------------------------------------------------------------- 
    PMaterial *material; 
    PAbstractGeometry *geometry;

    material = PNEW(PMaterial("color.pmt", resMgr));
    material->parameter("color") = pColorRGBA(1.0f, 0.0f, 0.0f, 1.0f);
    geometry = PNEW(PGeometryPlane(resMgr));
    PDrawable *plane1 = PNEW(PDrawable("plane1", this));
    plane1->setMaterial(material);
    plane1->setGeometry(geometry);
    plane1->transform().setScaling(pVector3(0.2f, 0.2f, 1.0f));
    plane1->transform().setTranslation(pVector3(0.0f, -0.6f, 0.0f));
    
    material = PNEW(PMaterial("color.pmt", resMgr));
    material->parameter("color") = pColorRGBA(0.0f, 1.0f, 0.0f, 1.0f);
    geometry = PNEW(PGeometryPlane(resMgr));
    PDrawable *plane2 = PNEW(PDrawable("plane2", this));
    plane2->setMaterial(material);
    plane2->setGeometry(geometry);
    plane2->transform().setScaling(pVector3(0.2f, 0.2f, 1.0f));
    plane2->transform().setTranslation(pVector3(0.0f, 0.0f, 0.0f));
    
    material = PNEW(PMaterial("color.pmt", resMgr));
    material->parameter("color") = pColorRGBA(0.0f, 0.0f, 1.0f, 1.0f);
    geometry = PNEW(PGeometryPlane(resMgr));
    PDrawable *plane3 = PNEW(PDrawable("plane3", this));
    plane3->setMaterial(material);
    plane3->setGeometry(geometry);
    plane3->transform().setScaling(pVector3(0.2f, 0.2f, 1.0f));
    plane3->transform().setTranslation(pVector3(0.0f, 0.6f, 0.0f));

    // -------------------------------------------------------------- 
    // Global post processing effect.
    // -------------------------------------------------------------- 
    PEffectGlow *glow;
    glow = PNEW(PEffectGlow(this));
    glow->setBlendMode(P_EFFECTBLEND_MIX);
    glow->setBlurIterations(1);
    glow->setBlurKernelSize(5);
    glow->setBlurDistance(2);
}

MyScene::~MyScene()
{
}
    
void MyScene::update()
{
}
