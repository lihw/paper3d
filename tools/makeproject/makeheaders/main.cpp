// main.cpp
// Convert files into CPP headers
//
// Copyright 2012 - 2014 Future Interface. 
// This software is licensed under the terms of the MIT license.
//
// Hongwei Li lihw81@gmail.com
//

#include <stdio.h>
#include <stdlib.h>
#include <string>

#include <windows.h>
#include <direct.h>

#include <common/version.h>
#include <common/log.h>

#include "makeheaders.h"

static void printUsage()
{
    logInfo("Usage:\n");
    logInfo("makeheaders.exe [-h] [-v] [-o <output-folder>] <folder>\n"
            "\n"
            "General:\n"
            "\t-h                    Print the usage.\n"
            "\t-v                    Print the version information.\n"
            "\t-d                    Create an default project.\n"
            "\t-o <output-folder>    The output folder.\n"
            "\t<folder>              The input files.\n"
            "\n"
            );
}

static void printVersion()
{
    logInfo("makeheaders version %s. Future Interface 2012 - 2014.\n\n", FI_VERSION_STR);
}


int main(int argc, const char* argv[])
{
    //
    // Parse arguments
    //
    std::string outputFolder("output");
    std::string inputFolder;

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
        else if (strncmp(argv[i], "-o", 2) == 0)
        {
            i++;
            if (i == argc)
            {
                logError("The output folder is missing!\n");
                printUsage();
                return EXIT_FAILURE;
            }
            outputFolder = std::string(argv[i]);
        }
        else 
        {
            inputFolder = std::string(argv[i]);
            if (i != argc - 1)
            {
                logError("Not support more than one input file folder!\n");
                printUsage();
                return EXIT_FAILURE;
            }
        }
    }

    if (inputFolder == "")
    {
        logError("Invalid input path!\n");
        printUsage();
        return EXIT_FAILURE;
    }

    //
    // Create directory of output
    //
    if (!CreateDirectoryA(outputFolder.c_str(), NULL))
    {
        DWORD err = GetLastError();
        if (err == ERROR_PATH_NOT_FOUND)
        {
            logError("Some intermediate directories in the path (%s) are missing.\n", outputFolder.c_str());
            return EXIT_FAILURE;
        }
    }

    if (!makeHeaders(inputFolder.c_str(), outputFolder.c_str()))
    {
        return EXIT_FAILURE;
    }

    // If debugger is present, a pause is required to keep the console output
    // visible. Otherwise the pause is automatic. 
    if (IsDebuggerPresent())
    {
        system("pause");
    }

    return EXIT_SUCCESS;
}
