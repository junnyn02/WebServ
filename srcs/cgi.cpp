#include "cgi.hpp"

char* getScript(const std::string& uri) //change return type
{
	//get script name/path + path_info
}

void getEnv(const Request& request, std::vector<std::string>& env)
{
	env.push_back("REQUEST_METHOD=" + request.getMethod());
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
	//HTTP_* headers?
}

void execCGI(const Request& request)
{
	pid_t cgi;
	int input[2];					//child process input
	int output[2];					//child process output

	if (pipe(input) == -1|| pipe(output) == -1)
	{
		//set status to 500 or exit program?
	}
	cgi = fork();
	if (cgi == -1)
	{
		//set status to 500 or exit program?
	}
	else if (cgi == 0)
	{
		close(input[1]);
		close(output[0]);
		dup2(input[0], STDIN_FILENO);			//receive input from parent
		dup2(output[1], STDOUT_FILENO);			//send output to parent
		close(input[0]);
		close(output[1]);
		
		char *interpreter = "/usr/bin/php-cgi";	//path to interpreter
		
		std::vector <char *> arg;				//path to interpreter, path to script, NULL
		char *scriptname = getScript(request.getURI());
		arg.push_back("/usr/bin/php-cgi");
		arg.push_back("/cgi-bin/register.php");	//change
		arg.push_back("\0");

		std::vector<std::string> env;
		getEnv(request, env);
		std::vector<char *> envp;
		for (size_t i = 0; i < env.size(); i++)
			envp.push_back(const_cast<char *>(env[i].c_str()));
		envp.push_back(NULL);
		execve(interpreter, &arg[0], &envp[0]);
	}
	else
	{
		close(input[0]);
		close(output[1]);
		if (request.getMethod() == "POST")
		{
			//write body to input[1];
		}
		close(input[1]);
		//read output from output[0];
		close(output[0]);
	}
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