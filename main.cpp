#include "serverCore.hpp"

int	main(int, char**)
{
	serverCore serv;

	serv.startServer();

	while (1)
	{
		clientData data = serv.receiveRequest();
		std::string hello("HTTP/1.1 200 OK\nContent-Type: text/plain\nContent-Length: 12\n\nHello world!");
		serv.sendResponse(data, hello);
	}
	return (0);
}