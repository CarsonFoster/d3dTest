#include "CwfException.h"
#include "Window.h"
#include "WindowFactory.h"
#include <Windows.h>

WindowFactory::WindowFactory(HINSTANCE aHInstance, LPCWSTR aClassName, 
							 LPCWSTR aWindowName, Window::ClientWindowProc windowProc) noexcept
							 : hInstance{ aHInstance }, className{ aClassName }, windowName{ aWindowName }, 
							 clientWindowProc{ windowProc } {
	// does nothing else I think
}

WindowFactory& WindowFactory::setExtendedWindowStyle(DWORD style) noexcept {
	extendedStyle = style;
	return *this;
}

WindowFactory& WindowFactory::addExtendedWindowStyle(DWORD style) noexcept {
	extendedStyle |= style;
	return *this;
}

WindowFactory& WindowFactory::setWindowStyle(DWORD style) noexcept {
	windowStyle = style;
	return *this;
}

WindowFactory& WindowFactory::addWindowStyle(DWORD style) noexcept {
	windowStyle |= style;
	return *this;
}

WindowFactory& WindowFactory::setClientSize(int aClientWidth, int aClientHeight) noexcept {
	clientWidth = aClientWidth;
	clientHeight = aClientHeight;
	return *this;
}

WindowFactory& WindowFactory::setPosition(int aX, int aY) noexcept {
	x = aX;
	y = aY;
	return *this;
}

WindowFactory& WindowFactory::setParentHandle(HWND parent) noexcept {
	hParent = parent;
	return *this;
}

WindowFactory& WindowFactory::setMenuHandle(HMENU menu) noexcept {
	hMenu = menu;
	return *this;
}

void WindowFactory::adjustRect() {
	RECT r{};
	r.left = 100;
	r.top = 100;
	r.right = r.left + clientWidth;
	r.bottom = r.top + clientHeight;
	bool hasMenu = (windowStyle & (WS_SYSMENU | WS_CAPTION));
	if (!AdjustWindowRectEx(&r, windowStyle, hasMenu, extendedStyle)) { // if it returns zero, there has been an error
		throw CWF_EXCEPTION(CwfException::CwfExceptionType::WINDOWS, L"AdjustWindowRectEx call failed");
	}
	windowWidth = r.right - r.left;
	windowHeight = r.bottom - r.top;
}

Window::WindowInitializationStruct WindowFactory::build() {
	adjustRect();
	Window::WindowInitializationStruct toReturn{ extendedStyle, className, windowName, windowStyle, x, y,
					windowWidth, windowHeight, hParent, hMenu, hInstance, clientWindowProc };
	return toReturn;
}