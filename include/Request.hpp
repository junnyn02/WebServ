#ifndef REQUEST_HPP
#define REQUEST_HPP

#define GREEN "\033[32m"
#define BOLDRED "\033[1m\033[31m"
#define RESET "\033[0m"

#include <iostream>
#include <vector>
#include <map>
#include <string>
#include <cstdlib>
#include <sstream>

class Request
{
	private:
		std::string							_method;
		std::string							_uri;
		std::string							_type; 		//content type
		size_t								_size;		//content size
		std::string							_body;		//use string or char* ?
		size_t								_length;	//request string size
		int									_status;
		std::map<std::string, std::string>	_headers;
		Request();
	public:
		Request(const struct clientData&);
		void fillRequest(const struct clientData&);
		int parseRequestLine(const std::string&);
		int parseHeaders(std::string&);
		void parseBody(const std::string&);
		void printRequest();

		const std::string& getMethod() const;
		const std::string& getURI() const;
		const std::string& getType() const;
		const std::string& getBody() const;
		int getSize() const;
		int getStatus() const;
};

#endif