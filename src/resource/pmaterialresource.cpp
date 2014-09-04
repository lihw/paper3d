// pmaterialresource.cpp
// The resource of material. All material of the same kind
// share one resource object.
// 
// Copyright 2012 - 2014 Future Interface . 
// This software is licensed under the terms of the MIT license.
//
// Hongwei Li lihw81@gmail.com
//

#include "pmaterialresource.h"

#include <Paper3D/ptexture.h>
#include <Paper3D/prenderstate.h>
#include <Paper3D/presourcemanager.h>
#include <Paper3D/pabstractrenderparameter.h>
#include <Paper3D/pmaterialparameter.h>
#include <Paper3D/ptransformparameter.h>
#include <Paper3D/plightparameter.h>
#include <Paper3D/pcameraparameter.h>

#include <PFoundation/pcontext.h>
#include <PFoundation/pinputstream.h>
#include <PFoundation/pparser_module.h>

#include <PFoundation/pglshader.h>
#include <PFoundation/plog.h>
#include <PFoundation/passert.h>
#include <PFoundation/pnew.h>
#include <PFoundation/pcrt.h>

#include "cache/pmaterialcache_private.h"
#include "cache/presourcecache_private.h"

        
static const pchar P_MATERIAL_DEFAULT[] = 
    "<material>\n"
      "<shader>\n"
        "<vs>\n"
          "uniform mat4 MVP; \n"
          "attribute mediump vec4 position; \n"
          "attribute mediump vec4 texcoord0; \n"
          "varying mediump vec2 vTexCoord; \n"
          "void main()\n"
          "{\n"
          "    gl_Position = MVP * position; \n"
          "    vTexCoord = texcoord0.xy; \n"
          "}\n"
        "</vs>\n"
        "<fs>\n"
          "precision mediump float; \n"
          "varying mediump vec2 vTexCoord; \n"
          "void main() \n"
          "{\n"
          "    gl_FragColor = vec4(vTexCoord.xy, 0, 1); \n"
          "}\n"
        "</fs>\n"
      "</shader>\n"
      "<parameters>\n"
        "<parameter name=\"mvp\" uniform=\"MVP\" source=\"transform\" />\n"
      "</parameters>\n"
    "</material>";

PMaterialResource::PMaterialResource(const pchar *id, 
                                     const pchar *materialText,
                                     PResourceManager* resourceManager)
    : PResourceObject(id, resourceManager, P_RESOURCE_GPU)
{
    // -------------------------------------------------------------- 
    // Create the shader object from the source code.
    // -------------------------------------------------------------- 
    if (pstrncmp(id, "default.pmt", 11) == 0)
    {
        createMaterial(P_MATERIAL_DEFAULT);
        PASSERT(m_shader != P_NULL);
        m_isDefault = true;
    }
    else
    {
        createMaterial(materialText);

        if (m_shader == P_NULL)
        {
            createMaterial(P_MATERIAL_DEFAULT);
            PASSERT(m_shader != P_NULL);
            m_isDefault = true;
            
            PLOG_ERROR("Failed to load material %s and default shader is used.", id);
        }
        else
        {
            m_isDefault = false;
        }
    }
            
    m_available = true;
}

PMaterialResource::~PMaterialResource()
{
    PDELETE(m_shader);

    puint32 num;

    num = m_renderParameters.count();
    for (puint32 i = 0; i < num; ++i)
    {
        PDELETE(m_renderParameters[i]);
    }
    num = m_materialParameters.count();
    for (puint32 i = 0; i < num; ++i)
    {
        PDELETE(m_materialParameters[i]);
    }
}

