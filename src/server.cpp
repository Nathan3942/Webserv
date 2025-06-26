/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   server.cpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: njeanbou <njeanbou@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/06/20 11:18:38 by ichpakov          #+#    #+#             */
/*   Updated: 2025/06/26 18:45:48 by njeanbou         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../include/server.hpp"

Server::Server(std::vector<int>& ports_) : ports(ports_)
{
	for (size_t i = 0; i < ports.size(); ++i)
	{
		struct sockaddr_in serverAddr;
		server_fd = socket(AF_INET, SOCK_STREAM, 0);
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

		sockets.push_back(server_fd);
        std::cout << "Listening on port " << ports[i] << std::endl;
	}
}

Server::~Server()
{
	
}

void	Server::close_socket()
{
	for (size_t i = 0; i < sockets.size(); ++i)
		close(sockets[i]);
}

// std::string	Server::get_path(const std::string& request)
// {
// 	std::string path = "/";

// 	size_t pos1 = request.find("GET ");
// 	size_t pos2 = request.find(" HTTP/");
// 	if (pos1 != std::string::npos && pos2 != std::string::npos)
// 		path = request.substr(pos1 + 4, pos2 - 4);
// 	if (path == "/")
// 		path = "/index.html";
// 	return (path);
// }

// std::string Server::read_file_binary(const std::string& filepath)
// {
// 	std::ifstream file(filepath.c_str(), std::ios::in | std::ios::binary);
// 	if (!file)
// 		return ("404");
	
// 	std::string content;
// 	char	buffer[1024];
// 	while (file.read(buffer, sizeof(buffer)))
// 		content.append(buffer, file.gcount());
// 	if (file.gcount() > 0)
// 		content.append(buffer, file.gcount());
// 	return (content);
// }

// std::string Server::read_file(const std::string& filepath)
// {
//     std::ifstream file(filepath.c_str());
//     if (!file)
//         return ("404");
//     std::string content;
//     char c;
//     while (file.get(c))
//         content += c;
//     return (content);
// }

// std::string Server::handle_client(int clientSocket)
// {
//     const int bufferSize = 1024;
//     char buffer[bufferSize];
//     memset(buffer, 0, bufferSize);

//     int bytesRead = read(clientSocket, buffer, bufferSize - 1);
//     if (bytesRead < 0) {
//         perror("read");
//         return ("");
//     }

//     printf("Requête reçue :\n%s\n", buffer);

// 	std::string request(buffer);
// 	return (request);
// }

// std::string	Server::read_and_path(const std::string path)
// {
// 	std::string full_path = std::string(WEBROOT) + path;
		
// 		if (full_path.find("png") == std::string::npos &&
// 			full_path.find("jpeg") == std::string::npos &&
// 			full_path.find("jpg") == std::string::npos)
// 		   	return (read_file(full_path));
// 		else
// 			return (read_file_binary(full_path));
// }

// int	get_type(const std::string& path)
// {
// 	const char* type[] = {"html", "css", "xml", "png", "jpg", "jpeg"};
// 	size_t	pos = path.find(".");
// 	std::string afterDot = path.substr(pos + 1);

// 	int i = 0;
// 	while (type[i])
// 	{
// 		if (type[i] == afterDot)
// 			return (static_cast<int>(i));
// 		++i;
// 	}
// 	return (-1);
// }


// std::string Server::get_content_type(const std::string& path)
// {
// 	// std::cout << "PATH : " << path << "\nindex : " << get_type(path) << std::endl;
// 	switch (get_type(path))
// 	{
// 		case 0:
// 			return ("text/html");
// 		case 1:
// 			return ("text/css");
// 		case 2:
// 			return ("text/xml");
// 		case 3:
// 			return ("image/png");
// 		case 4:
// 			return ("image/jpeg");
// 		case 5:
// 			return ("image/jpeg");
// 		default:
// 			return ("text/html");
// 	}
// }

// std::string	Server::build_reponse(const std::string& body, const std::string& content_type)
// {
// 	std::string reponse;
// 	if (body.find("404") != std::string::npos)
// 		reponse += "HTTP/1.1 404 \r\n";
// 	else
// 		reponse += "HTTP/1.1 200 \r\n";
// 	reponse += "Content-Type: " + content_type + "\r\n";
	
// 	char lenght[32];
// 	sprintf(lenght, "%lu", (unsigned long)body.size());
// 	reponse += "Content-Length: " + std::string(lenght) + "\r\n";
// 	reponse += "Connection: close\r\n\r\n";
// 	reponse += body;
// 	return (reponse);
// }

void Server::start()
{
	fd_set	readfds;
	
    isRunning = true;
    while (isRunning)
	{
		FD_ZERO(&readfds);
		int max_fd = 0;

		for (size_t i = 0; i < sockets.size(); ++i)
		{
			FD_SET(sockets[i], &readfds);
			if (sockets[i] > max_fd)
				max_fd = sockets[i];
		}

		if (select(max_fd + 1, &readfds, NULL, NULL, NULL) < 0)
		{
			perror("Select");
			continue; 
		}

		for (size_t i = 0; i < sockets.size(); ++i)
		{
			if (FD_ISSET(sockets[i], &readfds))
			{
				int client_fd;
				struct sockaddr_in clientAddr;
				socklen_t clientLen = sizeof(clientAddr);

				client_fd = accept(sockets[i], (struct sockaddr*)&clientAddr, &clientLen);
				if (client_fd < 0) {
					perror("accept");
					continue;
				}

				Request req(client_fd);
				Responce res(req.get_path());

				send(client_fd, res.get_response().c_str(), res.get_response().size(), 0);
				close(client_fd);
			}
		}
    }
	Server::close_socket();
}
