// pbackground.cpp
// Background is the plane at the very back of the scene. It renders
// a full screen quad with a texture.
//
// Copyright 2012 - 2014 Future Interface . 
// This software is licensed under the terms of the MIT license.
//
// Hongwei Li lihw81@gmail.com
//

#include "pbackground.h"

#include <Paper3D/ptexture.h>
#include <Paper3D/pscene.h>
#include <Paper3D/pgeometryplane.h>
#include <Paper3D/pmaterial.h>
#include <Paper3D/pmaterialparameter.h>
#include <Paper3D/presourcemanager.h>
#include <Paper3D/prenderstate.h>

#include <PFoundation/pcontext.h>
#include <PFoundation/pxmlelement.h>
#include <PFoundation/pconststring.h>


P_OBJECT_DEFINE(PBackground)

static const pchar *PBACKGROUND_PMT = 
	"<material>\n"
	"  <shader>\n"
	"    <vs>\n"
	"      attribute mediump vec3 position;\n"
	"      attribute mediump vec2 texcoord0;\n"
	"      varying mediump vec2 TexCoord;\n"
	"      uniform vec4 TexInfo;\n"
	"      uniform vec4 SizeInfo;\n"
	"\n"
	"      void main()\n"
	"      {\n"
	"          gl_Position = vec4(position.xy * SizeInfo.xy + SizeInfo.zw, 1.0, 1.0);\n"
    "          TexCoord = texcoord0 * TexInfo.xy + TexInfo.zw;\n"
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
	"    <parameter name=\"texture\" uniform=\"Tex\" source=\"material\"/>\n"
	"    <parameter name=\"texinfo\" uniform=\"TexInfo\" source=\"material\"/>\n"
    "    <parameter name=\"sizeinfo\" uniform=\"SizeInfo\" source=\"material\"/>\n"
	"  </parameters>\n"
	"</material>\n";

PBackground::PBackground(const pchar *name, PScene *scene)
    : PDrawable(name, scene, P_DRAWABLE_PRIORITY_DRAWLAST)
{
    P_OBJECT_INITIALIZE_PROPERTY(PDrawable)
    
    PResourceManager *resourceManager = scene->context()->module<PResourceManager>("resource-manager");

    m_texture  = P_NULL;

    setGeometry(PNEW(PGeometryPlane(resourceManager)));
    setMaterial(PNEW(PMaterial("internal/background.pmt", PBACKGROUND_PMT, false, resourceManager)));

    m_textureInfo  = pVector4(1, 1, 0, 0);
    m_sizeInfo     = pVector4(1, 1, 0, 0);
    m_layout       = (LAYOUT_CENTER | LAYOUT_MIDDLE);  
    m_dirty        = true;
    m_fillMode     = FILL_DEFAULT;
}

PBackground::~PBackground()
{
    P_OBJECT_UNINITIALIZE_PROPERTY();

    if (m_texture != P_NULL)
    {
        m_texture->release();
    }
}

void PBackground::setTexture(PTexture *texture)
{
    PASSERT(texture != P_NULL);

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
            m_texture->setRepeatWrappingEnabled(true);
        }

        material()->parameter("texture") = texture;
    }
}
    
void PBackground::prepareRender(PRenderState *renderState)
{
    if (m_texture == P_NULL)
    {
        PLOG_ERROR("Fail to render a background without a valid texture.");
    }

    if (m_dirty)
    {
        switch ((m_layout & 0xf0))
        {
            case LAYOUT_LEFT:
                m_sizeInfo[2] = m_sizeInfo[0] - 1.0f;;
                break;
            case LAYOUT_CENTER:
                m_sizeInfo[2] = 0;;
                break;
            case LAYOUT_RIGHT:
                m_sizeInfo[2] = 1.0f - m_sizeInfo[0];
                break;
            default:
                PASSERT_NOTREACHABLE("Unknown background layout value.");
                break;
        }

        switch ((m_layout & 0x0f))
        {
            case LAYOUT_TOP:
                m_sizeInfo[3] = 1.0f - m_sizeInfo[1];
                break;
            case LAYOUT_MIDDLE:
                m_sizeInfo[3] = 0;
                break;
            case LAYOUT_BOTTOM:
                m_sizeInfo[3] = m_sizeInfo[1] - 1.0f;
                break;
            default:
                PASSERT_NOTREACHABLE("Unknown background layout value.");
                break;
        }

        puint32 viewport[4];
        renderState->renderStateObject()->getViewport(viewport);

        pfloat32 width  = (pfloat32)viewport[2] * m_sizeInfo[0];
        pfloat32 height = (pfloat32)viewport[3] * m_sizeInfo[1];

        if (m_fillMode == FILL_STRETCHED_UNIFORM || m_fillMode == FILL_TILED)
        {
            m_textureInfo[0] = (pfloat32)viewport[2] / width;
            m_textureInfo[1] = (pfloat32)viewport[3] / height;

            if (m_fillMode == FILL_STRETCHED_UNIFORM)
            {
                if (m_textureInfo[0] < m_textureInfo[1])
                {
                    m_textureInfo[0] = m_textureInfo[0] / m_textureInfo[1];
                    m_textureInfo[1] = 1.0f;
                }
                else
                {
                    m_textureInfo[1] = m_textureInfo[1] / m_textureInfo[0];
                    m_textureInfo[0] = 1.0f;
                }
            }
        }
        else
        {
            m_textureInfo[0] = 1.0f;
            m_textureInfo[1] = 1.0f;
        }
        
	    material()->parameter("texinfo") = m_textureInfo;
        material()->parameter("sizeinfo") = m_sizeInfo;

        m_dirty = false;
    }
}

