#include <unistd.h>
#include <memory.h>
#include <arpa/inet.h>
#include "tlsserver.hpp"
#include "tlssocket.hpp"

namespace Network
{
    bool TlsServer::tls_create(const uint16_t &localPort, const std::string &localAddr,
                               const string &pathCert, const string &pathKey, const string &pathCacert)
    {
        bool created = false;
        if (!tls.is_tlssetup() && srvskt.create(SOCK_STREAM, localPort, localAddr))
        {
            created = tls.setup_tlscontext(Tls::Role::server, pathCert, pathKey, pathCacert);
        }
        return created;
    }

    size_t TlsServer::tls_send(const void *const buffer, const uint32_t &buflen, const TlsSocket &tlsclient)
    {
        return tls.write(tlsclient.get_SSL(), buffer, buflen);
    }

    bool TlsServer::tls_start_server(funcTlsRxCallback callback, void *data, const uint32_t &dataLen)
    {
        int32_t sfd = srvskt.get_sockfd();
        if (sfd <= 0 || dataLen == 0 || data == nullptr || callback == nullptr)
        {
            fprintf(stderr, "invalid arguments, tcp server could not be started\n");
            return false;
        }

        ::listen(sfd, MAX_CLIENTS);

        epollfd = epoll_create1(0);
        if (epollfd == -1)
        {
            perror("epoll_create1");
            return false;
        }
        // epoll needs non-blocking socket
        Socket::set_nonblocking(sfd);
        evtInterest.data.fd = sfd;
        evtInterest.events = EPOLLIN;
        if (epoll_ctl(epollfd, EPOLL_CTL_ADD, sfd, &evtInterest) == -1)
        {
            perror("epoll_ctl: server socket could not be added to epoll");
            return false;
        }

        while (true)
        {
            int32_t nfds = epoll_wait(epollfd, evtResponse.data(), MAX_CLIENTS, -1);
            for (int i = 0; i < nfds; ++i)
            {
                int32_t fd = evtResponse[i].data.fd;
                if (!(evtResponse[i].events & EPOLLIN))
                {
                    fprintf(stderr, "epoll event error\n");
                    ::close(fd);
                    continue;
                }
                else if (fd == sfd)
                {
                    sockaddr_in client{0};
                    socklen_t len = sizeof(client);
                    memset(&client, 0, len);
                    int32_t cfd = ::accept(sfd, (sockaddr *)&client, &len);
                    if (cfd <= 0)
                    {
                        perror("server:accept failed");
                        continue;
                    }
                    evtInterest.data.fd = cfd;
                    evtInterest.events = EPOLLIN | EPOLLET;
                    if (epoll_ctl(epollfd, EPOLL_CTL_ADD, cfd, &evtInterest) == -1)
                    {
                        perror("epoll_ctl: client socket could not be added to epoll");
                        continue;
                    }
                    // ssl accept
                    std::string cIp = std::string(inet_ntoa(client.sin_addr));
                    uint16_t cPort = ntohs(client.sin_port);
                    SSL *cSslSession = tls.get_new_ssl(cfd);
                    cliMgr.add(TlsSocket(cfd, cPort, cIp, cSslSession));
                    int ret = SSL_accept(cSslSession);
                    if (ret < 1)
                    {
                        fprintf(stderr, "SSL accept failed with ret:%d SSL_get_error:%d\n", ret, SSL_get_error(cSslSession, ret));
                        ERR_print_errors_fp(stderr);
                        cliMgr.remove(cfd);
                        continue;
                    }
                    tls.show_X509_cert(cSslSession);
                    Socket::set_nonblocking(cfd);
                    fprintf(stderr, "(+) client (%d). active clients# %lu\n", cfd, cliMgr.get_clientsockets().size());
                }
                else
                {
                    TlsSocket tlscliskt = cliMgr.get_client(fd);
                    memset(data, 0, sizeof(dataLen));
                    ssize_t recvsize = tls.read(tlscliskt.get_SSL(), data, dataLen);
                    if (recvsize > 0)
                    {
                        callback(data, recvsize, tlscliskt);
                    }
                    else
                    {
                        ERR_print_errors_fp(stderr);
                        cliMgr.remove(tlscliskt);
                        fprintf(stderr, "(-) client (%d). active clients# %ld\n", fd, cliMgr.get_clientsockets().size());
                    }
                }
            }
        }
        return true;
    }

    void TlsServer::tls_close()
    {
        srvskt.close();
        cliMgr.clear();
        tls.tls_close();
        epollfd = -1;
        evtInterest.data.fd = 0;
        evtInterest.events = 0;
    }

    TlsServer::~TlsServer()
    {
        this->tls_close();
    }
}