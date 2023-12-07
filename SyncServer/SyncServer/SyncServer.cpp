#include <iostream>
#include <set>
#include <boost/asio.hpp>
#include<memory>
using boost::asio::ip::tcp;
using namespace std;
const int max_length = 1024;
typedef std::shared_ptr<tcp::socket> socket_ptr;
std::set<std::shared_ptr<std::thread>> thread_set;
/*
    **同步读写的劣势：
    *   1 同步读写的缺陷在于读写是阻塞的，如果客户端对端不发送数据服务器的read操作是阻塞的，这将导致服务器处于阻塞等待状态。
        2 可以通过开辟新的线程为新生成的连接处理读写，但是一个进程开辟的线程是有限的，约为2048个线程，在Linux环境可以通过unlimit增加一个进程开辟的线程数，但是线程过多也会导致切换消耗的时间片较多。
        3 该服务器和客户端为应答式，实际场景为全双工通信模式，发送和接收要独立分开。
        4 该服务器和客户端未考虑粘包处理。
    
*/
//处理服务器的读和写
void session(socket_ptr sock) {
    try {
        for (;;) {
            char data[max_length];
            memset(data, '\0', max_length);
            boost::system::error_code error;
            //size_t length = boost::asio::read(sock, boost::asio::buffer(data, max_length), error);
            size_t length = sock->read_some(boost::asio::buffer(data, max_length), error);
            if (error == boost::asio::error::eof) {
                std::cout << "connection closed by peer" << endl;
                break;
            }
            else if (error) {
                throw boost::system::system_error(error);
            }

            cout << "receive from " << sock->remote_endpoint().address().to_string() << endl;
            cout << "receive message is： " << data << endl;
            //将数据回传给对方
            boost::asio::write(*sock, boost::asio::buffer(data, length));
        }
    }
    catch (exception& e) {
        std::cerr << "Exception in thread: " << e.what() << "\n" << std::endl;
    }
}

void server(boost::asio::io_context& io_context, unsigned short port) {
    tcp::acceptor a(io_context, tcp::endpoint(tcp::v4(), port));
    for (;;) {
        socket_ptr socket(new tcp::socket(io_context)); //智能指针
        a.accept(*socket);
        auto t = std::make_shared<std::thread>(session, socket);
        thread_set.insert(t);
    }
}
int main()
{
    try {
        boost::asio::io_context ioc;
        server(ioc, 10086);
        for (auto& t : thread_set) {
            t->join();
        }
    }
    catch (std::exception& e) {
        std::cerr << "Exception: " << e.what() << "\n" << std::endl;
    }
    return 0;
}

