#include <Windows.h>
#include <stdio.h>

DWORD WINAPI MyThread(LPVOID arg) {
	while (true);
	return 0;
}

int main() {

	// CPU ���� Ȯ��
	SYSTEM_INFO si;
	GetSystemInfo(&si);

	// CPU ������ŭ �����带 �����Ѵ�.
	for (int i = 0; i < (int)si.dwNumberOfProcessors; ++i) {
		HANDLE hThread = CreateThread(NULL, 0, MyThread, NULL, 0, NULL);
		if (hThread == NULL)	return 1;

		// �ְ� �켱������ ����
		SetThreadPriority(hThread, THREAD_PRIORITY_TIME_CRITICAL);
		CloseHandle(hThread);
	}
	
	Sleep(1000);
	
	while (true) {
		printf("Main Thread Running! \n");
		break;
	}

	return 0;

	/*
	
	MyThread�� �켱������ MainThread���� ���� ����������,
	�ü������ �����ð� CPU�� �Ҵ���� ���� �������� �켱������ �ܰ������� ���̱� ������,
	Main Thread�� ����� ��ȸ�� ����.
	
	*/

}