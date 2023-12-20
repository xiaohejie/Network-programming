#include "AsioIOServicePool.h"

AsioIOServicePool::~AsioIOServicePool()
{
	std::cout << "AsioIOServicePool destruct" << endl;
}

AsioIOServicePool::AsioIOServicePool(std::size_t size):_ioServices(size), _works(size), _nextIOService(0)
{
	//��ʼ��
	for (std::size_t i = 0; i < size; i++) {
		_works[i] = std::unique_ptr<Work>(new Work(_ioServices[i]));	//����ָ��δ����ֵʱ������ֵ�����Ը�ֵ��_works[i]��
	}
	//�������ioservice����������̣߳�ÿ���߳��ڲ�����ioservice
	for (size_t i = 0; i < _ioServices.size(); i++) {
		//�����̣߳�����push_back��ԭ���ǣ�push_backҪ�Ȱ��̴߳������ڲ��ȥ
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
