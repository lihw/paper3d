// mycontext.h
// "NormalMap" example
//
// Copyright 2012 - 2014 Future Interface. All rights reserved.
//
// Future Interface  lihw81@gmail.com
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

    virtual void onPanBegin(pint32 x, pint32 y) override;
    virtual void onPan(pint32 x, pint32 y, pint32 dx, pint32 dy) override;
    virtual void onPanEnd() override;

private:
    MyScene *m_scene;

    pfloat32 m_anglexzTarget;
    pfloat32 m_angleyTarget;
    pfloat32 m_anglexz;
    pfloat32 m_angley;
};

#endif
