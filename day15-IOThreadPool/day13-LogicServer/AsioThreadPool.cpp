#include "AsioThreadPool.h"

boost::asio::io_context& AsioThreadPool::GetIOService()
{
	// TODO: 在此处插入 return 语句
	return _service;
}

void AsioThreadPool::Stop()
{
	_work.reset();//释放智能指针
	//等待线程退出
	for (auto& t : _threads) {
		t.join();
	}
}

AsioThreadPool::AsioThreadPool(int threadNum) : _work(new boost::asio::io_context::work(_service))
{
	//通过传过来的线程，开辟i个线程
	for (int i = 0; i < threadNum; i++) {
		_threads.emplace_back([this]() {
			_service.run();
			});
	}
}
