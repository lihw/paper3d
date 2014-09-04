/*
 * Copyright 2012 - 2014 Future Interface. All rights reserved.
 * This software is licensed under the terms of the MIT license.
 */
package com.fi.tech;

/**
 *
 */
public class PContext {
	private static final String TAG = "PContext";

	/* Library state is needed for tracking pause/resume behavior */
	private static final int STATE_IDLE = 0;
	private static final int STATE_STARTING = 1;
	private static final int STATE_RUNNING = 2;
	private static final int STATE_PAUSING = 3;
	private static final int STATE_PAUSED = 4;
	private static final int STATE_RESUMING = 5;
	private static final int STATE_STOPPING = 6;

	private int mState;
	private int mContextId;
	private String mContextName;

	public static final boolean DEBUG = false;
	// Below enum value is to indicate the orientation value (Ref
	// http://developer.android.com/reference/android/content/pm/ActivityInfo.html#SCREEN_ORIENTATION_LANDSCAPE
	// )
	public static final int SCREEN_ORIENTATION_LANDSCAPE = 0;
	public static final int SCREEN_ORIENTATION_PORTRAIT = 1;

	private static native void createContextNative(String contextName);

	private static native int getContextIdByNameNative(String contextName);

	private static native void uninitializeContextNative(int contextId);

	private static native void pauseNative(int contextId);

	private static native void resumeNative(int contextId);

	private static native boolean updateNative(int contextId);

	private static native void resizeNative(int contextId, int width, int height);

	private static native void orientationChangeNative(int contextId,
			int orientation);

	private static native void initializeContextNative(int contextId,
			int width, int height);

	public PContext(String contextName) {
		mState = STATE_STARTING;
		mContextName = contextName;
		createContextNative(mContextName);
	}

	public void initialize(int width, int height) {
		mState = STATE_STARTING;
		mContextId = getContextIdByNameNative(mContextName);
		initializeContextNative(mContextId, width, height);
		mState = STATE_RUNNING;
	}

	public void deinitialize() {
		mState = STATE_STOPPING;
		uninitializeContextNative(mContextId);
		mState = STATE_IDLE;
		mContextId = -1;
	}

	public boolean isInitialized() {
		return ((mState == STATE_RUNNING) || (mState == STATE_PAUSED)
				|| (mState == STATE_PAUSING) || (mState == STATE_RESUMING));
	}

	public boolean isPaused() {
		return (mState == STATE_PAUSED);
	}

	public void pause() {
		mState = STATE_PAUSING;
		pauseNative(mContextId);
		mState = STATE_PAUSED;
	}

	public void resume() {
		mState = STATE_RESUMING;
		resumeNative(mContextId);
		mState = STATE_RUNNING;
	}

	/**
	 * set the orientation . use PNativeLibrary.SCREEN_ORIENTATION_LANDSCAPE or
	 * PNativeLibrary.SCREEN_ORIENTATION_PORTRAIT
	 * 
	 * @param orientation
	 */
	public void orientationChange(int orientation) {
		orientationChangeNative(mContextId, orientation);
	}

	public void resize(int width, int height) {
		resizeNative(mContextId, width, height);
	}

	public boolean update() {
		if (!updateNative(mContextId)) {
			mState = STATE_STOPPING;
			return false;
		}
		return true;
	}

	public int getState() {
		return mState;
	}

	/**
	 * @param touchEvent
	 * @param index
	 * @param id
	 * @param x
	 * @param y
	 * @param pressure
	 * @param area
	 * @param state
	 */
	public long touchEvent(int cursorCount, int state) {
		return touchEventNative(mContextId, cursorCount, state);
	}

	/**
	 * @param touchEvent
	 * @param index
	 * @param id
	 * @param x
	 * @param y
	 * @param pressure
	 * @param area
	 * @param state
	 */
	public void touchCursor(long touchEvent, int index, int id, float x,
			float y, float pressure, float area, int state, long timeStamp) {
		touchCursorNative(mContextId, touchEvent, index, id, x, y, pressure,
				area, state, timeStamp);
	}

	/**
	 * @param focusState
	 */
	public void focusEvent(boolean focusState) {
		focusEventNative(mContextId, focusState);
	}

	/**
	 * @param buttonCode
	 * @param state
	 */
	public void keyEvent(int buttonCode, int state, long timeStamp) {
		keyEventNative(mContextId, buttonCode, state, timeStamp);
	}

	/**
	 * debug function: switch Head-Up Display on or off
	 * 
	 * @param state
	 */
	public void setHUD(boolean state) {
		setHUDNative(mContextId, state);
	}

	/**
	 * debug function: switch free camera movement on or off
	 * 
	 * @param state
	 */
	public void setFreeCamera(boolean state) {
		setFreeCameraNative(mContextId, state);
	}

	public static native long touchEventNative(int contextId, int cursorCount,
			int state);

	public static native void touchCursorNative(int contextId, long touchEvent,
			int index, int id, float x, float y, float pressure, float area,
			int state, long timeStamp);

	public static native void focusEventNative(int contextId, boolean focusState);

	public static native void keyEventNative(int contextId, int buttonCode,
			int state, long timeStamp);

	public static native void setHUDNative(int contextId, boolean state);

	public static native void setFreeCameraNative(int contextId, boolean state);
}
