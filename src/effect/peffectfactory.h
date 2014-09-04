// peffectfactory.h
// Generate an effect object from a xml node.
//
// Copyright 2012 - 2014 Future Interface. 
// This software is licensed under the terms of the MIT license.
//
// Hongwei Li lihw81@gmail.com
//
// 

#ifndef PEFFECTFACTORY_H
#define PEFFECTFACTORY_H

#include <PFoundation/pstringmap.h>


class PScene;
class PAbstractEffect;
class PXmlElement;


class P_DLLEXPORT PEffectUnpacker
{
public:
    PEffectUnpacker() {}
    virtual ~PEffectUnpacker() {}
    
    virtual PAbstractEffect *unpack(PXmlElement *xmlElement, PScene *scene) = 0;
};

class P_DLLEXPORT PEffectFactory
{
    friend class PSceneManager;

    PEffectFactory(const PEffectFactory &other) {}
    void operator=(const PEffectFactory &other) {}

public:
    PEffectFactory();
    ~PEffectFactory();

    PAbstractEffect *unpack(PXmlElement *xmlElement, PScene *scene);

private:
    pbool initialize();
    void uninitialize();

protected:
    PStringMap<PEffectUnpacker*> *m_effects;
};

#endif // !PEFFECTFACTORY_H

