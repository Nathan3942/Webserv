/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   server.cpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: njeanbou <njeanbou@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/06/20 11:18:38 by ichpakov          #+#    #+#             */
/*   Updated: 2025/06/30 13:51:23 by njeanbou         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../include/server.hpp"

Server::Server(std::vector<int>& ports_) : ports(ports_), isRunning(false)
{
	epoll_fd = epoll_create1(0);
	if (epoll_fd == -1)
	{
		perror("epoll_creat1");
		exit(1);
	}

	for (size_t i = 0; i < ports.size(); ++i)
	{
		int server_fd = socket(AF_INET, SOCK_STREAM, 0);
		if (server_fd < 0) {
			perror("socket");
			exit(1);
		}

		int opt = 1;
		if (setsockopt(server_fd, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt)) < 0)
		{
			perror("Error set socket");
			return ;
		}

		set_nonblocking(server_fd);

		struct sockaddr_in serverAddr;
		memset(&serverAddr, 0, sizeof(serverAddr));
		serverAddr.sin_family = AF_INET;
		serverAddr.sin_addr.s_addr = INADDR_ANY;
		serverAddr.sin_port = htons(ports[i]);

		if (bind(server_fd, (struct sockaddr*)&serverAddr, sizeof(serverAddr)) < 0) {
			perror("bind");
			exit(1);
		}

		if (listen(server_fd, 5) < 0) {
			perror("listen");
			exit(1);
		}

		struct epoll_event ev;
		ev.events = EPOLLIN;
		ev.data.fd = server_fd;

		if (epoll_ctl(epoll_fd, EPOLL_CTL_ADD, server_fd, &ev) == -1)
		{
			perror("epoll_ctl: listen socket");
			exit(1);
		}

		sockets.push_back(server_fd);
        std::cout << "Listening on port " << ports[i] << std::endl;
	}
}

Server::~Server()
{
	close_socket();
	if (epoll_fd > 0)
		close(epoll_fd);
}

int	Server::set_nonblocking(int fd)
{
	int flags = fcntl(fd, F_GETFL, 0);
	if (flags == -1)
		return (-1);
	return (fcntl(fd, F_SETFL, flags | O_NONBLOCK));
}

bool	Server::is_listen_socket(int fd) const
{
	for (size_t i = 0; i < sockets.size(); ++i)
	{
		if (sockets[i] == fd)
			return (true);
	}
	return (false);
}


void	Server::accept_connection(int listen_fd)
{
	struct sockaddr_in client_addr;
	socklen_t	client_len = sizeof(client_addr);
	int client_fd = accept(listen_fd, (struct sockaddr*)&client_addr, &client_len);
	if (client_fd < 0)
	{
		perror("accept");
		return ;
	}

	set_nonblocking(client_fd);
	
	struct epoll_event	ev;
	ev.events = EPOLLIN | EPOLLET;
	ev.data.fd = client_fd;
	if (epoll_ctl(epoll_fd, EPOLL_CTL_ADD, client_fd, &ev) == -1)
	{
		perror("epoll_ctl: client");
		close(client_fd);
	}
}


ssize_t Server::send_all(int sockfd, const char* buf, size_t len) {
    size_t total_sent = 0;
    while (total_sent < len) {
        ssize_t sent = send(sockfd, buf + total_sent, len - total_sent, 0);
        if (sent <= 0) {
            if (sent == -1 && (errno == EAGAIN || errno == EWOULDBLOCK)) {
                // Socket en mode non bloquant, on peut attendre un peu ou continuer la boucle
                continue;
            }
            return -1; // Erreur fatale
        }
        total_sent += sent;
    }
    return total_sent;
}


void	Server::close_socket()
{
	for (size_t i = 0; i < sockets.size(); ++i)
		close(sockets[i]);
	sockets.clear();
}


void Server::start()
{
	const int	max_events = 100;
	struct epoll_event events[max_events];
	
    isRunning = true;
    while (isRunning)
	{
		int	nfds = epoll_wait(epoll_fd, events, max_events, -1);
		if (nfds == -1)
		{
			perror("epoll_wait");
			continue;
		}

		for (int i = 0; i < nfds; ++i)
		{
			int fd = events[i].data.fd;

			if (is_listen_socket(fd))
				accept_connection(fd);
			else
			{
				Request req(fd);
				Responce res(req.get_path());
				
				const std::vector<char>& buffer = res.get_response();
				if (send_all(fd, buffer.data(), buffer.size()) == -1)
					perror("send_all");
				// send(fd, buffer.data(), buffer.size(), 0);
				
				close(fd);
				epoll_ctl(epoll_fd, EPOLL_CTL_DEL, fd, NULL);
			}
		}	
	}
}

