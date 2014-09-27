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

    puint32 viewport[] = { 0, 0, 640, 240 };
    m_scene->setViewport(viewport);

    return true;
}

pbool MyContext::onUpdate()
{
    return true;
}

void MyContext::onDestroy()
{
}

pbool MyContext::onKeyboard(PEvent *event)
{
    pint32 key = event->parameter(P_EVENTPARAMETER__KEY_SCANCODE).toInt();
    pint32 type = event->getType();
    if (type == P_EVENT__KEYUP)
    {
        switch (key)
        {
            case P_KEY_ESC:
                quit();
                return true;
            case P_KEY_A:     // pan to left
                m_scene->pan(2.0f);
                return true;
            case P_KEY_D:    // pan to right
                m_scene->pan(-2.0f);
                return true;
            case P_KEY_W: // look up
                m_scene->yaw(2.0f);
                return true;
            case P_KEY_S: // look down
                m_scene->yaw(-2.0f);
                return true;
        }
    }
    
    return false;
}
