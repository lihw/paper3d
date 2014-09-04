// panimationresource.cpp
// The animation clip.
//
// Copyright 2012 - 2014 Future Interface. 
// This software is licensed under the terms of the MIT license.
//
// Hongwei Li lihw81@gmail.com
//

#include "panimationresource.h"

#include <Paper3D/pkeyframe.h>
#include <Paper3D/panimationinstance.h>
#include <Paper3D/presourcemanager.h>

#include <PFoundation/pabstractproperty.h>
#include <PFoundation/pcontext.h>
#include <PFoundation/parchivefile.h>
#include <PFoundation/pmathutility.h>
#include <PFoundation/pinputstream.h>

#include "cache/panimationloader_private.h"


bool PKeyframeLess::operator()(const PAbstractKeyframe *left, const PAbstractKeyframe *right) const
{
    return left->time() < right->time();
}

PAnimationResource::PAnimationResource(PResourceManager *parent, const pchar *id, PAnimationTypeEnum type) 
    : PResourceObject(id, parent, P_RESOURCE_CPU)
{
    m_animationType     = type;
    m_duration          = P_MAX_FLOAT32;
    m_interpolation     = P_ANIMATION_INTERPOLATION_LINEAR;
    m_storageType       = P_RESOURCE_FROM_MEMORY;
    m_available         = true;
}

PAnimationResource::~PAnimationResource()
{
    KeyframeIterator it = m_keyframes.begin();
    KeyframeIterator ie = m_keyframes.end();
    while (it != ie)
    {
        PAbstractKeyframe *keyframe = *it;
        keyframe->m_animation = P_NULL;
        PDELETE(keyframe);
        ++it;
    }
    m_keyframes.clear();
}

PAbstractKeyframe *PAnimationResource::createKeyframe(pfloat32 time)
{
    if (time > m_duration)
    {
        m_duration = time + (1.0f / 30.0f);
    }

    PAbstractKeyframe *ret;
    switch (m_animationType)
    {
        case P_ANIMATION_DATAVALUE:
            ret = PNEW(PValueKeyframe(time, this));
            break;
        case P_ANIMATION_SPRITE:
            ret = PNEW(PSpriteKeyframe(time, this));
            break;
        case P_ANIMATION_SKELETON:
            ret = PNEW(PSkeletonKeyframe(time, this));
            break;
        case P_ANIMATION_MORPHING:
            ret = PNEW(PMorphingKeyframe(time, this));
            break;
        default:
            PLOG_ERRORX(P_LOG_CHANNEL_ANIMATION, "unknown animation clip type");
            return P_NULL;
    }

    // Insert the Keyframe into the clip.
    m_keyframes.insert(ret);

    return ret;
}
    

void PAnimationResource::removeKeyframe(PAbstractKeyframe *keyframe)
{
    KeyframeIterator it = m_keyframes.find(keyframe);
    if (it != m_keyframes.end())
    { 
        m_keyframes.erase(it);
    }
}

void PAnimationResource::setInterpolation(PAnimationInterpolationEnum interpolation)
{
    m_interpolation = interpolation;
}

void PAnimationResource::setDuration(pfloat32 milliseconds)
{
    if (!m_keyframes.isEmpty())
    {
        pfloat32 lastKeyframeTime = m_keyframes.back()->time();
        if (milliseconds < lastKeyframeTime)
        {
            milliseconds = lastKeyframeTime + (1.0f / 30.0f);
        }
    }

    m_duration = milliseconds;
}

