#include "CSession.h"
#include "CServer.h"
#include <iostream>
#include "msg.pb.h"
#include <sstream>

CSession::CSession(boost::asio::io_context& io_context, CServer* server):
	_socket(io_context), _server(server), _b_close(false),_b_head_parse(false){
	boost::uuids::uuid  a_uuid = boost::uuids::random_generator()();
	_uuid = boost::uuids::to_string(a_uuid);
	_recv_head_node = make_shared<MsgNode>(HEAD_LENGTH);
}
CSession::~CSession() {
	std::cout << "~CSession destruct" << endl;
}

tcp::socket& CSession::GetSocket() {
	return _socket;
}

std::string& CSession::GetUuid() {
	return _uuid;
}

void CSession::Start(){
	::memset(_data, 0, MAX_LENGTH);
	_socket.async_read_some(boost::asio::buffer(_data, MAX_LENGTH), std::bind(&CSession::HandleRead, this, 
		std::placeholders::_1, std::placeholders::_2, SharedSelf()));
}

void CSession::Send(std::string msg) {
	std::lock_guard<std::mutex> lock(_send_lock);
	int send_que_size = _send_que.size();
	if (send_que_size > MAX_SENDQUE) {
		std::cout << "session: " << _uuid << " send que fulled, size is " << MAX_SENDQUE << endl;
		return;
	}

	_send_que.push(make_shared<MsgNode>(msg.c_str(), msg.length()));
	if (send_que_size > 0) {
		return;
	}
	auto& msgnode = _send_que.front();
	boost::asio::async_write(_socket, boost::asio::buffer(msgnode->_data, msgnode->_total_len),
		std::bind(&CSession::HandleWrite, this, std::placeholders::_1, SharedSelf()));
}

void CSession::Send(char* msg, int max_length) {
	std::lock_guard<std::mutex> lock(_send_lock);
	int send_que_size = _send_que.size();
	if (send_que_size > MAX_SENDQUE) {
		std::cout << "session: " << _uuid << " send que fulled, size is " << MAX_SENDQUE << endl;
		return;
	}

	_send_que.push(make_shared<MsgNode>(msg, max_length));
	if (send_que_size>0) {
		return;
	}
	auto& msgnode = _send_que.front();
	boost::asio::async_write(_socket, boost::asio::buffer(msgnode->_data, msgnode->_total_len), 
		std::bind(&CSession::HandleWrite, this, std::placeholders::_1, SharedSelf()));
}

void CSession::Close() {
	_socket.close();
	_b_close = true;
}

std::shared_ptr<CSession>CSession::SharedSelf() {
	return shared_from_this();
}

void CSession::HandleWrite(const boost::system::error_code& error, std::shared_ptr<CSession> shared_self) {
	//�����쳣����
	try {
		if (!error) {
			std::lock_guard<std::mutex> lock(_send_lock);
			//cout << "send data " << _send_que.front()->_data+HEAD_LENGTH << endl;
			_send_que.pop();
			if (!_send_que.empty()) {
				auto& msgnode = _send_que.front();
				boost::asio::async_write(_socket, boost::asio::buffer(msgnode->_data, msgnode->_total_len),
					std::bind(&CSession::HandleWrite, this, std::placeholders::_1, shared_self));
			}
		}
		else {
			std::cout << "handle write failed, error is " << error.what() << endl;
			Close();
			_server->ClearSession(_uuid);
		}
	}
	catch (std::exception& e) {
		std::cerr << "Exception code : " << e.what() << endl;
	}
	
}

