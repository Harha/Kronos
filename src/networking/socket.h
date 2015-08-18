#ifndef SOCKET_H
#define SOCKET_H

#ifdef _WIN32

// std includes
#include "winsock2.h"

// kronos includes

namespace kronos
{

class Socket
{
public:
    Socket();
    ~Socket();
    int bindaddr(const char *ip, const uint32_t port);
    int listenaddr();
    void acceptcon(Socket &socket);
    void closecon();
    int sData(char *buffer, uint32_t size);
    int rData(char *buffer, uint32_t size);
    void setWsaData(const WSADATA wsadata);
    void setSocket(const SOCKET socket);
    void setAddress(const SOCKADDR_IN address);
    WSADATA getWsaData() const;
    SOCKET getSocket() const;
    SOCKET getSocketAccept() const;
    SOCKADDR_IN getAddress() const;
private:
    WSADATA m_wsaData;
    SOCKET m_socket;
    SOCKADDR_IN m_address;
};

}

#endif // _WIN32

#endif // SOCKET_H
