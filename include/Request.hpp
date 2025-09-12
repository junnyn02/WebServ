#ifndef REQUEST_HPP
#define REQUEST_HPP

#include <iostream>
#include <vector>
#include <map>
#include <string>
#include <cstdlib>

class Request
{
	private:
		std::string							_method;
		std::string							_uri;
		std::string							_type;
		int									_size;
		int									_status;
	public:
		Request();
		void fillRequest(const std::string&);
		void printRequest();

		const std::string& getMethod();
		const std::string& getURI();
		const std::string& getType();
		int getSize();
		int getStatus();
};

Request& createRequest(const std::string);

#endif