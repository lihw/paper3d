// log.h
// The logging wrappers
//
// Copyright 2012 - 2014 Future Interface. 
// This software is licensed under the terms of the MIT license.
//
// Hongwei Li lihw81@gmail.com
//

#ifndef LOG_H
#define LOG_H

extern void logInfo(const char *fmt, ...);
extern void logWarning(const char *fmt, ...);
extern void logError(const char *fmt, ...);
extern void logFatal(const char *fmt, ...);
extern void logDebug(const char *fmt, ...);
// Setting false will disable the warning and error output. The default is true.
extern void logSetVerbose(bool verbose);


#endif // !LOG_H
