// presourceobject.cpp
// The base class of all resource objects.
// 
// Copyright 2012 - 2014 Future Interface. 
// This software is licensed under the terms of the MIT license.
//
// Hongwei Li lihw81@gmail.com
//

#include "presourceobject.h"

#include <Paper3D/presourcemanager.h>

#include <PFoundation/prandom.h>

#include <PFoundation/passert.h>
#include <PFoundation/plog.h>
#include <PFoundation/ptime.h>


PResourceObject::PResourceObject(const pchar *id, PResourceManager *parent, PResourceTypeEnum type)
{
    m_type       = type;
    m_parent     = parent;
    m_available  = false;

    if (id == P_NULL)
    {
        m_id = PNEWARRAY(pchar, 7);
        generateUniqueId(m_id);
    }
    else
    {
        m_id = pstrdup(id);
    }
}

PResourceObject::~PResourceObject()
{
    PDELETEARRAY(m_id);
}

void PResourceObject::release()
{
    // FIXME: make the following line thread-safe
    PASSERT(m_ref > 0);
    
    --m_ref; 
    
    if (m_ref == 0)
    {
        if (m_parent != P_NULL)
        {
            m_parent->removeResourceObject(m_id);
        }

        PDELETETHIS(this);
    }
}
    
void PResourceObject::discard()
{
    if (!m_available)
    {
        return ;
    }
    discardResource();
    m_available = false;
}

void PResourceObject::restore()
{
    if (m_available)
    {
        return ;
    }
    if (restoreResource())
    {
        m_available = true;
    }
}
    
const pchar *PResourceObject::generateUniqueId(pchar *out_id)
{
    // TODO: use UUID module
    puint32 t = pTimeGetCurrentTimestamp();
    puint16 r = (puint16)(PRandom::randomUint32() % 0xffff);

    union
    {
        struct
        {
            puint32 t;
            puint32 r;
        } i;
        pchar c[6];
    } id;

    id.i.t = t;
    id.i.r = r;

    out_id[0] = id.c[0];
    out_id[1] = id.c[1];
    out_id[2] = id.c[2];
    out_id[3] = id.c[3];
    out_id[4] = id.c[4];
    out_id[5] = id.c[5];
    out_id[6] = 0;

    return out_id;
}
