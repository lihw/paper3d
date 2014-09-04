// myscene.h
// "Network" example
//
// Copyright 2012 - 2014 Future Interface. All rights reserved.
//
// Future Interface  lihw81@gmail.com
//

#ifndef MYSCENE_H
#define MYSCENE_H

#include <Paper3D/pscene.h>

class PContext;
class PSprite2D;

class MyScene : public PScene
{
public:
    MyScene(PContext *context);
    ~MyScene();

    virtual void update();

    void setupServerScene();
    void setupClientScene();

    void moveObject(pint32 dx, pint32 dy);

private:
    PSprite2D *m_type;
    PSprite2D *m_object;
};

#endif
