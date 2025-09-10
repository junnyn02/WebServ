#ifndef REQUEST_HPP
#define REQUEST_HPP

#include <iostream>
#include <vector>
#include <map>

typedef enum e_method
{
	GET,
	POST,
	DELETE
} t_method;

class Request
{
	private:
		t_method							_method;
		std::string							_uri; //path?
		int									_port;
		std::map<std::string, std::string>	_headers;

	public:
		Request();
		Request(const Request&);
		Request &operator=(const Request&);
		~Request();
};

#endif