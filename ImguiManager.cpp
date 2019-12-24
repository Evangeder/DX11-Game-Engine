#include "ImguiManager.h"
#include "imgui/imgui.h"

ImguiManager::ImguiManager() {
	IMGUI_CHECKVERSION();
	ImGui::CreateContext();
	ImGuiIO& io = ImGui::GetIO(); (void)io;
	io.ConfigFlags |= ImGuiConfigFlags_DockingEnable;
	ImGui::StyleColorsDark();
}

ImguiManager::~ImguiManager() {
	ImGui::DestroyContext();
}