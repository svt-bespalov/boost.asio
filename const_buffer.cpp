#include <boost/asio.hpp>

void main()
{
    // Шаг 1. Подготовка данных в стандартном контейнере
    std::string buf = "Hello";

    // Шаг 2. Создание представления с помощью asio:buffer()
    boost::asio::const_buffers_1 outputBuffer = boost::asio::buffer(buf);

    // Шаг 3. Теперь outputBuffer можно передать в socket.send(outputBuffer);

}