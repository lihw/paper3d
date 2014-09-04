// myscene.h
// "Geometry" example 
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
class PDrawable;

class MyScene : public PScene
{
public:
    MyScene(PContext *context);
    ~MyScene();

    virtual void update();

private:
    PDrawable *m_sphere;
    PDrawable *m_plane;
    PDrawable *m_cube;
    PDrawable *m_mesh;
};

#endif
