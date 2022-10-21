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