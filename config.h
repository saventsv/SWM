/* config.h */

/* Appearance */
#define BORDERS        2;
#define GAPS           5;
#define ACTIVE_COLOR   0x00FF00; 
#define IDLE_COLOR     0x444444;
#define MASTER_RATIO   0.5;

/* Autostart */


/* Keybindings */
#define MOD Mod1Mask
#define ALT Mod1Mask
#define SHIFT ShiftMask

static const Keybinding keybindings[] = {
    /* modifier          key            function           argument */
    { MOD,               XK_Return,     spawn,    {.c = "kitty"} },
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
    { MOD,               XK_3,          move_workspace,    {.i = 2} },
    { MOD,               XK_4,          move_workspace,    {.i = 3} },
    { MOD,               XK_5,          move_workspace,    {.i = 4} },
    { MOD,               XK_6,          move_workspace,    {.i = 5} },
    { MOD,               XK_7,          move_workspace,    {.i = 6} },
    { MOD,               XK_8,          move_workspace,    {.i = 7} },
    { MOD,               XK_9,          move_workspace,    {.i = 8} },
    /* ... add the rest ... */
    { ALT,               XK_space,      spawn,    {.c = "rofi -show drun"} },
};

/* Chords (Quick actions after MOD+A) */
static const Chord chords[] = {
    { XK_1, move_workspace, {.i = 0}    },
};
