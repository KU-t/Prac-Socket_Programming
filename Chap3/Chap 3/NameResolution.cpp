/*
[������ �̸� -> IP�ּ�(��Ʈ��ũ ����Ʈ ����)] IPv4 �ּ� ���� ����
struct hostent *gethostbyname(
	const char *name;	// ������ �̸�
	);

[IP�ּ�(��Ʈ��ũ ����Ʈ ����) -> ������ �̸�]
struct hostent *gethostbyaddr(
	const char *addr,	//IP �ּ�(��Ʈ��ũ ����Ʈ ����)
	int len,			//IP �ּ� ����
	int type			//�ּ� ü��(AF_INET or AF_INET6)
	);

typedef struct hostent{
	char*	h_name;			// ���� ������ �̸�
	char**	h_aliases;		// ���� ������ �̸� ��, ���� ����Ʈ
	short	h_addrtype;		// �ּ� ü��
	short	h_length;		// IP �ּ��� ����
	char**	h_addr_list;	// ��Ʈ��ũ ����Ʈ ���ĵ� IP �ּ� ����Ʈ (��κ� ù��°( h_addr_list[0] )�� �ּ� ���)
*/

#pragma comment(lib, "ws2_32")
#include <WinSock2.h>
#include <stdio.h>

#define TESTNAME  "www.naver.com"

// ���� �Լ� ���� ���
void err_display(const char *msg) {
	LPVOID ipMsgBuf;
	// �����޼��� ���
	FormatMessage(
		// �����޼��� ���� ������ �˾Ƽ� �Ҵ� | �ü���κ��� ���� �޼��� ������
		FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM,
		// NULL , �����ڵ� - [��ü�� ���� �ڵ� Ȯ��]
		NULL, WSAGetLastError(),
		//�����޼��� ǥ�� ��� - ����� ������ �⺻ ���� ���
		MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT),
		// ���� ���ڿ� �����ּ�, nSize, Arguments
		(LPTSTR)&ipMsgBuf, 0, NULL);
	printf("[%s] %s", msg, (char*)ipMsgBuf);
	// �ý��� �Ҵ� �޸� ��ȯ
	LocalFree(ipMsgBuf);
}

// ������ �̸� -> IPv4�ּ�
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

// IPv4 �ּ� -> ������ �̸�
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

	// ������ �̸� -> IP �ּ�
	IN_ADDR addr;
	if (GetIPAddr(TESTNAME, &addr)) {
		// ������ �̸� -> IP �ּҰ� �����̸� ��� ���
		printf("IP �ּ� (after) = %s \n", inet_ntoa(addr));

		// IP �ּ� -> ������ �̸�
		char name[256];
		if (GetDomainName(addr, name, sizeof(name))) {
			// IP �ּ� -> ������ �̸��� �����̸� ��� ���
			printf("Domain Name(change) = %s \n", name);
		}
	}

	WSACleanup();
	return 0;
}