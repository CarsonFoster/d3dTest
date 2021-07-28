#include "CwfException.h"
#include "Window.h"
#include "WindowBuilder.h"
#include <Windows.h>

WindowBuilder::WindowBuilder(HINSTANCE hInstance, LPCWSTR className, 
							 LPCWSTR windowName, Window::ClientWindowProc windowProc) noexcept
							 : m_hInstance{ hInstance }, m_className{ className }, m_windowName{ windowName }, 
							 m_clientWindowProc{ windowProc } {
	// does nothing else I think
}

WindowBuilder& WindowBuilder::setExtendedWindowStyle(DWORD style) noexcept {
	m_extendedStyle = style;
	return *this;
}

WindowBuilder& WindowBuilder::addExtendedWindowStyle(DWORD style) noexcept {
	m_extendedStyle |= style;
	return *this;
}

WindowBuilder& WindowBuilder::setWindowStyle(DWORD style) noexcept {
	m_windowStyle = style;
	return *this;
}

WindowBuilder& WindowBuilder::addWindowStyle(DWORD style) noexcept {
	m_windowStyle |= style;
	return *this;
}

WindowBuilder& WindowBuilder::setClientSize(int clientWidth, int clientHeight) noexcept {
	m_clientWidth = clientWidth;
	m_clientHeight = clientHeight;
	return *this;
}

WindowBuilder& WindowBuilder::setPosition(int x, int y) noexcept {
	m_x = x;
	m_y = y;
	return *this;
}

WindowBuilder& WindowBuilder::setParentHandle(HWND parent) noexcept {
	m_hParent = parent;
	return *this;
}

WindowBuilder& WindowBuilder::setMenuHandle(HMENU menu) noexcept {
	m_hMenu = menu;
	return *this;
}

void WindowBuilder::adjustRect() {
	RECT r{};
	r.left = 100;
	r.top = 100;
	r.right = r.left + m_clientWidth;
	r.bottom = r.top + m_clientHeight;
	bool hasMenu = (m_windowStyle & WS_SYSMENU & WS_CAPTION);
	if (!AdjustWindowRectEx(&r, m_windowStyle, hasMenu, m_extendedStyle)) { // if it returns zero, there has been an error
		// throw CWF_EXCEPTION(CwfException::Type::WINDOWS, L"AdjustWindowRectEx call failed");
		throw CWF_LAST_EXCEPTION();
	}
	m_windowWidth = r.right - r.left;
	m_windowHeight = r.bottom - r.top;
}

Window::WindowInitializationStruct WindowBuilder::build() {
	adjustRect();
	Window::WindowInitializationStruct toReturn{ m_extendedStyle, m_className, m_windowName, m_windowStyle, m_x, m_y,
					m_windowWidth, m_windowHeight, m_clientWidth, m_clientHeight, m_hParent, m_hMenu, m_hInstance, m_clientWindowProc };
	return toReturn;
}