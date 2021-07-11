#pragma once
#include <types.h>

namespace PIT {
	volatile extern double TimeSinceBoot;
	const uint64_t BaseFrequency = 1193182;

	void SetDivisor(uint16_t divisor);
	uint64_t GetFrequency();
	void SetFrequency(uint64_t frequency);
	void Tick();

	void Sleepd(double seconds);
	void Sleep(uint64_t milliseconds);
}