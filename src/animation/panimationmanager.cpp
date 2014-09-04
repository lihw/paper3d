// panimationmanager.h 
// The animation manager which manages all animations
// in the context. 
//
// Copyright 2012 - 2014 Future Interface. 
// This software is licensed under the terms of the MIT license.
//
// Hongwei Li lihw81@gmail.com
//

#include "panimationmanager.h"

#include <Paper3D/panimationinstance.h>

#include <PFoundation/pcontext.h>
#include <PFoundation/pclock.h>

PAnimationManager::PAnimationManager(PContext *context)
{
    m_context = context;
}
PAnimationManager::~PAnimationManager()
{
    // The animation manager is deleted when context quits.
    PList<PAnimationInstance *>::iterator it = m_animations.begin();
    PList<PAnimationInstance *>::iterator ie = m_animations.end();
    for (; it != ie; ++it)
    {
        (*it)->m_manager = P_NULL;
        PDELETE(*it);
    }
}

void PAnimationManager::update()
{
    PList<PAnimationInstance *>::iterator it = m_animations.begin();
    PList<PAnimationInstance *>::iterator ie = m_animations.end();
    while (it != ie)
    {
        PAnimationInstance *instance = *it;

        // We have updated all active animations so can stop here.

        // As we can seek the postion when animation is paused,
        // we still need animations to be updated when paused.
        if (instance->getState() == P_ANIMATION_STOPPED)
        {
            break;
        }

        instance->update();
        
        if (instance->getState() == P_ANIMATION_STOPPED &&
            instance != m_animations.back())
        {
            PList<PAnimationInstance *>::iterator itt = it;
            ++itt;
            m_animations.erase(it);
            if (instance->isTemporary())
            {
                // We delete the animation instance if it is a temporary
                // one.
                PDELETE(instance);
            }
            else
            {
                m_animations.append(instance);
            }
            it = itt;
        }
        else
        {
            ++it;
        }
    }
}

void PAnimationManager::addAnimation(PAnimationInstance *animation)
{
    PASSERT(animation->getState() == P_ANIMATION_PLAYING ||
            animation->getState() == P_ANIMATION_READY);

    // Put the animation to the head of the list.
    PList<PAnimationInstance *>::iterator it = m_animations.find(animation);
    if (it != m_animations.end())
    {
        m_animations.erase(it);
    }
        
    m_animations.prepend(animation);
}

void PAnimationManager::repositionAnimation(PAnimationInstance *animation)
{
    // TODO: the list might not be a good data structure for storing
    // animations for animations will be inserted and removed
    // frequently.
    //
    PList<PAnimationInstance *>::iterator it = m_animations.find(animation);
    if (it != m_animations.end())
    {
        m_animations.erase(it);
        // Move the animation to the tail of the list. The heading nodes in
        // animation list are always active one while the tailing ones are 
        // either in pause or stop.
        m_animations.append(animation);
    }
    else
    {
        PLOG_WARNINGX(P_LOG_CHANNEL_ANIMATION, "Failed to find the animation in the manager");
    }
}

void PAnimationManager::removeAnimation(PAnimationInstance *animation)
{
    PList<PAnimationInstance *>::iterator it = m_animations.find(animation);
    if (it != m_animations.end())
    {
        m_animations.erase(it);
    }
    else
    {
        PLOG_WARNINGX(P_LOG_CHANNEL_ANIMATION, "Failed to find the animation in the manager");
    }
}

pfloat32 PAnimationManager::getCurrentTime() const
{
    return m_context->clock().currentTime();
}

pfloat32 PAnimationManager::getDeltaTime() const
{
    return m_context->clock().deltaTime();
}
