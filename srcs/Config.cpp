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
	findServer();
}

void Config::findHTTP(void)
{
	std::map<size_t, size_t>	part;
	size_t	found = _file.find("http");
	if (found == std::string::npos)
		throw (std::runtime_error("No HTTP context find"));
	std::string::reverse_iterator rit = _file.rend() - found;
	while (rit != _file.rend() && isspace(*rit))
		++rit;
	if (*rit == '#')
		throw (std::runtime_error("No HTTP context find"));
	std::string::iterator it = _file.begin() + found + 4;
	while (isspace(*it))
		++it;
	if (*it != '{')
		throw (std::runtime_error("Syntax (bracket) Error"));
	size_t	first = std::distance(_file.begin(), it);
	size_t	last = std::distance(_file.begin(), checkEnd(it));
	part.insert(std::pair<size_t, size_t>(first, last));
	// _context.insert(std::pair<std::string, std::map<std::string::iterator, std::string::iterator> >("http", part));
}

void	Config::findServer(void)
{
	// std::map<size_t, size_t>	part;
	// size_t	found = _file.find("server");
	// if (found == std::string::npos)
	// 	throw (std::runtime_error("No server context find"));
	// std::string::reverse_iterator rit = _file.rend() - found;
	// while (rit != _file.rend() && isspace(*rit))
	// 	++rit;
	// if (*rit == '#')
	// 	throw (std::runtime_error("No server context find"));
	// std::string::iterator it = _file.begin() + found + 4;
	// while (isspace(*it))
	// 	++it;
	// if (*it != '{')
	// 	throw (std::runtime_error("Syntax (bracket) Error"));
	// size_t	first = std::distance(_file.begin(), it);
	// size_t	last = std::distance(_file.begin(), checkEnd(it));
	// part.insert(std::pair<size_t, size_t>(first, last));
	// _context.insert(std::pair<std::string, std::map<size_t, size_t> >("server", part));
}

std::string::iterator	Config::checkEnd(std::string::iterator &it)
{
	std::string::iterator	tmp = it + 1;
	std::string::iterator	last;
	int count = 1;
	while (tmp != _file.end())
	{
		if (*tmp == '{')
			count += 1;
		if (*tmp == '}' && count > 0)
			count -= 1;
		else if (*tmp == '}' && count == 0)
		{
			last = tmp;
			return last;
		}
		++tmp;
	}
	if (count != 0)
		throw (std::runtime_error("Syntax (bracket) Error"));
	return last;
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
