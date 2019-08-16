#include <Windows.h>
#include <stdio.h>

#define BUFSIZE 10

HANDLE hReadEvent;
HANDLE hWriteEvent;
int buf[BUFSIZE];

// SetEvent(handle) : 비신호 -> 신호 상태
// ResetEvent(handle) : 신호 -> 비신호 상태

DWORD WINAPI WriteThread(LPVOID arg) {
	DWORD retval;
	
	for (int j = 1; j <= 500; j++) {
		// 읽기 완료 대기
		retval = WaitForSingleObject(hReadEvent, INFINITE);
		if (retval != WAIT_OBJECT_0)
			break;

		// 공유 버퍼에 데이터 저장
		for (int i = 0; i < BUFSIZE; i++)
			buf[i] = j;

		// 쓰기 완료 알림
		SetEvent(hWriteEvent);
	}

	return 0;
}

DWORD WINAPI ReadThread(LPVOID arg) {
	DWORD retval;

	while (true) {
		// 쓰기 완료 대기
		retval = WaitForSingleObject(hWriteEvent, INFINITE);
		if (retval != WAIT_OBJECT_0)
			break;

		// 읽은 데이터 출력
		printf("Thread %4d : ", GetCurrentThreadId());
		for (int i = 0; i < BUFSIZE; i++)
			printf("%3d ", buf[i]);
		printf("\n");

		// 버퍼 초기화
		ZeroMemory(buf, sizeof(buf));

		// 읽기 완료 알림
		SetEvent(hReadEvent);
	}

	return 0;
}

int main(int arg, char *argv[]) {

	// 자동 리셋 이벤트 2개 생성
	// CreateEvent(구조체 [대부분 NULL], 리셋 방식[TRUE:수동/FALSE:자동], 초기 상태[TRUE:신호/FALSE:비신호],이벤트 이름)
	// 쓰기 Event - 비신호
	hWriteEvent = CreateEvent(NULL, FALSE, FALSE, NULL);
	if (hWriteEvent == NULL)	return 1;
	// 읽기 Event - 신호
	hReadEvent= CreateEvent(NULL, FALSE, TRUE, NULL);
	if (hReadEvent == NULL)	return 1;

	// 스레드 3개 생성
	HANDLE hThread[3];
	hThread[0] = CreateThread(NULL, 0, WriteThread, NULL, 0, NULL);
	hThread[1] = CreateThread(NULL, 0, ReadThread, NULL, 0, NULL);
	hThread[2] = CreateThread(NULL, 0, ReadThread, NULL, 0, NULL);

	// 스레드 3개 종료 대기
	WaitForMultipleObjects(3, hThread, TRUE, INFINITE);

	// 이벤트 제거
	CloseHandle(hWriteEvent);
	CloseHandle(hReadEvent);

	return 0;
}