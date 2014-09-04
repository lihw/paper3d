// pabstractgeometry.h 
// The base geometry class
//
// Copyright 2012 - 2014 Future Interface . 
// This software is licensed under the terms of the MIT license.
//
// Hongwei Li lihw81@gmail.com
//
// 

#ifndef PABSTRACTGEOMETRY_H
#define PABSTRACTGEOMETRY_H

#include <PFoundation/pentity.h>
#include <PFoundation/pbox.h>
#include <PFoundation/pglvertexbuffer.h>
#include <PFoundation/pglstate.h>


class PMesh;
class PRenderState;
class PResourceManager;
class PXmlElement;


class PAbstractGeometry : public PEntity
{
    PAbstractGeometry(const PAbstractGeometry &other) {}
    void operator=(const PAbstractGeometry &other) {}

protected:
    PAbstractGeometry();
    virtual ~PAbstractGeometry();

public:
    P_INLINE PMesh *mesh() const { return m_mesh; }
    P_INLINE pint32 cullMode() const { return m_cullMode; }
    P_INLINE bool isValid() const { return m_mesh != P_NULL; }

    static PAbstractGeometry *unpack(PXmlElement *element, PResourceManager *resourceManager);
    
    // Render this geometry.
    void render(PRenderState *renderState);
    // Calibrate the mesh's vertex format with bound material's one.
    void calibrate(PGlVertexFormat *vertexFormat);
    // Set the face culling mode, P_GLCULL_FRONT, P_GLCULL_BACK,
    // P_GLCULL_NONE, or -1 (use global cull mode).
    void setCullMode(pint32 mode);

protected:
    PMesh            *m_mesh;
    PGlVertexFormat   m_vertexFormat;
    pbool             m_calibrated;
    pint32            m_cullMode;
};

#endif // !PABSTRACTGEOMETRY_H

