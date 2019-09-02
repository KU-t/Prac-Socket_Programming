#pragma comment(lib, "ws2_32")

#include <WinSock2.h>
#include <stdlib.h>
#include <stdio.h>

#define SERVERIP	"127.0.0.1"
#define SERVERPORT	9000
#define BUFSIZE		512

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

	// socket() : 프로토콜 결정
	SOCKET sock = socket(AF_INET, SOCK_DGRAM, 0);
	if (sock == INVALID_SOCKET)
		err_quit("socket()");

	// 소켓 주소 구조체 초기화
	SOCKADDR_IN serveraddr;
	ZeroMemory(&serveraddr, sizeof(serveraddr));
	serveraddr.sin_family = AF_INET;
	serveraddr.sin_addr.s_addr = inet_addr(SERVERIP);
	serveraddr.sin_port = htons(SERVERPORT);

	// 데이터 총신에 사용할 변수
	SOCKADDR_IN peeraddr;
	int addrlen;
	char buf[BUFSIZE + 1];
	int len;

	// 서버와 데이터 통신
	while(true){
		// 데이터 입력
		printf("[ Send DATA ] ");
		if (fgets(buf, BUFSIZE + 1, stdin) == NULL)
			break;

		// '\n' 문자 제거
		len = strlen(buf);
		if (buf[len - 1] == '\n')
			buf[len - 1] = '\0';
		if (strlen(buf) == 0)
			break;

		// 서버에 데이터 보내기
		// sendto(통신 소켓, 데이터 버퍼 주소, 데이터 버퍼 크기, 옵션[대부분 0], 목적지 주소, 주소 크기) 
		// - 응용 프로그램 데이터를 운영체제의 송신 버퍼에 복사해 전송
		// - 원격 IP주소 & 포트번호 설정 // 지역 IP주소 & 포트번호 결정
		retval = sendto(sock, buf, strlen(buf), 0, (SOCKADDR*)&serveraddr, sizeof(serveraddr));
		if (retval == SOCKET_ERROR) {
			err_display("sendto()");
			continue;
		}
		printf("[ UDP Client ] Send %d Bytes. \n", retval);

		// 서버가 보낸 데이터 받기
		// recvfrom(통신 소켓[미리 정의], 데이터 버퍼 주소, 데이터 버퍼 크기, 옵션[대부분 0], 송신자 주소, 주소 크기 )
		// - 운영체제의 수신 버퍼에 도착한 데이터를 응용 프로그램에 복사
		addrlen = sizeof(peeraddr);
		retval = recvfrom(sock, buf, BUFSIZE, 0, (SOCKADDR*)&peeraddr, &addrlen);
		if (retval == SOCKET_ERROR) {
			err_display("recvfrom()");
			continue;
		}

		// 송신자의 IP 주소 체크
		// peeraddr와 serveraddr가 틀릴 경우, 통신 불량
		if (memcmp(&peeraddr, &serveraddr, sizeof(peeraddr))) {
			printf("[Error] Wrong DATA");
			continue;
		}

		// 받은 데이터 출력
		buf[retval] = '\0';
		printf("[ UDP Client ] Receive %d Bytes. \n", retval);
		printf("[ RECEIVE DATA ] %s \n \n", buf);
	}

	// closesocket()
	closesocket(sock);

	// 윈속 종료
	WSACleanup();
	return 0;
}

/*

	<< connect() 유무 차이 >>

	[ connect() 이용O ]
	connect() : 서버 주소를 설정
	- send() & recv() : connect()로 설정된 서버와만 통신
		(설정된 서버 외 다른 데이터 수신x)

	[ connect() 이용X ]
	- sendto() & recvfrom() : 함수 파라미터에 서버 주소 필요 (주소 크기 포함)

	------
	
	sendto()
	- UDP & TCP 모두 사용 가능 (TCP일 경우, to/tolen 무시 & 옵션 사용 가능)
	- UDP에서 보낼 수 있는 데이터 제한 (비주얼 스튜디오에서)
	- 전송 후 데이터는 버려짐 (상대가 받았는지 확인 불가)

	recvfrom()
	- UDP & TCP 모두 사용 가능 (TCP일 경우, from/fromlen 무시 & 옵션 사용 가능)
	- 수신된 데이터가 버퍼보다 크면 남은 데이터는 버려짐 >> 버퍼 크기 중요

*/