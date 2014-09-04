// panimationinstance.h 
// The animation. 
//
// Copyright 2012 - 2014 Future Interface. 
// This software is licensed under the terms of the MIT license.
//
// Hongwei Li lihw81@gmail.com
//

#include "panimationinstance.h"

#include <Paper3D/panimationmanager.h>
#include <Paper3D/panimationresource.h>
#include <Paper3D/pnode.h>

#include <PFoundation/peventmanager.h>
#include <PFoundation/pcontext.h>
#include <PFoundation/pevent.h>


PAnimationInstance::PAnimationInstance(PAnimationManager *manager)
{
    m_manager = manager;
    m_target = P_NULL;
    m_property = P_NULL;
    m_repeatMode = P_ANIMATION_REPEAT_MODE_DEFAULT;
    m_playedTimes = 0;
    m_playTimes = 1;
    m_animation = P_NULL;
    m_state = P_ANIMATION_STOPPED;
    m_temporary = false;
    m_callback = P_NULL;
}

PAnimationInstance::~PAnimationInstance()
{
    if (m_manager != P_NULL)
    {
        m_manager->removeAnimation(this);
        m_manager = P_NULL;
    }
    if (m_animation != P_NULL)
    {
        m_animation->release();
    }
}

void PAnimationInstance::setTarget(PNode *target, const pchar *propertyName)
{
    m_target = target;
    m_property = m_target->property(propertyName);
    PASSERT(m_property != P_NULL);
    if (m_property == P_NULL)
    {
        PLOG_ERROR("Failed to find such property value (%s) in target", propertyName);
        // Reset the target and value.
        m_target = P_NULL;
    }
}

void PAnimationInstance::setTarget(PAnimationCallback *callback)
{
    m_callback = callback;
}

void PAnimationInstance::setRepeatMode(PAnimationRepeatModeEnum repeatMode)
{
    m_repeatMode = repeatMode;
}

void PAnimationInstance::setPlayTimes(pint32 times)
{
    m_playTimes = times;
}

void PAnimationInstance::setAnimation(PAnimationResource *clip)
{
    PASSERT(clip != P_NULL);
    if (m_animation == clip)
    {
        return ;
    }

    // Release old animation clip.
    if (m_animation != P_NULL)
    {
        m_animation->release();
    }

    // Use the new clip.
    m_animation = clip;
    m_animation->retain();
}

void PAnimationInstance::setTemporary(pbool flag)
{
    m_temporary = flag;
}

void PAnimationInstance::setScaling(pfloat32 scaling)
{
    // FIXME: Temporarily we don't support negative scalings.
    PASSERT(scaling > 0);
    m_timeline.setScaling(scaling);
}

void PAnimationInstance::play(pfloat32 after)
{
    if (m_animation == P_NULL)
    {
        PLOG_WARNINGX(P_LOG_CHANNEL_ANIMATION, "The animation clip is NULL");
        return ;
    }
    if (m_manager == P_NULL)
    {
        PLOG_WARNINGX(P_LOG_CHANNEL_ANIMATION, "The animation has no parent manager");
        return ;
    }

    // If the animation has been played before but stopped now.
    if (m_state == P_ANIMATION_STOPPED)
    {
        m_startTime = after + m_manager->getCurrentTime();
        m_state = P_ANIMATION_READY;
    }
    // If the animation is resumed to play and we ignore the after parameter.
    else if (m_state == P_ANIMATION_PAUSED)
    {
        PASSERT(m_timeline.isPaused());
        m_timeline.resume();
        m_state = P_ANIMATION_READY;
    }
    // If the animation has not been added to animation.
    else if (m_state == P_ANIMATION_READY)
    {
        m_startTime = after + m_manager->getCurrentTime();
    }
    else
    {
        PLOG_WARNINGX(P_LOG_CHANNEL_ANIMATION, "The animation is already playing");
        return ;
    }
        
    m_manager->addAnimation(this);
}

void PAnimationInstance::pause()
{
    if (m_manager == P_NULL)
    {
        PLOG_WARNINGX(P_LOG_CHANNEL_ANIMATION, "The animation has no parent manager");
        return ;
    }
    m_timeline.pause();
    m_state = P_ANIMATION_PAUSED;
    m_manager->repositionAnimation(this);

    // Send the paused event
    PEvent *event = createEvent(P_EVENT__ANIMATION_PAUSED);
    event->queue(reinterpret_cast<PObject *>(P_NULL));
}

