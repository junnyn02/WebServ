#include "serverCore.hpp"

serverCore::serverCore() : port(8080), error_num(0), client_max_body_size(0)
{
}

serverCore::~serverCore() 
{
	close(serverSocket);

	std::map<int, clientData>::iterator it;
	for (it = discussions.begin(); it != discussions.end(); it++)
		close(it->first);
	close (epoll_fd);
	discussions.clear();
}

void	serverCore::serverError(std::string str)
{
	std::cerr << "Internal Server Error: " << str << std::endl;
	std::exit(EXIT_FAILURE);
}

void	serverCore::resetDiscussion(int fd)
{
	discussions[fd].body.clear();
	discussions[fd].headerComplete = false;
	discussions[fd].requestComplete = false;
	discussions[fd].sendingResponse = false;
	// discussions[fd].request.clear()
}

void	serverCore::removeClient(int fd)
{
	close(fd); // This automatically removes it from epoll
	discussions.erase(fd);
}

void	serverCore::setBaseSocket() // LOOK UP HOW TO LISTEN TO MUTLIPLE PORTS
{
	serverSocket = socket(AF_INET, SOCK_STREAM, 0);
	if (serverSocket < 0)
		serverError("Failed to create socket.");

	sockAddr.sin_family = AF_INET;
	sockAddr.sin_port = htons(port);
	sockAddr.sin_addr.s_addr = htonl(INADDR_ANY);

	int opt = 1;
	if (setsockopt(serverSocket, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt)) < 0)
		serverError("Failed to set socket options.");

	if (bind(serverSocket, (struct sockaddr*)&sockAddr, sizeof(sockaddr_in)) < 0)
	{
		std::ostringstream s;
		s << "Failed to bind to port " << port << "." << std::endl;
		serverError(s.str());
	}
}

void	serverCore::setNonBlocking(int fd)
{
	int flags = fcntl(fd, F_GETFL, 0);
	if (flags < 0)
		serverError("fcntl(F_GETFL)");
	if (fcntl(fd, F_SETFL, flags | O_NONBLOCK) < 0) 
		serverError("fcntl(F_SETFL)");
}

void	serverCore::createEpoll()
{
	epoll_fd = epoll_create1(0);
	if (epoll_fd == -1)
		serverError("Failed to create epoll.");

	event.events = EPOLLIN; // read events
	event.data.fd = serverSocket;

	if (epoll_ctl(epoll_fd, EPOLL_CTL_ADD, serverSocket, &event) == -1) 
		serverError("epoll_ctl: serverSocket");
}

void serverCore::changeSocketState(int client_fd, int mode) 
{
	struct epoll_event ev;
	ev.data.fd = client_fd;
	ev.events = mode;

	if (epoll_ctl(epoll_fd, EPOLL_CTL_MOD, client_fd, &ev) == -1) 
		std::cerr << "Error: could not mark fd " << client_fd << " as " << (mode == EPOLLIN ? "EPOLLIN\n" : "EPOLLOUT\n");
}

void	serverCore::startServer() 
{
	// 1. Create socket
	// 2. Set socket options (e.g., SO_REUSEADDR)
	// 3. Bind to an address and port
	setBaseSocket();

	// Set the socket to be non-blocking
	setNonBlocking(serverSocket);

	// Listen for connections
	if (listen(serverSocket, SOMAXCONN) < 0) //change SOMAXCON value to however much we need
		serverError("Error: Failed to listen on server socket.");

	createEpoll();
}

void	serverCore::startServer(char *path) 
{
	(void)path;
	// Parse config file
	/*
		- open file with path

		- read until 
			1. error with open/read
			2. error in format
			3. end of file
		
		- get all info and setup serv with it
	*/

	std::cout << "Config file used (not implemented yet)" << std::endl;
	exit (1);
}

