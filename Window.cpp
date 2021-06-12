#include "CwfException.h"
#include "Window.h"
#include <exception>
#include <iostream>
#include <optional>
#include <string>
#include <Windows.h>
#include <windowsx.h>

Window::WindowInitializationStruct::WindowInitializationStruct(DWORD eStyle, LPCWSTR aClassName, LPCWSTR aWindowName,
	DWORD style, int aX, int aY, int width, int height, HWND parent, HMENU menu,
	HINSTANCE instance, ClientWindowProc aWindowProc) noexcept
	: extendedStyle{ eStyle }, className{ aClassName }, windowName{ aWindowName }, windowStyle{ style },
	x{ aX }, y{ aY }, windowWidth{ width }, windowHeight{ height }, hParent{ parent }, hMenu{ menu },
	hInstance{ instance }, windowProc{ aWindowProc } {}

Window::WindowInitializationStruct::WindowInitializationStruct(WindowInitializationStruct&& o) noexcept {
	extendedStyle = o.extendedStyle;
	o.extendedStyle = 0;
	className = o.className;
	o.className = nullptr;
	windowName = o.windowName;
	o.windowName = nullptr;
	windowStyle = o.windowStyle;
	o.windowStyle = 0;
	x = o.x;
	o.x = 0;
	y = o.y;
	o.y = 0;
	windowWidth = o.windowWidth;
	o.windowWidth = 0;
	windowHeight = o.windowHeight;
	o.windowHeight = 0;
	hParent = o.hParent;
	o.hParent = nullptr;
	hMenu = o.hMenu;
	o.hMenu = nullptr;
	hInstance = o.hInstance;
	o.hInstance = nullptr;
	windowProc = o.windowProc;
	o.windowProc = nullptr;
}
Window::WindowInitializationStruct& Window::WindowInitializationStruct::operator=(WindowInitializationStruct&& o) noexcept {
	if (&o == this) return *this;
	extendedStyle = o.extendedStyle;
	o.extendedStyle = 0;
	className = o.className;
	o.className = nullptr;
	windowName = o.windowName;
	o.windowName = nullptr;
	windowStyle = o.windowStyle;
	o.windowStyle = 0;
	x = o.x;
	o.x = 0;
	y = o.y;
	o.y = 0;
	windowWidth = o.windowWidth;
	o.windowWidth = 0;
	windowHeight = o.windowHeight;
	o.windowHeight = 0;
	hParent = o.hParent;
	o.hParent = nullptr;
	hMenu = o.hMenu;
	o.hMenu = nullptr;
	hInstance = o.hInstance;
	o.hInstance = nullptr;
	windowProc = o.windowProc;
	o.windowProc = nullptr;
	return *this;
}

Window::Window(Window::WindowInitializationStruct wis) 
	: clientWindowProc{ wis.windowProc }, hWnd{ 0 }, kbd{}, mouse{},
	windowWidth{ wis.windowWidth }, windowHeight{ wis.windowHeight } {
	
	hWnd = CreateWindowExW(wis.extendedStyle, wis.className, wis.windowName, wis.windowStyle, wis.x, wis.y,
		wis.windowWidth, wis.windowHeight, wis.hParent, wis.hMenu, wis.hInstance, this);
}

/*Window::Window(Window&& o) noexcept : hWnd{o.hWnd}, clientWindowProc{o.clientWindowProc} {
	o.hWnd = nullptr;
	o.clientWindowProc = nullptr;
}

Window& Window::operator=(Window&& o) noexcept {
	if (&o == this) return *this;
	// no need for deletion or anything yet
	hWnd = o.hWnd;
	clientWindowProc = o.clientWindowProc;

	o.hWnd = nullptr;
	o.clientWindowProc = nullptr;
	return *this;
}*/

Window::ClientWindowProc Window::getClientWindowProc() const noexcept {
	return clientWindowProc;
}

void Window::showWindow(int showCommand) {
	ShowWindow(hWnd, showCommand);
}

