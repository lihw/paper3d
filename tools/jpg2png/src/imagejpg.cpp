// imagejpg.cpp
// read and write jpg images.
//
// Copyright 2012 - 2014 Future Interface. 
// This software is licensed under the terms of the MIT license.
//
// Hongwei Li lihw81@gmail.com
//


#include <stdio.h>
#include <setjmp.h>
#include <assert.h>

extern "C" {
#include <jpeglib.h>
}

#include <common/log.h>


// JPG loading and storing utils 
struct JPGDecodeStruct
{
    jpeg_source_mgr _smgr;          ///< LibJPG source manager
    FILE*           _inputFp;    ///< The input stream.
    JOCTET*         _buffer;                ///< Buffer containing decoded data.
};

// Helper structure used for JPG compressing. 
struct JPGEncodeStruct
{
    jpeg_destination_mgr _dmgr;      ///< LibJPG destination manager. 
    FILE*                _outputFp;   ///< The output stream. 
    JOCTET*              _buffer;                 ///< Buffer containing compressed data. 
};

// JPG error management struct; custom extension stores a setjmp context for returning from exception 
struct JPGErrorManager 
{
    jpeg_error_mgr _pub;    ///< "public" fields 
    jmp_buf _setjmp_buffer; ///< for "try-catch" error handling in C.
};

// Maximum buffer size for JPG compress buffers. 
static const unsigned int _IMAGE_JPG_MAXIMUM_BUFFER_SIZE = 256;

// JPGlib decode callback function. 
static void imageJPGDecodeInitFunction(j_decompress_ptr cinfo)
{
    JPGDecodeStruct* decodeStruct = (JPGDecodeStruct*)cinfo->src;

    decodeStruct->_buffer = (JOCTET*)(*cinfo->mem->alloc_small)((j_common_ptr)(void*)cinfo, 
        JPOOL_IMAGE, _IMAGE_JPG_MAXIMUM_BUFFER_SIZE * sizeof(JOCTET));
    decodeStruct->_smgr.bytes_in_buffer = 0;
}

// JPGlib call back function for filling input buffer while decoding. Lint error for constant parameter suppressed. 
static boolean imageJPGDecodeFillInputBufferFunction(j_decompress_ptr cinfo)
{
    unsigned int bytesRead;
    JPGDecodeStruct* decodeStruct = (JPGDecodeStruct*)cinfo->src;
    
    bytesRead = fread((unsigned char*)decodeStruct->_buffer, _IMAGE_JPG_MAXIMUM_BUFFER_SIZE, 1, decodeStruct->_inputFp);
    decodeStruct->_smgr.next_input_byte = decodeStruct->_buffer;
    decodeStruct->_smgr.bytes_in_buffer = bytesRead;

    return TRUE;
}

// Internal callback function for skipping input data while decoding JPG data. Suppress the usage of long and const. 
static void imageJPGDecodeSkipInputData(j_decompress_ptr cinfo, long num_bytes)
{
    JPGDecodeStruct* decodeStruct = (JPGDecodeStruct*)cinfo->src;

    if (num_bytes > 0)
    {
        /* Skip buffer until done, avoiding that bytes in buffer never gets below zero. */
        while (num_bytes > (long)decodeStruct->_smgr.bytes_in_buffer)
        {
            num_bytes -= (long)decodeStruct->_smgr.bytes_in_buffer;
            imageJPGDecodeFillInputBufferFunction(cinfo); /*lint !e534 Return value ignored. */
        }
    }
    decodeStruct->_smgr.next_input_byte += (size_t)num_bytes;
    decodeStruct->_smgr.bytes_in_buffer -= (size_t)num_bytes;
}

// JPGlib call back function for terminating source while decoding. 
static void imageJPGDecodeTerminateSource(j_decompress_ptr cinfo)
{
}

// JPGlib callback function for initializing destination while compressing. 
static void imageJPGEncodeInitDestination(j_compress_ptr cinfo)
{
    JPGEncodeStruct* encodeStruct = (JPGEncodeStruct*)cinfo->dest;

    encodeStruct->_buffer = (JOCTET*)(*cinfo->mem->alloc_small)((j_common_ptr)(void*)cinfo, 
        JPOOL_IMAGE, _IMAGE_JPG_MAXIMUM_BUFFER_SIZE * sizeof(JOCTET));
    encodeStruct->_dmgr.next_output_byte = (JOCTET*)encodeStruct->_buffer;
    encodeStruct->_dmgr.free_in_buffer = _IMAGE_JPG_MAXIMUM_BUFFER_SIZE;
}

