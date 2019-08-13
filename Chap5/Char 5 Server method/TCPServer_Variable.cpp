/*

	TIME_WAIT ���� : closesocket()�Լ��� ȣ���� ������ TCP ��Ʈ�� �ٷ� �ı����� �ʰ� ��� �ӹ����� ����
		- �뷫 5������ ���

*/

#pragma comment(lib, "ws2_32")
#include <winsock2.h>
#include <stdio.h>
#include <stdlib.h>

#define SERVERPORT 9000
#define BUFSIZE 512		// --- Fixed

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

// ���� ���� ���۸� �ѹ��� �а� 1 ����Ʈ�� return		// --- variable
int _recv_ahead(SOCKET s, char *p) {
	// ������ ���� �Լ�, �Լ��� �����ص� ������ ����
	__declspec(thread) static int nbytes = 0;
	__declspec(thread) static char buf[1024];
	__declspec(thread) static char *ptr;

	// ���� ���� ���ۿ��� ���� �����Ͱ� ���ų� �������� ���,
	// - ���� �о� buf[]�� �����ϰ�, ptr�� �� ���� ����Ʈ�� ����Ű�� �Ѵ�.
	// recv(��� socket, ���� �������� ���� �ּ�, ���� ������ ũ��[���� ���α׷� ���ۺ��� �۾ƾ���], �ɼ�[��κ� 0]) 
	// - ���Ϲ��ۿ� ���� (���Ź���) // return : len
	// : �ü�� ���� ���� -> ������ ���� -> ���� ���α׷� ���ۿ� ����
	if (nbytes == 0 || nbytes == SOCKET_ERROR) {
		nbytes = recv(s, buf, sizeof(buf), 0);
		if (nbytes == SOCKET_ERROR)
			return SOCKET_ERROR;

		else if (nbytes == 0)
			return 0;

		ptr = buf;
	}

	// ���� nbytes�� 1����, ptr�� ����Ű�� �����͸� ���� p�� ����Ű�� ������ �־� ����
	--nbytes;
	*p = *ptr++;
	return 1;
}

// ����� ���� ������ ���� �Լ�							// --- variable
// : '\n'�� ���ö����� ������ �б�
// - ���� s���������͸� 1byte�� �о� buf�� ����Ű�� �޸� ���� ����
// - '\n'�� �����ų� �ִ� ����(maxlen - 1)�� �����ϸ� '\n'�� �ٿ��� ����
int recvline(SOCKET s, char *buf, int maxlen) {
	int n, nbytes;
	char c, *ptr = buf;

	for (n = 1; n < maxlen; n++) {
		nbytes = _recv_ahead(s, &c);
		// �� ���� �Ѿ���� ����
		if (nbytes == 1) {
			*ptr++ = c;
			// '\n'�̸� ������ ��
			if (c == '\n')
				break;
		}

		else if (nbytes == 0) {
			*ptr = 0;
			return n - 1;
		}

		else
			return SOCKET_ERROR;
	}

	*ptr = 0;
	return n;
}

int main(int argc, char* argv[]) {
	int retval;

	// ���� �ʱ�ȭ
	WSADATA wsa;
	if (WSAStartup(MAKEWORD(2, 2), &wsa) != 0)
		return 1;

	// socket()
	SOCKET listen_sock = socket(AF_INET, SOCK_STREAM, 0);
	if (listen_sock == INVALID_SOCKET)
		err_quit("socket()");


	// bind()
	// - socket �ּ� ����ü ���� �� �ʱ�ȭ
	SOCKADDR_IN serveraddr;
	ZeroMemory(&serveraddr, sizeof(serveraddr));
	// - IPv4, ���� IP, ���� port
	serveraddr.sin_family = AF_INET;
	serveraddr.sin_addr.s_addr = htonl(INADDR_ANY);
	serveraddr.sin_port = htons(SERVERPORT);
	// bind(Ŭ���̾�Ʈ ���� ���� ����[IP�ּ�, ����port ����x], socket �ּ� ����ü[����IP, ����port], socket �ּ� ����ü ����)
	// - socket�� ���� IP�ּҿ� ���� Port ����
	// �ٵι�° ���ڴ� �׻� (SOCKADDR*)�� �����ؾ���!
	retval = bind(listen_sock, (SOCKADDR*)&serveraddr, sizeof(serveraddr));
	if (retval == SOCKET_ERROR)
		err_quit("bind()");


	// listen(Ŭ���̾�Ʈ ���� ���� ����[IP�ּ�, ����port ����o], ���� ������ Ŭ���̾�Ʈ ��[SOMAXCONN == �Ϻ� �������� ���� ���� ��])
	// - socket�� TCP ��Ʈ ���¸� LISTENING���� ���� ( LISTENING : Ŭ���̾�Ʈ ������ ���� �� �ִ� ����)
	retval = listen(listen_sock, SOMAXCONN);
	if (retval == SOCKET_ERROR)
		err_quit("listen()");


	// ������ ��ſ� ����� ����
	// - accept() return�� ������ ����, ���� Ŭ���̾�Ʈ�� �ּ�[IP,port], �ּ� ����
	SOCKET client_sock;		// (accept return ����)
	SOCKADDR_IN clientaddr; // (accept �ι�° ����)
	int addrlen;			// (accept ����° ����)

	// Ŭ���̾�Ʈ���� ���� �����͸� ������ ���� ���α׷� ���� 
	char buf[BUFSIZE + 1];

	// ������ Ŭ���̾�Ʈ ��û�� ó���ؾ��ϱ� ������ ���ѷ��� (���� ����, �����߻� ������)
	while (true) {
		// accept((Ŭ���̾�Ʈ ���� ���� ����[IP�ּ�, ����port ����o & TCP port == LISTENING], ���� Ŭ���̾�Ʈ �ּ�[IP, port], �ּ� ���� ����)
		// - ������ Ŭ���̾�Ʈ�� ��Ű����ϵ��� ���ο� ���� ������ ����, ���� ��� ������ ��� ���·�
		addrlen = sizeof(clientaddr);
		client_sock = accept(listen_sock, (SOCKADDR *)&clientaddr, &addrlen);
		if (client_sock == INVALID_SOCKET) {
			err_display("accept()");
			break;
		}


		// ������ Ŭ���̾�Ʈ ���� ���
		printf("\n[TCP Server] client Connect : IP Addr = %s, Port = %d \n", inet_ntoa(clientaddr.sin_addr), ntohs(clientaddr.sin_port));

		// Ŭ���̾�Ʈ�� ������ ���
		while (true) {
			// ������ �ޱ�
			retval = recvline(client_sock, buf, BUFSIZE + 1);		// --- variable

			if (retval == SOCKET_ERROR) {
				err_display("recvn()");
				break;
			}
			else if (retval == 0)
				break;

			// ���� ������ ���
			// - �����Ϳ� \n�� �� ����
			printf("[TCP/%s:%d] %s", inet_ntoa(clientaddr.sin_addr), ntohs(clientaddr.sin_port), buf);
		}

		// closesocket()
		closesocket(client_sock);
		printf("[TCP Server] client Disconnect : IP Addr = %s, Port = %d \n", inet_ntoa(clientaddr.sin_addr), ntohs(clientaddr.sin_port));

	}
	// closesocket()
	closesocket(listen_sock);

	// ���� ����
	WSACleanup();
	return 0;
}