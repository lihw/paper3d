// pwaterwave.h
// Shader based water wave effects with reflection and refraction.
//
// Copyright 2012 - 2014 Future Interface . 
// This software is licensed under the terms of the MIT license.
//
// Hongwei Li lihw81@gmail.com
//

#ifndef PWATERWAVE_H
#define PWATERWAVE_H

#include <Paper3D/pdrawable.h>

#include <PFoundation/ppropertyfloat.h>
#include <PFoundation/ppropertyvector4.h>


class PCamera;
class PFrameBuffer;
class PTexture;
class PRenderPass;


class P_DLLEXPORT PWaterWave : public PDrawable
{
    P_OBJECT

private:
    PWaterWave(const PWaterWave &other) : PDrawable(P_NULL, (PScene *)P_NULL) {}
    void operator=(const PWaterWave &other) {}

public:
    PWaterWave(const pchar *name, PNode *parent, pbool reflection = false, pbool refraction = false);
    PWaterWave(const pchar *name, PScene *scene, pbool reflection = false, pbool refraction = false);
    ~PWaterWave();

    void setWaveScaling(const PVector4 &scaling);
    void setWaveVelocity(const PVector4 &velocity);
    void setReflectionDistortion(pfloat32 distortion);
    void setNormalTexture(PTexture *texture);
    void setColorTexture(PTexture *texture);
    // FIXME: it doesn't change the transform of the water geometry for the maximum flexibity (water plane
    // is not the same as the reflection plane) 
    void setWaterPlane(const PVector4 &plane);

    P_INLINE pbool reflection() const { return m_reflection; }
    P_INLINE pfloat32 reflectionDistortion() const { return m_reflectionDistortion.toFloat(); }
    P_INLINE PVector4 waveScaling() const { return m_waveScaling.toVector4(); }
    P_INLINE PTexture *colorTexture () const { return m_colorTexture; }
    P_INLINE PTexture *normalTexture () const { return m_normalTexture; }
    
    virtual pbool unpack(const PXmlElement* xmlElement);

protected:
    virtual void prepareRender(PRenderState *renderState);
    virtual void update();

private:
    void initialize(pbool reflection, pbool refraction);

private:
    pbool           m_reflection;
    pbool           m_refraction;
    PCamera        *m_reflectionCamera;
    PCamera        *m_refractionCamera;
    PFrameBuffer   *m_reflectionFB;
    PFrameBuffer   *m_refractionFB;
    PRenderPass    *m_reflectionPass;
    PRenderPass    *m_refractionPass;
    PPlane         *m_plane;

    // textures.
    PTexture       *m_colorTexture;
    PTexture       *m_normalTexture;

    // wave and distortion.
    PPropertyVector4        m_waveScaling;
    PPropertyVector4        m_waveVelocity;
    PPropertyFloat          m_reflectionDistortion;
    PVector4                m_waveTranslation;
};



#endif  // !PWATERWAVE_H

