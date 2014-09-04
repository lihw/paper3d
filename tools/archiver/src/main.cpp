// main.cpp
// Archiver
//
// Copyright 2012 - 2014 Future Interface. 
// This software is licensed under the terms of the MIT license.
//
// Hongwei Li lihw81@gmail.com
//

#include <stdio.h>
#include <stdlib.h>

#include <windows.h>

#include <common/version.h>
#include <common/log.h>

#include "archiver.h"

static void printUsage()
{
    // FIXME: version and help
    logInfo("Usage:\n");
    logInfo("Archiver.exe [-o <output-file>] <path>\n"
            "\n"
            "General:\n"
            "\t-h                   Print the usage.\n"
            "\t-v                   Print the version information.\n"
            "\t-o [output-file]     Output archive file. If missing, it is output.par.\n"
            "\t[path]               The folder to archive."
            "\n"
            "\n"
            "Examples:\n"
            "Arhicver.exe ../..              - Archive the ../.. directory into output.par. \n"
            "Archiver.exe -f noname.par ..   - Archive the ../ directory into noname.par\n\n"
           );
}

static void printVersion()
{
    logInfo(
        "archiver version %s. Future Interface 2012 - 2014.\n\n", 
        FI_VERSION_STR);
}


int main(int argc, const char* argv[])
{
    Arguments arguments;

    //
    // Parse the arguments
    //
    for (int i = 1; i < argc; ++i)
    {
        if (strcmp(argv[i], "-o") == 0)
        {
            i++;
            if (i == argc)
            {
                logError("The output folder is missing.");
                printUsage();
                return EXIT_FAILURE;
            }
            arguments.outputFile = std::string(argv[i]);
        }
        else if (strncmp(argv[i], "-v", 2) == 0)
        {
            printVersion();
            return EXIT_SUCCESS;
        }
        else if (strncmp(argv[i], "-h", 2) == 0)
        {
            printUsage();
            return EXIT_SUCCESS;
        }
        else 
        {
            arguments.inputPath = std::string(argv[i]);
            if (i != argc - 1)
            {
                logError("Archiver.exe can't archive more than one path!");
                return EXIT_FAILURE;
            }
        }
    }

    if (arguments.inputPath == "" || arguments.outputFile == "")
    {
        logError("Invalid arguments!", arguments.inputPath.c_str());
        printUsage();
        return EXIT_FAILURE;
    }

    //
    // Check if the input path exists
    //
    DWORD dwAttrib = GetFileAttributesA(arguments.inputPath.c_str());
    bool exists = ((dwAttrib != INVALID_FILE_ATTRIBUTES) && (dwAttrib & FILE_ATTRIBUTE_DIRECTORY));
    if (!exists)
    {
        logError("The %s doesn't exists or is not a directory.");
        return EXIT_FAILURE;
    }

    //
    // Archive the folder
    //
    Archiver archiver(arguments);
    if (!archiver.archive())
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
