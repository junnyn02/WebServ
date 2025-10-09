#include "serverCore.hpp"
#include "Request.hpp"
#include "Config.hpp"
#include "ResponseBuilder.hpp"

// Signal handler function
void signalHandler(int sig) {
    std::cout << "Interrupt handle " << sig << std::endl;

    exit(sig);
}

int	main(int ac, char** av)
{
	// Handle signal
    signal(SIGINT, signalHandler);
    signal(SIGPIPE, SIG_IGN);

	if (ac > 2)
	{
		std::cerr << "" << std::endl;
		return (1);
	}

	try
	{
		Config	conf(av[1]);
		std::cout << BOLD GREEN"[PARSING DONE]" RESET << std::endl;
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
			int client = events[i].data.fd;
			// Event handling logic
			if (client == serv.getfd()) // New client connection
				serv.acceptNewClients();
			else if (events[i].events & EPOLLIN)// Data received from an existing client
			{
				// int status = 
				serv.receiveRequest(client); 
	
				// if (status < 0)
				// {
				// 	std::cerr << "Error : request not received\n";
				// 	serv.discussions.erase(client);
				// }
				/* else status == 0 -> request incomplete still receiving */
			}
			else if (events[i].events & EPOLLOUT)// Data received from an existing client
			{
				// serv.discussions[client].request.printRequest();
				ResponseBuilder	response(serv.discussions[client].request);
			
				// std::string resp = response.sendResponse();
				// serv.setResponse(client, resp, resp.length());
				
				// serv.sendResponse(client);

				// serv.sendResponse(client, response.getHeader(), response.getBody());
			}
		}
	}
	return (0);
}