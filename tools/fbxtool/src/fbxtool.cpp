// fbxtool.cpp
// FbxTool.
//
// Copyright 2012 - 2014 Future Interface. 
// This software is licensed under the terms of the MIT license.
//
// Hongwei Li lihw81@gmail.com
//

#include "fbxtool.h"

#include "mesh/meshconverter.h"
#include "scene/sceneconverter.h"

#include <common/log.h>
#include <common/filestream.h>

#include <windows.h>

FbxTool::FbxTool(const Arguments &arguments)
    : m_arguments(arguments)
{
    m_manager = NULL;
    m_scene   = NULL;
}

FbxTool::~FbxTool()
{
    // TODO: should I delete m_manager and m_scene here and how?
    if (m_scene != NULL)
    {
        m_scene->Destroy();
    }
}

bool FbxTool::load()
{
    // Get the file name from the specified FBX file path
    FbxString fileName = FbxPathUtils::GetFileName(m_arguments.FBXFileName.Buffer());
    FbxString filePath = FbxPathUtils::GetFolderName(m_arguments.FBXFileName.Buffer());

    // FIXME: FbxImporter::initialize() doesn't recognize relative and absolute path! We
    // need to enter the folder where fbx file is.
    char prevCurrentDirectory[MAX_PATH];
    GetCurrentDirectoryA(MAX_PATH, prevCurrentDirectory);

    SetCurrentDirectoryA(filePath.Buffer());

    m_manager = FbxManager::Create();
    if (m_manager == NULL)
    {
        logError("Error: Unable to create FBX Manager!\n");
        return false;
    }
    
    // Use default IO settings.    
    FbxIOSettings* ios = FbxIOSettings::Create(m_manager, IOSROOT);
    m_manager->SetIOSettings(ios);
    
    int lSDKMajor,  lSDKMinor,  lSDKRevision;
    // Get the file version number generate by the FBX SDK.
    FbxManager::GetFileFormatVersion(lSDKMajor, lSDKMinor, lSDKRevision);

    // Create an importer.
    FbxImporter *importer = FbxImporter::Create(m_manager, "");

    bool importStatus = importer->Initialize(fileName.Buffer(), -1, m_manager->GetIOSettings());
    int lFileMajor, lFileMinor, lFileRevision;
    importer->GetFileVersion(lFileMajor, lFileMinor, lFileRevision);
    if (!importStatus)
    {
        FbxString error = importer->GetStatus().GetErrorString();
        logError("Call to FbxImporter::Initialize() failed.");
        logError("FBX error returned: %s.", error.Buffer());
		logError("Check if %s exists.\n\n", m_arguments.FBXFileName.Buffer());

        if (importer->GetStatus().GetCode() == FbxStatus::eInvalidFileVersion)
        {
            logInfo("FBX file format version for this FBX SDK is %d.%d.%d.", 
                lSDKMajor, lSDKMinor, lSDKRevision);
            logInfo("FBX file format version for file '%s' is %d.%d.%d.", 
                fileName, lFileMajor, lFileMinor, lFileRevision);
        }

        return false;
    }

    logInfo("FBX file format version is %d.%d.%d.", 
        lSDKMajor, lSDKMinor, lSDKRevision);

    // Import the scene.
    m_scene = FbxScene::Create(m_manager, "myscene");;
    bool status = importer->Import(m_scene);

    // Delete the importer after we import the scene 
    importer->Destroy();

    // TODO: Current we don't need the password here.
    if (!status)
    {
        logError("Failed to import to a scene.");
        m_scene->Destroy();
        m_scene = NULL;
        return false;
    }

    SetCurrentDirectoryA(prevCurrentDirectory);

    return true;
}

bool FbxTool::convert()
{
    if (m_scene == NULL)
    {
        logError("The scene is invalid.\n");
        return false;
    }

    // FIXME: should change to FbxAxisSystem::OpenGL
    FbxAxisSystem axis(FbxAxisSystem::eMax);
    axis.ConvertScene(m_scene);

    FbxManager * pManager = FbxManager::Create();
    if (!pManager)
    {
        logError("Error: Unable to create FBX Manager!\n");
        return false;
    }
    else 
    {
        logInfo("Autodesk FBX SDK version %s\n", pManager->GetVersion());
    }

    FbxGeometryConverter lGeometryConverter(pManager);
    lGeometryConverter.Triangulate(m_scene, true);

    return createProject();
}
    
