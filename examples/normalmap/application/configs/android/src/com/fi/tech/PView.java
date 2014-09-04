/*
 * Copyright 2012-2014 Future Interface. All rights reserved.
 * This software is licensed under the terms of the MIT license.
 */
package com.fi.tech;

import android.app.ActivityManager;
import android.app.Activity;
import android.content.Context;

import android.graphics.Rect;
import android.graphics.PixelFormat;
import android.opengl.GLSurfaceView;
import android.os.PowerManager;
import android.util.Log;
import android.view.KeyEvent;
import android.view.MotionEvent;

import javax.microedition.khronos.egl.EGL10;
import javax.microedition.khronos.egl.EGLConfig;
import javax.microedition.khronos.egl.EGLContext;
import javax.microedition.khronos.egl.EGLDisplay;
import javax.microedition.khronos.opengles.GL10;

import java.util.ArrayList;
import java.util.List;

/**
 * TODO: Clean up the file.
 */
public class PView extends GLSurfaceView {
	private static final String TAG = "Paper3D";
	private Activity activity;

	// public static final int ES1 = 1;
	// public static final int ES2 = 2;
	private static final int CURSOR_STATE_DOWN = 0; // < finger down.
	private static final int CURSOR_STATE_STATIONARY = 1; // < finger still.
	private static final int CURSOR_STATE_MOVE = 2; // < finger move.
	private static final int CURSOR_STATE_UP = 3; // < finger up.
	private ArrayList<Runnable> queue = new ArrayList<Runnable>();

	private PContext mContext = null;

	/**
	 * @param activity
	 * @param contextName
	 */
	public PView(Activity activity, String contextName) {
		this(activity, 2, false, contextName);
	}

	/**
	 * @param activity
	 * @param multisamples 
	 * @param translucent
	 * @param contextName
	 */
	public PView(Activity activity, int multisamples, boolean translucent,
			String contextName) {
		super(activity);

		this.activity = activity;

		mContext = new PContext(contextName);

		listServices(activity);

		// Comment out the following line to use GLES 1.1
		setEGLContextFactory(new ContextFactory());

		if (translucent) {
			// By default GLSurfaceView will create a PixelFormat.RGB_565 format
			// surface, override that here.
			getHolder().setFormat(PixelFormat.TRANSLUCENT);
			setZOrderOnTop(true);
		}

		setEGLConfigChooser(new ConfigChooser(multisamples, translucent));
		setRenderer(new Renderer(this));
		// setPreserveEGLContextOnPause(true);
	}

	public void quit() {
		activity.runOnUiThread(new Runnable() {
			public void run() {
				activity.finish();
			}
		});
	}

	public void queueEvent(Runnable runnable) {
		synchronized (this) {
			queue.add(runnable);
		}
	}

	public void handleQueuedEvents() {
		synchronized (this) {
			for (int i = 0; i < queue.size(); ++i) {
				Runnable runnable = queue.get(i);
				super.queueEvent(runnable);
			}
			queue.clear();
		}
	}

	/**
	 * @param context
	 */
	private void listServices(Context context) {
		// ActivityManager am =
		// (ActivityManager)this.getSystemService(ACTIVITY_SERVICE);
		ActivityManager am = (ActivityManager) context
				.getSystemService(context.ACTIVITY_SERVICE);

		List<ActivityManager.RunningServiceInfo> rs = am.getRunningServices(50);

		for (int i = 0; i < rs.size(); i++) {
			ActivityManager.RunningServiceInfo rsi = rs.get(i);
			Log.i(TAG, "Process " + rsi.process + " with component "
					+ rsi.service.getClassName());
		}
	}

	/**
	 * prevent the screen from going to sleep. Set this in constructor of the
	 * view
	 * 
	 * @param context
	 */
	public void setWakeLock(Context context) {
		PowerManager powermanager = (PowerManager) context
				.getSystemService(Context.POWER_SERVICE);
		wl = powermanager.newWakeLock(PowerManager.SCREEN_DIM_WAKE_LOCK,
				"My Tag");
		wl.acquire();
	}

