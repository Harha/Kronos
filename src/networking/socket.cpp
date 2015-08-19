#ifdef _WIN32

// std includes
#include <iostream>

// kronos includes
#include "socket.h"
#include "../macros.h"

namespace kronos
{

Socket::Socket()
{
    if (WSAStartup(MAKEWORD(2, 2), &m_wsaData) != NO_ERROR)
    {
        ERR("Socket: Initialization error, WSAStartup returned != NO_ERROR.");
        ERR("Socket: Error message: " << WSAGetLastError());
        WSACleanup();
        std::exit(1);
    }

    m_socket = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
    if (m_socket == INVALID_SOCKET)
    {
        ERR("Socket: Initialization error, SOCKET returned INVALID_SOCKET.");
        ERR("Socket: Error message: " << WSAGetLastError());
        WSACleanup();
        std::exit(1);
    }
}

Socket::~Socket()
{
    WSACleanup();
}

int Socket::bindaddr(const char *ip, const uint32_t port)
{
    m_address.sin_family = AF_INET;
    m_address.sin_addr.s_addr = inet_addr(ip);
    m_address.sin_port = htons(port);

    if (bind(m_socket, (SOCKADDR *) &m_address, sizeof(m_address)) == SOCKET_ERROR)
    {
        ERR("Socket: Failed to bind to address <" << ip << "> on port <" << port << ">.");
        ERR("Socket: Error message: " << WSAGetLastError());
        WSACleanup();
        return 1;
    }

    LOG("Socket: Succesfully bound to address <" << ip << "> on port <" << port << ">.");
    return 0;
}

int Socket::listenaddr()
{
    if (listen(m_socket, 1) == SOCKET_ERROR)
    {
        ERR("Socket: Failed to listen on given address/port.");
        ERR("Socket: Error message: " << WSAGetLastError());
        WSACleanup();
        return 1;
    }

    LOG("Socket: Listening for connections...");
    return 0;
}

void Socket::acceptcon(Socket &socket)
{
    SOCKADDR_IN address;
    int address_l = sizeof(address);
    SOCKET connected = accept(m_socket, (SOCKADDR *) &address, &address_l);
    socket.setSocket(connected);
    socket.setAddress(address);
}

void Socket::closecon()
{
    closesocket(m_socket);

    LOG("Socket: Close called, stopped listening for connections.");
}

int Socket::sData(unsigned char *buffer, uint32_t size)
{
    int result = send(m_socket, reinterpret_cast<char *>(buffer), size, 0);

    if (result != SOCKET_ERROR)
    {
        LOG("Socket: Bytes sent: " << result);
        return result;
    }

    LOG("Socket: Send failed with an error: " << WSAGetLastError());
    closesocket(m_socket);
    WSACleanup();
    return result;
}

int Socket::rData(unsigned char *buffer, uint32_t size)
{
    int result = recv(m_socket, reinterpret_cast<char *>(buffer), size, 0);

    if (result > 0)
    {
        LOG("Socket: Bytes received: " << result);
        return result;
    }
    else if (result == 0)
    {
        LOG("Socket: Connection closed.");
        return result;
    }

    LOG("Socket: Recv failed with an error: " << WSAGetLastError());
    closesocket(m_socket);
    WSACleanup();
    return result;
}

void Socket::setWsaData(const WSADATA wsadata)
{
    m_wsaData = wsadata;
}

void Socket::setSocket(const SOCKET socket)
{
    m_socket = socket;
}

void Socket::setAddress(const SOCKADDR_IN address)
{
    m_address = address;
}

WSADATA Socket::getWsaData() const
{
    return m_wsaData;
}

SOCKET Socket::getSocket() const
{
    return m_socket;
}

SOCKADDR_IN Socket::getAddress() const
{
    return m_address;
}

}

#endif // _WIN32
