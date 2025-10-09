#include "cgi.hpp"

void execCGI(const Request& request)
{
	std::vector<std::string> env;
	env.push_back("REQUEST_METHOD=");
	env.push_back("SCRIPT_NAME=");
	env.push_back("GATEWAY_INTERFACE=CGI/1.1");
	env.push_back("REMOTE_ADDR=");
	env.push_back("SERVER_NAME=");
	env.push_back("SERVER_PORT=8080");
	env.push_back("SERVER_PROTOCOL=HTTP/1.1");
	env.push_back("SERVER_SOFTWARE=Webserv/1.0");
	if (request.getMethod() == "POST")		//idk
	{
		env.push_back("CONTENT_LENGTH=" + request.getSize());	//opt
		env.push_back("CONTENT_TYPE=" + request.getType());		//opt
	}
	env.push_back("QUERY_STRING=");		//opt
	env.push_back("PATH_INFO=");		//opt
	env.push_back("PATH_TRANSLATED=");	//opt
}

/*
PI cgi
- get request target
- get script path
- pipe (2 fd + 2 in)
- fork
- child :
	- close read fd[0] and write in[1]
	- dup output to fd[1] and input to in[0]
	- close extra fds
	- get content length and type + query string
	- get cookies (bonus)
	- create env string vector, pushback meta variables
	- create char* vector pushback env
	- create char* argv vector, pushback interpreter (convert extension to path) 
	- pushback scriptpath
	- execve scriptpath or interpreter, av[0] and env[0]
- parent :
	- close pipes
	- if post:
		- get body from request
		- write body to pipein[1]
	- close pipein[1]
	- waitpid for child exit status
	- read output from pipe using buffer
	- close pipefd[0]
	- create ostringstream, insert strlen(buffer)
	- create response string with status + headers + buffer string
	- return response string.c_str()

cclaude cgi:
- get env in cstd array
- save stdin and out
- write request body to fdin
- fork
- in child :
	- dup in and out, exec script name with env, write output to pipe
- in parent :
	- waitpid
	- read buffer in loop, append to new body string
	- restore stdin and out
	- close everything left
	- delete env array
	- return new body
*/