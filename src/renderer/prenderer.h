// prenderer.h
// Classic forward rendering. In this renderer, we only support
// shadow casted by one directional light. The renderer first
// execute a shadow map generation pass if exists, which follows
// an forward render pass and finally executes a list of post
// processing passes.
//
// Copyright 2012 - 2014 Future Interface . 
// This software is licensed under the terms of the MIT license.
//
// Hongwei Li lihw81@gmail.com
//

#ifndef PRENDERER_H
#define PRENDERER_H

#include <Paper3D/prenderpass.h>

#include <PFoundation/pcolorrgba.h>
#include <PFoundation/parray.h>


class PRenderPass;
class PCamera;
class PRenderState;


class P_DLLEXPORT PRenderer 
{
    PRenderer(const PRenderer &other) {}
    void operator=(const PRenderer &other) {}

public:
    PRenderer(PScene *scene);
    ~PRenderer();

    void setBackgroundColor(const PColorRGBA &color);
    const PColorRGBA &backgroundColor() const;

    // Update the states of renderer when the scene is updated.
    void update();

    // Render pass by pass.
    void render(PRenderState *renderState);

    PRenderPass *renderPass(const pchar *name) const;
    void addRenderPass(PRenderPass *pass);
    void removeRenderPass(const pchar *name);

private:
    PArray<PRenderPass *>  m_renderPassArray;
    PScene                *m_scene;
    PColorRGBA             m_backgroundColor;
};


#endif // !PFORWARDRENDERER_H

