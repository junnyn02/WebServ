#ifndef CGI_HPP
#define CGI_HPP

#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include "Request.hpp"

std::string execCGI(const Request& request);
void getEnv(const Request& request, std::vector<std::string>& env);
void printConfig(const std::map<std::string, std::string>& config);
/*
- env (char** or map or char* vector)
- av (char ** or char* vector)
- exit status?
- cgi path (string)
- cgi pid (pid_t)
- body (request?)
*/

#endif