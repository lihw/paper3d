// pgeometrymesh.h 
// The geometry mesh
//
// Copyright 2012 - 2014 Future Interface . 
// This software is licensed under the terms of the MIT license.
//
// Hongwei Li lihw81@gmail.com
//
// 

#ifndef PGEOMETRYMESH_H
#define PGEOMETRYMESH_H

#include <Paper3D/pabstractgeometry.h>

class P_DLLEXPORT PGeometryMesh : public PAbstractGeometry
{
    PGeometryMesh(const PAbstractGeometry &other) {}
    void operator=(const PGeometryMesh &other) {}

public:
    PGeometryMesh(const pchar *id, PResourceManager *resourceManager);
    virtual ~PGeometryMesh();
};

#endif // !PGEOMETRYMESH_H


