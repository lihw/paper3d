// pskybox.h
// Skybox
//
// Copyright 2012 - 2014 Future Interface . 
// This software is licensed under the terms of the MIT license.
//
// Hongwei Li lihw81@gmail.com
//

#ifndef PSKYBOX_H
#define PSKYBOX_H

#include <Paper3D/pdrawable.h>

class PTexture;

class P_DLLEXPORT PSkyBox : public PDrawable
{
    P_OBJECT

private:
    PSkyBox(const PSkyBox &other) : PDrawable(P_NULL, (PScene *)P_NULL) {}
    void operator=(const PSkyBox &other) {}

public:
    PSkyBox(const pchar *name, PScene *scene);
    PSkyBox(const pchar *name, PNode *parent);
    virtual ~PSkyBox();

    // The texture must be a cube map texture.
    void setTexture(PTexture *texture);
    
    virtual pbool unpack(const PXmlElement* xmlElement);
    virtual void calculateModelCameraMatrix(const PMatrix4x4 &cameraMatrix, pfloat32 *out_matrix);

protected:
    virtual void update();
    
private:
    PTexture *m_texture;
};



#endif // !PSKYBOX_H
