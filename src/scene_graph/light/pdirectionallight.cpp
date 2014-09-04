// pdirectionallight.cpp
// The directional light.
//
// Copyright 2012 - 2014 Future Interface. 
// This software is licensed under the terms of the MIT license.
//
// Hongwei Li lihw81@gmail.com and Hongwei Li lihw81@gmail.com
//

#include "pdirectionallight.h"

#include <PFoundation/pmatrix3x3.h>


P_OBJECT_DEFINE(PDirectionalLight);

PDirectionalLight::PDirectionalLight(const pchar *name, PScene *scene)
    : PAbstractLight(P_LIGHT_DIRECTIONAL, name, scene)
{
    P_OBJECT_INITIALIZE_PROPERTY(PAbstractLight);

    P_OBJECT_REGISTER_PROPERTY("direction",    &m_direction);
    P_OBJECT_REGISTER_PROPERTY("cast-shadow",  &m_castShadow);
    m_direction = pVector3(-1.0f, 0.0f, 0.0f);
}

PDirectionalLight::PDirectionalLight(const pchar *name, PNode *node)
    : PAbstractLight(P_LIGHT_DIRECTIONAL, name, node)
{
    P_OBJECT_INITIALIZE_PROPERTY(PAbstractLight);

    P_OBJECT_REGISTER_PROPERTY("direction",    &m_direction);
    P_OBJECT_REGISTER_PROPERTY("cast-shadow",  &m_castShadow);
    m_direction = pVector3(-1.0f, 0.0f, 0.0f);
}

PDirectionalLight::~PDirectionalLight()
{
    P_OBJECT_UNINITIALIZE_PROPERTY();
}

void PDirectionalLight::setDirection(pfloat32 x, pfloat32 y, pfloat32 z)
{
    PVector3 v = pVector3(x, y, z);
    v.normalize();
    m_direction = v;
}

void PDirectionalLight::setDirection(const PVector3 &direction)
{
    PVector3 d(direction);
    d.normalize();
    m_direction = d;
}

void PDirectionalLight::setCastShadow(pbool castShadow)
{
    m_castShadow = castShadow;
}

PVector3 PDirectionalLight::worldDirection() const
{
    PVector3 ret;

    pfloat32 normalMatrix[9];
    pMatrix3x3InverseTransposeMatrix4x4(m_worldTransform.m_m, normalMatrix);
    pMatrix3x3MultiplyVector3(normalMatrix, m_direction.toVector3().m_v, ret.m_v);

    return ret;
}
    
