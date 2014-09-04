/*
 * Copyright 2012 - 2014 by Future Interface. 
 * This software is licensed under the terms of the MIT license.
 */

package com.fi.tech;

import java.util.Calendar;
import java.util.GregorianCalendar;

import android.os.Debug;
import android.util.Log;

public abstract class PGLWallpaperService extends GLWallpaperService 
{
    public static final boolean DEBUG = true;
    public String PACKAGE_NAME;

    public PGLWallpaperService() 
    {
        super();
    }

    public void onCreate() 
    {
        Log.i("Paper3D", "onCreate()");
        PACKAGE_NAME = super.getPackageName();
        //if (DEBUG) 
        //{
        //    Debug.startMethodTracing("paper3d");
        //}

        super.onCreate();

        PNativeLibrary nativeLibrary = PNativeLibrary.getInstance();
        nativeLibrary.initialize(this);
        if (!nativeLibrary.isInitialized())
        {
            Log.e("Paper3D", "Failed to initialize native library. Quitting.");
            stopSelf();
        }
    }

    public void onDestroy() 
    {
        Log.i("Paper3D", "onDestroy()");

        super.onDestroy();

        //if (DEBUG) 
        //{
        //    Debug.stopMethodTracing();
        //}
        
        PNativeLibrary.getInstance().uninitialize();
    }

    public int getDay() 
    {
        final Calendar c = GregorianCalendar.getInstance();
        return c.get(Calendar.DAY_OF_WEEK);
    }

    public int getDateDay() 
    {
        final Calendar c = GregorianCalendar.getInstance();
        return c.get(Calendar.DAY_OF_MONTH);
    }

    public int getDateMonth() 
    {
        final Calendar c = GregorianCalendar.getInstance();
        return c.get(Calendar.MONTH);
    }

    public int getDateYear() 
    {
        final Calendar c = GregorianCalendar.getInstance();
        return c.get(Calendar.YEAR);
    }

    @Override
    public Engine onCreateEngine() 
    {
        Log.i("Paper3D", "onCreateEngine()");
        PEngine engine = new PEngine();
        return engine;
    }

    class PEngine extends GLEngine 
    {
        public PEngine() 
        {
            super();
        }
    }
}
