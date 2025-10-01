#include "serverCore.hpp"

serverCore::serverCore() : port(8080), error_num(0), client_max_body_size(0)
{
}

serverCore::~serverCore() 
{
	close(serverSocket);
}

void	serverCore::serverError(std::string str)
{
	std::cerr << "Error: " << str << std::endl;
	std::exit(EXIT_FAILURE);
}

void	serverCore::setBaseSocket()
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
	/* ----- Set non blocking ----- */
	int flags = fcntl(fd, F_GETFL, 0);
	if (flags < 0)
		serverError("fcntl(F_GETFL)");
	if (fcntl(fd, F_SETFL, flags | O_NONBLOCK) < 0) 
		serverError("fcntl(F_SETFL)");
	/* ---------------------------- */
}

void	serverCore::setEpoll()
{
	/* -------- Set epoll  -------- */
	epoll_fd = epoll_create1(0);
	if (epoll_fd == -1)
		serverError("Failed to create epoll.");

	event.events = EPOLLIN; // read events
	event.data.fd = serverSocket;

	if (epoll_ctl(epoll_fd, EPOLL_CTL_ADD, serverSocket, &event) == -1) 
		serverError("epoll_ctl: serverSocket");
	/* ---------------------------- */
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

	setEpoll();
}

void	serverCore::acceptNewClients()
{
	while (1)
	{
		struct sockaddr_in client_addr;
		socklen_t client_len = sizeof(client_addr);
		int client_fd = accept(serverSocket, (struct sockaddr *)&client_addr, &client_len);

		if (client_fd == -1) 
		{
			if (errno == EAGAIN || errno == EWOULDBLOCK) // All connections have been processed
				return;
			else 
			{
				std::cerr << "Error: accept on server socket" << std::endl;
				return;
			}
		}

		// Set the new client socket to non-blocking
		setNonBlocking(client_fd);

		// Add the new client socket to the epoll interest list
		event.events = EPOLLIN | EPOLLET; // Monitor for read events, edge-triggered (ET) (instead of Level-Triggered (LT) -> single signal or repeating signals)
		event.data.fd = client_fd;
		if (epoll_ctl(epoll_fd, EPOLL_CTL_ADD, client_fd, &event) == -1) 
		{
			std::cerr << "Error: epoll_ctl: client_fd" << std::endl;
			close(client_fd);
		}
	}
}

clientData	serverCore::receiveRequest(int client_fd)
{
	clientData data;
	data.clientSocket = client_fd;
	ssize_t valread;

	// Read data from the client
	while ((valread = recv(data.clientSocket, data.buffer, BUFFER_SIZE, 0)) > 0)
	{
		data.body.append(data.buffer, valread);
		data.size += valread;
	}

	if (valread == 0) // Client closed the connection
	{
		std::cout << "Client " << data.clientSocket << " disconnected" << std::endl;
		close(data.clientSocket); // This automatically removes it from epoll

		// put in EPOLLOUT mode to send Answer ig
	}
	else if (data.size == -1) // internal server error ig
	{
		std::cerr << "Error: recv" << std::endl;
		close(data.clientSocket);
	}
	return (data);
}

void	serverCore::sendResponse(clientData data)
{
	send(data.clientSocket, data.buffer, data.size, 0); // do we need flags ???
	close(data.clientSocket);
}

int	serverCore::getfd() { return (serverSocket); }