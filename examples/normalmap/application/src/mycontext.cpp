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
		}
	}
}

void MyContext::onTouchDown(pint32 id, pint32 x, pint32 y)
{
    m_arcball.restart();
    m_scene->setRotating(true);
}

void MyContext::onTouchMove(pint32 id, pint32 x, pint32 y) 
{
    const puint32 *r = rect();
    pfloat32 xx = (pfloat32)x / (pfloat32)(r[2] - 1) * 2.0f - 1.0f;
    pfloat32 yy = (pfloat32)(r[3] - 1 - y) / (pfloat32)(r[3] - 1) * 2.0f - 1.0f;
    m_arcball.updateMouse(xx, yy);

    m_scene->rotate(m_arcball.rotation());
}

void MyContext::onTouchUp(pint32 id, pint32 x, pint32 y)
{
    m_scene->setRotating(false);
}
