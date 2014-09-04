// psprite2d.cpp
// A sprite in the normalized window coordinate which is not affected
// by the scene camera.
//
// Copyright 2012 - 2014 Future Interface. 
// This software is licensed under the terms of the MIT license.
//
// Hongwei Li lihw81@gmail.com

#include "psprite2d.h"

#include <Paper3D/presourcemanager.h>
#include <Paper3D/pscene.h>
#include <Paper3D/pmaterial.h>
#include <Paper3D/prenderstate.h>

#include <PFoundation/pcontext.h>
#include <PFoundation/pglstate.h>

P_OBJECT_DEFINE(PSprite2D)

static const pchar *SPRITE2D_PMT = 
    "<material>\n"
    "  <shader>\n"
    "    <vs>\n"
    "      attribute mediump vec3 position;\n"
    "      attribute mediump vec2 texcoord0;\n"
    "      varying mediump vec2 TexCoord;\n"
    "      uniform vec4 TexCoordMetric;\n"
    "      uniform mat4 MV; \n"
    "\n"
    "      void main()\n"
    "      {\n"
    "          gl_Position = MV * vec4(position.xyz, 1.0);\n"
    "          TexCoord    = TexCoordMetric.xy * texcoord0 + TexCoordMetric.zw;\n" // t * scale + offset
    "      }\n"
    "    </vs>\n"
    "    <fs>\n"
    "      precision mediump float;\n"
    "\n"
    "      uniform sampler2D Tex;\n"
    "      varying mediump vec2 TexCoord;\n"
    "      \n"
    "      void main()\n"
    "      {\n"
    "           gl_FragColor = texture2D(Tex, TexCoord.xy).rgba;\n"
    "      }\n"
    "    </fs>\n"
    "  </shader>\n"
    "  <parameters>\n"
    "    <parameter name=\"mv\" uniform=\"MV\" source=\"transform\"/>"
    "    <parameter name=\"texture\" uniform=\"Tex\" source=\"material\"/>\n"
    "    <parameter name=\"texcoordmetric\" uniform=\"TexCoordMetric\" source=\"material\"/>\n"
    "  </parameters>\n"
    "</material>\n";

PSprite2D::PSprite2D(const pchar *name, PScene *scene)
    : PSprite(name, scene)
{
    P_OBJECT_INITIALIZE_PROPERTY(PSprite)
    
    PResourceManager *resMgr = scene->context()->module<PResourceManager>("resource-manager");
    setMaterial(PNEW(PMaterial("internal/sprite2d.pmt", SPRITE2D_PMT, false, resMgr)));

    // TODO: discard the sprite.pmt loaded in PSprite
}

PSprite2D::~PSprite2D()
{
    P_OBJECT_UNINITIALIZE_PROPERTY();
}

void PSprite2D::prepareRender(PRenderState *renderState)
{
    puint32 viewport[4];
    renderState->renderStateObject()->getViewport(viewport);
    m_invAspect = (pfloat32)viewport[3] / (pfloat32)viewport[2];
}

void PSprite2D::calculateModelCameraMatrix(const PMatrix4x4 &cameraMatrix, 
        pfloat32 *out_matrix)
{
    pMatrix4x4Identity(out_matrix);
    
    out_matrix[0] = m_localTransform.scaling()[0];
    out_matrix[5] = m_localTransform.scaling()[1];
    out_matrix[10] = m_localTransform.scaling()[2];

    out_matrix[12] = m_localTransform.translation()[0];
    out_matrix[13] = m_localTransform.translation()[1];
    out_matrix[14] = m_localTransform.translation()[2];

 
    out_matrix[0]  *= m_invAspect;
    out_matrix[12] *= m_invAspect;

    // The valid range of depth of sprite2d is [-1, 0];
    out_matrix[14] = -out_matrix[14];
}

pbool PSprite2D::unpack(const PXmlElement* xmlElement)
{
    return PSprite::unpack(xmlElement);
}
