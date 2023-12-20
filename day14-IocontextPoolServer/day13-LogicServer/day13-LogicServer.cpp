#include <iostream>
#include "CServer.h"
#include "Singleton.h"
#include "LogicSystem.h"
#include <csignal>
#include <thread>
#include <mutex>
#include "AsioIOServicePool.h"
using namespace std;
bool bstop = false;
std::condition_variable cond_quit;
std::mutex mutex_quit;

void sig_handler(int sig) {
	if (sig == SIGINT || sig == SIGTERM) {
		std::unique_lock<std::mutex> lock_quit(mutex_quit);	//加锁，配合条件变量
		bstop = true; 
		cond_quit.notify_one();	//唤醒条件变量
	}
}

int main()
{
	try {
		auto pool = AsioIOServicePool::GetInstance();	//初始化池子 
		boost::asio::io_context  io_context;
		/*
		//通过C的方式
		std::thread net_work_thread([&io_context]() {	//另开一个线程处理，主线程处于挂起状态
			CServer s(io_context, 10086);
			io_context.run();
			});
		 //监听信号
		signal(SIGINT, sig_handler);
		signal(SIGTERM, sig_handler);
		while(!bstop){
			std::unique_lock<std::mutex> lock_quit(mutex_quit);	
			cond_quit.wait(lock_quit);
		}
		io_context.stop(); 
		net_work_thread.join();*/
		//通过boost::asio实现退出
		boost::asio::signal_set signals(io_context, SIGINT, SIGTERM);	//捕获想捕获得信号，然后想注册到哪个服务里
		signals.async_wait([&io_context](auto, auto) {	//异步等待
			io_context.stop();
			});
		CServer s(io_context, 10086);
		io_context.run();
	}
	catch (std::exception& e) {
		std::cerr << "Exception: " << e.what() << endl;
	}

}