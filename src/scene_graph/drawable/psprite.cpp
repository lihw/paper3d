// psprite.cpp
// A sprite is a quad which always faces the camera.
//
// Copyright 2012 - 2014 Future Interface. 
// This software is licensed under the terms of the MIT license.
//
// Hongwei Li lihw81@gmail.com
//

#include "psprite.h"

#include <Paper3D/presourcemanager.h>
#include <Paper3D/pscene.h>
#include <Paper3D/pgeometryplane.h>
#include <Paper3D/pmaterial.h>
#include <Paper3D/pmaterialparameter.h>
#include <Paper3D/ptexture.h>

#include <PFoundation/pclock.h>
#include <PFoundation/passert.h>
#include <PFoundation/pcontext.h>
#include <PFoundation/pxmlelement.h>
#include <PFoundation/pconststring.h>

#include <math.h>


P_OBJECT_DEFINE(PSprite)


static const pchar *SPRITE_PMT = 
    "<material>\n"
    "  <shader>\n"
    "    <vs>\n"
    "      attribute mediump vec3 position;\n"
    "      attribute mediump vec2 texcoord0;\n"
    "      varying mediump vec2 TexCoord;\n"
    "      uniform vec4 TexCoordMetric;\n"
    "      uniform mat4 MVP; \n"
    "\n"
    "      void main()\n"
    "      {\n"
    "          gl_Position = MVP * vec4(position.xyz, 1.0);\n"
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
    "    <parameter name=\"mvp\" uniform=\"MVP\" source=\"transform\"/>"
    "    <parameter name=\"texture\" uniform=\"Tex\" source=\"material\"/>\n"
    "    <parameter name=\"texcoordmetric\" uniform=\"TexCoordMetric\" source=\"material\"/>\n"
    "  </parameters>\n"
    "</material>\n";

PSprite::PSprite(const pchar *name, PScene *scene)
    : PDrawable(name, scene)
{
    P_OBJECT_INITIALIZE_PROPERTY(PDrawable)

    PResourceManager *resourceManager = scene->context()->module<PResourceManager>("resource-manager");

    m_textureAtlas[0] = P_NULL;
    m_textureAtlas[1] = P_NULL;
    m_textureAtlas[2] = P_NULL;
    m_textureAtlas[3] = P_NULL;

    m_numRows = 1;
    m_numCols = 1;

    m_numFrames = 1;
    m_duration  = 0;

    m_currentIndex = 0;
    m_previousIndex = 0xffffffff;
    m_previousTextureId = 0xffffffff;

    setGeometry(PNEW(PGeometryPlane(resourceManager)));
    setMaterial(PNEW(PMaterial("internal/sprite.pmt", SPRITE_PMT, false, resourceManager)));
}

PSprite::~PSprite()
{
    P_OBJECT_UNINITIALIZE_PROPERTY();

    if (m_textureAtlas[0] != P_NULL)  m_textureAtlas[0]->release();
    if (m_textureAtlas[1] != P_NULL)  m_textureAtlas[1]->release();
    if (m_textureAtlas[2] != P_NULL)  m_textureAtlas[2]->release();
    if (m_textureAtlas[3] != P_NULL)  m_textureAtlas[3]->release();
}

void PSprite::setTextureAtlas(PTexture **textures, puint32 numTextures, 
    puint32 numRows, puint32 numCols, pbool transparency)
{
    PASSERT(numTextures < 4);

    for (puint32 i = 0; i < numTextures; ++i)
    {
        if (m_textureAtlas[i] != P_NULL)
        {
            m_textureAtlas[i]->release();
        }
        m_textureAtlas[i] = textures[i];
        if (m_textureAtlas != P_NULL)
        {
            m_textureAtlas[i]->retain();
        }
    }

    m_numRows = numRows;
    m_numCols = numCols;

    m_currentIndex = 0;
    m_previousIndex = 0xffffffff;
    m_previousTextureId = 0xffffffff;

    material()->setTransparent(transparency);
}

void PSprite::setAnimationDuration(puint32 numFrames, pfloat32 milliseconds, puint32 startIndex)
{
    m_numFrames = numFrames;
    m_duration = milliseconds;
    m_deltaMilliseconds = milliseconds / m_numFrames;
    m_startIndex = startIndex;
}

