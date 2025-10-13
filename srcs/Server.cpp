#include "Server.hpp"

Server::Server(const std::string &context, const int &body_size) : Config()
{
	this->_body_size = body_size;
	std::cout << CYAN "[INHERIT BODY SIZE]: " RESET << this->_body_size << std::endl;
	this->_context = context;
	parseInfo();
}

void	Server::parseInfo(void)
{
	size_t	found = _context.find("listen");
	if (found == std::string::npos || !checkComment(_context, found))
		throw(std::runtime_error("Port not mapped"));
	std::string::const_iterator it = _context.begin() + found + strlen("listen");
	findPort(it);
	findArgs("root");
	findArgs("server_name");
	std::map<std::string, std::string>::iterator it_find = _parsed.find("server_name");
	if (it_find == _parsed.end())
	{
		std::stringstream port;
		port << _port;
		std::string	name = "localhost:" + port.str();
		_parsed.insert(std::pair<std::string, std::string>("server_name", name));
	}
	findArgs("index");
	findErrorPage();
	// findBodySize();
	for (std::map<std::string, std::string>::iterator it = _parsed.begin(); it != _parsed.end(); ++it)
		std::cout << it->first << " = " << it->second << std::endl;
	std::cout << GREEN "[PORT]: " RESET << _port << std:: endl;
	std::cout << GREEN "[BODY SIZE]: " RESET << _body_size << std:: endl;
}

void	Server::findPort(std::string::const_iterator &it)
{
	while (it != _context.end() && isspace(*it))
		++it;
	if (it == _context.end() || !isdigit(*it) || *it == '-')
		throw (std::runtime_error("No port mapped or wrong format"));
	std::string::const_iterator ite = it;
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

// void	Server::findBodySize(void)
// {
// 	size_t	found = _context.find("client_max_body_size");
// 	if (found == std::string::npos || !checkComment(_context, found))
// 		return ;
// 	std::string::const_iterator it = _context.begin() + found + strlen("client_max_body_size");
// 	while (it != _context.end() && isspace(*it))
// 		++it;
// 	if (it == _context.end() || !isdigit(*it) || *it == '-')
// 		throw (std::runtime_error("Client Max Body Size Syntax Error"));
// 	std::string::const_iterator ite = it;
// 	while (ite != _context.end() && *ite != ';')
// 		++ite;
// 	std::string	arg(it, ite);
// 	double d = strtod(arg.c_str(), NULL);
// 	if (errno == ERANGE && d < std::numeric_limits<double>::min())
// 		throw (std::runtime_error("Error: Underflow"));
// 	else if (errno == ERANGE && d > std::numeric_limits<double>::max())
// 		throw (std::runtime_error("Error: Overflow"));
// 	else if (d > 20000 || d <= 0)
// 		throw (std::runtime_error("Client Max Body Size Error"));
// 	_body_size = (int)d;
// }

void	Server::findArgs(const std::string &key)
{
	size_t	found = _context.find(key);
	if (found == std::string::npos || !checkComment(_context, found))
		return ;
	std::string::const_iterator it = _context.begin() + found + key.size();
	while (it != _context.end() && isspace(*it))
		++it;
	std::string::const_iterator ite = it;
	while (ite != _context.end() && *ite != ';')
		++ite;
	std::string	arg(it, ite);
	_parsed.insert(std::pair<std::string, std::string>(key, arg));
}

// void	Server::findErrorPage(void)
// {
// 	std::map<int, std::string>	error;
// 	size_t	found = 0;
// 	while (1)
// 	{
// 		found = _context.find("error_page", found);
// 		if (found != std::string::npos && checkComment(_context, found))
// 		{
// 			std::string::iterator it = _context.begin() + found + strlen("error_page");
// 			while (it != _context.end() && isspace(*it))
// 				++it;
// 			std::string::iterator ite = it;
// 			while (ite != _context.end() && (isdigit(*ite) || isspace(*ite)))
// 				++ite;
// 			std::string key(it, ite);
// 			// splitError(key, );
// 			it = ite;
// 			while (it != _context.end() && *it != ';')
// 				++it;
// 			std::string value(ite, it);
// 			splitError(key, value);
// 		}
// 		if (found == std::string::npos)
// 			break ;
// 		found += strlen("error_page");
// 	}
// 	for (std::map<int, std::string>::iterator it = _error_page.begin(); it != _error_page.end(); ++it)
// 		std::cout << it->first << " = " << it->second << std::endl;
// }

// void	Server::splitError(const std::string &key, const std::string &value)
// {
// 	std::string::const_iterator it = key.begin();
// 	while (it != key.end())
// 	{
// 		while (it != key.end() && isspace(*it))
// 			++it;
// 		if (it == key.end())
// 			return ;
// 		std::string::const_iterator ite = it;
// 		while (ite != key.end() && !isspace(*ite))
// 			++ite;
// 		std::string code(it, ite);
// 		double d = strtod(code.c_str(), NULL);
// 		if ((d == errno && d > std::numeric_limits<double>::max()) || d > std::numeric_limits<int>::max() || d < 0)
// 			throw (std::runtime_error("Code error"));
// 		_error_page.insert(std::pair<int, std::string>(d, value));
// 		it = ite;
// 	}
// }

int	Server::getPort(void) const
{
	return (this->_port);
}