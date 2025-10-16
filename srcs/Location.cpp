#include "Location.hpp"

Location::Location(const std::string &uri, const std::string &context, const int &body_size,
	const std::map<int, std::string> &error_page, const std::map<std::string, std::string> &parsed) : Config(), _uri(uri)
{
	// std::cout << YELLOW "[LOCATION URI]: " RESET << "|" << this->_uri << "|" << std::endl;
	this->_context = context;
	this->_body_size = body_size;
	// std::cout << YELLOW "[INHERIT BODY SIZE]: " RESET << this->_body_size << std::endl;
	if (error_page.size() != 0)
		this->_error_page = error_page;
	// std::cout << YELLOW "[INHERIT ERROR PAGE]: " RESET;
	// for (std::map<int, std::string>::iterator it = _error_page.begin(); it != _error_page.end(); ++it)
		// std::cout << it->first << " = " << it->second << std::endl;
	this->_parsed = parsed;
	// std::cout << YELLOW "[INHERIT INFO]: " RESET;
	// for (std::map<std::string, std::string>::iterator it = _parsed.begin(); it != _parsed.end(); ++it)
		// std::cout << it->first << "=" << it->second << std::endl;
	parseInfo("location");
	// std::cout << YELLOW "[LOCATION BODY SIZE]: " RESET << this->_body_size << std::endl;
	// std::cout << YELLOW "[LOCATION ERROR PAGE]: " RESET;
	// for (std::map<int, std::string>::iterator it = _error_page.begin(); it != _error_page.end(); ++it)
		// std::cout << it->first << " = " << it->second << std::endl;
	parseLocation();
	// std::cout << YELLOW "[LOCATION INFO]: " RESET;
	// for (std::map<std::string, std::string>::iterator it = _parsed.begin(); it != _parsed.end(); ++it)
		// std::cout << it->first << " = " << it->second << std::endl;
}

void	Location::parseLocation(void)
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
		// else if (_context.compare(std::distance(_context.begin(), it), strlen("location"), "location") == 0)
		// {
		// 	it += strlen("location");
		// 	while (it != _context.end() && *it != '{' && *it != '\n')
		// 		++it;
		// 	if (it == _context.end() || *it != '{')
		// 		throw (std::runtime_error("Syntax (bracket) Error"));
		// 	it = checkEnd(it);
		// }
		else
		{
			if (_context.compare(std::distance(_context.begin(), it), strlen("alias"), "alias") == 0)
				findArgs(it, "alias");
			else if (_context.compare(std::distance(_context.begin(), it), strlen("rewrite"), "rewrite") == 0)
				findArgs(it, "rewrite");
			else if (_context.compare(std::distance(_context.begin(), it), strlen("return"), "return") == 0)
				findArgs(it, "return");
			++it;
		}
	}
}

const std::string	&Location::getUri(void) const
{
	return (this->_uri);
}