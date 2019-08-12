#pragma comment(lib, "ws2_32")
#include <winsock2.h>
#include <WS2tcpip.h>
#include <stdlib.h>
#include <stdio.h>

#define SERVERIP "::1"		// --- IPv6
#define SERVERPORT 9000
#define BUFSIZE 512

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
	printf("[%s] %s", msg, (char *)lpMsgBuf);
	// �ý��� �Ҵ� �޸� ��ȯ
	LocalFree(lpMsgBuf);
}

// ����� ���� ��Ƽ�� �����Լ�
// recvn()�� recv()�� ���´� ����
int recvn(SOCKET s, char *buf, int len, int flags) {
	// ���������� ȣ���ϴ� recv() �Լ��� return�� ����
	int received;
	// �������α׷� ������ �����ּ�, �����͸� ���� ������ ptr�� ����
	char *ptr = buf;
	// ���� ���� ���� �������� ũ��. �����͸� ���� ������ left�� ����
	int left = len;

	// �������� �����Ͱ� ������ ��� ����
	while (left > 0) {
		received = recv(s, ptr, left, flags);

		// �Լ� ȣ�⿡ ������ �ִ� ���,
		if (received == SOCKET_ERROR)
			return SOCKET_ERROR;

		// recv() �Լ��� return���� 0 (���� ����)�� ���, ( == ����(����) ����)
		else if (received == 0)
			break;

		// ���� ����
		left -= received;
		ptr += received;
	}

	// �������̶�� 0
	return (len - left);
}

int main(int argc, char *argv[]) {
	int retval;

	// ���� �ʱ�ȭ
	WSADATA wsa;
	if (WSAStartup(MAKEWORD(2, 2), &wsa) != 0)
		return 1;

	// socket()
	SOCKET sock = socket(AF_INET6, SOCK_STREAM, 0);			// --- IPv6
	if (sock == INVALID_SOCKET)
		err_quit("socket()");

	// connect(������ ����� ���� ����, ���� �ּ� ����ü, �����ּ� ����ü ����)
	// - TCP �������ݿ��� ������ ���� ���� ����
	// ���� ����ü �ʱ�ȭ (IP, Port)
	SOCKADDR_IN6 serveraddr;
	ZeroMemory(&serveraddr, sizeof(serveraddr));
	serveraddr.sin6_family = AF_INET6;				// --- IPv6
	int addrlen = sizeof(serveraddr);				// --- IPv6
	WSAStringToAddress((LPSTR)SERVERIP, AF_INET6, NULL, (SOCKADDR *)&serveraddr, &addrlen);	// --- IPv6
	serveraddr.sin6_port = htons(SERVERPORT);		// --- IPv6
	retval = connect(sock, (SOCKADDR *)&serveraddr, sizeof(serveraddr));
	if (retval == SOCKET_ERROR)
		err_quit("connect()");

	// ������ ��ſ� ����� ����
	// �ۼ��� ������ ���� ���� & ���� ����
	char buf[BUFSIZE + 1];
	int len;

	// ������ ������ ���
	while (true) {
		// ������ �Է�
		printf("\n[Send Data] ");
		// ����ڷκ��� ���ڿ� �Է�
		if (fgets(buf, BUFSIZE + 1, stdin) == NULL)
			break;

		// '\n' ���� ����
		len = strlen(buf);
		if (buf[len - 1] == '\n')
			buf[len - 1] = '\0';
		// ����ڰ� ENTER�� ������ ��,
		if (strlen(buf) == 0)
			break;

		// ������ ������
		// retval == strlen(buf) [����]
		retval = send(sock, buf, strlen(buf), 0);
		if (retval == SOCKET_ERROR) {
			err_display("send()");
			break;
		}
		printf("[TCP client] %d Byte - send \n", retval);

		// ������ �ޱ�
		// �������� ���� ������ ũ�⸦ �˱⶧���� recvn() �̿�
		retval = recvn(sock, buf, retval, 0);
		if (retval == SOCKET_ERROR) {
			err_display("recv()");
			break;
		}
		else if (retval == 0)
			break;

		// ���� ������ ���
		// ���� ������ ���� '\0'�� �߰��� ���
		buf[retval] = '\0';
		printf("[TCP client] %d Byte - recive \n", retval);
		printf("[Recive Data] %s \n", buf);
	}

	// closesocket()
	closesocket(sock);

	// ���� ����
	WSACleanup();
	return 0;
}