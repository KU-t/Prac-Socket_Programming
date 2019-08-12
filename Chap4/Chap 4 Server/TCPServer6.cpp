/*

	TIME_WAIT 상태 : closesocket()함수를 호출한 소켓의 TCP 포트가 바로 파괴되지 않고 잠시 머무르는 상태
		- 대략 5분정도 대기

*/

#pragma comment(lib, "ws2_32")
#include <winsock2.h>
#include <WS2tcpip.h>
#include <stdio.h>
#include <stdlib.h>

#define SERVERPORT 9000
#define BUFSIZE 512

// 소켓 함수 오류 출력 후 종료 - [프로그램 실행]
void err_quit(const char *msg) {
	LPVOID lpMsgBuf;
	// 오류메세지 얻기
	FormatMessage(
		// 오류메세지 저장 공간을 알아서 할당 | 운영체제로부터 오류 메세지 가져옴
		FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM,
		// NULL , 오류코드 - [구체적 오류 코드 확인]
		NULL, WSAGetLastError(),
		//오류메세지 표시 언어 - 사용자 제어판 기본 설정 언어
		MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT),
		// 오류 문자열 시작주소, nSize, Arguments
		(LPTSTR)&lpMsgBuf, 0, NULL);
	// 메세지 상자 생성 - [오류 메세지 확인]
	MessageBox(NULL, (LPCTSTR)lpMsgBuf, (LPCTSTR)msg, MB_ICONERROR);
	// 시스템 할당 메모리 반환
	LocalFree(lpMsgBuf);
	exit(1);
}

// 소켓 함수 오류 출력 - 테스트용
// 내용 err_quit()과 동일
void err_display(const char *msg) {
	LPVOID lpMsgBuf;
	FormatMessage(
		FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM,
		NULL, WSAGetLastError(),
		MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT),
		(LPTSTR)&lpMsgBuf, 0, NULL);
	printf("[%s] %s", msg, (char *)lpMsgBuf);
	// 시스템 할당 메모리 반환
	LocalFree(lpMsgBuf);
}

int main(int argc, char* argv[]) {
	int retval;

	// 윈속 초기화
	WSADATA wsa;
	if (WSAStartup(MAKEWORD(2, 2), &wsa) != 0)
		return 1;

	// socket()
	SOCKET listen_sock = socket(AF_INET6, SOCK_STREAM, 0);	// --- IPv6
	if (listen_sock == INVALID_SOCKET)
		err_quit("socket()");


	// bind()
	// socket 주소 구조체 변수 및 초기화
	SOCKADDR_IN6 serveraddr;					// --- IPv6
	ZeroMemory(&serveraddr, sizeof(serveraddr));
	// - IPv6, 지역 IP, 지역 port
	serveraddr.sin6_family = AF_INET6;			// --- IPv6
	serveraddr.sin6_addr = in6addr_any;			// --- IPv6
	serveraddr.sin6_port = htons(SERVERPORT);	// --- IPv6
	// bind(클라이언트 수용 목적 소켓[IP주소, 지역port 결정x], socket 주소 구조체[지역IP, 지역port], socket 주소 구조체 길이)
	// - socket의 지역 IP주소와 지역 Port 결정
	// ☆두번째 인자는 항상 (SOCKADDR*)로 변형해야함!
	retval = bind(listen_sock, (SOCKADDR*)&serveraddr, sizeof(serveraddr));
	if (retval == SOCKET_ERROR)
		err_quit("bind()");


	// listen(클라이언트 수용 목적 소켓[IP주소, 지역port 결정o], 접속 가능한 클라이언트 수[SOMAXCONN == 하부 프로토콜 지원 가능 수])
	// - socket의 TCP 포트 상태를 LISTENING으로 변경 ( LISTENING : 클라이언트 접속을 받을 수 있는 상태)
	retval = listen(listen_sock, SOMAXCONN);
	if (retval == SOCKET_ERROR)
		err_quit("listen()");


	// 데이터 통신에 사용할 변수
	// accept() return값 저장할 소켓, 접속 클라이언트의 주소[IP,port], 주소 길이
	SOCKET client_sock;		// (accept return 인자)
	SOCKADDR_IN6 clientaddr; // (accept 두번째 인자)	// --- IPv6
	int addrlen;			// (accept 세번째 인자)

	// 클라이언트에게 받은 데이터를 저장할 응용 프로그램 버퍼
	char buf[BUFSIZE + 1];

	// 서버가 클라이언트 요청을 처리해야하기 때문에 무한루프 (정상 종료, 오류발생 전까지)
	while (true) {
		// accept((클라이언트 수용 목적 소켓[IP주소, 지역port 결정o & TCP port == LISTENING], 접속 클라이언트 주소[IP, port], 주소 정보 길이)
		// - 접속한 클라이언트와 통신가능하도록 새로운 소켓 생성해 리턴, 없을 경우 서버를 대기 상태로
		addrlen = sizeof(clientaddr);
		client_sock = accept(listen_sock, (SOCKADDR *)&clientaddr, &addrlen);
		if (client_sock == INVALID_SOCKET) {
			err_display("accept()");
			break;
		}


		// 접속한 클라이언트 정보 출력			// --- IPv6
		char ipaddr[50];
		DWORD ipaddrlen = sizeof(ipaddr);
		WSAAddressToString((SOCKADDR *)&clientaddr, sizeof(clientaddr), NULL, ipaddr, &ipaddrlen);
		printf("\n[TCP Server] client Connect : %s \n", ipaddr);

		// 클라이언트와 데이터 통신
		while (true) {
			// 데이터 받기
			// recv(대상 socket, 보낼 데이터의 버퍼 주소, 보낼 데이터 크기[응용 프로그램 버퍼보다 작아야함], 옵션[대부분 0]) 
			// - 소켓버퍼에 접근 (수신버퍼) // return : len
			// : 운영체제 수신 버퍼 -> 데이터 도착 -> 응용 프로그램 버퍼에 복사
			// 클라이언트에게 받을 데이터의 크기를 모르기때문에 recvn() 이용불가
			retval = recv(client_sock, buf, BUFSIZE, 0);

			if (retval == SOCKET_ERROR) {
				err_display("recv()");
				break;
			}
			else if (retval == 0)
				break;

			// 받은 데이터 출력
			buf[retval] = '\0';
			printf("[TCP/%s] %s \n", ipaddr, buf);

			// 데이터 보내기
			// send(대상 socket, 보낼 데이터의 버퍼 주소, 보낼 데이터 크기, 옵션[대부분 0])
			// - 소켓버퍼에 접근 (송신버퍼) // return : min = 1, max = len, 0 == 정상 종료
			// : 응용프로그램 데이터 -> 운영체제 송신버퍼에 복사 -> 데이터 전송
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

	// 윈속 종료
	WSACleanup();
	return 0;
}