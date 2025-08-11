#include <boost/asio.hpp>
#include <iostream>
#include <memory>
#include <utility>
#include <string>

namespace asio = boost::asio;

struct Session
{
    std::shared_ptr<asio::ip::tcp::socket> sock_;
    std::unique_ptr<char[]> buf_;
    std::size_t bufLength_;
    std::size_t totalBytesRead_;
};

void Callback(boost::system::error_code ec,
              std::size_t bytesReceived,
              std::shared_ptr<Session> sess)
{
    // 1. Проверка ошибки
    if (ec.value() != 0)
    {
        std::cerr << "Error occurred! Error code: " << ec.value()
            << ". Message: " << ec.what() << std::endl;

        return;
    }

    // 2. Обновления состояния
    sess->totalBytesRead_ += bytesReceived;

    // 3. Проверка завершения
    if (sess->totalBytesRead_ < sess->bufLength_)
    {
        // 4. Продолжение цепочки
        sess->sock_->async_read_some(
            asio::buffer(
                sess->buf_.get() + sess->totalBytesRead_,
                sess->bufLength_ - sess->totalBytesRead_
            ),
            std::bind(
                Callback,
                std::placeholders::_1,
                std::placeholders::_2,
                sess
            )
        );
    }
}

void ReadFromSocket(asio::ip::tcp::socket&& sock)
{
    // 1. Создание сессии для хранения состояния
    auto sess = std::make_shared<Session>();

    // 2. Подготовка буфера для приема данных
    constexpr std::size_t BUF_SIZE = 18;
    sess->sock_ = std::make_shared<asio::ip::tcp::socket>(std::move(sock));
    sess->totalBytesRead_ = 0;
    sess->buf_.reset(new char[BUF_SIZE]);
    sess->bufLength_ = BUF_SIZE;

    // 3. Запуск первой асинхронной операции чтения
    sess->sock_->async_read_some(
        asio::buffer(
            sess->buf_.get(),
            sess->bufLength_
        ),
        std::bind(
            Callback,
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
        std::cerr << "Error occurred! Error code: " << e.code()
            << ". Message: " << e.what() << std::endl;
        
        return e.code().value();
    }

    return 0;
}