bool FbxTool::createProject()
{
    bool ret = true;

    logInfo("Parsing %s.", m_arguments.FBXFileName.Buffer());
    
    //
    // Create the folder structure
    //
    FbxString rootPath = FbxPathUtils::Bind(m_arguments.outputFolder, "res");
    rootPath = FbxPathUtils::Resolve(rootPath.Buffer());
    if (!FbxPathUtils::Create(rootPath.Buffer()))
    {
        logError("Failed to create project folder %s.", rootPath.Buffer());
        return false;
    }

    //
    // Output the mesh to the res directory
    //
    {
        logInfo("Exporting meshes in %s format:",
            m_arguments.meshFormat == "obj"? "obj" : "pmh");
        FbxString meshOutputPath = FbxPathUtils::Bind(rootPath.Buffer(), "mesh");
        if (FbxPathUtils::Create(meshOutputPath.Buffer()))
        {
			m_arguments.outputFolder = meshOutputPath;
			MeshConverter converter(m_scene, &m_arguments);
			converter.convert();
        }
		else
		{
			logError("Failed to create folder %s.", meshOutputPath.Buffer());
            return false;
		}
    }

    //
    // Output the scene to the res directory
    //
    {
        logInfo("Creating scene graph: scene.psc");
        m_arguments.outputFolder = rootPath;
        SceneConverter converter(m_scene, &m_arguments);
        converter.convert();

        //
        // Copy texture to the res directory
        //
        logInfo("Exporting textures:");
        FbxString textureOutputPath = FbxPathUtils::Bind(rootPath.Buffer(), "texture");
        if (!FbxPathUtils::Create(textureOutputPath.Buffer()))
        {
            logError("Failed to create folder %s.\n", textureOutputPath.Buffer());
            return false;
        }
        size_t numTextures = converter.m_textures.size();
        for (size_t i = 0; i < numTextures; ++i)
        {
            // We assume textures are in the same directory tree as the FBX file.
            FbxString texPath = converter.m_textures[i]; // texture path in FBX file
            texPath.ReplaceAll('/', '\\');

            FbxString actualPath;

            FbxString textureName = FbxPathUtils::GetFileName(texPath);
            FbxString fbxFilePath = FbxPathUtils::Resolve(m_arguments.FBXFileName.Buffer());
            FbxString fbxFileFolder = FbxPathUtils::GetFolderName(fbxFilePath.Buffer());

            int nTokens = fbxFileFolder.GetTokenCount("\\");
            for (int i = 0; i < nTokens; ++i)
            {
                FbxString dir = fbxFileFolder.GetToken(i, "\\");
                int found = texPath.Find(dir.Buffer());
                if (found != -1)
                {
                    actualPath = texPath.Mid(found, texPath.GetLen() - found);
                    break;
                }
            }

            // If we can't find the texture file whose path is specified by fbx, we assume it is
            // in the same folder as fbx file.
            if (!FbxFileUtils::Exist(actualPath.Buffer()))
            {
                actualPath = FbxPathUtils::Bind(fbxFileFolder, textureName);
            }

            if (!FbxFileUtils::Exist(actualPath.Buffer()))
            {
                logError("Failed to find texture %s. Please put it into %s.",
                    texPath.Buffer(), actualPath.Buffer());
                continue;
            }

            FbxString dstPath = FbxPathUtils::Bind(textureOutputPath.Buffer(), textureName.Lower().Buffer());

            // TODO: texture compression.

            // Convert to PNG file
            FbxString oldSuffix = FbxPathUtils::GetExtensionName(actualPath.Buffer());
            if (oldSuffix.Lower() == "jpg")
            {
                dstPath = FbxPathUtils::ChangeExtension(dstPath, ".png");

                char cmdline[1024];
                FBXSDK_snprintf(cmdline, 1024, "jpg2png.exe %s %s", actualPath.Buffer(), dstPath.Buffer());
                ret = (system(cmdline) == 0);
            }
            else
            {
                if (!FbxFileUtils::Copy(dstPath.Buffer(), actualPath.Buffer()))
                {
                    logError("Failed to find texture %s. Please manually copy it to %s.",
                        texPath.Buffer(), textureOutputPath.Buffer());
                    ret = false;
                }
            }

            if (ret)
            {
                logInfo(FbxPathUtils::ChangeExtension(textureName.Lower(), ".png").Buffer());
            }
        }
    }


    //
    // The default material (texture.pmt)
    //
    logInfo("Creating materials:");
    FbxString materialOutputPath = FbxPathUtils::Bind(rootPath.Buffer(), "material");
    if (!FbxPathUtils::Create(materialOutputPath.Buffer()))
    {
        logError("Failed to create folder %s.\n", materialOutputPath.Buffer());
        return false;
    }
    FbxString materialFilePath = FbxPathUtils::Bind(materialOutputPath, "texture.pmt");
    extern const char *MATERIAL_TEXT;
        
    FileStream output(materialFilePath.Buffer());
    if (!output.openToWrite()) 
    {
        logError("Failed to create file %s.", materialFilePath.Buffer());
        return false;
    }
    output.writeBytes((FbxUInt8*)MATERIAL_TEXT, strlen(MATERIAL_TEXT));
    output.close();

    if (ret)
    {
        logInfo("texture.pmt");
    }

    return ret;
}