// JPGlib callback function for emptying buffer while compressing. Constant function parameters suppressed from lint. 
static boolean imageJPGEncodeEmptyBuffer(j_compress_ptr cinfo)
{
    JPGEncodeStruct* encodeStruct = (JPGEncodeStruct*)cinfo->dest;
    unsigned int bytesWritten = fwrite((unsigned char*)encodeStruct->_buffer, _IMAGE_JPG_MAXIMUM_BUFFER_SIZE, 1, encodeStruct->_outputFp);

    encodeStruct->_dmgr.next_output_byte = encodeStruct->_buffer;
    encodeStruct->_dmgr.free_in_buffer = _IMAGE_JPG_MAXIMUM_BUFFER_SIZE;

    return TRUE;
}

// JPGlib callback function for terminating destination buffer while compressing. Constant function parameters suppressed from lint. 
static void imageJPGEncodeTermDestination(j_compress_ptr cinfo)
{
    JPGEncodeStruct* encodeStruct = (JPGEncodeStruct*)cinfo->dest;
    unsigned int size = _IMAGE_JPG_MAXIMUM_BUFFER_SIZE - encodeStruct->_dmgr.free_in_buffer;

    /* If there are remaining bytes to be written, write them. */
    if (size > 0)
    {
        unsigned int bytesWritten = fwrite((unsigned char*)encodeStruct->_buffer, 
            size, 1, encodeStruct->_outputFp);
        assert(bytesWritten == size);
    }
}

// Replacement for the standard output_message method 
static void imageJPGErrorMessage(j_common_ptr cinfo)
{
    char buffer[JMSG_LENGTH_MAX];

    /* Show a clear-text error message */
    (*cinfo->err->format_message) (cinfo, buffer);  
    logWarning(buffer);
}


// Kiwi replacement for the standard error_exit method 
static void imageJPGErrorHandler(j_common_ptr cinfo)
{
    /* cinfo->err really points to a my_error_mgr struct, so coerce pointer */
    JPGErrorManager *myerr = (JPGErrorManager*)cinfo->err;

    // Display the message. 
    // TODO: instead of / in addition to logging, pass the message over the 
    //          longjmp so that it can be thrown with the kzsErrorThrow. 
    // (*cinfo->err->output_message) (cinfo); 
    imageJPGErrorMessage(cinfo);

    // Return control to the setjmp point.
    longjmp(myerr->_setjmp_buffer, 1);
}

bool readJPG(FILE* fp, unsigned int& width, unsigned int& height, unsigned int& nchannels, unsigned char*& data)
{
    JPGErrorManager jerr;

    unsigned char** rowPointers   = NULL;
    data                          = NULL;

    // We set up the normal JPG error routines, then override error_exit. 
    jpeg_decompress_struct cinfo;
    cinfo.err                = jpeg_std_error(&jerr._pub);
    jerr._pub.error_exit     = imageJPGErrorHandler;
    jerr._pub.output_message = imageJPGErrorMessage;
    
    // Establish the setjmp return context; "catch block" for "exceptions" during decompression.
    if (setjmp(jerr._setjmp_buffer))
    {
        // If we get here, the JPG code has signaled an error. 
        // Clean up the JPG object, close the input file, and relay the error forward. 
        // TODO: use error message from JPG 
        delete [] rowPointers;
        delete [] data;
        jpeg_destroy_decompress(&cinfo);        
        logError("error while loading a JPG image");
        return false;
    }

    // Create decompression info struct.
    jpeg_create_decompress(&cinfo);

    jpeg_stdio_src(&cinfo, fp);
    
    // Create and initialize the decode structure with function pointers and pass the input stream. */

    // Read the header, may "throw exceptions" 
    unsigned int jpegError = (unsigned int)jpeg_read_header(&cinfo, TRUE);
    if (jpegError != JPEG_HEADER_OK)
    {
        jpeg_destroy_decompress(&cinfo);        
        logError("invalid JPG header");
        return false;
    }
    if (cinfo.out_color_space != JCS_RGB && cinfo.out_color_space != JCS_GRAYSCALE)
    {
        jpeg_destroy_decompress(&cinfo);        
        logError("Unsupported JPG input format");
        return false;
    }

    unsigned int bytesPerPixel = cinfo.out_color_space == JCS_RGB ? 3 : 1;

    // Assign image characteristics. 
    width     = (unsigned int)cinfo.image_width;
    height    = (unsigned int)cinfo.image_height;
    nchannels = bytesPerPixel;

    // Calculate row size, since RGB is assumed using value of 3. 
    unsigned int rowSize = width * bytesPerPixel;

    data = new unsigned char [rowSize * height];
    assert(data != NULL);
    if (data == NULL)
    {
        jpeg_destroy_decompress(&cinfo);        
        logError("new unsigned char [%d] failed", rowSize * height);
        return false;
    }

    // Assign pointers for each row. 
    rowPointers = new unsigned char* [height];
    assert(rowPointers != NULL);
    if (rowPointers == NULL)
    {
        delete [] data;
        jpeg_destroy_decompress(&cinfo);        
        logError("rowPointers = new unsigned char* [%d] failed", height);
        return false;
    }

    // Assign pointers for each row. 
    unsigned char* datap = data;

    for (unsigned int i = 0; i < height; ++i)
    {
        rowPointers[i] = datap;
        datap += rowSize;
    }

    // Prepare jpeg decompression. 
    if (!jpeg_start_decompress(&cinfo))
    {
        delete [] rowPointers;
        delete [] data;
        jpeg_destroy_decompress(&cinfo);        
        logError("Error in decompressing JPG image", height);
        return false;
    }

    // Read scan lines, data assigned via row pointers. 
    while (cinfo.output_scanline < cinfo.output_height)
    {
        int rowsRead = (int)jpeg_read_scanlines(&cinfo, (JSAMPARRAY)&rowPointers[cinfo.output_scanline], 1);
        assert(rowsRead > 0);
        if (rowsRead <= 0)
        {
            delete [] rowPointers;
            delete [] data;
            jpeg_destroy_decompress(&cinfo);        
            logError("No rows were able to be read from JPG image");
            return false;
        }
    }

    delete [] rowPointers;

    // Finish decompressing. 
    if (!jpeg_finish_decompress(&cinfo))
    {
        delete [] data;
        jpeg_destroy_decompress(&cinfo);        
        logError("JPG decompression error in finishing");
        return false;
    }

    // Destroy decompress structure. 
    jpeg_destroy_decompress(&cinfo);

    return true;
}

