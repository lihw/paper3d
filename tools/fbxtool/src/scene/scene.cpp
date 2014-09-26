// scene.cpp
// FBXConverter.
//
// Copyright 2012 - 2014 Future Interface. 
// This software is licensed under the terms of the MIT license.
//
// Hongwei Li lihw81@gmail.com
//

#include "scene.h" 

#include <common/filestream.h>
#include <common/log.h>

Scene::Scene()
{
    m_root = NULL;
    m_camera = NULL;
}

Scene::~Scene()
{
    delete m_root;
}

bool Scene::output(const char *filename)
{
    FileStream stream(filename);
    if (!stream.openToWrite())
    {
        FBXSDK_printf("Fail to create %s.\n", filename);
        return false;
    }

    int depth = 0;

    if (m_camera != NULL)
    {
        stream.printf("<scene maincamera=\"%s\">\n", m_camera->attributes[0].second.Buffer());
    }
    else
    {
        stream.printf("<scene>\n");
    }

    outputNode(stream, m_root, ++depth);
    
    stream.printf("</scene>");

    return true;
}
    
void Scene::addNode(SceneNode *node, SceneNode *parent)
{
    if (parent != NULL)
    {
        parent->children.push_back(node);
    }
    else
    {
        FBX_ASSERT(m_root == NULL);
        m_root = node;
    }
}

void Scene::outputNode(FileStream &stream, SceneNode *node, int depth)
{
    // The indent
    for (int i = 0; i < depth; ++i)
    {
        stream.printf("  ");
    }
        
    for (int i = 0; i < depth; ++i)
    {
        FBXSDK_printf("  ");
    }

    // Output the type and attributes;
    stream.printf("<%s ", node->type.Buffer());
    for (size_t i = 0; i < node->attributes.size(); i++)
    {
        stream.printf("%s=\"%s\" ", node->attributes[i].first.Buffer(),
                node->attributes[i].second.Buffer());
    }
    stream.printf(">\n");
    
    // Only output the geometry and material of root.
    if (depth > 1 && node->type == "drawable")
    {
        // Geometry
        for (int i = 0; i <= depth; ++i)
        {
            stream.printf("  ");
        }
        stream.printf("<geometry id=\"mesh/%s\" />\n", node->geometry.Lower().Buffer());
        
        // Texture
        // FIXME: more complex material
        for (int i = 0; i <= depth; ++i)
        {
            stream.printf("  ");
        }
        FbxString texName = node->texture;
        FbxString pngTexName = FbxPathUtils::ChangeExtension(texName, ".png");
        stream.printf("<material id=\"material/texture.pmt\" texture=\"texture/%s\" />\n", pngTexName.Buffer());
        logInfo("%s(%s) mesh:%s tex:%s", node->attributes[0].second.Buffer(), 
            node->type.Buffer(), node->geometry.Lower().Buffer(), pngTexName.Buffer());
    }
    else
    {
        logInfo("%s(%s)", node->attributes[0].second.Buffer(), node->type.Buffer());
        // TODO: don't output "node" node which has no child.
    }

    // Output children
    for (size_t i = 0; i < node->children.size(); i++)
    {
        SceneNode *child = node->children[i];
        outputNode(stream, child, depth + 1);
    }

    // The indent
    for (int i = 0; i < depth; ++i)
    {
        stream.printf("  ");
    }
    stream.printf("</%s>\n", node->type.Buffer());
}
    
void Scene::setCamera(SceneNode *camera)
{
    if (m_camera == NULL)
    {
        m_camera = camera;
    }
}
