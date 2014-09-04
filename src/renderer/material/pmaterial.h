// pmaterial.h 
// The used defined material class.
//
// Copyright 2012 - 2014 Future Interface . 
// This software is licensed under the terms of the MIT license.
//
// Hongwei Li lihw81@gmail.com
//
// 

#ifndef PMATERIAL_H
#define PMATERIAL_H

#include <PFoundation/pentity.h>
#include <PFoundation/parray.h>
#include <PFoundation/pglvertexbuffer.h>


class PCamera;
class PRenderLightQueue;
class PMaterialParameter;
class PRenderState;
class PDrawable;
class PMaterialResource;
class PResourceManager;
class PXmlElement;


class P_DLLEXPORT PMaterial : public PEntity 
{
    friend class MyScene;

    PMaterial(const PMaterial &other) {}
    void operator=(const PMaterial &other) {}

public:
    PMaterial(const pchar *id, PResourceManager *resourceManager);
    PMaterial(const pchar *id, const pchar *text, pbool autoRelease, PResourceManager *resourceManager);
    virtual ~PMaterial();

    // Used in common drawable rendering.
    void apply(PRenderState            *renderState,
               PDrawable               *drawable,
               PCamera                 *camera,
               const PRenderLightQueue &lights);
    // Only used in effects.
    void apply(PRenderState *renderState);

    P_INLINE puint32 numberOfMaterialParameters() const { return m_materialParameters.count(); }
    P_INLINE PMaterialParameter *materialParameter(puint32 index) const { return m_materialParameters[index]; }
    P_INLINE pbool isTransparent() const { return m_isTransparent; }
    P_INLINE pbool isValid() const { return m_resource != P_NULL; }

    void setTransparent(pbool transparent);
    PGlVertexFormat *vertexFormat() const;
    PMaterialParameter &parameter(const pchar *name) const; 
    
    static PMaterial *unpack(PXmlElement *element, PResourceManager *resourceManager);

protected:
    PMaterialResource                  *m_resource;
    pbool                               m_isTransparent;

    // We don't have copies of render parameters because their value
    // are fetched from other sources or computed on the fly. On the
    // other side, each material instance should have its own set of
    // material parameters because they can have different values.
    // TODO: replace with dictionary
    PArray<PMaterialParameter *>        m_materialParameters;
    static PMaterialParameter          *s_phonyMaterialParameter;
};

#endif // !PMATERIAL_H


