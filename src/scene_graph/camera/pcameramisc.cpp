// pcameramisc.cpp
// Some misc functions related to camera
//
// Copyright 2013 Cooee. 
// This software is licensed under the terms of the MIT license.
//
// Hongwei Li lihw81@gmail.com
//

#include "pcameramisc.h"

#include <PCore/pvector4.h>

puint32 PCameraMisc::getGroundPolygon(const PCamera *camera,
                                      const PPlane &plane, 
                                      PVector3 *out_corners) 
{
    // Only works for perspective projection right now.
    PASSERT(camera->projection().getProjection() == P_PROJECTION_PERSPECTIVE);
    if (camera->projection().getProjection() == P_PROJECTION_PERSPECTIVE)
    {
        // Check whether the plane face the camera
        pfloat32 facing = plane.distance(camera->position().m_v);

        if (facing < 0)
        {
            // Camera is behind the plane, hence it is not visible
            return 0;
        }

        // Find 4 corners of the far clipping plane in the world space.
        pfloat32 w;
        pfloat32 h;
        pfloat32 z = camera->projection().getZFar();
        
        pfloat32 tangent = tanf(camera->projection().getFov() * 0.5f);

        w = tangent * z;
        h = w / camera->projection().getAspect();

        pfloat32 farInCamera[5][3]; // far plane in camera space
        pfloat32 farInWorld[5][3]; // far plane in world space

        farInCamera[0][0] = -w; farInCamera[0][1] = -h; farInCamera[0][2] = -z;
        farInCamera[1][0] =  w; farInCamera[1][1] = -h; farInCamera[1][2] = -z;
        farInCamera[2][0] =  w; farInCamera[2][1] =  h; farInCamera[2][2] = -z;
        farInCamera[3][0] = -w; farInCamera[3][1] =  h; farInCamera[3][2] = -z;
        
        PMatrix4x4 invCameraMatrix = camera->getInversedCameraMatrix();

        pMatrix4x4MultiplyVector3(invCameraMatrix.m_m, farInCamera[0], farInWorld[0]);
        pMatrix4x4MultiplyVector3(invCameraMatrix.m_m, farInCamera[1], farInWorld[1]);
        pMatrix4x4MultiplyVector3(invCameraMatrix.m_m, farInCamera[2], farInWorld[2]);
        pMatrix4x4MultiplyVector3(invCameraMatrix.m_m, farInCamera[3], farInWorld[3]);

        // Repeat the first 1st corner to make it a closed loop.
        farInWorld[4][0] = farInWorld[0][0];
        farInWorld[4][1] = farInWorld[0][1];
        farInWorld[4][2] = farInWorld[0][2];
    
        // Now build a pre-clipped polgyon
        puint32 numCorners = 0;
        pbool clipping = false;

        puint32 i = 5;

        while (i != 0)
        {
            i--;

            // Check if the corner of far clipping plane is behind the plane. Only if
            // it is in front of the plane, we need to do clipping.
            pfloat32 facing = plane.distance(farInWorld[i]);

            if (facing < 0)
            {
                if (clipping)
                {
                    clipping = false;

                    plane.intersect(PLine(farInWorld[i + 1], farInWorld[i]), &out_corners[numCorners]);
                    numCorners++;
                }
                if (i == 0)
                {
                    break;
                }

                plane.intersect(PLine(camera->position().m_v, farInWorld[i]), &out_corners[numCorners]);
                numCorners++;
            }
            else
            {
                if (clipping)
                {
                    // Already in clipping, skip this corner
                    continue;
                }
                
                // We're now in the front of plane, we then find the clipping point.
                clipping = true;

                // No need to bother first point as it will be taken care when dealing with last one
                if (i != 4)
                {
                    plane.intersect(PLine(farInWorld[i + 1], farInWorld[i]), &out_corners[numCorners]);
                    numCorners++;
                }
            }
        }

        // Valid vertex counts are 0, 3, 4, 5 
        PASSERT(numCorners <= 5);
        PASSERT(numCorners != 1);
        PASSERT(numCorners != 2);

        return numCorners;
    }

    return 0;
}

PMatrix4x4 PCameraMisc::getObliqueProjectionMatrix(const PCamera *camera, const PPlane &plane)
{
    pfloat32 planeInCameraSpace[4];
    PMatrix4x4 transInvCameraMatrix = camera->getTransposedInversedCameraMatrix();
    pMatrix4x4MultiplyVector4(transInvCameraMatrix.m_m, (const pfloat32 *)&plane, planeInCameraSpace);

    pfloat32 Qprime[] = {pSign(planeInCameraSpace[0]), pSign(planeInCameraSpace[1]), 1, 1};

    PMatrix4x4 inversedProjectionMatrix = camera->getInversedProjectionMatrix();

    pfloat32 Q[4];

    pfloat32 planeInCameraSpace[4];
    PMatrix4x4 transInvCameraMatrix = camera->getTransposedInversedCameraMatrix();
    pMatrix4x4MultiplyVector4(transInvCameraMatrix.m_m, (const pfloat32 *)&plane, planeInCameraSpace);

    pfloat32 Qprime[] = {pSign(planeInCameraSpace[0]), pSign(planeInCameraSpace[1]), 1, 1};

    PMatrix4x4 inversedProjectionMatrix = camera->getInversedProjectionMatrix();

    pfloat32 Q[4];
    pMatrix4x4MultiplyVector4(inversedProjectionMatrix.m_m, Qprime, Q);

    PMatrix4x4 ret = camera->getProjectionMatrix();

    pfloat32 M4[] = 
    {
        ret[3], 
        ret[7], 
        ret[11], 
        ret[15]
    };

    pfloat32 scaling = 2.0f * pVector4Dot(M4, Q) / pVector4Dot(planeInCameraSpace, Q);

    ret[2]  = scaling * planeInCameraSpace[0] - ret[3];
    ret[6]  = scaling * planeInCameraSpace[1] - ret[7];
    ret[10] = scaling * planeInCameraSpace[2] - ret[11];
    ret[14] = scaling * planeInCameraSpace[3] - ret[15];

    return ret;
}