	private PowerManager.WakeLock wl;

	/**
	 * @return
	 */
	public int getScreenWidth() {
		return getWidth();
	}

	/**
	 * @return
	 */
	public int getScreenHeight() {
		return getHeight();
	}

	/**
	 * 
	 * @return
	 */
	public int accelerometerAvailable() {
		return 0;
	}

	/**
	 * @return
	 */
	public float getAccelerationX() {
		return 0;
	}

	/**
	 * @return
	 */
	public float getAccelerationY() {
		return 0;
	}

	/**
	 * @return
	 */
	public float getAccelerationZ() {
		return 0;
	}

	/**
	 * @return
	 */
	public float[] getAcceleration() {
		float[] a = { 0.f, 0.f, 0.f };
		return a;
	}

	/**
     *
     */
	private static class ContextFactory implements
			GLSurfaceView.EGLContextFactory {
		private static int EGL_CONTEXT_CLIENT_VERSION = 0x3098;

		public EGLContext createContext(EGL10 egl, EGLDisplay display,
				EGLConfig eglConfig) {
			Log.i(TAG, "creating OpenGL ES 2.0 context");
			checkEglError("Before eglCreateContext", egl);
			int[] attrib_list = { EGL_CONTEXT_CLIENT_VERSION, 2, EGL10.EGL_NONE };
			EGLContext context = egl.eglCreateContext(display, eglConfig,
					EGL10.EGL_NO_CONTEXT, attrib_list);
			checkEglError("After eglCreateContext", egl);

			// Do not resume or initialize here, cause the context is not set as
			// current. Instead, do that in onSurfaceCreated(),
			// which ensures that the surface is current.

			return context;
		}

		public void destroyContext(EGL10 egl, EGLDisplay display,
				EGLContext context) {
			egl.eglDestroyContext(display, context);
		}
	}

	/**
	 * @param prompt
	 * @param egl
	 */
	private static void checkEglError(String prompt, EGL10 egl) {
		int error;
		while ((error = egl.eglGetError()) != EGL10.EGL_SUCCESS) {
			Log.e(TAG, String.format("%s: EGL error: 0x%x", prompt, error));
		}
	}

	/**
     *
     */
	private static class ConfigChooser implements
			GLSurfaceView.EGLConfigChooser {
		private int[] s_configAttribs;

		/*
		 * public ConfigChooser() { this(2, false); } public ConfigChooser(int
		 * ES_version) { this(ES_version, false); }
		 */

		/**
		 * @param multisamples
		 * @param translucent
		 */
		public ConfigChooser(int multisamples, boolean translucent) {
			int component_size = EGL10.EGL_DONT_CARE;
			if (translucent) {
				// translucent surface needs always 8 bits per component.
				// From Android SDK:
				// The exact format of a TRANSLUCENT surface is device
				// dependent, but it will be a 32-bit-per-pixel surface with 8
				// bits per component.
				component_size = 8;
			}

			if (multisamples > 1) {

				int[] conf = { EGL10.EGL_SAMPLES, multisamples, EGL10.EGL_SAMPLE_BUFFERS,
						1, EGL10.EGL_RED_SIZE, component_size,
						EGL10.EGL_GREEN_SIZE, component_size,
						EGL10.EGL_BLUE_SIZE, component_size,
						EGL10.EGL_DEPTH_SIZE, 16, EGL10.EGL_RENDERABLE_TYPE, 4,
						EGL10.EGL_STENCIL_SIZE, 8, EGL10.EGL_NONE };
				s_configAttribs = conf;

			} else {

				int[] conf = { EGL10.EGL_SAMPLES, 0, EGL10.EGL_SAMPLE_BUFFERS,
						0, EGL10.EGL_RED_SIZE, component_size,
						EGL10.EGL_GREEN_SIZE, component_size,
						EGL10.EGL_BLUE_SIZE, component_size,
						EGL10.EGL_DEPTH_SIZE, 16, EGL10.EGL_RENDERABLE_TYPE, 4,
						EGL10.EGL_STENCIL_SIZE, 8, EGL10.EGL_NONE };
				s_configAttribs = conf;

			}
		}

		/**
		 * @param egl
		 * @param display
		 * @return
		 */
		public EGLConfig chooseConfig(EGL10 egl, EGLDisplay display) {
			int[] num_config = new int[1];
			EGLConfig[] config = new EGLConfig[1];
			egl.eglChooseConfig(display, s_configAttribs, config, 1, num_config);

			return config[0];
		}
	}

