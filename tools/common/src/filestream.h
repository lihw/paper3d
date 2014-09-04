// filestream.h
// A file read/write stream
//
// Copyright 2012 - 2014 Future Interface. 
// This software is licensed under the terms of the MIT license.
//
// Hongwei Li lihw81@gmail.com
//

#ifndef FILESTREAM_H
#define FILESTREAM_H

#include <string>

class FileStream
{
public:
    FileStream(const char *filename);
    bool openToWrite();
    void close();

    void writeUint8(unsigned char v);
    void writeUint16(unsigned short v);
    void writeUint32(unsigned int v);
    void writeFloat32(float v);
    void writeBytes(unsigned char *bytes, unsigned int numBytes);
    void printf(const char *format, ...);

private:
    FILE *m_fp;
    std::string m_filename;
};

#endif // !FILESTREAM_H
