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
#include <PFoundation/parcball.h>

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

    virtual void onTouchDown(pint32 id, pint32 x, pint32 y) override;
    virtual void onTouchUp(pint32 id, pint32 x, pint32 y) override;
    virtual void onTouchMove(pint32 id, pint32 x, pint32 y) override;

private:
    MyScene *m_scene;
    PArcball m_arcball;
};

#endif
