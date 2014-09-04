// pmirror.cpp
// "Reflection" example
//
// Copyright 2012 - 2014 Future Interface. 
// This software is licensed under the terms of the MIT license.
//
// Hongwei Li lihw81@gmail.com
//

#include "pmirror.h"

#include <Paper3D/pcamera.h>
#include <Paper3D/pscene.h>
#include <Paper3D/pgeometryplane.h>
#include <Paper3D/pframebuffer.h>
#include <Paper3D/pmaterial.h>
#include <Paper3D/pmaterialparameter.h>
#include <Paper3D/presourcemanager.h>
#include <Paper3D/prenderstate.h>
#include <Paper3D/prenderer.h>
#include <Paper3D/prendertarget.h>
#include <Paper3D/prenderpass.h>
#include <Paper3D/prenderqueue.h>

#include <PFoundation/pcontext.h>
#include <PFoundation/pvector4.h>
#include <PFoundation/pplane.h>
#include <PFoundation/pglframebuffer.h>


P_OBJECT_DEFINE(PMirror)


PMirror::PMirror(const pchar *name, PNode *parent)
    : PDrawable(name, parent)
{
    P_OBJECT_INITIALIZE_PROPERTY(PDrawable)

    initialize();
}

PMirror::PMirror(const pchar *name, PScene *scene)
    : PDrawable(name, scene)
{
    P_OBJECT_INITIALIZE_PROPERTY(PDrawable)

    initialize();
}

PMirror::~PMirror()
{
    P_OBJECT_UNINITIALIZE_PROPERTY();

    m_reflectionFB->release();

	PDELETE(m_reflectionPass);
}
    
void PMirror::initialize()
{
    m_reflectionCamera = PNEW(PCamera("reflection", this));
    m_reflectionCamera->setFixed(true);

    // Framebuffer
    PResourceManager *resMgr = scene()->context()->module<PResourceManager>("resource-manager");
    
    const puint32 *rect = scene()->context()->rect();
    m_reflectionFB = resMgr->createFrameBuffer(P_NULL,
                                               rect[2],
                                               rect[3],
                                               P_GLTEXTURE_FORMAT_RGBA8888,
                                               P_GLTEXTURE_FORMAT_DEPTH_COMPONENT16);
    m_reflectionFB->retain();

    // Geometry and material
    setGeometry(PNEW(PGeometryPlane(resMgr)));
    setMaterial(PNEW(PMaterial("reflection.pmt", resMgr)));

    material()->parameter("texture")      = m_reflectionFB->colorBuffer();
    material()->parameter("inv-viewport") = pVector2(1.0f / (pfloat32)rect[2], 1.0f / (pfloat32)rect[3]);
    material()->parameter("color")        = pVector3(0.0f, 0.0f, 1.0f);
    material()->parameter("blend")        = 0.7f;

	// Render pass
    m_reflectionPass = PNEW(PRenderPass("reflection", scene())); 

}
    
void PMirror::prepareRender(PRenderState *renderState)
{
    // Delay the initialization of the reflection pass.
    if (m_reflectionPass->camera() == P_NULL)
    {
        m_reflectionPass->target()->setFrameBuffer(m_reflectionFB);
        m_reflectionPass->setCamera(m_reflectionCamera);
        m_reflectionPass->queue()->addNodes(m_scene->root());
        m_reflectionPass->queue()->removeNodes(this);   // Remove myself to avoid recursive rendering
    }

    PPlane plane(0, 0, 1, 0);
    plane.transform(m_worldTransform);

    PCamera *mainCamera = scene()->mainCamera();
    m_reflectionCamera->copyOf(*mainCamera);
    m_reflectionCamera->reflect(plane);
    
    // Render
    renderState->renderStateObject()->setCull(P_GLCULL_FRONT);
    m_reflectionPass->render(renderState);
	renderState->renderStateObject()->setCull(P_GLCULL_BACK);
    
    // FIXME: restore the default framebuffer
    renderState->useFrameBuffer(P_NULL);
}

