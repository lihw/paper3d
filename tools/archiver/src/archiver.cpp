// archiver.h
// Archiver
//
// Copyright 2012 - 2014 Future Interface. 
// This software is licensed under the terms of the MIT license.
//
// Hongwei Li lihw81@gmail.com
//

#include "archiver.h"

#include <common/filestream.h>
#include <common/version.h>
#include <common/log.h>

#if defined WIN32
# include <windows.h>
#elif defined __APPLE__
# include <unistd.h>
# include <dirent.h>
# include <sys/types.h>
# include <sys/stat.h>
#endif

Archiver::Archiver(const Arguments& arguments)
{
    m_arguments = arguments;
}

Archiver::~Archiver()
{
    for (size_t i = 0; i < m_entries.size(); ++i)
    {
        delete [] m_entries[i].fileData;
    }
}

bool Archiver::archive()
{
    return loadEntries() && writeEntries();
}
    
bool Archiver::loadEntries()
{
    // Recursively load the entries in that path.
    return readDirectory("", m_arguments.inputPath.c_str());
}

bool Archiver::writeEntries()
{
    FileStream stream(m_arguments.outputFile.c_str());
    if (!stream.openToWrite())
    {
        logError("Failed to open %s for writing.\n", m_arguments.outputFile.c_str());
        return false;
    }
    
    // Replace '\\' with '/' in file names and remove the leading '/'
    for (size_t i = 0; i < m_entries.size(); ++i)
    {
        char *buffer = (char *)m_entries[i].filePath.c_str();
        char *p = buffer;
        while (*p != 0)
        {
            if (*p == '\\')
            {
                *p = '/';
            }
            ++p;
        }

        m_entries[i].filePath.erase(0, 1);
    }

    //
    // Write the file list to a supplementary file.
    //
#if defined WIN32
    char auxFilePath[MAX_PATH];
    strcpy_s(auxFilePath, MAX_PATH, m_arguments.outputFile.c_str());
#elif defined __APPLE__
    char auxFilePath[4096];
    strncpy(auxFilePath, m_arguments.outputFile.c_str(), 4095);
    auxFilePath[4095] = 0;
#endif
    char *suffix = strrchr(auxFilePath, '.');
    suffix[1] = 't';
    suffix[2] = 'x';
    suffix[3] = 't';

    FILE *fp;
#if defined WIN32
    fopen_s(&fp, auxFilePath, "wb");
#elif defined __APPLE__
    fp = fopen(auxFilePath, "wb");
#endif
    logInfo("There are %d entries.\n", m_entries.size());
    for (size_t i = 0; i < m_entries.size(); ++i)
    {
        fprintf(fp, "%s (%d bytes)\n", 
                m_entries[i].filePath.c_str(),
                m_entries[i].fileSize);
    }

    fclose(fp);

    // See core/archive/parchivefile.h
    struct Header
    {
        uint8_t magic[4];
        uint8_t version[4];
        uint32_t endianess;
        uint32_t fileCount;
        uint32_t fileEntryOffset;
        uint32_t fileNameOffset;
        uint32_t allFileEntrySize;
        uint32_t allFileNameSize;
        uint8_t extension[128];
    };

    // Write header
    Header header;

    header.magic[0] = 0xca;
    header.magic[1] = 0xfe;
    header.magic[2] = 0xbe;
    header.magic[3] = 0xbe;
    
    header.version[0] = 0x00;
    header.version[1] = 0x00;
    header.version[2] = (FI_VERSION >> 8);
    header.version[3] = (FI_VERSION && 0x00ff);

    header.endianess = 0x01020304;

    header.fileCount = m_entries.size();

    header.fileNameOffset = sizeof(Header);
    header.allFileNameSize = 0;
    for (size_t i = 0; i < m_entries.size(); ++i)
    {
        header.allFileNameSize += m_entries[i].filePath.length() + 1;
    }

    header.fileEntryOffset = header.fileNameOffset + header.allFileNameSize;
    header.allFileEntrySize = 8 * header.fileCount;

    stream.writeBytes((uint8_t*)&header, sizeof(Header));

    // Write file names
    for (size_t i = 0; i < m_entries.size(); ++i)
    {
        stream.writeBytes((uint8_t*)m_entries[i].filePath.c_str(), 
            m_entries[i].filePath.length() + 1);
    }

    // Write file entires
    uint32_t offset = header.fileEntryOffset + header.allFileEntrySize;
    for (size_t i = 0; i < m_entries.size(); ++i)
    {
        stream.writeUint32(offset);
        stream.writeUint32(m_entries[i].fileSize);
        offset += m_entries[i].fileSize;
    }

    // Write file data
    for (size_t i = 0; i < m_entries.size(); ++i)
    {
        stream.writeBytes(m_entries[i].fileData, m_entries[i].fileSize);
    }

    stream.close();

    return true;
}

