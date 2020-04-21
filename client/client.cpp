#include "client.h"


client::client()
{
	user = 0;
	writing = 0;
	serverAddr.sin_family = PF_INET;
	serverAddr.sin_port = SERVER_PORT;
	serverAddr.sin_addr.s_addr = inet_addr(SERVER_IP);//将字符串类型转换uint32_t

}

void client::init()
{
	int   Ret;
	WSADATA   wsaData;                        // 用于初始化套接字环境
											  // 初始化WinSock环境
	if ((Ret = WSAStartup(MAKEWORD(2, 2), &wsaData)) != 0)
	{
		printf("WSAStartup()   failed   with   error   %d\n", Ret);
		WSACleanup();

	}

	user = socket(AF_INET, SOCK_STREAM, 0);//采用ipv4,TCP传输
	if (user <= 0)
	{
		perror("establish client faild");
		printf("Error at socket(): %ld\n", WSAGetLastError());
		exit(1);
	};
	printf("establish succesfully\n");//创建成功
									  //阻塞式的等待服务器连接
	if (connect(user, (const sockaddr *)&serverAddr, sizeof(serverAddr)) < 0)
	{
		perror("connect to server faild");
		printf("Error at socket(): %ld\n", WSAGetLastError());
		exit(1);
	}
	printf("connect IP:%s  Port:%d  succesfully\n", SERVER_IP, SERVER_PORT);//创建成功
}

void client::process()
{
	char recvbuf[1024];
	fd_set fdread, fedwrite;
	FD_ZERO(&fdread);//将fds清零
	FD_ZERO(&fedwrite);//将fds清零

	init();

	while (1)
	{
		FD_SET(user, &fdread);
		if (writing == 0) FD_SET(user, &fedwrite);

		struct timeval timeout = { 1,0 };//每个Select等待三秒
		switch (select(0, &fdread, &fedwrite, NULL, &timeout))
		{
		case -1:
		{
			//perror("select");
			printf("Error at socket(): %ld\n", WSAGetLastError());
			/*exit(1);*/
			break;
		}
		case 0:
		{
			//printf("select timeout......");
			break;
		}
		default:
		{
			if (FD_ISSET(user, &fdread))//则有读事件
			{
				int size = recv(user, recvbuf, sizeof(recvbuf) - 1, 0);
				if (size > 0)
				{
					printf("server:%s\n", recvbuf);
					memset(recvbuf, '\0', sizeof(recvbuf));
				}
				else if (size == 0)
				{
					printf("server is closed\n");
					exit(1);
				}
			}
			if (FD_ISSET(user, &fedwrite))
			{
				FD_ZERO(&fedwrite);//将fedwrite清零
				writing = 1;//表示正在写作
				thread sendtask(bind(&client::sendata, this));
				sendtask.detach();//将子线程和主进程分离且互相不影响
			}

			break;
		}
		}

	}

}

void client::sendata()
{
	char sendbuf[1024];
	char middle[1024];

	cin.getline(sendbuf, 1024);//读取一行
	send(user, sendbuf, sizeof(sendbuf) - 1, 0);
	writing = 0;
}
