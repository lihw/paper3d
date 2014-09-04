// prenderlightqueue.h
// The queue of lights to be used in the render queue.
//
// Copyright 2012 - 2014 Future Interface . 
// This software is licensed under the terms of the MIT license.
//
// Hongwei Li lihw81@gmail.com
//

#ifndef PRENDERLIGHTQUEUE_H
#define PRENDERLIGHTQUEUE_H

#include <Paper3D/pabstractlight.h>

#include <PFoundation/passert.h>

class PAbstractLight;


class PRenderLightQueue
{
public:
    PRenderLightQueue();
    ~PRenderLightQueue();

    void addLight(PAbstractLight *light);
    void removeLight(PAbstractLight *light);
    void clear();

    PAbstractLight *light(PLightTypeEnum type, puint32 index) const;
    
private:
    PAbstractLight    *m_lights[4];
};


#endif // !PRENDERLIGHTQUEUE_H

