#pragma once
#include<memory>
#include<iostream>
#include<queue>
#include"boost/asio.hpp"
using namespace boost;
using namespace std;
//����Ľ��մ�С
const int RECVSIZE = 1024;
class  MsgNode {
public:
    //д���������캯�������������ĸ�����д�ڵ㣬һ�������ĸ�������ڵ㡣
    //��Ϊ���ͣ�֪�����������Լ��ܳ���
    MsgNode(const char* msg, int total_len) : _total_len(total_len), _cur_len(0) {
        _msg = new char[total_len];
        memcpy(_msg, msg, total_len);
    }
    //��Ϊ���գ�ֻ֪���ܳ���
    MsgNode(int total_len) :_total_len(total_len), _cur_len(0) {
        _msg = new char[total_len];
    }
    ~MsgNode() {
        delete[]_msg;
    }
    //��Ϣ�׵�ַ
    char* _msg;
    //�ܳ���
    int _total_len;
    //��ǰ����
    int _cur_len;
};


class Session
{
public:
    Session(std::shared_ptr<asio::ip::tcp::socket> socket);
    void Connect(const asio::ip::tcp::endpoint& ep);   
    //�첽д�����ַ�ʽ
    void WriteCallBackErr(const boost::system::error_code& ec, std::size_t bytes_transferred, std::shared_ptr<MsgNode>);
    void WriteToSocketErr(const std::string& buf);

    void WriteCallBack(const boost::system::error_code& ec, std::size_t bytes_transferred);
    void WriteToSocket(const std::string& buf);

    void WriteAllCallBack(const boost::system::error_code& ec, std::size_t bytes_transferred);
    void WriteAllToSocket(const std::string& buf);

    //�첽��
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

