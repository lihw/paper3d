// pabstractgeometry.cpp
// The geometry data of a drawable.
//
// Copyright 2012 - 2014 Future Interface. 
// This software is licensed under the terms of the MIT license.
//
// Hongwei Li lihw81@gmail.com and Hongwei Li lihw81@gmail.com
//
// 

#include "pabstractgeometry.h"

#include <Paper3D/prenderstate.h>
#include <Paper3D/pmesh.h>
#include <Paper3D/pgeometryplane.h>
#include <Paper3D/pgeometrysphere.h>
#include <Paper3D/pgeometrycube.h>
#include <Paper3D/pgeometrymesh.h>

#include <PFoundation/pxmlelement.h>

#include <PFoundation/pglvertexbuffer.h>
#include <PFoundation/pglstate.h>
#include <PFoundation/pcrt.h>
#include <PFoundation/pnew.h>


PAbstractGeometry::PAbstractGeometry()
{
    m_mesh       = P_NULL;
    m_calibrated = false;
    m_cullMode   = -1;
}

PAbstractGeometry::~PAbstractGeometry()
{
    if (m_mesh != P_NULL)
    {
        m_mesh->release();
    }
}
    
void PAbstractGeometry::calibrate(PGlVertexFormat *vertexFormat)
{
    m_calibrated = PGlVertexFormat::calibrate(m_mesh->vertexBufferObject()->vertexFormat(),
        vertexFormat, &m_vertexFormat);
}

void PAbstractGeometry::render(PRenderState *renderState)
{
    PASSERT(m_mesh != P_NULL);
    if (m_mesh != P_NULL && m_calibrated)
    {
        PGlState *rso = renderState->renderStateObject();
        PGlStateEnum oldCullMode = rso->cull();
        if (m_cullMode != -1)
        {
            rso->setCull((PGlStateEnum)m_cullMode);
        }

        renderState->useVertexBuffer(m_mesh);
        // FXIME: we don't disable previous vertex format on the pipeline. It may
        // cause corrupted geometric object. But we are sure the current bound shader 
        // are fitting tightly with this vertex format. So even there are some loose
        // vertex attributes in this vertex format which are from previous one, it
        // won't be used by the shader.
        m_vertexFormat.enable(m_mesh->vertexBufferObject()->vertices());
        m_mesh->vertexBufferObject()->render();

        rso->setCull(oldCullMode);
    }
}

void PAbstractGeometry::setCullMode(pint32 mode)
{
    PASSERT(mode == -1 || (mode >= P_GLCULL_FIRST && mode <= P_GLCULL_LAST));
    m_cullMode = mode;
}
    
PAbstractGeometry *PAbstractGeometry::unpack(PXmlElement *element, PResourceManager *resourceManager)
{
    PASSERT(element->isValid());

    PAbstractGeometry *ret;

    const pchar *idValue = element->attribute("id");
    if (idValue == P_NULL)
    {
        PLOG_ERROR("Failed to find the id of the geometry in this xml node.");
        return P_NULL;
    }

    if (pstrcmp(idValue, "sphere") == 0)
    {
        ret = PNEW(PGeometrySphere(resourceManager));
    }
    else if (pstrcmp(idValue, "cube") == 0)
    {
        ret = PNEW(PGeometryCube(resourceManager));
    }
    else if (pstrcmp(idValue, "plane") == 0)
    {
        ret = PNEW(PGeometryPlane(resourceManager));
    }
    else
    {
        ret = PNEW(PGeometryMesh(idValue, resourceManager));
    }

    const pchar *cullModeValue = element->attribute("cull");
    if (cullModeValue != P_NULL)
    {
        pint32 cullMode = -1;

        if (pstrcmp(cullModeValue, "front") == 0)
        {
            cullMode = P_GLCULL_FRONT;
        }
        else if (pstrcmp(cullModeValue, "back") == 0)
        {
            cullMode = P_GLCULL_BACK;
        }
        else if (pstrcmp(cullModeValue, "none") == 0)
        {
            cullMode = P_GLCULL_NONE;
        }
        
        ret->setCullMode(cullMode);
    }


    return ret;
}
