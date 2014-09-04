// ptransformparameter.h
// Render command parameter from a transform
// 
// Copyright 2012 - 2014 Future Interface . 
// This software is licensed under the terms of the MIT license.
//
// Hongwei Li lihw81@gmail.com
//

#ifndef PTRANSFORMPARAMETER_H
#define PTRANSFORMPARAMETER_H


#include <Paper3D/pabstractrenderparameter.h>


class P_DLLEXPORT PTransformParameter : public PAbstractRenderParameter
{
    void operator=(const PTransformParameter &other) {}
    PTransformParameter(const PTransformParameter &other) : PAbstractRenderParameter(P_NULL, P_NULL) {}

public:
    PTransformParameter(const pchar *name, const pchar *uniformName);
    virtual ~PTransformParameter();

    virtual void upload(PGlShader              *shader,
                      PRenderTransform       *transform, 
                      PCamera                *camera, 
                      const PRenderLightQueue &lights); 

private:
    void uploadModelMatrix(PRenderTransform *transform, PGlShader *shader);
    void uploadViewMatrix(PRenderTransform *transform, PGlShader *shader);
    void uploadModelViewMatrix(PRenderTransform *transform, PGlShader *shader);
    void uploadProjectionMatrix(PRenderTransform *transform, PGlShader *shader);
    void uploadMVP(PRenderTransform *transform, PGlShader *shader);
    void uploadNormalMatrix(PRenderTransform *transform, PGlShader *shader);

private:
    typedef void (PTransformParameter::*UploadFunction)(PRenderTransform *transform, PGlShader *shader);
    UploadFunction m_upload;
};

#endif // !PTRANSFORMPARAMETER_H

