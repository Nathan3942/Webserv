/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.cpp                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: njeanbou <njeanbou@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/06/20 11:58:45 by ichpakov          #+#    #+#             */
/*   Updated: 2025/06/26 19:17:49 by njeanbou         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../include/server.hpp"

int main()
{
    std::vector<int> ports;
    ports.push_back(8080);
    ports.push_back(8081);
    ports.push_back(8082);
    Server* s = new Server(ports);
    s->start();
    
    delete s;
    return 0;
}
