#include "CwfException.h"
#include "Window.h"
#include "WindowBuilder.h"
#include <Windows.h>

WindowBuilder::WindowBuilder(HINSTANCE aHInstance, LPCWSTR aClassName, 
							 LPCWSTR aWindowName, Window::ClientWindowProc windowProc) noexcept
							 : hInstance{ aHInstance }, className{ aClassName }, windowName{ aWindowName }, 
							 clientWindowProc{ windowProc } {
	// does nothing else I think
}

WindowBuilder& WindowBuilder::setExtendedWindowStyle(DWORD style) noexcept {
	extendedStyle = style;
	return *this;
}

WindowBuilder& WindowBuilder::addExtendedWindowStyle(DWORD style) noexcept {
	extendedStyle |= style;
	return *this;
}

WindowBuilder& WindowBuilder::setWindowStyle(DWORD style) noexcept {
	windowStyle = style;
	return *this;
}

WindowBuilder& WindowBuilder::addWindowStyle(DWORD style) noexcept {
	windowStyle |= style;
	return *this;
}

WindowBuilder& WindowBuilder::setClientSize(int aClientWidth, int aClientHeight) noexcept {
	clientWidth = aClientWidth;
	clientHeight = aClientHeight;
	return *this;
}

WindowBuilder& WindowBuilder::setPosition(int aX, int aY) noexcept {
	x = aX;
	y = aY;
	return *this;
}

WindowBuilder& WindowBuilder::setParentHandle(HWND parent) noexcept {
	hParent = parent;
	return *this;
}

WindowBuilder& WindowBuilder::setMenuHandle(HMENU menu) noexcept {
	hMenu = menu;
	return *this;
}

void WindowBuilder::adjustRect() {
	RECT r{};
	r.left = 100;
	r.top = 100;
	r.right = r.left + clientWidth;
	r.bottom = r.top + clientHeight;
	bool hasMenu = (windowStyle & (WS_SYSMENU | WS_CAPTION)); // TODO: determine if this does what I want it to do
	if (!AdjustWindowRectEx(&r, windowStyle, hasMenu, extendedStyle)) { // if it returns zero, there has been an error
		// throw CWF_EXCEPTION(CwfException::Type::WINDOWS, L"AdjustWindowRectEx call failed");
		throw CWF_LAST_EXCEPTION();
	}
	windowWidth = r.right - r.left;
	windowHeight = r.bottom - r.top;
}

Window::WindowInitializationStruct WindowBuilder::build() {
	adjustRect();
	Window::WindowInitializationStruct toReturn{ extendedStyle, className, windowName, windowStyle, x, y,
					windowWidth, windowHeight, clientWidth, clientHeight, hParent, hMenu, hInstance, clientWindowProc };
	return toReturn;
}