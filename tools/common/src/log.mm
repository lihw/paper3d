// log.mm
// iOS debug output 
//
// Copyright 2012 - 2014 Future Interface. 
// This software is licensed under the terms of the MIT license.
//
// Hongwei Li lihw81@gmail.com
//

#import <Foundation/foundation.h>

void debugOutput(const char *text)
{
    // FIXME: may cause memory leak here.
    NSLog([NSString stringWithUTF8String:text]);  
}
