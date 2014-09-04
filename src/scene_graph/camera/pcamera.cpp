// pcamera.cpp
// The rendering state
//
// Copyright 2012 - 2014 Future Interface. 
// This software is licensed under the terms of the MIT license.
//
// Hongwei Li lihw81@gmail.com
//

#include "pcamera.h"

#include <Paper3D/pscene.h>
#include <Paper3D/pdirectionallight.h>

#include <PFoundation/pcontext.h>
#include <PFoundation/pmatrix4x4.h>
#include <PFoundation/pvector3.h>
#include <PFoundation/pbox.h>
#include <PFoundation/pxmlelement.h>
#include <PFoundation/pconststring.h>
#include <PFoundation/passert.h>


P_OBJECT_DEFINE(PCamera)

PCamera::PCamera(const pchar *name, PNode *parent)
    : PNode(name, parent->scene(), parent, false)
{
    P_OBJECT_INITIALIZE_PROPERTY(PNode)
    P_OBJECT_REGISTER_PROPERTY("projection", &m_projection);
    P_OBJECT_REGISTER_PROPERTY("fixed",      &m_fixed);

    const puint32 *viewport = m_scene->context()->rect();
    setViewport(pfloat32(viewport[0]), 
                pfloat32(viewport[1]), 
                pfloat32(viewport[2]), 
                pfloat32(viewport[3])); 

    m_fixed = false;
}

PCamera::PCamera(const pchar *name, PScene *scene)
    : PNode(name, scene, scene->root(), false)
{
    P_OBJECT_INITIALIZE_PROPERTY(PNode)
    P_OBJECT_REGISTER_PROPERTY("projection", &m_projection);
    P_OBJECT_REGISTER_PROPERTY("fixed",      &m_fixed);
    
    const puint32 *viewport = m_scene->context()->rect();
    setViewport(pfloat32(viewport[0]), 
                pfloat32(viewport[1]), 
                pfloat32(viewport[2]), 
                pfloat32(viewport[3])); 

    
    m_fixed = false;
}

PCamera::~PCamera()
{
    P_OBJECT_UNINITIALIZE_PROPERTY()
}
    
PVector3 PCamera::cameraPosition() const
{
    PVector3 v;

    v.m_v[0] = -pVector3Dot(&m_cameraTransform.m_m[0], &m_cameraTransform.m_m[12]);
    v.m_v[1] = -pVector3Dot(&m_cameraTransform.m_m[4], &m_cameraTransform.m_m[12]);
    v.m_v[2] = -pVector3Dot(&m_cameraTransform.m_m[8], &m_cameraTransform.m_m[12]);

    return v;
}

void PCamera::setViewport(pfloat32 x, pfloat32 y, pfloat32 width, pfloat32 height)
{
    m_viewport = pVector4(x, y, width, height);

    switch (m_projection.projection())
    {
        case P_PROJECTION_PERSPECTIVE:
            {
                pfloat32 aspect = width / height;
                m_projection.perspective(m_projection.fov(), 
                                         aspect, 
                                         m_projection.zNear(), 
                                         m_projection.zFar());
            }
            break;
        case P_PROJECTION_ORTHOGONAL:
            {
                pfloat32 aspect = width / height;
                m_projection.orthogonal(aspect, 
                                        m_projection.zNear(), 
                                        m_projection.zFar());
            }
            break;
        case P_PROJECTION_WINDOW:
            m_projection.window(width, height);
            break;
        default:
            PASSERT_NOTREACHABLE("Unknown projection type.");
            break;
    }
}

void PCamera::setFixed(pbool fixed)
{
    m_fixed = fixed;
}
    
void PCamera::updateWorldTransform()
{
    // -------------------------------------------------------------- 
    // Update this world transformation matrix.
    // -------------------------------------------------------------- 
    bool localTransformChanged = false;
    const PMatrix4x4 &localTransform = m_localTransform.toMatrix4x4(&localTransformChanged);

    if (m_fixed.toBool())
    {
        m_worldTransformChanged = localTransformChanged;
        if (m_worldTransformChanged)
        {
            m_worldTransform = localTransform;
            m_cameraTransform = m_localTransform.toCameraMatrix4x4();
        }
    }
    else
    {
        // FIXME: not validated!!!

        // Update the this world transformation either when the parent has its own 
        // or the local transformation is changed.
        if (m_parent->worldTransformChanged() || localTransformChanged)
        {
            m_worldTransform =  m_parent->worldTransform() * localTransform;
            // Camera matrix is the inverse of the transform matrix.
            pMatrix4x4AffineInverse(m_worldTransform.m_m, m_cameraTransform.m_m);
            m_worldTransformChanged = true;
        }
        else
        {
            m_worldTransformChanged = false;
        }
    }
}
    
void PCamera::copyOf(const PCamera &other)
{
    if (this != &other)
    {
        m_worldTransform   = other.m_worldTransform;
        m_localTransform   = other.m_localTransform;
        m_viewport         = other.m_viewport;
        m_projection       = other.m_projection;
        m_cameraTransform  = other.m_cameraTransform;
    }
}

void PCamera::reflect(const PPlane &plane)
{
    pfloat32 reflection[16];
    pMatrix4x4CreateReflection(plane, reflection);

    PMatrix4x4 tmp;
    pMatrix4x4Multiply(m_cameraTransform.m_m, reflection, tmp.m_m);
    m_cameraTransform = tmp;
}

