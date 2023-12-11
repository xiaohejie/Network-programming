#include "endpoint.h"
#include <boost/asio.hpp>
#include <iostream>
using namespace boost;
//客户端
int clint_end_point() {
	//对端地址和端口号
	std::string raw_ip_address = "127.4.8.1";
	unsigned short port_num = 3333;

	//知道对端地址和端口号后需要转换成asio使用的IP地址
	boost::system::error_code er;	//错误码，用来观察转换错误时出现的错误
	asio::ip::address ip_address = asio::ip::address::from_string(raw_ip_address, er);
	if (er.value() != 0) {	//不为0 表示转换失败有错误
		std::cout << "Failed to parse the IP address. Error code = " << er.value() << " . Message is" << er.message();
		return er.value(); 
	}

	asio::ip::tcp::endpoint ep(ip_address, port_num);
	return 0;
}

//服务端
int server_end_point() {
	unsigned short port_num = 3333;
	asio::ip::address ip_address = asio::ip::address_v6::any();
	asio::ip::tcp::endpoint ep(ip_address, port_num);
	return 0;
}


//创建tcpsocket
int create_tcp_socket() {
	asio::io_context ioc;	//创建上下文，告知是为那个socket服务的
	asio::ip::tcp protocol = asio::ip::tcp::v4();	//创建ipv4的协议
	//生成socket
	asio::ip::tcp::socket sock(ioc);
	//判断socket是否打开成功
	boost::system::error_code ec;
	sock.open(protocol, ec);
	if (ec.value() != 0) {
		std::cout << "Failed to open the socket. Error code = " << ec.value() << " . Message is" << ec.message();
		return ec.value();
	}
	return 0;
}


//服务器端还需要生成一个acceptor的socket，用来接收新的连接
int create_acceptor_socket() {
	asio::io_context ios;
	/*asio::ip::tcp protocol = asio::ip::tcp::v4();
	asio::ip::tcp::acceptor acceptor(ios);
	boost::system::error_code ec;
	acceptor.open(protocol, ec);
	if (ec.value() != 0) {
		std::cout
			<< "Failed to open the acceptor socket!"
			<< "Error code = "
			<< ec.value() << ". Message: " << ec.message();
		return ec.value();
	}*/

	//简化
	asio::ip::tcp::acceptor a(ios, asio::ip::tcp::endpoint(asio::ip::tcp::v4(), 3333));
	return 0;
}

//服务器端绑定acceptor
int bind_acceptor_socket() {
	unsigned short port_num = 3333;
	asio::ip::tcp::endpoint ep(asio::ip::address_v4::any(), port_num);
	//让服务器知道acceptor绑定在哪个服务上
	asio::io_context ios; 
	asio::ip::tcp::acceptor acceptor(ios, ep.protocol());	//生成一个acceptor
	boost::system::error_code ec;
	acceptor.bind(ep, ec);	//进行绑定
	// Handling errors if any.
	if (ec.value() != 0) {
		// Failed to bind the acceptor socket. Breaking
		// execution.
		std::cout << "Failed to bind the acceptor socket."
			<< "Error code = " << ec.value() << ". Message: "
			<< ec.message();
		return ec.value();
	}
	return 0;
}

//客户端连接服务器指定的端点
int connect_to_end() {
	//服务器地址和端口
	std::string raw_ip_address = "127.0.0.1";
	unsigned short port_num = 3333;

	try {
		asio::ip::tcp::endpoint ep(asio::ip::address::from_string(raw_ip_address), port_num);
		asio::io_context ios;
		// Step 3. Creating and opening a socket.
		asio::ip::tcp::socket sock(ios, ep.protocol());
		// Step 4. Connecting a socket.
		sock.connect(ep);
	}
	catch (system::system_error& e) {
		std::cout << "Error occured! Error code = " << e.code()
			<< ". Message: " << e.what();
		return e.code().value();
	}
	return 0;
}

