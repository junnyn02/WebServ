#pragma once

#include "utils.hpp"
#include "Request.hpp"
#include "Config.hpp"

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
	// bool		requestChunked;
	bool		requestComplete;
	bool		headerComplete;
	bool		sendingResponse;
	Request		request;
} clientData;

class serverCore
{
private:

	std::map<int, Server*>		_servers;
	
	void	setSocket(Server*, int);
	bool	addToEpoll(int);
	void	startServer(Server*);
	
public:
	int			_epoll_fd;
	struct epoll_event _event;
	//allowed methods   allow_methods POST GET; (could do an int value and check binary (like 1==GET, 10==POST, 100==other etc))
	
	std::map<int, clientData>	_clients;

	serverCore();
	serverCore(std::vector<Config*>&);
	~serverCore();
	
	void		serverError(std::string);
	void		setNonBlocking(int);
	void		changeSocketState(int, int);

	void		acceptNewClients(int);

	void		resetDiscussion(int);
	void		removeClient(int);

	int			receiveRequest(int);
	// void		sendResponse(int);
	void		sendResponse(int); //, std::string *); //,const std::vector<char> &);
	// clientData	receiveRequest(int fd);
	// void		sendResponse(clientData);

	bool		findServer(int);
	void		setResponse(int, std::string&, ssize_t);
};
