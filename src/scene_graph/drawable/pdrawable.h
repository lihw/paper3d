// pdrawble.h
// The drawable object.
//
// Copyright 2012 - 2014 Future Interface. 
// This software is licensed under the terms of the MIT license.
//
// Hongwei Li lihw81@gmail.com
//
// 

#ifndef PDRAWABLE_H
#define PDRAWABLE_H

#include <Paper3D/pnode.h>
#include <Paper3D/pnodefactory.h>

#include <PFoundation/ppropertybool.h>


class PAbstractGeometry;
class PMaterial;
class PRenderState;

// The smallest the priority number, the higher the rendering priority it is.
// Users can either use native values or choose something between 1 and 0xffffffff
// for certain rendering order.
// Note that when drawable is a transparent one, the rendering order is dependent
// on their z coordinate in the camera space rather than priority.
enum PDrawablePriorityEnum
{
    P_DRAWABLE_PRIORITY_DRAWLAST  = 0x01,
    P_DRAWABLE_PRIORITY_DRAWFIRST = 0xffffffff,
    P_DRAWABLE_PRIORITY_COMMON    = 0xffff,
};


class P_DLLEXPORT PDrawable : public PNode
{
    P_OBJECT

    friend class PScene;
    friend class PRenderQueue;

private:
    PDrawable(const PDrawable &other) : PNode(P_NULL, P_NULL) {}
    void operator=(const PDrawable &other) {}

public:
    PDrawable(const pchar *name, PNode* parent, PDrawablePriorityEnum priority = P_DRAWABLE_PRIORITY_COMMON);
    PDrawable(const pchar *name, PScene *scene, PDrawablePriorityEnum priority = P_DRAWABLE_PRIORITY_COMMON);
    virtual ~PDrawable();

    P_INLINE pbool castShadow() const { return m_castShadow.toBool(); }
    P_INLINE pbool receiveShadow() const { return m_receiveShadow.toBool(); }
    P_INLINE pbool isVisible() const { return m_visible.toBool(); };
    P_INLINE pbool isPickable() const { return m_pickable.toBool(); };
    P_INLINE PAbstractGeometry *geometry() const { return m_geometry; }
    P_INLINE PMaterial *material() const { return m_material; }
    P_INLINE puint32 priority() const { return m_priority; }

    void setVisible(pbool visible);
    void setPickable(pbool pickable);
    void setCastShadow(pbool castShadow);
    void setReceiveShadow(pbool receiveShadow);
    void setGeometry(PAbstractGeometry *geometry);
    void setMaterial(PMaterial *material);

    // Generate the model->camera space transformation 4x4 matrix.
    virtual void calculateModelCameraMatrix(const PMatrix4x4 &cameraMatrix, pfloat32 *out_matrix);

    virtual pbool unpack(const PXmlElement* xmlElement) override;

    static PDrawable *create(const pchar *name, PDrawable *parent, 
        const pchar *geometry, const pchar *material);
    static PDrawable *create(const pchar *name, PScene *scene, 
        const pchar *geometry, const pchar *material);
    
protected:
    virtual void resume();
    virtual void pause();

    // Right before the drawable is about to be rendered. For any actual rendering of
    // other drawables, we should put it inside this function. For example, if a texture
    // of this drawable is a screencapture of the entire scene, we can set up an off-line
    // framebuffer and do the rendering of other objects inside prepareRender(). Note that
    // we should restore the renderstate after finishing the off-line rendering.
    virtual void prepareRender(PRenderState *renderState);
    // Right after the drawable is rendered.
    virtual void postRender();

protected:
    puint32               m_priority;

private:
    PAbstractGeometry    *m_geometry;
    PMaterial            *m_material;
    PPropertyBool         m_visible;
    PPropertyBool         m_pickable;
    PPropertyBool         m_castShadow;
    PPropertyBool         m_receiveShadow;
};


#endif  //!PDRAWABLE_H

