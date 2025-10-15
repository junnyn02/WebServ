#ifndef REQUEST_HPP
#define REQUEST_HPP

#define GREEN "\033[32m"
#define BOLDRED "\033[1m\033[31m"
#define RESET "\033[0m"

#include "Server.hpp"
#include <iostream>
#include <vector>
#include <map>
#include <string>
#include <cstdlib>
#include <sstream>
#include <iostream>
#include <fstream>

class Request
{
	private:
		std::string							_method;
		std::string							_uri;
		std::string							_type; 		//content type
		size_t								_size;		//content size
		std::string							_name;		//filename
		std::string							_body;
		int									_status;
		std::map<std::string, std::string>	_headers;
		Server*								_server;

	public:
		Request();
		Request(const std::string&, int data_size);
		void fillRequest(const std::string& data, int data_size);
		int parseRequestLine(const std::string&);
		int parseHeaders(std::string&);
		std::string parseBody(const std::string&);
		void printRequest();

		const std::string& getMethod() const;
		const std::string& getURI() const;
		const std::string& getType() const;
		const std::string& getName() const;
		const std::string& getBody() const;
		int getSize() const;
		int getStatus() const;

		void	setStatus(int);
		void	setServer(Server*);
		void	setURI(const std::string &);
		void	setBody(const std::string& body);

};

std::string normalizeUri(const std::string&);

#endif