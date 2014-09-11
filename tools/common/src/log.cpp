// log.cpp
// The logging wrappers
//
// Copyright 2012 - 2014 Future Interface. 
// This software is licensed under the terms of the MIT license.
//
// Hongwei Li lihw81@gmail.com
//

#include "log.h"

#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>
#if defined WIN32
# include <Windows.h>
# include <DbgHelp.h>
#elif defined __APPLE__
#endif


static bool s_verbose = true;

void logInfo(const char *fmt, ...)
{
    if (!s_verbose)
    {
        return ;
    }
    
    va_list arguments;
    va_start(arguments, fmt);

    char text[4096];
#if defined WIN32
    vsprintf_s(text, 4096, fmt, arguments);
#elif defined __APPLE__
    vsnprintf(text, 4096, fmt, arguments);
#endif
    printf("%s\n", text);
}

void logWarning(const char *fmt, ...)
{
    if (!s_verbose)
    {
        return ;
    }
    
    va_list arguments;
    va_start(arguments, fmt);

    char text[4096];
#if defined WIN32
    vsprintf_s(text, 4096, fmt, arguments);
#elif defined __APPLE__
    vsnprintf(text, 4096, fmt, arguments);
#endif

    printf("(Warning): %s\n", text);
}

void logError(const char *fmt, ...)
{
    if (!s_verbose)
    {
        return ;
    }
    
    va_list arguments;
    va_start(arguments, fmt);

    char text[4096];
#if defined WIN32
    vsprintf_s(text, 4096, fmt, arguments);
#elif defined __APPLE__
    vsnprintf(text, 4096, fmt, arguments);
#endif

    printf("(Error): %s\n", text);
}

void logFatal(const char *fmt, ...)
{
    if (s_verbose)
    {
        va_list arguments;
        va_start(arguments, fmt);

        char text[4096];
#if defined WIN32
        vsprintf_s(text, 4096, fmt, arguments);
#elif defined __APPLE__
        vsnprintf(text, 4096, fmt, arguments);
#endif

        printf("(Fatal): %s\n", text);
    }

    exit(EXIT_FAILURE);
}

void logDebug(const char *fmt, ...)
{
    if (!s_verbose)
    {
        return ;
    }

    va_list arguments;
    va_start(arguments, fmt);

    char text[4096];
#if defined WIN32
    vsprintf_s(text, 4096, fmt, arguments);
#elif defined __APPLE__
    vsnprintf(text, 4096, fmt, arguments);
#endif

#if defined WIN32
    OutputDebugStringA(text);
    OutputDebugStringA("\n");
#elif defined __APPLE__
    extern void debugOutput(const char *);
    debugOutput(text);
#endif
}

void logSetVerbose(bool verbose)
{
    s_verbose = verbose;
}
