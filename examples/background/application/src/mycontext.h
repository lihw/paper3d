// mycontext.h
// "Background" example. 
//
// Copyright 2012 - 2014 Future Interface. 
// This software is licensed under the terms of the MIT license.
//
// Hongwei Li lihw81@gmail.com
//

#ifndef MYCONTEXT_H
#define MYCONTEXT_H

#include <PFoundation/pcontext.h>
#include <PFoundation/pcontextproperties.h>
#include <PFoundation/pinterpolatedvalue.h>


class MyScene;

class MyContext : public PContext
{
public:
    MyContext(const PContextProperties &properties);
    ~MyContext();
    
    virtual pbool onInitialized();
    virtual pbool onUpdate();
    virtual void onDestroy();
    
    virtual pbool onKeyboard(PEvent *event);
    virtual pbool onPanBegin(PEvent *event);
    virtual pbool onPan(PEvent *event);
    virtual pbool onPanEnd(PEvent *event);
    

private:
    MyScene *m_scene;
    PInterpolatedValue *m_value;
};

#endif
