// panimationmanager.h 
// The animation manager which manages all animations
// in the context. 
//
// Copyright 2012 - 2014 Future Interface. 
// This software is licensed under the terms of the MIT license.
//
// Hongwei Li lihw81@gmail.com
//

#ifndef PANIMATION_MANAGER_H
#define PANIMATION_MANAGER_H

#include <PFoundation/plist.h>

class PContext;
class PAnimationInstance;

class P_DLLEXPORT PAnimationManager 
{
    friend class PAnimationInstance;

public:
    PAnimationManager(PContext *context);
    ~PAnimationManager();

    void update();

    pfloat32 getCurrentTime() const;
    pfloat32 getDeltaTime() const;

    P_INLINE PContext *getContext() const { return m_context; }

private:
    void addAnimation(PAnimationInstance *animation);
    // remove the animation from the list.
    void removeAnimation(PAnimationInstance *animation);
    // put the animation to the end of the list.
    void repositionAnimation(PAnimationInstance *animation);

private:
    PList<PAnimationInstance *> m_animations; // animations either in ready, pause or play mode.
    PContext *m_context;
};


#endif // !PANIMATION_MANAGER_H
