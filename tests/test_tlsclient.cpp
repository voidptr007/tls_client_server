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
