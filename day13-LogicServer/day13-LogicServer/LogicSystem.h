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
	//往消息队列里投递逻辑节点
	void PostMsgToQue(shared_ptr<LogicNode> msg);
private:
	LogicSystem();
	//注册回调函数
	void RegisterCallBacks();
	//回调函数：处理hello world的回调函数
	void HelloWorldCallBack(shared_ptr<CSession>, const short& msg_id, const string& msg_data);
	//逻辑系统处理消息，工作线程调用
	void DealMsg();
	 
	std::queue<shared_ptr<LogicNode>> _msg_que; 
	std::mutex _mutex;
	std::condition_variable _consume;	 //条件变量
	std::thread _worker_thread;	//工作线程，处理队列中的逻辑节点
	bool _b_stop;	//判断是否停止处理
	std::map<short, FunCallBack> _fun_callback;	//将msg_id和回调函数进行绑定
};

