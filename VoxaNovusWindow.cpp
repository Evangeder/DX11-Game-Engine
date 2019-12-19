#include "VoxaNovusWindow.h"
#include "resource.h"
#include "imgui/imgui_impl_win32.h"
#include <sstream>

// Window class
Window::WindowClass Window::WindowClass::wndClass;

Window::WindowClass::WindowClass() noexcept : hInst(GetModuleHandle(nullptr)) {
	WNDCLASSEX WindowClass = { 0 };
	WindowClass.cbSize = sizeof(WindowClass);
	WindowClass.style = CS_OWNDC;
	WindowClass.lpfnWndProc = HandleMsgSetup;
	WindowClass.cbClsExtra = 0;
	WindowClass.hInstance = GetInstance();
	WindowClass.hIcon = nullptr;
	WindowClass.hCursor = nullptr;
	WindowClass.hIcon = static_cast<HICON>(LoadImage(hInst, MAKEINTRESOURCE(IDI_ICON1), IMAGE_ICON, 32,32,0));
	WindowClass.hbrBackground = nullptr;
	WindowClass.lpszMenuName = nullptr;
	WindowClass.lpszClassName = GetName();
	WindowClass.hIconSm = static_cast<HICON>(LoadImage(hInst, MAKEINTRESOURCE(IDI_ICON1), IMAGE_ICON, 16, 16, 0));;
	RegisterClassEx(&WindowClass);
}
Window::WindowClass::~WindowClass() {
	UnregisterClass(wndClassName, GetInstance());
}

const char* Window::WindowClass::GetName() noexcept {
	return wndClassName;
}

HINSTANCE Window::WindowClass::GetInstance() noexcept {
	return wndClass.hInst;
}

Window::Window(int width, int height, const char* name) : width(width), height(height) {
	// calc the windowsize
	RECT WindowRectangle;
	WindowRectangle.left = 100;
	WindowRectangle.right = width + WindowRectangle.left;
	WindowRectangle.top = 100;
	WindowRectangle.bottom = height + WindowRectangle.top;

	if (AdjustWindowRect(&WindowRectangle, WS_CAPTION | WS_MINIMIZEBOX | WS_SYSMENU, FALSE) == 0)
		throw VNWND_LAST_EXCEPT();
	
	hWnd = CreateWindow(
		WindowClass::GetName(),
		name,
		WS_CAPTION | WS_MINIMIZEBOX | WS_SYSMENU,
		CW_USEDEFAULT,
		CW_USEDEFAULT,
		WindowRectangle.right - WindowRectangle.left,
		WindowRectangle.bottom - WindowRectangle.top,
		nullptr,
		nullptr,
		WindowClass::GetInstance(),
		this
	);

	if (hWnd == nullptr)
		throw VNWND_LAST_EXCEPT();

	ShowWindow(hWnd, SW_SHOWDEFAULT);

	ImGui_ImplWin32_Init(hWnd);

	pGfx = std::make_unique<Graphics>(hWnd);
}

Window::~Window() {
	ImGui_ImplWin32_Shutdown();
	DestroyWindow(hWnd);
}

std::optional<int> Window::ProcessMessages() {
	MSG msg;
	while (PeekMessage(&msg, nullptr, 0, 0, PM_REMOVE)) {
		if (msg.message == WM_QUIT)
			return (int)msg.wParam;
		TranslateMessage(&msg);
		DispatchMessage(&msg);
	}
	return {};
}

Graphics& Window::Gfx()
{
	if (!pGfx)
		throw VNWND_NOGFX_EXCEPT();

	return *pGfx;
}

void Window::SetTitle(const std::string& title) noexcept {
	if (SetWindowText(hWnd, title.c_str()) == 0)
		throw VNWND_LAST_EXCEPT();
}

LRESULT WINAPI Window::HandleMsgSetup(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam) noexcept {
	if (msg == WM_NCCREATE) {
		const CREATESTRUCTW* const pCreate = reinterpret_cast<CREATESTRUCTW*>(lParam);
		Window* const pWnd = static_cast<Window*>(pCreate->lpCreateParams);
		SetWindowLongPtr(hWnd, GWLP_USERDATA, reinterpret_cast<LONG_PTR>(pWnd));
		SetWindowLongPtr(hWnd, GWLP_WNDPROC, reinterpret_cast<LONG_PTR>(&Window::HandleMsgThunk));
		return pWnd->HandleMsg(hWnd, msg, wParam, lParam);
	}
	return DefWindowProc(hWnd, msg, wParam, lParam);
}

LRESULT WINAPI Window::HandleMsgThunk(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam) noexcept {
	Window* const pWnd = reinterpret_cast<Window*>(GetWindowLongPtr(hWnd, GWLP_USERDATA));
	return pWnd->HandleMsg(hWnd, msg, wParam, lParam);
}

