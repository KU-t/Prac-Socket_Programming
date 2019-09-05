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
	// 윈속 초기화
	WSAData wsa;
	if (WSAStartup(MAKEWORD(2, 2), &wsa) != 0)
		return 1;

	MessageBox(NULL, "Winsock Init!", "inform", MB_OK);

	// socket() - socket(주소체계, 소켓타입, 프로토콜) [소켓 생성]
	// SOCKET TYPE (= socket descriptor) : 32bit 정수 
	// SOCKET tcp_sock = socket(AF_INET, SOCK_STREAM, 0);

	/*
	SOCKET WSASocket(int af,int type, int protocol, LPWSAPROTOCOL_INFO lpProtocollnfo, GROUP g, DWORD dwFlags)

	리턴값:성공하면 생성된 소켓을 리턴하고, 실패하면 INVALID_SOCKET을 리턴한다.
	1.af
	네트워크 주소 패밀리를 의미하는 지정자.
	2.type
	새로운 소켓의 타입.
	3.protocol
	사용할 프로토콜의 종류를 지정한다.이것은 주소 패밀리와 함께 사용된다.
	4.lpProtocollnfo
	WSAPROTOCOL_INFO 구조체의 주소.생성되는 소켓의 속성을 정의하는 구조체이다.
	5.g
	예약된 멤버 변수이다.
	6.dwFlags
	소켓 속성을 지정하는 플래그.이 플래그에서 WSA_FLAG_OVERLAPPED를 지정하면 OVERLAPPED를 상요할 수 있다.

	= WSASocket()함수는 마지막 dwFlags인자에 WSA_FLAG_OVERLAPPED를 지정하여 OVERLAPPED 입출력을 이용할 수 있도록 소켓을 생성할 수있다.
	*/

	SOCKET udp_sock = WSASocket(AF_INET, SOCK_DGRAM, 0, NULL, 0, WSA_FLAG_OVERLAPPED);
	if (udp_sock == INVALID_SOCKET)
		err_quit("socket()");

	MessageBox(NULL, "UDP success", "info", MB_OK);

	// closesocket() - 소켓 닫기
	closesocket(udp_sock);

	// 윈속 종료
	WSACleanup();
	return 0;
}