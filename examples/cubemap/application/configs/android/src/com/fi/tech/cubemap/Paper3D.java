/*
 * Copyright 2012 - 2014 Future Interface. All rights reserved.
 */
package com.fi.tech.cubemap;

import android.content.SharedPreferences;
import android.content.SharedPreferences.OnSharedPreferenceChangeListener;

import com.fi.tech.PGLWallpaperService;

public class Paper3D extends PGLWallpaperService implements OnSharedPreferenceChangeListener
{
    public static final String CONTEXT_NAME = "cubemap";
    private SharedPreferences mPrefs;

    public Paper3D ()
    {
        super();
    }

    @Override
    public void onCreate()
    {
        super.onCreate();
        
        mPrefs = getSharedPreferences(SHARED_PREFS_NAME, 0);
        mPrefs.registerOnSharedPreferenceChangeListener(this);
    }
	
    @Override
    public void onSharedPreferenceChanged(SharedPreferences prefs, String key)
    {
    }
}
