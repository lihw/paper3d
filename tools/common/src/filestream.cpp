// stream.cpp
// A file read/write stream
//
// Copyright 2012 - 2014 Future Interface. 
// This software is licensed under the terms of the MIT license.
//
// Hongwei Li lihw81@gmail.com
//

#include "filestream.h"

#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>

#include <string>

FileStream::FileStream(const char *filename)
    : m_filename(filename)
{
    m_fp = NULL;
}

bool FileStream::openToWrite()
{
    fopen_s(&m_fp, m_filename.c_str(), "wb");
    if (m_fp == NULL)
    {
        return false;
    }
    return true;
}

void FileStream::close()
{
    fclose(m_fp);
}

void FileStream::writeUint8(unsigned char v)
{
    return writeBytes(&v, 1);
}

void FileStream::writeUint16(unsigned short v)
{
    return writeBytes((unsigned char*)&v, 2);
}

void FileStream::writeUint32(unsigned int v)
{
    return writeBytes((unsigned char*)&v, 4);
}

void FileStream::writeFloat32(float v)
{
    return writeBytes((unsigned char*)&v, 4);
}

void FileStream::writeBytes(unsigned char *bytes, unsigned int numBytes)
{
    if (m_fp == NULL)
    {
        throw std::string("Invalid file handle.");
    }

    size_t writtenBytes = fwrite(bytes, 1, numBytes, m_fp);
    if (writtenBytes != numBytes)
    {
        throw std::string("Failed to write required number of bytes.");
    }
}

void FileStream::printf(const char *format, ...)
{
    va_list arguments;
    va_start(arguments, format);
    char buffer[4096];
    size_t nchars = vsprintf_s(buffer, 4096, format, arguments);
    writeBytes((unsigned char*)(&buffer[0]), nchars);
}


