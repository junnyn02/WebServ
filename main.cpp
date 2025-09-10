#include "serverCore.hpp"

int	main(int, char**)
{
	serverCore serv;
	serv.connectAndReceive();
	return (0);
}