//连接域名：解析域名，连接服务器
int dns_connect_to_end() {
	std::string host = "llfc.club";
	std::string port_num = "3333";
	asio::io_context ios;	//创建一个服务
	//创建DNS解析器
	asio::ip::tcp::resolver::query resolver_query(host, port_num, asio::ip::tcp::resolver::query::numeric_service);
	asio::ip::tcp::resolver resolver(ios);

	try {
		asio::ip::tcp::resolver::iterator it = resolver.resolve(resolver_query);	//进行解析，返回一个迭代器
		asio::ip::tcp::socket sock(ios);
		asio::connect(sock, it);
	}
	catch (system::system_error& e) {
		std::cout << "Error occured! Error code = " << e.code()
			<< ". Message: " << e.what();
		return e.code().value();
	}
	return 0;
}

//服务器接收连接
int accept_new_connection() {
	//设置监听队列的大小
	const int BACKLOG_SIZE = 30;
	unsigned short port_num = 3333;
	asio::ip::tcp::endpoint ep(asio::ip::address_v4::any(), port_num);
	asio::io_context ios;


	try {
		// Step 3. Instantiating and opening an acceptor socket.
		asio::ip::tcp::acceptor acceptor(ios, ep.protocol());
		// Step 4. Binding the acceptor socket to the 
		// server endpint.
		acceptor.bind(ep);
		// Step 5. Starting to listen for incoming connection
		// requests.
		acceptor.listen(BACKLOG_SIZE);
		// Step 6. Creating an active socket.
		asio::ip::tcp::socket sock(ios);
		// Step 7. Processing the next connection request and 
		// connecting the active socket to the client.
		acceptor.accept(sock);
		// At this point 'sock' socket is connected to 
		//the client application and can be used to send data to
		// or receive data from it.
	}
	catch (system::system_error& e) {
		std::cout << "Error occured! Error code = " << e.code()
			<< ". Message: " << e.what();
		return e.code().value();
	}
}

//字符串类型转换，比如boost的发送接口send要求的参数为ConstBufferSequence类型
void use_const_buffer() {
	std::string buf = "hello world!";
	asio::const_buffer  asio_buf(buf.c_str(), buf.length());
	std::vector<asio::const_buffer> buffers_sequence;
	buffers_sequence.push_back(asio_buf);
}

//也可以直接用buffer函数转化为send需要的参数类型
void use_buffer_str() {
	asio::const_buffers_1 output_buf = asio::buffer("hello world");
}

//有数组的情况
void use_buffer_array() {
	const size_t  BUF_SIZE_BYTES = 20;
	std::unique_ptr<char[] > buf(new char[BUF_SIZE_BYTES]);	//不用担心内存回收的问题了，new出来的数组首地址交给buf管理
	auto input_buf = asio::buffer(static_cast<void*>(buf.get()), BUF_SIZE_BYTES);	//需要先转换成指针类型
}


/*********同步读写************/
void  write_to_socket(asio::ip::tcp::socket& sock) {
	std::string buf = "Hello World!";
	std::size_t  total_bytes_written = 0;
	//循环发送
	//write_some返回每次写入的字节数
	//total_bytes_written是已经发送的字节数。
	//每次发送buf.length()- total_bytes_written)字节数据  
	while (total_bytes_written != buf.length()) {
		total_bytes_written += sock.write_some(
			asio::buffer(buf.c_str() + total_bytes_written,
				buf.length() - total_bytes_written));
	}
}

//一次性发不完
int send_data_by_write_some() {
	std::string raw_ip_address = "192.168.3.11";
	unsigned short port_num = 3333;
	try{
		asio::ip::tcp::endpoint ep(asio::ip::address::from_string(raw_ip_address), port_num);
		//asio::io_service ios;
		asio::io_context ioc;
		asio::ip::tcp::socket sock(ioc, ep.protocol());
		sock.connect(ep);	//客户端连接
		write_to_socket(sock);	//给对端发数据
	}
	catch (system::system_error& e) {
		std::cout << "Error occured! Error code = " << e.code()
			<< ". Message: " << e.what();
		return e.code().value();
	}
	return 0;
}

