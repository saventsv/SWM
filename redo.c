#include <X11/Xatom.h>
#include <X11/Xlib.h>
#include <X11/keysym.h>
#include <sys/wait.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <unistd.h>
#include <X11/X.h>

// TODO figure out how to handle floating windows with linked lists and tiling

/* ==================== Global Constant Definitions ==================== */

#define MAX_WORKSPACES 10
#define MAX_CHORD 4
#define MOD Mod1Mask

// Temporary
#define COLOR_ACTIVE "#5294e2"   
#define COLOR_INACTIVE "#444444"
#define GAPS 5
#define BORDER_WIDTH 2
#define MASTER_RATIO 0.5
const char *startup_commands[] = {
  "xterm",
  NULL         
};

/* ==================== Struct Definitions ==================== */

// Basic Heirarchy WindowManager -> Workspace -> Client
typedef struct Client
{
  Window window;
  int x, y, width, height;
  // Linked list
  struct Client *next_client;
} Client;

typedef struct
{
  Client *master_client;
  Client *focused;
  int n_clients;
  int id;
} Workspace;

typedef struct 
{
  int current_workspace;
  Workspace workspaces[MAX_WORKSPACES];
  int key_state;
  int running;
} WindowManager;

// Argument for shell commands and startup commands
typedef union 
{
  int i;
  Client client;
  const char *c;
  const void *v;
} Arg;


// Keybinds and Key Chords

typedef struct 
{
  unsigned int mod;
  KeySym key;
  // Funciton pointer for callbacks
  void (*func)(Display *dpy, const Arg *arg);
  const Arg arg;
} Keybinding;

typedef struct 
{
  KeySym key;
  void (*func)(Display *dpy, const Arg *arg);
  const Arg arg;
} Chord;


/* ==================== Global Variable Definitions ==================== */

WindowManager WM;


/* ==================== Function Definitions ==================== */

// Include user config file
// TODO uncomment when done
// #include "config.h"   

/* MISC */

unsigned long get_color(Display *dpy, const char *color_name) {
    Colormap cmap = DefaultColormap(dpy, DefaultScreen(dpy));
    XColor color;
    XAllocNamedColor(dpy, cmap, color_name, &color, &color);
    return color.pixel;
}

void update_borders(Display *dpy) {
  Workspace *ws = &WM.workspaces[WM.current_workspace];
  unsigned long active_px = get_color(dpy, COLOR_ACTIVE);
  unsigned long inactive_px = get_color(dpy, COLOR_INACTIVE);

  Client *client = ws -> master_client;

  while(client)
  {
    if(client == ws -> focused)
    {
      XSetWindowBorder(dpy, client -> window, active_px);
    }
    else
    {
      XSetWindowBorder(dpy, client -> window, inactive_px);
    }
    client = client -> next_client;
  }
}

void init_wm()
{
  for(int i = 0; i < MAX_WORKSPACES; i++)
  {
    WM.workspaces[i].master_client = NULL;
    // -1 means no windows
    WM.workspaces[i].focused = NULL;
    WM.workspaces[i].n_clients = 0;
  }
  WM.current_workspace = 0;
  WM.running = 1;
}


/* Windows */

