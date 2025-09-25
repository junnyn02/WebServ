#include "serverCore.hpp"

serverCore::serverCore() : port(8080), error_num(0), client_max_body_size(0)
{
}

serverCore::~serverCore() 
{
	close(serverSocket);
}

void	serverCore::startServer() 
{
	serverSocket = socket(AF_INET, SOCK_STREAM, 0);
	if (serverSocket < 0) 
	{
		std::cout << "Failed to create socket." << std::endl;
		std::exit(EXIT_FAILURE);
	}

// should we have several sockets ?
	sockAddr.sin_family = AF_INET;
	sockAddr.sin_port = htons(port);
	sockAddr.sin_addr.s_addr = htonl(INADDR_ANY);

	if (bind(serverSocket, (struct sockaddr*)&sockAddr, sizeof(sockaddr_in)) < 0)
	{
		std::cout << "Failed to bind to port " << port << "." << std::endl;
		std::exit(EXIT_FAILURE);
	}

	if (listen(serverSocket, 10) < 0) //change value to however much we need
	{
		std::cout << "Failed to listen on socket." << std::endl;
		std::exit(EXIT_FAILURE);
	}
	// where the fuck should i use select ??? and epoll ??????
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
	std::cout << data.buffer << std::endl;
	if(valread < 0)
		std::cout << "No bytes are there to read" << std::endl;
	data.size = valread;
	return (data);
}

void	serverCore::sendResponse(clientData data)
{
	send(data.clientSocket, data.buffer, data.size, 0); // do we need flags ???
	close(data.clientSocket);
}
