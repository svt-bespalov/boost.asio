#include <boost/asio.hpp>
#include <memory>
#include <utility>
#include <string>
#include <iostream>

namespace asio = boost::asio;

struct Session
{
    std::shared_ptr<asio::ip::tcp::socket> sock_;
    std::string buf_;
    std::size_t totalBytesRead_;
};

void ReadFromSocket(asio::ip::tcp::socket&& sock)
{
    // 1. Создание сессии для хранения состояния
    auto sess = std::make_shared<Session>();

    // 2. Подготовка буфера для приема данных
    constexpr std::size_t BUF_SIZE = 18;
    sess->sock_ = std::make_shared<asio::ip::tcp::socket>(std::move(sock));
    sess->totalBytesRead_ = 0;
    sess->buf_.reserve(BUF_SIZE);

    // 3. Запуск асинхронной операции чтения
    asio::async_read(
        *sess->sock_,
        asio::buffer(sess->buf_),
        std::bind(
            [](boost::system::error_code ec,
                std::size_t bytesReceived,
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

        ReadFromSocket(std::move(sock));

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