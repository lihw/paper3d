// PCacheObject.cpp
// The base class of all asset objects
//
// Copyright 2012 - 2014 Future Interface. 
// This software is licensed under the terms of the MIT license.
//
// Hongwei Li lihw81@gmail.com
//


#include "pcacheobject_private.h"

#include <Paper3D/presourcemanager.h>
#include "presourcecache_private.h"


PCacheObject::PCacheObject(PResourceCache *parent, const pchar *id, pbool autoRelease)
{
    m_id          = pstrdup(id);
    m_parent      = parent;
    m_available   = true;
    m_autoRelease = autoRelease;
    m_storageType = P_RESOURCE_STORAGE_UNKNOWN;
}

PCacheObject::~PCacheObject()
{
    PDELETEARRAY(m_id);
}

PArchiveFile *PCacheObject::archive() const
{
    PResourceManager *resourceManager = m_parent->resourceManager();
    return resourceManager->archive();
}
