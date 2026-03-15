#include <X11/Xatom.h>
#include <X11/Xlib.h>
#include <X11/keysym.h>
#include <signal.h>
#include <sys/wait.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <unistd.h>
#include <X11/X.h>
/* ... other system includes ... */

// 1. DEFINE YOUR TYPES FIRST

#define MAX_WORKSPACES 9
typedef struct {
  Window window;
  int x, y, width, height;
  int is_master;
} Client;

typedef union {
  int i;
  unsigned int ui;
  float f;
  Client client;
  const char **c;
  const void *v;
} Arg;

typedef struct {
  unsigned int mod;
  KeySym keysym;
  void (*func)(Display *dpy, const Arg *arg);
  const Arg arg;
} Keybinding;

typedef struct {
  Client *clients;
  Client *focused;
  int focused_idx;
  int n_clients;
  int id;
} Workspace;

typedef struct {
  int current_workspace;
  Workspace workspaces[MAX_WORKSPACES];
} WindowManager;

typedef struct {
  KeySym key;
  void (*action)(Display *dpy, const Arg *arg);
  const Arg arg;
} Chord;

// 2. DECLARE YOUR FUNCTIONS (Prototypes)
// This tells config.h that these functions exist somewhere
/*
void quit(Display *dpy, const Arg *arg);
void terminal_kitty(Display *dpy, const Arg *arg);
void focus(Display *dpy, const Arg *arg);
void move_workspace(Display *dpy, const Arg *arg);
void activate_chord(Display *dpy, const Arg *arg);
*/

void quit(Display *dpy, const Arg *arg);
void terminal_kitty(Display *dpy, const Arg *arg);
void close_window(Display *dpy, const Arg *arg);
void kill_client(Display *dpy, const Arg *arg);
void focus(Display *dpy, const Arg *arg);
void move_workspace(Display *dpy, const Arg *arg);
void activate_chord(Display *dpy, const Arg *arg);

#include "config.h"


/*
typedef struct {
  Window window;
  int x, y, width, height;
  int is_master;
} Client;

typedef union {
  int i;
  unsigned int ui;
  float f;
  Client client;
  const char **c;
  const void *v;
} Arg;

typedef struct {
  unsigned int mod;
  KeySym keysym;
  void (*func)(Display *dpy, const Arg *arg);
  const Arg arg;
} Keybinding;

typedef struct {
  Client *clients;
  Client *focused;
  int focused_idx;
  int n_clients;
  int id;
} Workspace;

typedef struct {
  int current_workspace;
  Workspace workspaces[MAX_WORKSPACES];
} WindowManager;

typedef struct {
  KeySym key;
  void (*action)(Display *dpy, const Arg *arg);
  const Arg arg;
} Chord;
*/
int KeyChordState = 0;


int running = 1;


unsigned int top_margin = 0;

Window top_bar = None;

WindowManager WM;

Atom net_wm_name, net_supporting_wm_check, net_active_window, net_current_desktop, net_number_of_desktops;

void quit(Display *dpy, const Arg *arg) {
  running = 0;
}

void init_wm() {

  for(int i = 0; i < MAX_WORKSPACES; i++) {
    WM.workspaces[i].clients = NULL;
    WM.workspaces[i].focused = NULL;
    WM.workspaces[i].focused_idx = 0;
    WM.workspaces[i].n_clients = 0;
    WM.workspaces[i].id = i;
  }
  WM.current_workspace = 0;
}

Atom get_window_type(Display *dpy, Window w) {
  Atom actual_type;
  int actual_format;
  unsigned long nitems, bytes_after;
  unsigned char *prop = NULL;
  Atom net_wm_window_type = XInternAtom(dpy, "_NET_WM_WINDOW_TYPE", False);

  if (XGetWindowProperty(dpy, w, net_wm_window_type, 0, sizeof(Atom), False, XA_ATOM,
        &actual_type, &actual_format, &nitems, &bytes_after, &prop) == Success && prop) {
    Atom type = *(Atom *)prop;
    XFree(prop);
    return type;
  }
  return None;
}

