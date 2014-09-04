// pmaterial.cpp
// The used defined material class.
//
// Copyright 2012 - 2014 Future Interface . 
// This software is licensed under the terms of the MIT license.
//
// Hongwei Li lihw81@gmail.com
//
// 

#include "pmaterial.h"

#include <Paper3D/prendertransform.h>
#include <Paper3D/pdrawable.h>
#include <Paper3D/pabstractgeometry.h>
#include <Paper3D/pabstractrenderparameter.h>
#include <Paper3D/pmaterialparameter.h>
#include <Paper3D/pmaterialresource.h>
#include <Paper3D/prenderstate.h>
#include <Paper3D/presourcemanager.h>

#include <PFoundation/pxmlelement.h>
#include <PFoundation/pcrt.h>
#include <PFoundation/pglerror.h>


PMaterialParameter *PMaterial::s_phonyMaterialParameter = P_NULL; 

PMaterial::PMaterial(const pchar *id, PResourceManager *resourceManager)
    : PEntity()
{
    PASSERT(resourceManager != P_NULL);
    m_resource = resourceManager->getMaterial(id);
    m_resource->retain();

    // Clone the parameters
    puint32 numParameters = m_resource->numberOfMaterialParameters();
    for (puint32 i = 0; i < numParameters; ++i)
    {
        PMaterialParameter *parameter = PNEW(PMaterialParameter(*m_resource->materialParameter(i)));
        // The uniform location of parameter in material instance is the index of the parameter in 
        // the material resource.
        parameter->setUniformLocation((pint32)i);
        m_materialParameters.append(parameter);
    }

    m_isTransparent = false;
  
    if (s_phonyMaterialParameter == P_NULL)
    {
        s_phonyMaterialParameter = PNEW(PMaterialParameter("phony", "Phony", P_GLSHADERUNIFORM_UNKNOWN, 1));
    }
}

PMaterial::PMaterial(const pchar *id, const pchar *text, pbool autoRelease, PResourceManager *resourceManager)
    : PEntity()
{
    PASSERT(resourceManager != P_NULL);
    m_resource = resourceManager->createMaterial(text, id, autoRelease);
    m_resource->retain();

    // Clone the parameters
    puint32 numParameters = m_resource->numberOfMaterialParameters();
    for (puint32 i = 0; i < numParameters; ++i)
    {
        PMaterialParameter *parameter = PNEW(PMaterialParameter(*m_resource->materialParameter(i)));
        // The uniform location of parameter in material instance is the index of the parameter in 
        // the material resource.
        parameter->setUniformLocation((pint32)i);
        m_materialParameters.append(parameter);
    }

    m_isTransparent = false;
  
    if (s_phonyMaterialParameter == P_NULL)
    {
        s_phonyMaterialParameter = PNEW(PMaterialParameter("phony", "Phony", P_GLSHADERUNIFORM_UNKNOWN, 1));
    }
}


PMaterial::~PMaterial()
{
    puint32 num = m_materialParameters.count();
    for (puint32 i = 0; i < num; ++i)
    {
        PDELETE(m_materialParameters[i]);
    }

    if (m_resource != P_NULL)
    {
        m_resource->release();
    }
    if (s_phonyMaterialParameter != P_NULL)
    {
        PDELETE(s_phonyMaterialParameter);
    }
}

void PMaterial::apply(PRenderState *renderState)
{
    PASSERT(m_resource != P_NULL);
    if (m_resource== P_NULL)
    {
        PLOG_WARNING("Use an invalid material.");
        return ;
    }

    // Enable the shader.
    renderState->useMaterial(m_resource);

    puint32 num = m_materialParameters.count();
    puint32 numTextures = 0;
    for (puint32 i = 0; i < num; ++i)
    {
        m_materialParameters[i]->upload(m_resource, renderState, numTextures);
    }
}
    
void PMaterial::apply(PRenderState           *renderState,
                      PDrawable              *drawable,
                      PCamera                *camera,
                      const PRenderLightQueue &lights)
{
    pGlErrorCheckError();
    
    PASSERT(m_resource != P_NULL);
    if (m_resource== P_NULL)
    {
        PLOG_WARNING("Use an invalid material.");
        return ;
    }

    // Enable the shader.
    renderState->useMaterial(m_resource);

    // Upload drawable's values to shader 
    PRenderTransform transform(drawable, camera);

    puint32 num;
    puint32 numTextures = 0;

    pGlErrorCheckError();
    
    num = m_resource->numberOfRenderParameters();
    for (puint32 i = 0; i < num; ++i)
    {
        m_resource->renderParameter(i)->upload(m_resource->shader(), &transform, camera, lights);
    }
    pGlErrorCheckError();
    
    num = m_materialParameters.count();
    for (puint32 i = 0; i < num; ++i)
    {
        m_materialParameters[i]->upload(m_resource, renderState, numTextures);
    }
}
    
PMaterialParameter &PMaterial::parameter(const pchar *name) const
{
    // TODO: use PDictionary instead of PArray
    for (puint32 i = 0; i < m_materialParameters.count(); ++i)
    {
        if (m_materialParameters[i]->name() == name)
        {
            return *m_materialParameters[i];
        }
    }

    PLOG_WARNING("Failed to find parameter %s in this material.", name);

    return *s_phonyMaterialParameter;
}
    
PGlVertexFormat *PMaterial::vertexFormat() const
{
    if (m_resource != P_NULL)
    {
        return m_resource->vertexFormat();
    }

    return P_NULL;
}
    
void PMaterial::setTransparent(pbool transparent)
{
    m_isTransparent = transparent;
}
    
PMaterial *PMaterial::unpack(PXmlElement *element, PResourceManager *resourceManager)
{
    PASSERT(element->isValid());

    PMaterial *ret;
    
    const pchar *idValue = element->attribute("id");
    if (idValue == P_NULL)
    {
        PLOG_ERROR("Failed to find the id of the material in this xml node.");
        return P_NULL;
    }

    ret = PNEW(PMaterial(idValue, resourceManager));

    // Go over all material parameters and set their values.
    puint32 numParameters = ret->numberOfMaterialParameters();
    for (puint32 i = 0; i < numParameters; ++i)
    {
        PMaterialParameter *parameter = ret->materialParameter(i);
        const pchar *parameterValue = element->attribute(parameter->name().c_str());
        if (parameterValue != P_NULL)
        {
            if (!parameter->unpack(parameterValue, resourceManager))
            {
                PLOG_WARNING("Failed to unpack parameter %s in materail %s.", parameterValue, idValue);
            }
        }
    }

	// TODO: validate the matching of type of uniform variable to the type of material parameter.

    const pchar *transparentValue = element->attribute("transparent");
    if (transparentValue != P_NULL && pstrcmp(transparentValue, "true") == 0)
    {
        ret->setTransparent(true);
    }

    return ret;
}
