#pragma once
#include <X11/X.h>
#include <X11/Xlib.h>
#include <X11/extensions/Xrandr.h>

#define MAX_WORKSPACES 9

typedef struct {
  Window window;
  int x, y, width, height;
} Client;


typedef struct {
  Client **data;
  size_t capacity;
  size_t size;
} ClientArray;

typedef struct {
  ClientArray clients;
  int focused_idx;
  int id;
} Workspace;

typedef struct Monitor{
  int monitor_width, monitor_height;
  int monitor_x, monitor_y;
  // eventually used for handling reserving space for status bars
  int usable_width, usable_height;
  int usable_x, usable_y;
  Workspace *ws;
  // eventual multi-monitor support
  // struct Monitor *next
} Monitor;

typedef struct {
  Workspace workspaces[MAX_WORKSPACES];
  int current_workspace;
  int running;
  int focused_monitor;
  Monitor *monitors;
  int n_monitors;
  Display *dpy;
} WindowManager;

