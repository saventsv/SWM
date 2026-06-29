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