LRESULT Window::HandleMsg(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam) noexcept {
	if (ImGui_ImplWin32_WndProcHandler(hWnd, msg, wParam, lParam))
	{
		return true;
	}
	const auto& imio = ImGui::GetIO();
	
	POINTS pt;

	switch (msg) {
	case WM_CLOSE:
		PostQuitMessage(0);
		return 0;

	case WM_KILLFOCUS:
		keyboard.ClearState();
		break;

	case WM_SYSKEYDOWN:
	case WM_KEYDOWN:
		if (imio.WantCaptureKeyboard) break;
		if (!(lParam & 0x40000000) || keyboard.AutorepeatIsEnabled())
			keyboard.OnKeyPressed(static_cast<unsigned char>(wParam));
		break;

	case WM_SYSKEYUP:
	case WM_KEYUP:
		if (imio.WantCaptureKeyboard) break;
		keyboard.OnKeyReleased(static_cast<unsigned char>(wParam));
		break;

	case WM_CHAR:
		if (imio.WantCaptureKeyboard) break;
		keyboard.OnChar(static_cast<unsigned char>(wParam));
		break;

	case WM_MOUSEMOVE:
		if (imio.WantCaptureMouse) break;
		pt = MAKEPOINTS(lParam);
		if (pt.x >= 0 && pt.x < width && pt.y >= 0 && pt.y < height) {
			mouse.OnMouseMove(pt.x, pt.y);
			if (!mouse.IsInWindow()) {
				SetCapture(hWnd);
				mouse.OnMouseEnter();
			}
		}
		else {
			if (wParam & (MK_LBUTTON | MK_RBUTTON | MK_MBUTTON))
				mouse.OnMouseMove(pt.x, pt.y);
			else {
				ReleaseCapture();
				mouse.OnMouseLeave();
			}
		}
		break;

	case WM_LBUTTONDOWN:
		if (imio.WantCaptureMouse) break;
		pt = MAKEPOINTS(lParam);
		mouse.OnLeftPressed(pt.x, pt.y);
		break;

	case WM_LBUTTONUP:
		if (imio.WantCaptureMouse) break;
		pt = MAKEPOINTS(lParam);
		mouse.OnLeftReleased(pt.x, pt.y);
		break;

	case WM_RBUTTONDOWN:
		if (imio.WantCaptureMouse) break;
		pt = MAKEPOINTS(lParam);
		mouse.OnRightPressed(pt.x, pt.y);
		break;

	case WM_RBUTTONUP:
		if (imio.WantCaptureMouse) break;
		pt = MAKEPOINTS(lParam);
		mouse.OnRightReleased(pt.x, pt.y);
		break;

	case WM_MBUTTONDOWN:
		if (imio.WantCaptureMouse) break;
		pt = MAKEPOINTS(lParam);
		mouse.OnWheelPressed(pt.x, pt.y);
		break;

	case WM_MBUTTONUP:
		if (imio.WantCaptureMouse) break;
		pt = MAKEPOINTS(lParam);
		mouse.OnWheelReleased(pt.x, pt.y);
		break;

	case WM_MOUSEWHEEL:
		if (imio.WantCaptureMouse) break;
		pt = MAKEPOINTS(lParam);
		const int delta = GET_WHEEL_DELTA_WPARAM(wParam);
		mouse.OnWheelDelta(pt.x, pt.y, delta);
	}

	return DefWindowProc(hWnd, msg, wParam, lParam);
}

// Window Exceptions
Window::Exception::Exception(int line, const char* file, HRESULT hr) noexcept
	: VoxaNovusException(line, file), hr(hr) { }

const char* Window::Exception::what() const noexcept {
	std::ostringstream sb;
	sb << GetType() << std::endl
		<< GetErrorCode() << ":" << GetOriginString() << std::endl;
	whatBuffer = sb.str();
	return whatBuffer.c_str();
}

const char* Window::Exception::GetType() const noexcept {
	return "Voxa Novus has crashed with code:";
}

std::string Window::Exception::TranslateErrorCode(HRESULT hr) noexcept {
	char* pMsgBuf = nullptr;
	DWORD nMsgLen = FormatMessage(
		FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM | FORMAT_MESSAGE_IGNORE_INSERTS,
		nullptr,
		hr,
		MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT),
		reinterpret_cast<LPSTR>(&pMsgBuf),
		0,
		nullptr
	);
	if (nMsgLen == 0) return "Unidentified error code.";
	std::string errorString = pMsgBuf;
	LocalFree(pMsgBuf);
	return errorString;
}

HRESULT Window::Exception::GetErrorCode() const noexcept {
	return hr;
}

std::string Window::Exception::GetErrorString() const noexcept {
	return TranslateErrorCode(hr);
}

const char* Window::NoGfxException::GetType() const noexcept
{
	return "VoxaNovus Crashed - No Graphics Card";
}