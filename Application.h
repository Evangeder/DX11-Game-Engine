#pragma once
#include "VoxaNovusWindow.h"
#include "VoxaNovusClock.h"
#include "ImguiManager.h"

class Application {
public:
	Application();
	int Go();
	~Application();
private:
	void DoFrame();
private:
	ImguiManager imgui;
	Window wnd;
	VoxaNovusClock timer;
	std::vector<std::unique_ptr<class Drawable>> drawables;
	static constexpr size_t nDrawables = 180;
	float SimulationSpeed = 1.0f;
	std::vector<float> FPS_History;
	bool ShowDebugWindows = true;
	float FPS_Max = 0.0f;
	float FPS_CurrentScale = 0.0f;
	float FPS_Min = 0.0f;
	float FPS_CurrentScaleMin = 0.0f;
	int FPS_Histogram_ElementCount = 50;
};