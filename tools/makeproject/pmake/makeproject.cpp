// makeproject.cpp
// Make project
//
// Copyright 2012 - 2014 Future Interface. 
// This software is licensed under the terms of the MIT license.
//
// Hongwei Li lihw81@gmail.com
//

#include "makeproject.h"

#include "headers/wallpaper/headers.h"
#include "headers/application/headers.h"

#include "project.h"

#include <map>
#include <string>

#include <windows.h>
#include <stdio.h>

Arguments::Arguments()
	: projectType("application")
    , projectName("Untitled")
	, shortProjectName("untitled")
    , projectDescription("A simple application")

    , company("Unknown")
    , email("anonymous@somewhere.com")
    , copyright("Copyright ? - 2014. All rights reserved.")

    , windowWidth(640)
    , windowHeight(480)
    , multisamples(2)
    , maxFPS(25)
{
    for (uint32_t i = 0; i < MODULE_NUMBER; ++i)
    {
        modules[i] = false;
    }

}
    
bool Arguments::parse(const char *confFile)
{
    FILE *fp;

    fopen_s(&fp, confFile, "rb");
    if (fp == NULL)
    {
        printf("Failed to open %s.\n", confFile);
        return false;
    }

    char line[1024];
    while (fgets(line, 1024, fp))
    {
        // Remove the '\n' at the tail.
        char *n = strrchr(line, '\n');
        char *r = strrchr(line, '\r');
        if (n != NULL)
        {
            *n = 0;
        }
        if (r != NULL)
        {
            *r = 0;
        }

        const char *l = trim(line);

        // The line has been commented out by a leading '#'
        if (l[0] == '#' || l[0] == 0)
        {
            continue;
        }
        

        if (strncmp(l, "project_name", 12) == 0)
        {
            projectName = std::string(getValue(l));
        }
		else if (strncmp(l, "project_type", 12) == 0)
        {
            projectType = std::string(getValue(l));
        }
        else if (strncmp(l, "project_description", 19) == 0)
        {
            projectDescription = std::string(getValue(l));
        }
        else if (strncmp(l, "company", 7) == 0)
        {
            company = std::string(getValue(l));
        }
        else if (strncmp(l, "copyright", 9) == 0)
        {
            copyright = std::string(getValue(l));
        }
        else if (strncmp(l, "version", 7) == 0)
        {
            version = std::string(getValue(l));
        }
        else if (strncmp(l, "email", 5) == 0)
        {
            email = std::string(getValue(l));
        }
        else if (strncmp(l, "width", 5) == 0)
        {
            windowWidth = atoi(getValue(l));
        }
        else if (strncmp(l, "height", 6) == 0)
        {
            windowHeight = atoi(getValue(l));
        }
        else if (strncmp(l, "multisamples", 12) == 0)
        {
            multisamples = atoi(getValue(l));
        }
        else if (strncmp(l, "maxfps", 6) == 0)
        {
            maxFPS = atoi(getValue(l));
        }
        else if (strncmp(l, "modules", 7) == 0)
        {
            const char *value = getValue(l);
            if (strstr(value, "network") != NULL)
            {
                modules[NETWORK_MODULE] = true;
            }
            if (strstr(value, "sound") != NULL)
            {
                modules[SOUND_MODULE] = true;
            }
            if (strstr(value, "physics") != NULL)
            {
                modules[PHYSICS_MODULE] = true;
            }
            if (strstr(value, "sensor") != NULL)
            {
                modules[SENSOR_MODULE] = true;
            }
        }
        else
        {
            logWarning("Unknown parameter at line \"%s\".", l);
        }
    }

    fclose(fp);

	// Generate the brief version of project name
    const char *q = projectName.c_str();
    char text[1024];
    char *p = text;

    while (*q != 0)
    {
        *p++ = tolower(*q++);
    }
    *p = 0;

    shortProjectName = std::string(text);

    // Generate the file header
    fileHeader = 
        std::string("// ") + projectDescription + std::string("\n") + 
        std::string("//\n") + 
        std::string("// ") + copyright + std::string("\n") +
        std::string("//\n") + 
        std::string("// ") + company + std::string("  ") + email;

    // Sanity check
    if (multisamples != 1 && multisamples != 2 && multisamples != 4)
    {
        logError("multisamples can only be 1, 2 or 4.");
        return false;
    }


    return true;
}
    