int has_proto(Display *dpy, Window w, Atom protocol) {
  Atom *protocols;
  int n, found = 0;

  if (XGetWMProtocols(dpy, w, &protocols, &n)) {
    while (!found && n--)
      found = protocols[n] == protocol;
    XFree(protocols);
  }
  return found;
}

void update_active_window(Display *dpy) {
  Workspace *ws = &WM.workspaces[WM.current_workspace];
  Window w = (ws -> focused) ? ws -> focused -> window : None;

  XChangeProperty(dpy, DefaultRootWindow(dpy), net_active_window, XA_WINDOW, 32, 
      PropModeReplace, (unsigned char *) &w, 1);
}


void tile(Display *dpy) {
  int screen = DefaultScreen(dpy);
  int screen_width = DisplayWidth(dpy, screen);
  int screen_height = DisplayHeight(dpy, screen) - top_margin;
  int screen_y = top_margin;
  Workspace *ws = &WM.workspaces[WM.current_workspace];



  for(int i = 0; i < ws -> n_clients; i++) {
    if (i == 0) {
      ws -> clients[i].is_master = 1;
      ws -> clients[i].x = 0;
      ws -> clients[i].y = screen_y;
      ws -> clients[i].width = (ws -> n_clients == 1)? screen_width: screen_width * master_ratio;
      ws -> clients[i].height = screen_height;
      XMoveResizeWindow(
          dpy, 
          ws -> clients[i].window, 
          ws -> clients[i].x + gaps, 
          ws -> clients[i].y + gaps, 
          ws -> clients[i].width - (2 * gaps) - (2 * borders), 
          ws -> clients[i].height - (2 * gaps) - (2 * borders)
          );
      XSetWindowBorderWidth(dpy, ws -> clients[i].window, borders);
    } else {
      ws -> clients[i].x = screen_width * master_ratio;
      ws -> clients[i].height = screen_height / ( ws -> n_clients - 1);
      ws -> clients[i].y = top_margin + (i - 1) * ws -> clients[i].height;
      ws -> clients[i].width = screen_width * ( 1 - master_ratio); 
      XMoveResizeWindow(
          dpy, 
          ws -> clients[i].window, 
          ws -> clients[i].x + gaps, 
          ws -> clients[i].y + gaps, 
          ws -> clients[i].width - (2 * gaps) - (2 * borders), 
          ws -> clients[i].height - (2 * gaps) - (2 * borders)
          );
      XSetWindowBorderWidth(dpy, ws -> clients[i].window, borders);
      XSetWindowBorder(dpy, ws -> clients[i].window, (i == ws -> focused_idx) ? active_color: idle_color);
    }
  }
}

void update_struts(Display *dpy, Window w) {
  Atom actual_type;
  int actual_format;
  unsigned long nitems, bytes_after;
  unsigned char *prop = NULL;
  Atom net_wm_strut = XInternAtom(dpy, "_NET_WM_STRUT_PARTIAL", False);

  if (XGetWindowProperty(dpy, w, net_wm_strut, 0, 12, False, XA_CARDINAL,
        &actual_type, &actual_format, &nitems, &bytes_after, &prop) == Success && prop) {
    unsigned long *struts = (unsigned long *)prop;
    // Index 2 is the top strut
    top_margin = (unsigned int)struts[2];
    XFree(prop);
    printf("Detected bar height: %u pixels\n", top_margin);
  }
}


// 0 = left
// 1 = right
// 2 = up
// 3 = down

