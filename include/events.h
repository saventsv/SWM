#pragma once

#include "wm.h"

void handle_maprequest(WindowManager *wm, XEvent event); 
void handle_configurerequest(WindowManager *wm, XEvent event);
void handle_destorynotify(WindowManager *wm, XEvent event);

