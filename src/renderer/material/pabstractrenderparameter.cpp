// pabstractrenderparameter.cpp
// Render command parameter
// 
// Copyright 2012 - 2014 Future Interface . 
// This software is licensed under the terms of the MIT license.
//
// Hongwei Li lihw81@gmail.com
//

#include "pabstractrenderparameter.h"

PAbstractRenderParameter::PAbstractRenderParameter(const pchar *name, const pchar *uniformName)
    : m_name(name)
    , m_uniformName(uniformName)
{
    m_uniformLocation = -1;
    m_uniformType     = P_GLSHADERUNIFORM_UNKNOWN;
}

PAbstractRenderParameter::~PAbstractRenderParameter()
{
}
    
void PAbstractRenderParameter::setUniformLocation(pint32 location)
{
    m_uniformLocation = location;
}
