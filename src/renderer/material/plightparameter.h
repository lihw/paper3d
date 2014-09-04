// plightparameter.h
// Render command parameter from a light
// 
// Copyright 2012 - 2014 Future Interface . 
// This software is licensed under the terms of the MIT license.
//
// Hongwei Li lihw81@gmail.com
//

#ifndef PLIGHTPARAMETER_H
#define PLIGHTPARAMETER_H

#include <Paper3D/pabstractrenderparameter.h>
#include <Paper3D/pabstractlight.h>


class P_DLLEXPORT PLightParameter : public PAbstractRenderParameter
{

    void operator=(const PLightParameter &other) {}
    PLightParameter(const PLightParameter &other) : PAbstractRenderParameter(P_NULL, P_NULL) {}

public:
    PLightParameter(PLightTypeEnum lightType, puint32 lightIndex,
        const pchar *name, const pchar *uniformName);
    virtual ~PLightParameter();

    virtual void upload(PGlShader              *shader,
                      PRenderTransform       *transform, 
                      PCamera                *camera, 
                      const PRenderLightQueue &lights); 

private:
    void uploadIntensity(PAbstractLight *light, PGlShader *shader);
    void uploadColor(PAbstractLight *light, PGlShader *shader);
    void uploadPosition(PAbstractLight *light, PGlShader *shader);
    void uploadRange(PAbstractLight *light, PGlShader *shader);
    void uploadDirection(PAbstractLight *light, PGlShader *shader);
    void uploadSpotExponent(PAbstractLight *light, PGlShader *shader);
    void uploadSpotCutoff(PAbstractLight *light, PGlShader *shader);

private:
    typedef void (PLightParameter::*UploadFunction)(PAbstractLight *light, PGlShader *shader);
    UploadFunction m_upload;
    PLightTypeEnum m_lightType;
    puint32        m_lightIndex;
};

#endif // !PRENDERPARAMETERLIGHT_H

