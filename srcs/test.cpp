#include "test.hpp"

ParserRequest::ParserRequest(const std::string &method, const std::string &path) : _method(method), _path(path)
{
}

ParserRequest::~ParserRequest(){
}

std::string ParserRequest::getMethod(void) const
{
    return (this->_method);
}

std::string ParserRequest::getPath(void) const
{
    return (this->_path);
}