void PAnimationResource::animate(pfloat32 time, PNode *target, PAbstractProperty *property)
{
    if (m_keyframes.isEmpty())
    {
        PLOG_WARNINGX(P_LOG_CHANNEL_ANIMATION, "animation clip %s is invalid", m_id);
        return ;
    }

    // TODO: a more effective way to locate two neighboring frames. 
    // 1) save the current neighboring two keyframes and update them when the new time arrives
    // 2) binary search
    // Find two neighboring keyframes at that point of time.
    KeyframeIterator it = m_keyframes.begin();
    KeyframeIterator ie = m_keyframes.end();
    while (it != ie && (*it)->time() < time)
    {
        ++it;
    }

    PAbstractKeyframe *left = P_NULL;
    PAbstractKeyframe *right = P_NULL;

    if (it != ie)
    {
        right = *it;
        if (it != m_keyframes.begin())
        {
            --it;
        }
        left = *it;
    }
    else
    {
        left = right = m_keyframes.back();
    }

    if (right == left)
    {
        // TODO: Use virtual functions instead of switch
        switch (m_animationType)
        {
            case P_ANIMATION_DATAVALUE:
                {
                    PValueKeyframe *leftKeyframe = dynamic_cast<PValueKeyframe *>(left);
                    property->setValue(leftKeyframe->getValue());
                }
                break;
            case P_ANIMATION_SKELETON:
            case P_ANIMATION_MORPHING:
            case P_ANIMATION_SPRITE:
            default:
                PASSERT_NOTREACHABLE("Unknown animation type.");
                break;
        }
    }
    else
    {
        // Compute the interpolation ratio.
        // TODO: currently no smooth interpolation is supported, like bezeir and catmull-rom.
        pfloat32 t = (time - left->time()) / (right->time() - left->time());

        switch (m_interpolation)
        {
            case P_ANIMATION_INTERPOLATION_NEAREST:
                if (t < 0.5f) 
                {
                    t = 0;
                }
                else
                {
                    t = 1;
                }
                break;
            case P_ANIMATION_INTERPOLATION_LINEAR:
                break;
            case P_ANIMATION_INTERPOLATION_CUBIC:
                // One of cubic Hermite spline basis functions
                t = -2 * t * t * t + 3 * t * t;
            case P_ANIMATION_INTERPOLATION_CATMULL:
                PASSERT_NOTREACHABLE("Unknown interpolation type.");
                break;
            // TODO: user-defined interpolation 
            default:
                PASSERT_NOTREACHABLE("Unknown interpolation type.");                  
                break;
        }

        switch (m_animationType)
        {
            case P_ANIMATION_DATAVALUE:
                {
                    PValueKeyframe *leftKeyframe = dynamic_cast<PValueKeyframe *>(left);
                    PValueKeyframe *rightKeyframe = dynamic_cast<PValueKeyframe *>(right);
                    PASSERT(leftKeyframe != P_NULL && rightKeyframe != P_NULL);
                    property->setValue(leftKeyframe->interpolate(*rightKeyframe, t));
                }
                break;
            case P_ANIMATION_SKELETON:
            case P_ANIMATION_MORPHING:
            case P_ANIMATION_SPRITE:
            default:
                PASSERT_NOTREACHABLE("Unknown animation type.");
                break;
        }
    }
}

