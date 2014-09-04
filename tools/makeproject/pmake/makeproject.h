// makeproject.h
// Make project
//
// Copyright 2012 - 2014 Future Interface. 
// This software is licensed under the terms of the MIT license.
//
// Hongwei Li lihw81@gmail.com
//

#ifndef MAKEPROJECT_H
#define MAKEPROJECT_H

#include <common/log.h>

#include <string>
#include <stdint.h>
#include <vector>
#include <list>
#include <map>

struct Arguments
{
	std::string projectType;
    std::string projectName;
    std::string shortProjectName;
    std::string projectDescription;
    std::string version;

    std::string company;
    std::string email;
    std::string copyright;
    
    std::string fileHeader;

    uint32_t windowWidth;
    uint32_t windowHeight;
    uint32_t multisamples;
    uint32_t maxFPS;

    enum 
    {
        NETWORK_MODULE,
        SOUND_MODULE,
        PHYSICS_MODULE,
        SENSOR_MODULE,

        MODULE_NUMBER,
    };

    bool modules[MODULE_NUMBER];

    Arguments();

    bool parse(const char *confFile);
    const char *getValue(const char *line);
    const char *trim(const char *text);
};

struct ProjectItem
{
    std::string path;
    std::list<ProjectItem *> children;
    ProjectItem *parent;
    bool isDir;

    ProjectItem()
    {
    }

    virtual ~ProjectItem()
    {
    }
};

struct FileItem : public ProjectItem
{
    std::list<std::string> args;
    
    FileItem()
    {
        isDir = false;
    }

    virtual ~FileItem()
    {
    }
};

struct DirectoryItem : public ProjectItem
{
    DirectoryItem()
    {
        isDir = true;
    }

    virtual ~DirectoryItem()
    {
    }
};

class MakeProject
{
public:
    MakeProject(const Arguments& arguments);
    ~MakeProject();

    bool run(); 

private:
    bool makeDirectory(const char *path);
    bool makeFile(const char *path,
                  const char *format,
                  const std::list<std::string> &args); 
    bool makeBinaryFile(const char *path,
                  const unsigned char *data,
                  size_t len);

    struct BinaryFile
    {
        const unsigned char *data;
        int len;
    };

    ProjectItem *parseDirectoryStructure(const char *str);
    bool constructDirectory(ProjectItem *root);
    void deleteDirectoryStructure(ProjectItem *root);
    
private:
    Arguments m_arguments;

    std::map<std::string, std::string> m_fileDatabase;
    std::map<std::string, BinaryFile> m_binaryFileDatabase;
};


#endif // !MAKEPROJECT_H
