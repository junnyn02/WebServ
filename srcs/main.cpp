#include "../include/Request.hpp"

int main()
{
	Request test;
	std::string request = "GET /index.html HTTP/1.1\r\n test idk \r\nContent-Type: HTML\r\nContent-Length:1200\r\n\r\n";
	test.fillRequest(request);
	test.printRequest();
	return 0;
}