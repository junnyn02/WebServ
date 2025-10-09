#pragma once

#include <iostream>
#include <sys/stat.h>
#include <sys/socket.h>
#include <sys/epoll.h>
#include <fstream>
#include <fcntl.h>
#include <ctime>
#include <csignal>
#include <cstring>
#include <cstdlib>
#include <map>
#include <sstream>
#include <unistd.h>
#include <dirent.h>
#include <netinet/in.h>
#include <iterator>
#include <vector>
#include <limits>

std::string size_t_to_string(size_t n);


#define BLACK   "\033[30m"
#define RED     "\033[31m"
#define GREEN   "\033[32m"
#define YELLOW  "\033[33m"
#define BLUE    "\033[34m"
#define MAGENTA "\033[35m"
#define CYAN    "\033[36m"
#define WHITE   "\033[37m"
#define ORANGE "\033[38;5;208m"
#define VIOLET "\033[38;5;129m"
#define PINK   "\033[38;5;200m"
#define HOTPINK "\033[38;2;255;105;180m"
#define SKYBLUE "\033[38;2;135;206;235m"
#define LIME    "\033[38;2;50;205;50m"

#define BOLD     "\033[1m"
#define RESET   "\033[0m"