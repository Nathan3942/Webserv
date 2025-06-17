/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   testSrvWeb.cpp                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: njeanbou <njeanbou@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/06/12 12:41:55 by njeanbou          #+#    #+#             */
/*   Updated: 2025/06/17 14:44:25 by njeanbou         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <iostream>
#include <string>
#include <sstream>
#include <unistd.h>
#include <netinet/in.h>
#include <cstring>
#include <cstdio>

#define PORT 8080


//reponse http
std::string	build_reponse(const std::string& body)
{
	std::string reponse;
	//protocole http et code reponse 200 = ok (faire les erreur)
	reponse += "HTTP/1.1 200 \r\n";
	/*indique le type du contenu (ici page http, autre ex )
	text = text/plain
	json = application/json
	image image/png
	css text/css

	cgi a part il faut fork, lancer le cgi depuis le server et le cgi envoie la reponse pour le client 
	*/
	reponse += "Content-Type: text/html\r\n";
	//longueur en octets
	reponse += "Content-Lengh: ";
	char lenght[20];
	sprintf(lenght, "%lu", (unsigned long)body.size());
	reponse += std::string(lenght);
	//\r\n\r\n indique fin du header http
	reponse += "\r\nConnection: close\r\n\r\n";
	reponse += body;
	return (reponse);
}

int main ()
{
	/*Premiere etape : read le fichier conf pour set le bon nombre de port etc*/

    int	server_fd, client_fd;
	struct sockaddr_in	address;	//info tcp ip port
	int addrlen = sizeof(address);

	//cree un socket
	//int socket(int __domain, int __type, int __protocol)
	//AF_INET pour utiliser IPv4, AF_INET6 IPv6, AF_UNIX en local(processus sur la meme machine)
	//SOCK_STREAM pour TCP, SOCK_DGRAM pour UDP, SOCK_RAW pour paquet brut
	//0 pour laiser le system choisir le protocol
	server_fd = socket(AF_INET, SOCK_STREAM, 0);
	if (server_fd < 0)
	{
		std::cerr << "Error socket server" << std::endl;
		return (1);
	}

	//attacher le socket au port
	int opt = 1;
	//int setsockopt(int __fd, int __level, int __optname, const void *__optval, socklen_t __optlen)
	//configurer les option du socket
	//level pour niveau de l'option ex option du socket ou option du protocol comme tcp
	//__optname pour l'option a activer, SO_REUSEADDR permet de pouvoir reutiliser le port et l'ip
	//quand server sarete le port tcp peut rester en TIME_WAIT pendant qlq minute, SO_REUSEADDR ignore le wait et pour le reutiliser direct
	//opt active ou desactive l'option, dernier taille de l'option (interet?)
	if (setsockopt(server_fd, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt)) < 0)
	{
		std::cerr << "Error set socket" << std::endl;
		return (1);
	}
	//memset met la structure a zero pour eviter les valeur indefinie(ui jave oublier)
	std::memset(&address, 0, addrlen);
	//pour lui dire quon travaille en ipv4
	address.sin_family = AF_INET;
	//lui dire d'ecouter sur toutes les interface reseau local
	//juste pour localhost inet_pton(AF_INET, "127.0.0.1", &address.sin_addr);, a tester
	address.sin_addr.s_addr = INADDR_ANY;
	//indique le port sur lequel le serveur va ecouter, hton = host to network short, pour convertir au format reseau
	address.sin_port = htons(PORT);

	/*pour ecouter sur plusieurs port il faut cree plusieurs socket dans une boucle for et les surveiller avec un select, poll ou epoll
	et detecter si l'un dentre eu est pret a recevoir une connexion*/


	//int bind(int __fd, const sockaddr *__addr, socklen_t __len)
	//associer un socket a une ip et un port pour ecouter dessus
	if (bind(server_fd, (struct sockaddr*)&address, addrlen) < 0)
	{
		std::cerr << "Error bind" << std::endl;
		return (1);
	}

	//met le socket en mode passif pour quil attende une connexion entrante
	//int listen(int __fd, int __n)
	//_n pour la taille de la fille dattente, ici 10 connexion en attente de accept
	if (listen(server_fd, 10) < 0)
	{
		std::cerr << "Error listen" << std::endl;
		return (1);
	}

	std::cout << "Serveur HTTP demarre sur le port " << PORT << std::endl;

	//boucle infini, sans interuption(logique)
	while (true)
	{
		//permet au serveur d'accepter une connexion qui est dans la file d'attente
		//cree un nouveau socket pour la communication avec le client accepte
		//int accept(int __fd, sockaddr *__restrict__ __addr, socklen_t *__restrict__ __addr_len)
		client_fd = accept(server_fd, (struct sockaddr*)&address, (socklen_t*)&addrlen);
		if (client_fd < 0)
		{
			std::cerr << "Error accept client" << std::endl;
			continue;
		}

		char	buffer[2048];
		std::memset(buffer, 0, sizeof(buffer));
		//read la demande du client
		int valread = read(client_fd, buffer, sizeof(buffer) - 1);
		if (valread > 0)
		{
			std::cout << "Requete recue :\n" << buffer << std::endl;
			//depuis firefox sur http://localhost:8080
			//GET .. ex /www/index.html / HTTP/1.1
			//Host: localhost:8080
			//User-Agent: Mozilla/5.0 (X11; Ubuntu; Linux x86_64; rv:135.0) Gecko/20100101 Firefox/135.0
			//Accept: text/html,application/xhtml+xml,application/xml;q=0.9,*/*;q=0.8
			//Accept-Language: fr,fr-FR;q=0.8,en-US;q=0.5,en;q=0.3
			//Accept-Encoding: gzip, deflate, br, zstd
			//Connection: keep-alive
			//Upgrade-Insecure-Requests: 1
			//Sec-Fetch-Dest: document
			//Sec-Fetch-Mode: navigate
			//Sec-Fetch-Site: none
			//Sec-Fetch-User: ?1
			//Priority: u=0, i

			std::string body = "<html><body><h1>Hello from njeanbou webserv!<h1><body><html>";
			std::string reponse = build_reponse(body);
			send(client_fd, reponse.c_str(), reponse.size(), 0);

			std::cout << "Reponse server :\n" << reponse << std::endl;
			/*HTTP/1.1 200 
			Content-Type: text/html
			Content-Lengh: 60
			Connection: close

			<html><body><h1>Hello from njeanbou webserv!<h1><body><html>
			*/
		}
		close(client_fd);
	}
	close(server_fd);
	return (0);
}


/*
Sujet :
	-prendre fichier conf ou utiliser chemin par defaut, deff const chemin defeau ou si non cree dossier?
	-pas executer un autre server web? pas 2 en meme temps? quand je lance deux en meme temps "Error bind" bon?
	-non bloquant? jamais fige, doit pouvoir gerer dautre client en attendant la reponse dun client(sans thread ou fork), socket non-bloquant (fcntl(sock, F_SETFL, O_NONBLOCK))
	-gerer les deconnexion proprement, detecter co ferme, clean les resources, ne pas crash ou lire un socket mort
	-un seul poll dans la boucle pour les entre/sortie, poll retourn si le client est pret ou sil a quiter (POLLHUP, POLLERR)
	-toujours passer par poll pour lecture ecriture
	-jamais verifier valeur errno
*/