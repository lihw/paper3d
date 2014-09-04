// pnodefactory.cpp
// Generate a scene node object from a xml node.
//
// Copyright 2012 - 2014 Future Interface. 
// This software is licensed under the terms of the MIT license.
//
// Hongwei Li lihw81@gmail.com
//
// 

#include "pnodefactory.h"

#include <Paper3D/pscene_graph_module.h>

#include <PFoundation/pxmlelement.h>


template<typename T>
class P_DLLEXPORT PObjUnpacker: public PNodeUnpacker 
{ 
public: 
    virtual PNode *unpack(PXmlElement *xmlElement, PNode *parent) 
    { 
        const pchar *nameValue = xmlElement->attribute("name"); 
        if (nameValue != P_NULL) 
        { 
            T *objInstance = PNEW(T(nameValue, parent)); 
            if (objInstance->unpack(xmlElement)) 
            { 
                return objInstance; 
            }
            PDELETE(objInstance);
        } 
        else 
        { 
            PLOG_ERROR("Failed to find the name of the scene node in this xml node."); 
        } 
        return P_NULL; 
    } 
};

typedef PObjUnpacker<PDrawable>         PDrawableUnpacker;
typedef PObjUnpacker<PCamera>           PCameraUnpacker;
typedef PObjUnpacker<PPointLight>       PPointLightUnpacker;
typedef PObjUnpacker<PDirectionalLight> PDirectionalLightUnpacker;
typedef PObjUnpacker<PSpotLight>        PSpotLightUnpacker;


class P_DLLEXPORT PWaterWaveUnpacker: public PNodeUnpacker 
{ 
public: 
    virtual PNode *unpack(PXmlElement *xmlElement, PNode *parent) 
    { 
        const pchar *nameValue = xmlElement->attribute("name"); 
        const pchar *reflectionValue = xmlElement->attribute("reflection"); 
        const pchar *refractionValue = xmlElement->attribute("refraction"); 

        if (nameValue != P_NULL) 
        { 
            pbool reflection = false;
            pbool refraction = false;
            
            if (reflectionValue != P_NULL && pstrcmp(reflectionValue, "true") == 0)
            {
                reflection = true;
            }
            if (refractionValue != P_NULL && pstrcmp(refractionValue, "true") == 0)
            {
                refraction = true;
            }

            PWaterWave *objInstance = PNEW(PWaterWave(nameValue, parent, reflection, refraction)); 
            if (objInstance->unpack(xmlElement)) 
            { 
                return objInstance; 
            } 
        } 
        else 
        { 
            PLOG_ERROR("Failed to find the name of the PWaterWave in this xml node."); 
        } 
        return P_NULL; 
    } 
};

class P_DLLEXPORT PSkyBoxUnpacker: public PNodeUnpacker 
{ 
public: 
    virtual PNode *unpack(PXmlElement *xmlElement, PNode *parent) 
    { 
        const pchar *nameValue = xmlElement->attribute("name"); 

        if (nameValue != P_NULL) 
        { 
            PSkyBox *objInstance = PNEW(PSkyBox(nameValue, parent->scene())); 
            if (objInstance->unpack(xmlElement)) 
            { 
                return objInstance; 
            } 
        } 
        else 
        { 
            PLOG_ERROR("Failed to find the name of the PSkyBox in this xml node."); 
        } 
        return P_NULL; 
    } 
};

class P_DLLEXPORT PBackgroundUnpacker: public PNodeUnpacker 
{ 
public: 
    virtual PNode *unpack(PXmlElement *xmlElement, PNode *parent) 
    { 
        const pchar *nameValue = xmlElement->attribute("name"); 

        if (nameValue != P_NULL) 
        { 
            PBackground *objInstance = PNEW(PBackground(nameValue, parent->scene())); 
            if (objInstance->unpack(xmlElement)) 
            { 
                return objInstance; 
            } 
        } 
        else 
        { 
            PLOG_ERROR("Failed to find the name of the PBackground in this xml node."); 
        } 
        return P_NULL; 
    } 
};

class P_DLLEXPORT PSceneNodeUnpacker: public PNodeUnpacker 
{ 
public: 
    virtual PNode *unpack(PXmlElement *xmlElement, PNode *parent) 
    { 
        const pchar *nameValue = xmlElement->attribute("name"); 

        if (nameValue != P_NULL) 
        { 
            PNode *objInstance = PNEW(PNode(nameValue, parent->scene(), parent, false)); 
            if (objInstance->unpack(xmlElement)) 
            { 
                return objInstance; 
            } 
        } 
        else 
        { 
            PLOG_ERROR("Failed to find the name of the PNode in this xml node."); 
        } 
        return P_NULL; 
    } 
};

PNodeFactory::PNodeFactory()
{
    m_nodes = P_NULL;
}

PNodeFactory::~PNodeFactory()
{
    PASSERT(m_nodes == P_NULL);
}

PNode *PNodeFactory::unpack(PXmlElement *xmlElement, PNode *parent)
{
    PASSERT(m_nodes != P_NULL);
    const pchar *elementName = xmlElement->name();
    if (elementName == P_NULL)
    {
        PLOG_ERROR("Failed to find the name attribute of an effect xml node.");
        return P_NULL;
    }
    PStringMap<PNodeUnpacker*>::iterator it = m_nodes->find(const_cast<pchar *>(elementName));
    if (it == m_nodes->end())
    {
        PLOG_ERROR("The node %s is not supported by this version.", elementName);
        return P_NULL;
    }

    return it.value()->unpack(xmlElement, parent);
}
    
pbool PNodeFactory::initialize()
{
    m_nodes = PNEW(PStringMap<PNodeUnpacker*>());

    m_nodes->insert("drawable",         PNEW(PDrawableUnpacker));
    m_nodes->insert("camera",           PNEW(PCameraUnpacker));
    m_nodes->insert("pointlight",       PNEW(PPointLightUnpacker));
    m_nodes->insert("directionallight", PNEW(PDirectionalLightUnpacker));
    m_nodes->insert("spotlight",        PNEW(PSpotLightUnpacker));
    m_nodes->insert("waterwave",        PNEW(PWaterWaveUnpacker));
    m_nodes->insert("skybox",           PNEW(PSkyBoxUnpacker));
    m_nodes->insert("background",       PNEW(PBackgroundUnpacker));
    m_nodes->insert("node",             PNEW(PSceneNodeUnpacker));

    // TODO: more

    return true;
}

void PNodeFactory::uninitialize()
{
    PStringMap<PNodeUnpacker*>::iterator it = m_nodes->begin();
    PStringMap<PNodeUnpacker*>::iterator ie = m_nodes->end(); 

    while (it != ie)
    {
        PDELETE(it.value());
        ++it;
    }
    
    PDELETE(m_nodes);
}
