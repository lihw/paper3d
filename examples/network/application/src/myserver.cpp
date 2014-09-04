// myserver.cpp
// "Network" example
//
// Copyright 2012 - 2014 Future Interface. All rights reserved.
//
// Future Interface  lihw81@gmail.com
//

#include "myserver.h"

#include "myscene.h"

#include <PFoundation/pcrt.h>
#include <PFoundation/plog.h>
#include <PFoundation/pconststring.h>


MyPeer::MyPeer(puint32 id)
    : PNetworkPeer(id)
{
    m_state = CLOSED;
}

MyPeer::~MyPeer()
{
}

void MyPeer::setState(StateEnum state)
{
    m_state = state;
}

void MyPeer::setName(const pchar *name)
{
    m_name = name;
}

MyServer::MyServer(MyScene *scene, const puint16 port, PNetworkManager *networkManager)
    : PNetworkServer(port, networkManager)
{
    m_scene = scene;
}

MyServer::~MyServer()
{
}

PNetworkPeer *MyServer::onPeerConnected(puint32 id)
{
    MyPeer *peer = PNEW(MyPeer(id));
    peer->setState(MyPeer::CONNECTED);

    return peer;
}

void MyServer::onPeerDisconnected(PNetworkPeer *peer)
{
    MyPeer *p = (MyPeer*)peer;
    PLOG_INFO("peer %s disconnected", p->name());
}

void MyServer::onMessageReceived(PNetworkPeer *peer, const puint8 *message, puint32 length)
{
    pchar msg[4096];
    pstrncpy(msg, (const pchar *)message, length);
    msg[length] = 0;

    MyPeer *p = (MyPeer*)peer;
    switch (p->state())
    {
        case MyPeer::CONNECTED:
            p->setState(MyPeer::LOGIN);
            p->setName(msg);
            break;
        case MyPeer::LOGIN:
            {
                pint32 dx;
                pint32 dy;
                const pchar *pstr = msg;
                pstr = pStringUnpackInt(pstr, &dx);
                pstr = pStringUnpackInt(pstr, &dy);
                
                PLOG_INFO("peer %s says: move by (%d, %d).", p->name(), dx, dy);

                m_scene->moveObject(dx, dy);
            }
            break;
    }
}
