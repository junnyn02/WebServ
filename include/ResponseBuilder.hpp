#pragma once

#include "utils.hpp"
#include "Request.hpp"

class ResponseBuilder
{
    private:
        std::string                         _code;
        Request                             _request;
        std::map<std::string, std::string>  _mime;
        std::string                         _type;

    public:
        ResponseBuilder(const Request &);
        ~ResponseBuilder(void);
        
        const std::string   exec(void);
        void                tryGet(void);
        void                isDir(const std::string &);
        void                isFile(const std::string &);
        
        const std::string   getCode(void) const;
        const std::string   getDate(void) const;
        const std::string   getType(void) const;
        const std::string   getBody(void) const;
        const std::string   getDir(void) const;
};