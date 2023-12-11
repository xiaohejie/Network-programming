#include <boost/asio.hpp>
#include <iostream>
using namespace boost::asio::ip;
using namespace std;
const int MAX_LENGTH = 1024;    //发送和接收的最大长度
int main()
{
    try {
        //创建上下文服务
        boost::asio::io_context ioc;
        //构造endpoint
        tcp::endpoint remote_ep(address::from_string("127.0.0.1"), 3333);
        //创建socket
        tcp::socket sock(ioc);
        //创建默认错误
        boost::system::error_code error = boost::asio::error::host_not_found;
        //连接
        sock.connect(remote_ep, error);
        if (error) {
            cout << "connect failed, code is " << error.value() << " error msg is " << error.message();
            return 0;
        }
        
        //发送信息给服务器
        std::cout << "Enter message: ";
        char request[MAX_LENGTH];
        std::cin.getline(request, MAX_LENGTH);
        size_t request_length = strlen(request);
        boost::asio::write(sock, boost::asio::buffer(request, request_length));

        //接收服务器传回来的信息
        char reply[MAX_LENGTH];
        size_t reply_length = boost::asio::read(sock, boost::asio::buffer(reply, request_length));
        std::cout << "Reply is: ";
        std::cout.write(reply, reply_length);
        std::cout << "\n";
    }
    catch (std::exception& e) {
        std::cerr << "Exception: " << e.what() << std::endl; 
    }
    return 0; 
}
