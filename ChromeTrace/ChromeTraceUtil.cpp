
#include "ChromeTraceUtil.h"
#ifndef BT_NO_PROFILE
btTimings gTimings[QUICKPROF_MAX_THREAD_COUNT];
#define MAX_NESTING 1024
int gStackDepths[QUICKPROF_MAX_THREAD_COUNT] = { 0 };
const char* gFuncNames[QUICKPROF_MAX_THREAD_COUNT][MAX_NESTING];
unsigned long long int gStartTimes[QUICKPROF_MAX_THREAD_COUNT][MAX_NESTING];
#endif

Clock clk;
bool gProfileDisabled = true;

void MyDummyEnterProfileZoneFunc(const char* msg) {}
void MyDummyLeaveProfileZoneFunc() {}

void MyEnterProfileZoneFunc(const char* msg)
{
	if (gProfileDisabled)
		return;
#ifndef BT_NO_PROFILE
	int threadId = btQuickprofGetCurrentThreadIndex2();
	if (threadId < 0 || threadId >= QUICKPROF_MAX_THREAD_COUNT)
		return;

	if (gStackDepths[threadId] >= MAX_NESTING)
	{
		assert(0);
		return;
	}
	gFuncNames[threadId][gStackDepths[threadId]] = msg;
	gStartTimes[threadId][gStackDepths[threadId]] = clk.getTimeNanoseconds();
	if (gStartTimes[threadId][gStackDepths[threadId]] <= gStartTimes[threadId][gStackDepths[threadId] - 1])
	{
		gStartTimes[threadId][gStackDepths[threadId]] = 1 + gStartTimes[threadId][gStackDepths[threadId] - 1];
	}
	gStackDepths[threadId]++;
#endif

}
void MyLeaveProfileZoneFunc()
{
	if (gProfileDisabled)
		return;
#ifndef BT_NO_PROFILE
	int threadId = btQuickprofGetCurrentThreadIndex2();
	if (threadId < 0 || threadId >= QUICKPROF_MAX_THREAD_COUNT)
		return;

	if (gStackDepths[threadId] <= 0)
	{
		return;
	}

	gStackDepths[threadId]--;

	const char* name = gFuncNames[threadId][gStackDepths[threadId]];
	unsigned long long int startTime = gStartTimes[threadId][gStackDepths[threadId]];

	unsigned long long int endTime = clk.getTimeNanoseconds();
	gTimings[threadId].addTiming(name, threadId, startTime, endTime);
#endif //BT_NO_PROFILE
}

void ChromeUtilsStartTimings()
{
	m_firstTiming = true;
	gProfileDisabled = false;//true;
	SetCustomEnterProfileZoneFunc(MyEnterProfileZoneFunc);
	SetCustomLeaveProfileZoneFunc(MyLeaveProfileZoneFunc);
}

void ChromeUtilsStopTimingsAndWriteJsonFile(const char* fileNamePrefix)
{
	SetCustomEnterProfileZoneFunc(MyDummyEnterProfileZoneFunc);
	SetCustomLeaveProfileZoneFunc(MyDummyLeaveProfileZoneFunc);
	char fileName[1024];
	static int fileCounter = 0;
	sprintf(fileName, "%s_%d.json", fileNamePrefix, fileCounter++);
	gTimingFile = fopen(fileName, "w");
	if (gTimingFile)
	{
		fprintf(gTimingFile, "{\"traceEvents\":[\n");
		//dump the content to file
		for (int i = 0; i < QUICKPROF_MAX_THREAD_COUNT; i++)
		{
			if (gTimings[i].m_numTimings)
			{
				printf("Writing %d timings for thread %d\n", gTimings[i].m_numTimings, i);
				gTimings[i].flush();
			}
		}
		fprintf(gTimingFile, "\n],\n\"displayTimeUnit\": \"ns\"}");
		fclose(gTimingFile);
	}
	else
	{
		printf("Error opening file");
		printf(fileName);
	}
	gTimingFile = 0;
}

void ChromeUtilsEnableProfiling()
{
	gProfileDisabled = false;
}