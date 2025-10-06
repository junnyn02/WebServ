#pragma once

#include "utils.hpp"

#define EXIT_FAILURE 1
#define INTERNAL_SERVER_ERROR 500
#define BUFFER_SIZE 100000
#define PORT 8080
#define MAX_EVENTS 10

typedef struct clientData
{
	int			clientSocket;
	int			size;
	std::string	body;
	bool		read;
	// bool	write;
} clientData;

class serverCore
{
private:

	int							serverSocket;
	sockaddr_in 				sockAddr;
	
	void	setBaseSocket();
	void	createEpoll();
	
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
	
	int			epoll_fd;
	struct epoll_event event;
	//allowed methods   allow_methods POST GET; (could do an int value and check binary (like 1==GET, 10==POST, 100==other etc))
	
	std::map<int, clientData>	discussions;

	serverCore();
	~serverCore();
	
	void		serverError(std::string);
	void		startServer();
	void		startServer(char *);
	void		setNonBlocking(int fd);
	void		acceptNewClients();

	void		receiveRequest(int fd);
	void		sendResponse(int fd);
	// clientData	receiveRequest(int fd);
	// void		sendResponse(clientData);

	int			getfd();
	clientData	getData(int client_fd);
	void		setResponse(int, std::string&, ssize_t);
};
