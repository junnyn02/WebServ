#ifndef REQUEST_HPP
#define REQUEST_HPP

#include <iostream>
#include <vector>
#include <map>
#include <string>

class Request
{
	private:
		std::string							_method;
		std::string							_uri; //path?
		int									_port;
		std::map<std::string, std::string>	_headers;
		int									_status;

		
		Request(const Request&);
		Request &operator=(const Request&);
	public:
		Request();
		~Request();
		void fillRequest(const std::string&);
		void printRequest();

		std::string& getMethod();
		std::string& getURI();
		int getPort();
		int getStatus();
};

Request& createRequest(const std::string);

#endif