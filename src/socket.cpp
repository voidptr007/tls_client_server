#include <unistd.h>
#include <fcntl.h>
#include <netdb.h>
#include <memory.h>
#include "socket.hpp"

namespace Network
{
    Socket::Socket(const int32_t &sktfd, const uint16_t &localPort, const std::string &localAddr)
    {
        sockfd = sktfd;
        address = localAddr;
        port = localPort;
    }

    Socket::Socket(const Socket &&other)
    {
        sockfd = other.sockfd;
        address = other.address;
        port = other.port;
    }

    Socket &Socket::operator=(const Socket &&other)
    {
        sockfd = other.sockfd;
        address = other.address;
        port = other.port;
        return *this;
    }

    bool Socket::operator==(const Socket &rhs)
    {
        return sockfd == rhs.sockfd &&
               address == rhs.address &&
               port == rhs.port;
    }

    bool Socket::create(const int &socketType, const uint16_t &localPort, const std::string &localAddr)
    {
        address = localAddr;
        port = localPort;
        if ((sockfd = ::socket(AF_INET, socketType, 0)) < 0)
        {
            fprintf(stderr, "error: socket creation failed for port(%d)\n", port);
            return false;
        }
        return (Socket::bind() > 0);
    }

    bool Socket::get_hostbyname(std::string address, sockaddr_in &sktaddr)
    {
        struct addrinfo hints;
        struct addrinfo *result;
        struct addrinfo *rp;

        memset(&hints, 0, sizeof(hints));
        hints.ai_family = AF_INET;
        hints.ai_socktype = SOCK_STREAM;
        hints.ai_flags = 0;
        hints.ai_protocol = 0;
        hints.ai_canonname = NULL;
        hints.ai_addr = NULL;
        hints.ai_next = NULL;

        int32_t s = getaddrinfo(address.c_str(), NULL, &hints, &result);
        if (s != 0)
        {
            fprintf(stderr, "getaddrinfo: %s\n", gai_strerror(s));
            return false;
        }
        memcpy(&sktaddr, (sockaddr_in *)result->ai_addr, result->ai_addrlen);
        freeaddrinfo(result);
        return true;
    }

    int Socket::bind()
    {
        if (sockfd < 0)
        {
            fprintf(stderr, "error: socket bind failed for %s:%d sockfd(%d)\n", address.c_str(), port, sockfd);
            return sockfd;
        }

        int enable = 1;
        if (::setsockopt(sockfd, SOL_SOCKET, SO_REUSEPORT, &enable, sizeof(int)) < 0)
        {
            std::string err = "error: setsockopt(SO_REUSEPORT) failed for " + address + ":" + std::to_string(port);
            perror(err.c_str());
        }
        if (::setsockopt(sockfd, SOL_SOCKET, SO_REUSEADDR, &enable, sizeof(int)) < 0)
        {
            std::string err = "error: setsockopt(SO_REUSEADDR) failed for " + address + ":" + std::to_string(port);
            perror(err.c_str());
        }

        sockaddr_in sktaddr{0};
        if (!get_hostbyname(address, sktaddr))
        {
            return -1;
        }
        sktaddr.sin_port = htons(port);
        if (::bind(sockfd, (sockaddr *)&sktaddr, sizeof(sktaddr)) < 0)
        {
            std::string err = "error: socket bind failed for " + address + ":" + std::to_string(port);
            perror(err.c_str());
            ::close(sockfd);
            sockfd = -1;
        }
        return sockfd;
    }

    int Socket::set_blocking(int32_t &sktfd)
    {
        if (fcntl(sktfd, F_GETFL, 0) < 0)
            return -1;
        return fcntl(sktfd, F_SETFL, ~O_NONBLOCK);
    }

    int Socket::set_nonblocking(int32_t &sktfd)
    {
        if (fcntl(sktfd, F_GETFL, 0) < 0)
            return -1;
        return fcntl(sktfd, F_SETFL, O_NONBLOCK);
    }

    int Socket::set_recvtimeout(const uint16_t &timeOutuSec)
    {
        struct timeval tv;
        tv.tv_sec = 0;
        tv.tv_usec = timeOutuSec;
        return setsockopt(sockfd, SOL_SOCKET, SO_RCVTIMEO, (const char *)&tv, sizeof tv);
    }

    const std::string &Socket::get_addr() const
    {
        return address;
    }

    const uint16_t &Socket::get_port() const
    {
        return port;
    }

    const int32_t &Socket::get_sockfd() const
    {
        return sockfd;
    }

    void Socket::close()
    {
        if (sockfd != -1)
        {
            ::close(sockfd);
            sockfd = -1;
        }
    }
}
