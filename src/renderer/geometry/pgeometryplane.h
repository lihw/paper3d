// pgeometryplane.h 
// The geometry plane
//
// Copyright 2012 - 2014 Future Interface . 
// This software is licensed under the terms of the MIT license.
//
// Hongwei Li lihw81@gmail.com
//
// 

#ifndef PGEOMETRYPLANE_H
#define PGEOMETRYPLANE_H

#include <Paper3D/pabstractgeometry.h>


class PGeometryPlane : public PAbstractGeometry
{
    PGeometryPlane(const PAbstractGeometry &other) {}
    void operator=(const PGeometryPlane &other) {}

public:
    PGeometryPlane(PResourceManager *resourceManager);
    virtual ~PGeometryPlane();

private:
    PMesh *createMesh(PResourceManager *resourceManager); 
};

#endif // !PGEOMETRYPLANE_H


