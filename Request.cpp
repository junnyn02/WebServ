#include "include/Request.hpp"

Request::Request(const Request& copy) { (void)copy; }
Request& Request::operator=(const Request& copy) { (void)copy; return *this;}
Request::~Request() {}

Request::Request()
{
	_method = "";
	_uri = "";
	_port = 80;
	_status = 0;
}

std::string& Request::getMethod()
{
	return (_method);
}

std::string& Request::getURI()
{
	return (_uri);
}

int Request::getPort()
{
	return (_port);
}

int Request::getStatus()
{
	return (_status);
}

void Request::printRequest()
{
	std::cout << "method : " << this->getMethod() << std::endl;
	std::cout << "uri : " << this->getURI() << std::endl;
	std::cout << "port : " << this->getPort() << std::endl;
	std::cout << "status : " << this->getStatus() << std::endl;
	if (_headers.empty() == false)
	{
		std::map<std::string, std::string>::iterator it = _headers.begin();
		while (it != _headers.end())
		{
			std::cout << it->first << ": " << it->second << std::endl;
			it++;
		}
	}
}

void Request::fillRequest(const std::string& request)
{
	//ignore if request line starts with CRLF (4.1) ?
	size_t sp = request.find(" ");
	std::string method = request.substr(0, sp);
	if (method != "GET" && method != "POST" && method != "DELETE")
	{
		_status = 501;
		std::cerr << "Not implemented\n";
		return;
	}
	_method = method;
	sp++;
	size_t end = request.find(" ", sp);
	_uri = request.substr(sp, (end - sp) );
	end++;
	sp = request.find(13, end);
	if (request.substr(end, (sp -end)) != "HTTP/1.1")
	{
		_status = 505;
		std::cout << request.substr(end, sp) << std::endl;
		std::cerr << "HTTP Version Not Supported\n";
		return;
	}
	sp = request.find("\r\n", end);
	if (sp < request.length())
	{
		std::cout << "Headers\n";
		//parse headers
	}
}

