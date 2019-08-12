#pragma comment(lib, "ws2_32")
#include <winsock2.h>
#include <WS2tcpip.h>
#include <stdlib.h>
#include <stdio.h>

#define SERVERIP "::1"		// --- IPv6
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

// 사용자 정의 데티어 수신함수
// recvn()와 recv()의 형태는 동일
int recvn(SOCKET s, char *buf, int len, int flags) {
	// 내부적으로 호출하는 recv() 함수의 return값 저장
	int received;
	// 응용프로그램 버퍼의 시작주소, 데이터를 읽을 때마다 ptr값 증가
	char *ptr = buf;
	// 아직 읽지 않은 데이터의 크기. 데이터를 읽을 때마다 left값 감소
	int left = len;

	// 읽지않은 데이터가 있으면 계속 읽음
	while (left > 0) {
		received = recv(s, ptr, left, flags);

		// 함수 호출에 오류가 있는 경우,
		if (received == SOCKET_ERROR)
			return SOCKET_ERROR;

		// recv() 함수의 return값이 0 (정상 종료)일 경우, ( == 전송(접속) 종료)
		else if (received == 0)
			break;

		// 변수 갱신
		left -= received;
		ptr += received;
	}

	// 정상적이라면 0
	return (len - left);
}

int main(int argc, char *argv[]) {
	int retval;

	// 윈속 초기화
	WSADATA wsa;
	if (WSAStartup(MAKEWORD(2, 2), &wsa) != 0)
		return 1;

	// socket()
	SOCKET sock = socket(AF_INET6, SOCK_STREAM, 0);			// --- IPv6
	if (sock == INVALID_SOCKET)
		err_quit("socket()");

	// connect(서버와 통신을 위한 소켓, 소켓 주소 구조체, 소켓주소 구조체 길이)
	// - TCP 프로토콜에서 서버와 논리적 연결 설정
	// 소켓 구조체 초기화 (IP, Port)
	SOCKADDR_IN6 serveraddr;
	ZeroMemory(&serveraddr, sizeof(serveraddr));
	serveraddr.sin6_family = AF_INET6;				// --- IPv6
	int addrlen = sizeof(serveraddr);				// --- IPv6
	WSAStringToAddress((LPSTR)SERVERIP, AF_INET6, NULL, (SOCKADDR *)&serveraddr, &addrlen);	// --- IPv6
	serveraddr.sin6_port = htons(SERVERPORT);		// --- IPv6
	retval = connect(sock, (SOCKADDR *)&serveraddr, sizeof(serveraddr));
	if (retval == SOCKET_ERROR)
		err_quit("connect()");

	// 데이터 통신에 사용할 변수
	// 송수신 데이터 저장 버퍼 & 길이 변수
	char buf[BUFSIZE + 1];
	int len;

	// 서버와 데이터 통신
	while (true) {
		// 데이터 입력
		printf("\n[Send Data] ");
		// 사용자로부터 문자열 입력
		if (fgets(buf, BUFSIZE + 1, stdin) == NULL)
			break;

		// '\n' 문자 제거
		len = strlen(buf);
		if (buf[len - 1] == '\n')
			buf[len - 1] = '\0';
		// 사용자가 ENTER만 눌렀을 때,
		if (strlen(buf) == 0)
			break;

		// 데이터 보내기
		// retval == strlen(buf) [동일]
		retval = send(sock, buf, strlen(buf), 0);
		if (retval == SOCKET_ERROR) {
			err_display("send()");
			break;
		}
		printf("[TCP client] %d Byte - send \n", retval);

		// 데이터 받기
		// 서버에게 받을 데이터 크기를 알기때문에 recvn() 이용
		retval = recvn(sock, buf, retval, 0);
		if (retval == SOCKET_ERROR) {
			err_display("recv()");
			break;
		}
		else if (retval == 0)
			break;

		// 받은 데이터 출력
		// 받은 데이터 끝에 '\0'을 추가해 출력
		buf[retval] = '\0';
		printf("[TCP client] %d Byte - recive \n", retval);
		printf("[Recive Data] %s \n", buf);
	}

	// closesocket()
	closesocket(sock);

	// 윈속 종료
	WSACleanup();
	return 0;
}