// pgeometrysphere.cpp
// The geometry sphere
//
// Copyright 2012 - 2014 Future Interface . 
// This software is licensed under the terms of the MIT license.
//
// Hongwei Li lihw81@gmail.com
//
// 

#include "pgeometrysphere.h"

#include <Paper3D/presourcemanager.h>
#include <Paper3D/pmesh.h>

#include <PFoundation/pmathutility.h>

#include <PFoundation/pglvertexbuffer.h>

#include <math.h>


PGeometrySphere::PGeometrySphere(PResourceManager *resourceManager)
{
    m_mesh = dynamic_cast<PMesh *>(resourceManager->findResourceObject("sphere"));
    if (m_mesh == P_NULL)
    {
        m_mesh = createMesh(resourceManager);
        PASSERT(m_mesh != P_NULL);
    }

    m_mesh->retain();
}

PGeometrySphere::~PGeometrySphere()
{
    
}
    
PMesh *PGeometrySphere::createMesh(PResourceManager *resourceManager)
{
    // -------------------------------------------------------------- 
    // Create the sphere vertex and index buffer
    // -------------------------------------------------------------- 
    puint32 numVertices = (P_SPHERE_RING_NUMBER + 1) * (P_SPHERE_SEGMENT_NUMBER + 1);
    puint32 numIndices = 6 * P_SPHERE_RING_NUMBER * (P_SPHERE_SEGMENT_NUMBER + 1);

    pfloat32 *vertices = PNEWARRAY(pfloat32, numVertices * 8);
    puint16 *indices  = PNEWARRAY(puint16, numIndices); 

    pfloat32 *pVertex = vertices;
    puint16 *pIndex = indices;

    pfloat32 deltaRingAngle = P_PI / P_SPHERE_RING_NUMBER;
    pfloat32 deltaSegAngle = (2.0f * P_PI / P_SPHERE_SEGMENT_NUMBER);

    puint16 verticeIndex = 0;

    // Generate the group of rings for the sphere
    for (puint32 ring = 0; ring <= P_SPHERE_RING_NUMBER; ++ring) 
    {
        pfloat32 r0 = sinf(ring * deltaRingAngle);
        pfloat32 y0 = cosf(ring * deltaRingAngle);

        // Generate the group of segments for the current ring
        for(puint32 seg = 0; seg <= P_SPHERE_SEGMENT_NUMBER; seg++) 
        {
            pfloat32 x0 = r0 * sinf(seg * deltaSegAngle);
            pfloat32 z0 = r0 * cosf(seg * deltaSegAngle);

            // Add one vertex to the strip which makes up the sphere
            
            // Position.
            *pVertex++ = x0;
            *pVertex++ = y0;
            *pVertex++ = z0;

            // Texture coordinate
            *pVertex++ = (pfloat32)seg / (pfloat32)P_SPHERE_SEGMENT_NUMBER;
            *pVertex++ = (pfloat32)ring / (pfloat32)P_SPHERE_RING_NUMBER;
            
            // Normal
            pfloat32 n[] = {x0, y0, z0};
            pVector3Normalize(n, pVertex);
            pVertex += 3;

            if (ring != P_SPHERE_RING_NUMBER) 
            {
                // each vertex (except the last) has six indices pointing to it
                *pIndex++ = verticeIndex + P_SPHERE_SEGMENT_NUMBER + 1;
                *pIndex++ = verticeIndex;               
                *pIndex++ = verticeIndex + P_SPHERE_SEGMENT_NUMBER;
                *pIndex++ = verticeIndex + P_SPHERE_SEGMENT_NUMBER + 1;
                *pIndex++ = verticeIndex + 1;
                *pIndex++ = verticeIndex;
                verticeIndex ++;
            }
        } // end for seg
    } // end for ring
    
    PGlVertexFormat vertexFormat;
    vertexFormat.addAttrib("position", 3, P_FLOAT32);
    vertexFormat.addAttrib("texcoord0", 2, P_FLOAT32);
    vertexFormat.addAttrib("normal", 3, P_FLOAT32);
    
    pfloat32 minPoint[] = {-1.0f, -1.0f, -1.0f};
    pfloat32 maxPoint[] = { 1.0f,  1.0f,  1.0f};
    PBox box(minPoint, maxPoint);

    PMesh *ret = resourceManager->createMesh(vertices, numVertices, indices, numIndices,
        box, &vertexFormat, "sphere", false, true);

    PDELETEARRAY(vertices);
    PDELETEARRAY(indices);

    return ret;
}
