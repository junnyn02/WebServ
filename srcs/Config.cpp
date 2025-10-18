#include "Config.hpp"
#include "Server.hpp"
#include "Location.hpp"

Config::Config(const std::string &filename) : _body_size(1)//, _autoindex(false)
{
	std::ifstream   infile(filename.c_str());
	if (!infile.is_open())
		throw (std::runtime_error("Can't open file"));
	std::string line;
	while (getline(infile, line))
        _file.append(line + '\n');
	infile.close();
	findHTTP();
	_parsed.insert(std::pair<std::string, std::string>("autoindex", "off"));
	// std::cout << BOLD GREEN "[HTTP BLOC]: " RESET << _context << std::endl;
	parseInfo("server");
	// std::cout << BOLD GREEN "[GENERAL BODY SIZE]: " RESET << _body_size << std::endl;
	// std::cout << BOLD GREEN "[GENERAL ERROR PAGE]: " RESET;
	// for (std::map<int, std::string>::iterator it = _error_page.begin(); it != _error_page.end(); ++it)
		// std::cout << it->first << " = " << it->second << std::endl;
	// std::cout << BOLD GREEN "[FIND SERVER]: " RESET << std::endl;
	findChild("server");
}

Config::~Config(void)
{
	for (size_t i = 0; i < _child.size(); i++)
		delete _child[i];
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
	// std::cout << *checkEnd(it) << std::endl;
	std::string parsed(it, checkEnd(it));
	_context = parsed;
}

void	Config::parseInfo(const std::string &child)
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
		else if (_context.compare(std::distance(_context.begin(), it), child.size(), child) == 0)
		{
			it += child.size();
			// while (it != _context.end() && isspace(*it))
			while (it != _context.end() && *it != '{' && *it != '\n')
				++it;
			if (it == _context.end() || *it != '{')
				throw (std::runtime_error("Syntax (bracket) Error"));
			it = checkEnd(it);
		}
		else
		{
			if (_context.compare(std::distance(_context.begin(), it), strlen("client_max_body_size"), "client_max_body_size") == 0)
				findBodySize(it);
			else if (_context.compare(std::distance(_context.begin(), it), strlen("error_page"), "error_page") == 0)
				findErrorPage(it);
			else if (_context.compare(std::distance(_context.begin(), it), strlen("index"), "index") == 0)
				findArgs(it, "index");
			else if (_context.compare(std::distance(_context.begin(), it), strlen("autoindex"), "autoindex") == 0)
				findArgs(it, "autoindex");
			++it;
		}
	}
}

void	Config::findBodySize(std::string::iterator &it)
{
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
}

void	Config::findErrorPage(std::string::iterator &it)
{
	it += strlen("error_page");
	while (it != _context.end() && isspace(*it))
		++it;
	if (it == _context.end())
		throw(std::runtime_error("Syntax Error"));
	std::string::iterator ite = it;
	while (ite != _context.end() && (isdigit(*ite) || isspace(*ite)))
		++ite;
	if (it == _context.end())
		throw(std::runtime_error("Syntax Error"));
	std::string	key(it, ite - 1);
	it = ite;
	while (ite != _context.end() && *ite != ';' && *ite != '\n')
		++ite;
	std::string	value(it, ite);
	splitError(key, value);
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

void	Config::findArgs(std::string::iterator &it, const std::string &key)
{
	it += key.size();
	while (it != _context.end() && isspace(*it))
		++it;
	if (it == _context.end())
		throw(std::runtime_error("Syntax Error"));
	std::string::iterator ite = it;
	while (it != _context.end() && *it != ';' && *it != '\n')
		++it;
	std::string	arg(ite, it);
	std::map<std::string, std::string>::iterator found = _parsed.find(key);
	if (found != _parsed.end())
		found->second = arg;
	else
		_parsed.insert(std::pair<std::string, std::string>(key, arg));
}

void	Config::findChild(const std::string &child)
{
	size_t	found = 0;
	while (1)
	{
		found = _context.find(child, found);
		if (found != std::string::npos && checkComment(_context, found))
		{
			std::string::iterator it = _context.begin() + found + child.size();
			std::string	uri;
			if (child == "location")
				uri = findUri(it);
			while (it != _context.end() && *it != '{' && *it != '\n')
				++it;
			if (*it == '{' && checkComment(_context, std::distance(_context.begin(), it)))
			{
				std::string parsed(it, checkEnd(it));
				if (child == "server")
				{
					Server *server = new Server(parsed, _body_size, _error_page, _parsed);
					if (!server)
						throw(std::runtime_error("Gerer erreur"));
					_child.push_back(server);					
				}
				else
				{
					Location *location = new Location(uri, parsed, _body_size, _error_page, _parsed);
					if (!location)
						throw(std::runtime_error("Gerer erreur"));
					_child.push_back(location);
					// std::cout << "* DO LOCATION *" << std::endl;
				}
			}
		}
		if (found == std::string::npos)
			break ;
		found += child.size();
	}
	if (_child.size() == 0 && child == "server")
		throw(std::runtime_error("No Server Context found"));
}

const std::string	Config::findUri(std::string::iterator &it) const
{
	while (it != _context.end() && isspace(*it))
		++it;
	std::string::iterator	ite = it;
	while (ite != _context.end() && *ite != '{')
		++ite;
	if (*ite == '{')
		--ite;
	while (isspace(*ite) && ite != it)
		--ite;
	if (ite != _context.end())
		++ite;
	std::string	uri(it, ite);
	return (uri);
}

std::string::iterator	Config::checkEnd(std::string::iterator &it)
{
	std::string::iterator	tmp = it + 1;
	std::string::iterator	last;
	int count = 0;
	while (tmp != _file.end())
	{
		//check si les brackets sont commentes ?
		if (*tmp == '#' || (*tmp == '/' && *(tmp + 1) == '/'))
		{
			while (tmp != _file.end() && *tmp != '\n')
				++tmp;
			if (tmp == _file.end())
				throw (std::runtime_error("Syntax (bracket Error)"));
		}
		if (*tmp == '{')
			count += 1;
		else if (*tmp == '}' && count > 0)
			count -= 1;
		else if (*tmp == '}' && count == 0)
		{
			last = tmp + 1;
			return last;
		}
		++tmp;
	}
	//std::cout << count << std::endl;
	if (count != 0 || tmp == _file.end())
		throw (std::runtime_error("Syntax (bracket) Error"));
	return last;
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

const std::vector<Config*>	&Config::getChild(void) const
{
	return (this->_child);
}

const int	&Config::getBodySize(void) const
{
	return (this->_body_size);
}

const std::map<int, std::string>	&Config::getError(void) const
{
	return (this->_error_page);
}

const std::map<std::string, std::string>	&Config::getInfo(void) const
{
	return (this->_parsed);
}