// assetconverter.cpp
// FBXConverter.
//
// Copyright 2012 - 2014 Future Interface. 
// This software is licensed under the terms of the MIT license.
//
// Hongwei Li lihw81@gmail.com
//

#include "assetconverter.h"


AssetConverter::AssetConverter(FbxScene *scene, Arguments *arguments)
{
    m_scene = scene;
    m_arguments = arguments;
}

FbxNode *AssetConverter::firstNode()
{
    FbxNode *node = m_scene->GetRootNode();

    if (node)
    {
        for (int i = 0; i < node->GetChildCount(); i++)
        {
            m_nodes.push_back(node->GetChild(i));
        }
    }

    if (!m_nodes.empty())
    {
        FbxNode *ret = m_nodes.front();
        m_nodes.pop_front();
        return ret;
    }

    return NULL;
}

FbxNode *AssetConverter::nextNode()
{
    if (!m_nodes.empty())
    {
        FbxNode *ret = m_nodes.front();
        for (int i = 0; i < ret->GetChildCount(); i++)
        {
            m_nodes.push_back(ret->GetChild(i));
        }
        m_nodes.pop_front();
        return ret;
    }

    return NULL;
}
