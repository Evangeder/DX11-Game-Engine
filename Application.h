#pragma once
#include "VoxaNovusWindow.h"
#include "VoxaNovusClock.h"

class Application {
public:
	Application();
	int Go();
private:
	void DoFrame();
private:
	Window wnd;
	VoxaNovusClock timer;
};