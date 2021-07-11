#include "pit.h"
#include <IO.h>

namespace PIT {
	volatile double TimeSinceBoot = 0;

	uint16_t  Divisor = 65535;

	void SetDivisor(uint16_t divisor) {
		if (divisor < 100)
			divisor = 100;
		Divisor = divisor;

		outb(0X40, (uint8_t)(divisor & 0x00FF));
		io_wait();
		outb(0X40, (uint8_t)((divisor & 0xFF00) >> 8));
		io_wait();
	}
	uint64_t GetFrequency() {
		return BaseFrequency / Divisor;
	}
	void SetFrequency(uint64_t frequency) {
		SetDivisor(BaseFrequency / frequency);
	}
	void Tick() {
		TimeSinceBoot += 1 / (double)GetFrequency();
	}

	void Sleepd(double seconds) {
		double startTime = TimeSinceBoot;
		while (TimeSinceBoot < startTime + seconds) {
			asm("hlt");
		}
	}
	void Sleep(uint64_t milliseconds) {
		Sleepd((double)milliseconds / 1000);
	}
}