// myscene.h
// "NormalMap" example
//
// Copyright 2012 - 2014 Future Interface. All rights reserved.
//
// Future Interface  lihw81@gmail.com
//

#ifndef MYSCENE_H
#define MYSCENE_H

#include <Paper3D/pscene.h>

class PContext;
struct PMatrix3x3;
class PDrawable;
class PDirectionalLight;
class PRenderPass;
class PFrameBuffer;

class MyScene : public PScene
{
public:
    MyScene(PContext *context);
    ~MyScene();

    virtual void update();
    void rotate(const PMatrix3x3 &matrix);
    void setRotating(pbool rotating);

private:
    PDrawable           *m_drawable;
    PDirectionalLight   *m_light;

    PFrameBuffer        *m_framebuffer;
    PRenderPass         *m_mainPass;
    PRenderPass         *m_shadowPass;

    pbool                m_rotating;
};

#endif

