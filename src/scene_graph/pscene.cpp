// pscene.cpp
// The scene. Each context has one scene. In future, a context may have a
// couple of scene, but there is only one active scene at one time. The scene
// contains all the information of the renderable objects and does the
// rendering.
//
// Copyright 2012 - 2014 Future Interface. 
// This software is licensed under the terms of the MIT license.
//
// Hongwei Li lihw81@gmail.com
//
// 

#include "pscene.h"

#include <Paper3D/prenderer.h>
#include <Paper3D/prenderpass.h>
#include <Paper3D/prendertarget.h>
#include <Paper3D/pabstracteffect.h>
#include <Paper3D/presourcemanager.h>
#include <Paper3D/pcamera.h>
#include <Paper3D/prootnode.h>

#include <PFoundation/pcontext.h>
#include <PFoundation/parchivefile.h>
#include <PFoundation/pxmlelement.h>
#include <PFoundation/pxmldocument.h>
#include <PFoundation/pnew.h>
#include <PFoundation/passert.h>
#include <PFoundation/plog.h>


P_OBJECT_DEFINE(PScene)

PNodeFactory PScene::s_nodeFactory;
PEffectFactory PScene::s_effectFactory;

PScene::PScene(const pchar *name, PContext *context)
    : m_context(context)
{
    P_OBJECT_INITIALIZE_PROPERTY(PObject);
    
    m_root          = PNEW(PRootNode(name, this));
    m_mainCamera    = P_NULL;
    m_renderer      = PNEW(PRenderer(this));
    m_shadowQuality = SHADOWQUALITY_DEFAULT;
}

PScene::~PScene()
{
    P_OBJECT_UNINITIALIZE_PROPERTY();

    PDELETE(m_renderer);
    PDELETE(m_root);

    PList<PAbstractEffect *>::iterator it = m_effects.begin();
    PList<PAbstractEffect *>::iterator ie = m_effects.end();
    while (it != ie)
    {
        PAbstractEffect *effect = *it;
        effect->m_scene = P_NULL;
        PDELETE(effect);
        ++it;
    }
}

void PScene::update()
{
}

pbool PScene::load(const pchar *path)
{
    PResourceManager *resourceManager = context()->module<PResourceManager>("resource-manager");
    PArchiveFile *archive = resourceManager->archive();
    PInputStream inputStream;
    if (!archive->createInputStream(path, &inputStream))
    {
        PLOG_ERROR("Failed to read scene configuration file at %s", path);
        return false;
    }

    PXmlDocument xmlDocument;
    if (!xmlDocument.parse(inputStream))
    {
        PLOG_ERROR("The syntax is wrong in %s.", path);
        return false;
    }
        
    PXmlElement firstNode = xmlDocument.firstChildElement();

    pbool ret = unpack(&firstNode);

    return ret;
}

void PScene::pause()
{
    PNode::iterator ni(m_root);
    
    PNode *node = *(++ni); // Skip the root
    while (node != P_NULL)
    {
        node->pause();
        node = *(++ni);
    }
}

void PScene::resume()
{
    PNode::iterator ni(m_root);
    
    PNode *node = *(++ni); // Skip the root
    while (node != P_NULL)
    {
        node->resume();
        node = *(++ni);
    }
}

void PScene::render(PRenderState *renderState)
{
	// Update the world transform of each node in the scene.
	PNode::iterator ni(m_root);

	PNode *node = *ni;
	while (node != P_NULL)
	{
		// Update the transformation of each node.
		node->updateWorldTransform();
        // Update the bounding box in the world space.
        node->updateBBox();
		// Call customized update routing of each node.
		node->update();

		node = *(++ni);
	}

    // Do the rendering.
    m_renderer->render(renderState);

    // Apply the post processing effects to the scene.
    PList<PAbstractEffect *>::iterator it = m_effects.begin();
    PList<PAbstractEffect *>::iterator ie = m_effects.end();
    while (it != ie)
    {
        (*it)->render(renderState);
        ++it;
    }
}
    
void PScene::setMainCamera(PCamera *camera)
{
    PASSERT(camera != P_NULL);
    if (camera->scene() != this)
    {
        PLOG_WARNING("The camera doesn't belong to this scene");
    }

    m_mainCamera = camera;
}

void PScene::setName(const pchar *name)
{
    m_root->setName(name);
}

void PScene::setShadowQuality(ShadowQualityEnum shadowQuality)
{
    m_shadowQuality = shadowQuality;
}

void PScene::onResized(pfloat32 x, pfloat32 y, pfloat32 width, pfloat32 height)
{
    // Update the main camera's viewport.
    if (m_mainCamera != P_NULL)
    {
        m_mainCamera->setViewport(x, y, width, height);
    }
}
    
