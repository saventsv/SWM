/* config.h */

/* Appearance */
static const int borders        = 2;
static const int gaps           = 10;
static const unsigned long active_color = 0x00FF00; 
static const unsigned long idle_color   = 0x444444;
static const float master_ratio         = 0.5;

/* Autostart */
static const char *autostart_cmds[][10] = {
    {NULL}
};

/* Keybindings */
#define MOD Mod1Mask
#define SHIFT ShiftMask

static const Keybinding keybindings[] = {
    /* modifier          key            function           argument */
    { MOD,               XK_Return,     terminal_kitty,    {.v = NULL} },
    { MOD,               XK_a,          activate_chord,    {.v = NULL} },
    { MOD,               XK_q,          close_window,      {.v = NULL} },
    { MOD|SHIFT,         XK_q,          kill_client,       {.v = NULL} },
    { MOD|SHIFT,         XK_e,          quit,              {.v = NULL} },
    
    /* Focus and Movement */
    { MOD,               XK_h,          focus,             {.i = 0} }, // Left/Master
    { MOD,               XK_l,          focus,             {.i = 1} }, // Right/Stack
    { MOD,               XK_j,          focus,             {.i = 2} }, // Up
    { MOD,               XK_k,          focus,             {.i = 3} }, // Down
    
    /* Workspace Switching (1-9) */
    { MOD,               XK_1,          move_workspace,    {.i = 0} },
    { MOD,               XK_2,          move_workspace,    {.i = 1} },
    /* ... add the rest ... */
};

/* Chords (Quick actions after MOD+A) */
static const Chord chords[] = {
    { XK_t, terminal_kitty, {.v = NULL} },
    { XK_1, move_workspace, {.i = 0}    },
};
