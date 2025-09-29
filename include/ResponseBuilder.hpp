#pragma once

#include "utils.hpp"
#include "Request.hpp"
#include "serverCore.hpp"

class ResponseBuilder
{
    private:
        std::string                         _code;
        Request                             _request;
        std::map<std::string, std::string>  _mime;
        std::string                         _type;
        std::string                         _body;

    public:
        ResponseBuilder(const Request &);
        ~ResponseBuilder(void);
        
        void                tryGet(void);
        void                tryPost(void);
        void                isDir(const std::string &);
        void                isFile(const std::string &);
        
        bool                checkMime(void);        

        const std::string   sendResponse(void);
        const std::string   getCode(void) const;
        const std::string   getDate(void) const;
        const std::string   getType(void) const;
        const std::string   getBody(void) const;
        const std::string   getDir(void) const;
};