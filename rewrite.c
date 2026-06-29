#include <X11/X.h>
#include <X11/Xatom.h>
#include <X11/Xlib.h>
#include <X11/Xutil.h>
#include <X11/keysym.h>

#define MAX_WORKSPACES 9

typedef struct Client {
  Window window;
  int x, y, width, height;
  struct Client *next_client;
} Client;

typedef struct {
  Client *master_client;
  int focused_idx;
  int n_clients;
  int id;
} Workspace;

typedef struct {
  Workspace workspaces[MAX_WORKSPACES];
  int current_workspace;
  int running;
} WindowManager;

typedef struct Monitor{
  int monitor_width, monitor_height;
  int usable_width, usable_height;
  Workspace *ws;
  eventual multi-monitor support
  struct Monitor *next
} Monitor;

