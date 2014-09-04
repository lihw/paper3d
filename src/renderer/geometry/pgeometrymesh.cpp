// pgeometrymesh.cpp
// The user defined mesh
//
// Copyright 2012 - 2014 Future Interface . 
// This software is licensed under the terms of the MIT license.
//
// Hongwei Li lihw81@gmail.com
//
// 

#include "pgeometrymesh.h"

#include <Paper3D/presourcemanager.h>
#include <Paper3D/pmesh.h>

    
PGeometryMesh::PGeometryMesh(const pchar *id, PResourceManager *resourceManager)
{
    m_mesh = resourceManager->getMesh(id);
    if (m_mesh != P_NULL)
    {
        m_mesh->retain();
    }
}

PGeometryMesh::~PGeometryMesh()
{
}
    
