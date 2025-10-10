#pragma once

#include "utils.hpp"
#include "Config.hpp"

class Server
{
	private:
		int									_port;
		// int									_body_size;
		std::string							_context;
		std::map<int, std::string>			_error_page;
		std::map<std::string, std::string>	_parsed;
		
	public:
		Server(const std::string &);
		~Server(void) {};

		void	parseInfo(void);
		void	findPort(std::string::const_iterator &);
		void	findArgs(const std::string &);
		void	findErrorPage(void);
		void	splitError(const std::string &, const std::string &);

		int		getPort(void) const;
};

bool	checkComment(std::string &str, const size_t &found);