void focus(Display *dpy, const Arg *direction) {
  Workspace *ws = &WM.workspaces[WM.current_workspace];

  if (ws -> n_clients == 0) return;

  // Find the index of the currently focused window
  int idx = -1;
  if (ws -> focused) {
    for (int i = 0; i < ws -> n_clients; i++) {
      if (ws -> clients[i].window == ws -> focused ->window) {
        idx = i;
        break;
      }
    }
  }

  // If nothing is focused, default to Master
  if (idx == -1) idx = 0;

  int next_idx = idx;

  switch(direction -> i) {
    case 0: // Left
      next_idx = 0; 
      break;
    case 1: // Right
      if (idx == 0 && ws -> n_clients > 1) next_idx = 1;
      break;
    case 2: // Up
      if (idx > 1) next_idx = idx - 1;
      else if (idx == 1) next_idx = ws -> n_clients - 1; // Wrap within stack
      break;
    case 3: // Down
      if (idx > 0 && idx < ws -> n_clients - 1) next_idx = idx + 1;
      else if (idx == ws -> n_clients - 1) next_idx = 1; // Wrap within stack
      break;
  }

  ws -> focused = &ws -> clients[next_idx];
  ws -> focused_idx = next_idx;

  update_active_window(dpy);

  XSetInputFocus(dpy, ws -> focused -> window, RevertToParent, CurrentTime);
  XRaiseWindow(dpy, ws -> focused -> window);
}

void update_borders(Display *dpy) {
  Workspace *ws = &WM.workspaces[WM.current_workspace];
  for(int i = 0; i < ws -> n_clients; i++) {
    XSetWindowBorderWidth(dpy, ws -> clients[i].window, borders);
    XSetWindowBorder(dpy, ws -> clients[i].window, (i == ws -> focused_idx) ? active_color: idle_color);
  }
}

void update_current_desktop(Display *dpy) {
  unsigned long current = (unsigned long)WM.current_workspace;
  XChangeProperty(dpy, DefaultRootWindow(dpy), net_current_desktop, XA_CARDINAL, 32, 
      PropModeReplace, (unsigned char *) &current, 1);
}

void move_workspace(Display *dpy, const Arg *arg) {
  Workspace *old_ws = &WM.workspaces[WM.current_workspace];

  for(int i = 0; i < old_ws -> n_clients; i++) {
    XUnmapWindow(dpy, old_ws -> clients[i].window);
  }

  WM.current_workspace = arg -> i;

  Workspace *next_ws = &WM.workspaces[arg -> i];

  if(next_ws -> n_clients == 0) tile(dpy);
  else {
    for(int i = 0; i < next_ws -> n_clients; i++) {
      XMapWindow(dpy, next_ws -> clients[i].window);
    }
    if(next_ws -> focused != NULL) {
      XSetInputFocus(dpy, next_ws -> focused -> window, RevertToParent, CurrentTime);
    }

    tile(dpy);
    update_borders(dpy);
    update_current_desktop(dpy);
  }


}

void move_window(Display *dpy, const Arg *direction) {
  Workspace *ws = &WM.workspaces[WM.current_workspace];

  int idx = -1;
  if (ws -> focused) {
    for (int i = 0; i < ws -> n_clients; i++) {
      if (ws -> clients[i].window == ws -> focused ->window) {
        idx = i;
        break;
      }
    }
  }

  if(ws -> n_clients < 2) return;


  switch (direction -> i) {

    case 0:
      {
        if(idx == 0) break;

        Client tmp = ws -> clients[idx];
        ws -> clients[idx] = ws -> clients[0];
        ws -> clients[idx].is_master = 0;
        ws -> clients[0] = tmp;
        ws -> clients[0].is_master = 1;
        ws -> focused = &ws -> clients[0];
        ws -> focused_idx = 0;
        tile(dpy);
        update_borders(dpy);
        break;
      }
    case 1: 
      {
        if(idx > 0) break;

        Client tmp = ws -> clients[0];
        ws -> clients[0] = ws -> clients[1];
        ws -> clients[0].is_master = 1;
        ws -> clients[1] = tmp;
        ws -> clients[1].is_master = 0;
        ws -> focused = &ws -> clients[1];
        ws -> focused_idx = 1;
        tile(dpy);
        update_borders(dpy);
        break;

      }
    case 2: 
      {
        if (idx == 0) break;

        Client tmp = ws -> clients[idx - 1];
        ws -> clients[idx - 1] =  ws -> clients[idx];
        ws -> clients[idx] = tmp;
        ws -> focused = &ws -> clients[idx - 1];
        ws -> focused_idx = idx - 1;
        tile(dpy);
        update_borders(dpy);
        break;
      }
    case 3: 
      {
        if (idx == ws -> n_clients - 1) break;
        Client tmp = ws -> clients[idx + 1];
        ws -> clients[idx + 1] =  ws -> clients[idx];
        ws -> clients[idx] = tmp;
        ws -> focused = &ws -> clients[idx + 1];
        ws -> focused_idx = idx + 1;
        tile(dpy);
        update_borders(dpy);
        break;
      }
  }
}

