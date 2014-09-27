// pmain.cpp
// "Background" example. 
//
// Copyright 2012 - 2014 Future Interface. 
// This software is licensed under the terms of the MIT license.
//
// Hongwei Li lihw81@gmail.com
//

#include "mycontext.h"

#include <Paper3D/paper3d.h>

#include <PFoundation/pfoundation.h>


void pMain(int argc, char* argv[])
{
    PASSERT(PActivity::s_activity != P_NULL);

    if (PActivity::s_activity != P_NULL)
    {
        PContextProperties contextProperties;
        contextProperties.m_contextName = PString("background");
        contextProperties.m_archiveName = PString("background.par");
        contextProperties.m_windowWidth = 640;
        contextProperties.m_windowHeight = 480;
#if defined P_WIN32
        contextProperties.m_multisamples = 2;
#endif

        PContext* context = PNEW(MyContext(contextProperties));
		context->addModule(PNEW(PResourceManager(context)));
		context->addModule(PNEW(PSceneManager(context)));
		// More modules
        PActivity::s_activity->addContext(context);
    }
    else
    {
        PLOG_ERROR("No running activity");
    }
}
