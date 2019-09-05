#pragma comment(lib, "ws2_32")
#include <WinSock2.h>

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
	// SOCKET tcp_sock = socket(AF_INET, SOCK_STREAM, 0);

	/*
	SOCKET WSASocket(int af,int type, int protocol, LPWSAPROTOCOL_INFO lpProtocollnfo, GROUP g, DWORD dwFlags)

	���ϰ�:�����ϸ� ������ ������ �����ϰ�, �����ϸ� INVALID_SOCKET�� �����Ѵ�.
	1.af
	��Ʈ��ũ �ּ� �йи��� �ǹ��ϴ� ������.
	2.type
	���ο� ������ Ÿ��.
	3.protocol
	����� ���������� ������ �����Ѵ�.�̰��� �ּ� �йи��� �Բ� ���ȴ�.
	4.lpProtocollnfo
	WSAPROTOCOL_INFO ����ü�� �ּ�.�����Ǵ� ������ �Ӽ��� �����ϴ� ����ü�̴�.
	5.g
	����� ��� �����̴�.
	6.dwFlags
	���� �Ӽ��� �����ϴ� �÷���.�� �÷��׿��� WSA_FLAG_OVERLAPPED�� �����ϸ� OVERLAPPED�� ����� �� �ִ�.

	= WSASocket()�Լ��� ������ dwFlags���ڿ� WSA_FLAG_OVERLAPPED�� �����Ͽ� OVERLAPPED ������� �̿��� �� �ֵ��� ������ ������ ���ִ�.
	*/

	SOCKET udp_sock = WSASocket(AF_INET, SOCK_DGRAM, 0, NULL, 0, WSA_FLAG_OVERLAPPED);
	if (udp_sock == INVALID_SOCKET)
		err_quit("socket()");

	MessageBox(NULL, "UDP success", "info", MB_OK);

	// closesocket() - ���� �ݱ�
	closesocket(udp_sock);

	// ���� ����
	WSACleanup();
	return 0;
}