void tile(Display *dpy)
{

  Workspace *ws = &WM.workspaces[WM.current_workspace];
  if( !ws -> master_client) return;

  // Needed Declarations
  int screen = DefaultScreen(dpy);
  int screen_width = DisplayWidth(dpy, screen);
  int screen_height = DisplayHeight(dpy, screen);
  // MASTER_RATIO from conifig file
  int master_width = ( ws -> n_clients > 1 ) ? screen_width * MASTER_RATIO: screen_width ;
  int master_height = screen_height;
  int stack_width = screen_width - master_width;
  int stack_x = master_width;


  Client *client = ws -> master_client;

  unsigned long active_px = get_color(dpy, COLOR_ACTIVE);
  unsigned long inactive_px = get_color(dpy, COLOR_INACTIVE);

  int i = 0;
  while(client) 
  {

    if ( i == 0 ) 
    {
      XSetWindowBorderWidth(dpy, client -> window, BORDER_WIDTH);
      if(client == ws -> focused)
        XSetWindowBorder(dpy, client -> window, active_px);
      else
        XSetWindowBorder(dpy, client -> window, inactive_px);
      XMoveResizeWindow(
          dpy, 
          client -> window, 
          0 + GAPS, 
          0 + GAPS, 
          master_width - (GAPS * 2) - (BORDER_WIDTH * 2), 
          master_height - (GAPS * 2) - (BORDER_WIDTH * 2)
          );
    }
    else
    {
      int stack_count = ws -> n_clients - 1;
      int stack_height = screen_height / stack_count;
      int stack_y = stack_height * (i - 1);
      XSetWindowBorderWidth(dpy, client -> window, BORDER_WIDTH);
      if(client == ws -> focused)
        XSetWindowBorder(dpy, client -> window, active_px);
      else
        XSetWindowBorder(dpy, client -> window, inactive_px);

      XMoveResizeWindow(
          dpy, 
          client -> window, 
          stack_x + GAPS, 
          stack_y + GAPS, 
          stack_width - (GAPS * 2) - (BORDER_WIDTH * 2), 
          stack_height - (GAPS * 2) - (BORDER_WIDTH * 2)
          );
    }
    client = client -> next_client;
    i++;
  }
}

// left = 0
// right = 1
// up = 2
// down = 3
void focus(Display *dpy, const Arg *arg) 
{
  Workspace *ws = &WM.workspaces[WM.current_workspace];
  Client *client = ws -> master_client;
  // I will be used as a index
  int i = 0;

  if(!arg || arg -> i > 3 || arg -> i < 0) return;
  if(ws -> n_clients < 2) return;

  switch(arg -> i)
  {
    
    case 0:
      {
        while(client)
        {
          if(client == ws -> focused)
          {
            if(i > 0)
            {
              ws -> focused = ws -> master_client;
              XSetInputFocus(dpy, ws -> focused -> window, RevertToPointerRoot, CurrentTime);
              break;
            }
            else 
            {
              ws -> focused = ws -> master_client -> next_client;
              XSetInputFocus(dpy, ws -> focused -> window, RevertToPointerRoot, CurrentTime);
              break;
            }
          }
          i++;
          client = client -> next_client;
        }
        break;
      }
    case 1:
      {
        while(client)
        {
          if(client == ws -> focused)
          {
            if(i == 0)
            {
              ws -> focused = ws -> master_client -> next_client;
              XSetInputFocus(dpy, ws -> focused -> window, RevertToPointerRoot, CurrentTime);
              break;
            }
            else
            {
              ws -> focused = ws -> master_client;
              XSetInputFocus(dpy, ws -> focused -> window, RevertToPointerRoot, CurrentTime);
              break;
            }
          }
          i++;
          client = client -> next_client;
        }
        break;
      }
    case 2:
      {
        while(client)
        {
          if(client == ws -> focused)
          {
            if(i == 0)
            {
              while(client -> next_client != NULL)
              {
                client = client -> next_client;
              }
              ws -> focused = client;
              XSetInputFocus(dpy, ws -> focused -> window, RevertToPointerRoot, CurrentTime);
              break;
            }
            else if (i == 1)
            {
              ws -> focused = ws -> master_client;
              XSetInputFocus(dpy, ws -> focused -> window, RevertToPointerRoot, CurrentTime);
              break;
            } else
            {
              Client *prev_client = ws -> master_client;
              while(prev_client -> next_client != client)
              {
                prev_client = prev_client -> next_client;
              }
              ws -> focused = prev_client;
              XSetInputFocus(dpy, ws -> focused -> window, RevertToPointerRoot, CurrentTime);
              break;
            }
          }
          i++;
          client = client -> next_client;
        }
        break;
      }
    case 3:
      {
        while(client)
        {
          if(client == ws -> focused)
          {
            if(client -> next_client == NULL)
            {
              ws -> focused = ws -> master_client;
              XSetInputFocus(dpy, ws -> focused -> window, RevertToPointerRoot, CurrentTime);
              break;
            }
            else
            {
              ws -> focused = ws -> focused -> next_client;
              XSetInputFocus(dpy, ws -> focused -> window, RevertToPointerRoot, CurrentTime);
              break;
            }
          }
          i++;
          client = client -> next_client;
        }
        break;
      }
  }
}



