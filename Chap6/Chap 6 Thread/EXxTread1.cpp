#include <Windows.h>
#include <stdio.h>

struct Point3D {
	int x, y, z;
};

// Thread가 하는 동작 [함수]
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
	// HANDLE CreateThread(구조체 [대부분 NULL], 스레드 할당 크기[대부분 0 (기본 크기 : 1MB),
	//	스레드 함수 시작주소, 스레드 함수 전달 인자, 스레드 생성 제어[대부분 0 (바로실행)], 스레드 ID[NULL]], )
	// CloseHandle()가 호출되면 Thread 닫음 - 종료의 의미x

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
	
	// 실행 결과
	처음에 [ 메인 - 첫 번째 - 두 번째 ] 순서
	한 프로세스 내의 스레드 실행 순서는 시스템에 존재하는 다른 스레드들의 영향을 받기 때문에 실행 패턴이 변경될 수 있음.

	*/
}