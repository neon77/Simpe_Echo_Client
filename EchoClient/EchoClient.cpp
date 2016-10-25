// EchoClient.cpp : 콘솔 응용 프로그램에 대한 진입점을 정의합니다.
//

#include "stdafx.h"

std::string CurrentTime()
{
	time_t now = ::time(0);
	struct tm tmstruct;
	localtime_s(&tmstruct, &now);
	char buf[100];
	strftime(buf, sizeof(buf), "[%Y-%m-%d %X] ", &tmstruct);
	return buf;
}

int main(int argc, char* argv[])
{
	if (argc < 3)
	{
		std::cout << CurrentTime() << "need ip & port!" << std::endl;
		system("pause");
		return -1;
	}
	std::string serverip(argv[1]);
	unsigned short serverport = atoi(argv[2]);
	std::ofstream filelog(std::string(argv[2]) + ".log");
	bool echo = false;
	if (3 < argc)
	{
		if ("-echo" == std::string(argv[3]))
		{
			echo = true;
			std::cout << CurrentTime() << "echo mode" << std::endl;
			filelog << CurrentTime() << "echo mode" << std::endl;
		}
		else
		{
			std::cout << CurrentTime() << "connect mode" << std::endl;
			filelog << CurrentTime() << "connect mode" << std::endl;
		}
	}
	else
	{
		std::cout << CurrentTime() << "connect mode" << std::endl;
		filelog << CurrentTime() << "connect mode" << std::endl;
	}

	WSADATA wsaData;
	int ret = WSAStartup(MAKEWORD(2, 2), &wsaData);
	if (NO_ERROR != ret)
	{
		std::cout << CurrentTime() << "WSAStartup failed with error : " << ret << std::endl;
		filelog << CurrentTime() << "WSAStartup failed with error : " << ret << std::endl;
		system("pause");
		return -2;
	}

	SOCKET connectsocket = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
	if (INVALID_SOCKET == connectsocket)
	{
		std::cout << CurrentTime() << "create connect socket failed!" << std::endl;
		std::cout << CurrentTime() << "socket error [ " << WSAGetLastError() << " ]" << std::endl;
		filelog << CurrentTime() << "create connect socket failed!" << std::endl;
		filelog << CurrentTime() << "socket error [ " << WSAGetLastError() << " ]" << std::endl;
		system("pause");
		return -3;
	}

	struct sockaddr_in serveraddr;
	memset(&serveraddr, 0, sizeof(serveraddr));
	serveraddr.sin_family = AF_INET;
	serveraddr.sin_port = htons(serverport);
	::inet_pton(AF_INET, serverip.c_str(), &(serveraddr.sin_addr));

	std::cout << CurrentTime() << "connecting " << serverip << " : " << serverport << std::endl;

	ret = connect(connectsocket, (SOCKADDR*)&serveraddr, sizeof(serveraddr));
	if (SOCKET_ERROR == ret)
	{
		std::cout << CurrentTime() << "connect server failed!" << std::endl;
		std::cout << CurrentTime() << "socket error [ " << WSAGetLastError() << " ]" << std::endl;
		filelog << CurrentTime() << "connect server failed!" << std::endl;
		filelog << CurrentTime() << "socket error [ " << WSAGetLastError() << " ]" << std::endl;
		closesocket(connectsocket);
		WSACleanup();
		system("pause");
		return -4;
	}

	if (true == echo)
	{
		std::cout << CurrentTime() << "staring echo!" << std::endl;
		filelog << CurrentTime() << "staring echo!" << std::endl;
	}
	else
	{
		std::cout << CurrentTime() << "waiting..." << std::endl;
		filelog << CurrentTime() << "waiting..." << std::endl;
	}

	char recvbuffer[2048];
	int recvbufferlength = (int)sizeof(recvbuffer);
	int i = 1;
	std::string sendbuffer;
	while (true)
	{
		if (true == echo)
		{
			sendbuffer = std::to_string(i);
			ret = send(connectsocket, sendbuffer.c_str(), (int)sendbuffer.length(), 0);
			if (SOCKET_ERROR == ret)
			{
				std::cout << CurrentTime() << "send failed!" << std::endl;
				std::cout << CurrentTime() << "socket error [ " << WSAGetLastError() << " ]" << std::endl;
				filelog << CurrentTime() << "send failed!" << std::endl;
				filelog << CurrentTime() << "socket error [ " << WSAGetLastError() << " ]" << std::endl;
				break;
			}
		}
		memset(recvbuffer, 0, sizeof(recvbuffer));
		ret = recv(connectsocket, recvbuffer, recvbufferlength, 0);
		if (0 < ret)
		{
			std::cout << CurrentTime() << ret << " bytes received!" << std::endl;
			filelog << CurrentTime() << ret << " bytes received!" << std::endl;
		}
		else if (0 == ret)
		{
			std::cout << CurrentTime() << "connection closed\n" << std::endl;
			filelog << CurrentTime() << "connection closed\n" << std::endl;
			break;
		}
		else
		{
			std::cout << CurrentTime() << "recv failed!" << std::endl;
			std::cout << CurrentTime() << "socket error [ " << WSAGetLastError() << " ]" << std::endl;
			filelog << CurrentTime() << "recv failed!" << std::endl;
			filelog << CurrentTime() << "socket error [ " << WSAGetLastError() << " ]" << std::endl;
			break;
		}
		++i;
		::Sleep(5000);
	}

	closesocket(connectsocket);
	WSACleanup();

	system("pause");

	return 0;
}

