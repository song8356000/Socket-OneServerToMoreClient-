#pragma once


//头文件引用
#include <winsock2.h>  
#include <stdio.h>
#include <vector>
#include<ws2tcpip.h>//定义socklen_t

using namespace std;


#pragma comment(lib, "WS2_32")  // 链接到WS2_32.lib 


#define SERVER_IP "127.0.0.1"// 默认服务器端IP地址
#define SERVER_PORT 8888// 服务器端口号


class server
{
public:
	server();
	void init();
	void process();

private:
	int listener;//监听套接字
	sockaddr_in  serverAddr;//IPV4的地址方式
	vector <int> socnum;//存放创建的套接字
};