void PScene::setBackgroundColor(const PColorRGBA &color)
{
    m_renderer->setBackgroundColor(color);
}

const PColorRGBA &PScene::backgroundColor() const
{
    return m_renderer->backgroundColor();
}
    
void PScene::addEffect(PAbstractEffect *effect)
{
    PASSERT(effect != P_NULL);
    m_effects.append(effect);
}

void PScene::removeEffect(PAbstractEffect *effect)
{
    PASSERT(effect != P_NULL);
    PList<PAbstractEffect *>::iterator it = m_effects.find(effect);
    if (it != m_effects.end())
    {
        m_effects.erase(it);
    }
    else
    {
        PLOG_WARNING("Fail to find such effect.");
    }
}
    
pbool PScene::unpack(const PXmlElement* xmlElement)
{
    // Sanity check
    if (pstrcmp(xmlElement->name(), "scene") != 0)
    {
        PLOG_ERROR("It is not a scene element");
        return false;
    }

    // Delete all scene nodes
    PDELETE(m_root);

    PResourceManager *resourceManager = context()->module<PResourceManager>("resource-manager");

    // Delete all effects
    PList<PAbstractEffect *>::iterator it = m_effects.begin();
    PList<PAbstractEffect *>::iterator ie = m_effects.end();
    while (it != ie)
    {
        PAbstractEffect *effect = *it;
        effect->m_scene = P_NULL;
        PDELETE(effect);
        ++it;
    }

    // Recursively unpack its children
    PXmlElement childElement = xmlElement->firstChild();
    while (childElement.isValid())
    {
        if (pstrcmp(childElement.name(), "root") == 0)
        {
            const pchar *rootNameValue = childElement.attribute("name");
            PASSERT(rootNameValue != P_NULL);
            m_root = PNEW(PRootNode(rootNameValue, this));
            if (!m_root->unpack(&childElement))
            {
                return false;
            }
        }
        else if (pstrcmp(childElement.name(), "effect") == 0)
        {
            PAbstractEffect *effect = s_effectFactory.unpack(&childElement, this);
            if (effect == P_NULL)
            {
                return false;
            }
        }
        else if (pstrcmp(childElement.name(), "texture") == 0)
        {
            const pchar *textureId = childElement.attribute("id");
            if (textureId != P_NULL)
            {
                PTexture *texture = resourceManager->getTexture(textureId);
                if (texture == P_NULL)
                {
                    PLOG_ERROR("Failed to find texture %s.", textureId);
                }
                else
                {
                    const pchar *wrapValue = childElement.attribute("wrap");
                    if (wrapValue != P_NULL)
                    {
                        if (pstrcmp(wrapValue, "repeat") == 0)
                        {
                            texture->setRepeatWrappingEnabled(true, true);
                        }
                    }
                    
                    const pchar *mipmapValue = childElement.attribute("mipmap");
                    if (mipmapValue != P_NULL)
                    {
                        if (pstrcmp(mipmapValue, "true") == 0)
                        {
                            // TODO:
                            PASSERT_NOTIMPLEMENTED();
                        }
                    }
                }
            }
            else
            {
                PLOG_ERROR("Failed to find the id of the texture is missing.");
            }
        }
        else
        {
            PLOG_WARNING("Unknown element node (%s) in scene.", childElement.name());
        }

        childElement = childElement.nextSibling();
    } 

    // Find the main camera.
    const pchar *mainCameraValue = xmlElement->attribute("maincamera");
    m_mainCamera = P_NULL;
    if (mainCameraValue == P_NULL)
    {
        // When the main camera is not specified, use the first camera in the 
        // scene nodes as the main camera.
        PNode::iterator ni(m_root);
        
        PNode *node = *(++ni); // Skip the root
        while (node != P_NULL)
        {
            PCamera *camera = dynamic_cast<PCamera *>(node);
            if (camera != P_NULL)
            {
                m_mainCamera = camera;
                break;
            }
            node = *(++ni);
        }

        if (m_mainCamera == P_NULL)
        {
            PLOG_WARNING("No camera is found. The scene may not be rendered properly.");
        }
    }
    else
    {
        PNode::iterator ni(m_root);
        
        PNode *node = *(++ni); // Skip the root
        while (node != P_NULL)
        {
            if (node->name() == mainCameraValue)
            {
                PCamera *camera = dynamic_cast<PCamera *>(node);
                if (camera != P_NULL)
                {
                    m_mainCamera = camera;
                    break;
                }
            }
            node = *(++ni);
        }
        
        if (m_mainCamera == P_NULL)
        {
            PLOG_WARNING("There is no such camera called %s in the scene.");
        }
    }

    return true;
}
    