void PMaterialResource::createMaterial(const pchar *materialText)
{
    puint32 num;

    // Parse the document.
    PXmlDocument xmlDocument;
    if (!xmlDocument.parse(materialText))
    {
        PLOG_ERROR("Failed to parse material %s.", m_id);
        return ;
    }

    // -------------------------------------------------------------- 
    // Compile the shader
    // -------------------------------------------------------------- 
    PXmlElement materialElement = xmlDocument.firstChildElement();
    if (pstrcmp(materialElement.name(), "material") != 0)
    {
        PLOG_ERROR("The root node of material %s is not <material>.", m_id);
        return ;
    }

    PXmlElement shaderElement = materialElement.child("shader");
    if (!shaderElement.isValid())
    {
        PLOG_ERROR("There is no shader section in this material %s.", m_id);
        return ;
    }

    PXmlElement vsElement = shaderElement.child("vs");
    PXmlElement fsElement = shaderElement.child("fs");
    if (!vsElement.isValid() || !fsElement.isValid())
    {
        PLOG_ERROR("Incomplete shader section in material %s. Either vertex or fragment shader is missing.", m_id);
        return ;
    }
    
    m_shader = PNEW(PGlShader);
    if (!m_shader->compile((pchar *)vsElement.text(), (pchar *)fsElement.text()))
    {
        PLOG_ERROR("Failed to compile shader of material %s.", m_id);
		PASSERT_NOTREACHABLE("Failed to compile shader.");
        PDELETE(m_shader);
        return ;
    }

    // -------------------------------------------------------------- 
    // Parameters
    // -------------------------------------------------------------- 
    PXmlElement parametersElement = materialElement.child("parameters");
    if (!parametersElement.isValid())
    {
        PLOG_ERROR("There is no parameters section in this material %s.", m_id);
        PDELETE(m_shader);
        return ;
    }

    puint32 numMaterialParameters = 0;
    const pchar *materialParameterNames[64][2];

    PXmlElement parameterElement = parametersElement.firstChild();
    while (parameterElement.isValid())
    {
        if (pstrcmp(parameterElement.name(), "parameter") != 0)
        {
            PLOG_ERROR("Section parameters can only contain parameter elements");
            goto bad;
        }

        const pchar *name        = parameterElement.attribute("name");
        const pchar *uniformName = parameterElement.attribute("uniform");
        const pchar *source      = parameterElement.attribute("source");

        if (name == P_NULL || uniformName == P_NULL || source == P_NULL)
        {
            PLOG_ERROR("An incomplete parameter.");
            goto bad;
        }
        
        pint32 location = m_shader->uniformLocation(uniformName);
        if (location < 0)
        {
            PLOG_ERROR("Parameter %s with uniform name = %s doesn't exist in the shader.", name, uniformName);
            goto bad;
        }

        if (pstrcmp(source, "transform") == 0)
        {
            PAbstractRenderParameter *parameter = PNEW(PTransformParameter(name, uniformName));
            parameter->setUniformLocation(location);
            m_renderParameters.append(parameter);
        }
        else if (pstrcmp(source, "camera") == 0)
        {
            PAbstractRenderParameter *parameter = PNEW(PCameraParameter(name, uniformName));
            parameter->setUniformLocation(location);
            m_renderParameters.append(parameter);
        }
        else if (pstrcmp(source, "light") == 0)
        {
            const pchar *type = parameterElement.attribute("type");
            puint32 index;
            if (type == P_NULL || !parameterElement.queryUintAttribute("index", index))
            {
                PLOG_ERROR("An incomplete light parameter in material %s.", m_id);
                goto bad;
            }

            PLightTypeEnum lightType = P_LIGHT_NONE;
            if (pstrcmp(type, "point") == 0)
            {
                lightType = P_LIGHT_POINT;
            }
            else if (pstrcmp(type, "directional") == 0)
            {
                lightType = P_LIGHT_DIRECTIONAL;
            }
            else if (pstrcmp(type, "spot") == 0)
            {
                lightType = P_LIGHT_SPOT;
            }
            else
            {
                PLOG_ERROR("Invalid light type in material %s.", m_id);
                goto bad;
            }

            PAbstractRenderParameter *parameter = PNEW(PLightParameter(lightType, index, name, uniformName));
            parameter->setUniformLocation(location);
            m_renderParameters.append(parameter);
        }
        else if (pstrcmp(source, "material") == 0)
        {
            materialParameterNames[numMaterialParameters][0] = name;
            materialParameterNames[numMaterialParameters][1] = uniformName;
            numMaterialParameters++;
        }
        else
        {
            PLOG_WARNING("The source of material parameter %s is not supported: %s.", name, source);
            goto bad;
        }

        parameterElement = parameterElement.nextSibling();
    }

    // -------------------------------------------------------------- 
    // Validate the parameters 
    // -------------------------------------------------------------- 
    {
        // Add a pair of braces to fix "jump to label 'bad' from here crosses initialization of 'puint32 numUniforms'.
        num = m_renderParameters.count();
        puint32 numUniforms = m_shader->numberOfUniforms();
        if (numUniforms != m_renderParameters.count() + numMaterialParameters)
        {
            PLOG_ERROR("There are more shader uniforms than parameters in material %s", m_id);
            PASSERT_NOTREACHABLE("Not enough material parameters.");
            goto bad;
        }

        // Find the type and location of each material parameter 
        for (puint32 j = 0; j < numMaterialParameters; ++j)
        {
            puint32 i;
            for (i = 0; i < numUniforms; ++i)
            {
                PGlShaderUniform uniform = m_shader->uniformInformation(i);

                if (pstrcmp(materialParameterNames[j][1], uniform.m_name) == 0)
                {
                    PMaterialParameter *materialParameter = PNEW(PMaterialParameter(
                        materialParameterNames[j][0],
                        materialParameterNames[j][1],
                        uniform.m_type,
                        uniform.m_size));
                    materialParameter->setUniformLocation(uniform.m_location);
                    m_materialParameters.append(materialParameter);
                    break;
                }
            }
            
            // The parameter must have the corresponding shader uniform.
            PASSERT(i != numUniforms);
        }
        m_vertexFormat = m_shader->vertexFormat();
    }
    
    return ;

bad:
    PDELETE(m_shader);

    num = m_renderParameters.count();
    for (puint32 i = 0; i < num; ++i)
    {
        PDELETE(m_renderParameters[i]);
    }
    m_renderParameters.clear();
    num = m_materialParameters.count();
    for (puint32 i = 0; i < num; ++i)
    {
        PDELETE(m_materialParameters[i]);
    }
    m_materialParameters.clear();
}

