#pragma once
#include "Singleton.h"
#include<queue>
#include<thread>
#include"CSession.h"
#include<map>
#include<functional>
#include"const.h"
#include<json/json.h>
#include<json/value.h>
#include<json/reader.h>

typedef function<void(shared_ptr<CSession>, const short& msg_id, const string& msg_data)> FunCallBack;

class LogicSystem:public Singleton<LogicSystem>
{
	friend class Singleton<LogicSystem>;
public:
	~LogicSystem();
	//����Ϣ������Ͷ���߼��ڵ�
	void PostMsgToQue(shared_ptr<LogicNode> msg);
private:
	LogicSystem();
	//ע��ص�����
	void RegisterCallBacks();
	//�ص�����������hello world�Ļص�����
	void HelloWorldCallBack(shared_ptr<CSession>, const short& msg_id, const string& msg_data);
	//�߼�ϵͳ������Ϣ�������̵߳���
	void DealMsg();
	 
	std::queue<shared_ptr<LogicNode>> _msg_que; 
	std::mutex _mutex;
	std::condition_variable _consume;	 //��������
	std::thread _worker_thread;	//�����̣߳���������е��߼��ڵ�
	bool _b_stop;	//�ж��Ƿ�ֹͣ����
	std::map<short, FunCallBack> _fun_callback;	//��msg_id�ͻص��������а�
};