void PCamera::oblique(const PPlane &plane)
{
    PMatrix4x4 inversedProjection = m_projection.toInversedMatrix4x4();
    PVector4 q = inversedProjection * pVector4(pSign(plane.A()), pSign(plane.B()), 1.0f, 1.0f);
    PVector4 c = plane.toVector4() * 2.0f * (1.0f / (plane.toVector4().dot(q)));

    // New projection matrix.
    PMatrix4x4 proj = m_projection.toMatrix4x4();

    proj.m_m[2]  = c[0] - proj.m_m[3];
    proj.m_m[6]  = c[1] - proj.m_m[7];
    proj.m_m[10] = c[2] - proj.m_m[11];
    proj.m_m[14] = c[3] - proj.m_m[15];

    m_projection.setMatrix4x4(proj.m_m);
}
    
pbool PCamera::unpack(const PXmlElement* xmlElement)
{
    if (PNode::unpack(xmlElement))
    {
        // When the aspect, width or height of the projection is left to be default value,
        // we need to explicitly set them.
        const puint32 *rect = scene()->context()->rect();
        if (m_projection.projection() == P_PROJECTION_ORTHOGONAL)
        {
            if (m_projection.aspect() < 0)
            {
                pfloat32 aspect = ((pfloat32)rect[2] / (pfloat32)rect[3]);
                m_projection.orthogonal(aspect, m_projection.zNear(), m_projection.zFar());
            }
        }
        else if (m_projection.projection() == P_PROJECTION_PERSPECTIVE)
        {
            if (m_projection.aspect() < 0)
            {
                pfloat32 aspect = ((pfloat32)rect[2] / (pfloat32)rect[3]);
                m_projection.perspective(m_projection.fov(), aspect, m_projection.zNear(), m_projection.zFar());
            }
        }
        else if (m_projection.projection() == P_PROJECTION_WINDOW)
        {
            if (m_projection.width() < 0 || m_projection.height())
            {
                m_projection.window((pfloat32)rect[2], (pfloat32)rect[3]);
            }
        }
        else
        {
            PASSERT_NOTREACHABLE("Unknown projection type.");
        }

        // See if there is any lookat attribute and the transform attribute is not used.
        // In this case, the lookat property will be used to set the transform the camera.
        const pchar *lookatValue = xmlElement->attribute("lookat");
        const pchar *transformValue = xmlElement->attribute("transform");
        if (lookatValue != P_NULL && transformValue == P_NULL)
        {
            const pchar *p = lookatValue;

            pfloat32 eyex, eyey, eyez;
            pfloat32 centerx, centery, centerz;
            pfloat32 upx, upy, upz;

            if ((p = pStringUnpackTrimAnnotation(p)) != P_NULL &&
                (p = pStringUnpackFloat(p, &eyex)) != P_NULL &&
                (p = pStringUnpackFloat(p, &eyey)) != P_NULL &&
                (p = pStringUnpackFloat(p, &eyez)) != P_NULL &&
                (p = pStringUnpackTrimAnnotation(p)) != P_NULL &&
                (p = pStringUnpackFloat(p, &centerx)) != P_NULL &&
                (p = pStringUnpackFloat(p, &centery)) != P_NULL &&
                (p = pStringUnpackFloat(p, &centerz)) != P_NULL &&
                (p = pStringUnpackTrimAnnotation(p)) != P_NULL &&
                (p = pStringUnpackFloat(p, &upx)) != P_NULL &&
                (p = pStringUnpackFloat(p, &upy)) != P_NULL &&
                (p = pStringUnpackFloat(p, &upz)) != P_NULL)
            {
                m_localTransform.setLookAt(eyex, eyey, eyez,
                                           centerx, centery, centerz,
                                           upx, upy, upz);
            }
            else
            {
                PLOG_ERROR("The camera %s has a valid lookat property in the xml node.", m_name.toString().c_str());
            }
        }

        return true;
    }

    return false;
}

void PCamera::fromLight(const PDirectionalLight *light, const PBox *bbox)
{
    PVector3 center = pVector3(0, 0, 0);
    PVector3 eye = light->direction() * 100.0f; // any point on the light ray is OK.
    PVector3 up;
    PVector3 lightSource = pVector3(-light->direction()[0],
                                    -light->direction()[1],
                                    -light->direction()[2]);
    if (lightSource[1] >= 0)
    {
        if (lightSource[1] < 0.99f)
        {
            up = pVector3(0, 1, 0);
        }
        else
        {
            up = pVector3(-1.0f, 0.0f, 0.0f);
        }
    }
    else
    {
        if (lightSource[1] > -0.99f)
        {
            up = pVector3(0, -1, 0);
        }
        else
        {
            up = pVector3(-1.0f, 0.0f, 0.0f);
        }
    }

    up.normalize();

    // Generate the camera matrix
    m_localTransform.setLookAt(eye[0], eye[1], eye[2], center[0], center[1], center[2], up[0], up[1], up[2]);

    PBox bboxInCameraSpace(*bbox);
    bboxInCameraSpace.transform(cameraTransform());

    // Generate the projection matrix
    m_projection.orthogonal(bboxInCameraSpace.min()[0],
                            bboxInCameraSpace.max()[0],
                            bboxInCameraSpace.min()[1],
                            bboxInCameraSpace.max()[1],
                            bboxInCameraSpace.min()[2] - 0.01f,
                            bboxInCameraSpace.max()[2] + 0.01f);
}