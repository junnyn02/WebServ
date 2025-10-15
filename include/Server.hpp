#pragma once

#include "utils.hpp"
#include "Config.hpp"

class Server : public Config
{
	private:
		int		_port;

		void	parseServer(void);
		void	findPort(std::string::iterator &);
		
	public:
		Server(const std::string &, const int &, const std::map<int, std::string> &, const std::map<std::string, std::string> &);
		virtual ~Server(void) {};

		virtual int	getPort(void) const;
};