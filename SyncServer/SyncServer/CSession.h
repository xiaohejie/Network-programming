#include <iostream>
#include <boost/asio.hpp>
#include <map>
#include<queue>
#include <boost/uuid/uuid_generators.hpp>
#include <boost/uuid/uuid_io.hpp>
using boost::asio::ip::tcp;
using namespace std;
class CServer;
//数据节点设计
class MsgNode
{
    friend class CSession;
public:
    MsgNode(char* msg, int max_len) {
        _data = new char[max_len];
        memcpy(_data, msg, max_len);
    }
    ~MsgNode() {
        delete[] _data;
    }
private:
    int _cur_len;
    int _max_len;
    char* _data;
};

class CSession :public std::enable_shared_from_this<CSession>
{
public:
    CSession(boost::asio::io_context& ioc, CServer* server) :_socket(ioc), _server(server) {
        boost::uuids::uuid  a_uuid = boost::uuids::random_generator()();
        _uuid = boost::uuids::to_string(a_uuid);
    }
    tcp::socket& Socket() {
        return _socket;
    }
    ~CSession() {
        std::cout << "session destruct delete this " << this << endl;
    }
	void Send(char* msg, int max_length);
    void Start();
    std::string& GetUuid();
private:
    void handle_read(const boost::system::error_code& error,
        size_t bytes_transferred, shared_ptr<CSession> _self_shared);
    void handle_write(const boost::system::error_code& error, shared_ptr<CSession> _self_shared);
    tcp::socket _socket;
    enum { max_length = 1024 };
    char _data[max_length];
    CServer* _server;
    std::string _uuid;
	std::queue<shared_ptr<MsgNode> > _send_que;
	std::mutex _send_lock;
};

