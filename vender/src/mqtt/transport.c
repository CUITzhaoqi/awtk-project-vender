#include "transport.h"
#include "winsock.h"

static SOCKET mqtt_socket;
int transport_sendPacketBuffer(unsigned char *buf, int buflen)
{
    int rc;
    rc = send(mqtt_socket, buf, buflen, 0);  
    if (rc != buflen) {  

    }
    return rc;
}

int transport_getdata(unsigned char *buf, int count)
{
    int rc;
    rc = recv(mqtt_socket, buf, count, 0);
    if (rc == 0) {
        
    }
    return rc;
}

SOCKET transport_open(unsigned char* servip, int port)
{
    uint32_t ret;
    WSADATA Data;  
     struct sockaddr_in destSockAddr;  
    SOCKET destSocket;  
    unsigned long destAddr;  
    int status;  
    int numsnt;  
  
    /* initialize the Windows Socket DLL */  
    status=WSAStartup(MAKEWORD(2, 2), &Data);  
    if (status != 0) {
        log_debug("ERROR: WSAStartup unsuccessful");  
    }
    
    /* specify the port portion of the address */  
    destSockAddr.sin_port=htons(port);  
    /* specify the address family as Internet */  
    destSockAddr.sin_family=AF_INET;  
    destSockAddr.sin_addr.s_addr = inet_addr(servip);
    /* create a socket */  
    destSocket=socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);  
    if (destSocket == INVALID_SOCKET) {  
        log_debug("ERROR: socket unsuccessful");  
        status=WSACleanup();  
        if (status == SOCKET_ERROR) {
            log_debug("ERROR: WSACleanup unsuccessful");  
        }
        return(1);  
    }

    log_debug("Trying to connect to IP Address: ");  
  
    /* connect to the server */  
    status=connect(destSocket, (struct sockaddr *)&destSockAddr, sizeof(destSockAddr));  
    if (status == SOCKET_ERROR)  
    {  
        log_debug("ERROR: connect unsuccessful");  
            
        status=closesocket(destSocket);  
        if (status == SOCKET_ERROR) {
            log_debug("ERROR: closesocket unsuccessful");  
        }
        status=WSACleanup();  
        if (status == SOCKET_ERROR) {
            log_debug("ERROR: WSACleanup unsuccessful");  
        }
            
        return(1);  
    }  
    mqtt_socket = destSocket;
    return destSocket;
}

int8_t transport_close()
{
    uint32_t rc;
    rc = closesocket(mqtt_socket);
    WSACleanup();
    return rc;
}
