// pskeleton.h 
// The data structures used in skeleton animation.
//
// Copyright 2012 - 2014 Future Interface. 
// This software is licensed under the terms of the MIT license.
//
// Hongwei Li lihw81@gmail.com
//

#ifndef PSKELETON_H
#define PSKELETON_H

#include <PFoundation/pquaternion.h>


struct PSkeletonJoint
{
    puint32      m_index;
    PMatrix4x4   m_inverseBindPose;
};

struct PSkeleton
{
    PArray<PSkeletonJoint> m_joints;
};

struct PJointTransformation
{
    PQuaternion m_rotation;
    //PVector4 m_translation;
    //PVector4 m_scaling;
};

struct PPose
{
    PArray<PJointTransformation> m_jointPoses;
};


#endif // !PSKELETON_H
