// pcacheobject.h
// The base class of all resource cache objects (CPU cache)
//
// Copyright 2012 - 2014 Future Interface. 
// This software is licensed under the terms of the MIT license.
//
// Hongwei Li lihw81@gmail.com
//

#ifndef PCACHEOBJECT_PRIVATE_H
#define PCACHEOBJECT_PRIVATE_H

#include <Paper3D/presourceobject.h>

#include <PFoundation/pstring.h>

class PResourceCache;
class PArchiveFile;

class P_DLLEXPORT PCacheObject
{
    friend class PResourceCache;
    
    PCacheObject(const PCacheObject &other) {}
    void operator=(const PCacheObject &other) {}

public:
    P_INLINE PResourceStorageTypeEnum storageType() const { return m_storageType; }
    P_INLINE const pchar *id() const { return m_id; }
    P_INLINE pbool isAvailable() const { return m_available; }

protected: 
    PCacheObject(PResourceCache *parent, const pchar *id, pbool autoRelease);
    virtual ~PCacheObject();

    // swap the object from CPU memory to storage
    virtual void swapOut() = 0;
    // read the content from the storage
    virtual void swapIn() = 0;

    PArchiveFile *archive() const;

protected:
    pchar                      *m_id;
    PResourceCache             *m_parent;
    pbool                       m_available; // If it is in system(CPU) memory
    PResourceStorageTypeEnum    m_storageType;
    pbool                       m_autoRelease; // If the data memory is from user
};


#endif // !PCACHEOBJECT_PRIVATE_H
