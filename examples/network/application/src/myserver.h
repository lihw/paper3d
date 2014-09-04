// myserver.h
// "Network" example
//
// Copyright 2012 - 2014 Future Interface. All rights reserved.
//
// Future Interface  lihw81@gmail.com
//

#ifndef MYSERVER_H
#define MYSERVER_H

#include <PFoundation/pnetworkserver.h>

class MyScene;

class MyPeer : public PNetworkPeer
{
public:
    MyPeer(puint32 id);
    virtual ~MyPeer();
    
    enum StateEnum
    {
        CONNECTED,
        LOGIN,
        CLOSED,
    };
    
    P_INLINE StateEnum state() const 
    { return m_state; }
    P_INLINE const pchar *name() const 
    { return m_name.c_str(); }
    
    void setName(const pchar *name);
    void setState(StateEnum state); 

private:
    PString   m_name;
    StateEnum m_state;
};

class MyServer : public PNetworkServer
{
public:
    MyServer(MyScene *scene, puint16 port, PNetworkManager *networkManager);
    virtual ~MyServer();

    virtual PNetworkPeer *onPeerConnected(puint32 id);
    virtual void onPeerDisconnected(PNetworkPeer *peer);
    virtual void onMessageReceived(PNetworkPeer *peer, const puint8 *message, puint32 length);

private:
    MyScene *m_scene;
};

#endif //!MYSERVER_H
