/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   responce.cpp                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: njeanbou <njeanbou@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/06/20 11:46:56 by ichpakov          #+#    #+#             */
/*   Updated: 2025/06/26 18:57:40 by njeanbou         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../include/responce.hpp"


Responce::Responce(const std::string& path)
{
    content = read_file(path);
    if (content == "404")
    {
        content = read_file("./www/404.html");
        content_type = "text/html";
    }
    else
        content_type = get_content_type(path);
    http_response = build_reponse(content);
}

Responce::~Responce()
{

}

std::string read_binary(const std::string& filepath)
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

std::string read_default(const std::string& filepath)
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

std::string Responce::read_file(const std::string& path)
{
    std::string full_path = std::string(WEBROOT) + path;
		
		if (full_path.find("png") == std::string::npos &&
			full_path.find("jpeg") == std::string::npos &&
			full_path.find("jpg") == std::string::npos)
		   	return (read_default(full_path));
		else
			return (read_binary(full_path));
}

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

std::string Responce::get_content_type(const std::string& path)
{
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


std::string Responce::build_reponse(const std::string& body)
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

std::string Responce::get_response() const
{
    return (http_response);
}
