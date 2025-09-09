#include "ResponseBuilder.hpp"

int main()
{
    ParserRequest get("GET", "srcs/index.html");
    ResponseBuilder a(get);
    a.exec();
    return 0;
}