	/**
     *
     */
	private class Renderer implements GLSurfaceView.Renderer {
		PView view;

		Renderer(PView fiView) {
			this.view = fiView;
		}

		/**
		 * @param gl
		 */
		public void onDrawFrame(GL10 gl) {
			if (mContext.isInitialized()) {
				view.handleQueuedEvents();

				if (!mContext.update()) {
					Log.i(TAG, "Terminating application now");
					view.quit();
				}
			}
		}

		/**
		 * @param gl
		 * @param width
		 * @param height
		 */
		public void onSurfaceChanged(GL10 gl, int width, int height) {
			Log.v(TAG, "SurfaceChanged (GLSurfaceView.Renderer)");

			if (mContext.isInitialized()) {
				mContext.resize(width, height);
				if (width > height) {
					mContext.orientationChange(PContext.SCREEN_ORIENTATION_LANDSCAPE);
				} else {
					mContext.orientationChange(PContext.SCREEN_ORIENTATION_PORTRAIT);
				}
			}
		}

		/**
		 * @param gl
		 * @param config
		 */
		public void onSurfaceCreated(GL10 gl, EGLConfig config) {
			Log.v(TAG, "SurfaceCreated (GLSurfaceView.Renderer)");

			if (!mContext.isInitialized()) {
				mContext.initialize(getScreenWidth(), getScreenHeight());
			} else if (mContext.isPaused()) {
				mContext.resume();
			}
		}
	}

	@Override
	public void onResume() {
		super.onResume();
	}

	@Override
	public void onPause() {
		// FIXME: We should call GLSurfaceView.onPause() before call
		// PContext.pause(), because
		// GLThread is still running now. If we don't pause GLThread,
		// PContext::update and PContext::pause
		// will cross-run and crash randomly.
		// Notice that GLSurfaceView.onPause() do not pause GLThread immediately
		// on Android 4.3.
		// And actually, we should not make GL calls here.
		// Leave these comments so that we can fix it later.
		queueEvent(new Runnable() {
			public void run() {
				if (mContext.isInitialized() && !mContext.isPaused()) {
					// Pause application here, we're getting called from a right
					// thread.
					// Note that there's no context set as current, so we can't
					// make GL calls.
					mContext.pause();
					activity.runOnUiThread(new Runnable() {
						public void run() {
							pause();
						}
					});
				}
			}
		});
	}

	private void pause() {
		super.onPause();
	}

	public void onDestroy() {
		mContext.deinitialize();
	}

	/**
	 * @param gainFocus
	 * @param direction
	 * @param previouslyFocusedRect
	 */
	@Override
	protected void onFocusChanged(boolean gainFocus, int direction,
			Rect previouslyFocusedRect) {
		if (mContext.isInitialized()) {
			mContext.focusEvent(gainFocus);
		}

		super.onFocusChanged(gainFocus, direction, previouslyFocusedRect);
	}

	/**
	 * @param keyCode
	 * @param event
	 * @return
	 */
	@Override
	public boolean onKeyDown(final int keyCode, KeyEvent event) {
		onKeyEvent(keyCode, event);

		return super.onKeyDown(keyCode, event);
	}

