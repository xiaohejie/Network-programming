#include "LogicSystem.h"
using namespace std;

LogicSystem::LogicSystem():_b_stop(false) {
	//将消息ID和回调函数绑定起来
	RegisterCallBacks();
	//启动线程:从队列里取消息节点，然后处理消息
	_worker_thread = std::thread(&LogicSystem::DealMsg, this);	//第一个参数传类的成员函数，第二个参数就需要传类的地址

}

LogicSystem::~LogicSystem()
{
	_b_stop = true;
	//先将工作线程唤醒（有可能挂起），然后再进行回收
	_consume.notify_one();
	_worker_thread.join();	//等待工作线程退出
}

//往消息队列里投递逻辑节点
void LogicSystem::PostMsgToQue(shared_ptr<LogicNode> msg)
{
	std::unique_lock<std::mutex> unique_lk(_mutex);//加锁
	_msg_que.push(msg);

	if (_msg_que.size() == 1) {
		//唤醒条件变量
		_consume.notify_one();

	}
}

void LogicSystem::RegisterCallBacks()
{
	 //将消息ID和回调函数进行绑定
	_fun_callback[MSG_HELLO_WORD] = std::bind(&LogicSystem::HelloWorldCallBack,
		this, placeholders::_1, placeholders::_2, placeholders::_3);
}

//helloworld的回调函数
void LogicSystem::HelloWorldCallBack(shared_ptr<CSession> session, const short& msg_id, const string& msg_data)
{
	Json::Reader reader; 
	Json::Value root;
	reader.parse(msg_data, root);
	std::cout << "receive msg id is" << root["id"].asInt() << "msg data is" << root["data"].asString() << endl;
	root["data"] = "server has receive msg, msg data is" + root["data"].asString();
	
	std::string return_str = root.toStyledString();	//转换成字符串类型
	session->Send(return_str, root["id"].asInt());
}

void LogicSystem::DealMsg()
{
	//死循环
	for (;;) {
		std::unique_lock<std::mutex> unique_lk(_mutex);	//加锁
		//判断线程是否为空，空了就需要用条件变量挂起等待
		while (_msg_que.empty() && !_b_stop) {	//队列为空并且当前不是停止状态
			_consume.wait(unique_lk);	//条件变量挂起
		}
		//线程激活状态，判断为什么程序能够走下来
		//判断如果为关闭状态，取出逻辑队列中所有数据及时处理并退出循环
		if (_b_stop) {
			while (!_msg_que.empty()) {
				auto msg_node = _msg_que.front();
				std::cout << "recv msg id is: " << msg_node->_recvnode->_msg_id << std::endl;
				//根据消息ID找出回调函数
				auto call_back_iter = _fun_callback.find(msg_node->_recvnode->_msg_id);
				if (call_back_iter == _fun_callback.end()) {	//没找到
					_msg_que.pop();	//抛出
					continue;
				}
				//找到，就处理回调函数
				call_back_iter->second(msg_node->_session, msg_node->_recvnode->_msg_id,
					std::string(msg_node->_recvnode->_data, msg_node->_recvnode->_cur_len));
				_msg_que.pop();
			}
			break;
		}

		//如果不是关闭状态，就是队列中有数据
		auto msg_node = _msg_que.front();
		std::cout << "recv msg id is: " << msg_node->_recvnode->_msg_id << std::endl;
		//根据消息ID找出回调函数
		auto call_back_iter = _fun_callback.find(msg_node->_recvnode->_msg_id);
		if (call_back_iter == _fun_callback.end()) {	//没找到
			_msg_que.pop();	//抛出
			continue;
		}
		//找到，就处理回调函数
		call_back_iter->second(msg_node->_session, msg_node->_recvnode->_msg_id,
			std::string(msg_node->_recvnode->_data, msg_node->_recvnode->_cur_len));
		_msg_que.pop();
	}
}
