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
    sendResponse();
    // std::cout << "[BODY SIZE]: " << _body.size() << std::endl;
    // std::cout << "[CONTENT BODY]:\n" << _body.data() << std::endl; 
}

ResponseBuilder::~ResponseBuilder()
{
}

const std::string  ResponseBuilder::getCode(void) const
{
    return (this->_code);
}

void    ResponseBuilder::sendResponse(void)
{
    std::string response;
    if (_request.getMethod() == "GET")
    {
        if (_request.getURI() == "/list-uploads")
            buildListUploads();
        else
            tryGet();
    }
    else if (_request.getMethod() == "DELETE")
    {
        tryDelete();
        if (_code == "204 No Content")
        {
            _header = build204();
            return ;
        }
    }
    else if (_request.getMethod() == "POST")
    {
        tryPost();
    }
    _header = "HTTP/1.1 " + getCode() + "\r\n";
    _header.append("Server: webserv\nDate: " + getDate() + "\r\n");
    _header.append("Content-Type: " + _type + "\r\n");
    _header.append("Content-Length: " + size_t_to_string(_body.size()) + "\r\n");
    if (_code == "201 Created")
        _header.append("Location: /uploads/" + _request.getURI() + "\r\n"); //URI = filename ?
    // _header.append("\r\n");
    _header.append("\r\n" + _body);
    // std::cout << "RESPONSE =" << std::endl << response << std::endl;
    // return response;
}

void    ResponseBuilder::buildListUploads(void)
{
    std::string folder = "data/www/html/upload/";
    DIR* dir = opendir(folder.c_str());
    std::vector<std::string> images;

    if (dir != NULL) {
        struct dirent* ent;
        while ((ent = readdir(dir)) != NULL) {
            std::string name = ent->d_name;

            // Ne pas inclure . et ..
            if (name == "." || name == "..")
                continue;

            // Tu peux filtrer plus ici (par extension)
            images.push_back("/upload/" + name);
        }
        closedir(dir);
    }

    // Formater en JSON
    std::ostringstream oss;
    oss << "[";
    for (size_t i = 0; i < images.size(); ++i) {
        oss << "\"" << images[i] << "\"";
        if (i != images.size() - 1)
            oss << ",";
    }
    oss << "]";
    _body = oss.str();
    // _body = std::vector<char>(str.begin(), str.end());
    _code = "200 OK";
    _type = "application/json";
    // std::cout << "[LIST UPLOADS]" << _body << " | [CODE]: " << _code << " | [TYPE]: " << _type << std::endl;
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
    _type = getType();
    _body = setBody();
}

bool    ResponseBuilder::checkMime(void)
{
    // std::cout << "[CHECK_MIME] type is : " << _request.getType() << std::endl;
    if (_request.getType() == "image/png" || _request.getType() == "image/jpeg" || _request.getType() == "image/jpg" || _request.getType() == "image/gif")
        return true;
    return false;
}

int checkIfFileExists(const std::string &filename)
{
  struct stat buffer;
  return (stat(filename.c_str(), &buffer));
}

bool    ResponseBuilder::createFile(void)
{
    std::string	param[4] = { ".jpg", ".jpeg", ".png", ".gif" };
    try
    {
        _filename = _request.getName();
        std::string full_path = "data/www/html/upload/" + _filename;
        if (checkIfFileExists(full_path) == 0)
        {
            int i = 1;
            while (checkIfFileExists(full_path) == 0)
            {
                std::stringstream nb;
                nb << i;
                std::size_t found;
                for (int j = 0; j < 4; j++)
                {
                    found = _request.getName().find(param[j]);
                    if (found != std::string::npos)
                        break;
                }
                _filename.insert(found, nb.str());
                full_path = "data/www/html/upload/" + _filename;
                i++;
            }
        }
        full_path = "data/www/html/upload/" + _filename;
        std::ofstream   outfile(full_path.c_str(), std::ios::binary);
        outfile.write(_request.getBody().c_str(), _request.getBody().length());
        outfile.close();
    }
    catch(const std::exception& e)
    {
        std::cerr << e.what() << '\n';
        return false;
    }
    return true;
}

