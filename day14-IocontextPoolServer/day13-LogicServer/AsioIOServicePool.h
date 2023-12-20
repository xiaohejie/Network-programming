#pragma once
#include "Singleton.h"
#include <boost/asio.hpp>
#include <vector>


class AsioIOServicePool :public Singleton<AsioIOServicePool>
{
	friend Singleton<AsioIOServicePool>;
public:
	using IOService = boost::asio::io_context;
	using Work = boost::asio::io_context::work;
	using Workerptr = std::unique_ptr<Work>;	//使用Workerptr是为了只让work不被拷贝，只能移动；或者从头用到尾
	~AsioIOServicePool(); 
	AsioIOServicePool(const AsioIOServicePool&) = delete;
	AsioIOServicePool& operator = (const AsioIOServicePool&) = delete;
	//使用round-robin的方式返回一个io_context
	boost::asio::io_context& GetIOService();
	void Stop();
private:
	AsioIOServicePool(std::size_t size = std::thread::hardware_concurrency());	//获取并行的线程数（即CPU的核数） 
	std::vector<IOService> _ioServices;
	std::vector<Workerptr> _works;
	std::vector<std::thread> _threads;
	std::size_t _nextIOService;		//记录下一个io_context的坐标
};

