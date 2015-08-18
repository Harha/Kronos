// std includes
#include <iostream>

// kronos includes
#include "server.h"
#include "../macros.h"
#include "../utils/utils.h"

namespace kronos
{

Server::Server(uint32_t revision, uint32_t maxplayers) : m_revision(revision), m_maxplayers(maxplayers)
{
    m_listener = Socket();
    m_running = false;
    m_gameinit = false;
    m_listenerinit = false;
    m_initialized = false;

    LOG("Server: Created a new Server object.");
    LOG("Server: Revision: " << m_revision << " Max players: " << m_maxplayers);
}

Server::~Server()
{
    LOG("Server: Destroyed the Server object.");
}

int Server::bind(const std::string ip, const uint32_t port)
{
    return m_listener.bindaddr(ip.c_str(), port);
}

int Server::listen()
{
    return m_listener.listenaddr();
}

void Server::start()
{
    if (m_running)
        return;

    m_running = true;
}

void Server::stop()
{
    if (!m_running)
        return;

    m_running = false;

    m_listener.closecon();
}

void Server::run()
{
    if (!m_running)
        return;

    m_threadGame = std::thread(&tickgame, this);
    m_threadListener = std::thread(&ticklistener, this);

    int tries = 0;
    while (!(m_gameinit && m_listenerinit))
    {
        if (tries > 5)
        {
            stop();
            break;
        }

        LOG("Server: Waiting for game & listener threads to initialize...");
        tries++;
        Sleep(1000);
    }

    if (m_gameinit && m_listenerinit)
    {
        m_initialized = true;

        LOG("Server: Main server threads started.");
        LOG("Server: Processing user input on terminal...");
    }

    while (m_running)
    {
        std::string input;

        std::cout << "input: ";
        std::getline(std::cin, input);

        switch(cstr2int(input.c_str()))
        {
        case cstr2int("exit"):
        case cstr2int("shutdown"):
            LOG("Server: Shutdown requested. Killing threads...");
            stop();
            break;
        default:
            LOG("Server: Invalid command entered.");
            break;
        }
    }

    m_threadGame.join();
    m_threadListener.join();
}

void Server::tickgame()
{
    LOG("Server: Game main tick thread started...");

    m_gameinit = true;

    while (m_running)
    {
        if (!m_initialized)
            continue;

    }

    LOG("Server: Game main tick thread stopped...");
}

void Server::ticklistener()
{
    LOG("Server: Listener main tick thread started...");

    m_listenerinit = true;

    while (m_running)
    {
        if (!m_initialized)
            continue;

        Socket connection;
        m_listener.acceptcon(connection);

        LOG("Server: Accepted a new connection from <" << inet_ntoa(connection.getAddress().sin_addr) << ":" << ntohs(connection.getAddress().sin_port) << ">.")
        char buffer;
        connection.rData(&buffer, 1);
    }

    LOG("Server: Listener main tick thread stopped...");
}

}
