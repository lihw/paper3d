// pkeyframe.cpp
// The Keyframe which constitute the animation clip.
//
// Copyright 2012 - 2014 Future Interface. 
// This software is licensed under the terms of the MIT license.
//
// Hongwei Li lihw81@gmail.com
//

#include "pkeyframe.h"

#include <Paper3D/panimationresource.h>

#include <PFoundation/passert.h>
#include <PFoundation/plog.h>


PAbstractKeyframe::PAbstractKeyframe(pfloat32 time, PAnimationResource *animation)
{
    m_time = time;

    PASSERT(animation != P_NULL);
    m_animation = animation;
}

PAbstractKeyframe::~PAbstractKeyframe()
{
    if (m_animation != P_NULL)
    {
        m_animation->removeKeyframe(this);
    }
}

PValueKeyframe::PValueKeyframe(pfloat32 time, PAnimationResource *animation)
    : PAbstractKeyframe(time, animation)
{
    PASSERT(m_animation->animationType() == P_ANIMATION_DATAVALUE);
    if (m_animation->animationType() != P_ANIMATION_DATAVALUE)
    {
        PLOG_ERRORX(P_LOG_CHANNEL_ANIMATION, "PValueKeyframe can be only bound to a datavalue animation clip");
    }
}

PValueKeyframe::~PValueKeyframe()
{
}

void PValueKeyframe::setValue(const void *value)
{
    setValue(*(reinterpret_cast<const PVariant*>(value)));
}

void PValueKeyframe::setValue(const PVariant &value)
{
    m_value = value;
}
    
PVariant PValueKeyframe::interpolate(const PValueKeyframe &right, pfloat32 t) const
{
    return interpolate(*this, right, t);
}
    
PVariant PValueKeyframe::interpolate(const PValueKeyframe &left, 
    const PValueKeyframe &right, pfloat32 t)
{
    PVariant result = PVariant::interpolate(left.getValue(), right.getValue(), t);
    return result;
}

PSpriteKeyframe::PSpriteKeyframe(pfloat32 time, PAnimationResource *animation)
    : PAbstractKeyframe(time, animation)
{
    PASSERT(animation->animationType() == P_ANIMATION_SPRITE);
    if (animation->animationType() != P_ANIMATION_SPRITE)
    {
        PLOG_ERRORX(P_LOG_CHANNEL_ANIMATION, "PSpriteKeyframe can be only bound to a sprite animation clip");
    }
}

PSpriteKeyframe::~PSpriteKeyframe()
{
}

void PSpriteKeyframe::setValue(const void *value)
{
    setValue(*(reinterpret_cast<const PVector4*>(value)));
}

void PSpriteKeyframe::setValue(const PVector4 &value)
{
    m_value = value;
}

PVector4 PSpriteKeyframe::interpolate(const PSpriteKeyframe &left,
        const PSpriteKeyframe &right, pfloat32 t)
{
    if (t < 0.5f)
    {
        return left.getValue();
    }
    return right.getValue();
}

PVector4 PSpriteKeyframe::interpolate(const PSpriteKeyframe &right, pfloat32 t) const
{
    return interpolate(*this, right, t);
}

PSkeletonKeyframe::PSkeletonKeyframe(pfloat32 time, PAnimationResource *animation)
    : PAbstractKeyframe(time, animation)
{
    PASSERT(m_animation->animationType() == P_ANIMATION_SKELETON);
    if (m_animation->animationType() != P_ANIMATION_SKELETON)
    {
        PLOG_ERRORX(P_LOG_CHANNEL_ANIMATION, "PSkeletonKeyframe can be only bound to a skeleton animation clip");
    }
}

PSkeletonKeyframe::~PSkeletonKeyframe()
{
}

void PSkeletonKeyframe::setValue(const void *value)
{
    PASSERT_NOTIMPLEMENTED();
}


PMorphingKeyframe::PMorphingKeyframe(pfloat32 time, PAnimationResource *animation)
    : PAbstractKeyframe(time, animation)
{
    PASSERT(m_animation->animationType() == P_ANIMATION_MORPHING);
    if (m_animation->animationType() != P_ANIMATION_MORPHING)
    {
        PLOG_ERRORX(P_LOG_CHANNEL_ANIMATION, "PMorphingKeyframe can be only bound to a morphing animation clip");
    }
}

PMorphingKeyframe::~PMorphingKeyframe()
{
}

void PMorphingKeyframe::setValue(const void *value)
{
    PASSERT_NOTIMPLEMENTED();
}