void	serverCore::acceptNewClients()
{
	while (1)
	{
		struct sockaddr_in client_addr;
		socklen_t client_len = sizeof(client_addr);
		int client_fd = accept(serverSocket, (struct sockaddr *)&client_addr, &client_len);

		if (client_fd == -1) // /!\ CANT USE ERRNO /!\ to research 
		{
			if (errno == EAGAIN || errno == EWOULDBLOCK) // All connections have been processed
				return;
			else 
			{
				std::cerr << "Internal Server Error: accept on server socket" << std::endl;
				return;
			}
		}

		// Set the new client socket to non-blocking
		setNonBlocking(client_fd);

		// Add the new client socket to the epoll interest list
		event.events = EPOLLIN;
		event.data.fd = client_fd;
		if (epoll_ctl(epoll_fd, EPOLL_CTL_ADD, client_fd, &event) == -1) 
		{
			std::cerr << "Error: epoll_ctl: client_fd" << std::endl;
			close(client_fd);
		}
		clientData data;
		data.clientSocket = client_fd;
		data.size = 0;
		data.requestComplete = false;
		data.headerComplete = false;
		data.sendingResponse = false;
		
		discussions[client_fd] = data;
	}
}

int	serverCore::receiveRequest(int fd)
{
	ssize_t valread;

	char buffer[BUFFER_SIZE];

	// Read data from the client
	// while (
	if ((valread = recv(fd, buffer, BUFFER_SIZE, 0)) > 0)
	{
		discussions[fd].body.append(buffer, valread);
		discussions[fd].size += valread;

		if (!discussions[fd].headerComplete && discussions[fd].body.find("\r\n\r\n") != std::string::npos)
		{
			discussions[fd].headerComplete = true;
			discussions[fd].request.fillRequest(discussions[fd].body, discussions[fd].size);
			if (discussions[fd].request.getSize() == 0)
			{
				discussions[fd].requestComplete = true;
				changeSocketState(fd, EPOLLOUT);
				return 1;
			}
			return 0;
		}
		if (discussions[fd].headerComplete)
		{
			int start = discussions[fd].body.find("\r\n\r\n") + 4;
			if (discussions[fd].size - start == discussions[fd].request.getSize()) 
			{
				std::string body = discussions[fd].request.parseBody(discussions[fd].body);
				discussions[fd].request.setBody(body);
				discussions[fd].requestComplete = true;
				changeSocketState(fd, EPOLLOUT);
				//discussions[fd].request.printRequest();
				return 1;
			}
			else
				return 0;
		}
	}
	else if (valread == 0) // Client closed the connection
	{
		std::cout << "Client " << fd << " disconnected" << std::endl;
		if (!discussions[fd].requestComplete)
		{
			discussions[fd].request.setStatus(400);
			changeSocketState(fd, EPOLLOUT);
			return (1);
		}
		removeClient(fd);
		return -1;
	}
	else // internal server error ig
	{
		std::cerr << "Internal Server Error: recv" << std::endl;
		removeClient(fd);
		return -1;
	}

	std::cerr << "NOT POSSIBLE : SEEK HELP FROM PRIEST IF YOU SEE THIS ERROR\n";
	return 0;
}

void	serverCore::sendResponse(int client_fd) //, std::string* response) //, const std::vector<char> &body)
{
	ssize_t	remaining = discussions[client_fd].size;
	ssize_t sent = send(client_fd, discussions[client_fd].body.c_str(), discussions[client_fd].size, 0); // do we need flags ???
	if (sent < 0)
	{
		// std::cerr << BOLD RED "Error: failed to send data to client "  RESET << client_fd << std::endl;
		removeClient(client_fd);
		return;
	}
	else if (sent == remaining)
	{
		// std::cout << BOLD GREEN "Success: All data sent to client (" << sent << ")"  RESET << client_fd << std::endl;
		resetDiscussion(client_fd) ; // clear data
		// changeSocketState(client_fd, EPOLLIN);
		removeClient(client_fd); // temporary : if we sent answer we kill the client after for cleanup
	}
	else 
	{
		// std::cout << BOLD CYAN "Success: " << sent << "/" << discussions[client_fd].size << "B sent to client "  RESET << client_fd << std::endl;
		discussions[client_fd].body.erase(0, sent);
		discussions[client_fd].size -= sent;
	}
}

int	serverCore::getfd()
{ 
	return (serverSocket);
}

clientData	serverCore::getData(int fd)
{
	return (discussions[fd]);
}


void		serverCore::setResponse(int fd, std::string& str, ssize_t len)
{
	discussions[fd].body = str;
	discussions[fd].size = len;
}