// main.cpp
// FbxTool.
//
// Copyright 2012 - 2014 Future Interface. 
// This software is licensed under the terms of the MIT license.
//
// Hongwei Li lihw81@gmail.com
//

#include <common/log.h>
#include <common/version.h>

#include "fbxtool.h"

#include <stdio.h>
#include <stdlib.h>

#include <windows.h>
#include <direct.h>


// fbxconverter.exe -mf pmh -mv pnu -f mushroom mushroom.fbx
// fbxconverter.exe -s -f island islandAllFBX_Yup_pnutb.fbx
// fbxconverter.exe -mv pnu -pn island islandAllFBX_Yup_pnutb.fbx

static void printUsage()
{
    logInfo("Usage:\n");
    logInfo("FBXConverter.exe [-texformat <format>] [-meshformat <format>] [-meshattrib <format>] [-help] [-version] <fbx-file>\n"
             "\n"
             "General:\n"
             "\t<fbx-file>                  The input FBX file.\n"
             "\t-help                       Print this usage.\n"
             "\t-version                    The version and copyright information.\n"
             "\n"
             "Mesh:\n"
             "\t-meshformat <format>        Output mesh format, including .obj and .pmh. Default is pmh. \n"
             "\t-meshattrib <p|t|b|n|u|c|a> Which attributes will be exported, p(position), t(tangent), b(binormal),\n"
             "\t                            n(normal), u(uv), c(color) and a(everything).\n"
             "Texture:\n"
             "\t-texformat <format>         Compress the texture in certain format. \n"
             "\t                            Only ETC and ETC2 are supported right now. \n"
             "\n"
             "Examples:\n"
             "FBXConverter.exe sample.fbx           - Create an \"empty\" project with everything from fbx.\n"
             "FBXConverter.exe -texformat ETC2 sample.fbx  - Create a project and textures are in ETC2 format.\n\n"
             );
}

static void printVersion()
{
    logInfo("FbxTool version %s. Future Interface 2012 - 2014.\n", FI_VERSION_STR);
}


int main(int argc, const char* argv[])
{
    Arguments arguments;

    //
    // Parse the arguments
    //
    for (int i = 1; i < argc; ++i)
    {
        if (strcmp(argv[i], "-texformat") == 0)
        {
            i++;
            if (i == argc)
            {
                logInfo("The compression texture format is missing!\n");
                printUsage();
                return EXIT_FAILURE;
            }
            arguments.compressionTextureFormat = FbxString(argv[i]);
        }
        else if (strcmp(argv[i], "-meshformat") == 0)
        {
            i++;
            if (i == argc)
            {
                logInfo("The output mesh format is missing!\n");
                printUsage();
                return EXIT_FAILURE;
            }
            arguments.meshFormat = FbxString(argv[i]);
        }
        else if (strcmp(argv[i], "-meshattrib") == 0)
        {
            i++;
            if (i == argc)
            {
                logInfo("The output vertex attribute is missing!\n");
                printUsage();
                return EXIT_FAILURE;
            }
            arguments.vertexAttributes = FbxString(argv[i]);
        }
        else if (strcmp(argv[i], "-help") == 0)
        {
            printUsage();
            return EXIT_SUCCESS;
        }
        else if (strcmp(argv[i], "-version") == 0)
        {
            printVersion();
            return EXIT_SUCCESS;
        }
        else 
        {
            arguments.FBXFileName = FbxString(argv[i]);
            if (i != argc - 1)
            {
                logInfo("FBXConverter.exe can't support more than one FBX file!\n\n");
                return EXIT_FAILURE;
            }
        }
    }

    //
    // Validate arguments
    //
    if (arguments.FBXFileName == "")
    {
        printUsage();
        return EXIT_FAILURE;
    }

    FbxString fileName = FbxPathUtils::GetFileName(arguments.FBXFileName.Buffer());
    int suffix = fileName.ReverseFind('.');
    arguments.projectName = fileName.Left(suffix);

    //
    // Create output folder if it doesn't exit.
    //
    const char* outputFolder = arguments.outputFolder.Buffer();
    if (arguments.outputFolder != "." && !CreateDirectoryA(outputFolder, NULL))
    {
        DWORD err = GetLastError();
        if (err == ERROR_PATH_NOT_FOUND)
        {
            logError("Some intermediate directories in the path (%s) are missing.", outputFolder);
            return EXIT_FAILURE;  
        }
    }

    //
    // Import the FBX file and export the converted data.
    //
    FbxTool tool(arguments);
    if (!tool.load())
    {
        return EXIT_FAILURE;
    }

    if (!tool.convert())
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
