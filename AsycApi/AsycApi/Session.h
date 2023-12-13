#pragma once
#include<memory>
#include<iostream>
#include<queue>
#include"boost/asio.hpp"
using namespace boost;
using namespace std;
//最大报文接收大小
const int RECVSIZE = 1024;
class  MsgNode {
public:
    //写了两个构造函数，两个参数的负责构造写节点，一个参数的负责构造读节点。
    //作为发送，知道发送数据以及总长度
    MsgNode(const char* msg, int total_len) : _total_len(total_len), _cur_len(0) {
        _msg = new char[total_len];
        memcpy(_msg, msg, total_len);
    }
    //作为接收，只知道总长度
    MsgNode(int total_len) :_total_len(total_len), _cur_len(0) {
        _msg = new char[total_len];
    }
    ~MsgNode() {
        delete[]_msg;
    }
    //消息首地址
    char* _msg;
    //总长度
    int _total_len;
    //当前长度
    int _cur_len;
};


class Session
{
public:
    Session(std::shared_ptr<asio::ip::tcp::socket> socket);
    void Connect(const asio::ip::tcp::endpoint& ep);   
    //异步写的三种方式
    void WriteCallBackErr(const boost::system::error_code& ec, std::size_t bytes_transferred, std::shared_ptr<MsgNode>);
    void WriteToSocketErr(const std::string& buf);

    void WriteCallBack(const boost::system::error_code& ec, std::size_t bytes_transferred);
    void WriteToSocket(const std::string& buf);

    void WriteAllCallBack(const boost::system::error_code& ec, std::size_t bytes_transferred);
    void WriteAllToSocket(const std::string& buf);

    //异步读
    void ReadFromSocket();
    void ReadCallBack(const boost::system::error_code& ec, std::size_t bytes_transferred);

    void ReadAllFromSocket();
    void ReadAllCallBack(const boost::system::error_code& ec, std::size_t bytes_transferred);
private:
    std::shared_ptr<asio::ip::tcp::socket> _socket;
    std::shared_ptr<MsgNode> _send_node;
    std::queue<std::shared_ptr<MsgNode>> _send_queue;
    bool _send_pending;

    std::shared_ptr<MsgNode> _recv_node;
    bool _recv_pending;
};

