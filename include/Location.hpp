#pragma once

#include "utils.hpp"
#include "Config.hpp"

class Location : public Config
{
	private:
		std::string	_uri;
		
		void	parseLocation(void);
		
	public:
		Location(const std::string &, const std::string &, const int &, const std::map<int,
			std::string> &, const std::map<std::string, std::string> &);
		virtual ~Location(void) {};

		const std::string	&getUri(void) const;
};