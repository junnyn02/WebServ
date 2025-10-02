#include "serverCore.hpp"
#include "Request.hpp"
#include "ResponseBuilder.hpp"

int	main(int ac, char** av)
{
	if (ac > 2)
	{
		std::cerr << "" << std::endl;
		return (1);
	}

	
	serverCore serv;
	ac == 2 ? serv.startServer() : serv.startServer(av[1]); // should add argv[1] : config file (-> parsing)

	struct epoll_event events[MAX_EVENTS];

	while (1)
	{

		int n_events = epoll_wait(serv.epoll_fd, events, MAX_EVENTS, -1); // SET TIMEOUT (instead of -1)
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
			
				data.body = response.sendResponse();
				std::cout << "[RECEIVE RESPONSE]" << std::endl;
				
				data.size = data.body.length();

				serv.sendResponse(data);
			}
		}
	}
	return (0);
}