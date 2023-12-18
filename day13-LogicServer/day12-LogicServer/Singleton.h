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
		static std::once_flag s_flag;	//只会在第一次调用的时候初始化，以后就不会初始化了
		std::call_once(s_flag, [&]() {		//只会调用一次  
			_instance = shared_ptr<T>(new T);
			});
		return _instance;
	}
	void PrintAddress() {
		std::cout << _instance.get() << std::endl;
	}
protected:
	Singleton() = default;
	Singleton(const Singleton<T>&) = delete;	//拷贝构造函数
	Singleton& operator = (const Singleton<T>& st) = delete;	//拷贝赋值运算符函数

	static std::shared_ptr<T> _instance;	//所有对象共享该静态的智能指针	
};

//初始化_instance
template<typename T>
std::shared_ptr<T> Singleton<T>::_instance = nullptr;
