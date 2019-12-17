#include "Application.h"
#include "VoxaNovusBox.h"
#include "VoxaNovusMath.h"
#include "Pyramid.h"
#include "Melon.h"
#include "VoxaNovusSurface.h"
#include "VoxaNovusGDIPlusManager.h"
#include "VoxaNovusSheet.h"
#include "imgui/imgui.h"
#include <iostream>
#include <iomanip>
#include <cmath>
#include <limits>
#include <sstream>
#include <algorithm>
#include <memory>

GDIPlusManager gdipm;

Application::Application() : wnd(800, 600, "VoxaNovus DX11") {
	class Factory {
	public:
		Factory(Graphics& gfx) : gfx(gfx) { }
		std::unique_ptr<Drawable> operator()() {
			switch (typedist(rng)) {
			case 0:
				return std::make_unique<Pyramid>(
					gfx, rng, adist, ddist,
					odist, rdist
				);

			case 1:
				return std::make_unique<Box>(
					gfx, rng, adist, ddist,
					odist, rdist, bdist
					);

			case 2:
				return std::make_unique<Melon>(
					gfx, rng, adist, ddist,
					odist, rdist, longdist, latdist
					);
			case 3:
				return std::make_unique<Sheet>(
					gfx, rng, adist, ddist,
					odist, rdist, "Images\\feb.png"
					);
			default:
				assert(false && "bad drawable type in factory");
				return {};
			}
		}
	private:
		Graphics& gfx;
		std::mt19937 rng{ std::random_device{}() };
		std::uniform_real_distribution<float> adist{ 0.0f,PI * 2.0f };
		std::uniform_real_distribution<float> ddist{ 0.0f,PI * 0.5f };
		std::uniform_real_distribution<float> odist{ 0.0f,PI * 0.08f };
		std::uniform_real_distribution<float> rdist{ 6.0f,20.0f };
		std::uniform_real_distribution<float> bdist{ 0.4f,3.0f };
		std::uniform_int_distribution<int> latdist{ 5,20 };
		std::uniform_int_distribution<int> longdist{ 10,40 };
		std::uniform_int_distribution<int> typedist{ 0,3 };
	};

	Factory f(wnd.Gfx());
	drawables.reserve(nDrawables);
	std::generate_n(std::back_inserter(drawables), nDrawables, f);

	wnd.Gfx().SetProjection(DirectX::XMMatrixPerspectiveLH(1.0f, 3.0f / 4.0f, 0.5f, 40.0f));
}

int Application::Go() {
	while (true) {
		if (const auto ecode = Window::ProcessMessages())
			return *ecode;
		DoFrame();
	}
}

Application::~Application() {}

void Application::DoFrame() {
	const auto dt = timer.Mark() * SimulationSpeed;
	wnd.Gfx().BeginFrameHex(0x10, 0x10, 0x10);
	//wnd.Gfx().ClearBufferHex(0x64, 0x95, 0xED);
	for (auto& b : drawables)
	{
		b->Update(dt);
		b->Update(wnd.keyboard.IsKeyPressed(VK_SPACE) ? 0.0f : dt);
		b->Draw(wnd.Gfx());
	}
	if (wnd.keyboard.ReadChar() == '`')
		ShowDebugWindows = !ShowDebugWindows;

	FPS_History.insert(FPS_History.begin(), 1000.0f / ImGui::GetIO().Framerate);
	if (FPS_History.size() > FPS_Histogram_ElementCount) FPS_History.pop_back();
	float* a = &FPS_History[0];

	FPS_Max = *std::max_element(FPS_History.begin(), FPS_History.end());
	FPS_Min = *std::min_element(FPS_History.begin(), FPS_History.end());

	float d = FPS_Max - FPS_Min;
	if (d > 0.3f) d = 0.3f;

	ImGui::ShowDemoWindow();

	FPS_CurrentScale = FPS_Max * (1.0f + d/10.0f);
	FPS_CurrentScaleMin = FPS_Min * (1.0f - d/10.0f);

	if (ShowDebugWindows) {
		char buf[128];
#pragma warning(suppress : 4996)
		sprintf(buf, "%.2fms (%.0f FPS)", 1000.0f / ImGui::GetIO().Framerate, ImGui::GetIO().Framerate);

		ImVec2 window_pos = ImVec2(1, 1);
		ImVec2 window_pos_pivot = ImVec2(0.0f, 0.0f);
		ImGui::SetNextWindowPos(window_pos, ImGuiCond_Always, window_pos_pivot);
		ImGui::SetNextWindowBgAlpha(0.75f);
		if (ImGui::Begin("Framerate", NULL, ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoDecoration | ImGuiWindowFlags_AlwaysAutoResize | ImGuiWindowFlags_NoSavedSettings | ImGuiWindowFlags_NoFocusOnAppearing | ImGuiWindowFlags_NoNav | ImGuiWindowFlags_NoMouseInputs)) {
			ImGui::PlotHistogram("", a, FPS_Histogram_ElementCount, 0, buf, FPS_CurrentScaleMin, FPS_CurrentScale, ImVec2(150, 40));
		}
		ImGui::End();

		if (ImGui::Begin("Simulation", NULL, ImGuiWindowFlags_AlwaysAutoResize | ImGuiWindowFlags_NoSavedSettings | ImGuiWindowFlags_NoFocusOnAppearing)) {
			ImGui::SliderFloat("DeltaTime scale", &SimulationSpeed, 0.0f, 2.0f);
		}
		ImGui::End();
	}
	wnd.Gfx().EndFrame();
}