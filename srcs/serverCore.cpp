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
	std::cout << str << std::endl;
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

void	serverCore::setNonBlocking()
{
	/* ----- Set non blocking ----- */
	int flags = fcntl(serverSocket, F_GETFL, 0);
	if (flags < 0) {
		std::cout << "fcntl(F_GETFL)" << std::endl;
		std::exit(EXIT_FAILURE);
	}
	if (fcntl(serverSocket, F_SETFL, flags | O_NONBLOCK) < 0) {
		std::cout << "fcntl(F_SETFL)" << std::endl;
		std::exit(EXIT_FAILURE);
    }
	/* ---------------------------- */
}

void	serverCore::setEpoll()
{
	/* -------- Set epoll  -------- */
	// epoll_fd = epoll_create1(0);
	// if (epoll_fd == -1)
	// {
	// 	std::cout << "Failed to listen on socket." << std::endl;
	// 	std::exit(EXIT_FAILURE);
	// }

	// event.events = EPOLLIN; // read events
	// event.data.fd = serverSocket;

	// if (epoll_ctl(epoll_fd, EPOLL_CTL_ADD, serverSocket, &event) == -1) 
	// {
	// 	std::cout << "Failed to listen on socket." << std::endl;
	// 	std::exit(EXIT_FAILURE);
	// }
	/* ---------------------------- */
}

void	serverCore::startServer() 
{
	// 1. Create socket
	// 2. Set socket options (e.g., SO_REUSEADDR)
	// 3. Bind to an address and port
	setBaseSocket();

	// Set the socket to be non-blocking
	setNonBlocking();

	// Listen for connections
	if (listen(serverSocket, 10) < 0) //change value to however much we need
	{
		std::cout << "Failed to listen on socket." << std::endl;
		std::exit(EXIT_FAILURE);
	}

	// setEpoll();
}

clientData	serverCore::receiveRequest()
{
	clientData data;
	unsigned long addrlen = sizeof(sockaddr_in);
	data.clientSocket = accept(serverSocket, (struct sockaddr*)&sockAddr, (socklen_t*)&addrlen);
	if (data.clientSocket < 0) {
		std::cout << "Failed to grab connection." << std::endl;
		std::exit(EXIT_FAILURE);
	}

	size_t valread = recv(data.clientSocket, data.buffer, BUFFER_SIZE, 0); // do we need flags ??? MSG_DONTWAIT ?
	// std::cout << "data.buffer:\n" << data.buffer << std::endl << std::endl;
	if(valread == 0)
		std::cout << "No bytes are there to read" << std::endl;
	data.size = valread;
	return (data);
}

void	serverCore::sendResponse(clientData data)
{
	send(data.clientSocket, data.buffer, data.size, 0); // do we need flags ???
	close(data.clientSocket);
}
