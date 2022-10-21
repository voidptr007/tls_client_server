#ifndef _TLS_SERVER_
#define _TLS_SERVER_

#include <openssl/crypto.h>
#include <openssl/ssl.h>
#include <openssl/err.h>
#include <functional>
#include <sys/epoll.h>
#include "clientmanager.hpp"
#include "tls.hpp"
#include "tlssocket.hpp"

namespace Network
{
    static const int MAX_CLIENTS = 200;
    typedef std::function<void(void *const, ssize_t &, const Network::TlsSocket &)> funcTlsRxCallback;
    class TlsServer
    {
    public:
        TlsServer() {}

        /// @brief creates a socket and sets up TLS session with certificates
        /// @param localPort local port to use for server
        /// @param localAddr local address to use for server
        /// @param pathCert path to certificate
        /// @param pathKey path to private key
        /// @param pathCacert path to CA cert (optional)
        /// @return true, if successful
        bool tls_create(const uint16_t &localPort, const std::string &localAddr,
                        const string &pathCert, const string &pathKey, const string &pathCacert = "");
        
        /// @brief starts starts server securely over TLS 
        /// @param callback callback registered by caller, to be invoked upon receiving data froma client
        /// @param data buffer to hold data received from client
        /// @param dataLen size of the buffer to hold data received from client
        /// @return true, if successful
        bool tls_start_server(funcTlsRxCallback callback, void *data, const uint32_t &dataLen);

        /// @brief securely send data over TLS
        /// @param buffer data to send
        /// @param buflen size of the data to send
        /// @param tlsclient tls client to send data to
        /// @return number of bytes received
        size_t tls_send(const void *const buffer, const uint32_t &buflen, const TlsSocket &tlsclient);

        /// @brief closes server socket, SSL session and context
        void tls_close();

        ///DTOR
        virtual ~TlsServer();

    private:
        Tls tls;
        Socket srvskt;
        ClientManager<TlsSocket> cliMgr;
        int32_t epollfd = -1;
        struct epoll_event evtInterest;
        std::array<struct epoll_event, MAX_CLIENTS> evtResponse;
    };
}
#endif