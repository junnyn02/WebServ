#pragma once

#include <iostream>

class ParserRequest
{
    private:
        std::string _method;
        std::string _path;
    
    public:
        ParserRequest(const std::string &, const std::string &);
        ~ParserRequest( void );
        std::string getMethod(void) const;
        std::string getPath(void) const;
};