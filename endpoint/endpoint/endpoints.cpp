#include "endpoint.h"
#include <boost/asio.hpp>
#include <iostream>
using namespace boost;
//�ͻ���
int clint_end_point() {
	//�Զ˵�ַ�Ͷ˿ں�
	std::string raw_ip_address = "127.4.8.1";
	unsigned short port_num = 3333;

	//֪���Զ˵�ַ�Ͷ˿ںź���Ҫת����asioʹ�õ�IP��ַ
	boost::system::error_code er;	//�����룬�����۲�ת������ʱ���ֵĴ���
	asio::ip::address ip_address = asio::ip::address::from_string(raw_ip_address, er);
	if (er.value() != 0) {	//��Ϊ0 ��ʾת��ʧ���д���
		std::cout << "Failed to parse the IP address. Error code = " << er.value() << " . Message is" << er.message();
		return er.value(); 
	}

	asio::ip::tcp::endpoint ep(ip_address, port_num);
	return 0;
}

//�����
int server_end_point() {
	unsigned short port_num = 3333;
	asio::ip::address ip_address = asio::ip::address_v6::any();
	asio::ip::tcp::endpoint ep(ip_address, port_num);
	return 0;
}


//����tcpsocket
int create_tcp_socket() {
	asio::io_context ioc;	//���������ģ���֪��Ϊ�Ǹ�socket�����
	asio::ip::tcp protocol = asio::ip::tcp::v4();	//����ipv4��Э��
	//����socket
	asio::ip::tcp::socket sock(ioc);
	//�ж�socket�Ƿ�򿪳ɹ�
	boost::system::error_code ec;
	sock.open(protocol, ec);
	if (ec.value() != 0) {
		std::cout << "Failed to open the socket. Error code = " << ec.value() << " . Message is" << ec.message();
		return ec.value();
	}
	return 0;
}


//�������˻���Ҫ����һ��acceptor��socket�����������µ�����
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

	//��
	asio::ip::tcp::acceptor a(ios, asio::ip::tcp::endpoint(asio::ip::tcp::v4(), 3333));
	return 0;
}

//�������˰�acceptor
int bind_acceptor_socket() {
	unsigned short port_num = 3333;
	asio::ip::tcp::endpoint ep(asio::ip::address_v4::any(), port_num);
	//�÷�����֪��acceptor�����ĸ�������
	asio::io_context ios; 
	asio::ip::tcp::acceptor acceptor(ios, ep.protocol());	//����һ��acceptor
	boost::system::error_code ec;
	acceptor.bind(ep, ec);	//���а�
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

//�ͻ������ӷ�����ָ���Ķ˵�
int connect_to_end() {
	//��������ַ�Ͷ˿�
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

//�����������������������ӷ�����
int dns_connect_to_end() {
	std::string host = "llfc.club";
	std::string port_num = "3333";
	asio::io_context ios;	//����һ������
	//����DNS������
	asio::ip::tcp::resolver::query resolver_query(host, port_num, asio::ip::tcp::resolver::query::numeric_service);
	asio::ip::tcp::resolver resolver(ios);

	try {
		asio::ip::tcp::resolver::iterator it = resolver.resolve(resolver_query);	//���н���������һ��������
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

//��������������
int accept_new_connection() {
	//���ü������еĴ�С
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

//�ַ�������ת��������boost�ķ��ͽӿ�sendҪ��Ĳ���ΪConstBufferSequence����
void use_const_buffer() {
	std::string buf = "hello world!";
	asio::const_buffer  asio_buf(buf.c_str(), buf.length());
	std::vector<asio::const_buffer> buffers_sequence;
	buffers_sequence.push_back(asio_buf);
}

//Ҳ����ֱ����buffer����ת��Ϊsend��Ҫ�Ĳ�������
void use_buffer_str() {
	asio::const_buffers_1 output_buf = asio::buffer("hello world");
}

//����������
void use_buffer_array() {
	const size_t  BUF_SIZE_BYTES = 20;
	std::unique_ptr<char[] > buf(new char[BUF_SIZE_BYTES]);	//���õ����ڴ���յ������ˣ�new�����������׵�ַ����buf����
	auto input_buf = asio::buffer(static_cast<void*>(buf.get()), BUF_SIZE_BYTES);	//��Ҫ��ת����ָ������
}


/*********ͬ����д************/
void  write_to_socket(asio::ip::tcp::socket& sock) {
	std::string buf = "Hello World!";
	std::size_t  total_bytes_written = 0;
	//ѭ������
	//write_some����ÿ��д����ֽ���
	//total_bytes_written���Ѿ����͵��ֽ�����
	//ÿ�η���buf.length()- total_bytes_written)�ֽ�����  
	while (total_bytes_written != buf.length()) {
		total_bytes_written += sock.write_some(
			asio::buffer(buf.c_str() + total_bytes_written,
				buf.length() - total_bytes_written));
	}
}

//һ���Է�����
int send_data_by_write_some() {
	std::string raw_ip_address = "192.168.3.11";
	unsigned short port_num = 3333;
	try{
		asio::ip::tcp::endpoint ep(asio::ip::address::from_string(raw_ip_address), port_num);
		//asio::io_service ios;
		asio::io_context ioc;
		asio::ip::tcp::socket sock(ioc, ep.protocol());
		sock.connect(ep);	//�ͻ�������
		write_to_socket(sock);	//���Զ˷�����
	}
	catch (system::system_error& e) {
		std::cout << "Error occured! Error code = " << e.code()
			<< ". Message: " << e.what();
		return e.code().value();
	}
	return 0;
}

//һ���Է��꣬ǰ��û����Ļ����������﷢��
int send_data_by_send() {
	std::string raw_ip_address = "192.168.3.11";
	unsigned short port_num = 3333;
	try {
		asio::ip::tcp::endpoint ep(asio::ip::address::from_string(raw_ip_address), port_num);
		//asio::io_service ios;
		asio::io_context ioc;
		asio::ip::tcp::socket sock(ioc, ep.protocol());
		sock.connect(ep);	//�ͻ�������
		//write_to_socket(sock);	//���Զ˷�����
		std::string buf = "Hello World";
		//ȫ�������ٷ���
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

//����һ����ͬ�����նԷ����͵�����
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

//����һ����ͬ����ȡ�Է����͵�����
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