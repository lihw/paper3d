// ppointlight.h
// The point light.
//
// Copyright 2012 - 2014 Future Interface. 
// This software is licensed under the terms of the MIT license.
//
// Hongwei Li lihw81@gmail.com and Hongwei Li lihw81@gmail.com
//

#ifndef PPOINTLIGHT_H
#define PPOINTLIGHT_H

#include <Paper3D/pabstractlight.h>

class P_DLLEXPORT PPointLight : public PAbstractLight
{
    P_OBJECT
    
    PPointLight(const PPointLight &other) : PAbstractLight(P_LIGHT_NONE, P_NULL, (PScene *)P_NULL) {}
    void operator=(const PPointLight &other) {}

public:
    PPointLight(const pchar *name, PScene *scene);
    PPointLight(const pchar *name, PNode *parent);
    virtual ~PPointLight();
    
    void setRange(pfloat32 distance);

    P_INLINE pfloat32 range() const { return m_range.toFloat(); }

protected:
    PPropertyFloat   m_range;
};


#endif // !PPOINTLIGHT_H

