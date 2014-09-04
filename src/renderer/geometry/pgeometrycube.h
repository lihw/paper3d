// pgeometrycube.h 
// The geometry cube
//
// Copyright 2012 - 2014 Future Interface . 
// This software is licensed under the terms of the MIT license.
//
// Hongwei Li lihw81@gmail.com
//
// 

#ifndef PGEOMETRYCUBE_H
#define PGEOMETRYCUBE_H

#include <Paper3D/pabstractgeometry.h>

class PGeometryCube : public PAbstractGeometry
{
    PGeometryCube(const PAbstractGeometry &other) {}
    void operator=(const PGeometryCube &other) {}

public:
    PGeometryCube(PResourceManager *resourceManager);
    virtual ~PGeometryCube();

private:
    // Texture coordinate of the cube
    //          +--------+--------+--------+
    //          | right  | top    | back   |
    //          +--------+--------+--------+
    //          | front  | bottom | left   |
    //          +--------+--------+--------+
    PMesh *createMesh(PResourceManager *resourceManager);
};

#endif // !PGEOMETRYCUBE_H

