#pragma once

#include <cstring>
#include <cstdlib>
#include <iostream>
#include <netinet/in.h>
#include <sys/socket.h>
#include <unistd.h>

#define EXIT_FAILURE 1

class serverCore
{
private:

public:
	serverCore(/* args */) {}
	~serverCore() {}
};


int	connectAndReceive();