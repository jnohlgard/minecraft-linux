#ifndef EGLUT_X11_H
#define EGLUT_X11_H

#ifdef __cplusplus
extern "C" {
#endif

#include <X11/Xlib.h>

Display* eglutGetDisplay();
Window eglutGetWindowHandle();

#ifdef __cplusplus
}
#endif

#endif /* EGLUT_H */