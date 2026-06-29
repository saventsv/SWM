#pragma once
#include "wm.h"

Client *get_client_win(Workspace *ws, Window win);
void swap_clients(Workspace *ws, int idx1, int idx2);
void append_client(Workspace *ws, Client *client);
void insert_client(Workspace *ws, Client *client, int idx);
void remove_client(Workspace *ws, int idx);
ClientArray init_client_arr();

