/*
 * Copyright 2012-2014 Future Interface. All rights reserved.
 * This software is licensed under the terms of the MIT license.
 * 
 */
package com.fi.tech;

import java.util.Calendar;
import java.util.GregorianCalendar;

import android.app.Activity;
import android.content.Context;
import android.os.Bundle;
import android.os.Debug;
import android.os.PowerManager;
import android.util.Log;
import android.view.ViewGroup;

/**
 * Implement your own activity so that it extends PActivity.
 */
public class PActivity extends Activity {
	private static final String TAG = "Paper3D";
	private static final String CONTEXT_NAME = "network";

	public String PACKAGE_NAME;

	static final int NUM_CONTEXTS = 16;
	public int deviceWidth = 0;
	public int deviceHeight = 0;

	protected PView mView = null;
	protected PowerManager.WakeLock wl;
	protected PNativeLibrary mNativeLibrary = null; // FIXME: to be shared among
							// activities.

	/**
	 * @param icicle
	 *            mapping to various Parcelable types
	 */
	@Override
	protected void onCreate(Bundle icicle) {
		PACKAGE_NAME = super.getPackageName();
		if (PContext.DEBUG) {
			Debug.startMethodTracing(TAG.toLowerCase());
		}
		super.onCreate(icicle);
		mNativeLibrary = PNativeLibrary.getInstance();
		mNativeLibrary.initialize(this);
		if (mNativeLibrary.getState() == PNativeLibrary.STATE_INITIALIZED) {
			// The view name should be as same as the
			// protoss's context name.
			// For each context, we should create a view.
			setView(getView(CONTEXT_NAME));

			Log.v(TAG, "setting wakelock");
			PowerManager pm = (PowerManager) getSystemService(Context.POWER_SERVICE);
			wl = pm.newWakeLock(PowerManager.FULL_WAKE_LOCK, TAG);

			/* Enable key events. */
			mView.setFocusableInTouchMode(true);
		} else {
			Log.e(TAG,
					"Failed to load/initialize protoss native library. Quitting.");
			finish();
		}
	}

	@Override
	protected void onDestroy() {
		super.onDestroy();
		Log.i(TAG, "onDestroy");
		if (PContext.DEBUG) {
			Debug.stopMethodTracing();
		}

		mView.onDestroy();
	}

	/**
	 * @return reference to current PView
	 */
	protected PView getView(String contextName) {
		if (mView == null) {
			mView = new PView(this, contextName);
		}
		return mView;
	}

	/**
	 * Attach view to parent and notify Protoss native side that the view
	 * reference has been updated.
	 * 
	 * @param view
	 */
	protected void setView(PView view) {
		if (view != null) {
			mView = view; // store the view
			addContentView(mView, new ViewGroup.LayoutParams(
					ViewGroup.LayoutParams.MATCH_PARENT,
					ViewGroup.LayoutParams.MATCH_PARENT));
		}
	}

	/**
	 * @see android.app.Activity#onStop()
	 */
	@Override
	protected void onStop() {
		super.onStop();
		Log.i(TAG, "onStop");
		if (PContext.DEBUG) {
			Debug.stopMethodTracing();
		}
	}

	/**
	 * @see android.app.Activity#onPause()
	 */
	@Override
	protected void onPause() {
		super.onPause();
		Log.i(TAG, "onPause");
		mView.onPause();
	}

	/**
	 * @see android.app.Activity#onResume()
	 */
	@Override
	protected void onResume() {
		super.onResume();
		Log.i(TAG, "onResume");
		mView.onResume();
	}

	public int getDeviceHeight() {
		if (deviceHeight != 0) {
			// Log.e("height", "1:"+height);
			return deviceHeight;
		}
		deviceHeight = getResources().getDisplayMetrics().heightPixels;
		return deviceHeight;
	}

	public int getDeviceWidth() {
		if (deviceWidth != 0)
			return deviceWidth;
		deviceWidth = getResources().getDisplayMetrics().widthPixels;
		return deviceWidth;
	}

	public int getDay() {
		final Calendar c = GregorianCalendar.getInstance();
		return c.get(Calendar.DAY_OF_WEEK);
	}

	public int getDateDay() {
		final Calendar c = GregorianCalendar.getInstance();
		return c.get(Calendar.DAY_OF_MONTH);
	}

	public int getDateMonth() {
		final Calendar c = GregorianCalendar.getInstance();
		return c.get(Calendar.MONTH);
	}

	public int getDateYear() {
		final Calendar c = GregorianCalendar.getInstance();
		return c.get(Calendar.YEAR);
	}

}
