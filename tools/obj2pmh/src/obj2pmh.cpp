// obj2pmh.cpp
// The converter
//
// Copyright 2012 - 2014 Future Interface. 
// This software is licensed under the terms of the MIT license.
//
// Hongwei Li lihw81@gmail.com
//
//

#include "obj2pmh.h"

#include <common/log.h>
#include <common/pmhmodel.h>

#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>

#include <algorithm>

#include <assert.h>

Obj2Pmh::Obj2Pmh(const char *inputFile, const char *outputFile)
    : m_inputpath(inputFile)
    , m_outputpath(outputFile)
{
}

Obj2Pmh::~Obj2Pmh()
{
}

bool Obj2Pmh::execute()
{
    //
    // Load the model
    Assimp::Importer importer;   
    
    // Load the model
    const aiScene* scene = importer.ReadFile(m_inputpath.c_str(),
        aiProcess_Triangulate |                 // Triangulation
        //aiProcess_CalcTangentSpace |            // TBN 
        //aiProcess_FixInfacingNormals |            // Fix the inward face vertex order
        //aiProcess_GenSmoothNormals |            // Generate normals if it doesn't exist
        aiProcess_JoinIdenticalVertices |       // Merge vertices
        aiProcess_SortByPType |                 // Sort 
        aiProcess_FlipUVs |                     // The origin of UV are in the top left.
        0);                                     //

    if( !scene)   
    {
        logError(importer.GetErrorString());     
        return false;  
    }   

    // We only convert the first mesh in the file.
    aiMesh *mesh = scene->mMeshes[0];

    PmhModel model;
    model.numVertices = mesh->mNumVertices;

    unsigned int stride = 3;
    unsigned int numAttribs = 0;

    model.attribs[numAttribs].name = "position";
    model.attribs[numAttribs].type = "float32";
    model.attribs[numAttribs].size = 3;
    numAttribs++;

    if (mesh->HasNormals())
    {
        model.attribs[numAttribs].name = "normal";
        model.attribs[numAttribs].type = "float32";
        model.attribs[numAttribs].size = 3;
        numAttribs++;

        stride += 3;
    }

    if (mesh->HasTextureCoords(0))
    {
        model.attribs[numAttribs].name = "texcoord0";
        model.attribs[numAttribs].type = "float32";
        model.attribs[numAttribs].size = 2;
        numAttribs++;

        stride += 2;
    }

    model.numAttribs = numAttribs;
    model.vertexSize = stride * sizeof(float);

    model.vertices = new float [model.numVertices * stride];
    float *v = model.vertices;
    for (unsigned int i = 0; i < model.numVertices; i++)
    {
        v[0] = mesh->mVertices[i].x;
        v[1] = mesh->mVertices[i].y;
        v[2] = mesh->mVertices[i].z;

        if (mesh->HasNormals())
        {
            v[3] = mesh->mNormals[i].x;
            v[4] = mesh->mNormals[i].y;
            v[5] = mesh->mNormals[i].z;
        }
        if (mesh->HasTextureCoords(0))
        {
            v[6] = mesh->mTextureCoords[0][i].x;
            v[7] = mesh->mTextureCoords[0][i].y;
        }

        model.bb[0][0] = std::min(v[0], model.bb[0][0]);
        model.bb[0][1] = std::min(v[1], model.bb[0][1]);
        model.bb[0][2] = std::min(v[2], model.bb[0][2]);
        
        model.bb[1][0] = std::max(v[0], model.bb[1][0]);
        model.bb[1][1] = std::max(v[1], model.bb[1][1]);
        model.bb[1][2] = std::max(v[2], model.bb[1][2]);

        v += stride;
    }

    model.numIndices = mesh->mNumFaces * 3;
    model.indices = new unsigned short [model.numIndices];
    unsigned short *id = model.indices;
    for (unsigned int i = 0; i < mesh->mNumFaces; i++)
    {
        aiFace *face = mesh->mFaces;
        assert(face[i].mNumIndices == 3);
        
        id[0] = face[i].mIndices[0];
        id[1] = face[i].mIndices[1];
        id[2] = face[i].mIndices[2];

        id += 3;
    }

    return model.write(m_outputpath.c_str());  
}