/* Keybinds */
void handle_chord(Display *dpy, const Chord chords[], int n_chords, KeySym key) {

  int matched = 0;

  for(int i = 0; i < n_chords; i++) {
    if(chords[i].key == key) {
      chords[i].func(dpy, &chords[i].arg);

      matched = 1;
      break;
    }
  }

  if(!matched) {
    WM.key_state = 0;
    XUngrabKeyboard(dpy, CurrentTime);
  }
}

void activate_chord(Display *dpy, const Arg *arg) {
  WM.key_state = 1;

  if (XGrabKeyboard(dpy, DefaultRootWindow(dpy), True,
        GrabModeAsync, GrabModeAsync, CurrentTime) != GrabSuccess) {
    WM.key_state = 0; 
  }
}





/* Executing Commands */

void spawn(Display *dpy, const Arg *arg)
{
  if (!arg || !arg -> c) return;

  pid_t pid = fork();

  if (pid == 0)
  {
    setsid();
    execlp(arg -> c, arg -> c, NULL);
    perror("execlp");
    exit(1);
  }
}

void run_startup(Display *dpy) 
{
  for (int i = 0; startup_commands[i] != NULL; i++) 
  {
    Arg arg = { .c = startup_commands[i] };
    spawn(dpy, &arg);
  }
}


/* ==================== Place Holders ==================== */

void kitty(Display *dpy, const Arg *arg) 
{
  pid_t pid = fork();
  if (pid == 0) 
  {
    setsid();
    setenv("WLR_BACKENDS", "x11", 1); 
    setenv("QT_QPA_PLATFORM", "xcb", 1);
    setenv("GDK_BACKEND", "x11", 1);

    execlp("env", "env", "-u", "WAYLAND_DISPLAY", "kitty", NULL);

    perror("execlp");
    exit(1);
  }
}



static const Keybinding keybindings[] = {
  /* modifier          key            function           argument */
  { MOD,               XK_Return,     kitty,             { .v = NULL }    },
  { MOD,               XK_a,          activate_chord,    { .v = NULL }    },
  { MOD,               XK_b,          spawn,             { .c = "xterm" } },
  { MOD,               XK_h,          focus,             { .i = 0 }       },
  { MOD,               XK_l,          focus,             { .i = 1 }       },
  { MOD,               XK_k,          focus,             { .i = 2 }       },
  { MOD,               XK_j,          focus,             { .i = 3 }       }
};

static const Chord chords[] = {
  { XK_1, kitty, {.v = NULL} },
};



/* ==================== Main ==================== */

