// main.cpp
// Make project
//
// Copyright 2012 - 2014 Future Interface. 
// This software is licensed under the terms of the MIT license.
//
// Hongwei Li lihw81@gmail.com
//

#include <stdio.h>
#include <stdlib.h>

#include <windows.h>
#include <direct.h>

#include <common/filestream.h>
#include <common/version.h>

#include "makeproject.h"

static void printUsage()
{
    logInfo("Usage:\n");
    logInfo("makeproject.exe [-f <makefile>]\n"
            "\n"
            "General:\n"
            "\t-h                 Print the usage.\n"
            "\t-v                 Print the version information.\n"
			"\t[-f <makefile>]    An ini file which stores the project parameters. project.mak will be used when not specified.\n"
            "\n"
            );
}

static void printVersion()
{
    logInfo("makeproject version %s. Future Interface 2012 - 2014. \n\n", FI_VERSION_STR);
}

int main(int argc, const char* argv[])
{
    //
    // Parse arguments
    //
    Arguments arguments;
    std::string makeFile("project.pmk");

    for (int i = 1; i < argc; ++i)
    {
        if (strncmp(argv[i], "-h", 2) == 0)
        {
            printUsage();
            return EXIT_SUCCESS;
        }
        else if (strncmp(argv[i], "-v", 2) == 0)
        {
            printVersion();
            return EXIT_SUCCESS;
        }
        else if (strncmp(argv[i], "-f", 2) == 0)
        {
            i++;
            if (i == argc)
            {
                logError("The project make file is missing!\n");
                printUsage();
                return EXIT_FAILURE;
            }
            makeFile = std::string(argv[i]);
        }
        else
        {
            logError("Unknown command line arguments.");
			printUsage();
			return EXIT_FAILURE;
        }
    }

    // Parse the configuration.
    if (!arguments.parse(makeFile.c_str()))
    {
        return EXIT_FAILURE;
    }
            
    //
    // Create the project
    //
    MakeProject makeProject(arguments);
    if (!makeProject.run())
    {
        return EXIT_FAILURE;
    }

    logInfo("Project %s has been created!", arguments.projectName.c_str());
    
    // If debugger is present, a pause is required to keep the console output
    // visible. Otherwise the pause is automatic. 
    if (IsDebuggerPresent())
    {
        system("pause");
    }

    return EXIT_SUCCESS;
}
