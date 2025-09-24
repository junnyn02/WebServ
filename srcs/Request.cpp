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
	_length = data.size;
	_size = 0;
	_status = 0;
	this->fillRequest(data);
}

const std::string& Request::getMethod() const
{
	return (_method);
}

const std::string& Request::getURI() const
{
	return (_uri);
}

const std::string& Request::getType() const
{
	return (_type);
}

const std::string& Request::getBody() const
{
	return (_body);
}

int Request::getLength() const
{
	return (_length);
}

int Request::getSize() const
{
	return (_size);
}

int Request::getStatus() const
{
	return (_status);
}

void Request::printRequest()
{
	if (_length == 0)
		return;
	std::cout << BOLDRED<<  "Request\n" << RESET;
	std::cout << "method : " << this->getMethod() << std::endl;
	std::cout << "uri : " << this->getURI() << std::endl;
	std::cout << "message length : " << this->getLength() << std::endl;
	std::cout << "size : " << this->getSize() << std::endl;
	std::cout << "type : " << this->getType() << std::endl;
	std::cout << "status : " << this->getStatus() << std::endl;
	std::cout << GREEN << "headers :\n" << RESET;
	std::map<std::string, std::string>::iterator it = _headers.begin();
	while (it != _headers.end())
	{
		std::cout << it->first << " : " << it->second << std::endl;
		it++;
	}
	std::cout << std::endl;
	std::cout << BOLDRED << "Body:\n" << RESET << getBody() << std::endl;
}

/* To-do :
 - handle chunking
 - check for forbidden characters in uri + percent encoding
 - check for unsupported media type
*/
int Request::parseRequestLine(const std::string& line)
{
	size_t sp = line.find(" ", 0);
	std::string method = line.substr(0, sp);
	if (method != "GET" && method != "POST" && method != "DELETE")
	{
		_status = 501;
		std::cerr << _status << " Not implemented\n";
		std::cout << BOLDRED << method << std::endl << RESET;
		return 0;
	}
	_method = method;
	sp++;
	size_t uri_end = line.find(" ", sp);
	_uri = line.substr(sp, (uri_end - sp));
	if (_uri.length() > 8000)
	{
		_status = 414;
		std::cerr << _status << " URI Too Long\n";
		return 0;
	}
	if (_uri.find("http://localhost8080") == 0)
		_uri.erase(0, 21);
	uri_end++;
	size_t crlf = line.find("\r\n", uri_end);
	if (line.substr(uri_end, (crlf - uri_end)) != "HTTP/1.1")
	{
		_status = 505;
		std::cout << line.substr(uri_end, crlf) << std::endl;
		std::cerr << _status << " HTTP Version Not Supported\n";
		return 0;
	}
	return 1;
}

int Request::parseHeaders(std::string& headers) //need to handle whitespace after ':'
{
	size_t line = headers.find("\r\n");
	while (line != std::string::npos)
	{
		std::string pair = headers.substr(0, line);
		size_t colon = pair.find(":");
		std::string key = pair.substr(0, colon);
		std::string value = pair.substr(colon + 1, pair.length() - 2);
		_headers.insert(std::pair<std::string, std::string>(key, value));
		headers.erase(0, line + 2);
		line = headers.find("\r\n");
		
	}
	if (_method == "POST")
	{
		std::map<std::string, std::string>::iterator it = _headers.find("Content-Length");
		if (it == _headers.end())
			it = _headers.find("content-length");
		if (it == _headers.end())
		{
			_status = 411;
			std::cerr << _status << " Length Required\n";
			return 0;
		}
		else
		{
			std::stringstream convert(it->second);
			convert >> _size;
		}
		it = _headers.find("Content-Type");
		if (it == _headers.end())
			it = _headers.find("content-type");
		if (it == _headers.end())
			_type = "octet-stream";										//see RFC s4.1
		else
			_type = it->second;
	}
	return 1;
}

void Request::parseBody(const std::string&) {}

void Request::fillRequest(const clientData& data)
{
	if (data.size == 0)													//might mess with chunking
		return;
	std::string request = data.buffer;
	size_t it = request.find("\r\n");
	while (it == 0)														//ignore empty lines before request line
	{
		request.erase(0, 2);
		it = request.find("\r\n");
	}
	std::string request_line = request.substr(0, it);
	if (!parseRequestLine(request_line))
		return;
	size_t empty = request.find("\r\n\r\n") + 2;						//skip first CRLF
	std::string headers = request.substr(it + 2, empty - (it + 2));
	if (!parseHeaders(headers))
		return;
	_body = request.substr(empty + 2, data.size - empty);
	//check if body is too long, error 413 -> defined in config file
}
