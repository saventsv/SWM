#include "../include/layout.h"
#include "../include/wm.h"
#include <X11/Xlib.h>

void calculate_geometry(Workspace *ws, Monitor *monitor) {
  if(ws->clients.size == 0) return;

  Client *master_client = ws->clients.data[0];
  master_client->x = 0;
  master_client->y = 0;
  // TODO add master ratio
  master_client->width = ws->clients.size == 1 ? monitor->usable_width : monitor->usable_width / 2;
  master_client->height = monitor->usable_height;

  if(ws->clients.size > 1) {

    int stack_width = monitor->usable_width - master_client->width;
    int stack_height = (monitor->usable_height / (ws->clients.size - 1));
    int stack_x = master_client->width;

    for(int i = 1; i < ws->clients.size; i++) {
      Client *client = ws->clients.data[i];
      int stack_y = stack_height * (i - 1);

      client->width = stack_width;
      client->height = stack_height;
      client->x = stack_x;
      client->y = stack_y;
    }
  }
}


void tile(Display *dpy, Workspace *ws, Monitor *monitor) {
  calculate_geometry(ws, monitor);

  for(int i = 0; i < ws->clients.size; i++) {
    Client *client = ws->clients.data[i];
    XMoveResizeWindow(
        dpy, 
        client->window, 
        client->x, 
        client->y, 
        client->width, 
        client->height
    );
  }
}


