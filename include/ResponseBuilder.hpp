#pragma once

#include "utils.hpp"
#include "Request.hpp"
#include "serverCore.hpp"
#include "Config.hpp"
#include "Location.hpp"

class ResponseBuilder
{
    private:
        std::string                         _code;
        Request                             _request;
        std::map<std::string, std::string>  _mime;
        std::vector<std::string>            _gallery;
        std::string                         _type;
        std::string                         _body;
        std::string                         _filename;
        std::string                         _header;
        Server*                             _server;
        std::vector<Location*>              _location;

    public:
        ResponseBuilder(const Request &);
        ~ResponseBuilder(void);
    
        void                    tryGet(void);
        void                    buildListUploads(void);
        void                    tryPost(void);
        void                    tryDelete(void);
        void                    isDir(const std::string &);
        void                    isFile(const std::string &);
        void                    sendResponse(void);
        
        void                    setError(const std::string &);
        void                    checkErrorPage(void);
        
        bool                    checkMime(void);        
        bool                    createFile(void);

        const std::string       build204(void);
        
        const std::string       getCode(void) const;
        const std::string       getDate(void) const;
        const std::string       getType(void) const;
        const std::string       getDir(void) const;
        const std::string       getHeader(void) const;
        const std::string       getBody(void) const;
        const std::string       setBody(void) const;
};