#pragma once

#include "utils.hpp"

class Config
{
    private:
		std::string							_file;
    	std::map<std::string, std::string>	_parsed;

    public:
        Config(const std::string &);
        ~Config(void){};

		void	findHTTP(void);
		void	checkEnd(std::string::iterator &);
};