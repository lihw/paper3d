// pspotlight.h
// The spot light.
//
// Copyright 2012 - 2014 Future Interface. 
// This software is licensed under the terms of the MIT license.
//
// Hongwei Li lihw81@gmail.com and Hongwei Li lihw81@gmail.com
//

#ifndef PSPOTLIGHT_H
#define PSPOTLIGHT_H

#include <Paper3D/pdirectionallight.h>

class P_DLLEXPORT PSpotLight : public PDirectionalLight
{
    P_OBJECT

    PSpotLight(const PSpotLight &other) : PDirectionalLight("", (PScene *)P_NULL) {}
    void operator=(const PSpotLight &other) {}

public:
    PSpotLight(const pchar *name, PScene *scene);
    PSpotLight(const pchar *name, PNode *parent);
    virtual ~PSpotLight();

    void setCutoff(pfloat32 cutoff);
    void setExponent(pfloat32 exponent);
    
    P_INLINE pfloat32 cutoff() const    { return m_cutoff.toFloat(); }
    P_INLINE pfloat32 cosCutoff() const { return m_cosCutOff; }
    P_INLINE pfloat32 exponent() const  { return m_exponent.toFloat(); }
    
private:
    PPropertyFloat      m_cutoff;
    PPropertyFloat      m_exponent;
    pfloat32            m_cosCutOff;
};


#endif // !PSPOTLIGHT_H

