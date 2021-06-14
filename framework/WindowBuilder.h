#ifndef CWF_WINDOWBUILDER_H
#define CWF_WINDOWBUILDER_H

#include <Windows.h>
#include "Window.h"

class WindowBuilder {
private:
	HINSTANCE hInstance;
	LPCWSTR className;
	LPCWSTR windowName;
	DWORD windowStyle{ WS_CAPTION | WS_SYSMENU };
	DWORD extendedStyle{ 0 };
	int x{ 0 };
	int y{ 0 };
	int clientWidth{ 640 };
	int clientHeight{ 480 };
	int windowWidth{};
	int windowHeight{};
	HWND hParent{};
	HMENU hMenu{};
	Window::ClientWindowProc clientWindowProc;

	void adjustRect();
public:
	WindowBuilder(HINSTANCE aHInstance, LPCWSTR aClassName, LPCWSTR aWindowName, Window::ClientWindowProc windowProc) noexcept;
	~WindowBuilder() = default;

	// no copy init/assign
	WindowBuilder(const WindowBuilder& o) = delete;
	WindowBuilder& operator=(const WindowBuilder& o) = delete;

	WindowBuilder& setExtendedWindowStyle(DWORD style) noexcept;
	WindowBuilder& addExtendedWindowStyle(DWORD style) noexcept;
	WindowBuilder& setWindowStyle(DWORD style) noexcept;
	WindowBuilder& addWindowStyle(DWORD style) noexcept;
	WindowBuilder& setClientSize(int aClientWidth, int aClientHeight) noexcept;
	WindowBuilder& setPosition(int aX, int aY) noexcept;
	WindowBuilder& setParentHandle(HWND parent) noexcept;
	WindowBuilder& setMenuHandle(HMENU menu) noexcept;

	Window::WindowInitializationStruct build(); // should use move semantics on the struct
};

#endif