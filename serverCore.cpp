#include "serverCore.hpp"

serverCore::serverCore() : port(8080), error_num(0), client_max_body_size(0)
{
}

serverCore::~serverCore() {}

int	serverCore::connectAndReceive()
{
	int serverSocket = socket(AF_INET, SOCK_STREAM, 0);
	if (serverSocket == -1) 
	{
		std::cout << "Failed to create socket." << std::endl;
		std::exit(EXIT_FAILURE);
	}
// should we have several sockets ?
	sockaddr_in sockAddr;
	sockAddr.sin_family = AF_INET;
	sockAddr.sin_addr.s_addr = INADDR_ANY;
	sockAddr.sin_port = htons(PORT);

	unsigned long addrlen = sizeof(sockaddr_in);
	if (bind(serverSocket, (struct sockaddr*)&sockAddr, addrlen))
	{
		std::cout << "Failed to bind to port " << PORT << "." << std::endl;
		std::exit(EXIT_FAILURE);
	}
	// listening to the assigned socket
    if (listen(serverSocket, 10) < 0) //change 5 to however much we need
	{
		std::cout << "Failed to listen on socket." << std::endl;
		std::exit(EXIT_FAILURE);
	}


	std::string hello("HTTP/1.1 200 OK\nContent-Type: text/plain\nContent-Length: 12\n\nHello world!");//IMPORTANT! WE WILL GET TO IT
	while (1)
	{
		//need to study accept. seems weird
    	// int clientSocket = accept(serverSocket, 0, 0); //if -1 error happened
		int clientSocket = accept(serverSocket, (struct sockaddr*)&sockAddr, (socklen_t*)&addrlen);
		if (clientSocket < 0) {
			std::cout << "Failed to grab connection." << std::endl;
			std::exit(EXIT_FAILURE);
		}

		//	Step 4 ->> send and receive messages ( Gael and Julien <3 )
		char buffer[1024] = {0};
		int valread = read(clientSocket , buffer, 1024); 
		std::cout << buffer << std::endl;
		if(valread < 0)
			std::cout << "No bytes are there to read" << std::endl;
	
		write(clientSocket , hello.c_str() , strlen(hello.c_str()));

		close(clientSocket);
	}
	close(serverSocket);

	return (0);
}