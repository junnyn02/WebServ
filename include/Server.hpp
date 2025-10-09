#pragma once

#include "utils.hpp"
#include "Config.hpp"

class Server
{
	private:
		std::map<std::string, std::string>	_parsed;
		int									_port;
		std::string							_context;
		
	public:
		Server(const std::string &);
		~Server(void) {};

		void	parseInfo(const std::string &);
		void	findPort(std::string::const_iterator &);
		void	findArgs(std::string::const_iterator &, const std::string &);
};

bool	checkComment(std::string &str, const size_t &found);