// pabstractlight.cpp
// The abstract class of all lights.
//
// Copyright 2012 - 2014 Future Interface. 
// This software is licensed under the terms of the MIT license.
//
// Hongwei Li lihw81@gmail.com and Hongwei Li lihw81@gmail.com
//

#include "pabstractlight.h"

#include <Paper3D/pscene.h>


P_OBJECT_DEFINE(PAbstractLight)

PAbstractLight::PAbstractLight(PLightTypeEnum lightType, const pchar* name, PScene *scene)
    : PNode(name, scene, scene->root(), false)
{
    m_type = lightType;

    P_OBJECT_INITIALIZE_PROPERTY(PNode);
    P_OBJECT_REGISTER_PROPERTY("enabled",    &m_enabled);
    P_OBJECT_REGISTER_PROPERTY("intensity",  &m_intensity);
    P_OBJECT_REGISTER_PROPERTY("color",      &m_color);

    m_intensity = 0.5f;
    m_color     = P_COLOR_WHITE;
    m_enabled   = true;
}
    
PAbstractLight::PAbstractLight(PLightTypeEnum lightType, const pchar* name, PNode *parent)
    : PNode(name, parent->scene(), parent, false)
{
    m_type = lightType;

    P_OBJECT_INITIALIZE_PROPERTY(PNode);
    P_OBJECT_REGISTER_PROPERTY("enabled",    &m_enabled);
    P_OBJECT_REGISTER_PROPERTY("intensity",  &m_intensity);
    P_OBJECT_REGISTER_PROPERTY("color",      &m_color);

    m_intensity = 0.5f;
    m_color     = P_COLOR_WHITE;
    m_enabled   = true;
}

PAbstractLight::~PAbstractLight()
{
    P_OBJECT_UNINITIALIZE_PROPERTY();
}
    
PVector3 PAbstractLight::worldPosition() const
{
    return pVector3(m_worldTransform.m_m[12],
                    m_worldTransform.m_m[13],
                    m_worldTransform.m_m[14]);
}

void PAbstractLight::setEnabled(pbool enabled)
{
    m_enabled = enabled;
}

void PAbstractLight::setColor(const PColorRGBA &color)
{
    m_color = color;
}

void PAbstractLight::setIntensity(const pfloat32 &intensity)
{
    m_intensity = intensity;
}
    
void PAbstractLight::updateWorldTransform()
{
    // -------------------------------------------------------------- 
    // Update this world transformation matrix.
    // -------------------------------------------------------------- 
    pbool localTransformChanged = false;
    const PMatrix4x4 &localTransform = m_localTransform.toMatrix4x4(&localTransformChanged);

    // Update the this world transformation either when the parent has its own 
    // or the local transformation is changed.
    if (m_parent->worldTransformChanged() || localTransformChanged)
    {
        m_worldTransform = m_parent->worldTransform() * localTransform;
        m_worldTransformChanged = true;
    }
    else
    {
        m_worldTransformChanged = false;
    }
}
