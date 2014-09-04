// pscenemanager.cpp
// Manage all the scenes
//
// Copyright 2012 - 2014 Future Interface. 
// This software is licensed under the terms of the MIT license.
//
// Hongwei Li lihw81@gmail.com
//
// 

#include "pscenemanager.h"

#include <Paper3D/pscene.h>
#include <Paper3D/prenderstate.h>
#include <Paper3D/pnodefactory.h>
#include <Paper3D/peffectfactory.h>

#include <PFoundation/pcontext.h>


PSceneManager::PSceneManager(PContext *context)
    : PModule("scene-manager", context)
{
    m_mainScene   = P_NULL;
    m_renderState = P_NULL;

    PScene::s_effectFactory.initialize();
    PScene::s_nodeFactory.initialize();
}

PSceneManager::~PSceneManager()
{
    PList<PScene *>::iterator it = m_scenes.begin();
    PList<PScene *>::iterator ie = m_scenes.end();
    while (it != ie)
    {
        PDELETE(*it);
        ++it;
    }
        
    PDELETE(m_renderState);

    PScene::s_effectFactory.uninitialize();
    PScene::s_nodeFactory.uninitialize();
}

void PSceneManager::addScene(PScene *scene)
{
    m_scenes.append(scene);
    if (m_scenes.count() == 1 && m_mainScene == P_NULL)
    {
        m_mainScene = scene;
    }
}

void PSceneManager::setMainScene(PScene *scene)
{
#if defined P_DEBUG
    PList<PScene *>::iterator it = m_scenes.find(scene);
    if (it == m_scenes.end())
    {
        PLOG_WARNING("The scene is not in the scene list of the context");
    }
#endif
    
    PASSERT(scene != P_NULL);
    m_mainScene = scene;
}

pbool PSceneManager::initialize()
{
    m_renderState = PNEW(PRenderState(m_context->rect()));
    return false;
}

pbool PSceneManager::resume()
{
    m_renderState->reset();

    return false;
}

void PSceneManager::pause()
{
}

void PSceneManager::update()
{
    if (m_mainScene != P_NULL)
    {
        m_mainScene->update();
        m_renderState->beginDraw();
        m_mainScene->render(m_renderState);
        m_renderState->endDraw();
    }
}
