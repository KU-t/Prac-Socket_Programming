#include <Windows.h>
#include <stdio.h>

#define BUFSIZE 10

HANDLE hReadEvent;
HANDLE hWriteEvent;
int buf[BUFSIZE];

// SetEvent(handle) : ���ȣ -> ��ȣ ����
// ResetEvent(handle) : ��ȣ -> ���ȣ ����

DWORD WINAPI WriteThread(LPVOID arg) {
	DWORD retval;
	
	for (int j = 1; j <= 500; j++) {
		// �б� �Ϸ� ���
		retval = WaitForSingleObject(hReadEvent, INFINITE);
		if (retval != WAIT_OBJECT_0)
			break;

		// ���� ���ۿ� ������ ����
		for (int i = 0; i < BUFSIZE; i++)
			buf[i] = j;

		// ���� �Ϸ� �˸�
		SetEvent(hWriteEvent);
	}

	return 0;
}

DWORD WINAPI ReadThread(LPVOID arg) {
	DWORD retval;

	while (true) {
		// ���� �Ϸ� ���
		retval = WaitForSingleObject(hWriteEvent, INFINITE);
		if (retval != WAIT_OBJECT_0)
			break;

		// ���� ������ ���
		printf("Thread %4d : ", GetCurrentThreadId());
		for (int i = 0; i < BUFSIZE; i++)
			printf("%3d ", buf[i]);
		printf("\n");

		// ���� �ʱ�ȭ
		ZeroMemory(buf, sizeof(buf));

		// �б� �Ϸ� �˸�
		SetEvent(hReadEvent);
	}

	return 0;
}

int main(int arg, char *argv[]) {

	// �ڵ� ���� �̺�Ʈ 2�� ����
	// CreateEvent(����ü [��κ� NULL], ���� ���[TRUE:����/FALSE:�ڵ�], �ʱ� ����[TRUE:��ȣ/FALSE:���ȣ],�̺�Ʈ �̸�)
	// ���� Event - ���ȣ
	hWriteEvent = CreateEvent(NULL, FALSE, FALSE, NULL);
	if (hWriteEvent == NULL)	return 1;
	// �б� Event - ��ȣ
	hReadEvent= CreateEvent(NULL, FALSE, TRUE, NULL);
	if (hReadEvent == NULL)	return 1;

	// ������ 3�� ����
	HANDLE hThread[3];
	hThread[0] = CreateThread(NULL, 0, WriteThread, NULL, 0, NULL);
	hThread[1] = CreateThread(NULL, 0, ReadThread, NULL, 0, NULL);
	hThread[2] = CreateThread(NULL, 0, ReadThread, NULL, 0, NULL);

	// ������ 3�� ���� ���
	WaitForMultipleObjects(3, hThread, TRUE, INFINITE);

	// �̺�Ʈ ����
	CloseHandle(hWriteEvent);
	CloseHandle(hReadEvent);

	return 0;
}