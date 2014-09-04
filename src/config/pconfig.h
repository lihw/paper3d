// pconfig.h
// The global configuration 
//
// Copyright 2012 - 2014 Future Interface. 
// This software is licensed under the terms of the MIT license.
//
// Hongwei Li lihw81@gmail.com
//
// 

#ifndef PCONFIG_H
#define PCONFIG_H

#include <PCore/pstring.h>

#include <PSystem/pcrt.h>
#include <PSystem/pnew.h>


// FIXME: use core/pini.h

// A few things about PConfig
//
// Currently we support entries with single values. The value
// type should be either pint32, puint32, bool, pfloat32 or pchar*.
// More complex or combinational value types, like matrix are
// not supported yet.
//
// To add one more entries to to PConfig, there are two places one
// has to put some code. 
// 1.  Inside the PConfig class declaration, put one more
//    P_DECLARE_CONFIG_ENTRY macro with specific parameters.
// 2. Inside iniHandler_internal in pconfig.cpp, put one more P_FIND_CONFIG_ENTRY
// at the end of the current list. 
//
// There is only one global PConfig instance which can be obtained
// by calling the global function pGetConfig(). Each entry in the PConfig 
// has a corresponding access function. Say we have an config entry called
// maxEventNumber, we can access to this entry by calling pGetConfig()->maxEventNumber().
// For the data type of maxEventNumber, one should refer to pconfig.h where
// maxEventNumber is declared by one of P_DECLARE_CONFIG_ENTRYs.

//P_DECLARE_CONFIG_ENTRY(defaultContextName, pchar*, (pchar*)"default");
// In order to resolve gcc compile warning: deprecated conversion from string 
// constant to 'pchar* {aka char*}' [-Wwrite-strings]. when pass const string ,for example ,"default"
// please  like (pchar*)"default", NOT "default".

#define P_DECLARE_CONFIG_ENTRY(entryName, entryType, entryDefaultValue) \
    struct entryName##_struct : public BaseEntry \
    { \
        entryType value; \
        entryName##_struct() \
        { \
            setDefaultValue(&value, entryDefaultValue);\
        }; \
        ~entryName##_struct() \
        { \
            destroyValue(&value);\
        }; \
    } m_##entryName; \
    P_INLINE entryType entryName() const \
    { \
        return m_##entryName.value; \
    }

struct P_DLLEXPORT PConfig
{
    struct BaseEntry
    {
        P_INLINE void setDefaultValue(puint32* value, puint32 defaultValue) 
        { *value = defaultValue; }
        P_INLINE void setDefaultValue(pint32* value, pint32 defaultValue) 
        { *value = defaultValue; }
        P_INLINE void setDefaultValue(pfloat32* value, pfloat32 defaultValue) 
        { *value = defaultValue; }
        P_INLINE void setDefaultValue(bool* value, bool defaultValue) 
        { *value = defaultValue; }
        P_INLINE void setDefaultValue(pchar** value, pchar* defaultValue) 
        { *value = pstrdup(defaultValue); }
        
        P_INLINE void destroyValue(puint32* value) { }
        P_INLINE void destroyValue(pint32* value) { } 
        P_INLINE void destroyValue(pfloat32* value) { }
        P_INLINE void destroyValue(bool* value) { } 
        P_INLINE void destroyValue(pchar** value) 
        { PDELETEARRAY(*value); *value = P_NULL; } 

        P_INLINE void setValue(puint32* value, const pchar *newValueString) 
        { 
            *value = patoi(newValueString); 
        }
        P_INLINE void setValue(pint32* value, const pchar *newValueString) 
        { 
            *value = patoi(newValueString); 
        }
        P_INLINE void setValue(pfloat32* value, const pchar *newValueString) 
        { 
            *value = patof(newValueString); 
        }
        P_INLINE void setValue(bool* value, const pchar *newValueString) 
        { 
            if (pstrcmp(newValueString, "true") == 0)
            {
                *value = true;
            }
            else
            {
                *value = false; 
            }
        }
        P_INLINE void setValue(pchar** value, const pchar* newValueString) 
        { 
            PDELETEARRAY(*value); 
            *value = pstrdup(newValueString); 
        }
    };

private:
    bool loadFromAsset(const pchar* fileName);
    bool loadFromFile(const pchar* fileName);

public:
    PConfig();
    ~PConfig();

    bool load();

    P_DECLARE_CONFIG_ENTRY(maxContextNumber, puint32, 10);
    P_DECLARE_CONFIG_ENTRY(defaultContextName, pchar*, (pchar*)"default");
    // TODO: add more entries here.
};

#undef P_DELCARE_CONFIG_ENTRY


#endif // !PCONFIG_H