const char *Arguments::getValue(const char *line)
{
    const char* dm = strrchr(line, '=');
    if (dm == NULL)
    {
        printf("Wrong format at line %s.\n", line);
        return NULL;
    }

    return trim(dm + 1);
}
    
const char *Arguments::trim(const char *text)
{
    const char* ret = text;
    while ((*ret == ' ' || *ret == '\t') && *ret != 0)
    {
        ++ret;
    }

    return ret;
}

//
// class MakeProject
//
MakeProject::MakeProject(const Arguments& arguments)
{
    m_arguments = arguments;
}

MakeProject::~MakeProject()
{
}

bool MakeProject::run()
{
	char capitalizedProjectName[1024];
	char *q = capitalizedProjectName;
	const char *p = m_arguments.shortProjectName.c_str();
	while (*p != 0)
	{
		*q++ = toupper(*p++);
	}
	*q = 0;

    if (m_arguments.projectType == "wallpaper")
    {
        m_fileDatabase.insert(std::make_pair(std::string(capitalizedProjectName) + std::string("SLN")         , std::string(wallpaper::MSVC2012SLN_STR)));
        m_fileDatabase.insert(std::make_pair(std::string(capitalizedProjectName) + std::string("VCXPROJ")     , std::string(wallpaper::MSVC2012VCXPROJ_STR)));
        m_fileDatabase.insert(std::make_pair(std::string(capitalizedProjectName) + std::string("VCXPROJUSER") , std::string(wallpaper::MSVC2012VCXPROJUSER_STR)));

        m_fileDatabase.insert(std::make_pair(std::string("ANDROIDMK")             , std::string(wallpaper::ANDROIDMK_STR)));
        m_fileDatabase.insert(std::make_pair(std::string("APPLICATIONMK")         , std::string(wallpaper::APPLICATIONMK_STR)));
        m_fileDatabase.insert(std::make_pair(std::string("MAINXML")               , std::string(wallpaper::MAINXML_STR)));
        m_fileDatabase.insert(std::make_pair(std::string("STRINGSXML")            , std::string(wallpaper::STRINGSXML_STR)));
        m_fileDatabase.insert(std::make_pair(std::string("SETTINGSXML")           , std::string(wallpaper::SETTINGSXML_STR)));
        m_fileDatabase.insert(std::make_pair(std::string("WALLPAPER_SETTINGSXML") , std::string(wallpaper::WALLPAPER_SETTINGSXML_STR)));
        m_fileDatabase.insert(std::make_pair(std::string("ANDROIDMANIFESTXML")    , std::string(wallpaper::ANDROIDMANIFESTXML_STR)));
        m_fileDatabase.insert(std::make_pair(std::string("ANTPROPERTIES")         , std::string(wallpaper::ANTPROPERTIES_STR)));
        m_fileDatabase.insert(std::make_pair(std::string("PROJECTPROPERTIES")     , std::string(wallpaper::PROJECTPROPERTIES_STR)));
        m_fileDatabase.insert(std::make_pair(std::string("BUILDBAT")              , std::string(wallpaper::BUILDBAT_STR)));
        m_fileDatabase.insert(std::make_pair(std::string("BUILDXML")              , std::string(wallpaper::BUILDXML_STR)));
        m_fileDatabase.insert(std::make_pair(std::string("PROGUARDCFG")           , std::string(wallpaper::PROGUARDCFG_STR)));
        
        m_fileDatabase.insert(std::make_pair(std::string("MYCONTEXTH")            , std::string(wallpaper::MYCONTEXTH_STR)));
        m_fileDatabase.insert(std::make_pair(std::string("MYCONTEXTCPP")          , std::string(wallpaper::MYCONTEXTCPP_STR)));
        m_fileDatabase.insert(std::make_pair(std::string("MYSCENEH")              , std::string(wallpaper::MYSCENEH_STR)));
        m_fileDatabase.insert(std::make_pair(std::string("MYSCENECPP")            , std::string(wallpaper::MYSCENECPP_STR)));
        m_fileDatabase.insert(std::make_pair(std::string("PMAINCPP")              , std::string(wallpaper::PMAINCPP_STR)));
        m_fileDatabase.insert(std::make_pair(std::string("MAINCPP")               , std::string(wallpaper::MAINCPP_STR)));

        m_fileDatabase.insert(std::make_pair(std::string("TEXTUREPMT")            , std::string(wallpaper::TEXTUREPMT_STR)));
        m_fileDatabase.insert(std::make_pair(std::string("SCENEPSC")              , std::string(wallpaper::SCENEPSC_STR)));

        m_fileDatabase.insert(std::make_pair(std::string("GLWALLPAPERSERVICEJAVA")       , std::string(wallpaper::GLWALLPAPERSERVICEJAVA_STR)));
        m_fileDatabase.insert(std::make_pair(std::string("PCONTEXTJAVA")                 , std::string(wallpaper::PCONTEXTJAVA_STR)));
        m_fileDatabase.insert(std::make_pair(std::string("PNATIVELIBRARYJAVA")           , std::string(wallpaper::PNATIVELIBRARYJAVA_STR)));
        m_fileDatabase.insert(std::make_pair(std::string("PGLWALLPAPERSERVICEJAVA")      , std::string(wallpaper::PGLWALLPAPERSERVICEJAVA_STR)));
        m_fileDatabase.insert(std::make_pair(std::string("PAPER3DJAVA")                  , std::string(wallpaper::PAPER3DWALLPAPERJAVA_STR)));
        m_fileDatabase.insert(std::make_pair(std::string("PAPER3DSETTINGSJAVA")			 , std::string(wallpaper::PAPER3DWALLPAPERSETTINGSJAVA_STR)));

        BinaryFile bf;

        bf.data = wallpaper::ICONPNG_DATA;
        bf.len = sizeof(wallpaper::ICONPNG_DATA);
        m_binaryFileDatabase.insert(std::make_pair(std::string("ICONPNG"), bf));
        
        bf.data = wallpaper::BG_D_0PNG_DATA;
        bf.len = sizeof(wallpaper::BG_D_0PNG_DATA);
        m_binaryFileDatabase.insert(std::make_pair(std::string("BG_D_0PNG"), bf));
        
        bf.data = wallpaper::CAO_D_0PNG_DATA;
        bf.len = sizeof(wallpaper::CAO_D_0PNG_DATA);
        m_binaryFileDatabase.insert(std::make_pair(std::string("CAO_D_0PNG"), bf));
        
        bf.data = wallpaper::FLOOR_D_0PNG_DATA;
        bf.len = sizeof(wallpaper::FLOOR_D_0PNG_DATA);
        m_binaryFileDatabase.insert(std::make_pair(std::string("FLOOR_D_0PNG"), bf));
        
        bf.data = wallpaper::MOGUPNG_DATA;
        bf.len = sizeof(wallpaper::MOGUPNG_DATA);
        m_binaryFileDatabase.insert(std::make_pair(std::string("MOGUPNG"), bf));
        
        bf.data = wallpaper::WATERPNG_DATA;
        bf.len = sizeof(wallpaper::WATERPNG_DATA);
        m_binaryFileDatabase.insert(std::make_pair(std::string("WATERPNG"), bf));
        
        bf.data = wallpaper::CAOPMH_DATA;
        bf.len = sizeof(wallpaper::CAOPMH_DATA);
        m_binaryFileDatabase.insert(std::make_pair(std::string("CAOPMH"), bf));
        
        bf.data = wallpaper::FLOORPMH_DATA;
        bf.len = sizeof(wallpaper::FLOORPMH_DATA);
        m_binaryFileDatabase.insert(std::make_pair(std::string("FLOORPMH"), bf));
        
        bf.data = wallpaper::MOGUPMH_DATA;
        bf.len = sizeof(wallpaper::MOGUPMH_DATA);
        m_binaryFileDatabase.insert(std::make_pair(std::string("MOGUPMH"), bf));
        
        bf.data = wallpaper::WATERPMH_DATA;
        bf.len = sizeof(wallpaper::WATERPMH_DATA);
        m_binaryFileDatabase.insert(std::make_pair(std::string("WATERPMH"), bf));
    }
    else if (m_arguments.projectType == "application")
    {
        m_fileDatabase.insert(std::make_pair(std::string(capitalizedProjectName) + std::string("SLN")         , std::string(application::MSVC2012SLN_STR)));
        m_fileDatabase.insert(std::make_pair(std::string(capitalizedProjectName) + std::string("VCXPROJ")     , std::string(application::MSVC2012VCXPROJ_STR)));
        m_fileDatabase.insert(std::make_pair(std::string(capitalizedProjectName) + std::string("VCXPROJUSER") , std::string(application::MSVC2012VCXPROJUSER_STR)));

        m_fileDatabase.insert(std::make_pair(std::string("ANDROIDMK")             , std::string(application::ANDROIDMK_STR)));
        m_fileDatabase.insert(std::make_pair(std::string("APPLICATIONMK")         , std::string(application::APPLICATIONMK_STR)));
        m_fileDatabase.insert(std::make_pair(std::string("MAINXML")               , std::string(application::MAINXML_STR)));
        m_fileDatabase.insert(std::make_pair(std::string("STRINGSXML")            , std::string(application::STRINGSXML_STR)));
        m_fileDatabase.insert(std::make_pair(std::string("ANDROIDMANIFESTXML")    , std::string(application::ANDROIDMANIFESTXML_STR)));
        m_fileDatabase.insert(std::make_pair(std::string("ANTPROPERTIES")         , std::string(application::ANTPROPERTIES_STR)));
        m_fileDatabase.insert(std::make_pair(std::string("PROJECTPROPERTIES")     , std::string(application::PROJECTPROPERTIES_STR)));
        m_fileDatabase.insert(std::make_pair(std::string("BUILDBAT")              , std::string(application::BUILDBAT_STR)));
        m_fileDatabase.insert(std::make_pair(std::string("BUILDXML")              , std::string(application::BUILDXML_STR)));
        m_fileDatabase.insert(std::make_pair(std::string("PROGUARDCFG")           , std::string(application::PROGUARDCFG_STR)));
        
        m_fileDatabase.insert(std::make_pair(std::string("MYCONTEXTH")            , std::string(application::MYCONTEXTH_STR)));
        m_fileDatabase.insert(std::make_pair(std::string("MYCONTEXTCPP")          , std::string(application::MYCONTEXTCPP_STR)));
        m_fileDatabase.insert(std::make_pair(std::string("MYSCENEH")              , std::string(application::MYSCENEH_STR)));
        m_fileDatabase.insert(std::make_pair(std::string("MYSCENECPP")            , std::string(application::MYSCENECPP_STR)));
        m_fileDatabase.insert(std::make_pair(std::string("PMAINCPP")              , std::string(application::PMAINCPP_STR)));
        m_fileDatabase.insert(std::make_pair(std::string("MAINCPP")               , std::string(application::MAINCPP_STR)));

        m_fileDatabase.insert(std::make_pair(std::string("TEXTUREPMT")            , std::string(application::TEXTUREPMT_STR)));
        m_fileDatabase.insert(std::make_pair(std::string("SCENEPSC")              , std::string(application::SCENEPSC_STR)));

        m_fileDatabase.insert(std::make_pair(std::string("PCONTEXTJAVA")                 , std::string(application::PCONTEXTJAVA_STR)));
        m_fileDatabase.insert(std::make_pair(std::string("PNATIVELIBRARYJAVA")           , std::string(application::PNATIVELIBRARYJAVA_STR)));
        m_fileDatabase.insert(std::make_pair(std::string("PVIEWJAVA")                    , std::string(application::PVIEWJAVA_STR)));
        m_fileDatabase.insert(std::make_pair(std::string("PAPER3DJAVA")                  , std::string(application::PAPER3DJAVA_STR)));
        m_fileDatabase.insert(std::make_pair(std::string("PACTIVITYJAVA")			     , std::string(application::PACTIVITYJAVA_STR)));

        BinaryFile bf;

        bf.data = application::ICONPNG_DATA;
        bf.len = sizeof(application::ICONPNG_DATA);
        m_binaryFileDatabase.insert(std::make_pair(std::string("ICONPNG"), bf));
        
        bf.data = application::BG_D_0PNG_DATA;
        bf.len = sizeof(application::BG_D_0PNG_DATA);
        m_binaryFileDatabase.insert(std::make_pair(std::string("BG_D_0PNG"), bf));
        
        bf.data = application::CAO_D_0PNG_DATA;
        bf.len = sizeof(application::CAO_D_0PNG_DATA);
        m_binaryFileDatabase.insert(std::make_pair(std::string("CAO_D_0PNG"), bf));
        
        bf.data = application::FLOOR_D_0PNG_DATA;
        bf.len = sizeof(application::FLOOR_D_0PNG_DATA);
        m_binaryFileDatabase.insert(std::make_pair(std::string("FLOOR_D_0PNG"), bf));
        
        bf.data = application::MOGUPNG_DATA;
        bf.len = sizeof(application::MOGUPNG_DATA);
        m_binaryFileDatabase.insert(std::make_pair(std::string("MOGUPNG"), bf));
        
        bf.data = application::WATERPNG_DATA;
        bf.len = sizeof(application::WATERPNG_DATA);
        m_binaryFileDatabase.insert(std::make_pair(std::string("WATERPNG"), bf));
        
        bf.data = application::CAOPMH_DATA;
        bf.len = sizeof(application::CAOPMH_DATA);
        m_binaryFileDatabase.insert(std::make_pair(std::string("CAOPMH"), bf));
        
        bf.data = application::FLOORPMH_DATA;
        bf.len = sizeof(application::FLOORPMH_DATA);
        m_binaryFileDatabase.insert(std::make_pair(std::string("FLOORPMH"), bf));
        
        bf.data = application::MOGUPMH_DATA;
        bf.len = sizeof(application::MOGUPMH_DATA);
        m_binaryFileDatabase.insert(std::make_pair(std::string("MOGUPMH"), bf));
        
        bf.data = application::WATERPMH_DATA;
        bf.len = sizeof(application::WATERPMH_DATA);
        m_binaryFileDatabase.insert(std::make_pair(std::string("WATERPMH"), bf));
    }
    else
    {
        logError("%s project is not supported for now!", m_arguments.projectType.c_str());
        return false;
    }

    // Construct the directory structure from the string.
    try
    {
        ProjectItem *root;
       
        if (m_arguments.projectType == "panel3d")
        {
            root = parseDirectoryStructure(panel3DProjectConf);
        }
        else if (m_arguments.projectType == "wallpaper")
        {
            root = parseDirectoryStructure(wallpaperProjectConf);
        }
        else if (m_arguments.projectType == "application")
        {
            root = parseDirectoryStructure(applicationProjectConf);
        }
        else
        {
            logError("Unknown project type %s.", m_arguments.projectType.c_str());
            return false;
        }

        if (root != NULL)
        {
            bool ret = constructDirectory(root);
            deleteDirectoryStructure(root);
            return ret;
        }
    }
    catch (std::string &errmsg)
    {
        logError("%s.\n", errmsg.c_str());
        return false;
    }

    return true;
}

