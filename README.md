## Description
This project provides a set of APIs to create a TLS based secure client-server application
over TCP. The easy and ready to use APIs will bringup a client and server in matter of minutes.
The APIs provide extension points to implement business specific logic. Refer instructions to
create self-signed certificates in detailed description. Sample certificates are also made
available for a quick test of the APIs.

## Index
1. Code Structure & Build
2. Dependencies
3. Usage with example
4. Certificate creation

# Code structure
- certs - contains self-sigend root, server and client certificates for test
- include - contains all header files (hpp)
- src - contains all implementation code (cpp)
- tests - example code illustrating usage of tls client server library

## Dependencies
- Pthread
- Cmake 3.8
- Openssl 1.1.1(x) or 3.0.(x)
- Lib Cyrpto
- Lib SSL

# How to build & run
Create build folder in project root directory and run cmake and make commands from there. Both client and server executables will be generated in build directory. Server port is optional, default 9000 is used.
```console
tls_client_server$ mkdir build
tls_client_server$ cd build
tls_client_server/build$ cmake ..
tls_client_server/build$ make
tls_client_server/build$ ./tlsserver <port number>
tls_client_server/build$ ./tlsclient <server port>
```
### result
![image](/tlsserveroutput.png)

![image](/tlsclientoutput.png)

# Usage with examples
## TlsServer
Create an object of type TlsServer, providing port number and server address (optional).
Start tls server in a thread. Provide receive callback handler and buffer to store received data.
start tls server method keeps monitoring for new client connection requests and data from connected
clients. It will invoke registered callback with data when data is received from a client.
The callback will provide received data, size in bytes and client socket that sent the data.
OpenSSL crypto and SSL libraries are used for mutual TLS authentication and encrypted communication.
Server and client will negotiate and choose highest version of supported protocol.
```c++
    #include <thread>
    #include "tlsserver.hpp"

    Network::TlsServer tlsserver;
    void MsgRecvCallback(void *const data, ssize_t &dataSize, const Network::TlsSocket &client)
    {
        tlsserver.tls_send(data, dataSize, client);
        return;
    }

    int main(int argc, char *argv[])
    {
        const std::string cert = "../certs/server_cert.crt";
        const std::string key = "../certs/server_key.key";
        const std::string cacert = "../certs/rootCA.crt";
        char buffer[64];
        int serverport = 9000;
        const std::string serveraddr = "localhost";
        if (argc > 1 && atoi(argv[1]) > 0)
        {
            serverport = atoi(argv[1]);
        }

        fprintf(stderr, "starting tls server @%s:%d\n", serveraddr.c_str(), serverport);
        if (tlsserver.tls_create(serverport, serveraddr, cert, key, cacert))
        {
            std::thread tServer = std::thread(&Network::TlsServer::tls_start_server, &tlsserver, &MsgRecvCallback, buffer, sizeof(buffer));
            tServer.join();
        }
        return 0;
    }
```
## TlsClient
Create an object of type TlsClient with optional local port, address, certificate and private key.
Connect to server using server's port and address. Send and receive data.
```c++
    #include <string.h>
    #include "tlsclient.hpp"

    int main(int argc, char *argv[])
    {
        const std::string cert = "../certs/client_cert.crt";
        const std::string key = "../certs/client_key.key";
        const std::string cacert = "../certs/rootCA.crt";
        int serverport = 9000;
        const std::string serverip = "localhost";
        const std::string testdata = "Tls client-server loopback test message!!";
        if (argc > 1 && atoi(argv[1]) > 0)
        {
            serverport = atoi(argv[1]);
        }

        Network::TlsClient tlsclient;
        if (tlsclient.tls_create(0, "localhost", cert, key, cacert) && tlsclient.tls_connect(serverport, serverip))
        {
            if (tlsclient.tls_send(testdata.c_str(), testdata.length()) > 0)
            {
                char datarcvd[64] = {0};
                ssize_t rcvdsize = tlsclient.tls_receive(datarcvd, sizeof(datarcvd));
                if (rcvdsize == testdata.length() && strcmp(datarcvd, testdata.c_str()) == 0)
                {
                    fprintf(stdout, "tls loopback test passed. recevied msg: %s\n", datarcvd);
                }
                else
                {
                    fprintf(stdout, "tls loopback test failed. recevied msg: %s\n", datarcvd);
                }
            }
        }
        return 0;
    }
```
## Certificates
Test program uses self-signed certificates. Certificates can be created using openssl.
Following example illustrates certificate creation.
```console
//create root ca
openssl genrsa -out rootCA.key 4096
openssl req -x509 -new -nodes -key rootCA.key -sha256 -days 1024 -out rootCA.crt

//create client cert
openssl genrsa -out client_key.key 2048
openssl req -new -sha256 -key client_key.key -subj "/C=US/ST=CA/O=OrgName/CN=clientid" -out client_csr.csr
openssl req -in client_csr.csr -noout -text
openssl x509 -req -in client_csr.csr -CA rootCA.crt -CAkey rootCA.key -CAcreateserial -out client_cert.crt -days 500 -sha256

//create server cert
openssl genrsa -out server_key.key 2048
openssl req -new -sha256 -key server_key.key -subj "/C=US/ST=CA/O=Orgname/CN=serverid" -out server_csr.csr
openssl req -in server_csr.csr -noout -text
openssl x509 -req -in server_csr.csr -CA rootCA.crt -CAkey rootCA.key -CAcreateserial -out server_cert.crt -days 500 -sha256
```
