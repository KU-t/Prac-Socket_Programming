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

int err_socket(int i) {
	if (i < 0) {
		WSASetLastError(WSAEMSGSIZE);
		return SOCKET_ERROR;
	}
	WSASetLastError(0);
	return 0;
}

int main(int argc, char * argv[]) {
	// 윈속 초기화
	WSAData wsa;
	if (WSAStartup(MAKEWORD(2, 2), &wsa) != 0)
		return 1;

	MessageBox(NULL, "Winsock Init!", "inform", MB_OK);

	// socket() - socket(주소체계, 소켓타입, 프로토콜) [소켓 생성]
	// SOCKET TYPE (= socket descriptor) : 32bit 정수 
	SOCKET tcp_sock = socket(AF_INET, SOCK_STREAM, 0);
	if (tcp_sock == INVALID_SOCKET)
		err_quit("socket()");
	
	int retval = err_socket(tcp_sock);
	if (retval == SOCKET_ERROR)
		err_quit("err_socket()");

	MessageBox(NULL, "TCP success", "info", MB_OK);

	// closesocket() - 소켓 닫기
	closesocket(tcp_sock);

	// 윈속 종료
	WSACleanup();
	return 0;
}