ProjectItem *MakeProject::parseDirectoryStructure(const char *str)
{
    ProjectItem *rootItem = NULL;
    ProjectItem *lastItem = NULL;
    int lastDepth = -1;

    char line[1024];
    const char *p = str;

    int lineno = 1;
        
    // Read line by line
    while (*p != 0)
    {
        char *l = &line[0];
        while (*p != '\n')
        {
            *l++ = *p++;
        }
        *l = 0;

        // Find the depth of the node
        l = &line[0];
        int depth = 0;
        while (*l == ' ')
        {
            depth++;
            l++;
        }

        // The type of the node
        bool isDir = false;
        if (*l == '+')
        {
            isDir = true;
        }
        else if (*l == '-')
        {
            isDir = false;
        }
        else
        {
            char errmsg[4096];
            sprintf_s(errmsg, 4096, "Unknown project item type at line %d", lineno);
            throw std::string(errmsg);
        }
        l++;

        // Parse the rest of line.
        std::list<std::string> args;
        char arg[1024];
        char *a = arg;
        while (*l != 0)
        {
            if (*l == ' ')
            {
                *a = 0;
                args.push_back(std::string(arg));
                a = arg;
                l++;
            }
            else
            {
                *a++ = *l++;
            }
        }
        *a = 0;
        args.push_back(std::string(arg));

        // Replace the parameter with value.
        std::list<std::string>::iterator it = args.begin();
        std::list<std::string>::iterator ie = args.end();
        while (it != ie)
        {
            if (it->compare(0, 14, "<project-name>") == 0)
            {
                it->replace(0, 14, m_arguments.shortProjectName);
            }
            else if (it->compare(0, 23, "<official-project-name>") == 0)
            {
                it->replace(0, 23, m_arguments.projectName);
            }
            else if (*it == "<company>")
            {
                *it = m_arguments.company;
            }
            else if (*it == "<copyinfo>")
            {
                *it = m_arguments.copyright;
            }
            else if (*it == "<email>")
            {
                *it = m_arguments.email;
            }
            else if (*it == "<project-description>")
            {
                *it = m_arguments.projectDescription;
            }
            else if (*it == "<window-width>")
            {
                char text[1024];
                sprintf_s(text, 1024, "%d", m_arguments.windowWidth);
                *it = std::string(text);
            }
            else if (*it == "<window-height>")
            {
                char text[1024];
                sprintf_s(text, 1024, "%d", m_arguments.windowHeight);
                *it = std::string(text);
            }
            else if (*it == "<multisamples>")
            {
                char text[1024];
                sprintf_s(text, 1024, "%d", m_arguments.multisamples);
                *it = std::string(text);
            }
            else if (*it == "<maxfps>")
            {
                char text[1024];
                sprintf_s(text, 1024, "%d", m_arguments.maxFPS);
                *it = std::string(text);
            }
            else if (*it == "<file-header>")
            {
                *it = m_arguments.fileHeader;
            }
            else if (*it == "<version>")
            {
                *it = m_arguments.version;
            }
            else
            {
                if (it->c_str()[0] == '<')
                {
                    char errmsg[4096];
                    sprintf_s(errmsg, 4096, "Unknown argument %s at line %d", it->c_str(), lineno);
                    throw std::string(errmsg);
                }
            }

            ++it;
        }

        ProjectItem *item = NULL;
        if (isDir)
        {
            item = new DirectoryItem;
        }
        else
        {
            FileItem *fileItem = new FileItem;

            std::list<std::string>::iterator it = args.begin();
            std::list<std::string>::iterator ie = args.end();
            fileItem->args.assign(++it, ie);
            item = fileItem;
        }

        ProjectItem *dir = NULL;

        // Construct the node.
        if (lastDepth > depth)
        {
            // Back to upper level.
            int steps = (lastDepth - depth) / 2;
            dir = lastItem->parent;
            for (int i = 0; i < steps; ++i)
            {
                dir = dir->parent;
            }
        }
        else if (lastDepth == depth)
        {
            // Back to upper level.
            dir = (ProjectItem *)(lastItem->parent);
        }
        else
        {
            dir = lastItem;
            if (lastItem != NULL && !lastItem->isDir)
            {
                char errmsg[4096];
                sprintf_s(errmsg, 4096, "File cant be put under another file line %d", lineno);
                throw std::string(errmsg);
            }
        }

        if (dir != NULL)
        {
            char path[4096];
            sprintf_s(path, 4096, "%s\\%s", dir->path.c_str(), args.front().c_str());
            item->path = std::string(path);

            dir->children.push_back(item);
        }
        else
        {
            char path[4096];
            strcpy_s(path, 4096, args.front().c_str());
            item->path = std::string(path);
        }
        
        item->parent = dir;

        if (lastItem == NULL)
        {
            rootItem = item;
        }

        lastItem = item;
        lastDepth = depth;

        p++;

        lineno++;
    }

    return rootItem;
}
    
