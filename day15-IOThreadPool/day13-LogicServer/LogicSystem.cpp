#include "LogicSystem.h"
using namespace std;

LogicSystem::LogicSystem():_b_stop(false) {
	//����ϢID�ͻص�����������
	RegisterCallBacks();
	//�����߳�:�Ӷ�����ȡ��Ϣ�ڵ㣬Ȼ������Ϣ
	_worker_thread = std::thread(&LogicSystem::DealMsg, this);	//��һ����������ĳ�Ա�������ڶ�����������Ҫ����ĵ�ַ

}

LogicSystem::~LogicSystem()
{
	_b_stop = true;
	//�Ƚ������̻߳��ѣ��п��ܹ��𣩣�Ȼ���ٽ��л���
	_consume.notify_one();
	_worker_thread.join();	//�ȴ������߳��˳�
}

//����Ϣ������Ͷ���߼��ڵ�
void LogicSystem::PostMsgToQue(shared_ptr<LogicNode> msg)
{
	std::unique_lock<std::mutex> unique_lk(_mutex);//����
	_msg_que.push(msg);

	if (_msg_que.size() == 1) {
		//������������
		_consume.notify_one();

	}
}

void LogicSystem::RegisterCallBacks()
{
	 //����ϢID�ͻص��������а�
	_fun_callback[MSG_HELLO_WORD] = std::bind(&LogicSystem::HelloWorldCallBack,
		this, placeholders::_1, placeholders::_2, placeholders::_3);
}

//helloworld�Ļص�����
void LogicSystem::HelloWorldCallBack(shared_ptr<CSession> session, const short& msg_id, const string& msg_data)
{
	Json::Reader reader; 
	Json::Value root;
	reader.parse(msg_data, root);
	std::cout << "receive msg id is" << root["id"].asInt() << "msg data is" << root["data"].asString() << endl;
	root["data"] = "server has receive msg, msg data is" + root["data"].asString();
	
	std::string return_str = root.toStyledString();	//ת�����ַ�������
	session->Send(return_str, root["id"].asInt());
}

void LogicSystem::DealMsg()
{
	//��ѭ��
	for (;;) {
		std::unique_lock<std::mutex> unique_lk(_mutex);	//����
		//�ж��߳��Ƿ�Ϊ�գ����˾���Ҫ��������������ȴ�
		while (_msg_que.empty() && !_b_stop) {	//����Ϊ�ղ��ҵ�ǰ����ֹͣ״̬
			_consume.wait(unique_lk);	//������������
		}
		//�̼߳���״̬���ж�Ϊʲô�����ܹ�������
		//�ж����Ϊ�ر�״̬��ȡ���߼��������������ݼ�ʱ�����˳�ѭ��
		if (_b_stop) {
			while (!_msg_que.empty()) {
				auto msg_node = _msg_que.front();
				std::cout << "recv msg id is: " << msg_node->_recvnode->_msg_id << std::endl;
				//������ϢID�ҳ��ص�����
				auto call_back_iter = _fun_callback.find(msg_node->_recvnode->_msg_id);
				if (call_back_iter == _fun_callback.end()) {	//û�ҵ�
					_msg_que.pop();	//�׳�
					continue;
				}
				//�ҵ����ʹ���ص�����
				call_back_iter->second(msg_node->_session, msg_node->_recvnode->_msg_id,
					std::string(msg_node->_recvnode->_data, msg_node->_recvnode->_cur_len));
				_msg_que.pop();
			}
			break;
		}

		//������ǹر�״̬�����Ƕ�����������
		auto msg_node = _msg_que.front();
		std::cout << "recv msg id is: " << msg_node->_recvnode->_msg_id << std::endl;
		//������ϢID�ҳ��ص�����
		auto call_back_iter = _fun_callback.find(msg_node->_recvnode->_msg_id);
		if (call_back_iter == _fun_callback.end()) {	//û�ҵ�
			_msg_que.pop();	//�׳�
			continue;
		}
		//�ҵ����ʹ���ص�����
		call_back_iter->second(msg_node->_session, msg_node->_recvnode->_msg_id,
			std::string(msg_node->_recvnode->_data, msg_node->_recvnode->_cur_len));
		_msg_que.pop();
	}
}