std::optional<int> Window::processMessagesOnQueue() {
	MSG msg;
	while (PeekMessageW(&msg, nullptr, 0, 0, PM_REMOVE)) { // uses nullptr here to receive all messages from current thread
		switch (msg.message) {
		// other messages
		case WM_QUIT:
			return msg.wParam; // return exit code
		case WM_THROW:
			throw CwfException{ CwfException::CwfExceptionType::WINDOWS, L"Windows failure in window proc callback function.",
				reinterpret_cast<const char*>(msg.lParam), static_cast<int>(msg.wParam) };
		
		// keyboard messages
		case WM_SYSKEYDOWN: // for F10 and Alt; we do the same thing as regular though
			[[fallthrough]];
		case WM_KEYDOWN:
			if (!(msg.lParam & 0x40000000) // the previous key state is 0
				|| kbd.isAutorepeatEnabled()) { // or autorepeat is enabled
				
				kbd.keyPressed(static_cast<unsigned char>(msg.wParam));
			}
			break;
		case WM_SYSKEYUP:
			[[fallthrough]];
		case WM_KEYUP:
			kbd.keyReleased(static_cast<unsigned char>(msg.wParam));
			break;
		case WM_CHAR:
			kbd.characterTyped(static_cast<unsigned char>(msg.wParam));	
			break;

		// mouse messages
		case WM_LBUTTONDOWN:
			mouse.buttonPressed(Mouse::Event::Button::LEFT, GET_X_LPARAM(msg.lParam), GET_Y_LPARAM(msg.lParam));
			break;
		case WM_MBUTTONDOWN:
			mouse.buttonPressed(Mouse::Event::Button::MIDDLE, GET_X_LPARAM(msg.lParam), GET_Y_LPARAM(msg.lParam));
			break;
		case WM_RBUTTONDOWN:
			mouse.buttonPressed(Mouse::Event::Button::RIGHT, GET_X_LPARAM(msg.lParam), GET_Y_LPARAM(msg.lParam));
			break;
		case WM_LBUTTONUP:
			mouse.buttonReleased(Mouse::Event::Button::LEFT, GET_X_LPARAM(msg.lParam), GET_Y_LPARAM(msg.lParam));
			break;
		case WM_MBUTTONUP:
			mouse.buttonReleased(Mouse::Event::Button::MIDDLE, GET_X_LPARAM(msg.lParam), GET_Y_LPARAM(msg.lParam));
			break;
		case WM_RBUTTONUP:
			mouse.buttonReleased(Mouse::Event::Button::RIGHT, GET_X_LPARAM(msg.lParam), GET_Y_LPARAM(msg.lParam));
			break;
		case WM_LBUTTONDBLCLK:
			mouse.buttonDoubleClicked(Mouse::Event::Button::LEFT, GET_X_LPARAM(msg.lParam), GET_Y_LPARAM(msg.lParam));
			break;
		case WM_MBUTTONDBLCLK:
			mouse.buttonDoubleClicked(Mouse::Event::Button::MIDDLE, GET_X_LPARAM(msg.lParam), GET_Y_LPARAM(msg.lParam));
			break;
		case WM_RBUTTONDBLCLK:
			mouse.buttonDoubleClicked(Mouse::Event::Button::RIGHT, GET_X_LPARAM(msg.lParam), GET_Y_LPARAM(msg.lParam));
			break;
		case WM_MOUSEWHEEL:
			mouse.scrolled(msg.wParam, GET_X_LPARAM(msg.lParam), GET_Y_LPARAM(msg.lParam));
			break;
		case WM_MOUSEMOVE:
			int x = GET_X_LPARAM(msg.lParam);
			int y = GET_Y_LPARAM(msg.lParam);
			/*
			// if mouse was in window, and now outside the window -> left
			if (mouse.isInWindow() && (x < 0 || x > windowWidth || y < 0 || y > windowHeight))
				mouse.left(x, y);
			// otherwise, cursor must be in window now; if mouse wasn't in window -> entered
			else if (!mouse.isInWindow())
				mouse.entered(x, y);*/
			mouse.moved(x, y);
			break;
		}
		TranslateMessage(&msg);
		DispatchMessage(&msg);
	}
	return {};
}

void Window::createExceptionMessageBox(CwfException e) {
	MessageBoxW(hWnd, e.getExceptionString().c_str(), exceptionCaption, MB_ICONERROR);
}

void Window::createExceptionMessageBox(std::exception e) {
	MessageBoxW(hWnd, CwfException::getStandardExceptionString(e).c_str(), exceptionCaption, MB_ICONERROR);
}

HWND Window::getHWND() const noexcept {
	return hWnd;
}

bool Window::setTitle(LPCWSTR title) noexcept {
	return SetWindowTextW(hWnd, title);
}

void Window::createExceptionMessageBoxStatic(CwfException e) {
	MessageBoxW(nullptr, e.getExceptionString().c_str(), exceptionCaption, MB_ICONERROR);
}

void Window::createExceptionMessageBoxStatic(std::exception e) {
	MessageBoxW(nullptr, CwfException::getStandardExceptionString(e).c_str(), exceptionCaption, MB_ICONERROR);
}