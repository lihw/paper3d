// pabstractlight.h
// The abstract class of all lights.
//
// Copyright 2012 - 2014 Future Interface. 
// This software is licensed under the terms of the MIT license.
//
// Hongwei Li lihw81@gmail.com and Hongwei Li lihw81@gmail.com
//

#ifndef PABSTRACTLIGHT_H
#define PABSTRACTLIGHT_H

#include <Paper3D/pnode.h>

#include <PFoundation/ppropertycolor.h>
#include <PFoundation/ppropertyfloat.h>
#include <PFoundation/ppropertybool.h>
#include <PFoundation/pcolorrgba.h>

class PLightManager;

enum PLightTypeEnum
{
    P_LIGHT_NONE = 0,
    P_LIGHT_POINT = 1,
    P_LIGHT_DIRECTIONAL = 2,
    P_LIGHT_SPOT = 3,

    P_LIGHT_TYPE_FIRST = P_LIGHT_NONE,
    P_LIGHT_TYPE_LAST = P_LIGHT_SPOT,
    P_LIGHT_TYPE_NUMBER = P_LIGHT_TYPE_LAST - P_LIGHT_TYPE_FIRST + 1,
};

class P_DLLEXPORT PAbstractLight : public PNode
{
    P_OBJECT

    PAbstractLight(const PAbstractLight &other) : PNode(P_NULL, P_NULL) {} 
    void operator=(const PAbstractLight &other) {} 
    
protected:
    PAbstractLight(PLightTypeEnum lightType, const pchar* name, PScene *scene);
    PAbstractLight(PLightTypeEnum lightType, const pchar* name, PNode *parent);
    virtual ~PAbstractLight();

public:    
    P_INLINE PLightTypeEnum type() const { return m_type; }

    P_INLINE pbool isEnabled() const     { return m_enabled.toBool(); }
    P_INLINE PColorRGBA color() const    { return m_color.toColor(); }
    P_INLINE pfloat32 intensity() const  { return m_intensity.toFloat(); }

    PVector3 worldPosition() const;
    
    void setEnabled(pbool enabled);
    void setColor(const PColorRGBA &color);
    void setIntensity(const pfloat32 &intensity);

protected:
    virtual void updateWorldTransform();
    
protected:
    PLightTypeEnum    m_type;
    PPropertyBool     m_enabled;
    PPropertyFloat    m_intensity;
    PPropertyColor    m_color;
};


#endif // !PABSTRACTLIGHT_H
