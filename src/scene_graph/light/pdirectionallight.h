// pdirectionallight.h
// The directional light.
//
// Copyright 2012 - 2014 Future Interface. 
// This software is licensed under the terms of the MIT license.
//
// Hongwei Li lihw81@gmail.com and Hongwei Li lihw81@gmail.com
//

#ifndef PDIRECTIONALLIGHT_H
#define PDIRECTIONALLIGHT_H

#include <Paper3D/pabstractlight.h>

#include <PFoundation/ppropertyvector3.h>


class P_DLLEXPORT PDirectionalLight : public PAbstractLight
{
    P_OBJECT
    
    PDirectionalLight(const PDirectionalLight &other) : PAbstractLight(P_LIGHT_NONE, "", (PScene *)P_NULL) {}
    void operator=(const PDirectionalLight &other) {}

public:    
    PDirectionalLight(const pchar *name, PScene *scene);
    PDirectionalLight(const pchar *name, PNode *parent);
    virtual ~PDirectionalLight();
    
    void setDirection(pfloat32 x, pfloat32 y, pfloat32 z);
    void setDirection(const PVector3 &direction);
    void setCastShadow(pbool castShadow);

    P_INLINE PVector3 direction() const { return m_direction.toVector3(); }
    P_INLINE pbool castShadow() const   { return m_castShadow.toBool(); }

    PVector3 worldDirection() const;

protected:
    PPropertyBool       m_castShadow;
    PPropertyVector3    m_direction;
};


#endif // !PDIRECTIONALLIGHT_H
