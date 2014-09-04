// pmain.cpp
// "Network" example
//
// Copyright 2012 - 2014 Future Interface. All rights reserved.
//
// Future Interface  lihw81@gmail.com
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
        contextProperties.m_contextName = PString("network");
        contextProperties.m_archiveName = PString("network.par");
#if defined P_WIN32
        contextProperties.m_windowWidth = 480;
        contextProperties.m_windowHeight = 854;
        contextProperties.m_multisamples = 2;
        contextProperties.m_maxFps = 15;
#elif defined P_ANDROID
	contextProperties.m_windowWidth = 0xffffffff;
        contextProperties.m_windowHeight = 0xffffffff;
#endif

        PContext *context = PNEW(MyContext(contextProperties));
        context->addModule(PNEW(PResourceManager(context)));
        context->addModule(PNEW(PSceneManager(context)));
#if P_ENABLE_NETWORK == 1
        context->addModule(PNEW(PNetworkManager(context)));
#endif
        // TODO: initialize more modules here.

        PActivity::s_activity->addContext(context);
    }
    else
    {
        PLOG_ERROR("No running activity.");
    }
}
