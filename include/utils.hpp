#pragma once

#include <iostream>
#include <sys/stat.h>
#include <sys/socket.h>
#include <sys/epoll.h>
#include <fstream>
#include <fcntl.h>
#include <ctime>
#include <cstring>
#include <cstdlib>
#include <map>
#include <sstream>
#include <unistd.h>
#include <dirent.h>
#include <netinet/in.h>

std::string size_t_to_string(size_t n);