bool MakeProject::constructDirectory(ProjectItem *root)
{
    std::list<ProjectItem *> items;

    // Breadth transverse to create the directory, subdirectories
    // and all files. 
    items.push_back(root);

    while (!items.empty())
    {
        ProjectItem *item = items.front();
        items.pop_front();

        std::list<ProjectItem *>::iterator it = item->children.begin();
        std::list<ProjectItem *>::iterator ie = item->children.end();
        while (it != ie)
        {
            items.push_back(*it);
            ++it;
        }

        if (item->isDir)
        {
            makeDirectory(item->path.c_str());
        }
        else 
        {
            // Find the content of the file in the database.
            const char *suffix = strrchr(item->path.c_str(), '.');
            const char *filename = strrchr(item->path.c_str(), '\\');
            if (filename == NULL)
            {
                char buffer[1024];
                sprintf_s(buffer, 1024, "Corrupted file path %s", item->path);
                throw std::string(buffer);
                return false;
            }

			// Create the key of the file in the database.
            const char *f = filename + 1; // Skip the leading '\'
            char strName[1024];
            char *p = strName;
            while (*f != 0)
            {
                if (*f != '.' && *f != ' ')
                {
                    if (*f >= 'a' && *f <= 'z')
                    {
                        *p = *f - 'a' + 'A';
                    }
                    else
                    {
                        *p = *f;
                    }
                    p++;
                }
                f++;
            }
            *p = 0;
            
            // Check if it is binary file.
            if (suffix == NULL ||
                strncmp(suffix, ".res", 4) == 0 ||
                strncmp(suffix, ".png", 4) == 0 ||
                strncmp(suffix, ".pmh", 4) == 0)
            {
                const BinaryFile &binaryFile = m_binaryFileDatabase[strName];
                makeBinaryFile(item->path.c_str(), binaryFile.data, binaryFile.len);
            }
            else
            {
                FileItem *fileItem = (FileItem *)(item);

                const std::string &file = m_fileDatabase[strName];
                makeFile(item->path.c_str(), file.c_str(), fileItem->args);
            }
        }
    }

    return true;
}

