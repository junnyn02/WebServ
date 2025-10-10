#include "include/CGI.hpp"

int main()
{
	std::string POST = "GET /register.php?first_name=Gael&last_name=Mariani&login=lemarian&consent=on HTTP/1.1\r\nHost: localhost:8080\r\nUser-Agent: Mozilla/5.0 (X11; Ubuntu; Linux x86_64; rv:128.0) Gecko/20100101 Firefox/128.0\r\nAccept: text/html,application/xhtml+xml,application/xml;q=0.9,*/*;q=0.8\r\nAccept-Language: en-US,en;q=0.5\r\nAccept-Encoding: gzip, deflate, br, zstd\r\nConnection: keep-alive\r\nReferer: http://localhost:8080/register.html\r\nUpgrade-Insecure-Requests: 1\r\nSec-Fetch-Dest: document\r\nSec-Fetch-Mode: navigate\r\nSec-Fetch-Site: same-origin\r\nSec-Fetch-User: ?1\r\nPriority: u=0, i\r\n\r\n";
	Request test;
	test.fillRequest(POST, POST.size());
	test.printRequest();
	execCGI(test);
	return 0;
}