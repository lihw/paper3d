/*
 * Copyright 2012 - 2014 Future Interface. 
 * This software is licensed under the terms of the MIT license.
 */
package com.fi.tech.loadscene;

import android.content.SharedPreferences;
import android.content.SharedPreferences.OnSharedPreferenceChangeListener;

import com.fi.tech.PGLWallpaperService;

public class Paper3D extends PGLWallpaperService implements OnSharedPreferenceChangeListener
{
    public static final String CONTEXT_NAME = "loadscene";
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
