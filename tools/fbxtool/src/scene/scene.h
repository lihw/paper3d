// scene.h
// FBXConverter.
//
// Copyright 2012 - 2014 Future Interface. 
// This software is licensed under the terms of the MIT license.
//
// Hongwei Li lihw81@gmail.com
//

#ifndef SCENE_H
#define SCENE_H

#include "../assetconverter.h"

#include <vector>

#include <common/filestream.h>

class SceneNode
{
public:
    FbxString type;
    std::vector<std::pair<FbxString, FbxString> > attributes;
    std::vector<SceneNode *> children;

    FbxString geometry;
    FbxString texture;

    ~SceneNode()
    {
        for (size_t i = 0; i < children.size(); ++i)
        {
            delete children[i];
        }
    }
};

class Scene
{
public:
    Scene();
    ~Scene();

    bool output(const char *filename);

    void addNode(SceneNode *node, SceneNode *parent = NULL);

    void setCamera(SceneNode *camera);

    SceneNode *root() { return m_root; }

private:
    void outputNode(FileStream &stream, SceneNode *node, int depth);

private:
    SceneNode *m_camera;
    SceneNode *m_root;
};



#endif // !SCENE_H
