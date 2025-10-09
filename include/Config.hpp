#pragma once

#include "utils.hpp"
#include "Server.hpp"

class Server;

class Config
{
    private:
		// std::string::iterator	_http_begin;
		// std::string::iterator	_http_end;
		int					_body_size;
		std::string			_file;
		std::string			_context;
		std::vector<Server>	_server;

    public:
        Config(const std::string &);
        ~Config(void){};

		void	findHTTP(void);
		void	findServer(void);
		void	findBodySize(void);

		std::string::iterator	checkEnd(std::string::iterator &);
};

bool	checkComment(std::string &str, const size_t &found);