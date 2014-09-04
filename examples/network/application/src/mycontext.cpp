// mycontext.cpp
// "Network" example
//
// Copyright 2012 - 2014 Future Interface. All rights reserved.
//
// Future Interface  lihw81@gmail.com
//

#include "mycontext.h"

#include "myscene.h"
#include "myserver.h"
#include "myclient.h"

#include <Paper3D/paper3d.h>

#include <PFoundation/pfoundation.h>


MyContext::MyContext(const PContextProperties &properties)
    : PContext(properties)
{
    m_scene = P_NULL;
    m_server = P_NULL;
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

    PIni ini("setting.ini");
    if (ini.numberOfEntries() == 0)
    {
        return false;
    }
    
    PString address = ini.entry("address");
    puint32 port    = ini.entry("port").toInt();
    PString type    = ini.entry("type");

    // Always make win32 application a server
#if defined P_WIN32
    type = PString("server");
#endif

    if (type == "server")
    {
        PNetworkManager *networkMgr = module<PNetworkManager>("network-manager");
        PLOG_INFO("Start a server.");
        m_server = PNEW(MyServer(m_scene, (puint16)port, networkMgr));
        m_scene->setupServerScene();
    }
    else if (type == "client")
    {
        PNetworkManager *networkMgr = module<PNetworkManager>("network-manager");
        PLOG_INFO("Start a client.");
        m_client = PNEW(MyClient(address.c_str(), (puint16)port, 1000, networkMgr));
        m_scene->setupClientScene();
    }

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

pbool MyContext::onPanBegin(PEvent *event)
{
	return true;
}

pbool MyContext::onPan(PEvent *event)
{
    if (m_client != P_NULL && m_client->type() == PNetworkClient::NETWORK_CLIENT)
    {
        pint32 deltaX = event->parameter(P_EVENTPARAMETER__PAN_DELTA_X).toInt();
        pint32 deltaY = event->parameter(P_EVENTPARAMETER__PAN_DELTA_Y).toInt();

        puint8 msg[1024];
        psprintf((pchar *)msg, 1024, "%d,%d", deltaX, deltaY);
        PLOG_INFO("message: %s.", msg);
        m_client->send(msg, -1);
    }

	return true;
}

pbool MyContext::onPanEnd(PEvent *event)
{
	return true;
}

