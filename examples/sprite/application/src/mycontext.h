// mycontext.h
// "Sprite" example 
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

class MyScene;

class MyContext : public PContext
{
public:
    MyContext(const PContextProperties &properties);
    ~MyContext();
    
    virtual pbool onInitialized() override;
    virtual pbool onUpdate() override;
    virtual void onDestroy() override;
    
    virtual void onKeyboard(puint32 state, puint32 key, puint32 scancode) override;

private:
    MyScene *m_scene;
};

#endif
