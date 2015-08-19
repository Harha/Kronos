// std includes
#include <iostream>

// c includes
#include <cstring>

// kronos includes
#include "chandler.h"
#include "../macros.h"
#include "../networking/buffer.h"

namespace kronos
{

ConnectionHandler::ConnectionHandler()
{
    LOG("CHandler: Created a new ConnectionHandler object.");
}

int ConnectionHandler::processLogin(Socket &socket)
{
    unsigned char connection_t;

    // Get the type of the connection
    if (socket.rData(&connection_t, 1) != 1)
    {
        ERR("CHandler: Invalid or corrupted connection request.");
        return 1;
    }

    // Init a variable used to store the result
    int result = 0;

    // Handle each connection type separately
    switch (connection_t)
    {
    case TYPE_HANDSHAKE:
        result = processHandshake(socket);
        break;
    case TYPE_UPDATE:
        result = 1;
        ERR("CHandler: Update connection request type not supported.");
        break;
    case TYPE_STANDARD:
    case TYPE_RECONNECTION:
        result = processHeader(socket);
        break;
    case TYPE_JAGGRAB:
        result = 1;
        ERR("CHandler: Jaggrab connection request type not supported.");
        break;
    }

    return result;
}

int ConnectionHandler::processHandshake(Socket &socket)
{
    LOG("CHandler: Processing handshake connection request.");

    // Allocate i/o buffers for us
    Buffer buffer_i(1);
    Buffer buffer_o(17);

    // Get the input name hash
    if (socket.rData(buffer_i.getData(), 1) < 1)
    {
        ERR("CHandler: Error while processing handshake, didn't receive the username hash.");
        return 1;
    }

    unsigned char m_usernamehash = buffer_i.read_byte();
    unsigned long m_serverseed = static_cast<unsigned long>(rand());

    // Send a successful response
    buffer_o.write_byte(STATUS_EXCHANGE_DATA); // Response status.
    buffer_o.write_long(0); // Client seed.
    buffer_o.write_long(m_serverseed); // Server seed, to be used later.

    if (socket.sData(buffer_o.getDataStart(), buffer_o.getData() - buffer_o.getDataStart()) < 17)
    {
        ERR("CHandler: Error, failed to send a correct handshake response.");
        return 1;
    }

    // Continue to process the header next
    return processHeader(socket);
}

int ConnectionHandler::processHeader(Socket &socket)
{
    LOG("CHandler: Processing header connection request.");

    // Allocate the i buffer for us
    Buffer buffer_i(2);

    // Get the request
    if (socket.rData(buffer_i.getData(), 2) < 2)
    {
        ERR("CHandler: Error while processing header, received wrong or corrupted data.");
        return 1;
    }

    // Get the type of the connection
    int type = buffer_i.read_byte();

    if (type != TYPE_STANDARD && type != TYPE_RECONNECTION)
    {
        return processRejection(socket, STATUS_LOGIN_SERVER_REJECTED_SESSION);
    }

    m_reconnecting = type = TYPE_RECONNECTION;
    m_loginlength = buffer_i.read_byte();

    // Continue to process the payload next
    return processPayload(socket);
}

int ConnectionHandler::processPayload(Socket &socket)
{
    LOG("CHandler: Processing payload connection request.");

    // Allocate the i buffer for us
    Buffer buffer_i(m_loginlength);

    // Get the request
    if (socket.rData(buffer_i.getData(), m_loginlength) < m_loginlength)
    {
        ERR("CHandler: Error while processing payload, received wrong or corrupted data.");
        return 1;
    }

    // Start reading the data
    LoginRequest loginrequest;
    loginrequest.client_version = 255 - buffer_i.read_byte();
    loginrequest.client_release = buffer_i.read_short();
    loginrequest.client_detail = buffer_i.read_byte();

    // Reject the session if client detail isn't 0 or 1
    if (loginrequest.client_detail != 0 && loginrequest.client_detail != 1)
    {
        return processRejection(socket, STATUS_LOGIN_SERVER_REJECTED_SESSION);
    }

    // CRCs, we don't care about them for now...
    for (size_t i = 0; i < 9; i++)
    {
        buffer_i.read_int();
    }

    // Message length, provided from client as well
    unsigned int length = buffer_i.read_byte();
    if (length != m_loginlength - 41)
    {
        return processRejection(socket, STATUS_LOGIN_SERVER_REJECTED_SESSION);
    }

    // Read id
    unsigned int id = buffer_i.read_byte();

    if (id != 10)
    {
        return processRejection(socket, STATUS_LOGIN_SERVER_REJECTED_SESSION);
    }

    // Client / Server seeds
    unsigned long clientSeed = buffer_i.read_long();
    unsigned long serverSeed = buffer_i.read_long();

    /*
    if (serverSeed != m_serverseed)
    {
        ERR("CHandler: The seed provided by the client doesn't match with server's, rejected session.");
        return processRejection(socket);
    }
    */

    // Continue reading user data
    unsigned int uid = buffer_i.read_int();
    char *username = buffer_i.read_string();
    char *password = buffer_i.read_string();

    // Reject if username or password lengths aren't inside required bounds
    if (strlen(password) < 6 || strlen(password) > 20 || strlen(username) < 1 || strlen(username) > 12)
    {
        return processRejection(socket, STATUS_LOGIN_SERVER_REJECTED_SESSION);
    }

    // Continue reading / assigning user data
    loginrequest.username = username;
    loginrequest.password = password;

    // Process the so far successful request
    return processSuccess(socket, loginrequest);
}

int ConnectionHandler::processRejection(Socket &socket, login_t status)
{
    // Allocate the o buffer for us
    Buffer buffer_o(1);

    // Send a rejected response
    buffer_o.write_byte(status);

    if (socket.sData(buffer_o.getDataStart(), buffer_o.getData() - buffer_o.getDataStart()) < 1)
    {
        ERR("CHandler: Error, failed to send a login server rejected session response.");
        return 1;
    }

    return status;
}

int ConnectionHandler::processSuccess(Socket &socket, LoginRequest loginrequest)
{
    // Initialize a login response
    LoginResponse loginresponse(STATUS_OK, 2, 0);

    // Allocate the o buffer for us
    Buffer buffer_o(3);

    // Send a success response
    buffer_o.write_byte(loginresponse.status);
    buffer_o.write_byte(loginresponse.rights);
    buffer_o.write_byte(loginresponse.flagged);

    if (socket.sData(buffer_o.getDataStart(), buffer_o.getData() - buffer_o.getDataStart()) < 3)
    {
        ERR("CHandler: Error, failed to send a login server success session response.");
        return 1;
    }

    // Successful login, print info
    LOG("req: " << loginrequest.username << " | " << loginrequest.password << " | " << loginrequest.client_detail << " | " << loginrequest.client_version << " | " << loginrequest.client_release
        << " | " << loginrequest.client_reconnecting);
    LOG("res: " << loginresponse.status << " | " << loginresponse.rights << " | " << loginresponse.flagged);

    return loginresponse.status;
}

}
