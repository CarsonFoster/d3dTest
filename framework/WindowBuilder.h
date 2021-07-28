#ifndef CWF_WINDOWBUILDER_H
#define CWF_WINDOWBUILDER_H

#include "Window.h"
#include <Windows.h>

class WindowBuilder {
private:
	HINSTANCE m_hInstance;
	LPCWSTR m_className;
	LPCWSTR m_windowName;
	DWORD m_windowStyle{ WS_CAPTION | WS_SYSMENU };
	DWORD m_extendedStyle{ 0 };
	int m_x{ 0 };
	int m_y{ 0 };
	int m_clientWidth{ 640 };
	int m_clientHeight{ 480 };
	int m_windowWidth{};
	int m_windowHeight{};
	HWND m_hParent{};
	HMENU m_hMenu{};
	Window::ClientWindowProc m_clientWindowProc;

	void adjustRect();
public:
	WindowBuilder(HINSTANCE hInstance, LPCWSTR className, LPCWSTR windowName, Window::ClientWindowProc windowProc) noexcept;
	~WindowBuilder() = default;

	// no copy init/assign
	WindowBuilder(const WindowBuilder& o) = delete;
	WindowBuilder& operator=(const WindowBuilder& o) = delete;

	WindowBuilder& setExtendedWindowStyle(DWORD style) noexcept;
	WindowBuilder& addExtendedWindowStyle(DWORD style) noexcept;
	WindowBuilder& setWindowStyle(DWORD style) noexcept;
	WindowBuilder& addWindowStyle(DWORD style) noexcept;
	WindowBuilder& setClientSize(int clientWidth, int clientHeight) noexcept;
	WindowBuilder& setPosition(int x, int y) noexcept;
	WindowBuilder& setParentHandle(HWND parent) noexcept;
	WindowBuilder& setMenuHandle(HMENU menu) noexcept;

	Window::WindowInitializationStruct build(); // should use move semantics on the struct
};

#endif