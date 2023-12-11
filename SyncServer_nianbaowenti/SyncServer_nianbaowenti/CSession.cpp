#include "CSession.h"
#include "CServer.h"
#include<iostream>
using namespace std;
void CSession::Send(char* msg, int max_length)
{
	bool pending = false;	//这里的pending为true表示上一次数据没发完，队列里还有数据
	std::lock_guard<std::mutex> lock(_send_lock);
	if (_send_que.size() > 0) {
		pending = true;
	}
	_send_que.push(make_shared<MsgNode>(msg, max_length));
	if (pending) {
		return;
	}
	boost::asio::async_write(_socket, boost::asio::buffer(msg, max_length),
		std::bind(&CSession::handle_write, this, std::placeholders::_1, shared_from_this()));
}
void CSession::Start() {
	memset(_data, 0, max_length);
	_socket.async_read_some(boost::asio::buffer(_data, max_length),
		std::bind(&CSession::handle_read, this, std::placeholders::_1, std::placeholders::_2, shared_from_this()));
}

//访问uuid
std::string& CSession::GetUuid() {
	return _uuid;
}

//读和写的回调函数
void CSession::handle_read(const boost::system::error_code& error, size_t bytes_transferred, shared_ptr<CSession> _self_shared) {
	if (!error) {
		cout << "read data is " << _data << endl;
		//发送数据
		Send(_data, bytes_transferred);
		memset(_data, 0, max_length);
		_socket.async_read_some(boost::asio::buffer(_data, max_length), std::bind(&CSession::max_length, this,
			std::placeholders::_1, std::placeholders::_2, _self_shared));
	}
	else { 
		cout << "read error" << error.value() << endl;
		//delete this; 
		_server->ClearSession(_uuid);
	}
}
void CSession::handle_write(const boost::system::error_code& error, shared_ptr<CSession> _self_shared) {
	if (!error) {
		std::lock_guard<std::mutex> lock(_send_lock);
		_send_que.pop();
		if (!_send_que.empty()) {
			auto& msgnode = _send_que.front();
			boost::asio::async_write(_socket, boost::asio::buffer(msgnode->_data, msgnode->_max_len),
				std::bind(&CSession::handle_write, this, std::placeholders::_1, _self_shared));
		}
	}
	else {
		cout << "write error" << error.value() << endl;
		//delete this;
		_server->ClearSession(_uuid);
	}
}