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

	// ������ IPv4 ���
	const char* ipv4test = "147.46.114.70";
	printf("IPv4 addr(before) = %s \n", ipv4test);

	// inet_addr() �Լ� : ���ڿ��� IPv4�ּҸ� �Է¹޾� 32��Ʈ ����(��Ʈ��ũ ����Ʈ ����)�� ����
	// "147.46.114.70" >> 0x46722e93
	printf("IPv4 addr(after) = 0x%x \n", inet_addr(ipv4test));

	// inet_ntoa() �Լ� : 32��Ʈ ����(��Ʈ��ũ ����Ʈ ����)�� IPv4�ּҸ� �Է¹޾� ���ڿ��� ����
	// 0x46722e93 >> "147.46.114.70" 
	IN_ADDR ipv4num;
	ipv4num.s_addr = inet_addr(ipv4test);	
	printf("IPv4 addr(change) = %s \n", inet_ntoa(ipv4num));

	printf("\n");

	// -------------
	//     IPv6
	// -------------

	// ������ IPv6 ���
	const char *ipv6test = "2001:0230:abcd:ffab:0023:eb00:ffff:1111";
	printf("IPv6 addr(before) = %s \n", ipv6test);

	//WSAStringToAddress() �Լ� : ���ڿ��� IPv6�ּҸ� �Է¹޾� 128��Ʈ ����(��Ʈ��ũ ����Ʈ ����)�� ����
	SOCKADDR_IN6 ipv6num;
	int addrlen = sizeof(ipv6num);
	
	WSAStringToAddress((LPSTR)ipv6test, AF_INET6, NULL, (SOCKADDR*)&ipv6num, &addrlen);
	printf("IPv6 addr(after) = 0x");
	for (int i = 0; i < 16; i++)
		printf("%02x", ipv6num.sin6_addr.u.Byte[i]);
	printf("\n");

	// WSAAddressToString() �Լ� : 128��Ʈ ����(��Ʈ��ũ ����Ʈ ����)�� IPv6�ּҸ� �Է¹޾� ���ڿ��� ����
	char ipaddr[50];
	DWORD ipaddrlen = sizeof(ipaddr);

	WSAAddressToString((SOCKADDR*)&ipv6num, sizeof(ipv6num), NULL, ipaddr, &ipaddrlen);
	printf("IPv6 addr(change) = %s \n", ipaddr);

	WSACleanup();
	return 0;
}