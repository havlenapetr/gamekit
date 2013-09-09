/*
-------------------------------------------------------------------------------
    This file is part of OgreKit.
    http://gamekit.googlecode.com/

    Copyright (c) 2012 Petr Havlena (havlenapetr@gmail.com)
-------------------------------------------------------------------------------
  This software is provided 'as-is', without any express or implied
  warranty. In no event will the authors be held liable for any damages
  arising from the use of this software.

  Permission is granted to anyone to use this software for any purpose,
  including commercial applications, and to alter it and redistribute it
  freely, subject to the following restrictions:

  1. The origin of this software must not be misrepresented; you must not
     claim that you wrote the original software. If you use this software
     in a product, an acknowledgment in the product documentation would be
     appreciated but is not required.
  2. Altered source versions must be plainly marked as such, and must not be
     misrepresented as being the original software.
  3. This notice may not be removed or altered from any source distribution.
-------------------------------------------------------------------------------
*/

#include <stdlib.h>
#include <stdio.h>
#include <assert.h>

#include <android/log.h>
#include <android/window.h>
#include <android/sensor.h>
#include <android_native_app_glue.h>

#include <OgreKit.h>
#include <Ogre.h>

#include <EGL/egl.h>
#include <Android/OgreAndroidEGLWindow.h>
#include <Android/OgreAPKFileSystemArchive.h>
#include <Android/OgreAPKZipArchive.h>

#define LOG_TAG    "OgreKit"
#define LOGI(...)  __android_log_print(ANDROID_LOG_INFO,LOG_TAG,__VA_ARGS__)
#define LOGE(...)  __android_log_print(ANDROID_LOG_ERROR,LOG_TAG,__VA_ARGS__)
#define LOG_FOOT   LOGI("%s %s %d", __FILE__, __FUNCTION__, __LINE__);

class gkAndroidApp : public gkCoreApplication, public gkWindowSystem::Listener {
public:
    gkAndroidApp(android_app* state);
    virtual ~gkAndroidApp() {}

    virtual void    run();

protected:
    virtual bool    setup(void);
    virtual void    keyReleased(const gkKeyboard& key, const gkScanCode& sc);

private:
    bool            init(AConfiguration* config, const gkString& file);
    static int32_t  handleInput(struct android_app* app, AInputEvent* event);
    static void     handleCmd(struct android_app* app, int32_t cmd);
    int32_t         handleInput(AInputEvent* event);
    void            handleCmd(int32_t cmd);

    gkString        m_blend;
    gkWindow*       m_window;
    android_app*    m_state;
    const ASensor*  m_accelerometerSensor;
    ASensorEventQueue* m_sensorEventQueue;
};

/* static proxy callback */
int32_t gkAndroidApp::handleInput(struct android_app* app, AInputEvent* event) {
    return static_cast<gkAndroidApp *>(app->userData)->handleInput(event);
}

/* static proxy callback */
void gkAndroidApp::handleCmd(struct android_app* app, int32_t cmd) {
    static_cast<gkAndroidApp *>(app->userData)->handleCmd(cmd);
}

gkAndroidApp::gkAndroidApp(android_app* state)
    : m_state(state),
      m_window(NULL) {
    state->userData = this;
    state->onAppCmd = handleCmd;
    state->onInputEvent = handleInput;

    // prepare to monitor accelerometer
    ASensorManager *sensorManager = ASensorManager_getInstance();
    m_accelerometerSensor = ASensorManager_getDefaultSensor(sensorManager,
            ASENSOR_TYPE_ACCELEROMETER);
    m_sensorEventQueue = ASensorManager_createEventQueue(sensorManager,
            state->looper, LOOPER_ID_USER, NULL, NULL);

    ANativeActivity_setWindowFlags(state->activity,
            AWINDOW_FLAG_FULLSCREEN | AWINDOW_FLAG_KEEP_SCREEN_ON, 0);
}

bool gkAndroidApp::init(AConfiguration* config, const gkString& file) {
    m_blend = file;

    getPrefs().debugFps = true;
    getPrefs().wintitle = gkString("OgreKit Demo [") + file + gkString("]");
    getPrefs().blendermat=true;
    getPrefs().enableshadows=false;
    getPrefs().viewportOrientation="portrait";
    getPrefs().extWinhandle  = Ogre::StringConverter::toString((int)m_state->window);
    getPrefs().androidConfig = Ogre::StringConverter::toString((int)config);

    return initialize() && m_engine->initializeStepLoop();
}

