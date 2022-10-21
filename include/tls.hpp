#ifndef _TLS_HPP_
#define _TLS_HPP_

#include <openssl/crypto.h>
#include <openssl/ssl.h>
#include <openssl/err.h>
#include <string>
#include <filesystem>

using namespace std;
using namespace filesystem;

namespace Network
{
    class Tls
    {
    public:
        /// @brief TLS role (client or server)
        enum class Role
        {
            server,
            client
        };

        /// @brief setsup TLS context and verification flags
        /// @param role TLS role (client or server)
        /// @param pathCert path to certificate
        /// @param pathKey path to private key
        /// @param pathCacert path to CA certificate (optional)
        /// @return true, if successful
        bool setup_tlscontext(const Role &role, const string &pathCert, const string &pathKey, const string &pathCacert = "");

        /// @brief creates a SSL session for given socket fd and binds them
        /// @param fd socket fd
        /// @return pointer to new SSL session
        SSL *get_new_ssl(const int32_t &fd);

        /// @brief secure write over SSL
        /// @param ssl ssl session to use for secure writing
        /// @param buffer data to be written
        /// @param buflen size of data to be written
        /// @return number of bytes written
        size_t write(SSL *ssl, const void *const buffer, const uint32_t &buflen);

        /// @brief secure read over SSL
        /// @param ssl ssl session to use for secure reading
        /// @param buffer data to be read
        /// @param buflen size of data to be read
        /// @return number of bytes read
        size_t read(SSL *ssl, void *const buffer, const uint32_t &buflen);

        /// @brief shows X509 certificate
        /// @param ssl SSL session to read certificate from
        void show_X509_cert(SSL *ssl);

        /// @brief returns active SSL context
        /// @return pointer to SSL contex
        SSL_CTX *get_tlscontext();

        /// @brief indicates if TLS is setup
        /// @return true, if setup
        bool is_tlssetup();

        /// @brief closes SSL session and context
        void tls_close();

    private:
        string cacert;
        string cert;
        string key;
        SSL_CTX *ctx = NULL;
        bool tlsSetup = false;
        const SSL_METHOD *method;
        const string capath = "/etc/ssl/certs/";
        const string defaultCacert = capath + "ca-certificates.crt";
    };
}
#endif