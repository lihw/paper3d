// pgeometryplane.cpp
// The geometry plane
//
// Copyright 2012 - 2014 Future Interface . 
// This software is licensed under the terms of the MIT license.
//
// Hongwei Li lihw81@gmail.com
//
// 

#include "pgeometryplane.h"

#include <Paper3D/presourcemanager.h>
#include <Paper3D/pmesh.h>
    
#include <PFoundation/pglvertexbuffer.h>


PGeometryPlane::PGeometryPlane(PResourceManager *resourceManager)
{
    m_mesh = dynamic_cast<PMesh *>(resourceManager->findResourceObject("plane"));
    if (m_mesh == P_NULL)
    {
        m_mesh = createMesh(resourceManager);
        PASSERT(m_mesh != P_NULL);
    }

    m_mesh->retain();
}

PGeometryPlane::~PGeometryPlane()
{
}

PMesh *PGeometryPlane::createMesh(PResourceManager *resourceManager)
{
    pfloat32 vertices[32];
    puint16 indices[6];

    vertices[0] = -1.0f;  vertices[1] = -1.0f; vertices[2] = 0.0f;
    vertices[3] = 0.0f;   vertices[4] = 1.0f; 
    vertices[5] = 0.0f;   vertices[6] = 0.0f;  vertices[7] = 1.0f;
    
    vertices[8] = 1.0f;   vertices[9] = -1.0f; vertices[10] = 0.0f;
    vertices[11] = 1.0f;  vertices[12] = 1.0f; 
    vertices[13] = 0.0f;  vertices[14] = 0.0f; vertices[15] = 1.0f;
    
    vertices[16] = 1.0f;  vertices[17] = 1.0f; vertices[18] = 0.0f;
    vertices[19] = 1.0f;  vertices[20] = 0.0f; 
    vertices[21] = 0.0f;  vertices[22] = 0.0f; vertices[23] = 1.0f;
    
    vertices[24] = -1.0f; vertices[25] = 1.0f; vertices[26] = 0.0f;
    vertices[27] = 0.0f;  vertices[28] = 0.0f; 
    vertices[29] = 0.0f;  vertices[30] = 0.0f; vertices[31] = 1.0f;
    
    indices[0] = 0; indices[1] = 1; indices[2] = 2;
    indices[3] = 2; indices[4] = 3; indices[5] = 0;
    
    PGlVertexFormat vertexFormat;
    vertexFormat.addAttrib("position", 3, P_FLOAT32);
    vertexFormat.addAttrib("texcoord0", 2, P_FLOAT32);
    vertexFormat.addAttrib("normal", 3, P_FLOAT32);

    pfloat32 minPoint[] = {-1.0f, -1.0f, 0.0f};
    pfloat32 maxPoint[] = { 1.0f,  1.0f, 0.0f};
    PBox box(minPoint, maxPoint);

    return resourceManager->createMesh(vertices, 4, indices, 6,
            box, &vertexFormat, "plane", false, true);
}
