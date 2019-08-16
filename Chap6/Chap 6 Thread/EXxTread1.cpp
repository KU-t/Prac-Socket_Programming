#include <Windows.h>
#include <stdio.h>

struct Point3D {
	int x, y, z;
};

// Thread�� �ϴ� ���� [�Լ�]
DWORD WINAPI MyThread(LPVOID arg) {
	Point3D *pt = (Point3D *)arg;

	while (true) {
		printf("Running MyThread() %d : %d, %d, %d \n", GetCurrentThreadId(), pt->x, pt->y, pt->z);
		Sleep(1000);
	}
	return 0;
}

int main(int argc, char *argv[]) {
	// [ Tread ]
	// HANDLE CreateThread(����ü [��κ� NULL], ������ �Ҵ� ũ��[��κ� 0 (�⺻ ũ�� : 1MB),
	//	������ �Լ� �����ּ�, ������ �Լ� ���� ����, ������ ���� ����[��κ� 0 (�ٷν���)], ������ ID[NULL]], )
	// CloseHandle()�� ȣ��Ǹ� Thread ���� - ������ �ǹ�x

	// [ Tread 1]
	Point3D pt1 = { 10, 20, 30 };
	HANDLE hThread1 = CreateThread(NULL, 0, MyThread, &pt1, 0, NULL);
	if (hThread1 == NULL)	return 1;
	CloseHandle(hThread1);

	// [ Tread 2]
	Point3D pt2 = { 40, 50, 60 };
	HANDLE hThread2 = CreateThread(NULL, 0, MyThread, &pt2, 0, NULL);
	if (hThread2 == NULL)	return 1;
	CloseHandle(hThread2);

	while (true) {
		printf("Running main() %d \n", GetCurrentThreadId());
		Sleep(1000);
	}

	return 0;
	/*
	
	// ���� ���
	ó���� [ ���� - ù ��° - �� ��° ] ����
	�� ���μ��� ���� ������ ���� ������ �ý��ۿ� �����ϴ� �ٸ� ��������� ������ �ޱ� ������ ���� ������ ����� �� ����.

	*/
}