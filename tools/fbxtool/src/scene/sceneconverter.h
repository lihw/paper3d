// sceneconverter.h
// FBXConverter.
//
// Copyright 2012 - 2014 Future Interface. 
// This software is licensed under the terms of the MIT license.
//
// Hongwei Li lihw81@gmail.com
//

#ifndef SCENECONVERTER_H
#define SCENECONVERTER_H

#include <vector>

#include <fbxsdk.h>

#include "../assetconverter.h"

class Scene;
class SceneNode;

class SceneConverter : public AssetConverter
{
public:
    SceneConverter(FbxScene *scene, Arguments *arguments);

    bool convert();

public:
	std::vector<FbxString> m_textures;

private:
    SceneNode *makeSceneNode(FbxNode *node);

    int m_numDrawables;
    int m_numCameras;
    int m_numLights;
};



#endif // !SCENECONVERTER_H
