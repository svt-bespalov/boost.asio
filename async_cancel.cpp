/*
    1. Разделение обязанностей между потоками:
        - основной поток (main) - управляющий;
        - рабочий поток (worker_thread) - исполнитель.

    2. Инициировать асинхронную операцию подключения
    3. Создать отдельный рабочий поток
    4. В основном потоке имитировать паузу
    5. Основной поток решает отменить операцию подключения
*/

#include <boost/asio.hpp>
#include <thread>
#include <iostream>
#include <utility>
#include <string>
#include <memory>
#include <chrono>

namespace asio = boost::asio;

#define LAMBDA

#ifdef LAMBDA
void Callback(boost::system::error_code ec,
              std::shared_ptr<asio::ip::tcp::socket> sock)
{
    if (ec)
    {
        if (ec == asio::error::operation_aborted)
            std::cerr << "[Worker thread]: Operation cancelled.\n";
        else
        {
            std::cerr << "[Worker thread]: Error occurred. Error code: " << ec.value()
                << ". Message: " << ec.what() << std::endl;
        }

        return;
    }

    std::cout << "[Worker thread]: Connected successfully.\n";
}
#endif // LAMBDA

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
        auto sock = std::make_shared<asio::ip::tcp::socket>(ioc, ep.protocol());

#ifdef LAMBDA
        sock->async_connect(
            ep,
            std::bind(Callback, std::placeholders::_1, sock)
        );
#else
        sock->async_connect(
            ep,
            [sock](boost::system::error_code ec)
            {
                if (ec)
                {
                    if (ec == asio::error::operation_aborted)
                        std::cerr << "[Worker thread]: Operation cancelled.\n";
                    else
                    {
                        std::cerr << "[Worker thread]: Error occurred. Error code: " << ec.value()
                            << ". Message: " << ec.what() << std::endl;
                    }

                    return;
                }

                std::cout << "[Worker thread]: Connected successfully.\n";
            }
        );

#endif // LAMBDA

        auto workerThread = std::thread([&ioc]() {
            try
            {
                std::this_thread::sleep_for(std::chrono::seconds(2));
                ioc.run();
            }
            catch(boost::system::system_error& e)
            {
                std::cerr << "[Worker thread]: Error occurred. Error code: " << e.code()
                    << ". Message: " << e.what() << std::endl;
                
                return;
            }
        });

        // Имитация тяжелой операции (2 сек)
        std::cout << "[Main thread]: waiting 2 sec before cancelling...\n";
        std::this_thread::sleep_for(std::chrono::seconds(2));

        sock->cancel();

        if (workerThread.joinable())
            workerThread.join();
    }
    catch(boost::system::system_error& e)
    {
        std::cerr << "[Main thread]: Error occurred. Error code: " << e.code()
            << ". Message: " << e.what() << std::endl;
        
        return e.code().value();
    }

    return 0;
}