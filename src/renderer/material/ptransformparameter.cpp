// ptransformparameter.cpp
// Render command parameter from a transform
// 
// Copyright 2012 - 2014 Future Interface . 
// This software is licensed under the terms of the MIT license.
//
// Hongwei Li lihw81@gmail.com
//

#include "ptransformparameter.h"

#include <Paper3D/prendertransform.h>

#include <PFoundation/pglshader.h>


PTransformParameter::PTransformParameter(const pchar *name, const pchar *uniformName)
    : PAbstractRenderParameter(name, uniformName)
{
    if (m_name == "mvp")
    {
        m_upload = &PTransformParameter::uploadMVP;
        m_uniformType = P_GLSHADERUNIFORM_FLOATMATRIX4X4;
    }
    else if (m_name == "mv")
    {
        m_upload = &PTransformParameter::uploadModelViewMatrix;
        m_uniformType = P_GLSHADERUNIFORM_FLOATMATRIX4X4;
    }
    else if (m_name == "model")
    {
        m_upload = &PTransformParameter::uploadModelMatrix;
        m_uniformType = P_GLSHADERUNIFORM_FLOATMATRIX4X4;
    }
    else if (m_name == "view")
    {
        m_upload = &PTransformParameter::uploadViewMatrix;
        m_uniformType = P_GLSHADERUNIFORM_FLOATMATRIX4X4;
    }
    else if (m_name == "normal")
    {
        m_upload = &PTransformParameter::uploadNormalMatrix;
        m_uniformType = P_GLSHADERUNIFORM_FLOATMATRIX3X3;
    }
    else if (m_name == "projection")
    {
        m_upload = &PTransformParameter::uploadProjectionMatrix;
        m_uniformType = P_GLSHADERUNIFORM_FLOATMATRIX4X4;
    }
    else
    {
        m_upload = P_NULL;
        PLOG_WARNING("Unknown transform property called %s.", name);
    }

	PASSERT(m_upload != P_NULL);
}

PTransformParameter::~PTransformParameter()
{
}
    
void PTransformParameter::upload(PGlShader              *shader,
                              PRenderTransform       *transform, 
                              PCamera                *camera, 
                              const PRenderLightQueue &lights)
{
    (this->*m_upload)(transform, shader);
}
    
void PTransformParameter::uploadModelMatrix(PRenderTransform *transform, PGlShader *shader)
{
    shader->uniformMatrix4fv(m_uniformLocation, transform->modelMatrix().m_m, 1);
}

void PTransformParameter::uploadViewMatrix(PRenderTransform *transform, PGlShader *shader)
{
    shader->uniformMatrix4fv(m_uniformLocation, transform->viewMatrix().m_m, 1);
}

void PTransformParameter::uploadModelViewMatrix(PRenderTransform *transform, PGlShader *shader)
{
    shader->uniformMatrix4fv(m_uniformLocation, transform->modelviewMatrix().m_m, 1);
}

void PTransformParameter::uploadNormalMatrix(PRenderTransform *transform, PGlShader *shader)
{
    shader->uniformMatrix3fv(m_uniformLocation, transform->normalMatrix().m_m, 1);
}

void PTransformParameter::uploadMVP(PRenderTransform *transform, PGlShader *shader)
{
    shader->uniformMatrix4fv(m_uniformLocation, transform->mvpMatrix().m_m, 1);
}

void PTransformParameter::uploadProjectionMatrix(PRenderTransform *transform, PGlShader *shader)
{
    shader->uniformMatrix4fv(m_uniformLocation, transform->projectionMatrix().m_m, 1);
}

