// psprite2d.h
// A sprite in the normalized window coordinate which is not affected
// by the scene camera.
//
// Copyright 2012 - 2014 Future Interface. 
// This software is licensed under the terms of the MIT license.
//
// Hongwei Li lihw81@gmail.com

#ifndef PSPRITE2D_H
#define PSPRITE2D_H

#include <Paper3D/psprite.h>

class P_DLLEXPORT PSprite2D : public PSprite
{
    P_OBJECT

    PSprite2D(const PSprite2D &other) : PSprite(P_NULL, (PScene *)P_NULL) {}
    void operator=(const PSprite2D &other) {}

public:
    PSprite2D(const pchar *name, PScene *scene);
    ~PSprite2D();
    
    virtual void calculateModelCameraMatrix(const PMatrix4x4 &cameraMatrix, 
        pfloat32 *out_matrix);

    virtual pbool unpack(const PXmlElement* xmlElement);

protected:
    virtual void prepareRender(PRenderState *renderState);

private:
    pfloat32 m_invAspect;
};



#endif  // !PSPRITE2D_H
