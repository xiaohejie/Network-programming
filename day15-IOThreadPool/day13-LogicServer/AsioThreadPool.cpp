#include "AsioThreadPool.h"

boost::asio::io_context& AsioThreadPool::GetIOService()
{
	// TODO: �ڴ˴����� return ���
	return _service;
}

void AsioThreadPool::Stop()
{
	_work.reset();//�ͷ�����ָ��
	//�ȴ��߳��˳�
	for (auto& t : _threads) {
		t.join();
	}
}

AsioThreadPool::AsioThreadPool(int threadNum) : _work(new boost::asio::io_context::work(_service))
{
	//ͨ�����������̣߳�����i���߳�
	for (int i = 0; i < threadNum; i++) {
		_threads.emplace_back([this]() {
			_service.run();
			});
	}
}
