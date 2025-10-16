#include "../include/Request.hpp"
#include "../include/serverCore.hpp"
#include "CGI.hpp"

Request::Request()
{
	_method = "";
	_uri = "";
	_query = "";
	_cgi = false;
	_type = "";
	_size = 0;
	_name = "";
	_body = "";
	_status = 0;
}

Request::Request(const std::string& data, int data_size)
{
	_method = "";
	_uri = "";
	_query = "";
	_cgi = false;
	_type = "";
	_size = 0;
	_name = "";
	_body = "";
	_status = 0;
	this->fillRequest(data, data_size);
}

Request::~Request() {}

void Request::clearRequest()
{
	_method = "";
	_uri = "";
	_query = "";
	_cgi = false;
	_type = "";
	_size = 0;
	_name = "";
	_body = "";
	_status = 0;
}


const std::string& Request::getMethod() const
{
	return (_method);
}

const std::string& Request::getURI() const
{
	return (_uri);
}

const std::string& Request::getQuery() const
{
	return (_query);
}

bool Request::getCgi() const
{
	return (_cgi);
}

const std::string& Request::getType() const
{
	return (_type);
}

const std::string& Request::getName() const
{
	return (_name);
}

const std::string& Request::getBody() const
{
	return (_body);
}

int Request::getSize() const
{
	return (_size);
}

int Request::getStatus() const
{
	return (_status);
}

const std::map<std::string, std::string>& Request::getInfo(void) const
{
	return this->_server->getInfo();
}

