// ppointlight.cpp
// The point light.
//
// Copyright 2012 - 2014 Future Interface. 
// This software is licensed under the terms of the MIT license.
//
// Hongwei Li lihw81@gmail.com and Hongwei Li lihw81gmail.com
//

#include "ppointlight.h"

P_OBJECT_DEFINE(PPointLight);

PPointLight::PPointLight(const pchar *name, PNode *parent)
    : PAbstractLight(P_LIGHT_POINT, name, parent)
{
    P_OBJECT_INITIALIZE_PROPERTY(PAbstractLight);
    P_OBJECT_REGISTER_PROPERTY("range", &m_range);
}

PPointLight::PPointLight(const pchar *name, PScene *scene)
    : PAbstractLight(P_LIGHT_POINT, name, scene)
{
    P_OBJECT_INITIALIZE_PROPERTY(PAbstractLight);
    P_OBJECT_REGISTER_PROPERTY("range", &m_range);
}

PPointLight::~PPointLight()
{
    P_OBJECT_UNINITIALIZE_PROPERTY();
}

void PPointLight::setRange(pfloat32 distance)
{
    m_range = distance;
}

