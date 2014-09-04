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
        }
    }
    return true;
}

pbool MyContext::onTouch(PEvent *event)
{    
    if (event->getType() == P_EVENT__TOUCH_DOWN)
    {
        m_arcball.restart();

        m_scene->setRotating(true);
    }
    else if (event->getType() == P_EVENT__TOUCH_MOVE)
    {
        puint32 x = event->parameter(P_EVENTPARAMETER__TOUCH_X).toInt();
        puint32 y = event->parameter(P_EVENTPARAMETER__TOUCH_Y).toInt();
        const puint32 *r = rect();
        pfloat32 xx = (pfloat32)x / (pfloat32)(r[2] - 1) * 2.0f - 1.0f;
        pfloat32 yy = (pfloat32)(r[3] - 1 - y) / (pfloat32)(r[3] - 1) * 2.0f - 1.0f;
        m_arcball.updateMouse(xx, yy);

        m_scene->rotate(m_arcball.getRotationMatrix());
    }
    else if (event->getType() == P_EVENT__TOUCH_UP)
    {
        m_scene->setRotating(false);
    }

    return true;
}



