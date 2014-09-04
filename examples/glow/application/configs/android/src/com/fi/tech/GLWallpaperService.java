/*
 * Copyright 2012 - 2014 Future Interface
 * This software is licensed under the terms of the MIT license.
 *
 */
/*
 * Copyright (C) 2008 The Android Open Source Project
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *      http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */


package com.fi.tech;

import java.util.ArrayList;
import java.util.HashMap;
import java.util.Iterator;

import javax.microedition.khronos.egl.EGL10;
import javax.microedition.khronos.egl.EGL11;
import javax.microedition.khronos.egl.EGLConfig;
import javax.microedition.khronos.egl.EGLContext;
import javax.microedition.khronos.egl.EGLDisplay;
import javax.microedition.khronos.egl.EGLSurface;
import javax.microedition.khronos.opengles.GL10;

import com.fi.tech.GLWallpaperService.GLEngine;
import com.fi.tech.glow.Paper3D;

import android.service.wallpaper.WallpaperService;
import android.util.Log;
import android.view.MotionEvent;
import android.view.SurfaceHolder;

// Original code provided by Robert Green
// http://www.rbgrn.net/content/354-glsurfaceview-adapted-3d-live-wallpapers
public class GLWallpaperService extends WallpaperService 
{
    public static final String SHARED_PREFS_NAME = "GLWallpaperSettings";

    protected static int mEngineCounter = 0;
    public GLThread mThread = null;
        
    static public EGLConfigChooser mEGLConfigChooser = new ConfigChooser(false);
    static public EGLContextFactory mEGLContextFactory = new DefaultContextFactory();
    static public EGLWindowSurfaceFactory mEGLWindowSurfaceFactory = new DefaultWindowSurfaceFactory();
            
    @Override
    public Engine onCreateEngine() 
    {
        return new GLEngine();
    }

    // FIXME: Better to move this function out of GLWallpaperService and put it with other GL helper functions.
    public static void checkEglError(String prompt, EGL10 egl) 
    {
        int error;
        while ((error = egl.eglGetError()) != EGL10.EGL_SUCCESS) 
        {
            Log.e("Paper3D", String.format("%s: EGL error: 0x%x", prompt, error));
        }
    }

    public class GLEngine extends Engine
    {
        private int mEngineID;
        private PContext mContext;

        @Override
        public void onTouchEvent(final MotionEvent event) 
        {
            final int pointerCount = event.getPointerCount();
            final int cursorIndex;
            final int cursorState;
            final int state;

            int cursorIndexLocal = -1; /* Match all pointers. */
            int action = event.getAction();

            if (action == MotionEvent.ACTION_DOWN) 
            {
                cursorState = 0;
                state = 0;
            } 
            else if (action == MotionEvent.ACTION_MOVE) 
            {
                cursorState = 2;
                state = 1;
            } 
            else if ((action & MotionEvent.ACTION_POINTER_DOWN) == MotionEvent.ACTION_POINTER_DOWN) 
            {
                cursorState = 0;
                cursorIndexLocal = event.findPointerIndex(action & MotionEvent.ACTION_POINTER_ID_MASK);
                state = 1;
            } 
            else if ((action & MotionEvent.ACTION_POINTER_UP) == MotionEvent.ACTION_POINTER_UP) 
            {
                cursorState = 3;
                cursorIndexLocal = event.findPointerIndex(action & MotionEvent.ACTION_POINTER_ID_MASK);
                state = 1;
            } 
            else 
            {
                cursorState = 3;
                state = 2;
            }

            cursorIndex = cursorIndexLocal;

            queueEvent(new Runnable() 
            {
                public void run() {
                    if (mContext != null)
                    {
                        long touchEvent = mContext.touchEvent(pointerCount, state);
                        for (int i = 0; i < pointerCount; ++i) 
                        {
                            int touchState = 1;
                            if ((cursorIndex < 0) || (cursorIndex == i)) 
                            {
                                touchState = cursorState;
                            }

                            mContext.touchCursor(
                                touchEvent, 
                                i,
                                event.getPointerId(i), 
                                event.getX(i),
                                event.getY(i), 
                                event.getPressure(i),
                                event.getSize(i), 
                                touchState,
                                event.getEventTime());
                        }
                    }
                }
            });

            try 
            {
                Thread.sleep(1);
            } 
            catch (InterruptedException e) {
            }

            super.onTouchEvent(event);
        }
        