void PBackground::setTextureOffset(pfloat32 x, pfloat32 y)
{
	m_textureInfo[2] = x;
	m_textureInfo[3] = y;

    m_dirty = true;
}

void PBackground::setTextureFillMode(PBackground::FillModeEnum fillMode)
{
    m_fillMode = fillMode;

    m_dirty = true;
}

void PBackground::setSize(pfloat32 width, pfloat32 height)
{
    PASSERT(width > 0 && width <= 1.0f);
    PASSERT(height > 0 && height <= 1.0f);

    m_sizeInfo[0] = width;
    m_sizeInfo[1] = height;

    m_dirty = true;
}

void PBackground::setLayout(puint32 layout)
{
    if (m_layout != layout)
    {
        m_layout = layout;

        m_dirty = true;
    }
}


pbool PBackground::unpack(const PXmlElement* xmlElement)
{
    PASSERT(xmlElement->isValid());

    const pchar *textureName = xmlElement->attribute("texture");
    if (textureName != P_NULL)
    {
        PResourceManager *resourceManager = scene()->context()->module<PResourceManager>("resource-manager");

        m_texture = resourceManager->getTexture(textureName);
        if (m_texture != P_NULL)
        {
            m_texture->retain();
        }
        material()->parameter("texture") = m_texture;
    }
    else
    {
        PLOG_ERROR("Failed to find the texture for PBackground in this xml node.");
        return false;
    }

    const pchar *p = P_NULL;

    const pchar *textureInfoValue = xmlElement->attribute("texinfo");
    if (textureInfoValue != P_NULL)
    {
        pfloat32 x, y;

        if ((p = pStringUnpackFloat(p, &x)) != P_NULL &&
            (p = pStringUnpackFloat(p, &y)) != P_NULL)
        {
	        m_textureInfo[2] = x;
            m_textureInfo[3] = y;
        }
    }

    const pchar *fillValue = xmlElement->attribute("fill");
    if (fillValue != P_NULL)
    {
        if (pstrcmp(fillValue, "stretched") == 0)
        {
            m_fillMode = FILL_STRETCHED;
        }
        else if (pstrcmp(fillValue, "stretched_uniform") == 0)
        {
            m_fillMode = FILL_STRETCHED_UNIFORM;
        }
        else if (pstrcmp(fillValue, "tiled") == 0)
        {
            m_fillMode = FILL_STRETCHED_UNIFORM;
        }
        else
        {
            PASSERT_NOTREACHABLE("Unknown background texture fill mode");
            return false;
        }
    }

    const pchar *sizeValue = xmlElement->attribute("size");
    if (sizeValue != P_NULL)
    {
        pfloat32 sx, sy;

        if ((p = pStringUnpackFloat(p, &sx)) != P_NULL &&
            (p = pStringUnpackFloat(p, &sy)) != P_NULL)
        {
	        m_sizeInfo[0] = sx;
            m_sizeInfo[1] = sy;
        }
    }

    const pchar *layoutValue = xmlElement->attribute("layout");
    pchar vLayout[16];
    pchar hLayout[16];
    if (layoutValue != P_NULL)
    {
        if ((p = pStringUnpackString(layoutValue, hLayout)) != P_NULL && 
            (p = pStringUnpackString(layoutValue, vLayout)) != P_NULL)
        {
            puint32 layout = 0;

            if (pstrcmp(vLayout, "top") == 0)
            {
                layout |= LAYOUT_TOP;
            }
            else if (pstrcmp(vLayout, "middle") == 0)
            {
                layout |= LAYOUT_MIDDLE;
            }
            else if (pstrcmp(vLayout, "bottom") == 0)
            {
                layout |= LAYOUT_BOTTOM;
            }
            else if (pstrcmp(vLayout, "left") == 0)
            {
                layout |= LAYOUT_LEFT;
            }
            else if (pstrcmp(vLayout, "center") == 0)
            {
                layout |= LAYOUT_CENTER;
            }
            else if (pstrcmp(vLayout, "right") == 0)
            {
                layout |= LAYOUT_RIGHT;
            }
    
            setLayout(layout);
        }
    }

    m_dirty = true;

    return true;
}

