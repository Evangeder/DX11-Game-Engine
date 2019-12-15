#include "Application.h"

int CALLBACK WinMain(
	_In_ HINSTANCE hInstance,
	_In_opt_ HINSTANCE hPrevInstance,
	_In_ LPSTR lpCmdLine,
	_In_ int nCmdShow)
{
	try { 
		return Application().Go(); 
	}
	catch (const VoxaNovusException & e) {
		MessageBox(nullptr, e.what(), "VoxaNovus has crashed", MB_OK | MB_ICONEXCLAMATION);
	}
	catch (const std::exception & e) {
		MessageBox(nullptr, e.what(), "VoxaNovus has crashed", MB_OK | MB_ICONEXCLAMATION);
	}
	catch (...) {
		MessageBox(nullptr, "No details available", "VoxaNovus has crashed", MB_OK | MB_ICONEXCLAMATION);
	}
	return -1;
}