        public GLEngine() 
        {
            super();
            mEngineID = ++mEngineCounter;
            
            // The thread is created if it has not. There is only one thread inside
            // GLWallpaperService instance.
            if (mThread == null) 
            {
                mThread = new GLThread();
                mThread.start();
            }

            mContext = null;
        }

        @Override
        public void onVisibilityChanged(boolean visible) 
        {
            if (visible) 
            {
                onResume();
            } 
            else 
            {
                onPause();
            }
            super.onVisibilityChanged(visible);
        }

        @Override
        public void onCreate(SurfaceHolder surfaceHolder) 
        {
            super.onCreate(surfaceHolder);
            Log.d("GLEngine"+mEngineID, "GLEngine.onCreate()");
            setTouchEventsEnabled(true);
        }

        @Override
        public void onDestroy() 
        {
            Log.d("GLEngine"+mEngineID, "GLEngine.onDestroy()");
            super.onDestroy();
        }

        @Override
        public void onSurfaceChanged(SurfaceHolder holder, int format, int width, int height) 
        {
            Log.d("GLEngine"+mEngineID, "onSurfaceChanged()");
            mThread.onWindowResize(this, holder, width, height);
            super.onSurfaceChanged(holder, format, width, height);
        }

        @Override
        public void onSurfaceCreated(SurfaceHolder holder) 
        {
            Log.d("GLEngine"+mEngineID, "onSurfaceCreated()");
            mThread.onSurfaceCreated(holder, this);
            super.onSurfaceCreated(holder);
        }

        @Override
        public void onSurfaceDestroyed(SurfaceHolder holder) 
        {
            Log.d("GLEngine"+mEngineID, "onSurfaceDestroyed()");
            super.onSurfaceDestroyed(holder);
        }

        public void onPause() 
        {
            Log.i("Paper3D", "GLEngine(" + mEngineID + "): onPause()");
            mThread.onPause(this);
        }

        public void onResume() 
        {
            Log.i("Paper3D", "GLEngine(" + mEngineID + "): onResume()");
            mThread.onResume(this);
        }

        public void queueEvent(Runnable r) 
        {
            mThread.queueEvent(r);
        }

        public void setContext(PContext context)
        {
            mContext = context;
        }
    }
}

class EngineRenderContext
{
    private static EGL10        mEgl;
    private static EGLDisplay   mEglDisplay;
    private static EGLConfig    mEglConfig;
    private static EGLContext   mEglContext;
    private static PContext     mContext;
    private EGLSurface          mEglSurface;
    private SurfaceHolder       mSurf;

    private int                 mWidth;
    private int                 mHeight;

    private boolean             mActive;
    private boolean             mPaused;
    private boolean             mSurfaceChanged;
    private boolean             mSizeChanged;

    public static void start()
    {
        Log.i("Paper3D", "EngineRenderContext: start()");

        if (mEglDisplay == null || mEgl == null || mEglContext == null)
        {
            mEgl = (EGL10) EGLContext.getEGL();
            mEglDisplay = mEgl.eglGetDisplay(EGL10.EGL_DEFAULT_DISPLAY);

            if (mEglConfig == null) 
            {
                int[] version = new int[2];
                mEgl.eglInitialize(mEglDisplay, version);
                mEglConfig = GLWallpaperService.mEGLConfigChooser.chooseConfig(mEgl, mEglDisplay);
            }
            mEglContext = GLWallpaperService.mEGLContextFactory.createContext(mEgl, mEglDisplay, mEglConfig);
            if (mEglContext == null || mEglContext == EGL10.EGL_NO_CONTEXT) 
            {
                throw new RuntimeException("Failed to create a GL context.");
            }

            mContext = new PContext(Paper3D.CONTEXT_NAME);
        }
        else
        {
            Log.e("Paper3D", "EngineRenderContext: Start multiple times.");
        }
    }

