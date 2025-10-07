#pragma once

#include "utils.hpp"

class Config
{
    private:
		std::map<std::string, std::map<std::string::iterator, std::string::iterator> >	_context; //maybe w/ iterators pour parcourir seulement context http
		// std::map<std::string::iterator, std::string::iterator>					_part;
		std::string	_file;

    public:
        Config(const std::string &);
        ~Config(void){};

		void	findHTTP(void);
		void	findServer(void);

		std::string::iterator	checkEnd(std::string::iterator &);
};