#include "serverCore.hpp"

serverCore::serverCore()
{
}

serverCore::serverCore(std::vector<Config*>& servers)
{
	_epoll_fd = epoll_create1(0);
	if (_epoll_fd == -1)
		throw (std::runtime_error("\nFailed to create epoll instance."));
	for (size_t i = 0; i < servers.size(); i++)
		if (!startServer(dynamic_cast<Server*>(servers[i])))
			throw (std::runtime_error("\nFailed to start server."));
}

serverCore::~serverCore() 
{
	// close(_serverSocket); //obsolete
	
	// closing client fd removes them from epoll
	std::map<int, clientData>::iterator it;
	for (it = _clients.begin(); it != _clients.end(); it++)
		close(it->first);

	std::map<int, Server*>::iterator it2;
	for (it2 = _servers.begin(); it2 != _servers.end(); it2++)
		close(it2->first);

	close (_epoll_fd);
	_clients.clear();
}

void	serverCore::resetDiscussion(int fd)
{
	_clients[fd].size = 0;
	_clients[fd].body.clear();
	_clients[fd].headerComplete = false;
	_clients[fd].requestComplete = false;
	_clients[fd].sendingResponse = false;
}

void	serverCore::removeClient(int fd)
{
	close(fd); // This automatically removes it from epoll apparently ???
	_clients.erase(fd);
}

void	serverCore::setSocket(Server* server, int server_sock)
{
	sockaddr_in	sock;
	sock.sin_family = AF_INET;
	sock.sin_port = htons(server->getPort());
	sock.sin_addr.s_addr = htonl(INADDR_ANY);

	int opt = 1;
	if (setsockopt(server_sock, SOL_SOCKET, SO_REUSEADDR | SO_REUSEPORT, &opt, sizeof(opt)) < 0)
		throw (std::runtime_error("\nFailed to set socket options."));

	if (bind(server_sock, (struct sockaddr*)&sock, sizeof(sockaddr_in)) < 0)
	{
		std::ostringstream s;
		s << "Failed to bind to port " << server->getPort() << "." << std::endl;
		throw (std::runtime_error(s.str()));
	}
}

void	serverCore::setNonBlocking(int fd)
{
	int flags = fcntl(fd, F_GETFL, 0);
	if (flags < 0)
		throw (std::runtime_error("fcntl(F_GETFL)"));
	if (fcntl(fd, F_SETFL, flags | O_NONBLOCK) < 0) 
		throw (std::runtime_error("fcntl(F_SETFL)"));
}

void	serverCore::addToEpoll(int socket)
{
	struct epoll_event ev;

	ev.events = EPOLLIN; // read events
	ev.data.fd = socket;
	
	if (epoll_ctl(_epoll_fd, EPOLL_CTL_ADD, socket, &ev) == -1) 
	{
		close(socket);
		throw (std::runtime_error("Error: epoll_ctl: could not mark server fd"));
	}
}

bool	serverCore::startServer(Server* serv)
{
	int serv_sock = socket(AF_INET, SOCK_STREAM, 0);
	if (serv_sock < 0)
		throw (std::runtime_error("Failed to create socket."));

	setSocket(serv, serv_sock);
	setNonBlocking(serv_sock);
	if (listen(serv_sock, 5) < 0)
		throw (std::runtime_error("Failed to listen on server Socket."));
	addToEpoll(serv_sock);

	_servers[serv_sock] = serv;
	return true;
}

void serverCore::changeSocketState(int client_fd, int mode) 
{
	struct epoll_event ev;
	ev.data.fd = client_fd;
	ev.events = mode;

	if (epoll_ctl(_epoll_fd, EPOLL_CTL_MOD, client_fd, &ev) == -1)
	{
		std::ostringstream s;
		s << "\nError: could not mark fd " << client_fd << " as " << (mode == EPOLLIN ? "EPOLLIN.\n" : "EPOLLOUT.\n");
		throw (std::runtime_error(s.str()));
	}
}

