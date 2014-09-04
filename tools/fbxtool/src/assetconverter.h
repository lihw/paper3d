// assetconverter.h
// FbxTool.
//
// Copyright 2012 - 2014 Future Interface. 
// This software is licensed under the terms of the MIT license.
//
// Hongwei Li lihw81@gmail.com
//

#ifndef ASSETCONVERTER_H
#define ASSETCONVERTER_H

#include <fbxsdk.h>
#include <list>

struct Arguments;

class AssetConverter 
{
public:
    AssetConverter(FbxScene *scene, Arguments *arguments);

protected:
    FbxNode *firstNode();
    FbxNode *nextNode();
    
protected:
    Arguments *m_arguments;
    FbxScene *m_scene;

private:
    std::list<FbxNode *> m_nodes;
};


#endif // !ASSETCONVERTER_H

