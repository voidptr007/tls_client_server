#ifndef _SOCKET_HPP_
#define _SOCKET_HPP_

#include <string>
#include <arpa/inet.h>

namespace Network
{
    class Socket
    {
    public:
        /// @brief get port being used for this socket
        /// @return port number
        const uint16_t &get_port() const;

        /// @brief get socket file descriptor of this socket
        /// @return socket file descriptor
        const int32_t &get_sockfd() const;

        /// @brief get address of the socket
        /// @return address
        const std::string &get_addr() const;

        /// @brief sets socket in blocking mode
        /// @param sockfd socket fd
        /// @return -1 on error
        static int set_blocking(int32_t &sockfd);

        /// @brief set socket in non-blocking mode
        /// @param sockfd socket fd
        /// @return -1 on error
        static int set_nonblocking(int32_t &sockfd);

        /// @brief sets receive timeout
        /// @param timeOutuSec timeout in micro-seconds
        /// @return 0 on success, -1 for errors
        int set_recvtimeout(const uint16_t &timeOutuSec);

        /// @brief creates a socket and binds it to address:port
        /// @param socketType SOCK_STREAM or SOCK_DGRAM
        /// @param localPort local port
        /// @param localAddr local address (optional)
        /// @return true on success
        bool create(const int &socketType, const uint16_t &localPort = 0, const std::string &localAddr = "");

        /// @brief closes a socket fd
        virtual void close();

        /// @brief gets sockaddr_in for a given url/domain address
        /// @param address domain address/url
        /// @param sktaddr sockaddr_in filled with ip address
        /// @return true, if successful
        bool get_hostbyname(std::string address, sockaddr_in &sktaddr);

        Socket() {}
        Socket(const Socket &other) = default;
        Socket &operator=(const Socket &other) = default;
        Socket(const Socket &&other);
        Socket &operator=(const Socket &&other);
        virtual ~Socket() = default;
        bool operator==(const Socket &rhs);
        Socket(const int32_t &sockfd, const uint16_t &localPort = 0, const std::string &localAddr = "");

    protected:
        int32_t sockfd{-1};
        std::string address;
        uint16_t port{0};

    private:
        int bind();
    };
}
#endif
