// myscene.h
// "SkyBox" example. The first example.
//
// Copyright 2012 - 2014 Future Interface . All rights reserved.
//
// Hongwei Li lihw81@gmail.com
//

#ifndef MYSCENE_H
#define MYSCENE_H

#include <Paper3D/pscene.h>

class PDrawable;
class PContext;

class MyScene : public PScene
{
public:
    MyScene(PContext *context);
    ~MyScene();

    virtual void update();

    void pan(pfloat32 angle);
    void yaw(pfloat32 angle);

private:
    PDrawable *m_skybox;
};

#endif
