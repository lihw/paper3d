// myscene.h
// "SceneGraph" example
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


class MyScene : public PScene
{
public:
    MyScene(PContext *context);
    ~MyScene();

    virtual void update();
    
    void stop();

private:
    pbool m_stopped;

    pfloat32 m_sunRotationSpeed;
    pfloat32 m_earthRotationSpeed;
    pfloat32 m_venusRotationSpeed;
    pfloat32 m_marsRotationSpeed;

    PDrawable *m_sun;
    PDrawable *m_earth;
    PDrawable *m_moon;
    PDrawable *m_venus;
    PDrawable *m_mars;

    PNode *m_earthOrbit;
    PNode *m_venusOrbit;
    PNode *m_marsOrbit;
    PNode *m_moonOrbit;
};

#endif
