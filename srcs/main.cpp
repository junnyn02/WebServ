#include "ResponseBuilder.hpp"

int main(int ac, char **av)
{
    (void)ac;
    ParserRequest get("GET", av[1]);
    ResponseBuilder a(get);
    a.exec();
    return 0;
}