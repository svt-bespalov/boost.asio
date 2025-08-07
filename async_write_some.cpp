#include <boost/asio.hpp>
#include <string>
#include <iostream>
#include <memory>
#include <utility>

namespace asio = boost::asio;

struct Session
{
    std::string buf_;
    std::size_t totalBytesWritten_;
    std::shared_ptr<asio::ip::tcp::socket> sock_ = nullptr;
};

void Callback(boost::system::error_code ec,
            std::size_t bytesTransferred,
            std::shared_ptr<Session> sess)
{
    // 1. Проверка на ошибки
    if (ec.value() != 0)
    {
        std::cerr << "Error occurred. Error code: " << ec.value()
            << ". Error message: " << ec.message() << std::endl;

        return;
    }

    // 2. Обновление состояния
    sess->totalBytesWritten_ += bytesTransferred;

    // 3. Проверка завершения
    if (sess->totalBytesWritten_ < sess->buf_.length())
    {
        // 4. Продолжение цепочки
        sess->sock_->async_write_some(
            asio::buffer(sess->buf_.c_str() + sess->totalBytesWritten_,
                         sess->buf_.length() - sess->totalBytesWritten_),
            std::bind(Callback, std::placeholders::_1, std::placeholders::_2, sess)
        );
    }
}

void WriteToSocket(asio::ip::tcp::socket&& sock)
{
    auto sess = std::make_shared<Session>();
    sess->buf_ = "Hello Async World!";
    sess->totalBytesWritten_ = 0;
    sess->sock_ = std::make_shared<asio::ip::tcp::socket>(std::move(sock));

    sess->sock_->async_write_some(
                 asio::buffer(sess->buf_),
                 std::bind(Callback, std::placeholders::_1, std::placeholders::_2, sess)
    );
}

int main()
{
    std::string ipAddress = "127.0.0.1";
    std::size_t portNum = 5555;

    try
    {
        asio::ip::tcp::endpoint ep(
            asio::ip::address::from_string(ipAddress),
            portNum
        );

        asio::io_context ioc;
        asio::ip::tcp::socket sock(ioc, ep.protocol());
        sock.connect(ep);

        WriteToSocket(std::move(sock));

        ioc.run();
    }
    catch(boost::system::system_error& e)
    {
        std::cerr << "Error occurred. Error code: " << e.code()
            << ". Error message: " << e.what() << std::endl;
        
        return e.code().value();
    }

    return 0;
}