/*
[도메인 이름 -> IP주소(네트워크 바이트 정렬)] IPv4 주소 정보 리턴
struct hostent *gethostbyname(
	const char *name;	// 도메인 이름
	);

[IP주소(네트워크 바이트 정렬) -> 도메인 이름]
struct hostent *gethostbyaddr(
	const char *addr,	//IP 주소(네트워크 바이트 정렬)
	int len,			//IP 주소 길이
	int type			//주소 체계(AF_INET or AF_INET6)
	);

typedef struct hostent{
	char*	h_name;			// 공식 도메인 이름
	char**	h_aliases;		// 공식 도메인 이름 외, 별명 리스트
	short	h_addrtype;		// 주소 체계
	short	h_length;		// IP 주소의 길이
	char**	h_addr_list;	// 네트워크 바이트 정렬된 IP 주소 리스트 (대부분 첫번째( h_addr_list[0] )의 주소 사용)
*/

#pragma comment(lib, "ws2_32")
#include <WinSock2.h>
#include <stdio.h>

#define TESTNAME  "www.naver.com"

// 소켓 함수 오류 출력
void err_display(const char *msg) {
	LPVOID ipMsgBuf;
	// 오류메세지 얻기
	FormatMessage(
		// 오류메세지 저장 공간을 알아서 할당 | 운영체제로부터 오류 메세지 가져옴
		FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM,
		// NULL , 오류코드 - [구체적 오류 코드 확인]
		NULL, WSAGetLastError(),
		//오류메세지 표시 언어 - 사용자 제어판 기본 설정 언어
		MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT),
		// 오류 문자열 시작주소, nSize, Arguments
		(LPTSTR)&ipMsgBuf, 0, NULL);
	printf("[%s] %s", msg, (char*)ipMsgBuf);
	// 시스템 할당 메모리 반환
	LocalFree(ipMsgBuf);
}

// 도메인 이름 -> IPv4주소
BOOL GetIPAddr(char* name, IN_ADDR* addr) {
	HOSTENT* ptr = gethostbyname(name);
	if (ptr == NULL) {
		err_display("gethostbyname()");
		return FALSE;
	}

	if (ptr->h_addrtype != AF_INET)
		return FALSE;

	memcpy(addr, ptr->h_addr, ptr->h_length);
	return TRUE;
}

// IPv4 주소 -> 도메인 이름
BOOL GetDomainName(IN_ADDR addr, char* name, int namelen) {
	HOSTENT* ptr = gethostbyaddr((char*)&addr, sizeof(addr), AF_INET);

	if (ptr == NULL) {
		err_display("gethostbyaddr()");
		return FALSE;
	}

	if (ptr->h_addrtype != AF_INET)
		return FALSE;

	strncpy(name, ptr->h_name, namelen);
	return TRUE;
}

int main(int argc, char* argv[]) {
	WSADATA wsa;
	if (WSAStartup(MAKEWORD(2, 2), &wsa) != 0)
		return 1;

	printf("Domain Name(before) = %s \n", TESTNAME);

	// 도메인 이름 -> IP 주소
	IN_ADDR addr;
	if (GetIPAddr(TESTNAME, &addr)) {
		// 도메인 이름 -> IP 주소가 성공이면 결과 출력
		printf("IP 주소 (after) = %s \n", inet_ntoa(addr));

		// IP 주소 -> 도메인 이름
		char name[256];
		if (GetDomainName(addr, name, sizeof(name))) {
			// IP 주소 -> 도메인 이름가 성공이면 결과 출력
			printf("Domain Name(change) = %s \n", name);
		}
	}

	WSACleanup();
	return 0;
}