// makeheaders.cpp
// Convert a file to a Cpp header
//
// Copyright 2012 - 2014 Future Interface. 
// This software is licensed under the terms of the MIT license.
//
// Hongwei Li lihw81@gmail.com
//

#include "makeheaders.h"

#include <common/log.h>

#include <windows.h>
#include <direct.h>

static bool makeAsciiHeader(const char *file, const char *varname, const char *header);
static bool makeBinaryHeader(const char *file, const char *varname, const char *header);

static char toLower(char c)
{
    if (c >= 'A' && c <= 'Z')
    {
        return c - 'A' + 'a';
    }

    return c;
}

static char toUpper(char c)
{
    if (c >= 'a' && c <= 'z')
    {
        return c - 'a' + 'A';
    }

    return c;
}

bool makeHeaders(const char *inputFolder, const char *outputFolder)
{
    // Fetch the item names in that directory.
    WIN32_FIND_DATAA fdFile;
    HANDLE hFind = NULL;

    char path[MAX_PATH];

    // Specify a file mask. *.* = We want everything!
    sprintf_s(path, MAX_PATH, "%s\\*.*", inputFolder);

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
                    char newInputFolder[MAX_PATH];
                    sprintf_s(newInputFolder, MAX_PATH, "%s\\%s", inputFolder, fdFile.cFileName);
                    makeHeaders(newInputFolder, outputFolder);
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
                    if (strncmp(suffix, ".java", 5) == 0 ||
                        strncmp(suffix, ".sln", 4) == 0 ||
                        strncmp(suffix, ".vcxproj", 8) == 0 ||
                        strncmp(suffix, ".user", 5) == 0 ||
                        strncmp(suffix, ".cpp", 4) == 0 ||
                        strncmp(suffix, ".xml", 4) == 0 ||
                        strncmp(suffix, ".psc", 4) == 0 ||
                        strncmp(suffix, ".cfg", 4) == 0 ||
                        strncmp(suffix, ".pmt", 4) == 0 ||
                        strncmp(suffix, ".mk", 3) == 0 ||
                        strncmp(suffix, ".properties", 11) == 0 ||
                        strncmp(suffix, ".bat", 4) == 0 ||
                        strncmp(suffix, ".h", 2) == 0)
                    {
                        char filepath[MAX_PATH];
                        sprintf_s(filepath, MAX_PATH, "%s\\%s", inputFolder, fdFile.cFileName);
                        
                        char outputFilename[MAX_PATH]; 
                        char varname[MAX_PATH];
                        char *p = fdFile.cFileName;
                        char *q = outputFilename;
                        char *v = varname;
                        while (*p != 0)
                        {
                            if (*p != '.')
                            {
                                *q++ = toLower(*p);
                                *v++ = toUpper(*p);
                            }
                            p++;
                        }
                        q[0] = '.';
                        q[1] = 'h';
                        q[2] = 0;

                        v[0] = '_';
                        v[1] = 'S';
                        v[2] = 'T';
                        v[3] = 'R';
                        v[4] = 0;

                        char outputFilepath[MAX_PATH]; 
                        sprintf_s(outputFilepath, MAX_PATH, "%s\\%s", outputFolder, outputFilename);

                        makeAsciiHeader(filepath, varname, outputFilepath);
                    }
					else if (strncmp(suffix, ".png", 4) == 0 ||
                             strncmp(suffix, ".pmh", 4) == 0)
					{
                        char filepath[MAX_PATH];
                        sprintf_s(filepath, MAX_PATH, "%s\\%s", inputFolder, fdFile.cFileName);
                        
                        char outputFilename[MAX_PATH]; 
                        char varname[MAX_PATH];
                        char *p = fdFile.cFileName;
                        char *q = outputFilename;
                        char *v = varname;
                        while (*p != 0)
                        {
                            if (*p != '.')
                            {
                                *q++ = toLower(*p);
                                *v++ = toUpper(*p);
                            }
                            p++;
                        }
                        q[0] = '.';
                        q[1] = 'h';
                        q[2] = 0;

                        v[0] = '_';
                        v[1] = 'D';
                        v[2] = 'A';
                        v[3] = 'T';
                        v[4] = 'A';
                        v[5] = 0;

                        char outputFilepath[MAX_PATH]; 
                        sprintf_s(outputFilepath, MAX_PATH, "%s\\%s", outputFolder, outputFilename);

                        makeBinaryHeader(filepath, varname, outputFilepath);
					}
                }
                else
                {
                    logWarning("%s is not supported in archiver.", path);
                }
            }
        } while (FindNextFileA(hFind, &fdFile)); // Find the next file.

        FindClose(hFind); //Always, Always, clean things up!
    }

    return true;

}

static bool makeAsciiHeader(const char *file, const char *varname, const char *header)
{
    FILE *src; 
    FILE *dst; 
    fopen_s(&src, file, "rb");
    fopen_s(&dst, header, "wb");
    if (src == NULL)
    {
        logError("Failed to open %s.\n", file);
        return false;
    }
    if (dst == NULL)
    {
        logError("Failed to open %s.\n", header);
        return false;
    }

    fprintf(dst, "const char %s[] = \n", varname);

    char line[1024];
    while (fgets(line, 1024, src))
    {
        // replace '"' with '\"' in the line.
        char outputLine[4096];
        char* p = line;
        char* q = outputLine;
            
        *q++ = '"';

        while (*p != '\n' && *p != '\r')
        {
            if (*p == '"')
            {
                *q++ = '\\';
                *q++ = '"';
                p++;
            }
            else if (*p == '\\')
            {
                *q++ = '\\';
                *q++ = '\\';
                p++;
            }
			else if (*p == '%')
            {
                *q++ = '%';
                *q++ = '%';
                p++;
            }
            else
            {
                *q++ = *p++;
            }
        }

        *q++ = '\\';
        *q++ = 'n';
        *q++ = '"';
        *q++ = '\n';
        *q = 0;

        fputs(outputLine, dst);
    } 

    fputs(";", dst);

    fclose(src);
    fclose(dst);

    return true;
}

static bool makeBinaryHeader(const char *file, const char *varname, const char *header)
{
    FILE *src; 
    FILE *dst; 
    fopen_s(&src, file, "rb");
    fopen_s(&dst, header, "wb");
    if (src == NULL)
    {
        logError("Failed to open %s.\n", file);
        return false;
    }
    if (dst == NULL)
    {
        logError("Failed to open %s.\n", header);
        return false;
    }

	fprintf(dst, "const unsigned char %s[] = {\n", varname);

    int c;
    int count = 0;
    do 
    {
        c = fgetc(src);
        
        // Convert the ascii to a number text and output to dst file.
        if (c != EOF)
        {
            fprintf(dst, "0x%02x,", c);
        }

        count++;
        if (count == 16)
        {
            fprintf(dst, "\n");
            count = 0;
        }
    } while (c != EOF);

	fprintf(dst, "};");

    fclose(src);
    fclose(dst);

    return true;
}

