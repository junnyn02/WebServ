#include "ResponseBuilder.hpp"

int main(int ac, char **av)
{
    if (ac != 2)
        return 0;
    ParserRequest get("GET", av[1]);
    ResponseBuilder a(get);
    a.exec();
    return 0;
}