#pragma comment(lib, "ws2_32")
#include <WinSock2.h>
#include <iostream>

using namespace std;

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
	
	// 윈속 초기화 Winsock 2.2 Ver.
	WSAData wsa;
	if (WSAStartup(MAKEWORD(2, 2), &wsa) != 0)
		return 1;

	cout << "[ Winsock Version 2.2 ]" << endl;
	cout << "wVersion : \t\t" << wsa.wVersion << endl;
	cout << "wHighVersion : \t\t" << wsa.wHighVersion << endl;
	cout << "szDescription : \t" << wsa.szDescription << endl;
	cout << "szSystemStatus : \t" << wsa.szSystemStatus << endl;

	cout << endl;

	// 윈속 초기화 Winsock 1.1 Ver.
	if (WSAStartup(MAKEWORD(1, 1), &wsa) != 0)
		return 1;

	cout << "[ Winsock Version 1.1 ]" << endl;
	cout << "wVersion : \t\t" << wsa.wVersion << endl;
	cout << "wHighVersion : \t\t" << wsa.wHighVersion << endl;
	cout << "szDescription : \t" << wsa.szDescription << endl;
	cout << "szSystemStatus : \t" << wsa.szSystemStatus << endl;

	MessageBox(NULL, "Winsock Init!", "inform", MB_OK);

	// 윈속 종료
	WSACleanup();
	return 0;
}