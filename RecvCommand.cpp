#include "Header.h"
#include "MyTime.h"
#include "MyThread.h"

extern bool g_progFlag;

void RecvCommand()
{
	int sockfd;
	struct sockaddr_in saddr;
	char recvline[382];
	struct sockaddr_in presaddr;
	MyTime m_time;
	MyThread m_thread;

#ifdef WIN32
	WSADATA wsa;
	WSAStartup(MAKEWORD(2, 2), &wsa); //initial Ws2_32.dll by a process
#endif
	bool flag = true;
	sockfd = socket(AF_INET, SOCK_DGRAM, 0);
	if (sockfd < 0)
	{
		//writeLog("create serviceBroadcast failed.\n");
		flag = false;
	}

	saddr.sin_family = AF_INET;
	saddr.sin_addr.s_addr = htonl(INADDR_ANY);
	//saddr.sin_addr.s_addr = inet_addr("193.1.103.16");
	saddr.sin_port = htons(COMMAND);

	if (flag == true && bind(sockfd, (struct sockaddr*)&saddr, sizeof(saddr)) < 0)
	{
		//writeLog("bind serviceBroadcast failed.\n");
		flag = false;
	}
#ifdef WIN32
	int len = sizeof(sockaddr_in);
#endif
#ifdef linux
	socklen_t len = sizeof(sockaddr_in);
#endif
#ifdef DVXWORK
	int len = sizeof(sockaddr_in);
#endif
	int r;
	char sign[5];
	char command[1];
	while (flag&&g_progFlag)
	{
		memset(recvline, 0, sizeof(recvline));
		r = recvfrom(sockfd, recvline, sizeof(recvline), 0, (struct sockaddr*)&presaddr, &len);
		memset(sign, 0, 5);
		// 获得报文标识
		memcpy(sign, recvline, 4);
		memcpy(command, recvline + 4, 1);

		if (strcmp(sign, "S102") == 0) // 表示全盘扫描
		{
			m_time.CoutTime();
			cout << "收到的报文是：S102" << endl;
			cout << "收到全盘扫描报文" << endl;
			Stru_Scans scans;
			int index = 4;
			memcpy(scans.browserID, recvline + index, sizeof(scans.browserID));
			// 获得申请ID，该信息是在返回扫描结果时有效，对本软件无意义
			index += sizeof(scans.browserID);
			memcpy(scans.deviceID, recvline + index, sizeof(scans.deviceID));
			// 获得设备ID，该信息是在返回扫描结果时有效，对本软件无意义
			index += sizeof(scans.deviceID);
			memcpy(scans.compID, recvline + index, sizeof(scans.compID));
			index += sizeof(scans.compID);
			// 获得扫描路径
			memcpy(scans.scanPath, recvline + index, sizeof(scans.scanPath));
			printf("the scans Path:%s\n", scans.scanPath);
			m_thread.FonudScanFiles(scans);
		}
		if (strcmp(sign, "S103") == 0) // 表示根据指定类型扫描
		{
			m_time.CoutTime();
			cout << "收到的报文是：S103" << endl;
			cout << "收到快速扫描报文" << endl;
			Stru_Scans scans;
			int index = 4;
			memcpy(scans.browserID, recvline + index, sizeof(scans.browserID));
			// 获得申请ID，该信息是在返回扫描结果时有效，对本软件无意义
			index += sizeof(scans.browserID);
			memcpy(scans.deviceID, recvline + index, sizeof(scans.deviceID));
			// 获得设备ID，该信息是在返回扫描结果时有效，对本软件无意义
			index += sizeof(scans.deviceID);
			memcpy(scans.compID, recvline + index, sizeof(scans.compID));
			index += sizeof(scans.compID);
			memcpy(scans.scanType, recvline + index, sizeof(scans.scanType));
			index += sizeof(scans.scanType);
			memcpy(scans.scanPath, recvline + index, sizeof(scans.scanPath));
			m_thread.FonudScanFiles(scans);
		}
		if (strcmp(sign, "S105") == 0)
		{
			m_time.CoutTime();
			cout << "收到的报文是：S105" << endl;
			Stru_Command com;
			memset(com.command, 0, sizeof(com.command));
			memset(com.sign, 0, sizeof(com.sign));
			int index = 4;
			// 获取查询类型
			memcpy(com.command, command, strlen(command));
			index += sizeof(com.command);

			cout << "收到获取进程信息报文" << endl;
			Stru_Module module;
			int dex = 5;
			memcpy(&module, recvline + dex, sizeof(Stru_Module));
			m_thread.FoundModuleProxy(module);

		}
		if (strcmp(sign, "S106") == 0)
		{
			m_time.CoutTime();
			cout << "收到的报文是：S106" << endl;
			Stru_Command com;
			memset(com.command, 0, sizeof(com.command));
			memset(com.sign, 0, sizeof(com.sign));
			int index = 4;
			// 获取查询类型
			memcpy(com.command, command, strlen(command));
			index += sizeof(com.command);

			cout << "收到获取磁盘信息报文" << endl;
			Stru_Disk disk;
			int dex = 5;
			memcpy(&disk, recvline + dex, sizeof(Stru_Disk));
			m_thread.FoundDiskThread(disk);
		}
	}
#ifdef WIN32
	closesocket(sockfd);
	WSACleanup();
#endif
#ifdef linux
	close(sockfd);
#endif
#ifdef DVXWORK
	close(sockfd);
#endif
}