void PAnimationResource::animate(pfloat32 time, PAnimationCallback *callback)
{
    if (m_keyframes.isEmpty())
    {
        PLOG_WARNINGX(P_LOG_CHANNEL_ANIMATION, "animation clip %s is invalid", m_id);
        return ;
    }

    // TODO: a more effective way to locate two neighboring frames. 
    // 1) save the current neighboring two keyframes and update them when the new time arrives
    // 2) binary search
    // Find two neighboring keyframes at that point of time.
    KeyframeIterator it = m_keyframes.begin();
    KeyframeIterator ie = m_keyframes.end();
    while (it != ie && (*it)->time() < time)
    {
        ++it;
    }

    PAbstractKeyframe *left = P_NULL;
    PAbstractKeyframe *right = P_NULL;

    pfloat32 t = 0;
    if (it != ie)
    {
        right = *it;
        if (it != m_keyframes.begin())
        {
            --it;
        }
        left = *it;
    }
    else
    {
        t = 1.0f;
        left = right = m_keyframes.back();
    }

    if (right == left)
    {
        switch (m_animationType)
        {
            case P_ANIMATION_DATAVALUE:
                {
                    PValueKeyframe *leftKeyframe = dynamic_cast<PValueKeyframe *>(left);
                    callback->animate(t, leftKeyframe->getValue());
                }
                break;
            case P_ANIMATION_SKELETON:
            case P_ANIMATION_MORPHING:
            case P_ANIMATION_SPRITE:
            default:
                PASSERT_NOTREACHABLE("Unknown animation type.");
                break;
        }
    }
    else
    {
        // Compute the interpolation ratio.
        // TODO: currently no smooth interpolation is supported, like bezeir and catmull-rom.
        t = (time - left->time()) / (right->time() - left->time());

        switch (m_interpolation)
        {
            case P_ANIMATION_INTERPOLATION_NEAREST:
                if (t < 0.5f) 
                {
                    t = 0;
                }
                else
                {
                    t = 1;
                }
                break;
            case P_ANIMATION_INTERPOLATION_LINEAR:
                break;
            case P_ANIMATION_INTERPOLATION_CUBIC:
                // One of cubic Hermite spline basis functions
                t = -2 * t * t * t + 3 * t * t;
            case P_ANIMATION_INTERPOLATION_CATMULL:
                PASSERT_NOTREACHABLE("Unknown animation interpolation type.");
                break;
            // TODO: user-defined interpolation 
            default:
                PASSERT_NOTREACHABLE("Unknown animation interpolation type.");
                break;
        }

        switch (m_animationType)
        {
            case P_ANIMATION_DATAVALUE:
                {
                    PValueKeyframe *leftKeyframe = dynamic_cast<PValueKeyframe *>(left);
                    PValueKeyframe *rightKeyframe = dynamic_cast<PValueKeyframe *>(right);
                    PASSERT(leftKeyframe != P_NULL && rightKeyframe != P_NULL);
                    callback->animate(t, leftKeyframe->interpolate(*rightKeyframe, t));
                }
                break;
            case P_ANIMATION_SKELETON:
            case P_ANIMATION_MORPHING:
            case P_ANIMATION_SPRITE:
            default:
                PASSERT_NOTREACHABLE("Unknown animation type.");
                break;
        }
    }
}

pbool PAnimationResource::load()
{
    PInputStream inputStream;
    PArchiveFile* archive = parent()->archive();
    if (archive->createInputStream(m_id, &inputStream))
    {
        PAnimationLoader loader;
        if (loader.read(&inputStream, this))
        {
            if (archive->isPathInArchive(m_id))
            {
                m_storageType = P_RESOURCE_FROM_ARCHIVE;
            }
            else
            {
                m_storageType = P_RESOURCE_FROM_FILE;
            }

            return true;
        }
    }

    return false;
}

pbool PAnimationResource::restoreResource()
{
    if (m_storageType == P_RESOURCE_FROM_ARCHIVE ||
        m_storageType == P_RESOURCE_FROM_FILE)
    {
        if (load())
        {
            return true;
        }
    }
    else if (m_storageType == P_RESOURCE_FROM_MEMORY)
    {
        // TODO: read in the temporary file written at swapping out.
        return true;
    }
    else
    {
        PLOG_ERROR("Unknown storage type of animation clip data %s", m_id);
    }

    return false;
}

void PAnimationResource::discardResource()
{
    if (m_storageType == P_RESOURCE_FROM_MEMORY)
    {
        // TODO: output the source to the temporary file.
        return;
    }

    KeyframeIterator it = m_keyframes.begin();
    KeyframeIterator ie = m_keyframes.end();
    while (it != ie)
    {
        PAbstractKeyframe *keyframe = *it;
        keyframe->m_animation = P_NULL;
        PDELETE(keyframe);
        ++it;
    }
    m_keyframes.clear();
}