    public static void finish()
    {   
        Log.i("Paper3D", "EngineRenderContext::finish()");

        if (mContext != null)
        {
            mContext.uninitialize();
            mContext = null;
        }

        if (mEglContext != null)
        {
            GLWallpaperService.mEGLContextFactory.destroyContext(mEgl, mEglDisplay, mEglContext);    
        }

        if (mEglDisplay != null) 
        {
            mEgl.eglTerminate(mEglDisplay);
        }
            
        mEglDisplay = null;
        mEgl        = null;
        mEglConfig  = null;
        mEglContext = null;
    }

    public EngineRenderContext()
    {
        mEglSurface     = null;
        mSurf           = null;
        mActive         = false;
        mPaused         = false;
        mSurfaceChanged = false;
        mSizeChanged    = false;
        mWidth          = 0;
        mHeight         = 0;
    }

    public void changeSurface(SurfaceHolder holder, int width, int height)
    {
        if (mActive)
        {
            throw new RuntimeException("Cannot change surface when render is activity");
        }

        if (holder != mSurf)
        {
            mSurf = holder;
            mSurfaceChanged = true;
        }

        if (mWidth != width || mHeight != height)
        {
            mWidth = width;
            mHeight = height;
            mSizeChanged = true;
        }

        destroySurface();
    }

    public void activate()
    {    
        if (mActive)
        {
            throw new RuntimeException("Activate called when active.");
        }

        if (mEglContext == null)
        {
            throw new RuntimeException("No EGL context is available.");
        }

        if (mSurf == null)
        {
            throw new RuntimeException("No native surface is available.");
        }

        // Delay to create context/surface
        if (mEglSurface == null)
        {
            createSurface();
        }
        mSurfaceChanged = true;

        if (!mEgl.eglMakeCurrent(mEglDisplay, mEglSurface, mEglSurface, mEglContext))
        {
            throw new RuntimeException("eglMakeCurrent failed.");
        }

        mActive = true;
    }

    public void deactivate()
    {
        if (!mActive)
        {
            throw new RuntimeException("Deactive called when inactive.");
        }

        if (!mEgl.eglMakeCurrent(mEglDisplay, EGL10.EGL_NO_SURFACE, EGL10.EGL_NO_SURFACE, EGL10.EGL_NO_CONTEXT))
        {
            throw new RuntimeException("eglMakeCurrent failed.");
        }

        mActive = false;
    }

    private void createSurface()
    {
        if (mActive)
        {
            throw new RuntimeException("createSurface() called when active.");
        }

        if (mEglSurface != null)
        {
            throw new RuntimeException("createSurface() called when mEglSurface is not null.");
        }

        mEglSurface = GLWallpaperService.mEGLWindowSurfaceFactory.createWindowSurface(mEgl, mEglDisplay, mEglConfig, mSurf);
        if (mEglSurface == null || mEglSurface == EGL10.EGL_NO_SURFACE) 
        {
            throw new RuntimeException("createWindowSurface failed");
        }
    }

    private void destroySurface()
    {
        if (mActive)
        {
            throw new RuntimeException("destroySurface() called when active.");
        }

        if (mEglSurface != null)
        {

            GLWallpaperService.mEGLWindowSurfaceFactory.destroySurface(mEgl, mEglDisplay, mEglSurface);
            mEglSurface = null;
        }
    }

    public void pause()
    {
        mPaused = true;
    }

    public void resume()
    {
        mPaused = false;
    }

