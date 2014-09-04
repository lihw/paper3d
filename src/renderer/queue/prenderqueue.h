// prenderqueue.h
// The queue of a render pass of forward rendering.
//
// Copyright 2012 - 2014 Future Interface . 
// This software is licensed under the terms of the MIT license.
//
// Hongwei Li lihw81@gmail.com
//

#ifndef PRENDERQUEUE_H
#define PRENDERQUEUE_H

#include <Paper3D/prenderlightqueue.h>
#include <Paper3D/prenderdrawablequeue.h>


class PAbstractLight;
class PDrawable;
class PMaterial;
class PRenderState;


class P_DLLEXPORT PRenderQueue 
{
public:
    PRenderQueue();
    ~PRenderQueue();

    void addNode(PNode *node);
    void addNodes(PNode *node);
    void removeNodes(PNode *node);
    void render(PScene *scene, PCamera *camera, PRenderState *renderState);

    // Sort the transparent drawables w.r.t. its distance to the camera.
    // By default, it is true.
    void setZSortingEnabled(pbool enabled);
    P_INLINE pbool isZSortingEnabled() const { return m_zSorting; }

private:
    void doRender(PRenderState *renderState, PCamera *camera, PDrawable *drawable, 
            PMaterial *material, const PRenderLightQueue &lights);

private:
    PRenderDrawableQueue     m_opaques;
    PRenderDrawableQueue     m_transparents;
    PRenderLightQueue        m_lights;
    pbool                    m_zSorting;
};


#endif // !PRENDERQUEUE_H

