#include "serverCore.hpp"
#include "Request.hpp"
#include "Config.hpp"
#include "ResponseBuilder.hpp"

// Signal handler function
void signalHandler(int sig) 
{
    std::cout << "\nServer closed manually." << std::endl;
    exit(sig);
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

	// only if config file provided, else : should use default config file (or default config hard coded ?)
	try
	{
		Config	conf(av[1]);
		std::cout << BOLD GREEN"[PARSING DONE]" RESET << std::endl;
		std::vector<Server> servers = conf.getServer();
		std::cout << BOLD GREEN"[CONFIG PORT]: " RESET << std::endl;
		for (size_t i = 0; i < servers.size(); i++)
			std::cout << "\t" << servers[i].getPort() << std::endl;
		
		serverCore serv(servers);
		return (0);
	}
	catch(const std::exception& e)
	{
		std::cerr << e.what() << '\n';
		return (1);
	}
	
	// serverCore serv;
	// serv.startServer();

	// struct epoll_event events[MAX_EVENTS];

	// while (1)
	// {
	// 	int n_events = epoll_wait(serv._epoll_fd, events, MAX_EVENTS, 5000); // SET TIMEOUT (instead of -1)
	// 	if (n_events == -1)
	// 	{
	// 		std::cerr << "Error: epoll_wait" << std::endl;
	// 		break;
	// 	}

	// 	for (int i = 0; i < n_events; i++) 
	// 	{
	// 		int client = events[i].data.fd;
	// 		// Event handling logic
	// 		if (client == serv.getfd()) // New client connection
	// 			serv.acceptNewClients();
	// 		else if (events[i].events & EPOLLIN)// Data received from an existing client
	// 		{
	// 			// int status = 
	// 			serv.receiveRequest(client); 
	
	// 			// if (status < 0)
	// 			// {
	// 			// 	std::cerr << "Error : request not received\n";
	// 			// 	serv._clients.erase(client);
	// 			// }
	// 			/* else status == 0 -> request incomplete still receiving */
	// 		}
	// 		else if (events[i].events & EPOLLOUT)// Data received from an existing client
	// 		{
	// 			// serv._clients[client].request.printRequest();
	// 			if (!serv._clients[client].sendingResponse)
	// 			{
	// 				ResponseBuilder	response(serv._clients[client].request);
	// 				std::string	resp = response.getHeader();
	// 				serv.setResponse(client, resp, resp.size());
	// 				serv._clients[client].sendingResponse = true;
	// 			}
	// 			serv.sendResponse(client);
	// 		}
	// 	}
	// }
	return (0);
}