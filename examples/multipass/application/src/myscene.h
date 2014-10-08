// myscene.h
// "Multipass" example.
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
class PRenderPass;
class PFrameBuffer;

class MyScene : public PScene
{
public:
    MyScene(PContext *context);
    ~MyScene();

    virtual void update();

    void switchRenderPass();

private:
    PRenderPass  *m_singlePass;
    PRenderPass  *m_twoPass0;
    PRenderPass  *m_twoPass1;
    pbool         m_useSinglePass;
    PFrameBuffer *m_framebuffer;
};

#endif
