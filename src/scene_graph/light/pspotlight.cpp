// pspotlight.cpp
// The spot light.
//
// Copyright 2012 - 2014 Future Interface. 
// This software is licensed under the terms of the MIT license.
//
// Hongwei Li lihw81@gmail.com and Hongwei Li lihw81@gmail.com
//

#include "pspotlight.h"

#include <PFoundation/pmathutility.h>

#include <math.h>

P_OBJECT_DEFINE(PSpotLight);

PSpotLight::PSpotLight(const pchar *name, PScene *scene)
    : PDirectionalLight(name, scene)
{
    m_type = P_LIGHT_SPOT;

    P_OBJECT_INITIALIZE_PROPERTY(PDirectionalLight);

    P_OBJECT_REGISTER_PROPERTY("cutoff",    &m_cutoff);
    P_OBJECT_REGISTER_PROPERTY("exponent",  &m_exponent);
    
    m_cutoff   = 45.0f;
    m_exponent = 0.0f;
}

PSpotLight::PSpotLight(const pchar *name, PNode *node)
    : PDirectionalLight(name, node)
{
    m_type = P_LIGHT_SPOT;

    P_OBJECT_INITIALIZE_PROPERTY(PDirectionalLight);

    P_OBJECT_REGISTER_PROPERTY("cutoff",    &m_cutoff);
    P_OBJECT_REGISTER_PROPERTY("exponent",  &m_exponent);
    
    m_cutoff   = 45.0f;
    m_exponent = 0.0f;
}

PSpotLight::~PSpotLight()
{
    P_OBJECT_UNINITIALIZE_PROPERTY();
}

void PSpotLight::setCutoff(pfloat32 cutoff)
{
    m_cutoff = cutoff;
}

void PSpotLight::setExponent(pfloat32 exponent)
{
    m_exponent = exponent;
}

