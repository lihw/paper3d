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
    
    virtual pbool onInitialized() override;
    virtual pbool onUpdate() override;
    virtual void onDestroy() override;
    
    virtual void onKeyboard(puint32 state, puint32 key, puint32 scancode) override;
    virtual void onPanBegin(pint32 x, pint32 y) override;
    virtual void onPan(pint32 x, pint32 y, pint32 dx, pint32 dy) override;
    virtual void onPanEnd() override;

private:
    MyScene *m_scene;
    PInterpolatedValue *m_value;
};

#endif