bool Archiver::readDirectory(const char *prefix, const char *dir)
{
#if defined WIN32
    // Fetch the item names in that directory.
    WIN32_FIND_DATAA fdFile;
    HANDLE hFind = NULL;

    char path[MAX_PATH];

    // Specify a file mask. *.* = We want everything!
    sprintf_s(path, MAX_PATH, "%s\\*.*", dir);

    if ((hFind = FindFirstFileA(path, &fdFile)) == INVALID_HANDLE_VALUE)
    {
        logError("Failed to read directory %s.", path);
    }
    else
    {
        do
        {
            // Find first file will always return "." and ".." 
            // as the first two directories.
            if (strcmp(fdFile.cFileName, ".") != 0 && 
                strcmp(fdFile.cFileName, "..") != 0)
            {
                //Is the entity a File or Folder?
                if (fdFile.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY)
                {
                    char newPrefix[MAX_PATH];
                    sprintf_s(newPrefix, MAX_PATH, "%s\\%s", prefix, fdFile.cFileName);
                    char newPath[MAX_PATH];
                    sprintf_s(newPath, MAX_PATH, "%s\\%s", dir, fdFile.cFileName);
                    readDirectory(newPrefix, newPath);
                }
                else if ((fdFile.dwFileAttributes & FILE_ATTRIBUTE_NORMAL) ||
                         (fdFile.dwFileAttributes & FILE_ATTRIBUTE_ARCHIVE))
                {
                    // Ignore misc files that should be excluded from the asset archiver.
                    const char *suffix = strrchr(fdFile.cFileName, '.');
                    if (suffix == NULL)
                    {
                        continue;
                    }
                    if (strncmp(suffix, ".exe", 4) == 0 ||
                        strncmp(suffix, ".par", 4) == 0 ||
                        strncmp(suffix, ".ini", 4) == 0 ||
                        strncmp(suffix, ".txt", 4) == 0)
                    {
                        continue;
                    }

                    m_entries.push_back(ArchiveEntry());
                    ArchiveEntry *entry = &m_entries.back();
                    createEntry(entry, dir, prefix, fdFile.cFileName); 
                }
                else
                {
                    logWarning("%s\\%s is not supported in archiver.", path, fdFile.cFileName);
                }
            }
        } while (FindNextFileA(hFind, &fdFile)); // Find the next file.

        FindClose(hFind); //Always, Always, clean things up!
    }
#elif defined __APPLE__
        // Fetch the item names in that directory.
        DIR* dirObject;
        dirent* dirEntry;

        if ((dirObject = opendir(dir)) == NULL)
        {
            logError("Failed to read directory %s.", dir);
        }
        else
        {
            char path[4096];
            while ((dirEntry = readdir(dirObject)) != NULL)
            {
                // Find first file will always return "." and ".." 
                // as the first two directories.
                if (strcmp(dirEntry->d_name, ".") != 0 &&
                    strcmp(dirEntry->d_name, "..") != 0)
                {
                    //Is the entity a File or Folder?
                    if (dirEntry->d_type == DT_DIR)
                    {
                        char newPrefix[4096];
                        snprintf(newPrefix, 4096, "%s/%s", prefix, dirEntry->d_name);
                        char newPath[4096];
                        snprintf(newPath, 4096, "%s/%s", dir, dirEntry->d_name);
                        readDirectory(newPrefix, newPath);
                    }
                    else if (dirEntry->d_type == DT_REG)
                    {
                        // Ignore misc files that should be excluded from the asset archiver.
                        const char *suffix = strrchr(dirEntry->d_name, '.');
                        if (suffix == NULL)
                        {
                            continue;
                        }
                        if (strncmp(suffix, ".exe", 4) == 0 ||
                            strncmp(suffix, ".par", 4) == 0 ||
                            strncmp(suffix, ".ini", 4) == 0 ||
                            strncmp(suffix, ".txt", 4) == 0)
                        {
                            continue;
                        }

                        m_entries.push_back(ArchiveEntry());
                        ArchiveEntry *entry = &m_entries.back();
                        createEntry(entry, dir, prefix, dirEntry->d_name); 
                    }
                    else
                    {
                        logWarning("%s\\%s is not supported in archiver.", path, dirEntry->d_name);
                    }
                }
            }

            closedir(dirObject); //Always, Always, clean things up!
        }

#endif
    
    return true;
}

bool Archiver::createEntry(ArchiveEntry *entry, const char *path, const char *prefix, const char *fileName)
{
    // Read the entire file.
    FILE *fp;
    uint8_t *content = NULL;

    int count = 0;

#if defined WIN32
    char dir[MAX_PATH];
    sprintf_s(dir, "%s\\%s", path, fileName);
    fopen_s(&fp, dir, "rb");
#elif defined __APPLE__
    char dir[4096];
    snprintf(dir, 4096, "%s/%s", path, fileName);
    fp = fopen(dir, "rb");
#endif

    if (fp != NULL)
    {
        fseek(fp, 0, SEEK_END);
        count = ftell(fp);
        rewind(fp);

        if (count > 0) 
        {
            content = new uint8_t [count + 1];
            count = fread(content, sizeof(uint8_t), count, fp);
            content[count] = '\0';
        }
        fclose(fp);
    }

    entry->fileSize = count;
    entry->fileData = content;

    // The entry path
    entry->filePath = std::string(prefix) + std::string("\\") + std::string(fileName);

    return true;
}

