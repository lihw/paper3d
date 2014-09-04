// meshexporter.h
// FbxTool.
//
// Copyright 2012 - 2014 Future Interface. 
// This software is licensed under the terms of the MIT license.
//
// Hongwei Li lihw81@gmail.com
//

#ifndef MESHEXPORTER_H
#define MESHEXPORTER_H


class Mesh;

class MeshExporter 
{
public:
    bool exportOBJ(const char *filename, Mesh *mesh);
    bool exportPMH(const char *filename, Mesh *mesh);
};


#endif // !MESHEXPORTER_H
