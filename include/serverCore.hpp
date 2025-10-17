#pragma once

#include "utils.hpp"
#include "Request.hpp"
#include "Config.hpp"
#include "Server.hpp"

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

	int							_epoll_fd;
	std::map<int, Server*>		_servers;
	std::map<int, clientData>	_clients;
	
	void	addToEpoll(int);
	void	setNonBlocking(int);
	void	setSocket(Server*, int);
	void	changeSocketState(int, int);

	bool	startServer(Server*);

	void	removeClient(int);
	void	resetDiscussion(int);
	
public:
	serverCore();
	serverCore(std::vector<Config*>&);
	~serverCore();

	void		acceptNewClients(int);
	int			receiveRequest(int);
	void		sendResponse(int);

	int			getfd() const;
	bool		findServer(int) const;
	bool		clientIsResponding(int);
	Request&	getRequest(int);

	void		changeClientState(int);
	void		setResponse(int, std::string&, ssize_t);
};
