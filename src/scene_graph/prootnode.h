// prootnode.h
// The root node of the scene.
//
// Copyright 2012 - 2014 Future Interface. 
// This software is licensed under the terms of the MIT license.
//
// Hongwei Li lihw81@gmail.com
//


#ifndef PROOTNODE_H
#define PROOTNODE_H

#include <Paper3D/pnode.h>

class P_DLLEXPORT PRootNode: public PNode
{
    P_OBJECT

    friend class PScene;

public:
    PRootNode(const pchar *name, PScene *scene);
    virtual ~PRootNode();

protected:
    virtual void updateWorldTransform();
};

#endif // !PROOTNODE_H
