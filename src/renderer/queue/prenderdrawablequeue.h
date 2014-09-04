// prenderdrawablequeue.h
// Render queue to host a number of drawables
//
// Copyright 2012 - 2014 Future Interface . 
// This software is licensed under the terms of the MIT license.
//
// Hongwei Li lihw81@gmail.com
//

#ifndef PRENDERDRAWABLEQUEUE_H
#define PRENDERDRAWABLEQUEUE_H

#include <PFoundation/parray.h>

class PDrawable;
class PCamera;


class P_DLLEXPORT PRenderDrawableQueue
{
public:
    PRenderDrawableQueue(pbool transparent);
    ~PRenderDrawableQueue();

    void addDrawable(PDrawable *drawable);
    void removeDrawable(PDrawable *drawable);
    void sort(PCamera *camera);
    puint32 count() const;
    PDrawable *operator[](puint32 i);

private:
    PArray<PDrawable *> m_drawables;
    pbool               m_transparent;
};


#endif // !PRENDERDRAWABLEQUEUE_H
