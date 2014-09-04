// psprite.h
// A sprite is a quad which always faces the camera.
//
// Copyright 2012 - 2014 Future Interface. 
// This software is licensed under the terms of the MIT license.
//
// Hongwei Li lihw81@gmail.com
//

#ifndef PSPRITE_H
#define PSPRITE_H

#include <Paper3D/pdrawable.h>

#include <PFoundation/ppropertyint.h>

class PTexture;


class P_DLLEXPORT PSprite : public PDrawable
{
    P_OBJECT

private:
    PSprite(const PSprite &other) : PDrawable(P_NULL, (PScene *)P_NULL) {}
    void operator=(const PSprite &other) {}

public:
    PSprite(const pchar *name, PScene *scene);
    virtual ~PSprite();

    void setTextureAtlas(PTexture **textures, puint32 numTextures, 
        puint32 numRows, puint32 numCols, pbool transparency);
    // The number of frames the animation lasts.
    void setAnimationDuration(puint32 numFrames, pfloat32 milliseconds, puint32 startIndex = 0);

    virtual void calculateModelCameraMatrix(const PMatrix4x4 &cameraMatrix, 
        pfloat32 *out_matrix);
    
    virtual pbool unpack(const PXmlElement* xmlElement);

protected:
    virtual void update();

private:
    PTexture      *m_textureAtlas[4];
    puint32        m_numRows;
    puint32        m_numCols;
    pint32         m_numFrames;
    pfloat32       m_duration;
    puint32        m_startIndex;

    puint32   m_currentIndex; 
    puint32   m_previousIndex; 
    puint32   m_previousTextureId;
    pfloat32  m_deltaMilliseconds;
};




#endif // !PSPRITE_H
