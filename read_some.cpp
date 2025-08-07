#include <boost/asio.hpp>
#include <string>
#include <iostream>

std::string ReadFromSocketDelim(boost::asio::ip::tcp::socket& sock,
                                char delim = '\n')
{
    boost::asio::streambuf buf;
    boost::asio::read_until(sock, buf, delim);

    std::string msg;
    std::istream input_stream(&buf);
    std::getline(input_stream, msg);

    return msg;
}

std::string ReadFromSocket(boost::asio::ip::tcp::socket& sock)
{
    constexpr unsigned char MESSAGE_SIZE = 15; 
    char buf[MESSAGE_SIZE];
    std::size_t totalBytesRead = 0;

    while (totalBytesRead != MESSAGE_SIZE)
    {
        totalBytesRead += sock.read_some(
                                boost::asio::buffer(buf + totalBytesRead,
                                                    MESSAGE_SIZE - totalBytesRead)
                          );
    }

    // OR
    totalBytesRead = boost::asio::read(sock, boost::asio::buffer(buf, MESSAGE_SIZE));

    return std::string{buf, totalBytesRead};
}

int main()
{
    std::string ipAddress = "127.0.0.1";
    std::size_t numPort = 5555;

    try
    {
        boost::asio::ip::tcp::endpoint ep(
            boost::asio::ip::address::from_string(ipAddress),
            numPort
        );

        boost::asio::io_context ioc;
        boost::asio::ip::tcp::socket sock(ioc, ep.protocol());
        sock.connect(ep);

        std::string buf;
        buf = ReadFromSocket(sock);
    }
    catch(const boost::system::system_error& e)
    {
        std::cerr << "Error occurred. Error message: " << e.what()
            << ". Error code: " << e.code();

        return e.code().value();
    }

    return 0;
}