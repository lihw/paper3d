// prendertransform.h
// Compute the transform matrices
//
// Copyright 2012 - 2014 Future Interface . 
// This software is licensed under the terms of the MIT license.
//
// Hongwei Li lihw81@gmail.com
//

#ifndef PRENDERTRANSFORM_H
#define PRENDERTRANSFORM_H

#include <Paper3D/pcamera.h>
#include <Paper3D/pdrawable.h>

#include <PFoundation/pmatrix3x3.h>
#include <PFoundation/pmatrix4x4.h>



class P_DLLEXPORT PRenderTransform
{
public:
    PRenderTransform(PDrawable *drawable, PCamera *camera);
    ~PRenderTransform();

    P_INLINE const PMatrix4x4 &modelMatrix() const { return m_drawable->worldTransform(); }
    P_INLINE const PMatrix4x4 &viewMatrix() const { return m_camera->cameraTransform(); }
    P_INLINE const PMatrix4x4 &projectionMatrix() const { return m_camera->projection().toMatrix4x4(); }

    PMatrix4x4 modelviewMatrix() const;
    PMatrix4x4 mvpMatrix() const;
    PMatrix3x3 normalMatrix() const;

private:
    PDrawable *m_drawable;
    PCamera   *m_camera;
};



#endif // !PRENDERTRANSFORM_H
