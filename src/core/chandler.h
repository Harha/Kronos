#ifndef CHANDLER_H
#define CHANDLER_H

// std includes

// kronos includes
#include "../networking/socket.h"

namespace kronos
{

enum login_t
{
    STATUS_EXCHANGE_DATA = 0,
    STATUS_DELAY = 1,
    STATUS_OK = 2,
    STATUS_INVALID_CREDENTIALS = 3,
    STATUS_ACCOUNT_DISABLED = 4,
    STATUS_ACCOUNT_ONLINE = 5,
    STATUS_GAME_UPDATED = 6,
    STATUS_SERVER_FULL = 7,
    STATUS_LOGIN_SERVER_OFFLINE = 8,
    STATUS_TOO_MANY_CONNECTIONS = 9,
    STATUS_BAD_SESSION_ID = 10,
    STATUS_LOGIN_SERVER_REJECTED_SESSION = 11,
    STATUS_MEMBERS_ACCOUNT_REQUIRED = 12,
    STATUS_COULD_NOT_COMPLETE = 13,
    STATUS_UPDATING = 14,
    STATUS_RECONNECTION_OK = 15,
    STATUS_TOO_MANY_LOGINS = 16,
    STATUS_IN_MEMBERS_AREA = 17,
    STATUS_INVALID_LOGIN_SERVER = 20,
    STATUS_PROFILE_TRANSFER = 21,
    TYPE_HANDSHAKE = 14,
    TYPE_UPDATE = 15,
    TYPE_STANDARD = 16,
    TYPE_JAGGRAB = 17,
    TYPE_RECONNECTION = 18
};

struct LoginResponse
{
    LoginResponse(unsigned char status = STATUS_LOGIN_SERVER_REJECTED_SESSION, unsigned char rights = 0, unsigned char flagged = 0) :
        status(status), rights(rights), flagged(flagged)
    {

    }

    unsigned char status;
    unsigned char rights;
    unsigned char flagged;
};

struct LoginRequest
{
    LoginRequest(char *username = "null", char *password = "null", unsigned short cversion = 317, unsigned short crelease = 317,
                 unsigned char cdetail = 0, unsigned char creconnecting = 0) : username(username), password(password), client_release(crelease),
        client_detail(cdetail), client_reconnecting(creconnecting)
    {

    }

    char *username;
    char *password;
    unsigned short client_version;
    unsigned short client_release;
    unsigned char client_detail;
    unsigned char client_reconnecting;
};

class ConnectionHandler
{
public:
    ConnectionHandler();
    int processLogin(Socket &socket);
    int processHandshake(Socket &socket);
    int processHeader(Socket &socket);
    int processPayload(Socket &socket);
    int processRejection(Socket &socket, login_t status);
    int processSuccess(Socket &socket, LoginRequest loginrequest);
private:
    unsigned char m_loginlength;
    unsigned char m_reconnecting;
    unsigned long m_serverseed;
    unsigned char m_usernamehash;
};

}

#endif // CHANDLER_H
