#pragma once

#include "wm.h"

void handle_maprequest(WindowManager *wm, XEvent event); 
void handle_configurerequest(WindowManager *wm, XEvent event);
void handle_destorynotify(WindowManager *wm, XEvent event);
void handle_keypress(WindowManager *wm, XEvent event);
// void handle_unmapnotify(WindowManager *wm, XEvent event);
// void handle_focusin(WindowManager *wm, XEvent event);
// void handle_focusout(WindowManager *wm, XEvent event);
// void handle_clientmessage(WindowManager *wm, XEvent event);

