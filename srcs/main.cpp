#include "serverCore.hpp"

int	main(int, char**)
{
	serverCore serv;

	serv.startServer();

	while (1)
	{
		clientData data = serv.receiveRequest();
		std::string hello("HTTP/1.1 200 OK\nContent-Type: text/plain\nContent-Length: 12\n\nHello world!");
		for (size_t i = 0; i < hello.length(); i++)
			data.buffer[i] = hello.c_str()[i];
		for (size_t i = hello.length(); i < BUFFER_SIZE; i++)
			data.buffer[i] = 0;
		data.size = hello.length();
		serv.sendResponse(data);
	}
	return (0);
}