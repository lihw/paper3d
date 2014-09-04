// pscene.h 
// The scene. Each context has many scenes, but there is only one active scene
// at one time. The scene contains all the information of the renderable
// objects and does the rendering.
//
// Copyright 2012 - 2014 Future Interface. 
// This software is licensed under the terms of the MIT license.
//
// Hongwei Li lihw81@gmail.com
//
// 

#ifndef PSCENE_H
#define PSCENE_H


#include <Paper3D/pnode.h>
#include <Paper3D/pnodefactory.h>
#include <Paper3D/peffectfactory.h>

#include <PFoundation/pobject.h>
#include <PFoundation/ppropertystring.h>
#include <PFoundation/ppropertytransform.h>
#include <PFoundation/pcolorrgba.h>
#include <PFoundation/pbox.h>


class PContext;
class PRenderState;
class PCamera;
class PRenderer;
class PAbstractEffect;


class P_DLLEXPORT PScene : public PObject
{
    P_OBJECT

    friend class PRenderQueue; // FIXME: avoid access private members of PScene
    friend class PAbstractEffect;

    PScene(const PScene &other) {}
    void operator=(const PScene &other) {}

public:
    enum ShadowQualityEnum 
    {
        SHADOWQUALITY_HIGH,
        SHADOWQUALITY_NORMAL,
        SHADOWQUALITY_LOW,

        SHADOWQUALITY_DEFAULT = SHADOWQUALITY_NORMAL,
    };
public:
    PScene(const pchar *name, PContext *context);
    virtual ~PScene();

    P_INLINE PNode *root() const  { return m_root; }
    P_INLINE PContext *context() const  { return m_context; }
    P_INLINE PCamera *mainCamera() const { return m_mainCamera; }
    P_INLINE PRenderer *renderer() const { return m_renderer; }
    P_INLINE ShadowQualityEnum shadowQuality() const { return m_shadowQuality; }
    P_INLINE const PBox *bbox() const { return &m_bbox; }

    void setMainCamera(PCamera *camera);
    void setName(const pchar *name);
    void setBackgroundColor(const PColorRGBA &color);
    // FIXME: only works before the first frame is drawn.
    void setShadowQuality(ShadowQualityEnum quality);

    const PColorRGBA &backgroundColor() const;

    virtual void update();
    virtual void pause();
    virtual void resume();
    virtual void render(PRenderState *renderState);
    virtual void onResized(pfloat32 x, pfloat32 y, pfloat32 width, pfloat32 height);

    // Load a scene from a .psc file.
    pbool load(const pchar *path);

protected:
    virtual pbool unpack(const PXmlElement* xmlElement);

private:
    void addEffect(PAbstractEffect *effect);
    void removeEffect(PAbstractEffect *effect);

public:
    static PNodeFactory    s_nodeFactory;
    static PEffectFactory  s_effectFactory;

protected:
    PContext                *m_context;
    PNode                   *m_root;
    PRenderer               *m_renderer;
    PCamera                 *m_mainCamera;
    PList<PAbstractEffect *> m_effects;
    ShadowQualityEnum        m_shadowQuality;
    PBox                     m_bbox; 
};

#endif // !PSCENE_H

