#pragma once
//copyright see https://github.com/bulletphysics/bullet3
#include "Clock.h"
#include "Quickprof.h"
#include <vector>
#include <assert.h>
struct btTiming
{
	const char* m_name;
	int m_threadId;
	unsigned long long int m_usStartTime;
	unsigned long long int m_usEndTime;
};

static FILE* gTimingFile = 0;
#ifndef __STDC_FORMAT_MACROS
#define __STDC_FORMAT_MACROS
#endif //__STDC_FORMAT_MACROS

//see http://stackoverflow.com/questions/18107426/printf-format-for-unsigned-int64-on-windows
#ifndef _WIN32
#include <inttypes.h>
#endif

#define BT_TIMING_CAPACITY 16*65536
static bool m_firstTiming = true;

struct btTimings
{
	btTimings()
		:m_numTimings(0),
		m_activeBuffer(0)
	{

	}
	void flush()
	{
		for (int i = 0; i < m_numTimings; i++)
		{
			const char* name = m_timings[m_activeBuffer][i].m_name;
			int threadId = m_timings[m_activeBuffer][i].m_threadId;
			unsigned long long int startTime = m_timings[m_activeBuffer][i].m_usStartTime;
			unsigned long long int endTime = m_timings[m_activeBuffer][i].m_usEndTime;

			if (!m_firstTiming)
			{
				fprintf(gTimingFile, ",\n");
			}

			m_firstTiming = false;

			unsigned long long int startTimeDiv1000 = startTime / 1000;
			unsigned long long int endTimeDiv1000 = endTime / 1000;

#if 0

			fprintf(gTimingFile, "{\"cat\":\"timing\",\"pid\":1,\"tid\":%d,\"ts\":%" PRIu64 ".123 ,\"ph\":\"B\",\"name\":\"%s\",\"args\":{}},\n",
				threadId, startTimeDiv1000, name);
			fprintf(gTimingFile, "{\"cat\":\"timing\",\"pid\":1,\"tid\":%d,\"ts\":%" PRIu64 ".234 ,\"ph\":\"E\",\"name\":\"%s\",\"args\":{}}",
				threadId, endTimeDiv1000, name);

#else
			if (startTime > endTime)
			{
				endTime = startTime;
			}
			unsigned int startTimeRem1000 = startTime % 1000;
			unsigned int endTimeRem1000 = endTime % 1000;

			char startTimeRem1000Str[16];
			char endTimeRem1000Str[16];

			if (startTimeRem1000 < 10)
			{
				sprintf(startTimeRem1000Str, "00%d", startTimeRem1000);
			}
			else
			{
				if (startTimeRem1000 < 100)
				{
					sprintf(startTimeRem1000Str, "0%d", startTimeRem1000);
				}
				else
				{
					sprintf(startTimeRem1000Str, "%d", startTimeRem1000);
				}
			}

			if (endTimeRem1000 < 10)
			{
				sprintf(endTimeRem1000Str, "00%d", endTimeRem1000);
			}
			else
			{
				if (endTimeRem1000 < 100)
				{
					sprintf(endTimeRem1000Str, "0%d", endTimeRem1000);
				}
				else
				{
					sprintf(endTimeRem1000Str, "%d", endTimeRem1000);
				}
			}

			char newname[1024];
			static int counter2 = 0;
			sprintf(newname, "%s%d", name, counter2++);

#ifdef _WIN32

			fprintf(gTimingFile, "{\"cat\":\"timing\",\"pid\":1,\"tid\":%d,\"ts\":%I64d.%s ,\"ph\":\"B\",\"name\":\"%s\",\"args\":{}},\n",
				threadId, startTimeDiv1000, startTimeRem1000Str, newname);
			fprintf(gTimingFile, "{\"cat\":\"timing\",\"pid\":1,\"tid\":%d,\"ts\":%I64d.%s ,\"ph\":\"E\",\"name\":\"%s\",\"args\":{}}",
				threadId, endTimeDiv1000, endTimeRem1000Str, newname);

#else
			fprintf(gTimingFile, "{\"cat\":\"timing\",\"pid\":1,\"tid\":%d,\"ts\":%" PRIu64 ".%s ,\"ph\":\"B\",\"name\":\"%s\",\"args\":{}},\n",
				threadId, startTimeDiv1000, startTimeRem1000Str, newname);
			fprintf(gTimingFile, "{\"cat\":\"timing\",\"pid\":1,\"tid\":%d,\"ts\":%" PRIu64 ".%s ,\"ph\":\"E\",\"name\":\"%s\",\"args\":{}}",
				threadId, endTimeDiv1000, endTimeRem1000Str, newname);
#endif
#endif

		}
		m_numTimings = 0;

	}

	void addTiming(const char* name, int threadId, unsigned long long int startTime, unsigned long long int endTime)
	{
		if (m_numTimings >= BT_TIMING_CAPACITY)
		{
			return;
		}

		if (m_timings[0].size() == 0)
		{
			m_timings[0].resize(BT_TIMING_CAPACITY);
		}

		int slot = m_numTimings++;

		m_timings[m_activeBuffer][slot].m_name = name;
		m_timings[m_activeBuffer][slot].m_threadId = threadId;
		m_timings[m_activeBuffer][slot].m_usStartTime = startTime;
		m_timings[m_activeBuffer][slot].m_usEndTime = endTime;
	}


	int m_numTimings;
	int m_activeBuffer;
	std::vector<btTiming> m_timings[1];
};
void ChromeUtilsStartTimings();
void ChromeUtilsStopTimingsAndWriteJsonFile(const char* fileNamePrefix);
void ChromeUtilsEnableProfiling();
