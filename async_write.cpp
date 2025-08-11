#include <boost/asio.hpp>
#include <iostream>
#include <string>
#include <utility>
#include <memory>

// #define CALLBACK

namespace asio = boost::asio;

struct Session
{
    std::string buf_;
    std::shared_ptr<asio::ip::tcp::socket> sock_;
};


#ifdef CALLBACK
void Callback(boost::system::error_code ec,
              std::size_t bytesTransferred,
              std::shared_ptr<Session> sess)
{
    // 1. Проверка на ошибки
    if (ec.value() != 0)
    {
        std::cerr << "Error occurred. Error code: " << ec.value()
            << ". Message: " << ec.what() << std::endl;

        return;
    }
}
#endif // CALLBACK

void WriteToSocket(asio::ip::tcp::socket&& sock)
{
    // 1. Создание объекта Session
    auto sess = std::make_shared<Session>();

    // 2. Инициализация объекта Session
    sess->buf_ = "Hello Async World";
    sess->sock_ = std::make_shared<asio::ip::tcp::socket>(std::move(sock));

    // 3. Вызов асинхронной записи
#ifdef CALLBACK
    asio::async_write(
        *sess->sock_,
        asio::buffer(sess->buf_),
        std::bind(
            Callback,
            std::placeholders::_1,
            std::placeholders::_2,
            sess
        )
    );
#else 
    asio::async_write(
        *sess->sock_,
        asio::buffer(sess->buf_),
        std::bind(
            [](boost::system::error_code ec,
               std::size_t bytesTransferred,
               std::shared_ptr<Session> sess)
            {
                // 1. Проверка на ошибки
                if (ec.value() != 0)
                {
                    std::cerr << "Error occurred. Error code: " << ec.value()
                        << ". Message: " << ec.what() << std::endl;

                    return;
                }
            },
            std::placeholders::_1,
            std::placeholders::_2,
            sess
        )
    );
#endif // CALLBACK

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
        asio::ip::tcp::socket socket(ioc, ep.protocol());
        socket.connect(ep);

        WriteToSocket(std::move(socket));

        ioc.run();
    }
    catch(boost::system::system_error& e)
    {
        std::cerr << "Error occurred. Error code: " << e.code()
            << ". Message: " << e.what() << std::endl;

        return e.code().value();
    }

    return 0;
}