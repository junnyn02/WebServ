#include "utils.hpp"

std::string size_t_to_string(size_t n) 
{
    std::ostringstream oss;
    oss << n;
    return oss.str();
}