void	serverCore::acceptNewClients(int server_fd)
{
	while (1)
	{
		struct sockaddr_in client_addr;
		socklen_t client_len = sizeof(client_addr);
		int client_fd = accept(server_fd, (struct sockaddr *)&client_addr, &client_len);
		if (client_fd == -1) // All connections have been processed (or server error but we can't check errno lol)
			return;

		setNonBlocking(client_fd);
		addToEpoll(client_fd);

		std::map<int, Server*>::iterator it = _servers.find(server_fd);
		if (it == _servers.end())
			throw (std::runtime_error("\nClient trying to communicate with non-existing server."));
		
		clientData data;
		data.clientSocket = client_fd;
		data.size = 0;
		data.requestComplete = false;
		data.headerComplete = false;
		data.sendingResponse = false;
		data.request.setServer(it->second);
		
		_clients[client_fd] = data;
	}
}

// /!\ Need to improve cause ugly af
int	serverCore::receiveRequest(int fd)
{
	ssize_t valread;
	char buffer[BUFFER_SIZE];

	if ((valread = recv(fd, buffer, BUFFER_SIZE, 0)) > 0)
	{
		_clients[fd].body.append(buffer, valread);
		_clients[fd].size += valread;

		if (!_clients[fd].headerComplete && _clients[fd].body.find("\r\n\r\n") != std::string::npos)
		{
			_clients[fd].headerComplete = true;
			_clients[fd].request.fillRequest(_clients[fd].body, _clients[fd].size);
			int start = _clients[fd].body.find("\r\n\r\n") + 4;
			if (_clients[fd].request.getSize() == 0)
			{
				_clients[fd].requestComplete = true;
				changeSocketState(fd, EPOLLOUT);
				return 1;
			}
			else if (_clients[fd].size - start == _clients[fd].request.getSize())
			{
				_clients[fd].requestComplete = true;
				std::string body = _clients[fd].request.parseBody(_clients[fd].body);
				_clients[fd].request.setBody(body);
				changeSocketState(fd, EPOLLOUT);
				return 1;
			}
			return 0;
		}
		if (_clients[fd].headerComplete)
		{
			int start = _clients[fd].body.find("\r\n\r\n") + 4;
			if (_clients[fd].size - start == _clients[fd].request.getSize()) 
			{
				std::string body = _clients[fd].request.parseBody(_clients[fd].body);
				_clients[fd].request.setBody(body);
				_clients[fd].requestComplete = true;
				changeSocketState(fd, EPOLLOUT);
				//_clients[fd].request.printRequest();
				return 1;
			}
			else
				return 0;
		}
	}
	else if (valread == 0) // Client closed the connection
	{
		std::cout << "Client " << fd << " disconnected" << std::endl; // need to remove ?
		if (!_clients[fd].requestComplete)
		{
			_clients[fd].request.setStatus(400);
			changeSocketState(fd, EPOLLOUT);
			return 1;
		}
		removeClient(fd);
		return -1;
	}
	else
		throw (std::runtime_error("\nError: recv fail."));
	// {
	// 	std::cerr << "Internal Server Error: recv" << std::endl;
	// 	removeClient(fd);
	// 	return -1;
	// }

	return 0;
}

void	serverCore::sendResponse(int client_fd)
{
	ssize_t	remaining = _clients[client_fd].size;
	ssize_t sent = send(client_fd, _clients[client_fd].body.c_str(), _clients[client_fd].size, 0); // do we need flags ???
	if (sent < 0)
	{
		removeClient(client_fd);
		return;
	}
	else if (sent == remaining)
	{
		resetDiscussion(client_fd) ; // clear data
		removeClient(client_fd); // temporary : if we sent answer we kill the client after for cleanup
	}
	else 
	{
		_clients[client_fd].body.erase(0, sent);
		_clients[client_fd].size -= sent;
	}
}

int	serverCore::getfd() const
{
	return _epoll_fd;
}

bool	serverCore::findServer(int fd) const
{
	if (_servers.find(fd) != _servers.end())
		return true;
	return false;
}

bool		serverCore::clientIsResponding(int client)
{

	return _clients[client].sendingResponse;
}

Request&	serverCore::getRequest(int client)
{
	return (_clients[client].request);
}

void		serverCore::changeClientState(int client)
{
	_clients[client].sendingResponse = true;
}

void		serverCore::setResponse(int fd, std::string& str, ssize_t len)
{
	_clients[fd].body = str;
	_clients[fd].size = len;
}
