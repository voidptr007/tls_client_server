#include <arpa/inet.h>
#include "tlsclient.hpp"

namespace Network
{
    bool TlsClient::tls_create(const uint16_t &localPort, const std::string &localAddr,
                               const string &pathCert, const string &pathKey, const string &pathCacert)
    {
        bool created = false;
        if (!tls.is_tlssetup() && cliskt.create(SOCK_STREAM, localPort, localAddr))
        {
            created = tls.setup_tlscontext(Tls::Role::client, pathCert, pathKey, pathCacert);
        }
        return created;
    }

    bool TlsClient::tls_connect(const uint16_t &serverport, const std::string &serveraddr)
    {
        if (!tls.is_tlssetup())
        {
            return false;
        }
        sockaddr_in sktaddr{0};
        cliskt.get_hostbyname(serveraddr, sktaddr);
        sktaddr.sin_port = htons(serverport);
        printf("connecting to server @%s(%s):%d\n", serveraddr.c_str(), inet_ntoa(sktaddr.sin_addr), ntohs(sktaddr.sin_port));
        if (::connect(cliskt.get_sockfd(), (sockaddr *)&sktaddr, sizeof(sktaddr)) != 0)
        {
            perror("client connect faield");
            return false;
        }
        cliskt.set_SSL(tls.get_new_ssl(cliskt.get_sockfd()));
        if (SSL_connect(cliskt.get_SSL()) < 1)
        {
            fprintf(stderr, "SSL_connect failed\n");
            ERR_print_errors_fp(stderr);
            return false;
        }
        fprintf(stdout, "SSL_connect successful\n");
        tls.show_X509_cert(cliskt.get_SSL());
        return true;
    }

    size_t TlsClient::tls_send(const void *const buffer, const uint32_t &buflen)
    {
        return tls.write(cliskt.get_SSL(), buffer, buflen);
    }

    size_t TlsClient::tls_receive(void *const buffer, const uint32_t &buflen)
    {
        return tls.read(cliskt.get_SSL(), buffer, buflen);
    }

    void TlsClient::tls_close()
    {
        cliskt.close();
        tls.tls_close();
    }

    TlsClient::~TlsClient()
    {
        this->tls_close();
    }

}