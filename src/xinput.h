#pragma once

#include <X11/Xlib.h>
#include <X11/extensions/XInput2.h>

extern struct _eglut_xinput_rt_t {
    int initialized;
    void* handle;
    Status (*QueryVersion)(Display*, int*, int*);
    Status (*SelectEvents)(Display* dpy, Window win, XIEventMask* masks, int num_masks);
} _eglut_xinput_rt;

extern struct eglut_xinput_state_t {
    int initialized;
    int available;
    int opcode;
} _eglut_xinput_state;

void _eglutInitXinputRt();
void _eglutInitXinput();

int _eglutIsXinputAvailable();

int _eglutXinputSetRawMotion(int raw);

#define XIQueryVersion (_eglut_xinput_rt.QueryVersion)
#define XISelectEvents (_eglut_xinput_rt.SelectEvents)