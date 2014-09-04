// peffectfactory.cpp
// Generate an effect object from a xml node.
//
// Copyright 2012 - 2014 Future Interface. 
// This software is licensed under the terms of the MIT license.
//
// Hongwei Li lihw81@gmail.com
//
// 

#include "peffectfactory.h"

#include <Paper3D/peffectglow.h>

#include <PFoundation/pxmlelement.h>

template<typename T>
class P_DLLEXPORT PObjUnpacker: public PEffectUnpacker 
{ 
public: 
    virtual PAbstractEffect *unpack(PXmlElement *xmlElement, PScene *scene) 
    { 
        T *effectInstance = PNEW(T(scene)); 
        effectInstance->unpack(xmlElement);
        return effectInstance; 
    } 
};

typedef PObjUnpacker<PEffectGlow> PEffectGlowUnpacker;
// TODO: more

PEffectFactory::PEffectFactory()
{
    m_effects = P_NULL;
}

PEffectFactory::~PEffectFactory()
{
    PASSERT(m_effects == P_NULL);
}

PAbstractEffect *PEffectFactory::unpack(PXmlElement *xmlElement, PScene *scene)
{
    PASSERT(m_effects != P_NULL);
    const pchar *nameValue = xmlElement->attribute("name");
    if (nameValue == P_NULL)
    {
        PLOG_ERROR("Failed to find the name attribute of an effect xml node.");
        return P_NULL;
    }
    PStringMap<PEffectUnpacker *>::iterator it = m_effects->find(const_cast<pchar *>(nameValue));
    if (it == m_effects->end())
    {
        PLOG_ERROR("The effect %s is not supported by this version.", nameValue);
        return P_NULL;
    }

    return it.value()->unpack(xmlElement, scene);
}
    
pbool PEffectFactory::initialize()
{
    m_effects = PNEW(PStringMap<PEffectUnpacker*>());

    m_effects->insert("glow", PNEW(PEffectGlowUnpacker));

    return true;
}

void PEffectFactory::uninitialize()
{
    PStringMap<PEffectUnpacker*>::iterator it = m_effects->begin(); 
    PStringMap<PEffectUnpacker*>::iterator ie = m_effects->end(); 

    while (it != ie)
    {
        PDELETE(it.value());
        ++it;
    }
    
    PDELETE(m_effects);
}
