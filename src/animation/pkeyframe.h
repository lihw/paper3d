// pkeyframe.h 
// The Keyframe which constitute the animation clip.
//
// Copyright 2012 - 2014 Future Interface. 
// This software is licensed under the terms of the MIT license.
//
// Hongwei Li lihw81@gmail.com
//

#ifndef PKEYFRAME_H
#define PKEYFRAME_H

#include <Paper3D/panimationresource.h>
#include <Paper3D/pskeleton.h>

class P_DLLEXPORT PAbstractKeyframe 
{
    friend class PAnimationResource;

public:
    PAbstractKeyframe(pfloat32 time, PAnimationResource *animation);
    virtual ~PAbstractKeyframe();
    PAbstractKeyframe(const PAbstractKeyframe &other) {}
    void operator=(const PAbstractKeyframe &other) {}

public:
    P_INLINE pfloat32 time() const { return m_time; }
    P_INLINE PAnimationResource *animation() const { return m_animation; }
    P_INLINE PAnimationTypeEnum type() const { return m_animation->animationType(); }

    virtual void setValue(const void *value) = 0;
    
protected:
    pfloat32            m_time;
    PAnimationResource *m_animation;
};

class P_DLLEXPORT PValueKeyframe : public PAbstractKeyframe
{
    friend class PAnimationResource;

    PValueKeyframe(pfloat32 time, PAnimationResource *animation);

public:
    virtual ~PValueKeyframe();
    virtual void setValue(const void *value);

    void setValue(const PVariant &value);
    P_INLINE const PVariant &getValue() const
    { return m_value; }
    
    PVariant interpolate(const PValueKeyframe &right, pfloat32 t) const;
    
    static PVariant interpolate(const PValueKeyframe &left, 
        const PValueKeyframe &right, pfloat32 t);

private:
    PVariant m_value;
};

class P_DLLEXPORT PSpriteKeyframe : public PAbstractKeyframe
{
    friend class PAnimationResource;

    PSpriteKeyframe(pfloat32 time, PAnimationResource *clip);

public:
    virtual ~PSpriteKeyframe();
    virtual void setValue(const void *value);
    void setValue(const PVector4 &value);
    const PVector4 &getValue() const
    { return m_value; }

private:
    static PVector4 interpolate(const PSpriteKeyframe &left,
            const PSpriteKeyframe &right, pfloat32 t);
    PVector4 interpolate(const PSpriteKeyframe &right, pfloat32 t) const;

private:
    PVector4 m_value; // Texture coordinate offset and scaling
};

class P_DLLEXPORT PSkeletonKeyframe : public PAbstractKeyframe
{
    // TODO:
    friend class PAnimationResource;

    PSkeletonKeyframe(pfloat32 time, PAnimationResource *clip);

public:
    virtual ~PSkeletonKeyframe();
    virtual void setValue(const void *value);
    // TODO:
    // Set the Keyframe's value

private:
    // TODO: interpolate
    

private:
    PPose m_pose;
};

class P_DLLEXPORT PMorphingKeyframe : public PAbstractKeyframe
{
    friend class PAnimationResource;

    PMorphingKeyframe(pfloat32 time, PAnimationResource *clip);

public:
    virtual ~PMorphingKeyframe();
    virtual void setValue(const void *value);
    // TODO:
public:

};


#endif // !PKEYFRAME_H
