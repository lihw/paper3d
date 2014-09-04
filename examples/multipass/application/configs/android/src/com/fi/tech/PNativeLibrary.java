/*
 * Copyright 2012 - 2014 Future Interface. 
 * This software is licensed under the terms of the MIT license.
 */
package com.fi.tech;

import android.content.Context;

public class PNativeLibrary 
{
    public static final int STATE_LOADED = 0;
    public static final int STATE_UNINITIALIZED = 1;
    public static final int STATE_INITIALIZED = 2;
    public static final int STATE_ERROR = 3;

    private int mState;

    private static PNativeLibrary mInstance = null;

    private static native boolean initializeLibraryNative(Context context);

    public static PNativeLibrary getInstance() 
    {
        if (mInstance == null) 
        {
            mInstance = new PNativeLibrary();
        }
        return mInstance;
    }

    private PNativeLibrary() 
    {
        System.loadLibrary("fi");
        mState = STATE_LOADED;
    }

    public void initialize(Context context) 
    {
        if (mState != STATE_INITIALIZED) 
        {
            if (initializeLibraryNative(context)) 
            {
                mState = STATE_INITIALIZED;
            } 
            else 
            {
                mState = STATE_UNINITIALIZED;
            }
        }
    }

    public void uninitialize() 
    {
        // Called by native code.
        mState = STATE_UNINITIALIZED;
    }

    public int getState() 
    {
        return mState;
    }

    public boolean isInitialized()
    {
        return mState == STATE_INITIALIZED;
    }

    // FIXME: add unload mechanism to this native library.
    // http://codethesis.com/sites/default/index.php?servlet=4&content=2
}
