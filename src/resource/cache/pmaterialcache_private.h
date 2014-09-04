// pshadersource.h
// Source code of a shader
//
// Copyright 2012 - 2014 Future Interface. 
// This software is licensed under the terms of the MIT license.
//
// Hongwei Li lihw81@gmail.com
//

#ifndef PMATERIALCACHE_H
#define PMATERIALCACHE_H

#include "pcacheobject_private.h"


class P_DLLEXPORT PMaterialCache : public PCacheObject
{
    friend class PResourceCache;
    friend class PResourceManager;

    PMaterialCache(const PMaterialCache &other) : PCacheObject(P_NULL, P_NULL, true) {}
    void operator=(const PMaterialCache &other) {}

public:
    const pchar *vsSource() const; 
    const pchar *fsSource() const;

    P_INLINE const pchar *text() { return m_text; }
    
protected:
    virtual void swapIn();
    virtual void swapOut();

private:
    PMaterialCache(PResourceCache *parent, const pchar *id); 
    PMaterialCache(PResourceCache *parent, const pchar *id, const pchar *text, pbool autoRelease); 
    virtual ~PMaterialCache();
    
    pbool load(const pchar *id);
        
private:
    mutable pchar *m_vsSource;
    mutable pchar *m_fsSource;
    mutable pchar *m_text;
};


#endif // !PMATERIALCACHE_H