int main()
{
  // Get Monitor and Events
  Display *dpy;
  XEvent event;

  // Open Display
  if(!(dpy = XOpenDisplay(0x0))) return 1;

  // Init Window Manager
  init_wm();
  run_startup(dpy);

  // Keybind Declarations
  unsigned int locks[] = {0, LockMask, Mod2Mask, LockMask | Mod2Mask};

  int n_chords = sizeof(chords) / sizeof(chords[0]);
  int n_keys = sizeof(keybindings) / sizeof(keybindings[0]);
  int n_locks = sizeof(locks) / sizeof(unsigned int);

  // Loop to have X11 pass all the keys we want to us
  for (int i = 0; i < n_keys; i++)
  {
    for(int j = 0; j < n_locks; j++) 
    {
      XGrabKey(
          dpy, 
          XKeysymToKeycode(dpy, keybindings[i].key), 
          keybindings[i].mod | locks[j], 
          DefaultRootWindow(dpy), 
          True, 
          GrabModeAsync, 
          GrabModeAsync
          );
    }
  }

  // Get Input From X11
  XSelectInput(dpy, DefaultRootWindow(dpy), SubstructureNotifyMask | SubstructureRedirectMask);

  while(WM.running) 
  {
    XFlush(dpy);
    XNextEvent(dpy, &event);

    switch(event.type)
    {
      // Handle KeyPresses
      case KeyPress:
        {
          // Definitions
          KeySym key = XLookupKeysym(&event.xkey, 0);
          unsigned int mods = event.xkey.state;
          unsigned int cleanmods = mods & ( ShiftMask | ControlMask | Mod1Mask | Mod2Mask);

          // Means We Are In a Key Chord
          if(WM.key_state == 1) 
          {
            handle_chord(dpy, chords, n_chords, key);
            tile(dpy);
            update_borders(dpy);
          }

          // Call regular binds
          for(int i = 0; i < n_keys; i++)
          {
            if (keybindings[i].key == key && cleanmods == keybindings[i].mod)
            {
              keybindings[i].func(dpy, &keybindings[i].arg);
              break;
            }
          }

          // Tile to be safe
          tile(dpy);
          update_borders(dpy);
          break;
        }


        // Called when a window is summoned
      case MapRequest:
        {

          Workspace *ws = &WM.workspaces[WM.current_workspace];
          // Make space for Client
          Client *client = malloc(sizeof(Client));

          // If malloc fails break
          if(!client)
            break;

          ws -> n_clients++;

          client -> window = event.xmaprequest.window;
          client -> next_client = NULL;
          client -> x = 0;
          client -> y = 0;
          client -> width = 0;
          client -> height = 0;

          Client *current_client = ws -> master_client;

          if(current_client == NULL)
          {
            ws -> master_client = client;
            XMapWindow(dpy, client -> window);
            ws -> focused = client;
            tile(dpy);
            XSetInputFocus(dpy, client -> window, RevertToPointerRoot, CurrentTime);
            break;
          }
          else
          {
            // Iterate Through Clients
            while(current_client -> next_client != NULL)
            {
              current_client = current_client -> next_client;
            }

            // When the Last Client is Found Set its next_client Equal to The New Client
            current_client -> next_client = client;
            XMapWindow(dpy, client -> window);
            ws -> focused = client;
            tile(dpy);
            update_borders(dpy);
            XSetInputFocus(dpy, client -> window, RevertToPointerRoot, CurrentTime);
          }
          break;
        }


        // When a window requests a size
      case ConfigureRequest:

        // Get the event so we have access to wanted size
        XConfigureRequestEvent *configure_event = &event.xconfigurerequest;
        XWindowChanges wc;

        // We give it the wanted dimensions to increase speed and so it does not complain
        wc.x = configure_event -> x;
        wc.y = configure_event -> y;
        wc.width = configure_event -> width;
        wc.height = configure_event -> height;
        wc.border_width = configure_event -> border_width;
        wc.sibling = configure_event -> above;
        wc.stack_mode = configure_event -> detail;

        tile(dpy);
        update_borders(dpy);
        XConfigureWindow(
            dpy, 
            configure_event -> window, 
            configure_event -> value_mask,
            &wc
            );
        // Just to make sure it is the dimensions we want it to be
        tile(dpy);
        break;

        // When a window closes
      case DestroyNotify:
        {
          // Definitions
          Workspace *ws = &WM.workspaces[WM.current_workspace];
          Client *client = ws -> master_client;
          Client *prev_client = NULL;

          // Loop Through and adjust pointers
          while(client)
          {
            if (client -> window == event.xdestroywindow.window)
            {

              if(prev_client == NULL) 
              {
                if(client -> next_client != NULL)
                {
                  XSetInputFocus(dpy, client -> next_client -> window, RevertToPointerRoot, CurrentTime);
                  ws -> focused = client -> next_client;
                }
                ws -> master_client = client -> next_client;
                ws -> focused = NULL;
              } 
              else 
              {
                XSetInputFocus(dpy, prev_client -> window, RevertToPointerRoot, CurrentTime);
                ws -> focused = prev_client;
                prev_client -> next_client = client -> next_client;
              }

              free(client);
              ws -> n_clients--;
              break;
            }
            prev_client = client;
            client = client -> next_client;
          }
          tile(dpy);
          update_borders(dpy);
          break;
        }

    }
  }

}





























