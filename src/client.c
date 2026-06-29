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

void insert_client(Workspace *ws, Client *client, int idx) {
  if(ws->clients.size == ws->clients.capacity) {
    ws->clients.data = realloc(ws->clients.data, ws->clients.capacity * 2 * sizeof(Client*));

    if(ws->clients.data == NULL) abort();

    ws->clients.capacity*=2;
  } 

  if(idx == ws->clients.size) {
    append_client(ws, client);
    return;
  }

  for(int i = ws->clients.size; i > idx; --i) {
    // Not using the swap_clients due to it only being allowed to swap non null indicies
    Client *client = ws->clients.data[i];
    ws->clients.data[i] = ws->clients.data[i - 1];
    ws->clients.data[i - 1] = client;
  }

  ws->clients.data[idx] = client;
}
