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
#include <Windows.h>
#include <DbgHelp.h>


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
    vsprintf_s(text, 4096, fmt, arguments);
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
    vsprintf_s(text, 4096, fmt, arguments);
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
    vsprintf_s(text, 4096, fmt, arguments);
    printf("(Error): %s\n", text);
}

void logFatal(const char *fmt, ...)
{
    if (s_verbose)
    {
        va_list arguments;
        va_start(arguments, fmt);

        char text[4096];
        vsprintf_s(text, 4096, fmt, arguments);
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
    vsprintf_s(text, 4096, fmt, arguments);
    OutputDebugStringA(text);
    OutputDebugStringA("\n");
}

void logSetVerbose(bool verbose)
{
    s_verbose = verbose;
}
