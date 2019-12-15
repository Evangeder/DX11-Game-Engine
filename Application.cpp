#include "Application.h"
#include <iostream>
#include <iomanip>
#include <cmath>
#include <limits>
#include <sstream>

Application::Application() : wnd(800, 600, "VoxaNovus DX11") { }

int Application::Go() {
	while (true) {
		if (const auto ecode = Window::ProcessMessages())
			return *ecode;
		DoFrame();
	}
}

void Application::DoFrame() {
	//float const c = sin(timer.Peek()) / 2.0f + 0.5f;
	//wnd.Gfx().ClearBuffer(c, c, 1.0f);
	wnd.Gfx().ClearBufferHex(0x64, 0x95, 0xED);
	wnd.Gfx().DrawTestTriangle(
		-timer.Peek() / 3.14f,
		0.0f,
		0.0f
	);
	wnd.Gfx().DrawTestTriangle(
		timer.Peek(),
		0.0f,
		0.0f
	);
	wnd.Gfx().EndFrame();
}