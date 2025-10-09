#ifndef CGI_HPP
#define CGI_HPP

#include <unistd.h>
#include "Request.hpp"

void execCGI(const Request& request);
void getEnv(const Request& request, std::vector<std::string>& env);

/*
- env (char** or map or char* vector)
- av (char ** or char* vector)
- exit status?
- cgi path (string)
- cgi pid (pid_t)
- body (request?)
*/

#endif