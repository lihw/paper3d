// pgeometrycube.h 
// The geometry cube
//
// Copyright 2012 - 2014 Future Interface . 
// This software is licensed under the terms of the MIT license.
//
// Hongwei Li lihw81@gmail.com
//
// 

#include "pgeometrycube.h"

#include <Paper3D/presourcemanager.h>
#include <Paper3D/pmesh.h>

#include <PFoundation/pglvertexbuffer.h>


PGeometryCube::PGeometryCube(PResourceManager *resourceManager)
{
    m_mesh = dynamic_cast<PMesh *>(resourceManager->findResourceObject("cube"));
    if (m_mesh == P_NULL)
    {
        m_mesh = createMesh(resourceManager);
        PASSERT(m_mesh != P_NULL);
    }

    m_mesh->retain();
}

PGeometryCube::~PGeometryCube()
{
}

PMesh *PGeometryCube::createMesh(PResourceManager *resourceManager)
{
    pfloat32 vertices[][8] = 
    {
        // front face
        {-1.0f, -1.0f, 1.0f, 0.00f, 0.0f, 0, 0, 1},  // v0
        { 1.0f, -1.0f, 1.0f, 0.33f, 0.0f, 0, 0, 1}, // v1
        {-1.0f,  1.0f, 1.0f, 0.00f, 0.5f, 0, 0, 1},  // v2
        { 1.0f,  1.0f, 1.0f, 0.33f, 0.5f, 0, 0, 1}, // v3

        // right face
        { 1.0f, -1.0f,  1.0f, 0.00f, 0.5f, 1, 0, 0}, // v4
        { 1.0f, -1.0f, -1.0f, 0.33f, 0.5f, 1, 0, 0}, // v5
        { 1.0f,  1.0f,  1.0f, 0.00f, 1.0f, 1, 0, 0},  // v6
        { 1.0f,  1.0f, -1.0f, 0.33f, 1.0f, 1, 0, 0}, // v7

        // back face
        { 1.0f, -1.0f, -1.0f, 0.66f, 0.5f, 0, 0, -1}, // v8
        {-1.0f, -1.0f, -1.0f, 1.00f, 0.5f, 0, 0, -1},  // v9
        { 1.0f,  1.0f, -1.0f, 0.66f, 1.0f, 0, 0, -1}, // v10
        {-1.0f,  1.0f, -1.0f, 1.00f, 1.0f, 0, 0, -1},  // v11

        // left face
        {-1.0f, -1.0f, -1.0f, 0.66f, 0.0f, -1, 0, 0}, // v12
        {-1.0f, -1.0f,  1.0f, 1.00f, 0.0f, -1, 0, 0},  // v13
        {-1.0f,  1.0f, -1.0f, 0.66f, 0.5f, -1, 0, 0}, // v14
        {-1.0f,  1.0f,  1.0f, 1.00f, 0.5f, -1, 0, 0},  // v15

        // bottom face
        {-1.0f, -1.0f, -1.0f, 0.33f, 0.0f, 0, -1, 0}, // v16
        { 1.0f, -1.0f, -1.0f, 0.66f, 0.0f, 0, -1, 0}, // v17
        {-1.0f, -1.0f,  1.0f, 0.33f, 0.5f, 0, -1, 0}, // v18
        { 1.0f, -1.0f,  1.0f, 0.66f, 0.5f, 0, -1, 0}, // v19

        // top face
        {-1.0f,  1.0f,  1.0f, 0.33f, 0.5f, 0, 1, 0}, // v20
        { 1.0f,  1.0f,  1.0f, 0.66f, 0.5f, 0, 1, 0}, // v21
        {-1.0f,  1.0f, -1.0f, 0.33f, 1.0f, 0, 1, 0}, // v22
        { 1.0f,  1.0f, -1.0f, 0.66f, 1.0f, 0, 1, 0}  // v23
    };

    puint16 indices[] = 
    {
        0,   1,  2,  1,  3,  2,  // Face font - triangle strip ( v0,  v1,  v2,  v3)
        4,   5,  6,  5,  7,  6,  // Face right - triangle strip ( v4,  v5,  v6,  v7)
        8,   9, 10,  9, 11, 10,  // Face back - triangle strip ( v8,  v9, v10, v11)
        12, 13, 14, 13, 15, 14,  // Face left - triangle strip (v12, v13, v14, v15)
        16, 17, 18, 17, 19, 18,  // Face bottom - triangle strip (v16, v17, v18, v19)
        20, 21, 22, 21, 23, 22,  // Face top - triangle strip (v20, v21, v22, v23)
    };

    puint32 numVertices = 24;
    puint32 numIndices  = 36;

    PGlVertexFormat vertexFormat;
    vertexFormat.addAttrib("position", 3, P_FLOAT32);
    vertexFormat.addAttrib("texcoord0", 2, P_FLOAT32);
    vertexFormat.addAttrib("normal", 3, P_FLOAT32);

    pfloat32 minPoint[] = {-1.0f, -1.0f, -1.0f};
    pfloat32 maxPoint[] = { 1.0f,  1.0f,  1.0f};
    PBox box(minPoint, maxPoint);
    return resourceManager->createMesh(&vertices[0][0], numVertices, indices, numIndices,
        box, &vertexFormat, "cube", false, true);
}