void Request::printRequest()
{
	std::cout << BOLDRED<<  "Request\n" << RESET;
	std::cout << "method : " << this->getMethod() << std::endl;
	std::cout << "uri : " << this->getURI() << std::endl;
	std::cout << "query : " << this->getQuery() << std::endl;
	std::cout << "CGI : " << this->getCgi() << std::endl;
	std::cout << "content type : " << this->getType() << std::endl;
	std::cout << "content size : " << this->getSize() << std::endl;
	std::cout << "file name : " << this->getName() << std::endl;
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

std::string Request::parseForm(const std::string& raw)
{
	size_t empty = raw.find("\r\n\r\n") + 4;
	std::string body = raw.substr(empty, _size - empty);
	return body;
}

std::string Request::parseBody(const std::string& raw)
{
	std::string body = "";
	if (_type.find("multipart/form-data") == std::string::npos)
	{
		if (_type == "application/x-www-form-urlencoded")
		{
			body = parseForm(raw);
			return body;
		}
		else
		{
			_status = 415;
			std::cerr << _status << " Unsupported Media Type\n";
			return body;
		}
	}
	size_t pos = _type.find("boundary=");
	if (pos == std::string::npos)
	{
		_status = 400;
		std::cerr << _status << " Bad request: missing media header\n";
		return "";
	}
	pos += 9;
	std::string boundary = _type.substr(pos, _type.length() - pos);
	size_t empty = raw.find("\r\n\r\n") + 4;
	pos = raw.find(boundary, empty);
	if (pos == std::string::npos)
	{
		_status = 400;
		std::cerr << _status << " Bad request: missing media header\n";
		return "";
	}
	pos += boundary.length();
	size_t end = raw.find("\r\n\r\n", pos);
	if (end == std::string::npos)
	{
		_status = 400;
		std::cerr << _status << " Bad request: missing CRLF\n";
		return "";
	}
	std::string header = raw.substr(pos, end - pos);
	pos = header.find("filename");
	if (pos == std::string::npos)
	{
		_status = 400;
		std::cerr << _status << " Bad request: missing file name\n";	//should we accept this?
		return "";
	}
	pos += 10;
	end = header.find("\r\n", pos);
	if (end == std::string::npos)
	{
		_status = 400;
		std::cerr << _status << " Bad request: missing CRLF\n";
		return "";
	}
	end -= 1;
	_name = header.substr(pos, end - pos);
	pos = header.find("Content-Type: image/");					//might be case insensitive
	if (pos == std::string::npos)
	{
		_status = 415;
		std::cerr << _status << " Unsupported Media Type\n";		//or bad request?
		return "";
	}
	pos += 14;
	_type = header.substr(pos, header.length() - pos);
	end = raw.find("\r\n\r\n", pos);
	end += 4;
	end = raw.find("\r\n\r\n", end);
	body = raw.substr(end + 4, _size - header.length());
	_size -=header.length();
	return body;
}

std::string removeSpace(const std::string& line)
{
	int i = 0;
	while (line[i] == ' ')
		i++;
	std::string new_line = line.substr(i, line.length() - i);
	return new_line;
}

void lowerKey(std::string& key)
{
	for (size_t i = 0; i < key.length(); i++)
	{
		if (key[i] >= 'A' && key[i] <= 'Z')
			key[i] += 32;
	}
}

int Request::parseHeaders(std::string& headers)
{
	size_t line = headers.find("\r\n");
	while (line != std::string::npos)
	{
		std::string pair = headers.substr(0, line);
		size_t colon = pair.find(":");
		if (colon == std::string::npos)
		{
			_status = 400;
			std::cerr << _status << " Malformed header (no ':')\n";
			return 0;
		}
		std::string key = pair.substr(0, colon);
		lowerKey(key);
		std::string raw_value = pair.substr(colon + 1, pair.length() - 2);
		std::string value = removeSpace(raw_value);
		_headers.insert(std::pair<std::string, std::string>(key, value));
		headers.erase(0, line + 2);
		line = headers.find("\r\n");
		
	}
	if (_method == "POST")
	{
		std::map<std::string, std::string>::iterator it = _headers.find("content-length");
		if (it == _headers.end())
		{
			_status = 411;
			std::cerr << _status << " Length Required\n";									//chunked requests have no content-length header, rework
			return 0;
		}
		else
		{
			std::stringstream convert(it->second);
			convert >> _size;
		}
		it = _headers.find("content-type");
		if (it == _headers.end())
			_type = "octet-stream";
		else
			_type = it->second;
	}
	return 1;
}

std::string Request::normalizeUri(const std::string& raw)
{
	std::string clean;
	if (raw.find("http://localhost8080") == 0)
		clean = raw.substr(21, raw.length() - 21);
	else
		clean = raw;
	if (raw.find(".php") != std::string::npos)
		_cgi = true;
	if (clean.find("?") != std::string::npos)
	{
		size_t query = clean.find("?") + 1;
		_query = raw.substr(query, raw.length() - query);
		clean.erase(query - 1);
	}
	return clean;
}

int	checkChar(const std::string& uri)
{
	for (size_t i = 0; i < uri.length(); i++)
	{
		if ((uri[i] >= '#' && uri[i] <= ';') || (uri[i] >= '?' && uri[i] <= '[') || (uri[i] >= 'a' && uri[i] <= 'z') ||
       uri[i] == '!' || uri[i] == '=' || uri[i] == ']' || uri[i] == '_' || uri[i] == '~')
		return 1;
	}
	return 0;
}

int Request::parseRequestLine(const std::string& line)
{
	size_t sp = line.find(" ", 0);
	if (sp == std::string::npos)
	{
		_status = 400;
		std::cerr << _status << " Bad request sp\n";
		return 0;
	}
	std::string method = line.substr(0, sp);
	if (method != "GET" && method != "POST" && method != "DELETE")
	{
		_status = 501;
		std::cerr << _status << " Not implemented\n";
		return 0;
	}
	_method = method;
	sp++;
	size_t uri_end = line.find(" ", sp);
	if (uri_end == std::string::npos || uri_end <= sp)
	{
		_status = 400;
		std::cerr << _status << " Bad request uri_end\n";
		return 0;
	}
	std::string raw_uri = line.substr(sp, (uri_end - sp));
	if (raw_uri.length() > 8000)
	{
		_status = 414;
		std::cerr << _status << " URI Too Long\n";
		return 0;
	}
	if (!checkChar(raw_uri))
	{
		_status = 400;
		std::cerr << " Bad request: forbidden character in URI\n";
		return 0;
	}
	_uri = normalizeUri(raw_uri);
	uri_end++;
	size_t crlf = line.length();
	if (line.substr(uri_end, (crlf - uri_end)) != "HTTP/1.1")
	{
		_status = 505;
		std::cout << line.substr(uri_end, crlf) << std::endl;
		std::cerr << _status << " HTTP Version Not Supported\n";
		return 0;
	}
	return 1;
}

void Request::fillRequest(const std::string& data, int data_size)
{
	std::string request = data;
	size_t it = request.find("\r\n");
	while (it == 0)
	{
		request.erase(0, 2);
		it = request.find("\r\n");
	}
	std::string request_line = request.substr(0, it);
	if (!parseRequestLine(request_line))
		return;
	ssize_t empty = request.find("\r\n\r\n") + 2;
	std::string headers = request.substr(it + 2, empty - (it + 2));
	if (!parseHeaders(headers))
		return;
	if (empty + 2 > data_size)
	{
		_status = 400;
		std::cerr << _status << " Bad request: claimed length inconsistent with received bytes\n";
		return;
	}
	// std::cout << "CONF SIZE " << _server->getInfo().size() << std::endl;
	// printConfig(_server->getInfo());
	//check if body is too long, error 413 -> defined in config file
}

void	Request::setURI(const std::string &new_str)
{
	this->_uri = new_str;
}

void	Request::setStatus(int status)
{
	this->_status = status;
}

void	Request::setServer(Server* server)
{
	this->_server = server;
}

void	Request::setBody(const std::string& body)
{
	this->_body = body;
}