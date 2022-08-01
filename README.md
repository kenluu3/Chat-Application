# Server-Client Chat Application in Linux/Unix
A simple implementation of a server-client chat application using blocking TCP sockets.

## Build
Make a build directory for the client and server applications respectively: 
> `mkdir -p build/client-app && mkdir -p build/server-app`

To build the client, change directory into the client build application folder:
> `cd build/client-app`

Run the CMake file from the client application source directory:
> `cmake ../../src/client-app`

Run the make file:
> `make`

Repeat the steps above for the server application and update the paths to point to the server application directory: `server-app`
> `cd build/server-app`
> `cmake ../../src/server-app`
> `make`

## Running the Application
To run the application, open two seperate terminal sessions and from the build directory, run the following commands on each terminal respectively (Note: Server must be started first):

For server:
> `./chat-server <port>`

For client:
> `./chat-client <ip-address> <port>`

For connecting over localhost address, pass `0.0.0.0` or `127.0.0.1` to the `<ip-address>` argument & ensure that the port numbers match for both client and server. Since these are blocking sockets, the client must send the first message before the server can respond.