    public boolean swap()
    {
        if (!mActive)
        {
            new RuntimeException("swap() called but inactive.");
        }

        if (!available())
        {
            new RuntimeException("swap() called but inavailable.");
        }

        mEgl.eglSwapBuffers(mEglDisplay, mEglSurface);
        return mEgl.eglGetError() != EGL11.EGL_CONTEXT_LOST;
    }

    public boolean active()
    {
        return mActive;
    }

    public boolean available()
    {
        return mSurf != null && !mPaused;
    }
                        
    public void onSurfaceChanged(GL10 gl)
    {
        if (!mContext.isInitialized())
        {
            if (!mContext.initialize(mWidth, mHeight))
            {
                mContext = null;
                throw new RuntimeException("Failed to create a Paper3D context.");
            }
        }
        mContext.resize(mWidth, mHeight);
    }
                    
    public void onDrawFrame(GL10 gl)
    {
        mContext.update();
    }

    public boolean popSizeChanged()
    {
        boolean ret = mSizeChanged;
        mSizeChanged = false;
        return ret;
    }

    public boolean popSurfaceChanged()
    {
        boolean ret = mSurfaceChanged;
        mSurfaceChanged = false;
        return ret;
    }

    public boolean popSurfaceOrSizeChanged()
    {
        boolean ret = mSizeChanged || mSurfaceChanged;
        popSizeChanged();
        popSurfaceChanged();

        return ret;
    }

    public int surfaceWidth() 
    {
        return mWidth;
    }

    public int surfaceHeight() 
    {
        return mHeight;
    }

    public PContext getContext()
    {
        return mContext;
    }
}

class GLThread extends Thread 
{
    private ArrayList<Runnable> mEventQueue = new ArrayList<Runnable>();
    private ArrayList<Runnable> mCommandQueue = new ArrayList<Runnable>();
    private static int MAX_EVENT_COUNT_PER_UPDATE = 100;
    
    private HashMap<GLEngine, EngineRenderContext> renderContexts = new HashMap<GLEngine, EngineRenderContext>();
    private EngineRenderContext activeRenderContext = null;
    
    private final GLThreadManager sGLThreadManager = new GLThreadManager();
    private GLThread mEglOwner;

    // Once the thread is started, all accesses to the following member
    // variables are protected by the sGLThreadManager monitor
    public boolean mDone;
    private boolean mEventsWaiting;
    private boolean mCommandsWaiting;

    GLThread()
    {
        super();
        mDone = false;
    }
    
    @Override
    public void run() 
    {
        setName("GLThread " + getId());

        try 
        {
            guardedRun();
        } 
        catch (InterruptedException e) 
        {
            // fall thru and exit normally
        } 
    }

    private void guardedRun() throws InterruptedException 
    {
        EngineRenderContext.start();

        try 
        {
            GL10 gl = null;
            
            // This is our main activity thread's loop, we go until asked to quit.
            while (!isDone()) 
            {
                // FIXME: eventsWaiting and commandsWaiting are not needed here as it might block the event handling.
                boolean eventsWaiting = true;
                boolean commandsWaiting  = true;
                
                synchronized (sGLThreadManager) 
                {
                    eventsWaiting = mEventsWaiting;
                    commandsWaiting = mCommandsWaiting;
                }

                if(true)
                {
                    Runnable r = null;
                    while ((r = getCommand()) != null)
                    {
                        r.run();
                    }
                }
                
                // Try activate an engine.
                // FIXME: why is render context now unrelated to Engine.
                if (activeRenderContext == null)
                {
                    Iterator<EngineRenderContext> iter = renderContexts.values().iterator();
                    EngineRenderContext renderContext;
                    while (iter.hasNext()) 
                    {
                        renderContext = iter.next();
                        if (renderContext.available())
                        {
                            activateContext(renderContext);
                            break;
                        }
                    } 
                }

                if (PNativeLibrary.getInstance().isInitialized()) 
                {
                    Runnable r = null;
                    
                    int eventCounter = 0;
                    while ((r = getEvent()) != null)
                    {
                    	if (eventCounter == 0)
                        {
                            Log.i("Paper3D", "GLThread: Process event ...");
                        }
                        try 
                        { 
                            r.run(); 
                        } 
                        catch (Exception e) 
                        {
                        }
                        ++eventCounter;
                        // Limit the event processing time in order to avoid
                        // blocking other incoming events.
                        if (eventCounter >= MAX_EVENT_COUNT_PER_UPDATE) 
                        {
                            break;
                        }
                    }

                    if (eventCounter > 0)
                    {
                        Log.i("Paper3D", "GLThread: Event was processed, count: " + eventCounter);
                    }
                }
                
                if (activeRenderContext != null)
                {
                    if (activeRenderContext.popSurfaceOrSizeChanged())
                    {
                    	Log.i("Paper3D", "GLThread: Surface has been changed.");
                        activeRenderContext.onSurfaceChanged(gl);
                    }
                    
                    activeRenderContext.onDrawFrame(gl);
                    
                    activeRenderContext.swap();

                    Thread.sleep(10);
                }
                else 
                {
                    synchronized (sGLThreadManager) 
                    {
                        sGLThreadManager.wait();
                    }
                }
            }
        } 
        finally 
        {
            synchronized (sGLThreadManager) 
            {
                EngineRenderContext.finish();
            }
        }
    }

