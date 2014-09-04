// myscene.h
// "Sprite" example 
//
// Copyright 2012 - 2014 Future Interface. 
// This software is licensed under the terms of the MIT license.
//
// Hongwei Li lihw81@gmail.com
//

#ifndef MYSCENE_H
#define MYSCENE_H

#include <Paper3D/pscene.h>

#include <PFoundation/pvector3.h>
#include <PFoundation/prandom.h>

class PContext;
class PDrawable;
class PSprite;


class MyScene : public PScene
{
public:
    MyScene(PContext *context);
    ~MyScene();

    virtual void update();

    void fire();

private:
    PDrawable *m_plane;
    
    enum { NUM_SPRITES = 10 };
    PSprite  *m_sprites[NUM_SPRITES];
    puint32   m_nextSprite;
    PVector3  m_velocity[NUM_SPRITES];

    PRandom m_random;
};

#endif
