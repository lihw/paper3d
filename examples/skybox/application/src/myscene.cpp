// myscene.h
// "SkyBox" example. The first example.
//
// Copyright 2012 - 2014 Future Interface . All rights reserved.
//
// Hongwei Li lihw81@gmail.com
//

#include "myscene.h"

#include <Paper3D/presourcemanager.h>
#include <Paper3D/pcamera.h>
#include <Paper3D/pskybox.h>

#include <PFoundation/pfoundation.h>


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
    camera->projection().perspective(60.0f, 2.0f * (pfloat32)rect[2] / (pfloat32)rect[3], 0.1f, 100.0f);
    setMainCamera(camera);
    
    // -------------------------------------------------------------- 
    // Scene objects
    // -------------------------------------------------------------- 
    PSkyBox *skybox = PNEW(PSkyBox("skybox", this));
    skybox->setTexture(resMgr->getTexture("sky/*.tga"));
}

MyScene::~MyScene()
{
}
    
void MyScene::update()
{
}
    
void MyScene::pan(pfloat32 deltaAngle)
{
    deltaAngle = deltaAngle * P_PI / 180.0f;
    PCamera *camera = mainCamera();
    pfloat32 y = camera->transform().rotationY();
    camera->transform().setRotationY(y + deltaAngle);
}

void MyScene::yaw(pfloat32 deltaAngle)
{
    deltaAngle = deltaAngle * P_PI / 180.0f;
    PCamera *camera = mainCamera();
    pfloat32 x = camera->transform().rotationX();
    camera->transform().setRotationX(x + deltaAngle);
}
