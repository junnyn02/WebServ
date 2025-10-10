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
			while (isspace(*it))
				++it;
			if (*it == '{')
			{
				std::string parsed(it, checkEnd(it));
				Server server(parsed);
				_server.push_back(server);
			}
			// if (*it != '{')
			// 	// continue;
			// 	throw (std::runtime_error("Syntax serv(bracket) Error"));
			// std::string parsed(it, checkEnd(it));
			// Server server(parsed);
			// _server.push_back(server);
		}
		if (found == std::string::npos)
			break ;
		found += strlen("server");
	}
	if (_server.size() == 0)
		throw(std::runtime_error("No Server Context found"));
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
// size_t	Config::checkEnd(std::string::iterator &it)
// {
// 	std::string::iterator ite = _file.end();
// 	std::string::iterator tmp = it;
// 	int	count = 0;

// 	while (tmp != ite)
// 	{
// 		if (*tmp == '{')
// 			count += 1;
// 		++tmp;
// 	}
// 	tmp = it;
// 	while (tmp != ite)
// 	{
// 		if (*tmp == '}')
// 			count -= 1;
// 		++tmp;
// 	}
// 	if (count != 0)
// 		throw(std::runtime_error("Syntax (bracket) Error"));
// }
