// prootnode.cpp
// The root node of the scene.
//
// Copyright 2012 - 2014 Future Interface. 
// This software is licensed under the terms of the MIT license.
//
// Hongwei Li lihw81@gmail.com
//

#include "prootnode.h"

P_OBJECT_DEFINE(PRootNode)

PRootNode::PRootNode(const pchar *name, PScene *scene)
    : PNode(name, scene, P_NULL, false)
{
    P_OBJECT_INITIALIZE_PROPERTY(PNode);
}
    
PRootNode::~PRootNode()
{
    P_OBJECT_UNINITIALIZE_PROPERTY();
}

void PRootNode::updateWorldTransform()
{
    // -------------------------------------------------------------- 
    // Update this world transformation matrix.
    // -------------------------------------------------------------- 
    m_worldTransform = m_localTransform.toMatrix4x4(&m_worldTransformChanged);
}

