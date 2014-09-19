// presourceobject.h
// The base class of all resource objects.
// 
// Copyright 2012 - 2014 Future Interface. 
// This software is licensed under the terms of the MIT license.
//
// Hongwei Li lihw81@gmail.com
//

#ifndef PRESOURCEOBJECT_H
#define PRESOURCEOBJECT_H

#include <PFoundation/pentity.h>

class PResourceManager;

enum PResourceTypeEnum
{
    P_RESOURCE_TYPE_UNKNOWN,
    P_RESOURCE_GPU,
    P_RESOURCE_CPU, 

    P_RESOURCE_TYPE_FIRST = P_RESOURCE_TYPE_UNKNOWN,
    P_RESOURCE_TYPE_LAST = P_RESOURCE_CPU,
    P_RESOURCE_TYPE_NUMBER = P_RESOURCE_TYPE_LAST - P_RESOURCE_TYPE_FIRST,
};

enum PResourceStorageTypeEnum
{
    P_RESOURCE_STORAGE_UNKNOWN,
    P_RESOURCE_FROM_ARCHIVE,
    P_RESOURCE_FROM_FILE,
    P_RESOURCE_FROM_MEMORY,
    // TODO: add a procedural type which is created procedurally, e.g., default
    // texture, plane and so on

    P_RESOURCE_STORAGE_TYPE_FIRST = P_RESOURCE_STORAGE_UNKNOWN,
    P_RESOURCE_STORAGE_TYPE_LAST = P_RESOURCE_FROM_MEMORY,
    P_RESOURCE_STORAGE_TYPE_NUMBER = 
        P_RESOURCE_STORAGE_TYPE_LAST - P_RESOURCE_STORAGE_TYPE_FIRST + 1,
};

class P_DLLEXPORT PResourceObject : public PEntity
{
    friend class PResourceManager;

    PResourceObject(const PResourceObject& other) {}
    void operator=(const PResourceObject& other) {}

public:
    PResourceObject(const pchar *id, PResourceManager* parent, PResourceTypeEnum type); 
    virtual ~PResourceObject();

    P_INLINE const pchar *id() const          { return m_id;  }
    P_INLINE pbool isAvailable() const         { return m_available; }
    P_INLINE PResourceManager* parent() const { return m_parent; }
    P_INLINE PResourceTypeEnum type() const   { return m_type; }

    void discard();
    void restore();

    virtual void release();

protected:
    virtual void discardResource() = 0;
    virtual pbool restoreResource() = 0;

    static const pchar *generateUniqueId(pchar *out_id);

protected:
    PResourceTypeEnum  m_type;
    pchar             *m_id;
    PResourceManager*  m_parent;
    // Whether this resource is available. It may be swapped out temporarily.
    pbool              m_available; 
};


#endif // !PRESOURCEOBJECT_H
