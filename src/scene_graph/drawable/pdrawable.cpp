// pdrawble.cpp
// The drawable objects
//
// Copyright 2012 - 2014 Future Interface. 
// This software is licensed under the terms of the MIT license.
//
// Hongwei Li lihw81@gmail.com and Hongwei Li (lihw81@gmail.com)
//
// 

#include "pdrawable.h"

#include <Paper3D/pabstractgeometry.h>
#include <Paper3D/pmaterial.h>
#include <Paper3D/pmesh.h>
#include <Paper3D/pscene.h>
#include <Paper3D/pgeometrymesh.h>
#include <Paper3D/presourcemanager.h>

#include <PFoundation/pcontext.h>
#include <PFoundation/pxmlelement.h>
#include <PFoundation/pmatrix4x4.h>
#include <PFoundation/pvector3.h>


P_OBJECT_DEFINE(PDrawable)

PDrawable::PDrawable(const pchar *name, PNode *parent, PDrawablePriorityEnum priority)
    : PNode(name, parent->scene(), parent, true)
{
    P_OBJECT_INITIALIZE_PROPERTY(PNode)

    P_OBJECT_REGISTER_PROPERTY("visible"        ,&m_visible);
    P_OBJECT_REGISTER_PROPERTY("pickable"       ,&m_pickable);
    P_OBJECT_REGISTER_PROPERTY("cast-shadow"    ,&m_castShadow);
    P_OBJECT_REGISTER_PROPERTY("receive-shadow" ,&m_receiveShadow);

    m_visible        = true;
    m_pickable       = true;
    m_castShadow     = false;
    m_receiveShadow  = false;
    m_geometry       = P_NULL;
    m_material       = P_NULL;
    m_priority       = priority;
}

PDrawable::PDrawable(const pchar *name, PScene *scene, PDrawablePriorityEnum priority)
    : PNode(name, scene, scene->root(), true)
{
    P_OBJECT_INITIALIZE_PROPERTY(PNode)

    P_OBJECT_REGISTER_PROPERTY("visible"        ,&m_visible);
    P_OBJECT_REGISTER_PROPERTY("pickable"       ,&m_pickable);
    P_OBJECT_REGISTER_PROPERTY("cast-shadow"    ,&m_castShadow);
    P_OBJECT_REGISTER_PROPERTY("receive-shadow" ,&m_receiveShadow);

    m_visible        = true;
    m_pickable       = true;
    m_castShadow     = false;
    m_receiveShadow  = false;
    m_geometry       = P_NULL;
    m_material       = P_NULL;
    m_priority       = priority;
}

PDrawable::~PDrawable()
{
    // Uninitialize property system
    P_OBJECT_UNINITIALIZE_PROPERTY();

    if (m_geometry != P_NULL)
    {
        m_geometry->release();
    }
    if (m_material != P_NULL)
    {
        m_material->release();
    }
}

void PDrawable::setVisible(pbool visible)
{
    m_visible = visible;
}

void PDrawable::setPickable(pbool pickable)
{
    m_pickable = pickable;
}

void PDrawable::setCastShadow(pbool castShadow)
{
    m_castShadow = castShadow;
}

void PDrawable::resume()
{
    // TODO:
    PASSERT_NOTIMPLEMENTED();
}

void PDrawable::pause()
{
    // TODO:
    PASSERT_NOTIMPLEMENTED();
}
    
void PDrawable::calculateModelCameraMatrix(const PMatrix4x4 &cameraMatrix, pfloat32 *out_matrix)
{
    PASSERT(out_matrix != P_NULL);
    pMatrix4x4Multiply(cameraMatrix.m_m, m_worldTransform.m_m, out_matrix);
}

void PDrawable::setGeometry(PAbstractGeometry *geometry)
{
    if (m_geometry != geometry)
    {
        if (m_geometry != P_NULL)
        {
            m_geometry->release();
        }

        m_geometry = geometry;

        if (m_geometry != P_NULL)
        {
            m_geometry->retain();

            if (m_geometry->isValid())
            {
                if (m_material != P_NULL && m_material->isValid())
                {
                    m_geometry->calibrate(m_material->vertexFormat());
                }
            }
        }
    }
}

