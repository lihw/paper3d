// panimationinstance.h 
// The animation. 
//
// Copyright 2012 - 2014 Future Interface. 
// This software is licensed under the terms of the MIT license.
//
// Hongwei Li lihw81@gmail.com
//

#ifndef PANIMATIONINSTANCE_H
#define PANIMATIONINSTANCE_H

#include <PFoundation/pabstractproperty.h>
#include <PFoundation/peventtype.h>
#include <PFoundation/ptimeline.h>
#include <PFoundation/plog.h>
#include <PFoundation/passert.h>



class PNode;
class PAnimationResource;
class PAnimationManager;
class PAbstractProperty;
class PEvent;

enum PAnimationRepeatModeEnum
{
    P_ANIMATION_REPEAT_NORMAL,
    P_ANIMATION_REPEAT_PINGPONG,

    P_ANIMATION_REPEAT_MODE_DEFAULT = P_ANIMATION_REPEAT_NORMAL,
    P_ANIMATION_REPEAT_MODE_FIRST = P_ANIMATION_REPEAT_NORMAL,
    P_ANIMATION_REPEAT_MODE_LAST = P_ANIMATION_REPEAT_PINGPONG,

    P_ANIMATION_REPEAT_MODE_NUMBER = P_ANIMATION_REPEAT_MODE_LAST - P_ANIMATION_REPEAT_MODE_FIRST + 1
};

enum PAnimationStateEnum 
{
    P_ANIMATION_READY,
    P_ANIMATION_STOPPED,
    P_ANIMATION_PLAYING,
    P_ANIMATION_PAUSED,
    
    P_ANIMATION_STATE_DEFUALT = P_ANIMATION_READY,

    P_ANIMATION_STATE_FIRST = P_ANIMATION_READY,
    P_ANIMATION_STATE_LAST = P_ANIMATION_PAUSED,
    P_ANIMATION_STATE_NUMBER = P_ANIMATION_STATE_LAST - P_ANIMATION_STATE_FIRST + 1,
};

class P_DLLEXPORT PAnimationCallback
{
public:
    PAnimationCallback() {}
    virtual ~PAnimationCallback() {}

    // t is the normalized time during the animation (0, 1).
    virtual void animate(pfloat32 t, const PVariant &value)
    {
    };
};


// TODO: inherite PAnimationInstace from a PObject so that can be an event sender.
class P_DLLEXPORT PAnimationInstance
{
    friend class PAnimationManager;

public:
    PAnimationInstance(PAnimationManager *manager);
    ~PAnimationInstance();

    // For skeleton, morphing and sprite animation, the propertyName can be omitted.
    void setTarget(PNode *target, const pchar *propertyName);
    void setTarget(PAnimationCallback *callback);
    void setRepeatMode(PAnimationRepeatModeEnum repeatMode);
    void setPlayTimes(pint32 times);
    void setAnimation(PAnimationResource *animation);
    void setTemporary(pbool flag);
    void setScaling(pfloat32 scaling);

    P_INLINE PNode *getTargetObject() const
    { return m_target; }
    P_INLINE const pchar *getTargetProperty() const
    { return m_property != P_NULL? m_property->name() : P_NULL; }
    P_INLINE PAnimationRepeatModeEnum getRepeatMode() const
    { return m_repeatMode; }
    P_INLINE puint32 getPlayTimes() const
    { return m_playTimes; }
    P_INLINE PAnimationStateEnum getState() const
    { return m_state; }
    P_INLINE bool isPlaying() const
    { return m_state == P_ANIMATION_PLAYING; }
    P_INLINE bool isStopped() const
    { return m_state == P_ANIMATION_STOPPED; }
    P_INLINE bool isPaused() const
    { return m_state == P_ANIMATION_PAUSED; }
    P_INLINE bool isTemporary() const
    { return m_temporary; }

    // When the after is an negative value, we will start the animation
    // immediately at corresponding position (as the animation has 
    // elapsed after milliseconds).
    void play(pfloat32 after);
    void pause();
    void stop();
    void rewind();
    void seek(pfloat32 time);

private:
    void update();
    // A helper function to create an animation event.
    PEvent *createEvent(PEventTypeEnum event);

private:
    PAnimationManager        *m_manager;
    PTimeLine                 m_timeline;
    PNode                    *m_target;
    PAbstractProperty        *m_property;
    PAnimationCallback       *m_callback;
    PAnimationRepeatModeEnum  m_repeatMode;
    pint32                    m_playTimes;
    pint32                    m_playedTimes; // Only effective in repeat mode.
    // TODO: support multiple of clips and animation blending.
    PAnimationResource       *m_animation;
    PAnimationStateEnum       m_state;
    pfloat32                  m_startTime; // The start time of this animation.
    pbool                     m_temporary;
};


#endif // !PANIMATIONINSTANCE_H