const pchar * PMaterialResource::vertexShaderSource() const
{
    PResourceCache *cache = parent()->cache();
    PMaterialCache *materialCache = cache->getMaterialCache(m_id);
    PASSERT(materialCache != P_NULL);

    return materialCache->vsSource();
}

const pchar * PMaterialResource::fragmentShaderSource() const
{
    PResourceCache *cache = parent()->cache();
    PMaterialCache *materialCache = cache->getMaterialCache(m_id);
    PASSERT(materialCache != P_NULL);

    return materialCache->fsSource();
}
    
void PMaterialResource::discardResource()
{
    PDELETE(m_shader);
}

pbool PMaterialResource::restoreResource()
{
    PASSERT(m_shader == P_NULL);
    if (m_shader == P_NULL)
    {
        PResourceCache *cache = parent()->cache();
        PMaterialCache *materialCache = cache->getMaterialCache(m_id);

        m_shader = PNEW(PGlShader);
        if (!m_shader->compile((pchar *)materialCache->vsSource(),
                               (pchar *)materialCache->fsSource()))
        {
            PDELETE(m_shader);
            return false;
        }

        puint32 num;

        num = m_renderParameters.count();
        for (puint32 i = 0; i < num; ++i)
        {
            const pchar *uniform = m_renderParameters[i]->uniformName().c_str();
            pint32 location = m_shader->uniformLocation(uniform);
            PASSERT(location >= 0);
            if (location < 0)
            {
                PLOG_ERROR("Failed to find render parameter %s with uniform name = %s in "
                    "shader %s", m_renderParameters[i]->name().c_str(), uniform, m_id);
                PDELETE(m_shader);
                return false;
            }
            m_renderParameters[i]->setUniformLocation(location);
        }
        num = m_materialParameters.count();
        for (puint32 i = 0; i < num; ++i)
        {
            const pchar *uniform = m_materialParameters[i]->uniformName().c_str();
            pint32 location = m_shader->uniformLocation(uniform);
            PASSERT(location >= 0);
            if (location < 0)
            {
                PLOG_ERROR("Failed to find material parameter %s with uniform name = %s in "
                    "shader %s", m_materialParameters[i]->name().c_str(), uniform, m_id);
                PDELETE(m_shader);
                return false;
            }
            m_materialParameters[i]->setUniformLocation(location);
        }
    }
    else
    {
        PLOG_ERROR("Failed to restore material %s because old shader program had not been discarded", m_id);
    }

    return true;
}

