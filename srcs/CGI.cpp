#include "../include/CGI.hpp"

std::string getPathInfo(const std::string& uri)
{
	size_t pos = uri.find(".php");
	pos += 4;
	if (pos + 1 != uri.length())
	{
		std::string convert = uri.substr(pos, uri.length() - pos);
		return convert;
	}
	else
		return NULL;
}

std::string getScriptName(const std::string& uri)
{
	size_t pos = uri.find(".php");
	pos += 4;
	std::string name = uri.substr(0, pos);
	return name;
}

void printEnv(std::vector<std::string>& env)
{
	std::vector<std::string>::iterator it = env.begin();
	while (it != env.end())
	{
		std::cout << *it << std::endl;
		it++;
	}
}

void getEnv(const Request& request, std::vector<std::string>& env)
{
	printConfig(request.getInfo());
	std::map<std::string, std::string> config = request.getInfo();
	env.push_back("REQUEST_METHOD=" + request.getMethod());
	std::string name = getScriptName(request.getURI());
	env.push_back("SCRIPT_NAME=" + name);
	std::map<std::string, std::string>::iterator it = config.find("root");			//will probably change to alias
	env.push_back("SCRIPT_FILENAME=" + it->second + name);							//root + filename -> conf
	env.push_back("GATEWAY_INTERFACE=CGI/1.1");
	env.push_back("REMOTE_ADDR=127.0.0.1");					//check
	env.push_back("SERVER_NAME=localhost");					//check config
	env.push_back("SERVER_PORT=8080");
	env.push_back("SERVER_PROTOCOL=HTTP/1.1");
	env.push_back("SERVER_SOFTWARE=Webserv/1.0");
	env.push_back("REDIRECT_STATUS=200");
	if (request.getMethod() == "POST")		//idk
	{
		std::stringstream convert;
		std::string size;
		convert << request.getSize();
		convert >> size;
		env.push_back("CONTENT_LENGTH=" + size);				//opt
		env.push_back("CONTENT_TYPE=" + request.getType());		//opt
	}
	env.push_back("QUERY_STRING=" + request.getQuery());								//opt
	std::string path_info = getPathInfo(request.getURI());
	env.push_back("PATH_INFO=" + path_info);					//opt
	env.push_back("PATH_TRANSLATED=");							//opt
	//HTTP_* headers?
	//printEnv(env);
}

std::string execCGI(const Request& request)						//define CGI timeout?
{
	pid_t cgi;
	int input[2];											//child process input
	int output[2];											//child process output

	if (pipe(input) == -1|| pipe(output) == -1)
	{
		//set status to 500 or exit program?
		return "";
	}
	cgi = fork();
	if (cgi == -1)
	{
		//set status to 500 or exit program?
		return "";
	}
	else if (cgi == 0)
	{
		close(input[1]);
		close(output[0]);
		dup2(input[0], STDIN_FILENO);						//receive input from parent
		dup2(output[1], STDOUT_FILENO);						//send output to parent
		close(input[0]);
		close(output[1]);
		
		std::vector <char *> arg;							//path to interpreter, path to script, NULL
		std::string wtf = "/usr/bin/php-cgi";
		char *interpreter = &wtf[0];
		arg.push_back(interpreter);							//path to interpreter
		std::string idk = getScriptName(request.getURI());
		char *name = &idk[0];
		arg.push_back(name);
		arg.push_back(NULL);

		std::vector<std::string> env;
		getEnv(request, env);
		std::vector<char *> envp;
		for (size_t i = 0; i < env.size(); i++)
			envp.push_back(const_cast<char *>(env[i].c_str()));
		envp.push_back(NULL);
		execve(interpreter, &arg[0], &envp[0]);
		exit(1);
	}
	else
	{
		close(input[0]);
		close(output[1]);
		if (request.getMethod() == "POST" && request.getSize() > 0)
		{
			std::string body = request.getBody();
			write(input[1], body.c_str(), body.size());
		}
		close(input[1]);
		int status;
		pid_t result = waitpid(cgi, &status, WUNTRACED);		//might change flag later
		if (result < 0)
		{
			close(output[0]);
			std::cerr << "CGI error\n";
			return "";
		}
		else
		{
			char buffer[8192] = {0};								//why?
			size_t bytes = read(output[0], buffer, sizeof(buffer) - 1);	// why - 1? remove \0?
			close(output[0]);
			if (bytes == 0)
			{
				std::cerr << "Nothing in buffer\n";
				return "";
			}
			if (bytes > 0)
			{
				std::ostringstream convert;
				std::stringstream to_string;
				std::string size;
				to_string << bytes;
				to_string >> size;
				convert << buffer;
				std::string response = 
				"HTTP/1.1 200 OK\r\nContent-Length: " + size + "\r\n"
				+ convert.str() + "\r\n\r\n";
				//std::cout << response << std::endl;
				return response;
			}
		}
	}
	return "";
}