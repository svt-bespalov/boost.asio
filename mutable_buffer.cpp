#include <boost/asio.hpp>
#include <memory>

void main()
{
    // Step 1. Выделение изменяемой памяти (оборачиваем в unique_ptr)
    constexpr size_t BUF_SIZE_BYTES = 128;
    std::unique_ptr<char[]> buf(new char[BUF_SIZE_BYTES]);

    // Step 2. Создание представления с помощью boost::asio::buffer
    boost::asio::mutable_buffers_1 inputBuffer = boost::asio::buffer(
                                                        static_cast<void*>(buf.get()),
                                                        BUF_SIZE_BYTES
                                                 );
                                                
    // Step 3. Теперь inputBuffer можно передать в socket.receive(inputBuffer);
}