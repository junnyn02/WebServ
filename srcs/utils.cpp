#include "utils.hpp"

std::string size_t_to_string(size_t n) 
{
    std::ostringstream oss;
    oss << n;
    return oss.str();
}

void printConfig(const std::map<std::string, std::string>& config)
{
	std::map<std::string, std::string>::const_iterator it = config.begin();
	while (it != config.end())
	{
		std::cout << it->first << " : " << it->second << std::endl;
		it++;
	}
}