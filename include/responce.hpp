/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   responce.hpp                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: njeanbou <njeanbou@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/06/20 11:48:05 by ichpakov          #+#    #+#             */
/*   Updated: 2025/06/26 18:56:56 by njeanbou         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

//REPONSE HTTP

#pragma once

#include <iostream>
#include <string>

#include "server.hpp"

#define WEBROOT "./www"

class Responce
{
private:
	std::string	content;
	std::string	content_type;
	std::string	http_response;
	
public:
    Responce(const std::string& path);
    ~Responce();

	std::string	read_file(const std::string& path);
	std::string get_content_type(const std::string& path);
	std::string	build_reponse(const std::string& body);

	std::string	get_response() const;
};


