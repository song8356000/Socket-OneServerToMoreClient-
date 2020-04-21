#include "server.h"




server::server()
{
	listener = 0;
	serverAddr.sin_family = PF_INET;
	serverAddr.sin_port = SERVER_PORT;
	serverAddr.sin_addr.s_addr = inet_addr(SERVER_IP);//将字符串类型转换uint32_t
}
//初始化函数，功能创建监听套接字，绑定端口，并进行监听
void server::init()
{
	int   Ret;
	WSADATA   wsaData;                        // 用于初始化套接字环境
											  // 初始化WinSock环境
	if ((Ret = WSAStartup(MAKEWORD(2, 2), &wsaData)) != 0)
	{
		printf("WSAStartup()   failed   with   error   %d\n", Ret);
		WSACleanup();

	}


	listener = socket(AF_INET, SOCK_STREAM, 0);//采用ipv4,TCP传输
	if (listener == -1) { printf("Error at socket(): %ld\n", WSAGetLastError()); perror("listener failed"); exit(1); }
	printf("创建成功\n");

	unsigned long ul = 1;
	if (ioctlsocket(listener, FIONBIO, (unsigned long*)&ul) == -1) { perror("ioctl failed"); exit(1); };
	///////////////////////////////////////////////////////////////////
	//中间的参数绑定的地址如果是IPV4则是///////////////////
	//struct sockaddr_in {
	//	sa_family_t    sin_family; /* address family: AF_INET */
	//	in_port_t      sin_port;   /* port in network byte order */
	//	struct in_addr sin_addr;   /* internet address */
	//};
	//Internet address. 
	//struct in_addr {
	//	uint32_t       s_addr;     /* address in network byte order */
	//}
	/////////////////////////////////////////////////////////////////
	if (bind(listener, (struct sockaddr *)&serverAddr, sizeof(serverAddr)) < 0)
	{
		perror("bind error");
		exit(1);
	}
	if (listen(listener, 6) < 0) { perror("listen failed"); exit(1); };
	socnum.push_back(listener);//将监听套接字加入
}

void server::process()
{

	int mount = 0;
	fd_set fds;
	FD_ZERO(&fds);//将fds清零
	init();
	//下面就是不断的检查
	printf("正在等待中\n");
	while (1)
	{
		mount = socnum.size();
		//将fds每次都重新赋值
		for (int i = 0; i<mount; ++i)
		{
			FD_SET(socnum[i], &fds);
		}

		struct timeval timeout = { 1,0 };//每个Select等待三秒
		switch (select(0, &fds, NULL, NULL, &timeout))
		{
		case -1:
		{
			perror("select\n");
			printf("Error at socket(): %ld\n", WSAGetLastError());
			printf("%d\n", mount);
			/*			for (int i = 0; i < mount; ++i)
			{
			printf("%d\n", socnum[i]);
			}*/
			Sleep(1000);
			break;
		}
		case 0:
		{
			//printf("select timeout......");
			break;
		}
		default:
		{
			//将数组中的每一个套接字都和剩余的额套接字进行比较得到当前的任务
			for (int i = 0; i < mount; ++i)
			{
				//如果第一个套接字可读的消息。就要建立连接
				if (i == 0 && FD_ISSET(socnum[i], &fds))
				{
					struct sockaddr_in client_address;
					socklen_t client_addrLength = sizeof(struct sockaddr_in);
					//返回一个用户的套接字
					int clientfd = accept(listener, (struct sockaddr*)&client_address, &client_addrLength);
					//添加用户，服务器上显示消息，并通知用户连接成功
					socnum.push_back(clientfd);
					printf("connect sucessfully\n");
					char ID[1024];
					sprintf(ID, "You ID is:%d", clientfd);
					char buf[30] = "welcome to yskn's chatroom\n";
					strcat(ID, buf);
					send(clientfd, ID, sizeof(ID) - 1, 0);//减去最后一个'/0'
				}
				if (i != 0 && FD_ISSET(socnum[i], &fds))
				{
					char buf[1024];
					memset(buf, '\0', sizeof(buf));
					int size = recv(socnum[i], buf, sizeof(buf) - 1, 0);
					//检测是否断线
					if (size == 0 || size == -1)
					{
						printf("remote client close,size is%d\n", size);

						//closesocket(socnum[i]);//先关闭这个套接字
						FD_CLR(socnum[i], &fds);//在列表列表中删除
						socnum.erase(socnum.begin() + i);//在vector数组中删除
					}
					//若是没有掉线
					else
					{
						printf("clint %d says: %s \n", socnum[i], buf);
						//发送给每个用户
						for (int j = 1; j < mount; j++)
						{
							char client[1024];
							sprintf(client, "client %d:", socnum[i]);
							strcat(client, buf);
							send(socnum[j], client, sizeof(client) - 1, 0);//如果
						}
					}

				}
			}
			break;
		}
		}


	}
}
