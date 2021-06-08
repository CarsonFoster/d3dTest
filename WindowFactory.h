#ifndef CWF_WINDOWFACTORY_H
#define CWF_WINDOWFACTORY_H

#include <Windows.h>
#include "Window.h"

class WindowFactory {
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
	WindowFactory(HINSTANCE aHInstance, LPCWSTR aClassName, LPCWSTR aWindowName, Window::ClientWindowProc windowProc) noexcept;
	~WindowFactory() = default;

	// no copy init/assign
	WindowFactory(const WindowFactory& o) = delete;
	WindowFactory& operator=(const WindowFactory& o) = delete;

	WindowFactory& setExtendedWindowStyle(DWORD style) noexcept;
	WindowFactory& addExtendedWindowStyle(DWORD style) noexcept;
	WindowFactory& setWindowStyle(DWORD style) noexcept;
	WindowFactory& addWindowStyle(DWORD style) noexcept;
	WindowFactory& setClientSize(int aClientWidth, int aClientHeight) noexcept;
	WindowFactory& setPosition(int aX, int aY) noexcept;
	WindowFactory& setParentHandle(HWND parent) noexcept;
	WindowFactory& setMenuHandle(HMENU menu) noexcept;

	Window::WindowInitializationStruct build(); // should use move semantics on the struct
};

#endif