// pmirror.h
// "Reflection" example
//
// Copyright 2012 - 2014 Future Interface. 
// This software is licensed under the terms of the MIT license.
//
// Hongwei Li lihw81@gmail.com
//

#ifndef PMIRROR_H
#define PMIRROR_H


#include <Paper3D/pdrawable.h>

class PCamera;
class PFrameBuffer;
class PRenderPass;


class P_DLLEXPORT PMirror : public PDrawable
{
    P_OBJECT

private:
    PMirror(const PMirror &other) : PDrawable(P_NULL, (PScene *)P_NULL) {}
    void operator=(const PMirror &other) {}

public:
    PMirror(const pchar *name, PNode *parent);
    PMirror(const pchar *name, PScene *scene);
    ~PMirror();

    virtual void prepareRender(PRenderState *renderState);

private:
    void initialize();

private:
    PCamera        *m_reflectionCamera;
	PRenderPass    *m_reflectionPass;
    PFrameBuffer   *m_reflectionFB;

};



#endif  // !PMIRROR_H
