#pragma once

#include <iostream>

#ifndef MSG
# define MSG 1
#endif

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