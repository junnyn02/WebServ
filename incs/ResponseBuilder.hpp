#pragma once

#include "utils.hpp"
#include "test.hpp"
class ResponseBuilder
{
    private:
        std::string                         _code;
        ParserRequest                       _request;
        std::map<std::string, std::string>  _mime;
        std::string                         _type;

    public:
        ResponseBuilder(const ParserRequest &);
        ~ResponseBuilder(void);

        std::string getCode(void) const;
        std::string buildResponse(void);

        void                exec(void);
        void                tryGet(void);
        const std::string   getDate(void) const;
        const std::string   getType(void) const;
        const std::string   getBody(void) const;
};