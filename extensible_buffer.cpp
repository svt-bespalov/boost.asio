#include <boost/asio.hpp>
#include <string>
#include <iostream>

int main()
{
    boost::asio::streambuf buf;
    std::ostream output(&buf);

    output << "Message1\nMessage2";

    std::istream input(&buf);
    std::string str;

    std::getline(input, str);

    std::cout << str << std::endl;

    return 0;
}