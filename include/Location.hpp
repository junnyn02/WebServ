#pragma once

#include "utils.hpp"
#include "Config.hpp"

class Location : public Config
{
	private:
		std::string					_uri;
		std::string					_modifier; //if '=' or ~ or none;
		std::vector<std::string>	_method;

		void	parseLocation(void);
		void	findLimit(std::string::iterator &);
	public:
		Location(const std::string &, const std::string &, const int &, const std::map<int,
			std::string> &, const std::map<std::string, std::string> &);
		virtual ~Location(void) {};

		const std::string				&getUri(void) const;
		const std::vector<std::string>	&getMethod(void) const;
};