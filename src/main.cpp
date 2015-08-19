// std includes
#include <iostream>
#include <thread>

// kronos includes
#include "config.h"
#include "macros.h"
#include "core/server.h"


int main(int argc, char **argv)
{
    LOG("Main: Kronos MMO Server, version " << VERSION_R << "." << VERSION_B << "." << VERSION_A);

    // Create the main server object
    kronos::Server server;

    // Create a variable to hold error messages
    int err = 0;

    // Bind the main server object to ip:port
    err = server.bind("0.0.0.0", 43594);
    if (err != 0)
    {
        ERR("Main: Failed to bind the server object to chosen ip:port.");
        return err;
    }

    // Start listening on ip:port
    err = server.listen();
    if (err != 0)
    {
        ERR("Main: Failed to start the server listener on ip:port.");
        return err;
    }

    // Start the server
    server.start();

    // Run the server
    server.run();

    // Exit the program
    LOG("Main: Kronos MMO Server, shutting down...");

    return 0;
}
