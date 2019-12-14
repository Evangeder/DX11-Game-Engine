#include "VoxaNovusWindow.h"
#include <sstream>

int CALLBACK WinMain(
	_In_ HINSTANCE hInstance,
	_In_opt_ HINSTANCE hPrevInstance,
	_In_ LPSTR lpCmdLine,
	_In_ int nCmdShow)
{
	try {
		Window wnd(800, 600, "Voxa Novus DX11");

		MSG msg; BOOL gResult;
		while ((gResult = GetMessage(&msg, nullptr, 0, 0)) > 0) {
			TranslateMessage(&msg);
			DispatchMessage(&msg);
		}

		if (gResult == -1)
			return -1;
		else
			return msg.wParam;
	}
	catch (const VoxaNovusException & e)
	{
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

/*
#include <sstream>

LRESULT CALLBACK WndProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam) 
{ 
	switch (msg) {
		case WM_CLOSE:
			PostQuitMessage(0);
			break;
		case WM_KEYDOWN:
			if (wParam == 'F') {
				SetWindowText(hWnd, "yep");
			}
			break;
		case WM_KEYUP:
			if (wParam == 'F') {
				SetWindowText(hWnd, "nope");
			}
			break;
		case WM_CHAR:
			if (wParam != 'F' && wParam != 'f') {
				static std::string title;
				title.push_back((char)wParam);
				SetWindowText(hWnd, title.c_str());
			}
			break;
		case WM_MOUSEMOVE:
			POINTS pt = MAKEPOINTS(lParam);
			std::ostringstream oss;
			oss << "(" << pt.x << "," << pt.y << ")";
			SetWindowText(hWnd, oss.str().c_str());
			break;
	}
	return DefWindowProc(hWnd, msg, wParam, lParam);
}


int CALLBACK WinMain(
	_In_ HINSTANCE hInstance,
	_In_opt_ HINSTANCE hPrevInstance,
	_In_ LPSTR lpCmdLine,
	_In_ int nCmdShow)
{
	const auto pClassName = "VNWclass";
	// register window class
	WNDCLASSEX WindowClass = { 0 };
	WindowClass.cbSize = sizeof(WindowClass);
	WindowClass.style = CS_OWNDC;
	WindowClass.lpfnWndProc = WndProc;
	WindowClass.cbClsExtra = 0;
	WindowClass.hInstance = hInstance;
	WindowClass.hIcon = nullptr;
	WindowClass.hCursor = nullptr;
	WindowClass.hIcon = nullptr;
	WindowClass.hbrBackground = nullptr;
	WindowClass.lpszMenuName = nullptr;
	WindowClass.lpszClassName = pClassName;
	WindowClass.hIconSm = nullptr;

	RegisterClassEx(&WindowClass);
	// create window instance from window class
	HWND hWnd = CreateWindowEx(
		0,
		pClassName,
		"testing!",
		WS_CAPTION | WS_MINIMIZEBOX | WS_SYSMENU,
		200, 200, 640, 480, nullptr, nullptr, hInstance, nullptr
	);
	
	ShowWindow(hWnd, SW_SHOW);

	// message pump
	MSG msg; BOOL gResult;
	while ((gResult = GetMessage(&msg, nullptr, 0, 0)) > 0) {
		TranslateMessage(&msg);
		DispatchMessage(&msg);
	}

	if (gResult == -1)
		return -1;
	else
		msg.wParam;
}*/