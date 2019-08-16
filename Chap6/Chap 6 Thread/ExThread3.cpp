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

	// 스레드 동기화 함수
	// WaitForSingleObject(해당 스레드, 대기시간)
	// WaitForMultipleObject(해당 스레드 배열, 모든 스레드 == TRUE, 대기시간)
	
	// 스레드 일시중지 / 재시작 함수 ==>  +1 / -1 (0보다 클 때, 실행 중지 상태)
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