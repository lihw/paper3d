// myscene.h
// "Cubemap" example. 
//
// Copyright 2012 - 2014 Future Interface. All rights reserved.
//
// Hongwei Li lihw81@gmail.com
//

#ifndef MYSCENE_H
#define MYSCENE_H

#include <Paper3D/pscene.h>

class PDrawable;
class PSkyBox;

class MyScene : public PScene
{
public:
    MyScene(PContext *context);
    ~MyScene();

    virtual void update();

private:
    PDrawable *m_teapot;
    PSkyBox   *m_skybox;
};

#endif
