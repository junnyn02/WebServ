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

const std::string    ResponseBuilder::sendResponse(void)
{
    std::string response;
    if (_request.getMethod() == "GET")
    {
        tryGet();
        _type = getType();
        _body = getBody();
    }
    else if (_request.getMethod() == "POST")
    {
        tryPost();
    }
    response = "HTTP/1.1 " + getCode() + "\r\n";
    response.append("Server: webserv\nDate: " + getDate() + "\r\n");
    response.append("Content-Type: " + _type + "\r\n");
    response.append("Content-Length: " + size_t_to_string(_body.length()) + "\r\n");
    if (_code == "201 Created")
        response.append("Location: /uploads/" + _request.getURI() + "\r\n"); //URI = filename ?
    response.append("\r\n" + _body);
    std::cout << "RESPONSE =" << std::endl << response << std::endl;
    return response;
}

bool isDirectoryAccessible(const std::string& path)
{
    DIR* dir = opendir(path.c_str());
    if (dir)
    {
        closedir(dir);
        return true;
    }
    return false;
}

void    ResponseBuilder::isDir(const std::string &path)
{
    struct stat buffer;
    if (isDirectoryAccessible(path))
        {
            std::string indexPath = path + "/index.html";
            if (stat(indexPath.c_str(), &buffer) == 0 && S_ISREG(buffer.st_mode))
            {
                std::ifstream infile(indexPath.c_str());
                if (infile.is_open())
                {
                    this->_code = "200 OK";
                    infile.close();
                    _request.setURI(indexPath); // Rediriger vers index.html
                }
                else
                    this->_code = "403 Forbidden";
            }
            else
                this->_code = "404 Not Found";
        }
        else
            this->_code = "404 Not Found";
}

void    ResponseBuilder::isFile(const std::string &path)
{
    std::ifstream infile(path.c_str());
    if (infile.is_open())
    {
        this->_code = "200 OK";
        infile.close();
    }
    else
        this->_code = "403 Forbidden";
}

void    ResponseBuilder::tryGet(void)
{
    struct stat buffer;
    std::string dir = getDir();
    _request.setURI(dir + "/data/www/html" + _request.getURI()); //modifier selon le parsing du fichier conf
    const std::string& path = _request.getURI();
    std::ifstream infile(path.c_str());
    if (stat(path.c_str(), &buffer) != 0)
    {
        std::cout << "URI not found: " << path << std::endl;
        this->_code = "404 Not Found";
    }
    else if (S_ISDIR(buffer.st_mode))
        isDir(path);
    else if (S_ISREG(buffer.st_mode))
        isFile(path);
    else
        this->_code = "403 Forbidden"; // Ni fichier ni dossier
    // std::cout << "code: " << this->_code << std::endl;
}

bool    ResponseBuilder::checkMime(void)
{
    std::cout << "[CHECK_MIME] type is : " << _request.getType() << std::endl;
    if (_request.getType() == "image/png" || _request.getType() == "image/jpeg" || _request.getType() == "image/jpg" || _request.getType() == "image/gif")
        return true;
    return false;
}

void    ResponseBuilder::tryPost(void)
{
    // std::cout << _request.get
    if (!checkMime())
    {
        _code = "415 Unsupported Media Type";
        _body = "<html><body><h1>415 - Unsupported Media Type</h1></body></html>";
        _type = "text/html";
    }
    else if (_request.getSize() > BUFFER_SIZE)
    {
        _code = "413 Payload Too Large";
        _body = "<html><body><h1>413 - Payload Too Large</h1></body></html>";
        _type = "text/html";
    }
    else
    {
        _code = "201 Created";
        _type = "application/json";
        _body = "{\"status\":\"success\",\"message\":\"Image uploaded\",\"url\":\"/uploads/photo.jpg\"}"; //change w/ name of file
    }
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

const std::string   ResponseBuilder::getDir(void) const
{
    char buffer[1096];
    if (getcwd(buffer, sizeof(buffer)) != NULL)
        return std::string(buffer);
    else
        return std::string("Error");
}