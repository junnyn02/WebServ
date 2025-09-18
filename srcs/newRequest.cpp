#include "../include/Request.hpp"
#include "../include/serverCore.hpp"

Request::Request()
{
	_method = "";
	_uri = "";
	_type = "";
	_size = 0;
	_status = 0;
}

Request::Request(const clientData& data)
{
	_method = "";
	_uri = "";
	_type = "";
	_body = "";
	_size = data.size;
	_status = 0;
	std::cout << "constructed\n";
	this->fillRequest(data);
}

const std::string& Request::getMethod()
{
	return (_method);
}

const std::string& Request::getURI()
{
	return (_uri);
}

const std::string& Request::getType()
{
	return (_type);
}

int Request::getSize()
{
	return (_size);
}

int Request::getStatus()
{
	return (_status);
}

void Request::printRequest()
{
	std::cout << "method : " << this->getMethod() << std::endl;
	std::cout << "uri : " << this->getURI() << std::endl;
	std::cout << "size : " << this->getSize() << std::endl;
	std::cout << "type : " << this->getType() << std::endl;
	std::cout << "status : " << this->getStatus() << std::endl;
}

void Request::fillRequest(const clientData& data)
{
	std::string request = data.buffer;
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
	if (request.substr(end, (sp - end)) != "HTTP/1.1")
	{
		_status = 505;
		std::cout << request.substr(end, sp) << std::endl;
		std::cerr << "HTTP Version Not Supported\n";
		return;
	}
	size_t body = request.find("\r\n\r\n", end);
	size_t size_key = request.rfind("Content-Length:", body);
	if (size_key != std::string::npos)
	{
		size_t line = request.find("\r\n", size_key);
		size_key += 15;
		std::string value = request.substr(size_key, line - size_key);
		_size = atoi(value.c_str());
	}
	size_t type_key = request.rfind("Content-Type:", body);
	if (type_key != std::string::npos)
	{
		size_t line = request.find("\r\n", type_key);
		type_key += 12;
		if (request[type_key] == ' ')
			while (request[type_key] == ' ' && type_key < line)
				type_key++;
		_type = request.substr(type_key, line - type_key);
	}
	_body = request.substr(body + 4, data.size - body); //check
	std::cout << "done\n";
}