void PDrawable::setMaterial(PMaterial *material)
{
    if (m_material != material)
    {
        if (m_material != P_NULL)
        {
            m_material->release();
        }

        m_material = material;

        if (m_material != P_NULL)
        {
            m_material->retain();

            if (m_material != P_NULL && m_material->isValid() && 
                m_geometry != P_NULL && m_geometry->isValid())
            {
                m_geometry->calibrate(m_material->vertexFormat());
            }
        }
    }
}
    
PDrawable *PDrawable::create(const pchar *name, 
                             PDrawable *parent, 
                             const pchar *geometry, 
                             const pchar *material)
{
    PResourceManager *resourceManager = parent->scene()->context()->module<PResourceManager>("resource-manager");

    PMaterial *materialInstance = PNEW(PMaterial(material, resourceManager));
    PAbstractGeometry *geometryInstance = PNEW(PGeometryMesh(geometry, resourceManager));
    if (materialInstance == P_NULL || geometryInstance == P_NULL)
    {
        return P_NULL;
    }
    
    PDrawable *drawable = PNEW(PDrawable(name, parent));
    drawable->setGeometry(geometryInstance);
    drawable->setMaterial(materialInstance);

    return drawable;
}

PDrawable *PDrawable::create(const pchar *name, 
                             PScene *scene, 
                             const pchar *geometry, 
                             const pchar *material)
{
    PResourceManager *resourceManager = scene->context()->module<PResourceManager>("resource-manager");


    PMaterial *materialInstance = PNEW(PMaterial(material, resourceManager));
    PAbstractGeometry *geometryInstance = PNEW(PGeometryMesh(geometry, resourceManager));
    if (materialInstance == P_NULL || geometryInstance == P_NULL)
    {
        return P_NULL;
    }
    
    PDrawable *drawable = PNEW(PDrawable(name, scene));
    drawable->setGeometry(geometryInstance);
    drawable->setMaterial(materialInstance);

    return drawable;
}
    
pbool PDrawable::unpack(const PXmlElement* xmlElement)
{
    // The drawable must be just created.
    PASSERT(m_children.isEmpty());
    PASSERT(m_material == P_NULL);
    PASSERT(m_geometry == P_NULL);
    
    PResourceManager *resourceManager = scene()->context()->module<PResourceManager>("resource-manager");

    // Unpack its children
    PXmlElement childElement = xmlElement->firstChild();
    while (childElement.isValid())
    {
        if (pstrcmp(childElement.name(), "material") == 0)
        {
            PMaterial *material = PMaterial::unpack(&childElement, resourceManager);
            if (material == P_NULL)
            {
                return false;
            }
            setMaterial(material);
        }
        else if (pstrcmp(childElement.name(), "geometry") == 0)
        {
            PAbstractGeometry *geometry = PAbstractGeometry::unpack(&childElement, resourceManager);
            if (geometry == P_NULL)
            {
                return false;
            }
            setGeometry(geometry);
        }
        else
        {
            PNode *node = PScene::s_nodeFactory.unpack(&childElement, this);
            if (node == P_NULL)
            {
                return false;
            }
        }

        childElement = childElement.nextSibling();
    } 

    if (m_material == P_NULL)
    {
        PLOG_ERROR("There is no valid material for PDrawable in this xml node.");
    }
    if (m_geometry == P_NULL)
    {
        PLOG_ERROR("There is no valid geometry for PDrawable in this xml node.");
    }

	pbool ret = PObject::unpack(xmlElement);

    // The PObject:unpack() will set the visiblity to false as the propertybool's default
    // value is false. However, for PDrawable, the default value of visibility is true.
	m_visible = true;

    return ret;
}

void PDrawable::prepareRender(PRenderState *renderState)
{
}

void PDrawable::postRender()
{
}
