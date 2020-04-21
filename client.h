#pragma once

//头文件引用
#include<conio.h>
#include <iostream>
#include <thread>
#include <winsock2.h>  
#include <stdio.h>
#include<ws2tcpip.h>//定义socklen_t

#pragma comment(lib, "WS2_32")  // 链接到WS2_32.lib 

using namespace std;



#define SERVER_IP "127.0.0.1"// 默认服务器端IP地址
#define SERVER_PORT 8888// 服务器端口号



class client
{
public:
	client();
	void init();
	void process();


private:
	int user;
	int writing;
	sockaddr_in  serverAddr;//IPV4的地址方式包括服务端地址族、服务端IP地址、服务端端口号
	void sendata();
};
