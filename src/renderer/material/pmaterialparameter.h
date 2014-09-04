// pmaterialparameter.h
// Material parameters
// 
// Copyright 2012 - 2014 Future Interface . 
// This software is licensed under the terms of the MIT license.
//
// Hongwei Li lihw81@gmail.com and Hongwei Li lihw81@gmail.com
//


#ifndef PMATERIALPARAMETER_H
#define PMATERIALPARAMETER_H

#include <Paper3D/pabstractrenderparameter.h>

#include <PFoundation/pvector2.h>
#include <PFoundation/pvector3.h>
#include <PFoundation/pvector4.h>
#include <PFoundation/pmatrix3x3.h>
#include <PFoundation/pmatrix4x4.h>
#include <PFoundation/pcolorrgba.h>

class PTexture;
class PMaterialResource;
class PRenderState;
class PResourceManager;


class P_DLLEXPORT PMaterialParameter : public PAbstractRenderParameter
{
    void operator=(const PMaterialParameter &other) {}

public:
    PMaterialParameter(const pchar *name, const pchar *uniformName, PGlShaderUniformTypeEnum type, puint32 size);
    PMaterialParameter(const PMaterialParameter &other); 

    virtual ~PMaterialParameter();
    
    // Scalar
    void operator=(pbool value);
    void operator=(pint32 value);
    void operator=(pfloat32 value);
    void operator=(const PVector2 &value);
    void operator=(const PVector3 &value);
    void operator=(const PVector4 &value);
    void operator=(const PMatrix3x3 &value);
    void operator=(const PMatrix4x4 &value);
    void operator=(const PColorRGBA &value);
    void operator=(PTexture *value);
    
    // Uniform array.
    void operator=(const pbool *value);
    void operator=(const pint32 *value);
    void operator=(const pfloat32 *value);
    void operator=(const PVector2 *value);
    void operator=(const PVector3 *value);
    void operator=(const PVector4 *value);
    void operator=(const PMatrix3x3 *value);
    void operator=(const PMatrix4x4 *value);

    void upload(PMaterialResource *materialResource, PRenderState *renderState, puint32 &numTextures);
    virtual void upload(PGlShader              *shader,
                        PRenderTransform       *transform, 
                        PCamera                *camera, 
                        const PRenderLightQueue &lights); 
    pbool unpack(const pchar *text, PResourceManager *resourceManager);

private:
    void uploadInt(pint32 location, PGlShader *shader);
    void uploadInt2(pint32 location, PGlShader *shader);
    void uploadInt3(pint32 location, PGlShader *shader);
    void uploadInt4(pint32 location, PGlShader *shader);
    void uploadFloat(pint32 location, PGlShader *shader);
    void uploadFloat2(pint32 location, PGlShader *shader);
    void uploadFloat3(pint32 location, PGlShader *shader);
    void uploadFloat4(pint32 location, PGlShader *shader);
    void uploadTexture(pint32 location, PGlShader *shader);
    void uploadMatrix2x2(pint32 location, PGlShader *shader);
    void uploadMatrix3x3(pint32 location, PGlShader *shader);
    void uploadMatrix4x4(pint32 location, PGlShader *shader);
    void uploadIntArray(pint32 location, PGlShader *shader);
    void uploadInt2Array(pint32 location, PGlShader *shader);
    void uploadInt3Array(pint32 location, PGlShader *shader);
    void uploadInt4Array(pint32 location, PGlShader *shader);
    void uploadFloatArray(pint32 location, PGlShader *shader);
    void uploadFloat2Array(pint32 location, PGlShader *shader);
    void uploadFloat3Array(pint32 location, PGlShader *shader);
    void uploadFloat4Array(pint32 location, PGlShader *shader);
    void uploadMatrix2x2Array(pint32 location, PGlShader *shader);
    void uploadMatrix3x3Array(pint32 location, PGlShader *shader);
    void uploadMatrix4x4Array(pint32 location, PGlShader *shader);

    pbool unpackFloat(const pchar *text);
    pbool unpackFloat2(const pchar *text);
    pbool unpackFloat3(const pchar *text);
    pbool unpackFloat4(const pchar *text);
    pbool unpackInt(const pchar *text);
    pbool unpackInt2(const pchar *text);
    pbool unpackInt3(const pchar *text);
    pbool unpackInt4(const pchar *text);
    pbool unpackFloat2x2(const pchar *text);
    pbool unpackFloat3x3(const pchar *text);
    pbool unpackFloat4x4(const pchar *text);

private:
    typedef void (PMaterialParameter::*UploadFuncPointer)(pint32 location, PGlShader *);
    union DataType
    {
        pint32    i;
        pfloat32  f;
        PTexture *t;
        puint8   *v;
    };

    UploadFuncPointer         m_upload;
    puint32                   m_size;         // The size of uniform array
    DataType                  m_value;
};

#endif // !PMATERIALPARAMETER_H


