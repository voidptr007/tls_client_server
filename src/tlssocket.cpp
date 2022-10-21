#include "tlssocket.hpp"

namespace Network
{
    TlsSocket::TlsSocket(int32_t &sktfd, uint16_t &localPort, std::string &localAddr, SSL *issl)
    {
        sockfd = sktfd;
        address = localAddr;
        port = localPort;
        ssl = issl;
    }

    TlsSocket::TlsSocket(const TlsSocket &&other)
    {
        sockfd = other.sockfd;
        address = other.address;
        port = other.port;
        ssl = other.ssl;
    }

    TlsSocket &TlsSocket::operator=(const TlsSocket &&other)
    {
        sockfd = other.sockfd;
        address = other.address;
        port = other.port;
        ssl = other.ssl;
        return *this;
    }

    bool TlsSocket::operator==(const TlsSocket &rhs)
    {
        return sockfd == rhs.sockfd &&
               address == rhs.address &&
               port == rhs.port;
    }

    SSL *TlsSocket::get_SSL() const
    {
        return ssl;
    }

    void TlsSocket::set_SSL(SSL *issl)
    {
        ssl = issl;
    }

    void TlsSocket::close()
    {
        if (sockfd != -1 && ssl != NULL)
        {
            SSL_shutdown(ssl);
            SSL_free(ssl);
            ssl = NULL;
            Socket::close();
        }
    }
}