void move_window_workspace(Display *dpy, const Arg *workspace) {
  Workspace *ws = &WM.workspaces[WM.current_workspace];
  Workspace *next_ws = &WM.workspaces[workspace -> i];

  if(ws -> focused == NULL) return;

  next_ws -> clients = realloc(next_ws -> clients, sizeof(Client) * (next_ws -> n_clients + 1));
  next_ws -> clients[next_ws -> n_clients] = ws -> clients[ws -> focused_idx];
  next_ws -> n_clients++;

  next_ws -> focused = &next_ws -> clients[next_ws -> n_clients - 1];
  next_ws -> focused_idx = next_ws->n_clients - 1;


  XUnmapWindow(dpy, ws -> clients[ws -> focused_idx].window);

  for (int i = ws -> focused_idx; i < ws -> n_clients - 1; i++)
    ws -> clients[i] = ws -> clients[i + 1];
  ws -> n_clients--;

  ws -> focused_idx = ws -> n_clients - 1;
  ws -> focused = (ws -> n_clients > 0) ? &ws -> clients[ws -> n_clients - 1] : NULL;


  WM.current_workspace = workspace -> i;

  for (int i = 0; i < next_ws -> n_clients; i++) {
    XMapWindow(dpy, next_ws -> clients[i].window);
  }

  if(next_ws -> focused)
    XSetInputFocus(dpy, next_ws -> focused -> window, RevertToParent, CurrentTime);
  XRaiseWindow(dpy, next_ws -> focused -> window);

}


void cleanup_zombies(int sig) {
  while (waitpid(-1, NULL, WNOHANG) > 0);
}

void terminal(Display *dpy) {
  pid_t pid = fork();
  if (pid == 0) {
    setsid();
    execlp("/usr/bin/xterm", "xterm", NULL);
    perror("execlp");
    exit(1);
  }
}

void terminal_kitty(Display *dpy, const Arg *arg) {
  pid_t pid = fork();
  if (pid == 0) {
    setsid();
    setenv("WLR_BACKENDS", "x11", 1); 
    setenv("QT_QPA_PLATFORM", "xcb", 1);
    setenv("GDK_BACKEND", "x11", 1);

    execlp("env", "env", "-u", "WAYLAND_DISPLAY", "kitty", NULL);

    perror("execlp");
    exit(1);
  }
}


// this will be used for auto start commands
void auto_start(const char *argv[]) {
  pid_t pid = fork();

  if (pid == 0) {
    setsid();
    execvp(argv[0], (char *const *)argv);
    perror("execlp");
    exit(1);
  }
}

void kill_client(Display *dpy, const Arg *arg) {
    Workspace *ws = &WM.workspaces[WM.current_workspace];
    if (ws->focused) {
        XKillClient(dpy, ws->focused->window);
    }
}

void auto_start_cmds() {
  for(int i = 0; autostart_cmds[i][0] != NULL; i++) {
    auto_start(autostart_cmds[i]);
  }
}

void spawn(Display *dpy, const Arg *arg) {
  pid_t pid = fork();

  if (pid == 0) {
    setsid();
    execvp(arg -> c[0], (char *const *) arg -> c);
    perror("execlp");
    exit(1);
  }
}