pbool PSprite::unpack(const PXmlElement* xmlElement)
{
    PASSERT(xmlElement->isValid());

    if (!PObject::unpack(xmlElement))
    {
        return false;
    }

    PResourceManager *resMgr = scene()->context()->module<PResourceManager>("resource-manager");

    setAnimationDuration(m_numFrames, m_duration);

    PXmlElement materialElement = xmlElement->child("material");
    PASSERT(materialElement.isValid());
    if (!materialElement.isValid())
    {
        PLOG_ERROR("Failed to find the material element to the PSprite in this xml node.");
        return false;
    }
        
    const pchar *colValue = materialElement.attribute("cols");
    const pchar *rowValue = materialElement.attribute("rows");
    if (colValue == P_NULL || rowValue == P_NULL)
    {
        PLOG_ERROR("Failed to find texture atlas resolution in material node of this PSprite.");
        return false;
    }
    pint32 numCols;
    pint32 numRows;
    pStringUnpackInt(colValue, &numCols);
    pStringUnpackInt(rowValue, &numRows);

    const pchar *textureNames[] = 
    {
        materialElement.attribute("texture1"),
        materialElement.attribute("texture2"),
        materialElement.attribute("texture3"),
        materialElement.attribute("texture4"),
    };
    puint32 numTextures = 0;
    PTexture *textures[4] = {P_NULL, P_NULL, P_NULL, P_NULL};
    for (puint32 i = 0; i < 4; ++i)
    {
        textures[i] = resMgr->getTexture(textureNames[i]);
        if (textures[i] != P_NULL)
        {
            numTextures++;
        }
    }

    pbool transparent = false;
    const pchar *transparentValue = materialElement.attribute("transparent");
    if (transparentValue != P_NULL && pstrcmp(transparentValue, "true") == 0)
    {
        transparent = true;
    }

    setTextureAtlas(textures, numTextures, numRows, numCols, transparent);

    if (numTextures == 0)
    {
        PLOG_ERROR("Failed to find the textures for PSprite in this xml node.");
        return false;
    }
    
    return true;    
}

void PSprite::update()
{
    puint32 currentIndex = ((puint32)(scene()->context()->clock().currentTime() / m_deltaMilliseconds) + m_startIndex) % 
        m_numFrames;  

    if (currentIndex == m_previousIndex)
    {
        return ;
    }
    m_previousIndex = currentIndex;

    puint32 numFramesPerTexture = (m_numRows * m_numCols);

    //
    // Update the frame index, the material
    //
    puint32 texId = currentIndex / numFramesPerTexture;
    puint32 rowId = (currentIndex - texId * numFramesPerTexture) / m_numCols;
    puint32 colId = (currentIndex - texId * numFramesPerTexture) % m_numCols;
        
    if (m_previousTextureId != texId)
    {
        PASSERT(m_textureAtlas[texId] != P_NULL);
        material()->parameter("texture") = m_textureAtlas[texId];
        m_previousTextureId = texId;
    }
    
    material()->parameter("texcoordmetric") = pVector4(1.0f / (pfloat32)m_numCols, 
        1.0f / (pfloat32)m_numRows, (pfloat32)colId / (pfloat32)m_numCols, 
        (pfloat32)rowId / (pfloat32)m_numRows);

    //PLOG_INFO("cell = (%d, %d, %d)", texId, rowId, colId);
}
    
void PSprite::calculateModelCameraMatrix(const PMatrix4x4 &cameraMatrix, pfloat32 *out_matrix)
{
    pMatrix4x4Multiply(cameraMatrix.m_m, m_worldTransform.m_m, out_matrix);

    // Remove the rotation component from the transformation matrix.
    pfloat32 sx;
    pfloat32 sy;
    pfloat32 sz;

    pMatrix4x4GetScaling(out_matrix, &sx, &sy, &sz);
    
    out_matrix[0] = sx;
    out_matrix[1] = 0;
    out_matrix[2] = 0;

    out_matrix[4] = 0;
    out_matrix[5] = sy;
    out_matrix[6] = 0;
    
    out_matrix[8]  = 0;
    out_matrix[9]  = 0;
    out_matrix[10] = sz;
}
