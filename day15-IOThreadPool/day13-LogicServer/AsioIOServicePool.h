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
	using Workerptr = std::unique_ptr<Work>;	//ʹ��Workerptr��Ϊ��ֻ��work����������ֻ���ƶ������ߴ�ͷ�õ�β
	~AsioIOServicePool(); 
	AsioIOServicePool(const AsioIOServicePool&) = delete;
	AsioIOServicePool& operator = (const AsioIOServicePool&) = delete;
	//ʹ��round-robin�ķ�ʽ����һ��io_context
	boost::asio::io_context& GetIOService();
	void Stop();
private:
	AsioIOServicePool(std::size_t size = std::thread::hardware_concurrency());	//��ȡ���е��߳�������CPU�ĺ����� 
	std::vector<IOService> _ioServices;
	std::vector<Workerptr> _works;
	std::vector<std::thread> _threads;
	std::size_t _nextIOService;		//��¼��һ��io_context������
};

