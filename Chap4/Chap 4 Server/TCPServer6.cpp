/*

	TIME_WAIT ���� : closesocket()�Լ��� ȣ���� ������ TCP ��Ʈ�� �ٷ� �ı����� �ʰ� ��� �ӹ����� ����
		- �뷫 5������ ���

*/

#pragma comment(lib, "ws2_32")
#include <winsock2.h>
#include <WS2tcpip.h>
#include <stdio.h>
#include <stdlib.h>

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

int main(int argc, char* argv[]) {
	int retval;

	// ���� �ʱ�ȭ
	WSADATA wsa;
	if (WSAStartup(MAKEWORD(2, 2), &wsa) != 0)
		return 1;

	// socket()
	SOCKET listen_sock = socket(AF_INET6, SOCK_STREAM, 0);	// --- IPv6
	if (listen_sock == INVALID_SOCKET)
		err_quit("socket()");


	// bind()
	// socket �ּ� ����ü ���� �� �ʱ�ȭ
	SOCKADDR_IN6 serveraddr;					// --- IPv6
	ZeroMemory(&serveraddr, sizeof(serveraddr));
	// - IPv6, ���� IP, ���� port
	serveraddr.sin6_family = AF_INET6;			// --- IPv6
	serveraddr.sin6_addr = in6addr_any;			// --- IPv6
	serveraddr.sin6_port = htons(SERVERPORT);	// --- IPv6
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
	// accept() return�� ������ ����, ���� Ŭ���̾�Ʈ�� �ּ�[IP,port], �ּ� ����
	SOCKET client_sock;		// (accept return ����)
	SOCKADDR_IN6 clientaddr; // (accept �ι�° ����)	// --- IPv6
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


		// ������ Ŭ���̾�Ʈ ���� ���			// --- IPv6
		char ipaddr[50];
		DWORD ipaddrlen = sizeof(ipaddr);
		WSAAddressToString((SOCKADDR *)&clientaddr, sizeof(clientaddr), NULL, ipaddr, &ipaddrlen);
		printf("\n[TCP Server] client Connect : %s \n", ipaddr);

		// Ŭ���̾�Ʈ�� ������ ���
		while (true) {
			// ������ �ޱ�
			// recv(��� socket, ���� �������� ���� �ּ�, ���� ������ ũ��[���� ���α׷� ���ۺ��� �۾ƾ���], �ɼ�[��κ� 0]) 
			// - ���Ϲ��ۿ� ���� (���Ź���) // return : len
			// : �ü�� ���� ���� -> ������ ���� -> ���� ���α׷� ���ۿ� ����
			// Ŭ���̾�Ʈ���� ���� �������� ũ�⸦ �𸣱⶧���� recvn() �̿�Ұ�
			retval = recv(client_sock, buf, BUFSIZE, 0);

			if (retval == SOCKET_ERROR) {
				err_display("recv()");
				break;
			}
			else if (retval == 0)
				break;

			// ���� ������ ���
			buf[retval] = '\0';
			printf("[TCP/%s] %s \n", ipaddr, buf);

			// ������ ������
			// send(��� socket, ���� �������� ���� �ּ�, ���� ������ ũ��, �ɼ�[��κ� 0])
			// - ���Ϲ��ۿ� ���� (�۽Ź���) // return : min = 1, max = len, 0 == ���� ����
			// : �������α׷� ������ -> �ü�� �۽Ź��ۿ� ���� -> ������ ����
			retval = send(client_sock, buf, retval, 0);
			if (retval == SOCKET_ERROR) {
				err_display("send()");
				break;
			}
		}

		// closesocket()
		closesocket(client_sock);
		printf("[TCP Server] client Disconnect :%s \n", ipaddr);

	}
	// closesocket()
	closesocket(listen_sock);

	// ���� ����
	WSACleanup();
	return 0;
}