void CSession::HandleRead(const boost::system::error_code& error, size_t  bytes_transferred, std::shared_ptr<CSession> shared_self){
	try {
		if (!error) {
			//�Ѿ��ƶ����ַ���
			int copy_len = 0;
			while (bytes_transferred > 0) {
				if (!_b_head_parse) {
					//�յ������ݲ���ͷ����С
					if (bytes_transferred + _recv_head_node->_cur_len < HEAD_LENGTH) {
						memcpy(_recv_head_node->_data + _recv_head_node->_cur_len, _data + copy_len, bytes_transferred);
						_recv_head_node->_cur_len += bytes_transferred;
						::memset(_data, 0, MAX_LENGTH);
						_socket.async_read_some(boost::asio::buffer(_data, MAX_LENGTH),
							std::bind(&CSession::HandleRead, this, std::placeholders::_1, std::placeholders::_2, shared_self));
						return;
					}
					//�յ������ݱ�ͷ����
					//ͷ��ʣ��δ���Ƶĳ���
					int head_remain = HEAD_LENGTH - _recv_head_node->_cur_len;
					memcpy(_recv_head_node->_data + _recv_head_node->_cur_len, _data + copy_len, head_remain);
					//�����Ѵ����data���Ⱥ�ʣ��δ����ĳ���
					copy_len += head_remain;
					bytes_transferred -= head_remain;
					//��ȡͷ������
					short data_len = 0;
					memcpy(&data_len, _recv_head_node->_data, HEAD_LENGTH);
					//�����ֽ���ת��Ϊ�����ֽ���
					data_len = boost::asio::detail::socket_ops::network_to_host_short(data_len);
					std::cout << "data_len is " << data_len << endl;
					//ͷ�����ȷǷ�
					if (data_len > MAX_LENGTH) {
						std::cout << "invalid data length is " << data_len << endl;
						_server->ClearSession(_uuid);
						return;
					}
					_recv_msg_node = make_shared<MsgNode>(data_len);

					//��Ϣ�ĳ���С��ͷ���涨�ĳ��ȣ�˵������δ��ȫ�����Ƚ�������Ϣ�ŵ����սڵ���
					if (bytes_transferred < data_len) {
						memcpy(_recv_msg_node->_data + _recv_msg_node->_cur_len, _data + copy_len, bytes_transferred);
						_recv_msg_node->_cur_len += bytes_transferred;
						::memset(_data, 0, MAX_LENGTH);
						_socket.async_read_some(boost::asio::buffer(_data, MAX_LENGTH),
							std::bind(&CSession::HandleRead, this, std::placeholders::_1, std::placeholders::_2, shared_self));
						//ͷ���������
						_b_head_parse = true;
						return;
					}

					memcpy(_recv_msg_node->_data + _recv_msg_node->_cur_len, _data + copy_len, data_len);
					_recv_msg_node->_cur_len += data_len;
					copy_len += data_len;
					bytes_transferred -= data_len;
					_recv_msg_node->_data[_recv_msg_node->_total_len] = '\0';
					//cout << "receive data is " << _recv_msg_node->_data << endl;
					//�˴����Ե���Send���Ͳ���
					MsgData msgdata;
					std::string receive_data;
					msgdata.ParseFromString(std::string(_recv_msg_node->_data, _recv_msg_node->_total_len));
					std::cout << "recevie msg id  is " << msgdata.id() << " msg data is " << msgdata.data() << endl;
					std::string return_str = "server has received msg, msg data is " + msgdata.data();
					MsgData msgreturn;
					msgreturn.set_id(msgdata.id());
					msgreturn.set_data(return_str);
					msgreturn.SerializeToString(&return_str);
					Send(return_str);
					//������ѯʣ��δ��������
					_b_head_parse = false;
					_recv_head_node->Clear();
					if (bytes_transferred <= 0) {
						::memset(_data, 0, MAX_LENGTH);
						_socket.async_read_some(boost::asio::buffer(_data, MAX_LENGTH),
							std::bind(&CSession::HandleRead, this, std::placeholders::_1, std::placeholders::_2, shared_self));
						return;
					}
					continue;
				}

				//�Ѿ�������ͷ���������ϴ�δ���������Ϣ����
				//���յ������Բ���ʣ��δ�����
				int remain_msg = _recv_msg_node->_total_len - _recv_msg_node->_cur_len;
				if (bytes_transferred < remain_msg) {
					memcpy(_recv_msg_node->_data + _recv_msg_node->_cur_len, _data + copy_len, bytes_transferred);
					_recv_msg_node->_cur_len += bytes_transferred;
					::memset(_data, 0, MAX_LENGTH);
					_socket.async_read_some(boost::asio::buffer(_data, MAX_LENGTH),
						std::bind(&CSession::HandleRead, this, std::placeholders::_1, std::placeholders::_2, shared_self));
					return;
				}
				memcpy(_recv_msg_node->_data + _recv_msg_node->_cur_len, _data + copy_len, remain_msg);
				_recv_msg_node->_cur_len += remain_msg;
				bytes_transferred -= remain_msg;
				copy_len += remain_msg;
				_recv_msg_node->_data[_recv_msg_node->_total_len] = '\0';
				//cout << "receive data is " << _recv_msg_node->_data << endl;
				//�˴����Ե���Send���Ͳ���
				MsgData msgdata;
				std::string receive_data;
				msgdata.ParseFromString(std::string(_recv_msg_node->_data, _recv_msg_node->_total_len));
				std::cout << "recevie msg id  is " << msgdata.id() << " msg data is " << msgdata.data() << endl;
				std::string return_str = "server has received msg, msg data is " + msgdata.data();
				MsgData msgreturn;
				msgreturn.set_id(msgdata.id());
				msgreturn.set_data(return_str);
				msgreturn.SerializeToString(&return_str);
				Send(return_str);
				//������ѯʣ��δ��������
				_b_head_parse = false;
				_recv_head_node->Clear();
				if (bytes_transferred <= 0) {
					::memset(_data, 0, MAX_LENGTH);
					_socket.async_read_some(boost::asio::buffer(_data, MAX_LENGTH),
						std::bind(&CSession::HandleRead, this, std::placeholders::_1, std::placeholders::_2, shared_self));
					return;
				}
				continue;
			}
		}
		else {
			std::cout << "handle read failed, error is " << error.what() << endl;
			Close();
			_server->ClearSession(_uuid);
		}
	}
	catch (std::exception& e) {
		std::cout << "Exception code is " << e.what() << endl;
	}
}
