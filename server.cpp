#include "serverCore.hpp"

int	connectAndReceive()
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
	sockAddr.sin_port = htons(8080);

	if (bind(serverSocket, (struct sockaddr*)&sockAddr, sizeof(sockaddr_in)))
	{
		std::cout << "Failed to bind to port 8080." << std::endl;
		std::exit(EXIT_FAILURE);
	}
	// listening to the assigned socket
    if (listen(serverSocket, 5) < 0) //change 5 to however much we need
	{
		std::cout << "Failed to listen on socket." << std::endl;
		std::exit(EXIT_FAILURE);
	}
	
	unsigned long addrlen = sizeof(sockAddr);
	//need to study accept. seems weird
    // int clientSocket = accept(serverSocket, 0, 0); //if -1 error happened
	int clientSocket = accept(serverSocket, (struct sockaddr*)&sockAddr, (socklen_t*)&addrlen);
	if (clientSocket < 0) {
		std::cout << "Failed to grab connection." << std::endl;
		std::exit(EXIT_FAILURE);
	}


	/* this part sucks */
	// char buffer[1024] = { 0 };
	// recv(clientSocket, buffer, sizeof(buffer), 0);
	// std::cout << "Message from client: " << buffer << std::endl;

	//	Step 4 ->> send and receive messages ( Gael and Julien <3 )
	char buffer[1024] = {0};
	
	int valread = read(clientSocket , buffer, 1024); 
	std::cout << buffer << std::endl;
	if(valread < 0)
		std::cout << "No bytes are there to read" << std::endl;
	
	char *hello = "Hello from the server";//IMPORTANT! WE WILL GET TO IT
	write(clientSocket , hello , strlen(hello));

	close(clientSocket);
	close(serverSocket);

	return (0);
}