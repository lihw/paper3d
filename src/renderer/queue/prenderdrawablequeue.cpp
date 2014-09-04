// prenderdrawablequeue.h
// Render queue to host a number of drawables
//
// Copyright 2012 - 2014 Future Interface . 
// This software is licensed under the terms of the MIT license.
//
// Hongwei Li lihw81@gmail.com
//


#include "prenderdrawablequeue.h"

#include <Paper3D/pdrawable.h>
#include <Paper3D/pcamera.h>
#include <Paper3D/pmaterial.h>

#include <PFoundation/passert.h>
#include <PFoundation/pmatrix4x4.h>


PRenderDrawableQueue::PRenderDrawableQueue(pbool transparent)
{
    m_transparent = transparent;
}

PRenderDrawableQueue::~PRenderDrawableQueue()
{
}

void PRenderDrawableQueue::addDrawable(PDrawable *drawable)
{
    // Sanity check
    PASSERT(m_transparent == drawable->material()->isTransparent());
    if (m_transparent != drawable->material()->isTransparent())
    {
        PLOG_ERROR("Can't put opaque and transparent in one render queue.");
        return ;
    }
    
    // Bubble the drawable w.r.t. its rendering priority for the opaque queue.
    m_drawables.append(drawable);
    if (!m_transparent)
    {
        for (puint32 i = m_drawables.count() - 1; i > 0; --i)
        {
            if (m_drawables[i]->priority() > m_drawables[i - 1]->priority())
            {
                pSwap(m_drawables[i], m_drawables[i - 1]);
            }
        }
    }
}

void PRenderDrawableQueue::sort(PCamera *camera)
{
    // We only need to sort transparent objects
    PASSERT(m_transparent);

    pfloat32 distToCamera[1024];
    for (puint32 i = 0; i < m_drawables.count(); ++i) 
    {
        const PMatrix4x4 &cameraMatrix = camera->cameraTransform();
        const PMatrix4x4 &worldMatrix =  m_drawables[i]->worldTransform();
        pfloat32 modelview[16];
        pMatrix4x4Multiply(cameraMatrix.m_m, worldMatrix.m_m, modelview);

        distToCamera[i] = modelview[14];
    }
    for (puint32 i = 0; i < m_drawables.count() - 1; ++i) 
    {
        puint32 minId = i;
        for (puint32 j = i + 1; j < m_drawables.count(); ++j) 
        {
            if (distToCamera[i] > distToCamera[j])
            {
                minId = j;
            }
        }

        if (minId != i)
        {
            pSwap(distToCamera[minId], distToCamera[i]);
            pSwap(m_drawables[minId], m_drawables[i]);    
        }
    }
}
    
void PRenderDrawableQueue::removeDrawable(PDrawable *drawable)
{
    for (puint32 i = 0; i < m_drawables.count(); ++i)
    {
        if (drawable == m_drawables[i])
        {
            pSwap(m_drawables[m_drawables.count() - 1], m_drawables[i]); 
            m_drawables.resize(m_drawables.count() - 1);
            return ;
        }
    }

    PLOG_WARNING("Failed to find drawable %s in the queue.", drawable->name().c_str());
}

puint32 PRenderDrawableQueue::count() const
{
    return m_drawables.count();
}

PDrawable *PRenderDrawableQueue::operator[](puint32 i)
{
    return m_drawables[i];
}

