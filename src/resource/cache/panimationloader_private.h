// panimationloader.h
// The animation loader.
//
// Copyright 2012 - 2014 Future Interface. 
// This software is licensed under the terms of the MIT license.
//
// Hongwei Li lihw81@gmail.com 
//

#ifndef PANIMATIONLOADER_H
#define PANIMATIONLOADER_H

#include <PFoundation/pglobal.h>

class PInputStream;
class PAnimationResource;

// TODO: unify the loading framework of all resources
class P_DLLEXPORT PAnimationLoader
{
public:
    PAnimationLoader();
    ~PAnimationLoader();

    bool read(PInputStream *inputSteam, PAnimationResource *animation);

private:
    PAnimationResource *m_animationResource;
};

#endif //!PANIMATIONLOADER_H
