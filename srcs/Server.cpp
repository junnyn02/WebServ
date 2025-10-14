#include "Server.hpp"

Server::Server(const std::string &context, const int &body_size, const std::map<int, std::string> &error_page, const std::map<std::string, std::string> &parsed) : Config()
{
	this->_context = context;
	this->_body_size = body_size;
	std::cout << CYAN "[INHERIT BODY SIZE]: " RESET << this->_body_size << std::endl;
	if (error_page.size() != 0)
		this->_error_page = error_page;
	std::cout << CYAN "[INHERIT ERROR PAGE]: " RESET;
	for (std::map<int, std::string>::iterator it = _error_page.begin(); it != _error_page.end(); ++it)
		std::cout << it->first << " = " << it->second << std::endl;
	this->_parsed = parsed;
	std::cout << CYAN "[INHERIT INFO]: " RESET;
	for (std::map<std::string, std::string>::iterator it = _parsed.begin(); it != _parsed.end(); ++it)
		std::cout << it->first << "=" << it->second << std::endl;
	parseInfo("location");
	std::cout << CYAN "[SERVER BODY SIZE]: " RESET << this->_body_size << std::endl;
	std::cout << CYAN "[SERVER ERROR PAGE]: " RESET;
	for (std::map<int, std::string>::iterator it = _error_page.begin(); it != _error_page.end(); ++it)
		std::cout << it->first << " = " << it->second << std::endl;
	parseServer();
	std::cout << CYAN "[SERVER INFO]: " RESET;
	for (std::map<std::string, std::string>::iterator it = _parsed.begin(); it != _parsed.end(); ++it)
		std::cout << it->first << " = " << it->second << std::endl;
	findChild("location");
}

void	Server::parseServer(void)
{
	std::string::iterator it = _context.begin();
	while (it != _context.end())
	{
		while (it != _context.end() && isspace(*it))
			++it;
		if (*it == '#')
		{
			while (it != _context.end() && *it != '\n')
				++it;
			if (it != _context.end() && *it == '\n')
				++it;
		}
		else if (_context.compare(std::distance(_context.begin(), it), strlen("location"), "location") == 0)
		{
			it += strlen("location");
			while (it != _context.end() && *it != '{' && *it != '\n')
				++it;
			if (it == _context.end() || *it != '{')
				throw (std::runtime_error("Syntax (bracket) Error"));
			it = checkEnd(it);
		}
		else
		{
			if (_context.compare(std::distance(_context.begin(), it), strlen("listen"), "listen") == 0)
				findPort(it);
			else if (_context.compare(std::distance(_context.begin(), it), strlen("server_name"), "server_name") == 0)
				findArgs(it, "server_name");
			++it;
		}
	}
	std::map<std::string, std::string>::iterator it_find = _parsed.find("server_name");
	if (it_find == _parsed.end())
	{
		std::stringstream port;
		port << _port;
		std::string	name = "localhost:" + port.str();
		_parsed.insert(std::pair<std::string, std::string>("server_name", name));
	}
	std::cout << CYAN "[PORT]: " RESET << _port << std:: endl;
}

void	Server::findPort(std::string::iterator &it)
{
	it += strlen("listen");
	while (it != _context.end() && isspace(*it))
		++it;
	if (it == _context.end() || !isdigit(*it) || *it == '-')
		throw (std::runtime_error("No port mapped or wrong format"));
	std::string::iterator ite = it;
	while (ite != _context.end() && isdigit(*ite))
		++ite;
	std::string to_convert(it, ite);
	double d = strtod(to_convert.c_str(), NULL);
	if (errno == ERANGE && d < std::numeric_limits<double>::min())
		throw (std::runtime_error("Error: Underflow"));
	else if (errno == ERANGE && d > std::numeric_limits<double>::max())
		throw (std::runtime_error("Error: Overflow"));
	else if (d > 65535 || d <= 0)
		throw (std::runtime_error("Client Max Body Size Error"));
	_port = (int)d;
}

const int	&Server::getPort(void) const
{
	return (this->_port);
}