    private boolean isDone() 
    {
        return mDone;
    }

    private EngineRenderContext getEngineRenderContext(GLEngine engine)
    {
        EngineRenderContext renderContext;
        if (renderContexts.containsKey(engine))
        {
            renderContext = (EngineRenderContext)renderContexts.get(engine);
        }
        else
        {
            renderContext = new EngineRenderContext();
            engine.setContext(renderContext.getContext());
            renderContexts.put(engine, renderContext);
        }
        return renderContext;
    }
    
    private void activateContext(EngineRenderContext context)
    {
        deactiveCurrentContext();
        activeRenderContext = context;
        activeRenderContext.activate();
    }
    
    private void deactiveCurrentContext()
    {
        if (activeRenderContext != null)
        {
            activeRenderContext.deactivate();
        }
        activeRenderContext = null;
    }
    
    public void onSurfaceCreated(final SurfaceHolder holder, final GLEngine engine) 
    {
        Log.i("Paper3D", "GLThread: surfaceCreated()");
        
        queueCommand( new Runnable() 
        {
            @Override
            public void run() 
            {
                Log.i("Paper3D", "GLThread: SurfaceCreated on " + engine);
                EngineRenderContext context = getEngineRenderContext(engine);
                if (context.active()) 
                {
                    deactiveCurrentContext();
                }
                context.changeSurface(holder, holder.getSurfaceFrame().width(), holder.getSurfaceFrame().height());
            }
        });
        
        synchronized(sGLThreadManager)
        {
            sGLThreadManager.notifyAll();
        }
    }

    public void onWindowResize(final GLEngine engine, final SurfaceHolder surf, final int width, final int height) 
    {
        Log.i("Paper3D", "GLThread: onWindowResize()");
        
        queueCommand( new Runnable() 
        {
            @Override
            public void run() 
            {
                Log.i("Paper3D", "GLThread: Size Changed on " + engine);
                EngineRenderContext context = getEngineRenderContext(engine);
                if (context.active()) 
                {
                    deactiveCurrentContext();
                }
                context.changeSurface(surf, width, height);
            }
        });
        
        synchronized(sGLThreadManager)
        {
            sGLThreadManager.notifyAll();
        }
    }
    
    public void surfaceDestroyed(final GLEngine engine) {
        Log.i("Paper3D", "GLThread: surfaceDestroyed()");
        
        queueCommand( new Runnable() 
        {
            @Override
            public void run() 
            {
                Log.i("Paper3D", "GLThread: SurfaceDestroyed on " + engine);
                EngineRenderContext context = getEngineRenderContext(engine);
                if(context.active()) 
                {
                    deactiveCurrentContext();
                }
                context.changeSurface(null, 0, 0);
            }
        });
        
        synchronized(sGLThreadManager)
        {
            sGLThreadManager.notifyAll();
        }
    }

