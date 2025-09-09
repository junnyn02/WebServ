#include "ResponseBuilder.hpp"

ResponseBuilder::ResponseBuilder(const ParserRequest &request) : _request(request)
{
    if (MSG)
        std::cout << "Default constructor called" << std::endl;
}

ResponseBuilder::~ResponseBuilder()
{
    if (MSG)
        std::cout << "Default destructor called" << std::endl;
}

std::string  ResponseBuilder::getCode(void) const
{
    return this->_code;
}

std::string ResponseBuilder::buildResponse(void)
{
    return "build";
}

void    ResponseBuilder::exec(void)
{
    std::ofstream outfile("Response.txt");
    if (_request.getMethod() == "GET")
    {
        std::cout << "GET METHOD" << std::endl;
        tryGet();
    }
    outfile << "HTTP/1.1 " << getCode() << std::endl;
    outfile << "Server: webserv" << std::endl;
    outfile << "Date: " << getDate() << std::endl;
    outfile.close();
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

std::string    ResponseBuilder::getDate(void)
{
    time_t my_time;
    time(&my_time);
    return ctime(&my_time);
}