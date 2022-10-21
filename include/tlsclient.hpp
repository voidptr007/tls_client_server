#ifndef _TLS_CLIENT_
#define _TLS_CLIENT_

#include <openssl/crypto.h>
#include <openssl/ssl.h>
#include <openssl/err.h>
#include "tls.hpp"
#include "tlssocket.hpp"

namespace Network
{
    class TlsClient
    {
    public:
        /// @brief creates a socket and sets up TLS session with certificates
        /// @param localPort local port to use for client
        /// @param localAddr local address to use for client
        /// @param pathCert path to certificate
        /// @param pathKey path to private key
        /// @param pathCacert path to CA cert (optional)
        /// @return true, if successful
        bool tls_create(const uint16_t &localPort, const std::string &localAddr,
                        const string &pathCert, const string &pathKey, const string &pathCacert = "");

        /// @brief connects to server securely over TLS
        /// @param serverport server port to connect to
        /// @param serveraddr server address to connect to
        /// @return
        bool tls_connect(const uint16_t &serverport, const std::string &serveraddr);

        /// @brief sends data securely over TLS
        /// @param buffer data to send
        /// @param buflen size of data to send
        /// @return true, if successful
        size_t tls_send(const void *const buffer, const uint32_t &buflen);

        /// @brief receives data securely over TLS
        /// @param buffer buffer to hold received data
        /// @param buflen size of the buffer that holds received data
        /// @return true, if successful
        size_t tls_receive(void *const buffer, const uint32_t &buflen);

        /// @brief cloes socket, SSL session and SSL context
        void tls_close();

        /// DTOR
        virtual ~TlsClient();

    private:
        Tls tls;
        TlsSocket cliskt;
    };
}
#endif