    public void onPause(final GLEngine engine) 
    {
        Log.i("Paper3D", "GLThread: onPause()");
        
        queueCommand( new Runnable() 
        {
            @Override
            public void run() 
            {
                Log.i("Paper3D", "GLThread: Pause on " + engine);
                EngineRenderContext context = getEngineRenderContext(engine);
                if(context.active()) {
                    deactiveCurrentContext();
                }
                context.pause();
            }
        });
        
        synchronized(sGLThreadManager)
        {
            sGLThreadManager.notifyAll();
        }
    }

    public void onResume(final GLEngine engine) 
    {
        Log.i("Paper3D", "GLThread: onResume()");
        
        queueCommand( new Runnable() {
            @Override
            public void run() 
            {
                Log.i("Paper3D", "GLThread: Resume on " + engine);
                EngineRenderContext context = getEngineRenderContext(engine);
                context.resume();
            }
        });
        
        synchronized(sGLThreadManager)
        {
            sGLThreadManager.notifyAll();
        }
    }

    public void requestExitAndWait() 
    {
        Log.i("Paper3D", "GLThread: requestExitAndWait()");
        synchronized (sGLThreadManager) 
        {
            mDone = true;
            sGLThreadManager.notifyAll();
        }
        
        try 
        {
            join();
        } 
        catch (InterruptedException ex) 
        {
            Thread.currentThread().interrupt();
        }
    }

    public void queueEvent(Runnable r) 
    {
        synchronized (this) 
        {
            mEventQueue.add(r);

            synchronized (sGLThreadManager) 
            {
                mEventsWaiting = true;
                sGLThreadManager.notifyAll();
            }
        }
    }
    
    public void queueCommand(Runnable r)
    {
        synchronized (this) 
        {
            mCommandQueue.add(r);

            synchronized (sGLThreadManager) 
            {
                mCommandsWaiting = true;
                sGLThreadManager.notifyAll();
            }
        }
    }
    
    private Runnable getCommand() 
    {
        synchronized (this) 
        {
            if (mCommandQueue.size() > 0) 
            {
                return mCommandQueue.remove(0);
            }
        }
        return null;
    }
    
    private Runnable getEvent() 
    {
        synchronized (this) 
        {
            if (mEventQueue.size() > 0) 
            {
                return mEventQueue.remove(0);
            }
        }
        return null;
    }

    private class GLThreadManager 
    {
    }
}

interface EGLConfigChooser 
{
    EGLConfig chooseConfig(EGL10 egl, EGLDisplay display);
}

class ConfigChooser implements EGLConfigChooser 
{
    private int[] configAttribs;

    /**
     * @param antialias
     */
    public ConfigChooser(boolean antialias) 
    {
        if (antialias) 
        {
            int[] conf = { EGL10.EGL_SAMPLES, 4, EGL10.EGL_SAMPLE_BUFFERS,
                1, EGL10.EGL_RED_SIZE, EGL10.EGL_DONT_CARE,
                EGL10.EGL_GREEN_SIZE, EGL10.EGL_DONT_CARE,
                EGL10.EGL_BLUE_SIZE, EGL10.EGL_DONT_CARE,
                EGL10.EGL_DEPTH_SIZE, 16, EGL10.EGL_RENDERABLE_TYPE, 4,
                EGL10.EGL_STENCIL_SIZE, 8, EGL10.EGL_NONE };
            configAttribs = conf;
        } 
        else 
        {
            int[] conf = { EGL10.EGL_SAMPLES, 0, EGL10.EGL_SAMPLE_BUFFERS,
                0, EGL10.EGL_RED_SIZE, EGL10.EGL_DONT_CARE,
                EGL10.EGL_GREEN_SIZE, EGL10.EGL_DONT_CARE,
                EGL10.EGL_BLUE_SIZE, EGL10.EGL_DONT_CARE,
                EGL10.EGL_DEPTH_SIZE, 16, EGL10.EGL_RENDERABLE_TYPE, 4,
                EGL10.EGL_STENCIL_SIZE, 8, EGL10.EGL_NONE };
            configAttribs = conf;
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
        egl.eglChooseConfig(display, configAttribs, config, 1, num_config);

        return config[0];
    }
}

// ////////////////////////////////////////////////////////////////////////////////////////////////////////

abstract class BaseConfigChooser implements EGLConfigChooser 
{
    public BaseConfigChooser(int[] configSpec) 
    {
        mConfigSpec = configSpec;
    }

