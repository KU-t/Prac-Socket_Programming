#include <Windows.h>
#include <stdio.h>

int sum = 0;

DWORD WINAPI MyThread(LPVOID arg) {
	int num = (int)arg;

	for (int i = 0; i <= num; i++)
		sum += i;
	
	return 0;
}

int main(int argc, char * argv[]) {

	// ������ ����ȭ �Լ�
	// WaitForSingleObject(�ش� ������, ���ð�)
	// WaitForMultipleObject(�ش� ������ �迭, ��� ������ == TRUE, ���ð�)
	
	// ������ �Ͻ����� / ����� �Լ� ==>  +1 / -1 (0���� Ŭ ��, ���� ���� ����)
	// SuspendThread() <=> ResumeThread()

	int num = 100;
	HANDLE hThread = CreateThread(NULL, 0, MyThread, (LPVOID)num, CREATE_SUSPENDED, NULL);
	if (hThread == NULL) return 1;

	printf("Before Thread Running, Result sum =  %d \n", sum);
	ResumeThread(hThread);

	WaitForSingleObject(hThread, INFINITE);
	printf("After Thread Running, Result sum = %d \n", sum);

	CloseHandle(hThread);

	return 0;
}