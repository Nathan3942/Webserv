/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   testWebPath.cpp                                    :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: njeanbou <njeanbou@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/06/12 12:41:55 by njeanbou          #+#    #+#             */
/*   Updated: 2025/06/17 18:20:30 by njeanbou         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <iostream>
#include <string>
#include <fstream>
#include <sstream>
#include <unistd.h>
#include <netinet/in.h>
#include <cstring>
#include <cstdio>
#include <vector>

#define PORT 8080
#define WEBROOT "./www"

enum Status 
{
    OK = 200,
    NOT_FOUND = 404,
    SERVER_ERROR = 500
};

int	get_type(const std::string& path)
{
	const char* type[] = {"html", "css", "xml", "png", "jpg", "jpeg"};
	size_t	pos = path.find(".");
	std::string afterDot = path.substr(pos + 1);

	int i = 0;
	while (type[i])
	{
		if (type[i] == afterDot)
			return (static_cast<int>(i));
		++i;
	}
	return (-1);
}


std::string get_content_type(const std::string& path)
{
	// std::cout << "PATH : " << path << "\nindex : " << get_type(path) << std::endl;
	switch (get_type(path))
	{
		case 0:
			return ("text/html");
		case 1:
			return ("text/css");
		case 2:
			return ("text/xml");
		case 3:
			return ("image/png");
		case 4:
			return ("image/jpeg");
		case 5:
			return ("image/jpeg");
		default:
			return ("text/html");
	}
}

std::string read_file_binary(const std::string& filepath)
{
	std::ifstream file(filepath.c_str(), std::ios::in | std::ios::binary);
	if (!file)
		return ("404");
	
	std::string content;
	char	buffer[1024];
	while (file.read(buffer, sizeof(buffer)))
		content.append(buffer, file.gcount());
	if (file.gcount() > 0)
		content.append(buffer, file.gcount());
	return (content);
}

std::string read_file(const std::string& filepath)
{
    std::ifstream file(filepath.c_str());
    if (!file)
        return ("404");
    std::string content;
    char c;
    while (file.get(c))
        content += c;
    return (content);
}

std::string	get_path(const std::string& request)
{
	std::string path = "/";

	size_t pos1 = request.find("GET ");
	size_t pos2 = request.find(" HTTP/");
	if (pos1 != std::string::npos && pos2 != std::string::npos)
		path = request.substr(pos1 + 4, pos2 - 4);
	// std::cout << "Path: " << path << std::endl;
	if (path == "/")
		path = "/index.html";
	return (path);
}

//reponse http
std::string	build_reponse(const std::string& body, const std::string& content_type)
{
	std::string reponse;
	if (body.find("404") != std::string::npos)
		reponse += "HTTP/1.1 404 \r\n";
	else
		reponse += "HTTP/1.1 200 \r\n";
	reponse += "Content-Type: " + content_type + "\r\n";
	
	char lenght[32];
	sprintf(lenght, "%lu", (unsigned long)body.size());
	reponse += "Content-Length: " + std::string(lenght) + "\r\n";
	reponse += "Connection: close\r\n\r\n";
	reponse += body;
	return (reponse);
}

int main ()
{

    int	server_fd, client_fd;
	struct sockaddr_in	address;	
	int addrlen = sizeof(address);


	server_fd = socket(AF_INET, SOCK_STREAM, 0);
	if (server_fd < 0)
	{
		std::cerr << "Error socket server" << std::endl;
		return (1);
	}

	//attacher le socket au port
	int opt = 1;
	if (setsockopt(server_fd, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt)) < 0)
	{
		std::cerr << "Error set socket" << std::endl;
		return (1);
	}

	std::memset(&address, 0, addrlen);
	address.sin_family = AF_INET;
	address.sin_addr.s_addr = INADDR_ANY;
	address.sin_port = htons(PORT);

	if (bind(server_fd, (struct sockaddr*)&address, addrlen) < 0)
	{
		std::cerr << "Error bind" << std::endl;
		return (1);
	}

	if (listen(server_fd, 10) < 0)
	{
		std::cerr << "Error listen" << std::endl;
		return (1);
	}

	std::cout << "Serveur HTTP demarre sur le port " << PORT << std::endl;

	//boucle infini, sans interuption(logique)
	while (true)
	{
		client_fd = accept(server_fd, (struct sockaddr*)&address, (socklen_t*)&addrlen);
		if (client_fd < 0)
		{
			std::cerr << "Error accept client" << std::endl;
			continue;
		}
		char	buffer[2048];
		std::memset(buffer, 0, sizeof(buffer));
		int valread = read(client_fd, buffer, sizeof(buffer) - 1);
		if (valread > 0)
		{
			std::string request(buffer);
			std::string path = get_path(request);
			
			std::cout << "Requete :\n" << request << std::endl;

            std::string full_path = std::string(WEBROOT) + path;
			std::string content;
			if (full_path.find("png") == std::string::npos || full_path.find("jpeg") == std::string::npos || full_path.find("jpg") == std::string::npos)
            	content = read_file(full_path);
			else
				content = read_file_binary(full_path);
			//std::cout << "Body :" << content << std::endl;
            if (content == "404")
            {
                content = "<html><body><h1>404 Not Found</h1></body></html>";
                std::string reponse = build_reponse(content, "text/html");
				std::cout << "Reponse :\n" << reponse << std::endl;
                send(client_fd, reponse.c_str(), reponse.size(), 0);
            }
            else
            {
                std::string type = get_content_type(path);
				std::cout << "Type : " << type << std::endl;
                std::string reponse = build_reponse(content, type);
				std::cout << "Reponse :\n" << reponse << std::endl;
                send(client_fd, reponse.c_str(), reponse.size(), 0);
            }
		}
		close(client_fd);
	}
	close(server_fd);
	return (0);
}

