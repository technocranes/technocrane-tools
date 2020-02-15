
#define WS_VERSION_REQUIRED 0x0101
#include <winsock.h>
#include <windows.h>
#include <fstream>
#include <iostream>


bool Network_Cleanup()
{
    if (WSACleanup()) 
	{
	  //         GetErrorStr();
        WSACleanup();
        
        return false;
    }
    return true;
}
bool Network_Initialize()
{
    WSADATA wsadata;
    if (WSAStartup(MAKEWORD(2,2), &wsadata)) 
	{
	  //          GetErrorStr();
        Network_Cleanup();
        return false;
    }
    return true;
}

void bzero(char *b, size_t length)
{
    memset( b,0,length );
}


int Network_ClientSend(int socket, int shapeHandPort, void *buf, size_t buf_size)
{
	struct sockaddr_in serv_addr;
	serv_addr.sin_family = AF_INET;
	serv_addr.sin_addr.s_addr = inet_addr( "127.0.0.1" );
	serv_addr.sin_port = htons( shapeHandPort );

	int nbytes_send = sendto( socket, 
		(const char*) buf,
		buf_size, 
		0, 
		(struct sockaddr*)&serv_addr, 
		sizeof(serv_addr) );

	if (nbytes_send != buf_size )
	{
		printf( " > error sending a request packet\n" );
		printf("sendto failed with error: %d\n", WSAGetLastError() );
		return 0;
	}
	if (nbytes_send == SOCKET_ERROR )
	{
		printf (" > socket error while sending a request\n" );
		return 0;
	}

	//printf( "packet sent - %d\n", nbytes_send);

	return nbytes_send;
}


int Network_StartServer(int pluginPort)
{
    int lSocket;
    struct protoent* lP;
    struct sockaddr_in  lSin;

    lP = getprotobyname("tcp");
    
	lSocket = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP /*lP->p_proto*/);
    
    if (lSocket)
    {
        bzero((char *)&lSin, sizeof(lSin));

		lSin.sin_family = AF_INET;
		lSin.sin_port = htons(pluginPort);
		lSin.sin_addr.s_addr = 0L;

		//Bind socket
		if( bind(lSocket, (struct sockaddr*)&lSin, sizeof(lSin)) < 0 )
		{
			printf("failed to bind a socket\n");
			closesocket(lSocket);
			return 0;
		}

		DWORD nonBlocking = 1;
        if ( ioctlsocket( lSocket, FIONBIO, &nonBlocking ) != 0 )
        {
            printf( "failed to set non-blocking socket\n" );
			closesocket(lSocket);
            return 0;
        }
	}

	return lSocket;
}

void Network_StopServer(int lSocket)
{
	if (lSocket) 
    {
        closesocket( lSocket );
	}
}