//一次性发完，前面没发完的会阻塞在那里发完
int send_data_by_send() {
	std::string raw_ip_address = "192.168.3.11";
	unsigned short port_num = 3333;
	try {
		asio::ip::tcp::endpoint ep(asio::ip::address::from_string(raw_ip_address), port_num);
		//asio::io_service ios;
		asio::io_context ioc;
		asio::ip::tcp::socket sock(ioc, ep.protocol());
		sock.connect(ep);	//客户端连接
		//write_to_socket(sock);	//给对端发数据
		std::string buf = "Hello World";
		//全部发完再返回
		int send_length = sock.send(asio::buffer(buf.c_str(), buf.length()));
		if (send_length <= 0) {
			return 0;
		}
	}
	catch (system::system_error& e) {
		std::cout << "Error occured! Error code = " << e.code()
			<< ". Message: " << e.what();
		return e.code().value();
	}
	return 0;
}

int send_data_by_wirte() {
	std::string raw_ip_address = "127.0.0.1";
	unsigned short port_num = 3333;
	try {
		asio::ip::tcp::endpoint
			ep(asio::ip::address::from_string(raw_ip_address),
				port_num);
		asio::io_service ios;
		// Step 1. Allocating and opening the socket.
		asio::ip::tcp::socket sock(ios, ep.protocol());
		sock.connect(ep);
		std::string buf = "Hello World!";
		int send_length = asio::write(sock, asio::buffer(buf.c_str(), buf.length()));
		if (send_length <= 0) {
			std::cout << "send failed" << std::endl;
			return 0;
		}
	}
	catch (system::system_error& e) {
		std::cout << "Error occured! Error code = " << e.code()
			<< ". Message: " << e.what();
		return e.code().value();
	}
	return 0;
}


std::string read_from_socket(asio::ip::tcp::socket& sock) {
	const unsigned char MESSAGE_SIZE = 7;
	char buf[MESSAGE_SIZE];
	std::size_t total_bytes_read = 0;
	while (total_bytes_read != MESSAGE_SIZE) {
		total_bytes_read += sock.read_some(
			asio::buffer(buf + total_bytes_read,
				MESSAGE_SIZE - total_bytes_read));
	}
	return std::string(buf, total_bytes_read);
}


int read_data_by_read_some() {
	std::string raw_ip_address = "127.0.0.1";
	unsigned short port_num = 3333;
	try {
		asio::ip::tcp::endpoint
			ep(asio::ip::address::from_string(raw_ip_address),
				port_num);
		//asio::io_service ios;
		asio::io_context ioc;
		asio::ip::tcp::socket sock(ioc, ep.protocol());
		sock.connect(ep);
		read_from_socket(sock);
	}
	catch (system::system_error& e) {
		std::cout << "Error occured! Error code = " << e.code()
			<< ". Message: " << e.what();
		return e.code().value();
	}
	return 0;
}

//可以一次性同步接收对方发送的数据
int read_data_by_receive() {
	std::string raw_ip_address = "127.0.0.1";
	unsigned short port_num = 3333;
	try {
		asio::ip::tcp::endpoint
			ep(asio::ip::address::from_string(raw_ip_address),
				port_num);
		asio::io_service ios;
		asio::ip::tcp::socket sock(ios, ep.protocol());
		sock.connect(ep);
		const unsigned char BUFF_SIZE = 7;
		char buffer_receive[BUFF_SIZE];
		int receive_length = sock.receive(asio::buffer(buffer_receive, BUFF_SIZE));
		if (receive_length <= 0) {
			std::cout << "receive failed" << std::endl;
		}
	}
	catch (system::system_error& e) {
		std::cout << "Error occured! Error code = " << e.code()
			<< ". Message: " << e.what();
		return e.code().value();
	}
	return 0;
}

//可以一次性同步读取对方发送的数据
int read_data_by_read() {
	std::string raw_ip_address = "127.0.0.1";
	unsigned short port_num = 3333;
	try {
		asio::ip::tcp::endpoint
			ep(asio::ip::address::from_string(raw_ip_address),
				port_num);
		asio::io_service ios;
		asio::ip::tcp::socket sock(ios, ep.protocol());
		sock.connect(ep);
		const unsigned char BUFF_SIZE = 7;
		char buffer_receive[BUFF_SIZE];
		int receive_length = asio::read(sock, asio::buffer(buffer_receive, BUFF_SIZE));
		if (receive_length <= 0) {
			std::cout << "receive failed" << std::endl;
		}
	}
	catch (system::system_error& e) {
		std::cout << "Error occured! Error code = " << e.code()
			<< ". Message: " << e.what();
		return e.code().value();
	}
	return 0;
}