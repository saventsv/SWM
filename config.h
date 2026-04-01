/* config.h */
#include <X11/Xlib.h>

/* Appearance */

static const char *color_active = "#5e81ac";
static const char *color_chord = "#b48ead";
static const char *color_inactive = "#3b4252";
static const int gaps = 5;
static const int border_width = 2;
static const double master_ratio = 0.5;
static const double scratchpad_width = 0.8;
static const double scratchpad_height = 0.7;
const char *startup_commands[] = {
  "firefox",
  NULL         
};


/* Autostart */

static const Scratchpad scratchpads[] = {
  { "alacritty", "alacritty -class scratchpad", "scratchpad" },
};


/* Keybindings */
#define MOD Mod4Mask
#define ALT Mod1Mask
#define SHIFT ShiftMask

static const Keybinding keybindings[] = {
  /* modifier          key            function                argument */

  /* Processes */
  { MOD,               XK_Return,     spawn,                  { .c = "kitty"}  },
  { MOD,               XK_space,      activate_chord,         { .v = NULL }    },
  { MOD | SHIFT,       XK_e,          quit,                   { .v = NULL }    },

  /* Windows */

  /* Focusing */
  { MOD,               XK_h,          focus,                  { .d = LEFT }    },
  { MOD,               XK_l,          focus,                  { .d = RIGHT }   },
  { MOD,               XK_k,          focus,                  { .d = UP }       },
  { MOD,               XK_j,          focus,                  { .d = DOWN }       },

  /* Moving Windows */
  { MOD | SHIFT,       XK_h,          move_window,            { .d = LEFT }    },
  { MOD | SHIFT,       XK_l,          move_window,            { .d = RIGHT }   },
  { MOD | SHIFT,       XK_k,          move_window,            { .d = UP }      },
  { MOD | SHIFT,       XK_j,          move_window,            { .d = DOWN }    },

  /* Focusing Workspace */
  { MOD,               XK_1,          focus_workspace,        { .i = 0 }       },
  { MOD,               XK_2,          focus_workspace,        { .i = 1 }       },
  { MOD,               XK_3,          focus_workspace,        { .i = 2 }       },
  { MOD,               XK_4,          focus_workspace,        { .i = 3 }       },
  { MOD,               XK_5,          focus_workspace,        { .i = 4 }       },
  { MOD,               XK_6,          focus_workspace,        { .i = 5 }       },
  { MOD,               XK_7,          focus_workspace,        { .i = 6 }       },
  { MOD,               XK_8,          focus_workspace,        { .i = 7 }       },
  { MOD,               XK_9,          focus_workspace,        { .i = 8 }       },

  /* Moving Windows Between Workspaces */
  { MOD | SHIFT,       XK_1,          move_window_workspace,  { .i = 0 }       },
  { MOD | SHIFT,       XK_2,          move_window_workspace,  { .i = 1 }       },
  { MOD | SHIFT,       XK_3,          move_window_workspace,  { .i = 2 }       },
  { MOD | SHIFT,       XK_4,          move_window_workspace,  { .i = 3 }       },
  { MOD | SHIFT,       XK_5,          move_window_workspace,  { .i = 4 }       },
  { MOD | SHIFT,       XK_6,          move_window_workspace,  { .i = 5 }       },
  { MOD | SHIFT,       XK_7,          move_window_workspace,  { .i = 6 }       },
  { MOD | SHIFT,       XK_8,          move_window_workspace,  { .i = 7 }       },
  { MOD | SHIFT,       XK_9,          move_window_workspace,  { .i = 8 }       },

  { MOD,               XK_q,          close_window,           { .v = NULL }    },
};

static const Chord chords[] = {
  { XK_t, toggle_scratchpad, {.i = 0} },
};