bool writeJPG(FILE* fp, unsigned int width, unsigned int height, 
                     unsigned int quality, unsigned int nchannels, const unsigned char* data) 
{
    assert(nchannels == 3);
    assert(quality <= 100);

    if (quality > 100)
    {
        quality = 100;
    }

    if (nchannels != 3)
    {
        logError("unsupported JPG format");
        return false;
    } 

    jpeg_error_mgr jerr;

    // TODO: do not use default jpeg error handler since it automatically calls exit() internally upon failure 
    // Set the error manager. 
    jpeg_compress_struct cinfo;
    cinfo.err = jpeg_std_error(&jerr);
    // Create compression structure. 
    jpeg_create_compress(&cinfo);

    // Allocate memory for destination manager. 
    JPGEncodeStruct* encodeStruct = new JPGEncodeStruct;

    // Set helper struct for compression structure. 
    cinfo.dest = (jpeg_destination_mgr*)encodeStruct;

    // Setup write parameters for image. 
    encodeStruct->_outputFp                 = fp;
    encodeStruct->_dmgr.init_destination    = imageJPGEncodeInitDestination;
    encodeStruct->_dmgr.empty_output_buffer = imageJPGEncodeEmptyBuffer;
    encodeStruct->_dmgr.term_destination    = imageJPGEncodeTermDestination;
    
    // Prepare for compression. Ignore alpha channel.
    cinfo.image_width      = (JDIMENSION)width;
    cinfo.image_height     = (JDIMENSION)height;
    cinfo.input_components = 3;
    cinfo.in_color_space   = JCS_RGB;

    // Set JPEG default after the color space has been defined. 
    jpeg_set_defaults(&cinfo);
    jpeg_set_quality(&cinfo, (int)quality, TRUE);
    jpeg_start_compress(&cinfo, 1);
    
    // Write the JPEG scan lines, one by one. 
    JSAMPROW sampleRow[1];
    JDIMENSION rowsWritten;

    while(cinfo.next_scanline < cinfo.image_height)
    {
        unsigned char* imageRow;
        imageRow = (unsigned char*)data + width * 3 * (unsigned int)cinfo.next_scanline;

        sampleRow[0] = (JSAMPROW)&imageRow[0];
        rowsWritten = jpeg_write_scanlines(&cinfo, (JSAMPARRAY)sampleRow, 1);
        if (rowsWritten != 1)
        {
            delete encodeStruct;
            jpeg_destroy_compress(&cinfo);
            logError("invalid amount of rows written when outputting JPG image");
            return false;
        }
    }

    // Finish compression. 
    jpeg_finish_compress(&cinfo);
    // Destroy compression structure. 
    jpeg_destroy_compress(&cinfo);

    // Release helper structure from memory. 
    delete encodeStruct;

    return true;
}


