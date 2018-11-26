#include "Clock.h"

#ifdef __CELLOS_LV2__
#include <sys/sys_time.h>
#include <sys/time_util.h>
#include <stdio.h>
#endif

#if defined (SUNOS) || defined (__SUNOS__)
#include <stdio.h>
#endif
#ifdef __APPLE__
#include <mach/mach_time.h>
#include <TargetConditionals.h>
#endif

#if defined(WIN32) || defined(_WIN32)

#define BT_USE_WINDOWS_TIMERS
#define WIN32_LEAN_AND_MEAN
#define NOWINRES
#define NOMCX
#define NOIME

#ifdef _XBOX
#include <Xtl.h>
#else //_XBOX
#include <windows.h>

#if WINVER <0x0602
#define GetTickCount64 GetTickCount
#endif

#endif //_XBOX

#include <time.h>


#else //_WIN32
#include <sys/time.h>

#ifdef BT_LINUX_REALTIME
//required linking against rt (librt)
#include <time.h>
#endif //BT_LINUX_REALTIME

#endif //_WIN32

#define mymin(a,b) (a > b ? a : b)

struct ClockData
{

#ifdef BT_USE_WINDOWS_TIMERS
	LARGE_INTEGER mClockFrequency;
	LONGLONG mStartTick;
	LARGE_INTEGER mStartTime;
#else
#ifdef __CELLOS_LV2__
	uint64_t	mStartTime;
#else
#ifdef __APPLE__
	uint64_t mStartTimeNano;
#endif
	struct timeval mStartTime;
#endif
#endif //__CELLOS_LV2__

};

///The btClock is a portable basic clock that measures accurate time in seconds, use for profiling.
Clock::Clock()
{
	m_data = new ClockData;
#ifdef BT_USE_WINDOWS_TIMERS
	QueryPerformanceFrequency(&m_data->mClockFrequency);
#endif
	reset();
}

Clock::~Clock()
{
	delete m_data;
}

Clock::Clock(const Clock& other)
{
	m_data = new ClockData;
	*m_data = *other.m_data;
}

Clock& Clock::operator=(const Clock& other)
{
	*m_data = *other.m_data;
	return *this;
}


/// Resets the initial reference time.
void Clock::reset()
{
#ifdef BT_USE_WINDOWS_TIMERS
	QueryPerformanceCounter(&m_data->mStartTime);
	m_data->mStartTick = GetTickCount64();
#else
#ifdef __CELLOS_LV2__

	typedef uint64_t  ClockSize;
	ClockSize newTime;
	//__asm __volatile__( "mftb %0" : "=r" (newTime) : : "memory");
	SYS_TIMEBASE_GET(newTime);
	m_data->mStartTime = newTime;
#else
#ifdef __APPLE__
	m_data->mStartTimeNano = mach_absolute_time();
#endif
	gettimeofday(&m_data->mStartTime, 0);
#endif
#endif
}

/// Returns the time in ms since the last call to reset or since
/// the btClock was created.
unsigned long long int Clock::getTimeMilliseconds()
{
#ifdef BT_USE_WINDOWS_TIMERS
	LARGE_INTEGER currentTime;
	QueryPerformanceCounter(&currentTime);
	LONGLONG elapsedTime = currentTime.QuadPart -
		m_data->mStartTime.QuadPart;
	// Compute the number of millisecond ticks elapsed.
	unsigned long msecTicks = (unsigned long)(1000 * elapsedTime /
		m_data->mClockFrequency.QuadPart);

	return msecTicks;
#else

#ifdef __CELLOS_LV2__
	uint64_t freq = sys_time_get_timebase_frequency();
	double dFreq = ((double)freq) / 1000.0;
	typedef uint64_t  ClockSize;
	ClockSize newTime;
	SYS_TIMEBASE_GET(newTime);
	//__asm __volatile__( "mftb %0" : "=r" (newTime) : : "memory");

	return (unsigned long int)((double(newTime - m_data->mStartTime)) / dFreq);
#else

	struct timeval currentTime;
	gettimeofday(&currentTime, 0);
	return (currentTime.tv_sec - m_data->mStartTime.tv_sec) * 1000 +
		(currentTime.tv_usec - m_data->mStartTime.tv_usec) / 1000;
#endif //__CELLOS_LV2__
#endif
}

