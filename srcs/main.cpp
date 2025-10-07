#include "serverCore.hpp"
#include "Request.hpp"
#include "ResponseBuilder.hpp"
#include "Config.hpp"

int	main(int ac, char** av)
{
	if (ac > 2)
	{
		std::cerr << "" << std::endl;
		return (1);
	}

	/*
		NEED TO : manage signal so it destroys the server correctly on SIGINT
	*/
	try
	{
		Config	conf(av[1]);
	}
	catch(const std::exception& e)
	{
		std::cerr << e.what() << '\n';
		return (1);
	}
	

	serverCore serv;
	ac == 1 ? serv.startServer() : serv.startServer(av[1]); // should add argv[1] : config file (-> parsing)

	struct epoll_event events[MAX_EVENTS];
	// map with client fd / client data

	while (1)
	{
		int n_events = epoll_wait(serv.epoll_fd, events, MAX_EVENTS, 5000); // SET TIMEOUT (instead of -1)
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
			else // Data received from an existing client
			{
				int client = events[i].data.fd;
				// clientData data = 
				int status = serv.receiveRequest(client); 
				
				 if (status > 0)
				{
					// std::cout << "Trying to create Request clas for Client " << client << std::endl;
					// std::cout << "Body : " << serv.discussions[client].body << std::endl;
					// Request fresh(serv.discussions[client]);
					serv.discussions[client].request.printRequest();
					ResponseBuilder	response(serv.discussions[client].request);

					// fresh.printRequest(); // remove later
				
					// data.body = response.sendResponse();
					// data.size = data.body.length();
					// std::string header = response.getHeader();
					// std::vector<char> body = response.getBody();
					// serv.setResponse(client, header, header.size());
					// std::cout << "[RECEIVE RESPONSE]" << std::endl;
					serv.sendResponse(client, response.getHeader(), response.getBody());
				}
				else if (status < 0)
					serv.discussions.erase(client);
				/* else status == 0 -> request incomplete still receiving */
			}
		}
	}
	return (0);
}