void    ResponseBuilder::tryPost(void)
{
    // std::cout << _request.get
    std::string msg;
    if (!checkMime())
    {
        _code = "415 Unsupported Media Type";
        // msg = "<html><body><h1>415 - Unsupported Media Type</h1></body></html>";
        _body = "<html><body><h1>415 - Unsupported Media Type</h1></body></html>";
        _type = "text/html";
    }
    else if (_request.getSize() > 30000000)
    {
        std::cerr << "[SIZE TOO LARGE]" << std::endl;
        _code = "413 Payload Too Large";
        // msg = "<html><body><h1>413 - Payload Too Large</h1></body></html>";
        _body = "<html><body><h1>413 - Payload Too Large</h1></body></html>";
        _type = "text/html";
    }
    else
    {
        std::cout << "[CREATING FILE]" << std::endl;
        createFile();
        std::cout << "[FILE CREATED]" << std::endl;
        _code = "201 Created";
        _type = "application/json";
        // msg = "{\"status\":\"success\",\"message\":\"Image uploaded\",\"url\":\"/upload/" + _filename + "\"}"; //change w/ name of file
        _body = "{\"status\":\"success\",\"message\":\"Image uploaded\",\"url\":\"/upload/" + _filename + "\"}";
    }
}

const std::string   ResponseBuilder::build204(void)
{
    std::string response;
    response = "HTTP/1.1 " + getCode() + "\r\n";
    response.append("Server: webserv\nDate: " + getDate() + "\r\n");
    std::cout << "[RESPONSE] : " << response << std::endl;
    return response;
}

void    ResponseBuilder::tryDelete(void)
{
    std::cout << "[TRY DELETE]" << std::endl;
    std::string full_path = "data/www/html/upload/" + _request.getURI();
    if (checkIfFileExists(full_path) == 0)
    {
        if (std::remove(full_path.c_str()) != 0)
            _code = "403 Forbidden";
        else
            _code = "204 No Content";
    }
    _code = "404 Not Found";
    if (_code != "204 No Content")
    {
        _type = getType();
        _body = getBody();
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

const std::string   ResponseBuilder::setBody(void) const
{
    if (_code == "404 Not Found")
    {
        // std::string error = "<html><body><h1>404 - Page Not Found</h1></body></html>";
        return ("<html><body><h1>404 - Page Not Found</h1></body></html>");
    }
    if (_code == "403 Forbidden")
    {
        // std::string error = "<html><body><h1>403 - Fordbidden</h1></body></html>";
        return ("<html><body><h1>403 - Fordbidden</h1></body></html>");
    }
    std::size_t found = _type.find("image");
    if (found == std::string::npos)
    {
        std::ifstream       infile(_request.getURI().c_str()); //gerer si image pas trouve ?
        std::vector<char>   body((std::istreambuf_iterator<char>(infile)), std::istreambuf_iterator<char>());
        std::string str(body.begin(), body.end());
        infile.close();
        return (str);
    }
    std::string media;
    std::ifstream   infile(_request.getURI().c_str(), std::ios::binary);
    std::vector<char>   body((std::istreambuf_iterator<char>(infile)), std::istreambuf_iterator<char>());
    std::string str(body.begin(), body.end());
    infile.close();
    return (str);
}

// const std::vector<char> ResponseBuilder::getBody(void) const
// {
//     return (this->_body);
// }

const std::string   ResponseBuilder::getHeader(void) const
{
    return (this->_header);
}

const std::string   ResponseBuilder::getDir(void) const
{
    char buffer[1096];
    if (getcwd(buffer, sizeof(buffer)) != NULL)
        return std::string(buffer);
    else
        return std::string("Error");
}