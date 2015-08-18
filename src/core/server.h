#ifndef SERVER_H
#define SERVER_H

// std includes
#include <string>
#include <atomic>
#include <mutex>
#include <thread>

// kronos includes
#include "../networking/socket.h"

namespace kronos
{

class Server
{
public:
    Server(uint32_t revision = 317, uint32_t maxplayers = 1028);
    ~Server();
    int bind(const std::string ip = "0.0.0.0", const uint32_t port = 43594);
    int listen();
    void start();
    void stop();
    void run();
private:
    void tickgame();
    void ticklistener();

    Socket m_listener;
    uint32_t m_revision;
    uint32_t m_maxplayers;
    std::atomic<bool> m_running;
    std::atomic<bool> m_gameinit;
    std::atomic<bool> m_listenerinit;
    std::atomic<bool> m_initialized;
    std::mutex m_mutex;
    std::thread m_threadGame;
    std::thread m_threadListener;
};

}

#endif // SERVER_H
