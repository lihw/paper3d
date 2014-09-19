// pnodefactory.h
// Generate a scene node object from a xml node.
//
// Copyright 2012 - 2014 Future Interface. 
// This software is licensed under the terms of the MIT license.
//
// Hongwei Li lihw81@gmail.com
//
// 

#ifndef PNODEFACTORY_H
#define PNODEFACTORY_H

#include <PFoundation/pstringmap.h>


class PNode;
class PXmlElement;


class P_DLLEXPORT PNodeUnpacker
{
public:
    PNodeUnpacker() {}
    virtual ~PNodeUnpacker() {}
    
    virtual PNode *unpack(PXmlElement *xmlElement, PNode *parent) = 0;
};

class P_DLLEXPORT PNodeFactory
{
    PNodeFactory(const PNodeFactory &other) {}
    void operator=(const PNodeFactory &other) {}

public:
    PNodeFactory();
    ~PNodeFactory();

    PNode *unpack(PXmlElement *xmlElement, PNode *parent);

    pbool initialize();
    void uninitialize();

protected:
    PStringMap<PNodeUnpacker*> *m_nodes;
};

#endif // !PNODEFACTORY_H


