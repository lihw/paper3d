// pgeometrysphere.h 
// The geometry sphere
//
// Copyright 2012 - 2014 Future Interface . 
// This software is licensed under the terms of the MIT license.
//
// Hongwei Li lihw81@gmail.com
//
// 

#ifndef PGEOMETRYSPHERE_H
#define PGEOMETRYSPHERE_H

#include <Paper3D/pabstractgeometry.h>

class PGeometrySphere : public PAbstractGeometry
{
    PGeometrySphere(const PAbstractGeometry &other) {}
    void operator=(const PGeometrySphere &other) {}

public:
    PGeometrySphere(PResourceManager *resourceManager);
    virtual ~PGeometrySphere();

private:
    PMesh *createMesh(PResourceManager *resourceManager);
};

#endif // !PGEOMETRYSPHERE_H


