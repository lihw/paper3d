// pmaterialresource.h
// The resource of material. All material of the same kind
// share one resource object.
// 
// Copyright 2012 - 2014 Future Interface . 
// This software is licensed under the terms of the MIT license.
//
// Hongwei Li lihw81@gmail.com
//

#ifndef PMATERIALRESOURCE_H
#define PMATERIALRESOURCE_H

#include <Paper3D/presourceobject.h>

#include <PFoundation/parray.h>
#include <PFoundation/pglvertexbuffer.h>
#include <PFoundation/pglshader.h>

class PMaterialParameter;
class PAbstractRenderParameter;


class P_DLLEXPORT PMaterialResource : public PResourceObject
{
    friend class PResourceManager;
    
    PMaterialResource(const PMaterialResource& other) : PResourceObject(P_NULL, P_NULL, P_RESOURCE_TYPE_UNKNOWN) {};
    void operator=(PMaterialResource& other) {};

public:
    P_INLINE pbool isDefault() const { return m_isDefault; }
    P_INLINE PGlShader *shader() const { return m_shader; }
    P_INLINE PGlVertexFormat *vertexFormat() { return &m_vertexFormat; }
    P_INLINE puint32 numberOfMaterialParameters() const { return m_materialParameters.count(); }
    P_INLINE PMaterialParameter *materialParameter(puint32 index) const { return m_materialParameters[index]; }
    P_INLINE puint32 numberOfRenderParameters() const { return m_renderParameters.count(); }
    P_INLINE PAbstractRenderParameter *renderParameter(puint32 index) const { return m_renderParameters[index]; }
    
    const pchar *vertexShaderSource() const;
    const pchar *fragmentShaderSource() const;

protected:
    virtual void discardResource();
    virtual pbool restoreResource();

private:
    PMaterialResource(const pchar *id, const pchar *materialText, PResourceManager* resourceManager);
    virtual ~PMaterialResource();

    // Create material from the instance
    void createMaterial(const pchar *materialText);

private:
    PGlShader                          *m_shader;
    pbool                               m_isDefault;
    PGlVertexFormat                     m_vertexFormat;
    PArray<PAbstractRenderParameter *>  m_renderParameters;
    PArray<PMaterialParameter *>        m_materialParameters;
};


#endif // !PMATERIALRESOURCE_H
