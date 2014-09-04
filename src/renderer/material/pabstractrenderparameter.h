// pabstractrenderparameter.h
// Render command parameter
// 
// Copyright 2012 - 2014 Future Interface . 
// This software is licensed under the terms of the MIT license.
//
// Hongwei Li lihw81@gmail.com
//


#ifndef PABSTRACTRENDERPARAMETER_H
#define PABSTRACTRENDERPARAMETER_H

#include <PFoundation/pstring.h>

#include <PFoundation/pglshader.h>


class PRenderTransform;
class PCamera;
class PRenderLightQueue;
class PWaterWave;
class PRenderState;


class P_DLLEXPORT PAbstractRenderParameter 
{
    friend class PMaterial;
    
    PAbstractRenderParameter(const PAbstractRenderParameter &other) {}
    void operator=(const PAbstractRenderParameter &other) {}

public:
    PAbstractRenderParameter(const pchar *name, const pchar *uniformName);
    virtual ~PAbstractRenderParameter();

    virtual void upload(PGlShader              *shader,
                      PRenderTransform       *transform, 
                      PCamera                *camera, 
                      const PRenderLightQueue &lights) = 0; 

    P_INLINE const PString &name() const { return m_name; }
    P_INLINE const PString &uniformName() const { return m_uniformName; }
    P_INLINE pint32 uniformLocation() const { return m_uniformLocation; }
    P_INLINE PGlShaderUniformTypeEnum uniformType() const { return m_uniformType; }
    
    void setUniformLocation(pint32 location); 

protected:
    PString                   m_name;
    PString                   m_uniformName;
    pint32                    m_uniformLocation;
    PGlShaderUniformTypeEnum  m_uniformType;
};

#endif // !PABSTRACTRENDERPARAMETER_H


