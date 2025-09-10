#pragma once

#include <cstring>
#include <cstdlib>
#include <iostream>
#include <netinet/in.h>
#include <sys/socket.h>
#include <unistd.h>

#define EXIT_FAILURE 1
#define PORT 8080

class serverCore
{
private:
public:

	int			port;					// listening port, mandatory parameter
	// int			host[4];				// host or 127.0.0.1 by default
	int			error_num;				//404 ?
	int			client_max_body_size;	// max request body size in bytes
	std::string	server_name;			// specify server_name, need to be added into /etc/hosts to work
	std::string	root;					// root folder of site directory, full or relative path, mandatory parameter
	std::string	index;					// default page when requesting a directory, index.html by default
	std::string	error_page;				// default error page

	std::string	cgi_root;
	std::string	cgi_path;
	std::string	cgi_ext;
	//allowed methods   allow_methods POST GET;

	serverCore();
	~serverCore();
	
	int	connectAndReceive();
};
