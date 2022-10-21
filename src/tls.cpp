#include <string.h>
#include "tls.hpp"

namespace Network
{
    bool Tls::setup_tlscontext(const Role &role, const string &pathCert, const string &pathKey, const string &pathCacert)
    {
        if (!filesystem::exists(path(pathCert)) ||
            !filesystem::exists(path(pathKey)))
        {
            fprintf(stderr, "invalid certificate input\n");
        }
        key = pathKey;
        cert = pathCert;
        cacert = pathCacert.empty() ? defaultCacert : pathCacert;

        SSL_library_init();
        OpenSSL_add_all_algorithms();
        SSL_load_error_strings();

        // TLS_x_method() negotiates highest supproted TLS version between server & client
        if (role == Role::server)
        {
            method = TLS_server_method();
        }
        else
        {
            method = TLS_client_method();
        }

        ctx = SSL_CTX_new(method);
        if (!ctx)
        {
            ERR_print_errors_fp(stderr);
            return false;
        }

        // check whether public and private keys match
        if (SSL_CTX_use_certificate_file(ctx, cert.c_str(), SSL_FILETYPE_PEM) <= 0)
        {
            ERR_print_errors_fp(stderr);
            return false;
        }
        if (SSL_CTX_use_PrivateKey_file(ctx, key.c_str(), SSL_FILETYPE_PEM) <= 0)
        {
            ERR_print_errors_fp(stderr);
            return false;
        }
        if (!SSL_CTX_check_private_key(ctx))
        {
            fprintf(stderr, "Private key (%s) does not match the certificate public key (%s)\n", path(key).filename().c_str(), path(cert).filename().c_str());
            return false;
        }

        // Load CA cert for peer verification
        if (!SSL_CTX_load_verify_locations(ctx, cacert.c_str(), capath.c_str()))
        {
            ERR_print_errors_fp(stderr);
            return false;
        }

        // Set flag in context to require peer certificate verification
        SSL_CTX_set_verify(ctx, SSL_VERIFY_PEER | SSL_VERIFY_FAIL_IF_NO_PEER_CERT, NULL);
        SSL_CTX_set_verify_depth(ctx, 4);
        tlsSetup = true;

        return tlsSetup;
    }

    void Tls::tls_close()
    {
        SSL_CTX_free(ctx);
    }

    size_t Tls::write(SSL *ssl, const void *const buffer, const uint32_t &buflen)
    {
        if (ssl == NULL)
            return -1;
        return SSL_write(ssl, buffer, buflen);
    }

    size_t Tls::read(SSL *ssl, void *const buffer, const uint32_t &buflen)
    {
        if (ssl == NULL)
            return -1;
        return SSL_read(ssl, buffer, buflen);
    }

    SSL *Tls::get_new_ssl(const int32_t &fd)
    {
        SSL *ssl_sesson = SSL_new(ctx);
        SSL_set_fd(ssl_sesson, fd);
        return ssl_sesson;
    }

    SSL_CTX *Tls::get_tlscontext()
    {
        return ctx;
    }

    bool Tls::is_tlssetup()
    {
        return tlsSetup;
    }

    void Tls::show_X509_cert(SSL *ssl)
    {
        char *line;
        X509 *cert = SSL_get_peer_certificate(ssl);
        if (cert != NULL)
        {
            fprintf(stdout, "Certificate:\n");
            printf("Subject: %s\n", (line = X509_NAME_oneline(X509_get_subject_name(cert), 0, 0)));
            free(line);
            printf("Issuer: %s\n", (line = X509_NAME_oneline(X509_get_issuer_name(cert), 0, 0)));
            free(line);
            X509_free(cert);
        }
        else
        {
            printf("show_X509cert: certificate is empty\n");
        }
    }
}