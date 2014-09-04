// pcameraparameter.cpp
// Render command parameter from a camera
// 
// Copyright 2012 - 2014 Future Interface . 
// This software is licensed under the terms of the MIT license.
//
// Hongwei Li lihw81@gmail.com
//


#include "pcameraparameter.h"

#include <Paper3D/pcamera.h>

#include <PFoundation/pvector3.h>
#include <PFoundation/pvector4.h>

#include <PFoundation/pglshader.h>


PCameraParameter::PCameraParameter(const pchar *name, const pchar *uniformName)
    : PAbstractRenderParameter(name, uniformName)
{
    if (m_name == "position")
    {
        m_upload = &PCameraParameter::uploadPosition;
        m_uniformType = P_GLSHADERUNIFORM_FLOAT3;
    }
    else if (m_name == "viewport")
    {
        m_upload = &PCameraParameter::uploadViewport;
        m_uniformType = P_GLSHADERUNIFORM_FLOAT4;
    }
    else if (m_name == "inversed-viewport")
    {
        m_upload = &PCameraParameter::uploadInversedViewport;
        m_uniformType = P_GLSHADERUNIFORM_FLOAT2;
    }
    else
    {
        m_upload = P_NULL;
        PLOG_WARNING("Failed to find such camera property called %s", name);
    }
}

PCameraParameter::~PCameraParameter()
{
}

void PCameraParameter::upload(PGlShader              *shader,
                              PRenderTransform       *transform, 
                              PCamera                *camera, 
                              const PRenderLightQueue &lights)
{
    if (m_uniformLocation < 0)
    {
        PLOG_ERROR("The camera parameter doesn't link to the shader");
    }
    else
    {
        (this->*m_upload)(camera, shader);
    }
}

void PCameraParameter::uploadPosition(PCamera *camera, PGlShader *shader)
{
    const PVector3 &position = camera->cameraPosition();
    shader->uniform(m_uniformLocation, position[0], position[1], position[2]);
}

void PCameraParameter::uploadViewport(PCamera *camera, PGlShader *shader)
{
    const PVector4 &viewport = camera->viewport();
    shader->uniform(m_uniformLocation, viewport[0], viewport[1], viewport[2], viewport[3]);
}

void PCameraParameter::uploadInversedViewport(PCamera *camera, PGlShader *shader)
{
    const PVector4 &viewport = camera->viewport();
    shader->uniform(m_uniformLocation, 1.0f / (viewport[2] - viewport[0]), 1.0f / (viewport[3] - viewport[1]));
}


