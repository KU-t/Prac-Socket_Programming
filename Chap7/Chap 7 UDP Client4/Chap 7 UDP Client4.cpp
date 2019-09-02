#pragma comment(lib, "ws2_32")

#include <WinSock2.h>
#include <stdlib.h>
#include <stdio.h>

#define SERVERIP	"127.0.0.1"
#define SERVERPORT	9000
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

	// ���� �ʱ�ȭ
	WSADATA wsa;
	if (WSAStartup(MAKEWORD(2, 2), &wsa) != 0)
		return 1;

	// socket() : �������� ����
	SOCKET sock = socket(AF_INET, SOCK_DGRAM, 0);
	if (sock == INVALID_SOCKET)
		err_quit("socket()");

	// ���� �ּ� ����ü �ʱ�ȭ
	SOCKADDR_IN serveraddr;
	ZeroMemory(&serveraddr, sizeof(serveraddr));
	serveraddr.sin_family = AF_INET;
	serveraddr.sin_addr.s_addr = inet_addr(SERVERIP);
	serveraddr.sin_port = htons(SERVERPORT);

	// ������ �ѽſ� ����� ����
	SOCKADDR_IN peeraddr;
	int addrlen;
	char buf[BUFSIZE + 1];
	int len;

	// ������ ������ ���
	while(true){
		// ������ �Է�
		printf("[ Send DATA ] ");
		if (fgets(buf, BUFSIZE + 1, stdin) == NULL)
			break;

		// '\n' ���� ����
		len = strlen(buf);
		if (buf[len - 1] == '\n')
			buf[len - 1] = '\0';
		if (strlen(buf) == 0)
			break;

		// ������ ������ ������
		// sendto(��� ����, ������ ���� �ּ�, ������ ���� ũ��, �ɼ�[��κ� 0], ������ �ּ�, �ּ� ũ��) 
		// - ���� ���α׷� �����͸� �ü���� �۽� ���ۿ� ������ ����
		// - ���� IP�ּ� & ��Ʈ��ȣ ���� // ���� IP�ּ� & ��Ʈ��ȣ ����
		retval = sendto(sock, buf, strlen(buf), 0, (SOCKADDR*)&serveraddr, sizeof(serveraddr));
		if (retval == SOCKET_ERROR) {
			err_display("sendto()");
			continue;
		}
		printf("[ UDP Client ] Send %d Bytes. \n", retval);

		// ������ ���� ������ �ޱ�
		// recvfrom(��� ����[�̸� ����], ������ ���� �ּ�, ������ ���� ũ��, �ɼ�[��κ� 0], �۽��� �ּ�, �ּ� ũ�� )
		// - �ü���� ���� ���ۿ� ������ �����͸� ���� ���α׷��� ����
		addrlen = sizeof(peeraddr);
		retval = recvfrom(sock, buf, BUFSIZE, 0, (SOCKADDR*)&peeraddr, &addrlen);
		if (retval == SOCKET_ERROR) {
			err_display("recvfrom()");
			continue;
		}

		// �۽����� IP �ּ� üũ
		// peeraddr�� serveraddr�� Ʋ�� ���, ��� �ҷ�
		if (memcmp(&peeraddr, &serveraddr, sizeof(peeraddr))) {
			printf("[Error] Wrong DATA");
			continue;
		}

		// ���� ������ ���
		buf[retval] = '\0';
		printf("[ UDP Client ] Receive %d Bytes. \n", retval);
		printf("[ RECEIVE DATA ] %s \n \n", buf);
	}

	// closesocket()
	closesocket(sock);

	// ���� ����
	WSACleanup();
	return 0;
}

/*

	<< connect() ���� ���� >>

	[ connect() �̿�O ]
	connect() : ���� �ּҸ� ����
	- send() & recv() : connect()�� ������ �����͸� ���
		(������ ���� �� �ٸ� ������ ����x)

	[ connect() �̿�X ]
	- sendto() & recvfrom() : �Լ� �Ķ���Ϳ� ���� �ּ� �ʿ� (�ּ� ũ�� ����)

	------
	
	sendto()
	- UDP & TCP ��� ��� ���� (TCP�� ���, to/tolen ���� & �ɼ� ��� ����)
	- UDP���� ���� �� �ִ� ������ ���� (���־� ��Ʃ�������)
	- ���� �� �����ʹ� ������ (��밡 �޾Ҵ��� Ȯ�� �Ұ�)

	recvfrom()
	- UDP & TCP ��� ��� ���� (TCP�� ���, from/fromlen ���� & �ɼ� ��� ����)
	- ���ŵ� �����Ͱ� ���ۺ��� ũ�� ���� �����ʹ� ������ >> ���� ũ�� �߿�

*/