void close_window(Display *dpy, const Arg *arg) {
  Workspace *ws = &WM.workspaces[WM.current_workspace];
  Window window = ws -> focused -> window;
  if (!ws -> focused) return;

  Atom WM_DELETE_WINDOW = XInternAtom(dpy, "WM_DELETE_WINDOW", False);
  Atom WM_PROTOCOLS = XInternAtom(dpy, "WM_PROTOCOLS", False);
  Atom proto = XInternAtom(dpy, "WM_DELETE_WINDOW", False);

  XEvent event = {0};


  if(has_proto(dpy, window, proto)) {
    event.xclient.type = ClientMessage;
    event.xclient.window = window ;
    event.xclient.message_type = WM_PROTOCOLS;
    event.xclient.format = 32;
    event.xclient.data.l[0] = WM_DELETE_WINDOW;
    event.xclient.data.l[1] = CurrentTime;
    XSendEvent(dpy, ws -> focused -> window, False, NoEventMask, &event);
  } else {
    XKillClient(dpy, window);
  }

  ws -> focused_idx = (ws -> n_clients > 0) ? ws -> n_clients - 1 : -1;
  ws -> focused = (ws -> n_clients > 0) ? &ws -> clients[ws -> focused_idx] : NULL;

  XSetInputFocus(dpy, ws -> focused -> window, RevertToParent, CurrentTime);

  XFlush(dpy);
  if(ws -> n_clients > 0)
    tile(dpy);
}

void activate_chord(Display *dpy, const Arg *arg) {
  KeyChordState = 1;

  if (XGrabKeyboard(dpy, DefaultRootWindow(dpy), True,
        GrabModeAsync, GrabModeAsync, CurrentTime) != GrabSuccess) {
    KeyChordState = 0; 
  }
}

void handle_chord(Display *dpy, const Chord chords[], int n_chords, KeySym key) {

  int matched = 0;

  for(int i = 0; i < n_chords; i++) {
    if(chords[i].key == key) {
      chords[i].action(dpy, &chords[i].arg);

      matched = 1;
      break;
    }
  }

  if(!matched) {
    KeyChordState = 0;
    XUngrabKeyboard(dpy, CurrentTime);
  }
}

void setup_ewmh(Display *dpy) {
  Window root = DefaultRootWindow(dpy);

  net_wm_name = XInternAtom(dpy, "_NET_WM_NAME", False);
  net_supporting_wm_check = XInternAtom(dpy, "_NET_SUPPORTING_WM_CHECK", False);
  net_current_desktop = XInternAtom(dpy, "_NET_CURRENT_DESKTOP", False);
  net_number_of_desktops = XInternAtom(dpy, "_NET_NUMBER_OF_DESKTOPS", False);
  net_active_window = XInternAtom(dpy, "_NET_ACTIVE_WINDOW", False);

  XChangeProperty(
      dpy, 
      root, 
      net_wm_name, 
      XInternAtom(dpy, "UTF8_STRING", False), 
      8, 
      PropModeReplace, 
      (unsigned char *) "PlaceHolderWM", 
      10
      );

  XChangeProperty(
      dpy, 
      root, 
      net_supporting_wm_check, 
      XA_WINDOW, 
      32, 
      PropModeReplace, 
      (unsigned char *) &root, 
      1
      );

  unsigned long num_ws = MAX_WORKSPACES;
  XChangeProperty(
      dpy, 
      root, 
      net_number_of_desktops, 
      XA_CARDINAL, 
      32, 
      PropModeReplace, 
      (unsigned char *) &num_ws, 
      1
      );
}

