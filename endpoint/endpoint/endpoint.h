#pragma once
//ʹ��extern��Ҫ�����ض���
/**********************************************/
/*�׽��ֵĴ�����connect��bind��*/
/*�����ͻ��˺ͷ���˵Ķ˵�*/
extern int clint_end_point();	//�ͻ���ȥ���ӶԶ˵�һ���˵�
extern int server_end_point();	//������������һ��ͨ�ŵĶ˵� 
/*���ڴ����׽���*/
extern int create_tcp_socket();	//����TCP��socket
extern int create_acceptor_socket();	//�����������˵�socket
/*�൱�ڷ������е�bind*/
extern int bind_acceptor_socket(); //��socket
/*�൱�ڿͻ����е�connect*/
extern int connect_to_end();		//���ӷ�����ָ���Ķ˵�
extern int dns_connect_to_end();		//��������
/*�൱�ڷ������е�accept*/
extern int accept_new_connection();	//��������������



/***********************************************/
/*buffer�ṹ��ͬ����д*/
extern void use_const_buffer();		//�ַ�������ת��������boost�ķ��ͽӿ�sendҪ��Ĳ���ΪConstBufferSequence����
extern void use_buffer_str();		//�򻯰汾
//��������ô��
extern void use_buffer_array();


//ͬ����д