	/**
	 * @param keyCode
	 * @param event
	 * @return
	 */
	@Override
	public boolean onKeyUp(final int keyCode, KeyEvent event) {
		onKeyEvent(keyCode, event);

		return super.onKeyUp(keyCode, event);
	}

	/**
	 * Internal key handler, same for up and down.
	 * 
	 * @param keyCode
	 * @param event
	 */
	private void onKeyEvent(final int keyCode, KeyEvent event) {
		final int touchState;
		final long timeStamp = event.getEventTime();
		switch (event.getAction()) {
		case KeyEvent.ACTION_DOWN:
			touchState = 0;
			break;
		case KeyEvent.ACTION_MULTIPLE:
			touchState = 2;
			break;
		default:
		case KeyEvent.ACTION_UP:
			touchState = 1;
			break;
		}
		// The touch event is queued and will be forwarded to P before the next
		// update
		queueEvent(new Runnable() {
			public void run() {
				mContext.keyEvent(keyCode, touchState, timeStamp);
			}
		});
	}

	/**
	 * @param event
	 * @return
	 */
	@Override
	public boolean onTrackballEvent(MotionEvent event) {
		// TODO: send keyEvents to P

		return super.onTrackballEvent(event);
	}

	/**
	 * @param event
	 * @return
	 */
	@Override
	public boolean onTouchEvent(final MotionEvent originalEvent) {
		final MotionEvent event = MotionEvent.obtain(originalEvent);
		final int pointerCount = event.getPointerCount();
		final int cursorIndex;
		final int cursorState;
		final int state;

		int cursorIndexLocal = -1; /* Match all pointers. */
		int action = event.getAction() & MotionEvent.ACTION_MASK;
		;
		int pointerIndex = (event.getAction() & MotionEvent.ACTION_POINTER_ID_MASK) >> MotionEvent.ACTION_POINTER_ID_SHIFT;

		if (action == MotionEvent.ACTION_DOWN) {
			cursorState = CURSOR_STATE_DOWN;
			state = 0;
		} else if (action == MotionEvent.ACTION_MOVE) {
			cursorState = CURSOR_STATE_MOVE;
			state = 1;
		} else if ((action & MotionEvent.ACTION_POINTER_DOWN) == MotionEvent.ACTION_POINTER_DOWN) {
			cursorState = CURSOR_STATE_DOWN;
			// cursorIndexLocal = event.findPointerIndex(action &
			// MotionEvent.ACTION_POINTER_ID_MASK);
			cursorIndexLocal = pointerIndex;
			state = 1;
		} else if ((action & MotionEvent.ACTION_POINTER_UP) == MotionEvent.ACTION_POINTER_UP) {
			cursorState = CURSOR_STATE_UP;
			// cursorIndexLocal = event.findPointerIndex(action &
			// MotionEvent.ACTION_POINTER_ID_MASK);
			cursorIndexLocal = pointerIndex;
			state = 1;
		} else /* Default. */
		{
			cursorState = CURSOR_STATE_UP;
			state = 2;
		}

		cursorIndex = cursorIndexLocal;

		// The touch event is queued and will be forwarded to engine before the
		// next update
		queueEvent(new Runnable() {
			public void run() {
				long touchEvent = mContext.touchEvent(pointerCount, state);

				for (int i = 0; (i < pointerCount); ++i) {
					int touchState;

					if ((cursorIndex < 0) || (cursorIndex == i)) {
						touchState = cursorState;
					} else {
						touchState = CURSOR_STATE_STATIONARY;
					}
					mContext.touchCursor(touchEvent, i, event.getPointerId(i),
							event.getX(i), event.getY(i), event.getPressure(i),
							event.getSize(i), touchState, event.getEventTime());
				}

				event.recycle();
			}
		});

		return true;
	}
}