int main()
{
  // Getting display the window attirbues and the events
  Display *dpy;
  XWindowAttributes attr;
  XEvent event;

  if(!(dpy = XOpenDisplay(NULL))) return 1;

  setup_ewmh(dpy);
  init_wm();
  update_current_desktop(dpy);
  auto_start_cmds();

  Workspace *ws = &WM.workspaces[WM.current_workspace];

  int n_chords = sizeof(chords) / sizeof(chords[0]);

  int n_keys = sizeof(keybindings) / sizeof(Keybinding);

  unsigned int locks[] = {0, LockMask, Mod2Mask, LockMask | Mod2Mask};
  int n_locks = sizeof(locks)/sizeof(unsigned int);

  for (int i = 0; i < n_keys; i++) {
    for (int j = 0; j < n_locks; j++) {
      XGrabKey(dpy,
          XKeysymToKeycode(dpy, keybindings[i].keysym),
          keybindings[i].mod | locks[j],
          DefaultRootWindow(dpy),
          True, GrabModeAsync, GrabModeAsync);
    }
  }

  XSelectInput(dpy, DefaultRootWindow(dpy), SubstructureNotifyMask | SubstructureRedirectMask);

  signal(SIGCHLD, cleanup_zombies);

  // Main loop
  while(running) {
    XFlush(dpy);
    XNextEvent(dpy, &event);
    update_borders(dpy);
    Workspace *ws = &WM.workspaces[WM.current_workspace];

    switch (event.type) {
      case KeyPress:
        KeySym key = XLookupKeysym(&event.xkey, 0);
        unsigned int mods = event.xkey.state;
        unsigned int cleanmods = mods & (ShiftMask | ControlMask | Mod1Mask | Mod4Mask);

        if(KeyChordState == 1) {
          handle_chord(dpy, chords, n_chords, key);
          tile(dpy);
        }

        for(int i = 0; i < n_keys; i++) {
          if (keybindings[i].keysym == key && cleanmods == keybindings[i].mod) {
            keybindings[i].func(dpy, &keybindings[i].arg);
            break;
          }
        }

        tile(dpy);
        break;
      case MapRequest:
        Window w = event.xmaprequest.window;
        Atom type = get_window_type(dpy, w); 
        Atom dock = XInternAtom(dpy, "_NET_WM_WINDOW_TYPE_DOCK", False);

        if (type == dock) {
          top_bar = w;
          update_struts(dpy, w);
          XMapWindow(dpy, w);
          tile(dpy);
          break;
        }

        ws -> clients = realloc(ws -> clients, sizeof(Client) * (ws -> n_clients + 1));
        ws -> focused = &ws -> clients[ws -> n_clients];
        ws -> focused_idx = ws -> n_clients;
        ws -> clients[ws -> n_clients++].window = event.xmaprequest.window;
        tile(dpy);
        XMapWindow(dpy, event.xmaprequest.window);
        XSetInputFocus(dpy, ws -> focused -> window, RevertToParent, CurrentTime);
        XFlush(dpy);
        break;
      case DestroyNotify:
        if (event.xdestroywindow.window == top_bar) {
          top_bar = None;
          top_margin = 0;
          tile(dpy);
        }
        for(int i = 0; i < ws -> n_clients; i++) {
          if (ws -> clients[i].window == event.xdestroywindow.window) {
            for (int j = i; j < ws -> n_clients - 1; j++) ws -> clients[j] = ws -> clients[j+1];
            ws -> n_clients--;
            ws -> focused = (ws -> n_clients > 0) ? &ws -> clients[ws -> n_clients - 1] : NULL;
            ws -> focused_idx = ws -> n_clients - 1;
            break;
          }
        }
        tile(dpy);
        XFlush(dpy);
        break;
      case ConfigureRequest: 
        XConfigureRequestEvent *ev = &event.xconfigurerequest;
        XWindowChanges wc;
        wc.x = ev->x;
        wc.y = ev->y;
        wc.width = ev->width;
        wc.height = ev->height;
        wc.border_width = ev->border_width;
        wc.sibling = ev->above;
        wc.stack_mode = ev->detail;
        // Grant the window's request immediately so it doesn't hang
        tile(dpy);
        XConfigureWindow(dpy, ev->window, ev->value_mask, &wc);  
        tile(dpy);
        break;
      case PropertyNotify:
        if (event.xproperty.window == top_bar && 
            event.xproperty.atom == XInternAtom(dpy, "_NET_WM_STRUT_PARTIAL", False)) {
          update_struts(dpy, top_bar);
          tile(dpy);
        }
        break;
    }
  }
}


