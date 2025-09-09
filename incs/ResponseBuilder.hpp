#pragma once

#ifndef MSG
# define MSG 1
#endif

#include <iostream>
#include <sys/stat.h>
#include <fstream>
#include <ctime>
#include "test.hpp"
class ResponseBuilder
{
    private:
        std::string     _code;
        ParserRequest   _request;

    public:
        ResponseBuilder(const ParserRequest &);
        ~ResponseBuilder(void);

        std::string getCode(void) const;
        std::string buildResponse(void);

        void            exec(void);
        void            tryGet(void);
        std::string     getDate(void);
};