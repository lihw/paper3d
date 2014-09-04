// pskybox.cpp
// Shader based water wave effects with reflection and refraction.
//
// Copyright 2012 - 2014 Future Interface . 
// This software is licensed under the terms of the MIT license.
//
// Hongwei Li lihw81@gmail.com
//

#include "pskybox.h"

#include <Paper3D/ptexture.h>
#include <Paper3D/pscene.h>
#include <Paper3D/pgeometrycube.h>
#include <Paper3D/pmaterial.h>
#include <Paper3D/pmaterialparameter.h>
#include <Paper3D/presourcemanager.h>

#include <PFoundation/pcontext.h>
#include <PFoundation/pxmlelement.h>
#include <PFoundation/pglstate.h>


P_OBJECT_DEFINE(PSkyBox)

// FIXME: use brief version of variable names to make this string shorter.
static const pchar *SKYBOX_PMT = 
"<material>\n"
"  <shader>\n"
"    <vs>\n"
"      uniform mat4 MVP;\n"
"\n"
"      attribute mediump vec3 position;\n"
"\n"
"      varying mediump vec3 vTexCoord;\n"
"\n"
"      void main()\n"
"      {\n"
"          vec4 pos = MVP * vec4(position, 1);\n"
"          gl_Position = pos.xyww;\n"
"          vTexCoord = position;\n"
"      }\n"
"    </vs>\n"
"    <fs>\n"
"      precision mediump float;\n"
"\n"
"      uniform samplerCube Texture;\n"
"\n"
"      varying mediump vec3 vTexCoord;\n"
"\n"
"      void main()\n"
"      {\n"
"          gl_FragColor = textureCube(Texture, vTexCoord);\n"
"      }\n"
"    </fs>\n"
"  </shader>\n"
"  <parameters>\n"
"    <parameter name=\"mvp\" uniform=\"MVP\" source=\"transform\"/>\n"
"    <parameter name=\"texture\" uniform=\"Texture\" source=\"material\"/>\n"
"  </parameters>\n"
"</material>\n";


//
// http://ogldev.atspace.co.uk/www/tutorial25/tutorial25.html
//
PSkyBox::PSkyBox(const pchar *name, PScene *scene)
    : PDrawable(name, scene, P_DRAWABLE_PRIORITY_DRAWLAST)
{
    P_OBJECT_INITIALIZE_PROPERTY(PSkyBox)
    
    PResourceManager *resourceManager = scene->context()->module<PResourceManager>("resource-manager");

    m_texture  = P_NULL;

    setGeometry(PNEW(PGeometryCube(resourceManager)));
    setMaterial(PNEW(PMaterial("internal/skybox.pmt", SKYBOX_PMT, false, resourceManager)));

    geometry()->setCullMode(P_GLCULL_FRONT);
}

PSkyBox::~PSkyBox()
{
    P_OBJECT_UNINITIALIZE_PROPERTY();

    if (m_texture != P_NULL)
    {
        m_texture->release();
    }
}

void PSkyBox::setTexture(PTexture *texture)
{
    if (m_texture != texture)
    {
        if (m_texture != P_NULL)
        {
            m_texture->release();
        }
        m_texture = texture;
        if (m_texture != P_NULL)
        {
            m_texture->retain();
        }

        material()->parameter("texture") = texture;
    }
}
    
void PSkyBox::update()
{
    if (m_texture == P_NULL)
    {
        PLOG_ERROR("Fail to render a sky box without a valid texture.");
    }
}

void PSkyBox::calculateModelCameraMatrix(const PMatrix4x4 &cameraMatrix, pfloat32 *out_matrix)
{
    PASSERT(out_matrix != P_NULL);
    pMatrix4x4Multiply(cameraMatrix.m_m, m_worldTransform.m_m, out_matrix);
    // Remove the translation.
    out_matrix[12] = 0;
    out_matrix[13] = 0;
    out_matrix[14] = 0;
}

pbool PSkyBox::unpack(const PXmlElement* xmlElement)
{
    PASSERT(xmlElement->isValid());

    const pchar *textureName = xmlElement->attribute("texture");
    if (textureName != P_NULL)
    {
        PResourceManager *resourceManager = scene()->context()->module<PResourceManager>("resource-manager");
        m_texture = resourceManager->getTexture(textureName);
        setTexture(m_texture);
        return true;
    }
    else
    {
        PLOG_ERROR("Failed to find the texture for PBackground in this xml node.");
        return false;
    }

    return false;
}

