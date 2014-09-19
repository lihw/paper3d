// panimationresource.h 
// The animation clip.
//
// Copyright 2012 - 2014 Future Interface. 
// This software is licensed under the terms of the MIT license.
//
// Hongwei Li lihw81@gmail.com
//

#ifndef PANIMATIONRESOURCE_H
#define PANIMATIONRESOURCE_H

#include <Paper3D/presourceobject.h>

#include <PFoundation/pvariant.h>
#include <PFoundation/pqueue.h>

// TODO: move to animation folder and rename to PAnimationResource

class PAbstractProperty;
class PAnimationCallback;

enum PAnimationTypeEnum
{
    P_ANIMATION_DATAVALUE,
    P_ANIMATION_SPRITE,
    P_ANIMATION_SKELETON,
    P_ANIMATION_MORPHING,

    P_ANIMATION_TYPE_DEFAULT = P_ANIMATION_DATAVALUE,
    P_ANIMATION_TYPE_FIRST = P_ANIMATION_DATAVALUE,
    P_ANIMATION_TYPE_LAST = P_ANIMATION_MORPHING,

    P_ANIMATION_NUMBER = P_ANIMATION_TYPE_LAST - P_ANIMATION_TYPE_FIRST + 1,
};

enum PAnimationInterpolationEnum
{
    P_ANIMATION_INTERPOLATION_NEAREST,
    P_ANIMATION_INTERPOLATION_LINEAR,
    P_ANIMATION_INTERPOLATION_CUBIC,
    P_ANIMATION_INTERPOLATION_CATMULL,
    
    P_ANIMATION_INTERPOLATION_DEFAULT = P_ANIMATION_INTERPOLATION_LINEAR,

    P_ANIMATION_INTERPOLATION_FIRST = P_ANIMATION_INTERPOLATION_NEAREST,
    P_ANIMATION_INTERPOLATION_LAST = P_ANIMATION_INTERPOLATION_CATMULL,
    P_ANIMATION_INTERPOLATION_NUMBER = P_ANIMATION_INTERPOLATION_LAST - P_ANIMATION_INTERPOLATION_FIRST + 1,
};


class PAbstractKeyframe;
class PNode;


class PKeyframeLess 
{
public:
    pbool operator()(const PAbstractKeyframe *left, const PAbstractKeyframe *right) const;
};

class P_DLLEXPORT PAnimationResource: public PResourceObject
{
    friend class PResourceManager;
    friend class PAbstractKeyframe;
    friend class PAnimationLoader;

    PAnimationResource(const PAnimationResource &other) : PResourceObject(P_NULL, P_NULL, P_RESOURCE_TYPE_UNKNOWN) {}
    void operator=(const PAnimationResource &other) {}

public:
    PAbstractKeyframe* createKeyframe(pfloat32 time);
    
    P_INLINE puint32 numberOfKeyframes() const
    { return m_keyframes.size(); }
    P_INLINE PAnimationInterpolationEnum interpolation() const
    { return m_interpolation; }
    P_INLINE pfloat32 duration() const
    { return m_duration; }
    P_INLINE PAnimationTypeEnum animationType() const
    { return m_animationType; }
    
    void setInterpolation(PAnimationInterpolationEnum interpolation);
    void setDuration(pfloat32 milliseconds);

    // TODO: reduce the code duplication in the two functions below.
    void animate(pfloat32 time, PNode *target, PAbstractProperty *property);
    void animate(pfloat32 time, PAnimationCallback *callback);

protected:
    virtual void discardResource();
    virtual pbool restoreResource();

private:
    PAnimationResource(PResourceManager *parent, const pchar *id, PAnimationTypeEnum type = P_ANIMATION_DATAVALUE); 
    virtual ~PAnimationResource();
    
    pbool load();

    void removeKeyframe(PAbstractKeyframe *keyframe);

private:
    typedef PQueue<PAbstractKeyframe*, PKeyframeLess> KeyframeQueue;
    typedef KeyframeQueue::iterator                   KeyframeIterator;

    KeyframeQueue                m_keyframes;
    pfloat32                     m_duration;
    PAnimationTypeEnum           m_animationType;
    PAnimationInterpolationEnum  m_interpolation;
    PResourceStorageTypeEnum     m_storageType;
};

#endif // !PANIMATIONRESOURCE_H