void MakeProject::deleteDirectoryStructure(ProjectItem *root)
{
    std::list<ProjectItem *> items;

    // Breadth tranverse to delete the entire tree.
    items.push_back(root);

    while (!items.empty())
    {
        ProjectItem *node = items.front();
        items.pop_front();

        std::list<ProjectItem *>::iterator it = node->children.begin();
        std::list<ProjectItem *>::iterator ie = node->children.end();
        while (it != ie)
        {
            items.push_back(*it);
            ++it;
        }

        delete node;
    }
}
    
bool MakeProject::makeDirectory(const char *path)
{
    if (!CreateDirectoryA(path, NULL))
    {
        DWORD err = GetLastError();
        if (err == ERROR_ALREADY_EXISTS)
        {
            printf("Directory (%s) already exists.\n", path);
            return true;
        }
        else if (err == ERROR_PATH_NOT_FOUND)
        {
            printf("Some intermediate directories in the path (%s) are missing.\n", path);
            return false;
        }

        return false;
    }

    return true;
}
    
bool MakeProject::makeFile(const char *path,
                  const char *format,
                  const std::list<std::string> &args)
{
    FILE *fp;
    fopen_s(&fp, path, "wb");
    if (fp == NULL)
    {
        printf("Failed to open %s for writing.\n", path);
        throw false;

        return false;
    }

    std::list<std::string>::const_iterator it = args.begin();
    const char *p = format;
    while (*p != 0)
    {
        if (*p == '%')
        {
            if (*(p + 1) == 's')
            {
                if (it == args.end())
                {
                    throw std::string("There are not enough format arguments in outputing" + 
                            std::string(path));
                }
                fputs(it->c_str(), fp);
                ++it;
                ++p;
            }
            else if (*(p + 1) == '%')
            {
                fputc('%', fp);
                ++p;
            }
            else
            {
                char buffer[6];
                buffer[0] = p[0];
                buffer[1] = p[1];
                buffer[2] = p[2];
                buffer[3] = p[3];
                buffer[4] = p[4];
                buffer[5] = 0;
                throw std::string("Unknown format indicator " + 
                        std::string(buffer) + std::string("at") + std::string(path));
                fclose(fp);
                return false;
            }
        }
        else
        {
            fputc(*p, fp);
        }

        p++;
    }

    fclose(fp);

    return true;
}
    
bool MakeProject::makeBinaryFile(const char *path,
                  const unsigned char *data,
                  size_t len)
{
    FILE *fp;
    fopen_s(&fp, path, "wb");
    if (fp == NULL)
    {
        char errmsg[1024];
        sprintf_s(errmsg, 1024, "Failed to open %s for writing.", path);
        throw std::string(errmsg);

        return false;
    }

    size_t writtenBytes = fwrite(data, 1, len, fp);
    if (writtenBytes != len)
    {
        fclose(fp);
        
        char errmsg[1024];
        sprintf_s(errmsg, 1024, "Failed to write to %s.\n", path);
        throw std::string(errmsg);

        return false;
    }

    fclose(fp);

    return true;
}
