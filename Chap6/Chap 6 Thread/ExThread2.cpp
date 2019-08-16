#include <Windows.h>
#include <stdio.h>

DWORD WINAPI MyThread(LPVOID arg) {
	while (true);
	return 0;
}

int main() {

	// CPU 개수 확인
	SYSTEM_INFO si;
	GetSystemInfo(&si);

	// CPU 개수만큼 스레드를 생성한다.
	for (int i = 0; i < (int)si.dwNumberOfProcessors; ++i) {
		HANDLE hThread = CreateThread(NULL, 0, MyThread, NULL, 0, NULL);
		if (hThread == NULL)	return 1;

		// 최고 우선순위로 변경
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
	
	MyThread의 우선순위를 MainThread보다 높게 설정하지만,
	운영체제에서 오랜시간 CPU를 할당받지 못한 스레드의 우선순위를 단계적으로 높이기 때문에,
	Main Thread가 실행될 기회를 얻음.
	
	*/

}