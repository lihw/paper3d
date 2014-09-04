// main.cpp 
// The main entry of obj2pmh
//
// Copyright 2012 - 2014 Future Interface. 
// This software is licensed under the terms of the MIT license.
//
// Hongwei Li lihw81@gmail.com
//

#include <common/log.h>
#include <common/version.h>

#include <stdlib.h>
#include <stdio.h>

#include "obj2pmh.h"

static void printUsage(void)
{
    logInfo(
        "obj2pmh.exe [-verbose] [-help|-h] [-version|-v] <input file> [output file]\n\n"
        "parameters:\n"
        "\t-verbose     - print verbose message.\n"
        "\t-help        - print usage.\n"
        "\t-version     - print the version of this application.\n\n"
    );
}

static void printVersion(void)
{
    logInfo(
        "obj2pmh version %s. Future Interface 2012 - 2014.\n\n", 
    FI_VERSION_STR);
}
    
int main(int argc, char* argv[])
{
    if (argc < 3)
    {
        printUsage();
        return EXIT_FAILURE;
    }

    bool verbose = false;
    const char *inputFile = NULL;
    const char *outputFile = NULL;

    for (int i = 1; i < argc; ++i)
    {
        if (strcmp(argv[i], "-verbose") == 0)
        {
            verbose = true;
        }
        else if (strcmp(argv[i], "-help") == 0 ||
                 strcmp(argv[i], "-h") == 0)
        {
            printUsage();
            return EXIT_SUCCESS;
        }
        else if (strcmp(argv[i], "-version") == 0 ||
                 strcmp(argv[i], "-v") == 0)
        {
            printVersion();
            return EXIT_SUCCESS;
        }
        else
        {
            if (inputFile != NULL)
            {
                outputFile = argv[i];
            }
            else
            {
                inputFile = argv[i];
            }
        }
    }

    // Sanity check
    if (inputFile == NULL)
    {
        logInfo("input file is missing.");
        return EXIT_FAILURE;
    }
    if (outputFile == NULL)
    {
        logInfo("output file is missing.");
        return EXIT_FAILURE;
    }

    logSetVerbose(verbose);

    Obj2Pmh converter(inputFile, outputFile);
    if (converter.execute())
    {
        return EXIT_SUCCESS;
    }

    return EXIT_FAILURE;
}

