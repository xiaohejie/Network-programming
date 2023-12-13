#pragma once
#include<iostream>
#include<boost/asio.hpp>
#include<map>
#include<memory>
#include<boost/uuid/uuid_generators.hpp>
#include<boost/uuid/uuid_io.hpp>
using boost::asio::ip::tcp;
using namespace std;
class Server;
class Session:public std::enable_shared_from_this<Session>
{
public:
	//socket��������ioc
	Session(boost::asio::io_context& ioc, Server* server) :_socket(ioc), _server(server) {
		boost::uuids::uuid a_uuid = boost::uuids::random_generator()();	//����uuid
		_uuid = boost::uuids::to_string(a_uuid);	//ת���ַ�������
	}
	//��ȡsocket
	tcp::socket& Socket() {
		return _socket;
	}
	//�����ͻ��˵Ķ���д
	void Start();
	//����uuid
	std::string& GetUuid();
private:
	//����д�Ļص�����
	void handle_read(const boost::system::error_code& error,
		size_t bytes_transferred, shared_ptr<Session> _self_shared);
	void handle_write(const boost::system::error_code& error, shared_ptr<Session> _self_shared);

	tcp::socket _socket;
	enum {max_length = 1024};
	char _data[max_length];
	Server* _server;
	std::string _uuid;
};

class Server {
public:
	Server(boost::asio::io_context& ioc, short port);
	void ClearSession(std::string uuid);	//��session��map���Ƴ�
private:
	void start_accept();
	void handle_accept(shared_ptr<Session> new_session, const boost::system::error_code& error);
	boost::asio::io_context& _ioc;
	tcp::acceptor _acceptor;
	std::map<std::string, shared_ptr<Session>> _sessions;
};

