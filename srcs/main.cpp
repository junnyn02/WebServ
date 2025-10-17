#include "serverCore.hpp"
#include "Request.hpp"
#include "Config.hpp"
#include "Server.hpp"
#include "ResponseBuilder.hpp"
#include "Location.hpp"

// Signal handler function
void signalHandler(int sig) 
{
	(void)sig;
	throw (std::runtime_error("\nServer closed manually."));
}

int	main(int ac, char** av)
{
	// Handle signal
    signal(SIGINT, signalHandler);
    signal(SIGPIPE, SIG_IGN);

	if (ac > 2)
	{
		std::cerr << "Too many arguments./nFormat : ./webserv [config file](optional)]" << std::endl;
		return (1);
	}
	try
	{
		Config	conf(av[1]);
		std::cout << BOLD GREEN"[PARSING DONE]" RESET << std::endl;
		std::vector<Config*> servers = conf.getServer();
		serverCore serv(servers);
		std::cout << BOLD GREEN"[SERVER STARTED SUCCESSFULLY]" RESET << std::endl;

		struct epoll_event events[MAX_EVENTS];
		while (1)
		{
			int n_events = epoll_wait(serv._epoll_fd, events, MAX_EVENTS, 5000); // SET TIMEOUT (instead of -1)
			if (n_events == -1)
				throw (std::runtime_error("\nEpoll error: epoll_wait failed."));

			for (int i = 0; i < n_events; i++) 
			{
				int client = events[i].data.fd;
				// Event handling logic
				if (serv.findServer(client)) // client is one of the server's fd -> new connection
					serv.acceptNewClients(client);
				else if (events[i].events & EPOLLIN)// Data received from an existing client
					serv.receiveRequest(client);
				else if (events[i].events & EPOLLOUT)
				{
					// serv._clients[client].request.printRequest();
					if (!serv._clients[client].sendingResponse)
					{
						ResponseBuilder	response(serv._clients[client].request);
						std::string	resp = response.getHeader();
						serv.setResponse(client, resp, resp.size());
						serv._clients[client].sendingResponse = true;
					}
					serv.sendResponse(client);
				}
			}
		}

		return (0);
	}
	catch(const std::exception& e)
	{
		std::cerr << e.what() << '\n';
		return (1);
	}

	return (0);
}