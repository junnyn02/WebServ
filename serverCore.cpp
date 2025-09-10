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
	if (serverSocket == -1) 
	{
		std::cout << "Failed to create socket." << std::endl;
		std::exit(EXIT_FAILURE);
	}

// should we have several sockets ?
	sockAddr.sin_family = AF_INET;
	sockAddr.sin_addr.s_addr = INADDR_ANY;
	sockAddr.sin_port = htons(port);

	if (bind(serverSocket, (struct sockaddr*)&sockAddr, sizeof(sockaddr_in)))
	{
		std::cout << "Failed to bind to port " << port << "." << std::endl;
		std::exit(EXIT_FAILURE);
	}
	// listening to the assigned socket
	if (listen(serverSocket, 10) < 0) //change 5 to however much we need
	{
		std::cout << "Failed to listen on socket." << std::endl;
		std::exit(EXIT_FAILURE);
	}
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
	//	Step 4 ->> send and receive messages ( Gael and Julien <3 )

	int valread = read(data.clientSocket , data.buffer, 1024); 
	std::cout << data.buffer << std::endl;
	if(valread < 0)
		std::cout << "No bytes are there to read" << std::endl;
	return (data);
}

void	serverCore::sendResponse(clientData data, std::string response)
{
	write(data.clientSocket , response.c_str() , strlen(response.c_str()));

	close(data.clientSocket);
}
