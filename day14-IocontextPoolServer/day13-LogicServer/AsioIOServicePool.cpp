#include "AsioIOServicePool.h"

AsioIOServicePool::~AsioIOServicePool()
{
	std::cout << "AsioIOServicePool destruct" << endl;
}

AsioIOServicePool::AsioIOServicePool(std::size_t size):_ioServices(size), _works(size), _nextIOService(0)
{
	//初始化
	for (std::size_t i = 0; i < size; i++) {
		_works[i] = std::unique_ptr<Work>(new Work(_ioServices[i]));	//智能指针未被赋值时属于右值，可以赋值给_works[i]；
	}
	//遍历多个ioservice，创建多个线程，每个线程内部启动ioservice
	for (size_t i = 0; i < _ioServices.size(); i++) {
		//插入线程，不用push_back的原因是，push_back要先把线程创建好在插进去
		_threads.emplace_back([this, i]() {
			_ioServices[i].run();
			});
		/*thread t([this, i]() {
			_ioServices[i].run();
			});
		_threads.push_back(t);*/
	}
}

boost::asio::io_context& AsioIOServicePool::GetIOService() {
	auto& service = _ioServices[_nextIOService++];
	if (_nextIOService == _ioServices.size()) {
		_nextIOService = 0;
	}
	return service; 
}

void AsioIOServicePool::Stop()
{
	for (auto& work : _works) {
		work.reset();
	}

	for (auto& t :_threads) {
		t.join();
	}
}
