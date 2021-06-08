#include "Window.h"
#include "WindowClass.h"
#include "WindowFactory.h"
#include <optional>
#include <Windows.h>

LRESULT WndProc(Window* pWindow, HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam) {
	switch (msg) {
	case WM_CLOSE:
		PostQuitMessage(1);
		break;
	}
	return DefWindowProcW(hWnd, msg, wParam, lParam);
}

int CALLBACK WinMain(
	HINSTANCE hInstance,
	HINSTANCE hPrevInstance,
	LPSTR lpCmdLine,
	int nShowCmd
) {
	const auto pClassName = L"d3dTest";
	const auto pWindowName = L"d3dTest Window";
	
	// Register class
	WindowClass wc{ hInstance, pClassName };
	wc.registerClass();

	// Create window instance
	Window w{
		WindowFactory{ hInstance, wc.getClassName(), pWindowName, WndProc }
		.addWindowStyle(WS_MINIMIZEBOX)
		.setClientSize(960, 540)
		.build()
	};
	/*HWND hWnd = CreateWindowExW(
		0, wc.getClassName(), pWindowName, WS_SYSMENU | WS_CAPTION | WS_MINIMIZEBOX,
		0, 0, 960, 540,
		nullptr, nullptr, hInstance, nullptr
	);*/

	// Show window
	w.showWindow();

	std::optional<int> exitCode{};
	while (true) {
		exitCode = w.processMessagesOnQueue();
		if (exitCode) return *exitCode; // if the exitCode isn't empty, return its value
	}

	return 0;
}