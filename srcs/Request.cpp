#include "../include/Request.hpp"
#include "../include/serverCore.hpp"

Request::Request() {}

Request::Request(const clientData& data)
{
	_method = "";
	_uri = "";
	_type = "";
	_size = 0;
	_name = "";
	_body = "";
	_length = data.size;
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

const std::string& Request::getName() const
{
	return (_name);
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
	std::cout << "content type : " << this->getType() << std::endl;
	std::cout << "content size : " << this->getSize() << std::endl;
	std::cout << "file name : " << this->getName() << std::endl;
	std::cout << "message length : " << this->getLength() << std::endl;
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

/*
POST requests don't always have a body, ex: a form can be included in a query string instead of body.
Should rework parsing to handle empty body depending on content-type.
*/

std::string Request::parseBody(const std::string& raw)
{
	if (_size == 0)
		return "";
	if (_type.find("multipart/form-data") == std::string::npos)
	{
		_status = 415;
		std::cerr << _status << " Unsupported Media Type\n";
		return "";
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
	pos = raw.find(boundary);
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
	pos = header.find("Content-Type: image/");
	if (pos == std::string::npos)
		pos = header.find("content-type: image/");
	if (pos == std::string::npos)
	{
		_status = 415;
		std::cerr << _status << " Unsupported Media Type\n";		//or bad request?
		return "";
	}
	pos += 14;
	_type = header.substr(pos, header.length() - pos);
	end = raw.find("\r\n\r\n");
	std::string body = raw.substr(end + 4, _size);
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
		std::string raw_value = pair.substr(colon + 1, pair.length() - 2);
		std::string value = removeSpace(raw_value);
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
			std::cerr << _status << " Length Required\n";									//chunked requests have no content-length header, rework
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
			_type = "octet-stream";
		else
			_type = it->second;
	}
	return 1;
}

std::string normalizeUri(const std::string& raw)// rework to parse query strings
{
	std::string clean;
	if (raw.find("http://localhost8080") == 0)
		clean = raw.substr(21, raw.length() - 21);
	else
		clean = raw;
	if (clean.find("?") != std::string::npos)
	{
		size_t query = clean.find("?");
		clean.erase(query);
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



void Request::fillRequest(const clientData& data)
{
	if (data.size == 0)
		return;
	std::string request(data.buffer, data.size);
	size_t it = request.find("\r\n");
	while (it == 0)
	{
		request.erase(0, 2);
		it = request.find("\r\n");
	}
	std::string request_line = request.substr(0, it);
	if (!parseRequestLine(request_line))
		return;
	size_t empty = request.find("\r\n\r\n") + 2;
	std::string headers = request.substr(it + 2, empty - (it + 2));
	if (!parseHeaders(headers))
		return;
	if (empty + 2 > data.size)
	{
		_status = 400;
		std::cerr << _status << " Bad request: claimed length inconsistent with received bytes\n";
		return;
	}
	if (_method == "POST")
	{
		std::string raw_body = request.substr(empty + 2, data.size - empty);
		_body = parseBody(raw_body);
		// std::ofstream test(_name.c_str(), std::ios::binary);
		// test << _body;
		// test.close();
	}
	//check if body is too long, error 413 -> defined in config file
}

void	Request::setURI(const std::string &new_str)
{
	this->_uri = new_str;
}