#include "ResponseBuilder.hpp"

ResponseBuilder::ResponseBuilder(const ParserRequest &request) : _request(request)
{
    if (MSG)
        std::cout << "Default constructor called" << std::endl;
    _mime.insert(std::pair<std::string, std::string>(".htm", "text/html"));
    _mime.insert(std::pair<std::string, std::string>(".html", "text/html"));
    _mime.insert(std::pair<std::string, std::string>(".css", "text/css"));
    _mime.insert(std::pair<std::string, std::string>(".js", "application/javascript"));
    _mime.insert(std::pair<std::string, std::string>(".json", "application/json"));
    _mime.insert(std::pair<std::string, std::string>(".png", "image/png"));
    _mime.insert(std::pair<std::string, std::string>(".jpeg", "image/jpeg"));
    _mime.insert(std::pair<std::string, std::string>(".jpg", "image/jpeg"));
    _mime.insert(std::pair<std::string, std::string>(".gif", "image/gif"));
    _mime.insert(std::pair<std::string, std::string>(".txt", "text/plain"));
}

ResponseBuilder::~ResponseBuilder()
{
    if (MSG)
        std::cout << "Default destructor called" << std::endl;
}

std::string  ResponseBuilder::getCode(void) const
{
    return (this->_code);
}

std::string ResponseBuilder::buildResponse(void)
{
    return "build";
}

void    ResponseBuilder::exec(void)
{
    if (_request.getMethod() == "GET")
    {
        std::cout << "GET METHOD" << std::endl;
        tryGet();
    }
    std::string response, body;
    response = "HTTP/1.1 " + getCode() + "\r\n";
    response.append("Server: webserv\nDate: " + getDate() + "\r\n");
    _type = getType();
    response.append("Content-Type: " + _type + "\r\n");
    body = getBody();
    response.append("Content-Length: " + size_t_to_string(body.length()) + "\r\n");
    response.append("\r\n" + body);
    std::cout << "RESPONSE =" << std::endl << response << std::endl;
}

void    ResponseBuilder::tryGet(void)
{
    struct stat buffer;
    std::ifstream infile(this->_request.getPath().c_str());
    if (stat(_request.getPath().c_str(), &buffer) != 0)
        this->_code = "404 Not Found";
    else if (!infile.is_open())
        this->_code = "403 Forbidden";
    else
        this->_code = "200 OK";
}

const std::string    ResponseBuilder::getDate(void) const
{
    char format[31];
  	std::time_t time = std::time(0);
 	std::tm	*now = std::gmtime(&time);
    strftime(format, 31, "%a, %d %h %Y %H:%M:%S %Z", now);
    return (format);
}

const std::string   ResponseBuilder::getType(void) const
{
    if (_code == "404 Not Found")
        return ("text/html");
    std::string end;
    std::size_t found = _request.getPath().find('.');
    if (found != std::string::npos)
        end = _request.getPath().substr(found, _request.getPath().length());
    std::map<std::string, std::string>::const_iterator it = _mime.begin();
    for (; it != _mime.end(); ++it)
    {
        if (end == it->first)
            return (it->second);
    }
    return ("application/octet-stream");
}

const std::string   ResponseBuilder::getBody(void) const
{
    if (_code == "404 Not Found")
        return ("<html><body><h1>404 - Page Not Found</h1></body></html>");
    if (_code == "403 Forbidden")
        return ("<html><body><h1>403 - Fordbidden</h1></body></html>");
    
    std::ifstream   infile(_request.getPath().c_str());
    std::string     line;
    char            c;
	while(infile.get(c))
		line.append(1, c);
    return (line);
}