void PAnimationInstance::stop()
{
    m_state = P_ANIMATION_STOPPED;
    m_timeline.reset();
    if (!m_temporary)
    {
        m_manager->repositionAnimation(this);
    }
    else
    {
        // As stop() is called by users, we have no way to notify manager to 
        // destroy the instance.
        m_manager->removeAnimation(this);
        PDELETETHIS(this);
    }
    PEvent *event = createEvent(P_EVENT__ANIMATION_STOPPED);
    event->queue(reinterpret_cast<PObject *>(P_NULL));
}

void PAnimationInstance::rewind()
{
    if (m_manager == P_NULL)
    {
        PLOG_WARNINGX(P_LOG_CHANNEL_ANIMATION, "The animation has no parent manager");
        return ;
    }

    // Reset the local timeline to the beginning.
    m_timeline.reset();
    
    m_startTime = m_manager->getCurrentTime();

    if (isPaused())
    {
        // As the timeline is resetted, the timeline is active again (not paused).
        // We need to pause the timeline.
        m_timeline.pause();

        // FIXME: to reset the target's value to the first frame
        // even in the pause state.
    }
    PEvent *event = createEvent(P_EVENT__ANIMATION_REWOUND);
    event->queue(reinterpret_cast<PObject *>(P_NULL));
}

void PAnimationInstance::seek(pfloat32 time)
{
    if (m_manager == P_NULL)
    {
        PLOG_WARNINGX(P_LOG_CHANNEL_ANIMATION, "The animation has no parent manager");
        return ;
    }

    m_timeline.seek(time);
}

void PAnimationInstance::update()
{
    PASSERT(m_manager != P_NULL);

    if ((m_target == P_NULL && m_callback == P_NULL) ||
         m_animation == P_NULL || m_manager == P_NULL)
    {
        return ;
    }

    // If the animation is not started.
    if (m_manager->getCurrentTime() < m_startTime)
    {
        return ;
    }

    if (m_state == P_ANIMATION_READY)
    {
        m_state = P_ANIMATION_PLAYING;
        PEvent *event = createEvent(P_EVENT__ANIMATION_PLAYING);
        event->queue(reinterpret_cast<PObject *>(P_NULL));
    }
       
    if (m_state == P_ANIMATION_PLAYING)
    {
        m_timeline.update(m_manager->getDeltaTime());
    }
    
    // Fetch the local time from the local time line.
    pfloat32 time = m_timeline.getElapsedTime();

    // When reaching the end of the animation clip.
    if (time > m_animation->duration() || time < -m_animation->duration())
    {
        pfloat32 difference;
        if (time > m_animation->duration())
        {
            difference = time - m_animation->duration();
        }
        else
        {
            difference = -m_animation->duration() - time;
        }

        m_playedTimes++;

        if (m_playedTimes < m_playTimes || m_playTimes == -1)
        {
            // Reset the local timeline and advance the start time to
            // the new position.
            m_timeline.reset();

            // Flip the scaling ratio of the timeline if it is in ping pong
            // repeat mode.
            if (m_repeatMode == P_ANIMATION_REPEAT_PINGPONG)
            {
                m_timeline.setScaling(m_timeline.getScaling() * -1.0f);
            }
            m_timeline.update(difference);
        
            time = m_timeline.getElapsedTime();
        }
        else
        {
            time = m_timeline.getElapsedTime();

            // Stop this animation.
            m_state = P_ANIMATION_STOPPED;
            m_timeline.reset();
        }
    }
    
    // It must be in the ping pong mode and we should
    // reverse the time.
    if (time < 0 && m_repeatMode == P_ANIMATION_REPEAT_PINGPONG)
    {
        time = m_animation->duration() + time;
    }

    if (m_target != P_NULL)
    {
        m_animation->animate(time, m_target, m_property);
    }
    
    if (m_callback != P_NULL)
    {
        m_animation->animate(time, m_callback);
    }
}
    
PEvent *PAnimationInstance::createEvent(PEventTypeEnum event)
{
    PContext *context = m_manager->getContext();
    // TODO: replace the tailing P_NULL with this after PAnimationInstance
    // is refactored to a subclass of PObject.
    return context->eventManager()->createEvent(event, P_NULL);
}
