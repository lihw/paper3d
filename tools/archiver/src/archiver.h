// archiver.cpp
// Archiver
//
// Copyright 2012 - 2014 Future Interface. 
// This software is licensed under the terms of the MIT license.
//
// Hongwei Li lihw81@gmail.com
//

#ifndef ARCHIVER_H
#define ARCHIVER_H

#include <string>
#include <stdint.h>
#include <vector>

struct Arguments
{
    std::string inputPath;
    std::string outputFile;

    Arguments()
        : outputFile("output.par")
        , inputPath("")
    {

    }
};

struct ArchiveEntry
{
    uint32_t fileSize;
    std::string filePath;
    uint8_t* fileData;
};

class Archiver
{
public:
    Archiver(const Arguments& arguments);
    ~Archiver();

    bool archive(); 
    
private:
    bool loadEntries();
    bool writeEntries();
    // prefix the prefix path in the archive. Say there is a a.txt
    // in directory of path ".". Its physical path is ./a.txt. Its
    // archive path should be ":/a.txt" when the prefix is ":/".
    bool readDirectory(const char *prefix, const char *path);
    bool createEntry(ArchiveEntry *entry, const char *dir, const char *prefix, const char *fileName);

private:
    std::vector<ArchiveEntry> m_entries; 
    Arguments m_arguments;
};


#endif // !ARCHIVER_H
