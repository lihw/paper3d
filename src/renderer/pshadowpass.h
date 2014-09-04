// pshadowpass.h
// Render pass that generates shadow map
//
// Copyright 2012 - 2014 Future Interface . 
// This software is licensed under the terms of the MIT license.
//
// Hongwei Li lihw81@gmail.com
//

#ifndef PSHADOWPASS_H
#define PSHADOWPASS_H

#include <Paper3D/prenderpass.h>

class PFrameBuffer;


class P_DLLEXPORT PShadowPass : public PRenderPass
{
    friend class PRenderer;

public:
    PShadowPass(const pchar *name, PScene *scene);
    virtual ~PShadowPass();
    
    virtual void render(PRenderState *renderState);

private:
    // Invalidate some interface methods in PRenderPass that are not 
    // available in PShadowPass.
    void setOverridedMaterial(PMaterial *material) {}
    void setCamera(PCamera *camera) {}
    void setRenderQueue(PRenderQueue *renderQueue) {}

private:
    PFrameBuffer *m_framebuffer;
};


#endif // !PSHADOWPASS_H
