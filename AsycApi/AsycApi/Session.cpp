#include "Session.h"
Session::Session(std::shared_ptr<asio::ip::tcp::socket> socket):_socket(socket), _send_pending(false),_recv_pending(false) {

}
void Session::Connect(const asio::ip::tcp::endpoint& ep) {
	_socket->connect(ep);
}


/***********************异步读的三种方式***************************/
void Session::WriteCallBackErr(const boost::system::error_code& ec, std::size_t bytes_transferred, std::shared_ptr<MsgNode> msg_node)
{
	if (bytes_transferred + msg_node->_cur_len < msg_node->_total_len) {	//如果没发送完
		_send_node->_cur_len += bytes_transferred;	//更新已发送的长度
		//继续发
		this->_socket->async_write_some(asio::buffer(_send_node->_msg + _send_node->_cur_len, 
			_send_node->_total_len - _send_node->_cur_len),
			std::bind(&Session::WriteCallBackErr, this, std::placeholders::_1, std::placeholders::_2));
	}
}

void Session::WriteToSocketErr(const std::string& buf)
{
	_send_node = make_shared<MsgNode>(buf.c_str(), buf.length());
	this->_socket->async_write_some(asio::buffer(_send_node->_msg, _send_node->_total_len),
		std::bind(&Session::WriteCallBackErr, this, std::placeholders::_1, std::placeholders::_2,
			_send_node));
}

void Session::WriteCallBack(const boost::system::error_code& ec, std::size_t bytes_transferred)
{
	if (ec.value() != 0) {
		std:: cout << "Error, code is " << ec.value() << ".Message is " << ec.message() << endl;
		return;
	}
	auto& send_data = _send_queue.front();
	send_data->_cur_len += bytes_transferred;
	if (send_data->_cur_len < send_data->_total_len) { 	//说明没发完
		this->_socket->async_write_some(asio::buffer(send_data->_msg + send_data->_cur_len,
			send_data->_total_len - send_data->_cur_len),
			std::bind(&Session::WriteCallBack, this, std::placeholders::_1, std::placeholders::_2)
			);
		return;
	}
	//发送完
	_send_queue.pop();
	if (_send_queue.empty()) {	//为空
		_send_pending = false;
	}
	if (!_send_queue.empty()) {	//不为空，就继续发送
		auto& send_data = _send_queue.front();
		this->_socket->async_write_some(asio::buffer(send_data->_msg + _send_node->_cur_len,
			send_data->_total_len - send_data->_cur_len),
			std::bind(&Session::WriteCallBack, this, std::placeholders::_1, std::placeholders::_2));
	}
}

void Session::WriteToSocket(const std::string& buf)
{
	_send_queue.emplace(new MsgNode(buf.c_str(), buf.length()));	//将要发送到数据放到队列里面
	if (_send_pending) {//如果还有数据，返回继续发
		return;
	}
	//没有数据，异步调用发送队列
	this->_socket->async_write_some(asio::buffer(buf), 
		std::bind(&Session::WriteCallBack, this, std::placeholders::_1, std::placeholders::_2));
	_send_pending = true;
}

void Session::WriteAllCallBack(const boost::system::error_code& ec, std::size_t bytes_transferred)
{
	if (ec.value() != 0) {
		std::cout << "Error occured! Error code = "
			<< ec.value()
			<< ". Message: " << ec.message();
		return;
	}
	//如果发送完，则pop出队首元素
	_send_queue.pop();
	//如果队列为空，则说明所有数据都发送完,将pending设置为false
	if (_send_queue.empty()) {
		_send_pending = false;
	}
	//如果队列不是空，则继续将队首元素发送
	if (!_send_queue.empty()) {
		auto& send_data = _send_queue.front();
		this->_socket->async_send(asio::buffer(send_data->_msg + send_data->_cur_len, send_data->_total_len - send_data->_cur_len),
			std::bind(&Session::WriteAllCallBack,
				this, std::placeholders::_1, std::placeholders::_2));
	} 
}

void Session::WriteAllToSocket(const std::string& buf)
{
	//插入发送队列
	_send_queue.emplace(new MsgNode(buf.c_str(), buf.length()));
	//pending状态说明上一次有未发送完的数据
	if (_send_pending) {
		return;
	}
	//异步发送数据，因为异步所以不会一下发送完
	this->_socket->async_send(asio::buffer(buf),
		std::bind(&Session::WriteAllCallBack, this,
			std::placeholders::_1, std::placeholders::_2));
	_send_pending = true; 
}



/*****************************异步读***************************/
void Session::ReadFromSocket()
{
	if (_recv_pending) {
		return;
	}
	//可以调用构造函数直接构造，但不可用已经构造好的智能指针赋值
	/*auto _recv_nodez = std::make_unique<MsgNode>(RECVSIZE);
	_recv_node = _recv_nodez;*/
	_recv_node = std::make_shared<MsgNode>(RECVSIZE);
	_socket->async_read_some(asio::buffer(_recv_node->_msg, _recv_node->_total_len), std::bind(&Session::ReadCallBack, this,
		std::placeholders::_1, std::placeholders::_2));
	_recv_pending = true;
}

void Session::ReadCallBack(const boost::system::error_code& ec, std::size_t bytes_transferred)
{
	_recv_node->_cur_len += bytes_transferred;
	//没读完继续读
	if (_recv_node->_cur_len < _recv_node->_total_len) {
		_socket->async_read_some(asio::buffer(_recv_node->_msg + _recv_node->_cur_len,
			_recv_node->_total_len - _recv_node->_cur_len), std::bind(&Session::ReadCallBack, this,
				std::placeholders::_1, std::placeholders::_2));
		return;
	}
	//将数据投递到队列里交给逻辑线程处理，此处略去
	//如果读完了则将标记置为false
	_recv_pending = false;
	//指针置空
	_recv_node = nullptr;
}

void Session::ReadAllFromSocket()
{
	if (_recv_pending) {
		return;
	}
	//可以调用构造函数直接构造，但不可用已经构造好的智能指针赋值
	/*auto _recv_nodez = std::make_unique<MsgNode>(RECVSIZE);
	_recv_node = _recv_nodez;*/
	_recv_node = std::make_shared<MsgNode>(RECVSIZE);
	_socket->async_receive(asio::buffer(_recv_node->_msg, _recv_node->_total_len), std::bind(&Session::ReadAllCallBack, this,
		std::placeholders::_1, std::placeholders::_2));
	_recv_pending = true;
}

void Session::ReadAllCallBack(const boost::system::error_code& ec, std::size_t bytes_transferred)
{
	_recv_node->_cur_len += bytes_transferred;
	//将数据投递到队列里交给逻辑线程处理，此处略去
	//如果读完了则将标记置为false
	_recv_pending = false;
	//指针置空
	_recv_node = nullptr;
}
