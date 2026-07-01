#include "../include/client.h"
#include <stdlib.h>

ClientArray init_client_arr() {
  ClientArray client_arr;
  client_arr.capacity = 4;
  client_arr.size = 0;

  client_arr.data = malloc(client_arr.capacity * sizeof(Client*));

  if(client_arr.data == NULL) abort();

  return client_arr;
}

void append_client(Workspace *ws, Client *client) {
  if(ws->clients.size == ws->clients.capacity) {
    ws->clients.data = realloc(ws->clients.data, ws->clients.capacity * 2 * sizeof(Client*));

    if(ws->clients.data == NULL) abort();

    ws->clients.capacity*=2;
  } 
  ws->clients.data[ws->clients.size] = client;
  ws->clients.size++;
}

void swap_clients(Workspace *ws, int idx1, int idx2) {
  if(idx1 < 0 || idx1 > ws->clients.size - 1) return;
  if(idx2 < 0 || idx2 > ws->clients.size - 1) return;
  if(idx2 == idx1) return;

  Client *client = ws->clients.data[idx1];

  ws->clients.data[idx1] = ws->clients.data[idx2];
  ws->clients.data[idx2] = client;
}

void swap_idx(Workspace *ws, int idx1, int idx2) {
  if(idx1 < 0 || idx1 > ws->clients.capacity - 1) return;
  if(idx2 < 0 || idx2 > ws->clients.capacity - 1) return;
  if(idx2 == idx1) return;

  Client *client = ws->clients.data[idx1];

  ws->clients.data[idx1] = ws->clients.data[idx2];
  ws->clients.data[idx2] = client;
}


void insert_client(Workspace *ws, Client *client, int idx) {

  if(idx == ws->clients.size) {
    append_client(ws, client);
    return;
  }

  if(idx < 0 || idx > ws->clients.size) return;

  if(ws->clients.size == ws->clients.capacity) {
    ws->clients.data = realloc(ws->clients.data, ws->clients.capacity * 2 * sizeof(Client*));

    if(ws->clients.data == NULL) abort();

    ws->clients.capacity*=2;
  } 

  for(int i = ws->clients.size; i > idx; --i) {
    swap_idx(ws, i, i - 1);
  }

  ws->clients.data[idx] = client;
}

void remove_client(Workspace *ws, int idx) {
  if(idx < 0 || idx > ws->clients.size) return;

  Client *client = ws->clients.data[idx];

  int i = 0;
  for(int i = idx; i < ws->clients.size; i++) {
    swap_idx(ws, i, i + 1);
  }

  free(client);

  ws->clients.data[ws->clients.size] = NULL;

  ws->clients.size--;

  return;
}

Client *init_client(Window win) {
  Client *client = malloc(sizeof(Client*));
  client->x = 0;
  client->y = 0;
  client->width = 0;
  client->height = 0;
  return client;
}


int get_client(Workspace *ws, Window win) {
  for(int i = 0; i < ws->clients.size; i++) {
    if(ws->clients.data[i]->window == win) return i; 
  }

  return -1;
}
