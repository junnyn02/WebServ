#include "ResponseBuilder.hpp"

//remplacer le root par le dossier reel ou est l'index, ex: http://localhost:8080/home/junguyen/Desktop/C05/WebServ/data/www/html/index.html

ResponseBuilder::ResponseBuilder(const Request &request) : _request(request)
{
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
}

const std::string  ResponseBuilder::getCode(void) const
{
    return (this->_code);
}

const std::string    ResponseBuilder::exec(void)
{
    if (_request.getMethod() == "GET")
    {
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
    return response;
}

void    ResponseBuilder::tryGet(void)
{
    struct stat buffer;
    std::ifstream infile(this->_request.getURI().c_str());
    if (stat(_request.getURI().c_str(), &buffer) != 0)
    {
        std::cout << "URI: " << this->_request.getURI().c_str() << std::endl;
        this->_code = "404 Not Found";
    }
    else if (!infile.is_open())
        this->_code = "403 Forbidden";
    else
    {
        this->_code = "200 OK";
        infile.close();
    }
    std::cout << "code: " << this->_code << std::endl;
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
    if (_code == "404 Not Found" || _code == "403 Forbidden")
        return ("text/html");
    std::string end;
    std::size_t found = _request.getURI().find('.');
    if (found != std::string::npos)
        end = _request.getURI().substr(found, _request.getURI().length());
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
    std::size_t found = _type.find("image");
    if (found == std::string::npos)
    {
        std::ifstream   infile(_request.getURI().c_str());
        std::string     line;
        char            c;
        while(infile.get(c))
            line.append(1, c);
        infile.close();
        return (line);
    }
    std::string media;
    std::ifstream   infile(_request.getURI().c_str(), std::ios::binary);
    infile.seekg (0, infile.end);
    unsigned long length = infile.tellg();
    infile.seekg (0, infile.beg);
    media.resize(length);
    infile.read(&media[0], media.size());
    infile.close();
    return (media);
}