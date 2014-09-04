// pshadersource.cpp
// Source code of a shader
//
// Copyright 2012 - 2014 Future Interface. 
// This software is licensed under the terms of the MIT license.
//
// Hongwei Li lihw81@gmail.com
//

#include "pmaterialcache_private.h"

#include <PFoundation/pinputstream.h>
#include <PFoundation/parchivefile.h>
#include <PFoundation/pparser_module.h>

    
PMaterialCache::PMaterialCache(PResourceCache *parent, const pchar *id)
    : PCacheObject(parent, id, false)
{
    m_text     = P_NULL;
    m_vsSource = P_NULL;
    m_fsSource = P_NULL;

    if (load(id))
    {
        m_storageType = archive()->isPathInArchive(id)?
            P_RESOURCE_FROM_ARCHIVE : P_RESOURCE_FROM_FILE;
    }
}
    
PMaterialCache::PMaterialCache(PResourceCache *parent, const pchar *id, 
    const pchar *text, pbool autoRelease) 
    : PCacheObject(parent, id, autoRelease)
{
    if (m_autoRelease)
    {
        m_text     = const_cast<pchar *>(text);
        m_vsSource = P_NULL;
        m_fsSource = P_NULL;
    }
    else
    {
        m_text     = pstrdup(text);
        m_vsSource = P_NULL;
        m_fsSource = P_NULL;
    }
        
    m_storageType = P_RESOURCE_FROM_MEMORY;
}

PMaterialCache::~PMaterialCache()
{
    if (!m_autoRelease)
    {
        PDELETEARRAY(m_vsSource);
        PDELETEARRAY(m_fsSource);
        PDELETEARRAY(m_text);
    }
}

void PMaterialCache::swapIn()
{
    PASSERT(!m_available);
    if (!m_available)
    {
        if (m_storageType == P_RESOURCE_FROM_ARCHIVE ||
            m_storageType == P_RESOURCE_FROM_FILE)
        {
            if (load(m_id))
            {
                m_available = true;
            }
        }
        else if (m_storageType == P_RESOURCE_FROM_MEMORY)
        {
            if (!m_autoRelease)
            {
                // TODO: read in the temporary file written at swapping out.
            }
            m_available = true;
        }
        else
        {
            PLOG_ERROR("Unknown storage type of shader source %s", m_id);
        }
    }
}

void PMaterialCache::swapOut()
{
    PASSERT(m_available);
    if (m_available && !m_autoRelease)
    {
        if (m_storageType == P_RESOURCE_FROM_MEMORY)
        {
            // TODO: output the image to the temporary file.
        }

        PDELETEARRAY(m_vsSource);
        PDELETEARRAY(m_fsSource);
        PDELETEARRAY(m_text);

        m_available = false;
    }
}
    
const pchar *PMaterialCache::vsSource() const
{
    if (m_vsSource == P_NULL)
    {
        // Parse the vertex shader source from the text. As it is the second
        // time we parse the material text, we can skip the validation.
        PXmlDocument xmlDocument;
        xmlDocument.parse(m_text);
        
        PXmlElement materialElement = xmlDocument.firstChildElement();

        PXmlElement shaderElement = materialElement.child("shader");

        PXmlElement vsElement = shaderElement.child("vs");
        PXmlElement fsElement = shaderElement.child("fs");

        m_vsSource = pstrdup(vsElement.text());
        m_fsSource = pstrdup(fsElement.text());

        // From now on, we don't need the text any more. We can delete it now.
        if (!m_autoRelease)
        {
            PDELETEARRAY(m_text);
        }
    }

    return m_vsSource;
}

const pchar *PMaterialCache::fsSource() const
{
    if (m_fsSource == P_NULL)
    {
        // Parse the fragment shader source from the text. As it is the second
        // time we parse the material text, we can skip the validation.
        PXmlDocument xmlDocument;
        xmlDocument.parse(m_text);

        PXmlElement materialElement = xmlDocument.firstChildElement();

        PXmlElement shaderElement = materialElement.child("shader");

        PXmlElement vsElement = shaderElement.child("vs");
        PXmlElement fsElement = shaderElement.child("fs");

        m_vsSource = pstrdup(vsElement.text());
        m_fsSource = pstrdup(fsElement.text());

        // From now on, we don't need the text any more. We can delete it now.
        if (!m_autoRelease)
        {
            PDELETEARRAY(m_text);
        }
    }

    return m_fsSource;
}
    
pbool PMaterialCache::load(const pchar *id)
{
    PInputStream inputStream;
    puint8 *text;
    if (archive()->createInputStream(id, &inputStream) &&
        inputStream.readAllBytesAsString(text) > 0)
    {
        m_text = (pchar *)text;
    }
    else
    {
		PASSERT(!"Failed to read material %s.");
        PLOG_ERROR("Failed to read material %s.", id);
        return false;
    }

    return true;
}