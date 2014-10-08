// mycontext.cpp
// "SkyBox" example. The first example.
//
// Copyright 2012 - 2014 Future Interface . All rights reserved.
//
// Hongwei Li lihw81@gmail.com
//

#include "mycontext.h"

#include <Paper3D/pscenemanager.h>

#include <PFoundation/pevent.h>
#include <PFoundation/ptime.h>
#include <PFoundation/pinput.h>
#include <PFoundation/pgesturemanager.h>

#include "myscene.h"

    
MyContext::MyContext(const PContextProperties &properties)
    : PContext(properties)
{
    m_scene = P_NULL;
}

MyContext::~MyContext()
{
}

pbool MyContext::onInitialized()
{
    m_scene = PNEW(MyScene(this));
    PSceneManager *sceneMgr = module<PSceneManager>("scene-manager");
    sceneMgr->addScene(m_scene);
    sceneMgr->setMainScene(m_scene);

    PGestureManager *gestureMgr = gestureManager();
    gestureMgr->setGestureEnabled(P_GESTURE_TYPE_PAN, true);

    //puint32 viewport[] = { 0, 0, 640, 480 };
    //m_scene->setViewport(viewport);

    return true;
}

pbool MyContext::onUpdate()
{
    return true;
}

void MyContext::onDestroy()
{
}

void MyContext::onKeyboard(puint32 state, puint32 key, puint32 scancode)
{
    if ((state & 0x03)== P_KEY_DEVICE_STATE_UP)
    {
        switch (scancode)
        {
            case P_KEY_ESC:
                quit();
                break;
            case P_KEY_A:     // pan to left
                m_scene->pan(2.0f);
                break;
            case P_KEY_D:    // pan to right
                m_scene->pan(-2.0f);
                break;
            case P_KEY_W: // look up
                m_scene->yaw(2.0f);
                break;
            case P_KEY_S: // look down
                m_scene->yaw(-2.0f);
                break;
        }
    }
}
