#ifndef B3_CLOCK_H
#define B3_CLOCK_H
#define USE_CLOCK 1
//copyright see https://github.com/bulletphysics/bullet3
// Credits: The Clock class was inspired by the Timer classes in 
// Ogre (www.ogre3d.org).
#ifdef USE_CLOCK
#define FLT_EPSILON     1.192092896e-07F        /* smallest such that 1.0+FLT_EPSILON != 1.0 */
#define SIMD_EPSILON FLT_EPSILON
///The btClock is a portable basic clock that measures accurate time in seconds, use for profiling.
class Clock
{
public:
	Clock();

	Clock(const Clock& other);
	Clock& operator=(const Clock& other);

	~Clock();

	/// Resets the initial reference time.
	void reset();

	/// Returns the time in ms since the last call to reset or since 
	/// the btClock was created.
	unsigned long long int getTimeMilliseconds();

	/// Returns the time in us since the last call to reset or since 
	/// the Clock was created.
	unsigned long long int getTimeMicroseconds();

	unsigned long long int getTimeNanoseconds();

	/// Returns the time in s since the last call to reset or since 
	/// the Clock was created.
	float getTimeSeconds();

private:
	struct ClockData* m_data;
};

#endif //USE_BT_CLOCK

#endif //B3_CLOCK_H