bool gkAndroidApp::setup(void) {
    AAssetManager* assetMgr = m_state->activity->assetManager;
    if (assetMgr) {
        Ogre::ArchiveManager::getSingleton().addArchiveFactory(new Ogre::APKFileSystemArchiveFactory(assetMgr));
        Ogre::ArchiveManager::getSingleton().addArchiveFactory(new Ogre::APKZipArchiveFactory(assetMgr));
        Ogre::ResourceGroupManager::getSingleton().addResourceLocation("/", "APKFileSystem",
                Ogre::ResourceGroupManager::DEFAULT_RESOURCE_GROUP_NAME);
    }

    Ogre::DataStreamPtr stream = Ogre::ResourceGroupManager::getSingleton().
            openResource(m_blend, Ogre::ResourceGroupManager::DEFAULT_RESOURCE_GROUP_NAME);
    if (stream.isNull()) {
        gkPrintf("Error: Can't open file %s.\n", m_blend.c_str());
        return false;
    }

    size_t size = stream->size();
    char* buf = new char[size];
    stream->read(buf, size);

    gkBlendFile* blend = gkBlendLoader::getSingleton().loadFromMemory(buf,size,gkBlendLoader::LO_ALL_SCENES);
    gkScene* scene = blend->getMainScene();
    if (!scene) {
        gkPrintf("No usable scenes found in blend.\n");
        return false;
    }

    scene->createInstance();
    scene->getMainCamera()->getCamera()->setAutoAspectRatio(true);

    return true;
}

int32_t gkAndroidApp::handleInput(AInputEvent* event) {
    return 0;
}

void gkAndroidApp::handleCmd(int32_t cmd) {
    switch (cmd) {
        case APP_CMD_SAVE_STATE:
            LOG_FOOT
            break;

        case APP_CMD_INIT_WINDOW:
            LOG_FOOT
            // The window is being shown, get it ready.
            if (m_state->window) {
                AConfiguration* config = AConfiguration_new();

                if(!m_window) {
                    AConfiguration_fromAssetManager(config, m_state->activity->assetManager);

                    init(config, "momo_ogre_i.blend");

                    m_window = gkWindowSystem::getSingleton().getMainWindow();
                } else {
                    static_cast<Ogre::AndroidEGLWindow*>(m_window->getRenderWindow())->
                            _createInternalResources(m_state->window, config);
                }

                AConfiguration_delete(config);
            }
            break;

        case APP_CMD_TERM_WINDOW: {
            LOG_FOOT
            if(m_window) {
                static_cast<Ogre::AndroidEGLWindow*>(m_window->getRenderWindow())->
                        _destroyInternalResources();
            }
            break;
        }

        case APP_CMD_GAINED_FOCUS:
            LOG_FOOT
            // When our app gains focus, we start monitoring the accelerometer.
            if (m_accelerometerSensor) {
                ASensorEventQueue_enableSensor(m_sensorEventQueue,
                        m_accelerometerSensor);
                // We'd like to get 60 events per second (in us).
                ASensorEventQueue_setEventRate(m_sensorEventQueue,
                        m_accelerometerSensor, (1000L/60)*1000);
            }
            break;

        case APP_CMD_LOST_FOCUS:
            LOG_FOOT
            // When our app loses focus, we stop monitoring the accelerometer.
            // This is to avoid consuming battery while not being used.
            if (m_accelerometerSensor) {
                ASensorEventQueue_disableSensor(m_sensorEventQueue,
                        m_accelerometerSensor);
            }
            break;

        case APP_CMD_CONFIG_CHANGED:
            LOG_FOOT
            break;
    }
}

void gkAndroidApp::keyReleased(const gkKeyboard& key, const gkScanCode& sc) {
    if (sc == KC_ESCKEY)
        m_engine->requestExit();
}

void gkAndroidApp::run() {
    int ident;
    int events;

    while (!m_state->destroyRequested) {
        struct android_poll_source* source;

        // If not animating, we will block forever waiting for events.
        // If animating, we loop until all events are read, then continue
        // to draw the next frame of animation.
        while ((ident=ALooper_pollAll((m_engine && m_engine->isRunning()) ? 0 : -1,
                NULL, &events, (void**)&source)) >= 0) {

            // Process this event.
            if (source != NULL) {
                source->process(m_state, source);
            }
        }

        if(m_window && m_window->isActive()) {
            m_engine->stepOneFrame();
        }
    }
}

/**
 * This is the main entry point of a native application that is using
 * android_native_app_glue.  It runs in its own thread, with its own
 * event loop for receiving input events and doing other things.
 */
void android_main(struct android_app* state) {
    assert(state);

    // Make sure glue isn't stripped.
    app_dummy();

    gkAndroidApp app(state);
    app.run();
}
