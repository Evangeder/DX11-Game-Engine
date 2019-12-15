#include "VoxaNovusClock.h"

using namespace std::chrono;

VoxaNovusClock::VoxaNovusClock() noexcept {
	last = steady_clock::now();
}

float VoxaNovusClock::Mark() noexcept {
	const auto old = last;
	last = steady_clock::now();
	const duration<float> frameTime = last - old;
	return frameTime.count();
}

float VoxaNovusClock::Peek() const noexcept {
	return duration<float>(steady_clock::now() - last).count();
}