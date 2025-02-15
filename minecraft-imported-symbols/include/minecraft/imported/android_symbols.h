 #pragma once

static const char* android_symbols[] = {
        "ANativeWindow_getWidth",
        "ANativeWindow_getHeight",
        "ANativeWindow_setBuffersGeometry",
        "AAssetManager_open",
        "AAsset_getLength",
        "AAsset_getBuffer",
        "AAsset_close",
        "AAsset_read",
        "AAsset_seek64",
        "AAsset_getLength64",
        "AAsset_getRemainingLength64",
        "ALooper_pollAll",
        "ANativeActivity_finish",
        "AInputQueue_getEvent",
        "AKeyEvent_getKeyCode",
        "AInputQueue_preDispatchEvent",
        "AInputQueue_finishEvent",
        "AKeyEvent_getAction",
        "AMotionEvent_getAxisValue",
        "AKeyEvent_getRepeatCount",
        "AKeyEvent_getMetaState",
        "AInputEvent_getDeviceId",
        "AInputEvent_getType",
        "AInputEvent_getSource",
        "AMotionEvent_getAction",
        "AMotionEvent_getPointerId",
        "AMotionEvent_getX",
        "AMotionEvent_getRawX",
        "AMotionEvent_getY",
        "AMotionEvent_getRawY",
        "AMotionEvent_getPointerCount",
        "AConfiguration_new",
        "AConfiguration_fromAssetManager",
        "AConfiguration_getLanguage",
        "AConfiguration_getCountry",
        "ALooper_prepare",
        "ALooper_addFd",
        "AInputQueue_detachLooper",
        "AConfiguration_delete",
        "AInputQueue_attachLooper",
        "AAssetManager_openDir",
        "AAssetDir_getNextFileName",
        "AAssetDir_close",
        "AAssetManager_fromJava",
        nullptr
};
