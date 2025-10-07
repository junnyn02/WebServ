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
	// findServor;
}

void Config::findHTTP(void)
{
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
	checkEnd(it);
}

void	Config::checkEnd(std::string::iterator &it)
{
	std::string::iterator ite = _file.end();
	std::string::iterator tmp = it;
	int	count = 0;

	while (tmp != ite)
	{
		if (*tmp == '{')
			count += 1;
		++tmp;
	}
	tmp = it;
	while (tmp != ite)
	{
		if (*tmp == '}')
			count -= 1;
		++tmp;
	}
	if (count != 0)
		throw(std::runtime_error("Syntax (bracket) Error"));
}
