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
		std::string							_body; //use string or char* ?
		int									_size; //content size
		int									_status;
		Request();
	public:
		Request(const struct clientData&);
		//void fillRequest(const std::string&);
		void fillRequest(const struct clientData&);
		void printRequest();

		const std::string& getMethod();
		const std::string& getURI();
		const std::string& getType();
		int getSize();
		int getStatus();
};

#endif