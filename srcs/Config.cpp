#include "Config.hpp"

Config::Config(const std::string &filename)
{
	std::ifstream   infile(filename.c_str());
	if (!infile.is_open())
		throw (std::runtime_error("Can't open file"));
	std::string line;
	while (getline(infile, line))
        _file.append(line + '\n');
	infile.close();
	// std::cout << _file << std::endl;
	findHTTP();
	// std::cout << _context << std::endl;
	findServer();
	findBodySize();
	std::cout << _body_size << std::endl;
}

void Config::findHTTP(void)
{
	size_t	found = _file.find("http");
	if (found == std::string::npos || !checkComment(_context, found))
		throw (std::runtime_error("No HTTP context found"));
	std::string::iterator it = _file.begin() + found + strlen("http");
	while (isspace(*it))
		++it;
	if (*it != '{')
		throw (std::runtime_error("Syntax (bracket) Error"));
	std::string parsed(it, checkEnd(it));
	_context = parsed;
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
			if (*it == '{')
			{
				std::string parsed(it, checkEnd(it));
				Server server(parsed);
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


//si c'est dans bloc server ? a gerer ou non ?
void	Config::findBodySize(void)
{
	size_t	found = _context.find("client_max_body_size");
	if (found == std::string::npos || !checkComment(_context, found))
	{
		_body_size = 1;
		return ;
	}
	std::string::iterator it = _context.begin() + found + strlen("client_max_body_size");
	while (isspace(*it))
		++it;
	std::string::iterator ite = it;
	while (ite != _context.end() && isdigit(*ite))
		++ite;
	std::string parsed(it, ite);
	double d = strtod(parsed.c_str(), NULL);
	if (d > std::numeric_limits<int>::max() || d <= 0)
		throw (std::runtime_error("Client Max Body Size Error"));
	_body_size = (int)d;
}

std::string::iterator	Config::checkEnd(std::string::iterator &it)
{
	std::string::iterator	tmp = it + 1;
	std::string::iterator	last;
	int count = 0;
	while (tmp != _file.end())
	{
		//check si les brackets sont commentes ?
		if (*tmp == '{')
			count += 1;
		if (*tmp == '}' && count > 0)
			count -= 1;
		else if (*tmp == '}' && count == 0)
		{
			last = tmp + 1;
			return last;
		}
		++tmp;
	}
	if (count != 0)
		throw (std::runtime_error("Syntax (bracket) Error"));
	return last;
}

bool	checkComment(std::string &str, const size_t &found)
{
	std::string::reverse_iterator rit = str.rend() - found;
	while (rit != str.rend() && isspace(*rit))
		++rit;
	if (*rit == '#')
		return false;
	return true;
}

const std::vector<Server>	&Config::getServer(void) const
{
	return (this->_server);
}