// myscene.h
// "Light" example
//
// Copyright 2012 - 2014 Future Interface. 
// This software is licensed under the terms of the MIT license.
//
// Hongwei Li lihw81@gmail.com
//

#ifndef MYSCENE_H
#define MYSCENE_H

#include <Paper3D/pscene.h>

class PContext;
class PNode;
class PDrawable;
class PContext;
class PPointLight;
class PDirectionalLight;
class PSpotLight;

class MyScene : public PScene
{
public:
    MyScene(PContext *context);
    ~MyScene();

    void enablePointLight();
    void enableDirectionalLight();
    void enableSpotLight();
    void switchHighQualityLighting();

    virtual void update();

private:
    PDirectionalLight *m_directionalLight;
    PPointLight *m_pointLight;
    PSpotLight *m_spotLight;
    pbool m_isHighQuality;
    PDrawable *m_drawable;
    PNode *m_emptyNode;
};

#endif
