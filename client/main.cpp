#include <iostream>
#include <stdio.h>//����getline
#include <sys/socket.h>
#include <arpa/inet.h>//��ַ�صĿ�
#include <string.h>
using namespace std;

int main()
{
	//����
	int client = socket(AF_INET, SOCK_STREAM, 0);
	if (client == -1)
	{
		perror("socket error");
		exit(1);
	}
	//��
	struct sockaddr_in client_addr;
	client_addr.sin_family = AF_INET;
	client_addr.sin_port = htons(6969);
	client_addr.sin_addr.s_addr = inet_addr("192.168.148.128");

	if (-1 == connect(client, (struct sockaddr*)&client_addr, sizeof(client_addr)))
	{
		perror("connect error");
		return -1;
	}
	while (1)
	{
		char msg[1024];
		cin.getline(msg, 1024);
		//����
		send(client, msg, sizeof(msg), 0);
		//����
		char buf[1024];
		memset(buf, 0, sizeof(buf));
		int ret = recv(client, buf, sizeof(buf), 0);
		cout << buf << endl;
	}
	

	return 0;
}