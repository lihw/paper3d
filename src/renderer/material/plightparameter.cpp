// prenderparameterlight.cpp
// Render command parameter from a light
// 
// Copyright 2012 - 2014 Future Interface . 
// This software is licensed under the terms of the MIT license.
//
// Hongwei Li lihw81@gmail.com
//

#include "plightparameter.h"

#include <Paper3D/ppointlight.h>
#include <Paper3D/pdirectionallight.h>
#include <Paper3D/pspotlight.h>
#include <Paper3D/prenderlightqueue.h>

#include <PFoundation/pglshader.h>


PLightParameter::PLightParameter(PLightTypeEnum lightType, puint32 lightIndex, 
    const pchar *name, const pchar *uniformName)
    : PAbstractRenderParameter(name, uniformName)
    , m_lightType(lightType)
    , m_lightIndex(lightIndex)
{
    if (pstrcmp(name, "color") == 0)
    {
        m_upload = &PLightParameter::uploadColor;
        m_uniformType = P_GLSHADERUNIFORM_FLOAT3;
    }
    else if (pstrcmp(name, "intensity") == 0)
    {
        m_upload = &PLightParameter::uploadIntensity;
        m_uniformType = P_GLSHADERUNIFORM_FLOAT;
    }
    else if (pstrcmp(name, "position") == 0)
    {
        m_upload = &PLightParameter::uploadPosition;
        m_uniformType = P_GLSHADERUNIFORM_FLOAT3;
    }
    else if (pstrcmp(name, "range") == 0)
    {
        m_upload = &PLightParameter::uploadRange;
        m_uniformType = P_GLSHADERUNIFORM_FLOAT;
    }
    else if (pstrcmp(name, "direction") == 0)
    {
        m_upload = &PLightParameter::uploadDirection;
        m_uniformType = P_GLSHADERUNIFORM_FLOAT3;
    }
    else if (pstrcmp(name, "exponent") == 0)
    {
        m_upload = &PLightParameter::uploadSpotExponent;
        m_uniformType = P_GLSHADERUNIFORM_FLOAT;
    }
    else if (pstrcmp(name, "cutoff") == 0)
    {
        m_upload = &PLightParameter::uploadSpotCutoff;
        m_uniformType = P_GLSHADERUNIFORM_FLOAT;
    }
    else
    {
        PLOG_WARNING("Unknown light property called %s", name);
        m_upload = P_NULL;
    }
}

PLightParameter::~PLightParameter()
{
}

void PLightParameter::upload(PGlShader              *shader,
                          PRenderTransform       *transform, 
                          PCamera                *camera, 
                          const PRenderLightQueue &lights)
{
    if (m_uniformLocation < 0)
    {
        PLOG_ERROR("The light parameter doesn't link to the shader");
    }
    else
    {
        PAbstractLight *light = lights.light(m_lightType, m_lightIndex);
        if (light != P_NULL)
        {
            (this->*m_upload)(light, shader);
        }
        else
        {
            // Disable the affect of this light
            if (m_name == "intensity")
            {
                shader->uniform(m_uniformLocation, 0.0f);
            }

            const pchar *lightNames[] = { "point", "directional", "spot" };
            PLOG_DEBUG("Failed to find %s light %d in the render pass", 
                    lightNames[m_lightType - 1], m_lightIndex);
        }
    }
}

void PLightParameter::uploadIntensity(PAbstractLight *light, PGlShader *shader)
{
    shader->uniform(m_uniformLocation, light->intensity());
}

void PLightParameter::uploadColor(PAbstractLight *light, PGlShader *shader)
{
    PVector4 v = light->color().toVector4();
    shader->uniform(m_uniformLocation, v[0], v[1], v[2]);
}

void PLightParameter::uploadPosition(PAbstractLight *light, PGlShader *shader)
{
    PVector3 v = light->worldPosition();
    shader->uniform(m_uniformLocation, v[0], v[1], v[2]);
}

void PLightParameter::uploadRange(PAbstractLight *light, PGlShader *shader)
{
    PPointLight *pointLight = reinterpret_cast<PPointLight *>(light);
    shader->uniform(m_uniformLocation, pointLight->range());
}

void PLightParameter::uploadDirection(PAbstractLight *light, PGlShader *shader)
{
    PDirectionalLight *directionalLight = reinterpret_cast<PDirectionalLight *>(light);
    PVector3 v = directionalLight->worldDirection();
    shader->uniform(m_uniformLocation, v[0], v[1], v[2]);
}

void PLightParameter::uploadSpotExponent(PAbstractLight *light, PGlShader *shader)
{
    PSpotLight *spotLight = reinterpret_cast<PSpotLight *>(light);
    shader->uniform(m_uniformLocation, spotLight->exponent());
}

void PLightParameter::uploadSpotCutoff(PAbstractLight *light, PGlShader *shader)
{
    PSpotLight *spotLight = reinterpret_cast<PSpotLight *>(light);
    shader->uniform(m_uniformLocation, spotLight->cosCutoff());
}


