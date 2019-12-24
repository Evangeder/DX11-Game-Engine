#include "Application.h"
#include "VoxaNovusMath.h"
#include "VoxaNovusSurface.h"
#include "VoxaNovusGDIPlusManager.h"
#include "Box.h"
#include "Pyramid.h"
#include "imgui/imgui.h"
#include <iostream>
#include <iomanip>
#include <cmath>
#include <limits>
#include <sstream>
#include <algorithm>
#include <memory>

namespace dx = DirectX;

GDIPlusManager gdipm;

Application::Application() : wnd(1024, 768, "VoxaNovus DX11"), light(wnd.Gfx()) {
	class Factory {
	public:
		Factory(Graphics& gfx) : gfx(gfx) { }

		std::unique_ptr<Drawable> operator()() {
			return std::make_unique<Box>(
				gfx, rng, adist, ddist,
				odist, rdist, bdist
			);
		}
	private:
		Graphics& gfx;
		std::mt19937 rng{ std::random_device{}() };
		std::uniform_real_distribution<float> adist{ 0.0f,PI * 2.0f };
		std::uniform_real_distribution<float> ddist{ 0.0f,PI * 0.5f };
		std::uniform_real_distribution<float> odist{ 0.0f,PI * 0.08f };
		std::uniform_real_distribution<float> rdist{ 6.0f,20.0f };
		std::uniform_real_distribution<float> bdist{ 0.4f,3.0f };
	};

	Factory f(wnd.Gfx());
	drawables.reserve(nDrawables);
	std::generate_n(std::back_inserter(drawables), nDrawables, f);

	wnd.Gfx().SetProjection(dx::XMMatrixPerspectiveLH(1.0f, 3.0f / 4.0f, 0.5f, 400.0f));
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
	//wnd.Gfx().BeginFrameHex(0x64, 0x95, 0xED);
	wnd.Gfx().BeginFrameHex(0x10, 0x10, 0x10);
	wnd.Gfx().SetCamera(cam.GetMatrix());
	light.Bind(wnd.Gfx());
	//wnd.Gfx().ClearBufferHex(0x64, 0x95, 0xED);
	for (auto& b : drawables)
	{
		b->Update(dt);
		b->Update(wnd.keyboard.IsKeyPressed(VK_SPACE) ? 0.0f : dt);
		b->Draw(wnd.Gfx());
	}
	
	light.Draw(wnd.Gfx());
	
	if (wnd.keyboard.ReadChar() == '`')
		ShowDebugWindows = !ShowDebugWindows;

	FPS_History.insert(FPS_History.begin(), 1000.0f / ImGui::GetIO().Framerate);
	if (FPS_History.size() > FPS_Histogram_ElementCount) FPS_History.pop_back();
	float* a = &FPS_History[0];

	FPS_Max = *std::max_element(FPS_History.begin(), FPS_History.end());
	FPS_Min = *std::min_element(FPS_History.begin(), FPS_History.end());

	float d = FPS_Max - FPS_Min;
	if (d > 0.3f) d = 0.3f;

	FPS_CurrentScale = FPS_Max * (1.0f + d/10.0f);
	FPS_CurrentScaleMin = FPS_Min * (1.0f - d/10.0f);

	ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0.0f, 0.0f));
	ImGui::Begin("DockSpace Demo", NULL, ImGuiWindowFlags_NoDocking | ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoDecoration | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoBringToFrontOnFocus | ImGuiWindowFlags_NoFocusOnAppearing | ImGuiWindowFlags_NoNavFocus | ImGuiWindowFlags_NoBackground);
	ImGui::PopStyleVar();
	ImGuiID dockspace_id = ImGui::GetID("MyDockSpace");
	ImGui::DockSpace(dockspace_id, ImVec2(0.0f, 0.0f), ImGuiDockNodeFlags_PassthruCentralNode | ImGuiWindowFlags_NoMove);
	ImGui::End();

	if (ShowDebugWindows) {
		char buf[128];
#pragma warning(suppress : 4996)
		sprintf(buf, "%.2fms (%.0f FPS)", 1000.0f / ImGui::GetIO().Framerate, ImGui::GetIO().Framerate);

		//ImVec2 window_pos = ImVec2(1, 1);
		//ImVec2 window_pos_pivot = ImVec2(0.0f, 0.0f);
		//ImGui::SetNextWindowPos(window_pos, ImGuiCond_Always, window_pos_pivot);
		//ImGui::SetNextWindowBgAlpha(0.75f);
		if (ImGui::Begin("Framerate", NULL, ImGuiWindowFlags_AlwaysAutoResize | ImGuiWindowFlags_NoFocusOnAppearing)) {
			ImGui::PlotHistogram("", a, FPS_Histogram_ElementCount, 0, buf, FPS_CurrentScaleMin, FPS_CurrentScale, ImVec2(150, 40));
		}
		ImGui::End();

		if (ImGui::Begin("Simulation", NULL, ImGuiWindowFlags_AlwaysAutoResize | ImGuiWindowFlags_NoFocusOnAppearing)) {
			ImGui::SliderFloat("DeltaTime scale", &SimulationSpeed, 0.0f, 2.0f);
		}
		ImGui::End();
	}

	cam.SpawnControlWindow();
	light.SpawnControlWindow();

	wnd.Gfx().EndFrame();
}