#pragma once
#include<memory>
#include<mutex>
#include<iostream>
using namespace std;
template<typename T>
class Singleton {
public:
	~Singleton() {
		std::cout << "this is singlenton destruct" << std::endl;
	}
	static std::shared_ptr<T> GetInstance() {
		static std::once_flag s_flag;	//ֻ���ڵ�һ�ε��õ�ʱ���ʼ�����Ժ�Ͳ����ʼ����
		std::call_once(s_flag, [&]() {		//ֻ�����һ��  
			_instance = shared_ptr<T>(new T);
			});
		return _instance;
	}
	void PrintAddress() {
		std::cout << _instance.get() << std::endl;
	}
protected:
	Singleton() = default;
	Singleton(const Singleton<T>&) = delete;	//�������캯��
	Singleton& operator = (const Singleton<T>& st) = delete;	//������ֵ���������

	static std::shared_ptr<T> _instance;	//���ж�����þ�̬������ָ��	
};

//��ʼ��_instance
template<typename T>
std::shared_ptr<T> Singleton<T>::_instance = nullptr;
