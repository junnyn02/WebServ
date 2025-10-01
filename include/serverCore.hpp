#pragma once

#include <cstring>
#include <cstdlib>
#include <iostream>
#include <netinet/in.h>
#include <sys/socket.h>
#include <unistd.h>

#define EXIT_FAILURE 1
#define BUFFER_SIZE 1000000
#define PORT 8080

typedef struct clientData
{
	char	buffer[BUFFER_SIZE];
	size_t	size;
	int		clientSocket;
} clientData;

class serverCore
{
private:

	int	serverSocket;
	sockaddr_in sockAddr;

public:

	int			port;					// listening port, mandatory parameter
	// int			host[4];				// host ip adress  127.0.0.1 ???
	int			error_num;				//404 ?
	int			client_max_body_size;	// max request body size in bytes
	std::string	server_name;			// specify server_name, need to be added into /etc/hosts to work
	std::string	root;					// root folder of site directory, full or relative path, mandatory parameter
	std::string	index;					// default page when requesting a directory, index.html by default
	std::string	error_page;				// default error page

	std::string	cgi_root;
	std::string	cgi_path;
	std::string	cgi_ext;
	//allowed methods   allow_methods POST GET; (could do an int value and check binary (like 1==GET, 10==POST, 100==other etc))

	serverCore();
	~serverCore();
	
	void		startServer();

	clientData	receiveRequest();
	void		sendResponse(clientData);

	int	connectAndReceive();
};
