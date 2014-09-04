// pcameramisc.h
// Some misc functions related to camera
//
// Copyright 2013 Cooee. 
// This software is licensed under the terms of the MIT license.
//
// Hongwei Li lihw81@gmail.com
//

#ifndef PCAMERA_MISC_H
#define PCAMERA_MISC_H

#include <Paper3D/pcamera.h>

class P_DLLEXPORT PCameraMisc 
{
public:
    // Find the corners of polygon on a given plane that fills the screen (one
    // can't see the corner of the plane quad). The given plane is in the world
    // space.  Return the number of corners of that planar polygon.
    static puint32 getGroundPolygon(const PCamera *camera, const PPlane &plane, PVector3 *out_corners);
    
    // Oblique projection
    // Modify the input projection matrix to use user defined plane as near plane. Note that
    // the plane is now in the world space.
    // http://www.terathon.com/lengyel/Lengyel-Oblique.pdf
    static PMatrix4x4 getObliqueProjectionMatrix(const PCamera *camera, const PPlane &plane);
};


#endif // !PCAMERA_MISC_H
