// pconfig.cpp
// The global configuration 
//
// Copyright 2012 - 2014 Future Interface. 
// This software is licensed under the terms of the MIT license.
//
// Hongwei Li lihw81@gmail.com
//
// 

#include "pconfig.h"

#include <PSystem/pcrt.h>
#include <PSystem/passet.h>
#include <PSystem/ppath.h>

#include "ini.h"

static int iniHandler_internal(void *user, const char *section, const char *name,
    const char *value)
{
    PConfig *config = (PConfig *)user;

#define P_FIND_CONFIG_ENTRY(entryName, entryType) \
    else if (pstrcmp(#entryName, name) == 0) \
    { \
        config->m_##entryName.setValue(&(config->m_##entryName.value), value); \
    }

    if (pstrcmp(section, "global") != 0)
    {
        PLOG_WARNING("Unidentified configuration section: %s", section);
        return 0;
    }
    P_FIND_CONFIG_ENTRY(maxContextNumber, puint32)
    P_FIND_CONFIG_ENTRY(defaultContextName, pchar*)
    // TODO: add more entries here.
    else
    {
        PLOG_WARNING("Unidentified configuration key: %s", name);
        return 0;
    }

#undef P_FIND_CONFIG_ENTRY

    return 1;
}

PConfig::PConfig()
{
}

PConfig::~PConfig()
{
}
// Search the pre-defined paths one by one till the first configuration file 
// is found. The order of pre-defined path is current folder, the system configuration
// folder and sdcard.
//
// TODO: complete the a system layer submodule for spitfire pre-defined path list.
bool PConfig::loadFromFile(const pchar* fileName)
{
    // First check the current directory
    if (fileName == NULL)
    {
        return false;
    }

    const pchar* pathList[] =
    {
        ".",
        pPathGetSystemDirectory(),
#if defined P_ANDROID
        pPathGetSDCardPath(),
#endif
    };

    for (size_t i = 0; i < sizeof(pathList) / sizeof(pathList[0]); ++i)
    {
        if (pathList[i] != P_NULL)
        {
            PString cfgPath = PString(pathList[i]) + PString(pPathGetDelimiter()) + PString(fileName);
            PFile *fp = pfopen(cfgPath.c_str(), "rb");
            if (fp != P_NULL)
            {
                if (ini_parse_file((FILE *)(fp), iniHandler_internal, this) < 0)
                {
                    PLOG_ERROR("Failed to parse spitfire.cfg");
                    pfclose(fp);
                    return false;
                }
                pfclose(fp);
                PLOG_INFO("Configuration file found and loaded at %s", cfgPath.c_str());

                return true;
            }
        }
    }

#if defined P_ANDROID
    PLOG_ERROR("Failed to open spitfire.cfg. See if the file exists in %s;%s;%s",
            pathList[0],
            pathList[1],
            pathList[2]);
#else
    PLOG_ERROR("Failed to open spitfire.cfg. See if the file exists in %s;%s",
            pathList[0],
            pathList[1]);
#endif
    
    return false;
}

bool PConfig::loadFromAsset(const pchar* fileName)
{
    PAsset asset = pAssetOpen(fileName);
    if (pAssetIsValid(&asset))
    {
        PIniBufferObject iniBufferObject;
        iniBufferObject.m_buffer = (const pchar*)pAssetGetBuffer(&asset);
        iniBufferObject.m_bufferSize = pAssetGetSize(&asset);
        iniBufferObject.m_position = 0;
        
        if (ini_parse_buffer(&iniBufferObject, iniHandler_internal, this) < 0)
        {
            PLOG_ERROR("Failed to parse spitfire.cfg");
            pAssetClose(&asset);
            return false;
        }

        pAssetClose(&asset);
    }
    return true;
       
}
bool PConfig::load()
{

    PString configPath(":spitfire.cfg");
    const pchar* newPath = configPath.c_str();
    if (pstrchr(newPath, ':') != P_NULL)
    {
        return loadFromAsset(newPath + 1);
    }
    else
    {
        return loadFromFile(newPath);
    }

    PASSERT_NOTREACHABLE();
    return false;
}
