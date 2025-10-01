#include "serverCore.hpp"
#include "Request.hpp"
#include "ResponseBuilder.hpp"

int	main(int, char**)
{
	serverCore serv;

	serv.startServer(); // should add argv[1] : config file (-> parsing)

	struct epoll_event events[MAX_EVENTS];

	while (1)
	{

		int n_events = epoll_wait(serv.epoll_fd, events, MAX_EVENTS, -1);
		if (n_events == -1)
		{
			std::cerr << "Error: epoll_wait" << std::endl;
			break;
		}

		for (int i = 0; i < n_events; i++) 
		{
			// Event handling logic
			if (events[i].data.fd == serv.getfd()) // New client connection
				serv.acceptNewClients();
			else // Data from an existing client
			{
				clientData data = serv.receiveRequest(events[i].data.fd); 

				Request fresh(data);
				ResponseBuilder	response(fresh);

				fresh.printRequest(); // remove later
			
				std::string hello = response.exec();

				for (size_t i = 0; i < hello.length(); i++)
					data.buffer[i] = hello.c_str()[i];
				for (size_t i = hello.length(); i < BUFFER_SIZE; i++)
					data.buffer[i] = 0;

				data.size = hello.length();

				serv.sendResponse(data);

				memset(&data, 0, sizeof(clientData));
			}
		}
	}
	return (0);
}