// prenderpass.h
// The rendering pass
//
// Copyright 2012 - 2014 Future Interface . 
// This software is licensed under the terms of the MIT license.
//
// Hongwei Li lihw81@gmail.com
//

#ifndef PRENDERPASS_H
#define PRENDERPASS_H

#include <PFoundation/pstring.h>
#include <PFoundation/pentity.h>

class PCamera;
class PMaterial;
class PRenderQueue;
class PScene;
class PRenderTarget;
class PRenderState;


class P_DLLEXPORT PRenderPass : public PEntity
{
    friend class PRenderer;

public:
    PRenderPass(const pchar *name, PScene *scene);
    virtual ~PRenderPass();

    P_INLINE PRenderTarget *target() const       { return m_renderTarget; }
    P_INLINE PCamera *camera() const             { return m_camera; }
    P_INLINE PRenderQueue *queue() const         { return m_renderQueue; }
    P_INLINE const PString &name() const         { return m_name; }
    P_INLINE PMaterial *overrideMaterial() const { return m_overridedMaterial; }
    
    virtual void render(PRenderState *renderState);
    
    void setOverridedMaterial(PMaterial *material);
    void setCamera(PCamera *camera);
    void setRenderTarget(PRenderTarget *renderTarget);
    void setRenderQueue(PRenderQueue *renderQueue);

protected:
    PRenderTarget    *m_renderTarget;
    PRenderQueue     *m_renderQueue;
    PCamera          *m_camera;
    PScene           *m_scene;
    PString           m_name;
    PMaterial        *m_overridedMaterial;
};


#endif // !PPASS_H

