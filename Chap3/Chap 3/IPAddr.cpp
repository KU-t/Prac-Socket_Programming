#pragma comment(lib,"ws2_32")
#include <winsock2.h>
#include <WS2tcpip.h>
#include <stdio.h>

int main(int argc, char* argv[]) {
	WSADATA wsa;

	if (WSAStartup(MAKEWORD(2, 2), &wsa) != 0)
		return 1;

	// -------------
	//     IPv4
	// -------------

	// 원래의 IPv4 출력
	const char* ipv4test = "147.46.114.70";
	printf("IPv4 addr(before) = %s \n", ipv4test);

	// inet_addr() 함수 : 문자열로 IPv4주소를 입력받아 32비트 숫자(네트워크 바이트 정렬)로 리턴
	// "147.46.114.70" >> 0x46722e93
	printf("IPv4 addr(after) = 0x%x \n", inet_addr(ipv4test));

	// inet_ntoa() 함수 : 32비트 숫자(네트워크 바이트 정렬)로 IPv4주소를 입력받아 문자열로 리턴
	// 0x46722e93 >> "147.46.114.70" 
	IN_ADDR ipv4num;
	ipv4num.s_addr = inet_addr(ipv4test);	
	printf("IPv4 addr(change) = %s \n", inet_ntoa(ipv4num));

	printf("\n");

	// -------------
	//     IPv6
	// -------------

	// 원래의 IPv6 출력
	const char *ipv6test = "2001:0230:abcd:ffab:0023:eb00:ffff:1111";
	printf("IPv6 addr(before) = %s \n", ipv6test);

	//WSAStringToAddress() 함수 : 문자열로 IPv6주소를 입력받아 128비트 숫자(네트워크 바이트 정렬)로 리턴
	SOCKADDR_IN6 ipv6num;
	int addrlen = sizeof(ipv6num);
	
	WSAStringToAddress((LPSTR)ipv6test, AF_INET6, NULL, (SOCKADDR*)&ipv6num, &addrlen);
	printf("IPv6 addr(after) = 0x");
	for (int i = 0; i < 16; i++)
		printf("%02x", ipv6num.sin6_addr.u.Byte[i]);
	printf("\n");

	// WSAAddressToString() 함수 : 128비트 숫자(네트워크 바이트 정렬)로 IPv6주소를 입력받아 문자열로 리턴
	char ipaddr[50];
	DWORD ipaddrlen = sizeof(ipaddr);

	WSAAddressToString((SOCKADDR*)&ipv6num, sizeof(ipv6num), NULL, ipaddr, &ipaddrlen);
	printf("IPv6 addr(change) = %s \n", ipaddr);

	WSACleanup();
	return 0;
}