    public EGLConfig chooseConfig(EGL10 egl, EGLDisplay display) 
    {
        int[] num_config = new int[1];
        egl.eglChooseConfig(display, mConfigSpec, null, 0, num_config);

        int numConfigs = num_config[0];

        if (numConfigs <= 0) {
            throw new IllegalArgumentException("No configs match configSpec");
        }

        EGLConfig[] configs = new EGLConfig[numConfigs];
        egl.eglChooseConfig(display, mConfigSpec, configs, numConfigs,
                num_config);
        EGLConfig config = chooseConfig(egl, display, configs);
        if (config == null) 
        {
            throw new IllegalArgumentException("No config chosen");
        }
        return config;
    }

    abstract EGLConfig chooseConfig(EGL10 egl, EGLDisplay display, EGLConfig[] configs);

    protected int[] mConfigSpec;

    public static class ComponentSizeChooser extends BaseConfigChooser 
    {
        public ComponentSizeChooser(int redSize, int greenSize, int blueSize,
                int alphaSize, int depthSize, int stencilSize) 
        {
            super(new int[] 
                    {
                        EGL10.EGL_RED_SIZE, redSize,
                        EGL10.EGL_GREEN_SIZE, greenSize, EGL10.EGL_BLUE_SIZE,
                        blueSize, EGL10.EGL_ALPHA_SIZE, alphaSize,
                        EGL10.EGL_DEPTH_SIZE, depthSize, EGL10.EGL_STENCIL_SIZE,
                        stencilSize, EGL10.EGL_NONE 
                    });

            mValue = new int[1];
            mRedSize = redSize;
            mGreenSize = greenSize;
            mBlueSize = blueSize;
            mAlphaSize = alphaSize;
            mDepthSize = depthSize;
            mStencilSize = stencilSize;
        }

        @Override
        public EGLConfig chooseConfig(EGL10 egl, EGLDisplay display, EGLConfig[] configs) 
        {
            EGLConfig closestConfig = null;
            int closestDistance = 1000;
            for (EGLConfig config : configs) 
            {
                int d = findConfigAttrib(egl, display, config, EGL10.EGL_DEPTH_SIZE, 0);
                int s = findConfigAttrib(egl, display, config, EGL10.EGL_STENCIL_SIZE, 0);
                if (d >= mDepthSize && s >= mStencilSize) 
                {
                    int r = findConfigAttrib(egl, display, config, EGL10.EGL_RED_SIZE, 0);
                    int g = findConfigAttrib(egl, display, config, EGL10.EGL_GREEN_SIZE, 0);
                    int b = findConfigAttrib(egl, display, config, EGL10.EGL_BLUE_SIZE, 0);
                    int a = findConfigAttrib(egl, display, config, EGL10.EGL_ALPHA_SIZE, 0);

                    int distance = Math.abs(r - mRedSize)
                        + Math.abs(g - mGreenSize)
                        + Math.abs(b - mBlueSize)
                        + Math.abs(a - mAlphaSize);

                    if (distance < closestDistance) 
                    {
                        closestDistance = distance;
                        closestConfig = config;
                    }
                }
            }
            return closestConfig;
        }

        private int findConfigAttrib(EGL10 egl, EGLDisplay display,
                EGLConfig config, int attribute, int defaultValue) 
        {

            if (egl.eglGetConfigAttrib(display, config, attribute, mValue)) 
            {
                return mValue[0];
            }
            return defaultValue;
        }

