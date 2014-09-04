// pcamera.h
// The camera
//
// Copyright 2012 - 2014 Future Interface. 
// This software is licensed under the terms of the MIT license.
//
// Hongwei Li lihw81@gmail.com
//

#ifndef PCAMERA_H
#define PCAMERA_H

#include <Paper3D/pnode.h>

#include <PFoundation/ppropertyprojection.h>
#include <PFoundation/ppropertybool.h>
#include <PFoundation/pmatrix4x4.h>
#include <PFoundation/pplane.h>

// TODO: free camera, camera mesh and thir-person view camera for debugging.

class PScene;
class PBox;
class PDirectionalLight;


class P_DLLEXPORT PCamera : public PNode
{
    P_OBJECT

    friend class PCameraMisc;

    PCamera(const PCamera &other) : PNode(P_NULL, P_NULL, P_NULL, false) {}
    void operator=(const PCamera &other) {}

public:
    PCamera(const pchar *name, PNode *parent);
    PCamera(const pchar *name, PScene *scene);
    ~PCamera();

    P_INLINE const PMatrix4x4& cameraTransform() const { return m_cameraTransform; }
    P_INLINE const PPropertyProjection& projection() const { return m_projection; }
    P_INLINE PPropertyProjection& projection() { return m_projection; }
    P_INLINE const PVector4 &viewport() const { return m_viewport; }

    // Get the current camera position.
    PVector3 cameraPosition() const;
    // Whether the camera will be affected by the parent's transform.
    void setFixed(pbool fixed);
    // When the size of viewport changed, we need to adjust the camera
    // accordingly.
    void setViewport(pfloat32 x, pfloat32 y, pfloat32 width, pfloat32 height);
    // Copy from another camera. The projection, viewport and transform are copied.
    void copyOf(const PCamera &other);
    // Reflect this camera. It modifies the camera transform but won't change
    // local and world transform of the camera.
    void reflect(const PPlane &plane);
    // Construct this camera to be an oblique one to certain plane.
    // FIXME: buggy! See reflection example.
    void oblique(const PPlane &plane);
    // Construct the camera a directional light, used in shadowmap generation.
    // bbox is the bounding box of the scene.
    void fromLight(const PDirectionalLight *light, const PBox *bbox);
    
    virtual pbool unpack(const PXmlElement* xmlElement);

protected:
    virtual void updateWorldTransform();

protected:
    PPropertyProjection m_projection;
    PVector4            m_viewport;
    PMatrix4x4          m_cameraTransform; // The inverse of the world transform
    PPropertyBool       m_fixed;
};

#endif // !PCAMERA_H
