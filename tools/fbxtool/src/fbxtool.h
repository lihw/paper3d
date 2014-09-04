// fbxtool.h
// The FBX tool
//
// Copyright 2012 - 2014 Future Interface. 
// This software is licensed under the terms of the MIT license.
//
// Hongwei Li lihw81@gmail.com
//

#ifndef FBXTOOL_H
#define FBXTOOL_H

#include <fbxsdk.h>

enum ExportEnum
{
    EXPORTING_MESH      = 0x01,
    EXPORTING_ANIMATION = 0x02,
    EXPORTING_IMAGE     = 0x04,
    EXPORTING_MATERIAL  = 0x08,
    EXPORTING_SCENE     = 0x10,
};


struct Arguments
{
    FbxUInt   exporting;
    FbxString outputFolder;
    FbxString compressionTextureFormat;
    FbxString meshFormat;
    FbxString FBXFileName;
    FbxString vertexAttributes;
    FbxString projectName;

    Arguments()
        : outputFolder(".")
        , compressionTextureFormat("")
        , FBXFileName("")
        , meshFormat("pmh")
        , vertexAttributes("pnu")
    {
        exporting = 0;
    }
};

class FbxTool
{
public:
    FbxTool(const Arguments &arguments);
    ~FbxTool();

    bool load();
    bool convert();

private:
    bool createProject();

private:
    Arguments    m_arguments;
    FbxManager  *m_manager;
    FbxScene    *m_scene;
};


#endif // !FBXTOOL_H