/// Returns the time in us since the last call to reset or since
/// the Clock was created.
unsigned long long int Clock::getTimeMicroseconds()
{
#ifdef BT_USE_WINDOWS_TIMERS
	//see https://msdn.microsoft.com/en-us/library/windows/desktop/dn553408(v=vs.85).aspx	
	LARGE_INTEGER currentTime, elapsedTime;

	QueryPerformanceCounter(&currentTime);
	elapsedTime.QuadPart = currentTime.QuadPart -
		m_data->mStartTime.QuadPart;
	elapsedTime.QuadPart *= 1000000;
	elapsedTime.QuadPart /= m_data->mClockFrequency.QuadPart;

	return (unsigned long long) elapsedTime.QuadPart;
#else

#ifdef __CELLOS_LV2__
	uint64_t freq = sys_time_get_timebase_frequency();
	double dFreq = ((double)freq) / 1000000.0;
	typedef uint64_t  ClockSize;
	ClockSize newTime;
	//__asm __volatile__( "mftb %0" : "=r" (newTime) : : "memory");
	SYS_TIMEBASE_GET(newTime);

	return (unsigned long int)((double(newTime - m_data->mStartTime)) / dFreq);
#else

	struct timeval currentTime;
	gettimeofday(&currentTime, 0);
	return (currentTime.tv_sec - m_data->mStartTime.tv_sec) * 1000000 +
		(currentTime.tv_usec - m_data->mStartTime.tv_usec);
#endif//__CELLOS_LV2__
#endif
}

unsigned long long int Clock::getTimeNanoseconds()
{
#ifdef BT_USE_WINDOWS_TIMERS
	//see https://msdn.microsoft.com/en-us/library/windows/desktop/dn553408(v=vs.85).aspx
	LARGE_INTEGER currentTime, elapsedTime;

	QueryPerformanceCounter(&currentTime);
	elapsedTime.QuadPart = currentTime.QuadPart -
		m_data->mStartTime.QuadPart;
	elapsedTime.QuadPart *= 1000000000;
	elapsedTime.QuadPart /= m_data->mClockFrequency.QuadPart;

	return (unsigned long long) elapsedTime.QuadPart;
#else

#ifdef __CELLOS_LV2__
	uint64_t freq = sys_time_get_timebase_frequency();
	double dFreq = ((double)freq) / 1e9;
	typedef uint64_t  ClockSize;
	ClockSize newTime;
	//__asm __volatile__( "mftb %0" : "=r" (newTime) : : "memory");
	SYS_TIMEBASE_GET(newTime);

	return (unsigned long int)((double(newTime - m_data->mStartTime)) / dFreq);
#else
#ifdef __APPLE__
	uint64_t ticks = mach_absolute_time() - m_data->mStartTimeNano;
	static long double conversion = 0.0L;
	if (0.0L == conversion)
	{
		// attempt to get conversion to nanoseconds
		mach_timebase_info_data_t info;
		int err = mach_timebase_info(&info);
		if (err)
		{
			btAssert(0);
			conversion = 1.;
		}
		conversion = info.numer / info.denom;
	}
	return (ticks * conversion);


#else//__APPLE__

#ifdef BT_LINUX_REALTIME
	timespec ts;
	clock_gettime(CLOCK_REALTIME, &ts);
	return 1000000000 * ts.tv_sec + ts.tv_nsec;
#else
	struct timeval currentTime;
	gettimeofday(&currentTime, 0);
	return (currentTime.tv_sec - m_data->mStartTime.tv_sec) * 1e9 +
		(currentTime.tv_usec - m_data->mStartTime.tv_usec) * 1000;
#endif //BT_LINUX_REALTIME

#endif//__APPLE__
#endif//__CELLOS_LV2__
#endif 
}


/// Returns the time in s since the last call to reset or since 
/// the Clock was created.
float Clock::getTimeSeconds()
{
	static const float microseconds_to_seconds = float(0.000001);
	return float(getTimeMicroseconds()) * microseconds_to_seconds;
}