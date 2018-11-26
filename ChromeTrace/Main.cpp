#include "ChromeTraceUtil.h"
#include "Quickprof.h"

#include "stdio.h"
#include <windows.h>
#include <thread>

void testA()
{
	PROFILE("testA");
	Sleep(1234);
}

int testB()
{
	PROFILE("testB");
	int i = 0;
	Sleep(233);
	return i;
}

int testC()
{
	PROFILE("testC");
	int i = 0;
	for (; i < 5; i++)
	{
		testB();
	}
	Sleep(2000);
	return i;
}

void testD()
{
	std::thread([&]() {
		PROFILE("testD");
		std::this_thread::sleep_for(std::chrono::milliseconds(666));
	}).detach();
}

int main()
{
	ChromeUtilsStartTimings();
	while (true)
	{
		testA();
		testC();
		testD();
		if (GetKeyState('A') & 0x8000/*check if high-order bit is set (1 << 15)*/)
		{
			printf("A press!\n");
			ChromeUtilsStopTimingsAndWriteJsonFile("AAtimings");
			break;
		}
	}

	system("pause");
    return 0;
}

