// peffectglow.h
// Full screen glow effect
//
// Copyright 2012 - 2014 Future Interface . 
// This software is licensed under the terms of the MIT license.
//
// Hongwei Li lihw81@gmail.com
//

#ifndef PEFFECTGLOW_H
#define PEFFECTGLOW_H

#include <Paper3D/pabstracteffect.h>

class P_DLLEXPORT PEffectGlow : public PAbstractEffect
{
public:
    PEffectGlow(PScene *scene);
    virtual ~PEffectGlow();

    void setBlurIterations(pint32 number);
    void setBlurKernelSize(pint32 size);
    void setBlurDistance(pint32 step);

    P_INLINE pint32 blurIterations() const { return m_blurIterations; }
    P_INLINE pint32 blurKernelSize() const { return m_blurKernelSize; }
    P_INLINE pint32 blurDistance() const { return m_blurDistance; }

    virtual void unpack(PXmlElement *xmlElement);

protected:
    virtual void renderEffect(PRenderState *renderState);

private:
    pint32 m_blurIterations;
    pint32 m_blurKernelSize;
    pint32 m_blurDistance;

    PMaterial *m_xMaterial; // blur in x direction
    PMaterial *m_yMaterial; // blur in y direction
};


#endif // !PEFFECTGLOW_H
