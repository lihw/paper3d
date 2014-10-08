// mycontext.cpp
// "Light" example
//
// Copyright 2012 - 2014 Future Interface. 
// This software is licensed under the terms of the MIT license.
//
// Hongwei Li lihw81@gmail.com
//

#include "mycontext.h"

#include "myscene.h"

#include <Paper3D/pscenemanager.h>

#include <PFoundation/pevent.h>
#include <PFoundation/pinput.h>
    
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

	PLOG_INFO("<Key settings>");
    PLOG_INFO("P - use point lighting");
    PLOG_INFO("D - use directional lighting");
    PLOG_INFO("S - use spot lighting");
    PLOG_INFO("H - switch low and high lighting quality");

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
            case P_KEY_P:
                m_scene->enablePointLight();
                break;
            case P_KEY_D:
                m_scene->enableDirectionalLight();
                break;
            case P_KEY_S:
                m_scene->enableSpotLight();
                break;
            case P_KEY_H:
                m_scene->switchHighQualityLighting();
                break;
        }
    }
}
