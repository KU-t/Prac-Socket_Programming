#pragma comment(lib, "ws2_32")
#include <winsock2.h>
#include <stdlib.h>
#include <stdio.h>

#define SERVERIP "127.0.0.1"
#define SERVERPORT 9000
#define BUFSIZE 50		

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
	printf("[%s] %s \n", msg, (char *)lpMsgBuf);
	// 시스템 할당 메모리 반환
	LocalFree(lpMsgBuf);
}

int main(int argc, char *argv[]) {
	int retval;

	// 윈속 초기화
	WSADATA wsa;
	if (WSAStartup(MAKEWORD(2, 2), &wsa) != 0)
		return 1;

	// socket()
	SOCKET sock = socket(AF_INET, SOCK_STREAM, 0);
	if (sock == INVALID_SOCKET)
		err_quit("socket()");

	// connect(서버와 통신을 위한 소켓, 소켓 주소 구조체, 소켓주소 구조체 길이)
	// - TCP 프로토콜에서 서버와 논리적 연결 설정
	// 소켓 구조체 초기화 (IP, Port)
	SOCKADDR_IN serveraddr;
	ZeroMemory(&serveraddr, sizeof(serveraddr));
	serveraddr.sin_family = AF_INET;
	serveraddr.sin_addr.s_addr = inet_addr(SERVERIP);
	serveraddr.sin_port = htons(SERVERPORT);
	retval = connect(sock, (SOCKADDR *)&serveraddr, sizeof(serveraddr));
	if (retval == SOCKET_ERROR)
		err_quit("connect()");

	// 데이터 통신에 사용할 변수
	// - 송수신 데이터 저장 버퍼
	char buf[BUFSIZE];
	// - 전송할 문자열 데이터 & 길이
	const char *testdata[] = { "Hi", "Nice to meet you", "I have something to talk", "Me too" };
	int len;

	// 서버와 데이터 통신
	for (int i = 0; i < 4; i++) {
		// 데이터 입력
		// - 문자열 데이터를 버퍼에 복사하고 끝에 '\n' 추가
		len = strlen(testdata[i]);
		strncpy(buf, testdata[i], len);
		buf[len++] = '\n';

		// 데이터 보내기
		// - len은 '\n'이 포함된 문자열 길이
		retval = send(sock, buf, len, 0);
		if (retval == SOCKET_ERROR) {
			err_display("send()");
			break;
		}
		printf("[TCP client] %d Byte - send \n", retval);
	}

	// closesocket()
	closesocket(sock);

	// 윈속 종료
	WSACleanup();
	return 0;
}