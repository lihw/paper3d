/*
 * Copyright 2012-2014 by Future Interface. All rights reserved.
 *
 */
package com.fi.tech.background;

import com.fi.tech.background.R;

import android.content.SharedPreferences;
import android.os.Bundle;
import android.preference.PreferenceActivity;
import android.util.Log;

public class Paper3DSettings extends PreferenceActivity
    implements SharedPreferences.OnSharedPreferenceChangeListener 
{

    @Override
    protected void onCreate(Bundle icicle) 
    {
        super.onCreate(icicle);
        Log.i("Paper3D", "**********************************");
        getPreferenceManager().setSharedPreferencesName("BackgroundSettings");
        addPreferencesFromResource(R.xml.settings);
        getPreferenceManager().getSharedPreferences().registerOnSharedPreferenceChangeListener(this);
    }

    @Override
    protected void onResume() 
    {
        super.onResume();
    }

    @Override
    protected void onDestroy() 
    {
        getPreferenceManager().getSharedPreferences().unregisterOnSharedPreferenceChangeListener(this);
        super.onDestroy();
    }

    public void onSharedPreferenceChanged(SharedPreferences sharedPreferences, String key) 
    {
    }
}
