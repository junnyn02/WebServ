#include "Config.hpp"
#include "Server.hpp"

Config::Config(const std::string &filename) : _body_size(1)
{
	std::ifstream   infile(filename.c_str());
	if (!infile.is_open())
		throw (std::runtime_error("Can't open file"));
	std::string line;
	while (getline(infile, line))
        _file.append(line + '\n');
	infile.close();
	findHTTP();
	findRule();
	// findBodySize();
	std::cout << BOLD GREEN "[GENERAL BODY SIZE]: " RESET << _body_size << std::endl;
	// findErrorPage();
	// std::cout << BOLD GREEN "[FIND SERVER]: " RESET << std::endl;
	// findServer();
}

Config::~Config(void)
{
	for (size_t i = 0; i < _server.size(); i++)
		delete _server[i];
}

void Config::findHTTP(void)
{
	size_t	found = _file.find("http");
	if (found == std::string::npos || !checkComment(_context, found))
		throw (std::runtime_error("No HTTP context found"));
	std::string::iterator it = _file.begin() + found + strlen("http");
	while (it != _context.end() && isspace(*it))
		++it;
	if (it == _context.end() || *it != '{')
		throw (std::runtime_error("Syntax (bracket) Error"));
	std::string parsed(it, checkEnd(it));
	_context = parsed;
}

void	Config::findErrorPage(void)
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

void	Config::splitError(const std::string &key, const std::string &value)
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

void	Config::findServer(void)
{
	size_t	found = 0;
	while (1)
	{
		found = _context.find("server", found);
		if (found != std::string::npos && checkComment(_context, found))
		{
			std::string::iterator it = _context.begin() + found + strlen("server");
			while (it != _context.end() && isspace(*it))
				++it;
			if (*it == '{' && checkComment(_context, std::distance(_context.begin(), it)))
			{
				std::string parsed(it, checkEnd(it));
				Server *server = new Server(parsed, _body_size);
				if (!server)
					throw(std::runtime_error("Gerer erreur"));
				_server.push_back(server);
			}
		}
		if (found == std::string::npos)
			break ;
		found += strlen("server");
	}
	if (_server.size() == 0)
		throw(std::runtime_error("No Server Context found"));
}

void	Config::findRule()
{
	// std::string _context;
	std::string::iterator it = _context.begin();
	while (it != _context.end())
	{
		while (it != _context.end() && isspace(*it))
			++it;
		if (*it == '#')
		{
			// std::cout << *it << *(it + 1) << *(it + 2) << std::endl;
			while (it != _context.end() && *it != '\n')
				++it;
			if (it != _context.end() && *it == '\n')
				++it;
			// std::cout << *(it - 2) << *(it - 1) << *it << std::endl;
		}
		else if (_context.compare(std::distance(_context.begin(), it), strlen("server"), "server") == 0)
		{
			it += strlen("server");
			while (it != _context.end() && isspace(*it))
				++it;
			if (it == _context.end() || *it != '{')
				throw (std::runtime_error("Syntax (bracket) Error"));
			std::cout << *it << *(it + 1) << *(it + 2) << *(it + 3) << std::endl;
			it = checkEnd(it);
			std::cout << *(it - 5) << *(it - 4) << *(it - 3) << *(it - 2) << *(it - 1) << *it << std::endl;
		}
		else
		{
			if (_context.compare(std::distance(_context.begin(), it), strlen("client_max_body_size"), "client_max_body_size") == 0)
				findBodySize(it);
			// findErrorPage();
			++it;
		}
	}
}

void	Config::findBodySize(std::string::iterator &it)
{
	// size_t	found = _context.find("client_max_body_size");
	// if (found == std::string::npos || !checkComment(_context, found))
	// 	return ;
	// std::string::iterator it = _context.begin() + found + strlen("client_max_body_size");
	// std::advance(it, strlen("client_max_body_size"));
	std::cout << RED "BEGIN FIND BODY SIZE: " RESET << *it << std::endl;
	it += strlen("client_max_body_size");
	while (isspace(*it))
		++it;
	std::string::iterator ite = it;
	while (ite != _context.end() && isdigit(*ite))
		++ite;
	std::string parsed(it, ite);
	double d = strtod(parsed.c_str(), NULL);
	if (errno == ERANGE && d < std::numeric_limits<double>::min())
		throw (std::runtime_error("Error: Underflow"));
	else if (errno == ERANGE && d > std::numeric_limits<double>::max())
		throw (std::runtime_error("Error: Overflow"));
	else if (d > 20000 || d <= 0)
		throw (std::runtime_error("Client Max Body Size Error"));
	_body_size = (int)d;
	std::cout << RED "END FIND BODY SIZE: " RESET << *it << std::endl;
}

std::string::iterator	Config::checkEnd(std::string::iterator &it)
{
	std::string::iterator	tmp = it + 1;
	std::string::iterator	last;
	int count = 0;
	while (tmp != _file.end())
	{
		//check si les brackets sont commentes ?
		if (*tmp == '{' && checkComment(_file, std::distance(_file.begin(), tmp)))
			count += 1;
		if (*tmp == '}' && checkComment(_file, std::distance(_file.begin(), tmp)) && count > 0)
			count -= 1;
		else if (*tmp == '}' && checkComment(_file, std::distance(_file.begin(), tmp)) && count == 0)
		{
			last = tmp + 1;
			std::cout << "LAST: " << *(last - 2) << *(last - 1) << *last << std::endl;
			return last;
		}
		++tmp;
	}
	if (count != 0)
		throw (std::runtime_error("Syntax (bracket) Error"));
	return last;
}

const std::vector<Server*>	&Config::getServer(void) const
{
	return (this->_server);
}

const int	&Config::getBodySize(void) const
{
	return (this->_body_size);
}

bool	Config::checkComment(std::string &str, const size_t &found)
{
	std::string::reverse_iterator rit = str.rend() - found;
	while (rit != str.rend() && isspace(*rit))
		++rit;
	if (*rit == '#')
		return false;
	return true;
}