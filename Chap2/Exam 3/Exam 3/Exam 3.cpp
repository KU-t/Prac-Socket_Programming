#pragma comment(lib, "ws2_32")
#include <WinSock2.h>
#include <WS2tcpip.h>

void err_quit(const char* msg) {
	LPVOID lpMsgBuf;

	FormatMessage(
		FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM,
		NULL, WSAGetLastError(),
		MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT),
		(LPTSTR)&lpMsgBuf, 0, NULL);

	MessageBox(NULL, (LPCTSTR)lpMsgBuf, msg, MB_ICONERROR);

	LocalFree(lpMsgBuf);
	exit(1);
}

int main(int argc, char * argv[]) {
	// ���� �ʱ�ȭ
	WSAData wsa;
	if (WSAStartup(MAKEWORD(2, 2), &wsa) != 0)
		return 1;

	MessageBox(NULL, "Winsock Init!", "inform", MB_OK);

	// socket() - socket(�ּ�ü��, ����Ÿ��, ��������) [���� ����]
	// SOCKET TYPE (= socket descriptor) : 32bit ���� 
	SOCKET tcp_sock = socket(AF_INET6, SOCK_STREAM, 0);
	if (tcp_sock == INVALID_SOCKET)
		err_quit("socket()");

	MessageBox(NULL, "TCP IPv6 success", "info", MB_OK);

	// closesocket() - ���� �ݱ�
	closesocket(tcp_sock);

	// ���� ����
	WSACleanup();
	return 0;
}