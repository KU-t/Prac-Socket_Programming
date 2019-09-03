#pragma comment(lib, "ws2_32")

#include <winsock2.h>
#include <stdlib.h>
#include <stdio.h>

#define REMOTEIP	"255.255.255.255"
#define REMOTEPORT	9000
#define BUFSIZE		512

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

int main(int argc, char* argv[]) {
	int retval;

	//���� �ʱ�ȭ
	WSADATA wsa;
	if (WSAStartup(MAKEWORD(2, 2), &wsa) != 0)
		return 1;

	// socket()
	SOCKET sock = socket(AF_INET, SOCK_DGRAM, 0);
	if (sock == INVALID_SOCKET)
		err_quit("socket()");

	// ��ε�ĳ���� Ȱ��ȭ
	// setsockopt() : ������ �ڼ���
	// - SO_BRODCAST : ��ε�ĳ��Ʈ Ȱ��ȭ
	BOOL bEnable = TRUE;
	retval = setsockopt(sock, SOL_SOCKET, SO_BROADCAST, (char*)&bEnable, sizeof(bEnable));
	if (retval == SOCKET_ERROR)
		err_quit("setsockopt()");

	// ���� �ּ� ����ü �ʱ�ȭ
	SOCKADDR_IN remoteaddr;
	ZeroMemory(&remoteaddr, sizeof(remoteaddr));
	remoteaddr.sin_family = AF_INET;
	remoteaddr.sin_addr.s_addr = inet_addr(REMOTEIP);
	remoteaddr.sin_port = htons(REMOTEPORT);

	// ������ ��ſ� ����� ����
	char buf[BUFSIZE + 1];
	int len;

	// ��ε�ĳ��Ʈ ������ ������
	// - fgets() : ����� �Է� -> sendto() : ������ ������
	while (true) {	
		// ������ �Է�
		printf("\n[ Send DATA ] ");
		if (fgets(buf, BUFSIZE + 1, stdin) == NULL)
			break;

		// '/n' ���� ����
		len = strlen(buf);
		if (buf[len - 1] == '\n')
			buf[len - 1] = '\0';
		if (strlen(buf) == 0)
			break;

		// ������ ������
		retval = sendto(sock, buf, strlen(buf), 0, (SOCKADDR*)&remoteaddr, sizeof(remoteaddr));
		if (retval == SOCKET_ERROR) {
			err_display("sento()");
			continue;
		}
		printf("[ UDP ] Send %d Bytes. \n", retval);
	}
	// closesocket()
	closesocket(sock);

	// ���� ����
	WSACleanup();
	return 0;
}