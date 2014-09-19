// panimationloader.cpp
// The animation loader.
//
// Copyright 2012 - 2014 Future Interface. 
// This software is licensed under the terms of the MIT license.
//
// Hongwei Li lihw81@gmail.com 
//

#include "panimationloader_private.h"

#include <Paper3D/panimationresource.h>
#include <Paper3D/pkeyframe.h>

#include <PFoundation/pinputstream.h>

#define CHUNK_ID_ANIMATION_SCENE        0x01004b43
#define CHUNK_ID_ANIMATION_STACK        0x01014b43
#define CHUNK_ID_ANIMATION_LAYER        0x01024b43
#define CHUNK_ID_ANIMATION_NODE         0x01034b43
#define CHUNK_ID_ANIMATION_CURVE_NODE   0x01044b43
#define CHUNK_ID_ANIMATION_KEYFRAME     0x01054b43


PAnimationLoader::PAnimationLoader()
{
}

PAnimationLoader::~PAnimationLoader()
{

}

pbool PAnimationLoader::read(PInputStream *inputStream, PAnimationResource *animation)
{    
    PASSERT(inputStream != P_NULL);
    if (inputStream == P_NULL)
    {
        return false;
    }

    pint32 uselessData;
    pint32 duration = 0;
    pint32 keyframeSize = 0;
    pint32 propertyType;
    pint32 componentType;
    if (!inputStream->readInt32(&propertyType) || //propertyType
        !inputStream->readInt32(&componentType) ||  //componentType
        !inputStream->readInt32(&duration) || 
        !inputStream->readInt32(&uselessData) || //interpolationType
        !inputStream->readInt32(&uselessData) || //constantMode
        !inputStream->readInt32(&uselessData) || //tangentMode
        !inputStream->readInt32(&uselessData) || //tangentWeightMode
        !inputStream->readInt32(&uselessData) || // tangentVelocityMode
        !inputStream->readInt32(&keyframeSize)) // keyframe number
    {
        PLOG_ERROR("Corrupted animation file header");
        return false;
    }

    animation->setDuration((pfloat32)duration);

    for (pint32 i = 0; i < keyframeSize; ++i)
    {
        pint32 keyTime;

        if (!inputStream->readInt32(&keyTime))
        {
            goto err;
        }

        PAbstractKeyframe *keyframe = animation->createKeyframe((pfloat32)keyTime);
        pfloat32 keyValue;
        if (!inputStream->readFloat32(&keyValue))
        {
            goto err;
        }

        if (propertyType == 1)
        {
            keyValue = keyValue / 180.0f * 3.1415f;
        }
        else if (propertyType == 0)
        {
            
        }
        PVariant v(keyValue);
        keyframe->setValue(&v);
    }

    return true;

err:
    PLOG_ERROR("Corrupted keyframe data");
    return false;
}
