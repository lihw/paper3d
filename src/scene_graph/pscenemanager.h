// pscenemanager.h 
// Manage all the scenes
//
// Copyright 2012 - 2014 Future Interface. 
// This software is licensed under the terms of the MIT license.
//
// Hongwei Li lihw81@gmail.com
//
// 

#ifndef PSCENEMANAGER_H
#define PSCENEMANAGER_H

#include <PFoundation/pmodule.h>
#include <PFoundation/plist.h>

class PScene;
class PRenderState;


class P_DLLEXPORT PSceneManager : public PModule
{
public:
    PSceneManager(PContext *context);
    virtual ~PSceneManager();

    void addScene(PScene *scene);
    void setMainScene(PScene *scene);

    virtual pbool initialize();
    virtual pbool resume();
    virtual void pause();
    virtual void update();

private:
    PList<PScene *> m_scenes;
    PScene *        m_mainScene;
    PRenderState   *m_renderState;
};


#endif // !PSCENEMANAGER_H
