// pnode.cpp
// Every object in a scene is a node. A node has transformation interface and
// hierachical organization.
//
// Copyright 2012 - 2014 Future Interface. 
// This software is licensed under the terms of the MIT license.
//
// Hongwei Li lihw81@gmail.com
//
// 

#include "pnode.h"

#include <Paper3D/pscene.h>

#include <PFoundation/pxmlelement.h>


P_OBJECT_DEFINE(PNode)

PNode::PNode(const pchar *name, PScene *scene, PNode *parent, pbool isDrawable)
    : PObject()
{
    P_OBJECT_INITIALIZE_PROPERTY(PObject);

    P_OBJECT_REGISTER_PROPERTY("name",      &m_name);
    P_OBJECT_REGISTER_PROPERTY("transform", &m_localTransform);
    
    m_drawable = isDrawable;
    
    PASSERT(name != P_NULL);
    m_name = PString(name);

    PASSERT(scene != P_NULL);
    m_scene = scene;

    m_parent = parent;
    if (m_parent != P_NULL)
    {
        m_parent->addChild(this);
    }
}

PNode::~PNode()
{
    // Delete all its children
    PStringMap<PNode *>::iterator it = m_children.begin();
    PStringMap<PNode *>::iterator ie = m_children.end();
    while (it != ie)
    {
        PNode *child = it.value();
        child->m_parent = P_NULL;
        PDELETE(child);

        ++it;
    }

    // Remove it from its parent
    if (m_parent != P_NULL)
    {
        m_parent->removeChild(m_name.toString().c_str());
    }

    P_OBJECT_UNINITIALIZE_PROPERTY();
}

PNode *PNode::child(const pchar *name) const
{
    PStringMap<PNode *>::const_iterator it = m_children.find(const_cast<pchar*>(name));
    if (it != m_children.end())
    {
        return it.value();
    }
    return P_NULL;
}

void PNode::removeChild(const pchar *name)
{
    PStringMap<PNode *>::iterator it = m_children.find(const_cast<pchar*>(name));
    if (it != m_children.end())
    {
        m_children.erase(it);

        onChildRemoved(it.value());
    }
    else
    {
        PLOG_ERROR("Failed to find the child %s in node %s", name, m_name.toString().c_str());
    }
}

void PNode::setParent(PNode *parent)
{
    PASSERT(parent != P_NULL);
    // This new parent can't be child of this object node.
    if (parent != P_NULL)
    {
        if (child(parent->name().c_str()) != P_NULL)
        {
            PLOG_WARNING("Can't have its child to be the parent");
            return ;
        }
        
        m_parent->removeChild(m_name.toString().c_str());
    
        m_parent = parent;
        
        m_parent->addChild(this);
    }
}

void PNode::setName(const pchar *name)
{
    if (m_name.toString() == name)
    {
        return ;
    }

    PString oldName = m_name.toString();
    
    PASSERT(name != P_NULL);
    m_name.setValue(name);

    // Remove from its parent.
    PStringMap<PNode*>::iterator it = m_parent->m_children.find(oldName.c_str());
    PASSERT(it != m_children.end());
    m_parent->m_children.erase(it);

    // and insert again
    m_parent->m_children.insert(const_cast<pchar *>(m_name.toString().c_str()), this);
}

void PNode::addChild(PNode *child)
{
    if (this->child(child->m_name.toString().c_str()) != P_NULL)
    {
        PLOG_ERROR("Cannot add another %s to the parent node %s ",
            m_name.toString().c_str(), child->m_name.toString().c_str());
    }
    else
    {
        pchar *name = const_cast<pchar*>(child->name().c_str());
        m_children.insert(name, child);
        
        onChildAdded(child);
    }
}
    
void PNode::onChildAdded(PNode *child)
{
}

void PNode::onChildRemoved(PNode *child)
{
}
    
void PNode::update()
{
}

void PNode::pause()
{
}

void PNode::resume()
{
}
    
pbool PNode::unpack(const PXmlElement* xmlElement)
{
    // Delete all its children
    PStringMap<PNode *>::iterator it = m_children.begin();
    PStringMap<PNode *>::iterator ie = m_children.end();
    while (it != ie)
    {
        PNode *child = it.value();
        child->m_parent = P_NULL;
        PDELETE(child);

        ++it;
    }

    // Unpack its children
    PXmlElement childElement = xmlElement->firstChild();
    while (childElement.isValid())
    {
        PNode *node = PScene::s_nodeFactory.unpack(&childElement, this);
        if (node == P_NULL)
        {
            return false;
        }

        childElement = childElement.nextSibling();
    } 

    return PObject::unpack(xmlElement);
}

void PNode::updateWorldTransform()
{
    // -------------------------------------------------------------- 
    // Update this world transformation matrix.
    // -------------------------------------------------------------- 
    pbool localTransformChanged = false;

    const PMatrix4x4 &localTransform = m_localTransform.toMatrix4x4(&localTransformChanged);

    // Update the this world transformation either when the parent has its own 
    // or the local transformation is changed.
    if (m_parent->worldTransformChanged() || localTransformChanged)
    {
        m_worldTransform =  m_parent->worldTransform() * localTransform;
        m_worldTransformChanged = true;
    }
    else
    {
        m_worldTransformChanged = false;
    }
}
    
    
void PNode::updateBBox()
{
    // TODO:
}
