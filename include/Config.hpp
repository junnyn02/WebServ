#pragma once

#include "utils.hpp"
// #include "Server.hpp"

class Server;

class Config
{
	private:
		void	findHTTP(void);

		const std::string findUri(std::string::iterator &) const;

    protected:
		int									_body_size;
		std::string							_file;
		std::string							_context;
		std::vector<Config*>				_child;
		std::map<int, std::string>			_error_page;
		std::map<std::string, std::string>	_parsed;

		Config(void) {};

		void	parseInfo(const std::string &);
		void	findChild(const std::string &);
		void	findBodySize(std::string::iterator &);
		void	findErrorPage(std::string::iterator &);
		void	splitError(const std::string &, const std::string &);
		void	findArgs(std::string::iterator &, const std::string &);

		bool	checkComment(std::string &str, const size_t &found);
		
		std::string::iterator	checkEnd(std::string::iterator &);

	public:
		Config(const std::string &);
        virtual ~Config(void);

		const int							&getBodySize(void) const;
		const std::vector<Config*>			&getServer(void) const;
		const std::map<int, std::string>	&getError(void) const;
		const std::map<std::string, std::string>	&getInfo(void) const;
};
