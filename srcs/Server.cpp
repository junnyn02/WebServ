#include "Server.hpp"

Server::Server(const std::string &context) : _context(context)
{
	parseInfo(context);
}

void	Server::parseInfo(const std::string &context)
{
	size_t	found = _context.find("listen");
	if (found == std::string::npos || !checkComment(_context, found))
		throw(std::runtime_error("Port not mapped"));
	std::string::const_iterator it = context.begin() + found + strlen("listen");
	findPort(it);
}

void	Server::findPort(std::string::const_iterator &it)
{
	while (it != _context.end() && isspace(*it))
		++it;
	if (!isdigit(*it) || *it == '-')
		throw (std::runtime_error("No port mapped or wrong format"));
	std::string::const_iterator ite = it;
	while (ite != _context.end() && isdigit(*ite))
		++ite;
	std::string to_convert(it, ite);
	int i = atoi(to_convert.c_str());
	if (i > 65535)
		throw (std::runtime_error("Impossible port"));
	_port = i;
	std::cout << _port << std::endl;
}