#include <boost/asio.hpp>
#include <string>
#include <iostream>


void WriteToSocket(boost::asio::ip::tcp::socket& sock)
{
    std::string buf = "HelloBuf";
    std::size_t totalBytesWritten = 0;

    while (totalBytesWritten != buf.length())
    {
        totalBytesWritten += sock.write_some(
                                    boost::asio::buffer(buf.c_str() + totalBytesWritten,
                                                        buf.length() - totalBytesWritten)
                             );
    }

    // OR 
    
    // write() обертка над write_some() с циклом
    boost::asio::write(sock, boost::asio::buffer(buf));
}

int main()
{
    std::string rawIpAddress = "127.0.0.1";
    unsigned short portNum = 5555;

    try
    {
        boost::asio::ip::tcp::endpoint ep(
            boost::asio::ip::address::from_string(rawIpAddress),
            portNum
        );

        boost::asio::io_context ioc;
        boost::asio::ip::tcp::socket sock(ioc, ep.protocol());

        sock.connect(ep);

        WriteToSocket(sock);
    }
    catch(boost::system::system_error& e)
    {
        std::cerr << "Error occurred! Error code: " << e.code()
            << ". Message: " << e.what();

        return e.code().value();
    }

    return 0;
}