#pragma once
//使用extern主要是怕重定义
/**********************************************/
/*套接字的创建，connect、bind等*/
/*创建客户端和服务端的端点*/
extern int clint_end_point();	//客户端去连接对端的一个端点
extern int server_end_point();	//服务器段生成一个通信的端点 
/*用于创建套接字*/
extern int create_tcp_socket();	//创建TCP的socket
extern int create_acceptor_socket();	//创建服务器端的socket
/*相当于服务器中的bind*/
extern int bind_acceptor_socket(); //绑定socket
/*相当于客户端中的connect*/
extern int connect_to_end();		//连接服务器指定的端点
extern int dns_connect_to_end();		//连接域名
/*相当于服务器中的accept*/
extern int accept_new_connection();	//服务器接收连接



/***********************************************/
/*buffer结构和同步读写*/
extern void use_const_buffer();		//字符串类型转换，比如boost的发送接口send要求的参数为ConstBufferSequence类型
extern void use_buffer_str();		//简化版本
//有数组怎么办
extern void use_buffer_array();


//同步读写
