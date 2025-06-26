/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   server.hpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: njeanbou <njeanbou@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/06/20 11:17:59 by ichpakov          #+#    #+#             */
/*   Updated: 2025/06/26 18:38:42 by njeanbou         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

//CLASSE DE BASE DU SERVEUR

#pragma once

#include "request.hpp"
#include "responce.hpp"

#include <iostream>
#include <string>
#include <unistd.h>
#include <stdlib.h>
#include <cstring>
#include <cstdio>
#include <sys/socket.h>
#include <netinet/in.h>
#include <fstream>
#include <vector>



class Server {
    private:
        std::vector<int>    ports;
        std::vector<int>    sockets;
        int port;
        int server_fd;
        bool isRunning; 

    public:
        Server(std::vector<int>& ports_);
        ~Server();
        void    start();
        void    close_socket();
};
