// mycontext.cpp
// "Background" example. 
//
// Copyright 2013 - 2014 Future Interface. 
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

	m_value = PNEW(PInterpolatedValue(0, 0, 9));
	m_value->setRange(0.0f, 20.0f);

    PLOG_INFO("Press 'F' to switch background fill mode.");

    return true;
}

pbool MyContext::onUpdate()
{
	pfloat32 deltaTime = clock().deltaTime();
	m_value->update(puint32(deltaTime));    
    pfloat32 speed = m_value->getValue();
	
    pfloat32 offset = speed * deltaTime / 20000.0f;
	m_scene->rotate(offset);

    return true;
}

void MyContext::onDestroy()
{
	PDELETE(m_value);
}

pbool MyContext::onKeyboard(PEvent *event)
{
	pint32 key = event->parameter(P_EVENTPARAMETER__KEY_SCANCODE).toInt();
	pint32 type = event->getType();

	if (type == P_EVENT__KEYDOWN)
	{
		switch (key)
		{
		case P_KEY_ESC:
            quit();
			break;
        case P_KEY_F:
            m_scene->switchBackgroundFillMode();
		}
	}

    return true;
}

pbool MyContext::onPanBegin(PEvent *event)
{
	return true;
}

pbool MyContext::onPan(PEvent *event)
{
	pint32 deltaX = event->parameter(P_EVENTPARAMETER__PAN_DELTA_X).toInt();

	m_value->modifySpeed(-deltaX * (15000.0f / 4096.0f));

	return true;
}

pbool MyContext::onPanEnd(PEvent *event)
{
	return true;
}
    
