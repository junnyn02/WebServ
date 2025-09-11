#include "include/Request.hpp"

int main()
{
	Request test;
	std::string request = "GET /index.html HTTP/1.1\r\n test";
	test.fillRequest(request);
	test.printRequest();
	return 0;
}