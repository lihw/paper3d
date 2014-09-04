// pmeshloader.h
// load objc file, convert to mesh.
//
// Copyright 2012 - 2014 Future Interface. 
// This software is licensed under the terms of the MIT license.
//
// Hongwei Li lihw81@gmail.com and Hongwei Li liwh81@gmail.com
//
// 
#ifndef PMESHLOADER_H
#define PMESHLOADER_H

#include <PFoundation/pnew.h>

#include "pmeshcache_private.h"

class PInputStream;

class P_DLLEXPORT PMeshLoader
{

public:
    PMeshLoader();
    ~PMeshLoader();

    pbool load(PInputStream *inputStream, PMeshCache *cache) const;

private:

};


#endif // PMESHLOADER_H
