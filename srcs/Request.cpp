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

const std::string& Request::getBody()
{
	return (_body);
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
	if (_size == 0)
		return;
	std::cout << "Request :\n";
	std::cout << "method : " << this->getMethod() << std::endl;
	std::cout << "uri : " << this->getURI() << std::endl;
	std::cout << "body : " << this->getBody() << std::endl;
	std::cout << "size : " << this->getSize() << std::endl;
	std::cout << "type : " << this->getType() << std::endl;
	std::cout << "status : " << this->getStatus() << std::endl;
}

/* To-do :
 - handle relative vs absolute uri = normalize, how?
 - handle chunking
 - check for forbidden characters in uri
 - check leading / trailing whitespace, are empty lines allowed?
 - check for wrong CRLF
 - check if no content length and type for POST request
 - check if body too long -> define limit
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
		return 0;
	}
	_method = method;
	sp++;
	size_t uri_end = line.find(" ", sp);
	_uri = line.substr(sp, (uri_end - sp));							//normalise uri + check forbidden characters
	if (_uri.length() > 8000)
	{
		_status = 414;
		std::cerr << _status << " URI Too Long\n";
		return 0;
	}
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

int Request::parseHeaders(const std::string& headers) //header field names are case insensitive?? RFC s4.2
{
	size_t size = headers.rfind("Content-Length", 0);
	if (size != std::string::npos)
	{
		size_t end = headers.find("\r\n", size);
		size += 15;
		if (headers[size] == ' ')
		{
			while (headers[size] == ' ' && size < end)
				size++;
		}
		std::string value = headers.substr(size, end - size);
		_size = atoi(value.c_str());									// risk of int overflow?
	}
	else if (_method == "POST")
		_type = "octet-stream";											//see RFC s4.1
	size_t type = headers.rfind("Content-Type:", 0);
	if (type != std::string::npos)
	{
		size_t end = headers.find("\r\n", type);
		type += 12;
		if (headers[type] == ' ')
		{
			while (headers[type] == ' ' && type < end)
				type++;
		}
		_type = headers.substr(type, end - type);
	}
	else if (_method == "POST")
	{
		_status = 411;
		std::cerr << _status << " Length Required\n";
		return 0;
	}
	return 1;
}

void Request::parseBody(const std::string&) {}

void Request::fillRequest(const clientData& data)
{
	if (data.size == 0)												//might mess with chunking
		return;
	std::string request = data.buffer;
	size_t it = request.find("\r\n");
	while (it == 0)													//ignore empty lines before request line
	{
		request.erase(0, 2);
		it = request.find("\r\n");
	}
	std::string request_line = request.substr(0, it);
	if (!parseRequestLine(request_line))
		return;
	size_t empty = request.find("\r\n\r\n") + 2;					//skip first CRLF
	std::string headers = request.substr(empty, empty - (it + 2));
	if (!parseHeaders(headers))
		return;
	_body = request.substr(empty + 2, data.size - empty);
	//parse body
}

// void Request::fillRequest(const clientData& data)
// {
// 	if (data.size == 0)
// 		return;
// 	std::string request = data.buffer;
// 	size_t sp = request.find(" ", 0);
// 	std::string method = request.substr(0, sp);
// 	if (method != "GET" && method != "POST" && method != "DELETE")
// 	{
// 		_status = 501;
// 		std::cerr << "Not implemented\n";
// 		return;
// 	}
// 	_method = method;
// 	sp++;
// 	size_t end = request.find(" ", sp);
// 	_uri = request.substr(sp, (end - sp) );
// 	end++;
// 	sp = request.find(13, end);
// 	if (request.substr(end, (sp - end)) != "HTTP/1.1")
// 	{
// 		_status = 505;
// 		std::cout << request.substr(end, sp) << std::endl;
// 		std::cerr << "HTTP Version Not Supported\n";
// 		return;
// 	}
// 	size_t body = request.find("\r\n\r\n", end);
// 	size_t size_key = request.rfind("Content-Length:", body);
// 	if (size_key != std::string::npos)
// 	{
// 		size_t line = request.find("\r\n", size_key);
// 		size_key += 15;
// 		std::string value = request.substr(size_key, line - size_key);
// 		_size = atoi(value.c_str());									// risk of int overflow?
// 	}
// 	size_t type_key = request.rfind("Content-Type:", body);
// 	if (type_key != std::string::npos)
// 	{
// 		size_t line = request.find("\r\n", type_key);
// 		type_key += 12;
// 		if (request[type_key] == ' ')
// 			while (request[type_key] == ' ' && type_key < line)
// 				type_key++;
// 		_type = request.substr(type_key, line - type_key);
// 	}
// 	_body = request.substr(body + 4, data.size - body);
// }
