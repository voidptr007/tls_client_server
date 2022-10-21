#ifndef _TLSSOCKET_HPP
#define _TLSSOCKET_HPP

#include "socket.hpp"
#include <openssl/ssl.h>

namespace Network
{
    class TlsSocket : public Socket
    {
    public:
        /// @brief get SSL session object
        /// @return pointer to SSL sesion object
        SSL *get_SSL() const;

        /// @brief set SSL session
        /// @param issl pointer to SSL session object
        void set_SSL(SSL *issl);

        /// @brief Closes SSL session and socket fd
        void close() override;

        TlsSocket() {}
        TlsSocket(const TlsSocket &other) = default;
        TlsSocket &operator=(const TlsSocket &other) = default;
        TlsSocket(const TlsSocket &&other);
        TlsSocket &operator=(const TlsSocket &&other);
        TlsSocket(int32_t &sktfd, uint16_t &localPort, std::string &localAddr, SSL *issl);
        virtual ~TlsSocket() = default;
        bool operator==(const TlsSocket &rhs);

    private:
        SSL *ssl;
    };
}
#endif