#ifndef _CLIENTMANAGER_HPP_
#define _CLIENTMANAGER_HPP_

#include <mutex>
#include <vector>
#include "socket.hpp"

namespace Network
{
    template <class SOCKTYPE>
    class ClientManager final
    {
    private:
        std::mutex m;
        std::vector<SOCKTYPE> clientSockets;
        const SOCKTYPE nullsocket;

    public:
        ClientManager() {}
        int32_t get_sockfd(const std::string &caddr, const uint16_t &cport)
        {
            typename std::vector<SOCKTYPE>::iterator it = std::find_if(clientSockets.begin(), clientSockets.end(),
                                                                       [caddr, cport](const SOCKTYPE &client)
                                                                       {
                                                                           return (client.get_addr() == caddr && client.get_port() == cport);
                                                                       });
            if (it != clientSockets.end())
            {
                return it->get_sockfd();
            }
            return -1;
        }

        const SOCKTYPE &get_client(const int32_t &sockfd)
        {
            typename std::vector<SOCKTYPE>::iterator it = std::find_if(clientSockets.begin(), clientSockets.end(),
                                                                       [sockfd](const SOCKTYPE &cli)
                                                                       {
                                                                           return (cli.get_sockfd() == sockfd);
                                                                       });
            if (it != clientSockets.end())
            {
                return *it;
            }
            return nullsocket;
        }

        void add(const SOCKTYPE &client)
        {
            std::scoped_lock<std::mutex> lock(m);
            clientSockets.push_back(client);
        }

        void remove(SOCKTYPE &client)
        {
            std::scoped_lock<std::mutex> lock(m);
            clientSockets.erase(std::remove(clientSockets.begin(), clientSockets.end(), client), clientSockets.end());
            client.close();
        }

        void remove(const int32_t &sockfd)
        {
            SOCKTYPE client = get_client(sockfd);
            if (client.get_sockfd() != -1)
            {
                remove(client);
            }
        }

        const std::vector<SOCKTYPE> &get_clientsockets()
        {
            return clientSockets;
        }

        void clear()
        {
            for (int idx = 0; idx < clientSockets.size(); ++idx)
            {
                clientSockets[idx].close();
            }
            clientSockets.clear();
        }

        ~ClientManager()
        {
            this->clear();
        }
    };
}
#endif