        private int[] mValue;
        // Subclasses can adjust these values:
        protected int mRedSize;
        protected int mGreenSize;
        protected int mBlueSize;
        protected int mAlphaSize;
        protected int mDepthSize;
        protected int mStencilSize;
    }

    /**
     * This class will choose a supported surface as close to RGB565 as
     * possible, with or without a depth buffer.
     * 
     */
    public static class SimpleEGLConfigChooser extends ComponentSizeChooser 
    {
        public SimpleEGLConfigChooser(boolean withDepthBuffer) 
        {
            super(4, 4, 4, 0, withDepthBuffer ? 16 : 0, 0);
            // Adjust target values. This way we'll accept a 4444 or
            // 555 buffer if there's no 565 buffer available.
            mRedSize = 5;
            mGreenSize = 6;
            mBlueSize = 5;
        }
    }
}

/**
 * An interface for customizing the eglCreateWindowSurface and eglDestroySurface
 * calls.
 * 
 * 
 * This interface must be implemented by clients wishing to call
 * {@link GLWallpaperService#setEGLWindowSurfaceFactory(EGLWindowSurfaceFactory)}
 */
interface EGLWindowSurfaceFactory 
{
    EGLSurface createWindowSurface(EGL10 egl, EGLDisplay display,
            EGLConfig config, Object nativeWindow);

    void destroySurface(EGL10 egl, EGLDisplay display, EGLSurface surface);
}

class DefaultWindowSurfaceFactory implements EGLWindowSurfaceFactory 
{
    public EGLSurface createWindowSurface(EGL10 egl, EGLDisplay display,
            EGLConfig config, Object nativeWindow) 
    {
        // this is a bit of a hack to work around Droid init problems - if you
        // don't have this, it'll get hung up on orientation changes
        EGLSurface eglSurface = null;
        while (eglSurface == null) 
        {
            try 
            {
                eglSurface = egl.eglCreateWindowSurface(display, config, nativeWindow, null);
            } 
            catch (Throwable t) 
            {
            } 
            finally 
            {
                if (eglSurface == null) 
                {
                    try 
                    {
                        Thread.sleep(10);
                    } 
                    catch (InterruptedException t) 
                    {
                    }
                }
            }
        }
        Log.i("Paper3D", "GLThread: Surface created " + eglSurface.toString() + " on " + nativeWindow.toString() );
        return eglSurface;
    }

    public void destroySurface(EGL10 egl, EGLDisplay display, EGLSurface surface) 
    {
        Log.i("Paper3D", "GLThread: Destory surface " + surface.toString());
        egl.eglDestroySurface(display, surface);
    }
}

/**
 * An interface for customizing the eglCreateContext and eglDestroyContext
 * calls.
 * 
 * 
 * This interface must be implemented by clients wishing to call
 * {@link GLWallpaperService#setEGLContextFactory(EGLContextFactory)}
 */
interface EGLContextFactory 
{
    EGLContext createContext(EGL10 egl, EGLDisplay display, EGLConfig eglConfig);

    void destroyContext(EGL10 egl, EGLDisplay display, EGLContext context);
}

class DefaultContextFactory implements EGLContextFactory 
{
    private static int EGL_CONTEXT_CLIENT_VERSION = 0x3098;

    public EGLContext createContext(EGL10 egl, EGLDisplay display, EGLConfig config) 
    {
        Log.i("Paper3D", "GLThread: creating OpenGL ES 2.0 context");
        int[] attrib_list = { EGL_CONTEXT_CLIENT_VERSION, 2, EGL10.EGL_NONE };
        EGLContext context = egl.eglCreateContext(display, config, EGL10.EGL_NO_CONTEXT, attrib_list);
        return context;
    }

    public void destroyContext(EGL10 egl, EGLDisplay display, EGLContext context) 
    {
        egl.eglDestroyContext(display, context);
    }
}
