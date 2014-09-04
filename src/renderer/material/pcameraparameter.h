// pcameraparameter.h
// Render command parameter from a camera
// 
// Copyright 2012 - 2014 Future Interface . 
// This software is licensed under the terms of the MIT license.
//
// Hongwei Li lihw81@gmail.com
//

#ifndef PCAMERAPARAMETER_H
#define PCAMERAPARAMETER_H

#include <Paper3D/pabstractrenderparameter.h>


class P_DLLEXPORT PCameraParameter : public PAbstractRenderParameter
{
    void operator=(const PCameraParameter &other) {}
    PCameraParameter(const PCameraParameter &other) : PAbstractRenderParameter(P_NULL, P_NULL) {}

public:
    PCameraParameter(const pchar *name, const pchar *uniformName);
    virtual ~PCameraParameter();

    virtual void upload(PGlShader              *shader,
                      PRenderTransform       *transform, 
                      PCamera                *camera, 
                      const PRenderLightQueue &lights); 

private:
    void uploadPosition(PCamera *camera, PGlShader *shader);
    void uploadViewport(PCamera *camera, PGlShader *shader);
    void uploadInversedViewport(PCamera *camera, PGlShader *shader);

private:
    typedef void (PCameraParameter::*UploadFunction)(PCamera *camera, PGlShader *shader);
    UploadFunction m_upload;
};

#endif // !PCAMERAPARAMETER_H

