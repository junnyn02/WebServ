#include "Server.hpp"

Server::Server(const std::string &context) : _context(context)
{
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
	findArgs("index");
	findErrorPage();
	for (std::map<std::string, std::string>::iterator it = _parsed.begin(); it != _parsed.end(); ++it)
		std::cout << it->first << " = " << it->second << std::endl;
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

void	Server::findErrorPage(void)
{
	std::map<int, std::string>	error;
	size_t	found = 0;
	while (1)
	{
		found = _context.find("error_page", found);
		if (found != std::string::npos && checkComment(_context, found))
		{
			std::string::iterator it = _context.begin() + found + strlen("error_page");
			while (it != _context.end() && isspace(*it))
				++it;
			std::string::iterator ite = it;
			while (ite != _context.end() && (isdigit(*ite) || isspace(*ite)))
				++ite;
			std::string key(it, ite);
			// splitError(key, );
			it = ite;
			while (it != _context.end() && *it != ';')
				++it;
			std::string value(ite, it);
			splitError(key, value);
		}
		if (found == std::string::npos)
			break ;
		found += strlen("error_page");
	}
	for (std::map<int, std::string>::iterator it = _error_page.begin(); it != _error_page.end(); ++it)
		std::cout << it->first << " = " << it->second << std::endl;
}

void	Server::splitError(const std::string &key, const std::string &value)
{
	std::string::const_iterator it = key.begin();
	while (it != key.end())
	{
		while (it != key.end() && isspace(*it))
			++it;
		if (it == key.end())
			return ;
		std::string::const_iterator ite = it;
		while (ite != key.end() && !isspace(*ite))
			++ite;
		std::string code(it, ite);
		double d = strtod(code.c_str(), NULL);
		if ((d == errno && d > std::numeric_limits<double>::max()) || d > std::numeric_limits<int>::max() || d < 0)
			throw (std::runtime_error("Code error"));
		_error_page.insert(std::pair<int, std::string>(d, value));
		it = ite;
	}
}

int	Server::getPort(void) const
{
	return (this->_port);
}