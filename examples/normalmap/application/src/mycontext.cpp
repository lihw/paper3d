// mycontext.cpp
// "NormalMap" example
//
// Copyright 2012 - 2014 Future Interface. All rights reserved.
//
// Future Interface  lihw81@gmail.com
//

#include "mycontext.h"

#include "myscene.h"

#include <Paper3D/paper3d.h>

#include <PFoundation/pfoundation.h>


MyContext::MyContext(const PContextProperties &properties)
    : PContext(properties)
{
    m_scene = P_NULL;

    m_anglexzTarget = 0;
    m_angleyTarget  = 0;
    m_anglexz       = 0;
    m_angley        = 0;
    
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

    return true;
}

pbool MyContext::onUpdate()
{
    const pfloat32 it1 = 0.85f;
    const pfloat32 itt1 = 1.0f - it1;

    m_anglexz = m_anglexz * it1 + m_anglexzTarget * itt1;
    m_angley  = m_angley * it1 + m_angleyTarget * itt1;

    m_scene->rotate(m_anglexz, m_angley, 0);
    
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
		}
	}
}

void MyContext::onPanBegin(pint32 x, pint32 y)
{
    m_scene->setRotating(true);
}

void MyContext::onPan(pint32 x, pint32 y, pint32 dx, pint32 dy) 
{
    m_anglexzTarget += dy * 0.008f;
    m_angleyTarget  += dx * 0.008f;
}

void MyContext::onPanEnd()
{
    m_scene->setRotating(false);

    m_anglexzTarget = 0;
    m_angleyTarget  = 0;
    m_anglexz       = 0;
    m_angley        = 0;
}
