#pragma once

#include "utils.hpp"
// #include "Server.hpp"

class Server;

class Config
{
	private:
		std::vector<Server*>			_server;

		void	findHTTP(void);
		void	findServer(void);

    protected:
		int							_body_size;
		std::string					_file;
		std::string					_context;
		std::map<int, std::string>	_error_page;
		Config(void) {};

    public:
        Config(const std::string &);
        virtual ~Config(void);

		void	findBodySize(std::string::iterator &);
		void	findErrorPage(void);
		void	splitError(const std::string &, const std::string &);

		bool					checkComment(std::string &str, const size_t &found);
		std::string::iterator	checkEnd(std::string::iterator &);

		void	findRule(void);

		const int					&getBodySize(void) const;
		const std::vector<Server*>	&getServer(void) const;


};
