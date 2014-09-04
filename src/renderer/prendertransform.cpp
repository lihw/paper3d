// prendertransform.cpp
// Compute the transform matrices
//
// Copyright 2012 - 2014 Future Interface . 
// This software is licensed under the terms of the MIT license.
//
// Hongwei Li lihw81@gmail.com
//

#include "prendertransform.h"


PRenderTransform::PRenderTransform(PDrawable *drawable, PCamera *camera)
{
    PASSERT(drawable != P_NULL);
    PASSERT(camera != P_NULL);

    m_drawable = drawable;
    m_camera   = camera;
}

PRenderTransform::~PRenderTransform()
{
}

PMatrix4x4 PRenderTransform::mvpMatrix() const
{
    pfloat32 mv[16];
    m_drawable->calculateModelCameraMatrix(viewMatrix(), mv);

    PMatrix4x4 ret;
    pMatrix4x4Multiply(projectionMatrix().m_m, mv, ret.m_m);
    
    return ret;
}

PMatrix4x4 PRenderTransform::modelviewMatrix() const
{
    PMatrix4x4 ret;
    m_drawable->calculateModelCameraMatrix(viewMatrix(), ret.m_m);

    return ret;
}

PMatrix3x3 PRenderTransform::normalMatrix() const
{
    PMatrix3x3 ret;
    pMatrix3x3InverseTransposeMatrix4x4(modelMatrix().m_m, ret.m_m);
    return ret;
}
