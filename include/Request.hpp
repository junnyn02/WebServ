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
		std::string							_type; // content type
		std::string							_body; // use string or char* ?
		int									_size; // content size
		int									_status;
		Request();
	public:
		Request(const struct clientData&);
		void fillRequest(const struct clientData&);
		int parseRequestLine(const std::string&);
		int parseHeaders(const std::string&);
		void parseBody(const std::string&);
		void printRequest();

		const std::string& getMethod();
		const std::string& getURI() const;
		const std::string& getType();
		const std::string& getBody();
		int getSize();
		int getStatus();

		void	setURI(const std::string &);
};

#endif