#pragma comment(lib, "ws2_32")
#include <winsock2.h>
#include <stdlib.h>
#include <stdio.h>

#define SERVERIP "127.0.0.1"
#define SERVERPORT 9000
#define BUFSIZE 50		// --- Fixed

// ���� �Լ� ���� ��� �� ���� - [���α׷� ����]
void err_quit(const char *msg) {
	LPVOID lpMsgBuf;
	// �����޼��� ���
	FormatMessage(
		// �����޼��� ���� ������ �˾Ƽ� �Ҵ� | �ü���κ��� ���� �޼��� ������
		FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM,
		// NULL , �����ڵ� - [��ü�� ���� �ڵ� Ȯ��]
		NULL, WSAGetLastError(),
		//�����޼��� ǥ�� ��� - ����� ������ �⺻ ���� ���
		MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT),
		// ���� ���ڿ� �����ּ�, nSize, Arguments
		(LPTSTR)&lpMsgBuf, 0, NULL);
	// �޼��� ���� ���� - [���� �޼��� Ȯ��]
	MessageBox(NULL, (LPCTSTR)lpMsgBuf, (LPCTSTR)msg, MB_ICONERROR);
	// �ý��� �Ҵ� �޸� ��ȯ
	LocalFree(lpMsgBuf);
	exit(1);
}

// ���� �Լ� ���� ��� - �׽�Ʈ��
// ���� err_quit()�� ����
void err_display(const char *msg) {
	LPVOID lpMsgBuf;
	FormatMessage(
		FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM,
		NULL, WSAGetLastError(),
		MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT),
		(LPTSTR)&lpMsgBuf, 0, NULL);
	printf("[%s] %s \n", msg, (char *)lpMsgBuf);
	// �ý��� �Ҵ� �޸� ��ȯ
	LocalFree(lpMsgBuf);
}

int main(int argc, char *argv[]) {
	int retval;

	// ���� �ʱ�ȭ
	WSADATA wsa;
	if (WSAStartup(MAKEWORD(2, 2), &wsa) != 0)
		return 1;

	// socket()
	SOCKET sock = socket(AF_INET, SOCK_STREAM, 0);
	if (sock == INVALID_SOCKET)
		err_quit("socket()");

	// connect(������ ����� ���� ����, ���� �ּ� ����ü, �����ּ� ����ü ����)
	// - TCP �������ݿ��� ������ ���� ���� ����
	// ���� ����ü �ʱ�ȭ (IP, Port)
	SOCKADDR_IN serveraddr;
	ZeroMemory(&serveraddr, sizeof(serveraddr));
	serveraddr.sin_family = AF_INET;
	serveraddr.sin_addr.s_addr = inet_addr(SERVERIP);
	serveraddr.sin_port = htons(SERVERPORT);
	retval = connect(sock, (SOCKADDR *)&serveraddr, sizeof(serveraddr));
	if (retval == SOCKET_ERROR)
		err_quit("connect()");

	// ������ ��ſ� ����� ����
	// �ۼ��� ������ ���� ���� & ���� ����
	char buf[BUFSIZE];		// --- Fixed
	const char *testdata[] = { "Hi", "Nice to meet you", "I have something to talk", "Me too" };	

	// ������ ������ ���
	for(int i = 0 ; i < 4 ; i++){
		// ������ �Է�
		memset(buf, '#', sizeof(buf));
		strncpy(buf, testdata[i], strlen(testdata[i]));

		// ������ ������
		// retval == strlen(buf) [����]
		retval = send(sock, buf, BUFSIZE, 0);
		if (retval == SOCKET_ERROR) {
			err_display("send()");
			break;
		}
		printf("[TCP client] %d Byte - send \n", retval);
	}

	// closesocket()
	closesocket(sock);

	// ���� ����
	WSACleanup();
	return 0;
}