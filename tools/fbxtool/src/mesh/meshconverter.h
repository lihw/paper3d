// meshconverter.h
// FbxTool.
//
// Copyright 2012 - 2014 Future Interface. 
// This software is licensed under the terms of the MIT license.
//
// Hongwei Li lihw81@gmail.com
//

#ifndef MESHCONVERTER_H
#define MESHCONVERTER_H

#include "../assetconverter.h"
#include "mesh.h"


class Mesh;

class MeshConverter : public AssetConverter
{
public:
    MeshConverter(FbxScene *scene, Arguments *arguments);

    bool convert();

private:
    FbxString getMeshName(FbxNode *node, FbxMesh *mesh);

    bool exportMesh(FbxNode *node, FbxMesh *mesh);
    // Each vertex is only of a triangle, and the number of
    // indices is equal to that of vertices.
    void convertSplitted(FbxMesh *mesh, const VertexFormat &vertexFormat, Mesh *converted);
    // Vertices are shared by triangles.
    void convertShared(FbxMesh *mesh, const VertexFormat &vertexFormat, Mesh *converted);

    FbxVector4 getNormal(FbxMesh *mesh, int index);
    FbxVector4 getBinormal(FbxMesh *mesh, int index);
    FbxVector4 getTangent(FbxMesh *mesh, int index);
    FbxVector2 getUV(FbxMesh *mesh, int id, int index);
    FbxVector4 getColor(FbxMesh *mesh, int index);
    VertexAttribValue getVertexAttrib(FbxMesh *mesh, int attrib, int index);

};

#endif // !MESHCONVERTER_H
