
#ifndef _NETWORK
#define _NETWORK

bool Network_Initialize();
bool Network_Cleanup();

int Network_StartServer(int port);	// return a socket id
void Network_StopServer(int lSocket);

int Network_ClientSend(int socket, int port, void *buf, size_t buf_size);

#endif