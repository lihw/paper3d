// mycontext.h
// "Network" example
//
// Copyright 2012 - 2014 Future Interface. All rights reserved.
//
// Future Interface  lihw81@gmail.com
//

#ifndef MYCONTEXT_H
#define MYCONTEXT_H

#include <PFoundation/pcontext.h>
#include <PFoundation/pcontextproperties.h>

class MyServer;
class MyClient;
class MyScene;

class MyContext : public PContext
{
public:
    MyContext(const PContextProperties &properties);
    ~MyContext();
    
    virtual pbool onInitialized();
    virtual pbool onUpdate();
    virtual void onDestroy();
    

protected:
    virtual pbool onKeyboard(PEvent *event);
    virtual pbool onPanBegin(PEvent *event);
    virtual pbool onPan(PEvent *event);
    virtual pbool onPanEnd(PEvent *event);

private:
    MyScene      *m_scene;
    union
    {
        MyServer *m_server;
        MyClient *m_client;
    };
    
};

#endif
